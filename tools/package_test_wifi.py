#!/usr/bin/env python3
"""Create an explicit test-network installer without rebuilding the application.

Uses only verified generated release artifacts and an SDK-generated NVS partition;
never reads a connected device, identity partition or device backup.
"""
import argparse
import csv
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import zipfile

from profiles import select


def sha(data):
    return hashlib.sha256(data).hexdigest()


def run(*args):
    return subprocess.check_output([str(a) for a in args], text=True, stderr=subprocess.STDOUT)


def package(board, source_manifest, wifi_file, idf_path, output):
    profile = select(board)
    if board not in ('m09-s3-hw210', 'm301-c5-rtc-v1-5g'):
        raise ValueError('Unsupported application profile')
    source = json.loads(source_manifest.read_text())
    if source['profile'] != board or source['chip'] != profile['target']:
        raise ValueError('Source image does not match the complete hardware profile')
    full = source['full_image']
    if full['write_offset'] != '0x0' or full['flash_size_bytes'] != 0x1000000:
        raise ValueError('Expected the verified 16 MB complete installer')
    raw = (source_manifest.parent / full['file']).read_bytes()
    if sha(raw) != full['sha256'] or len(raw) != full['size_bytes']:
        raise ValueError('Source full-image checksum mismatch')
    wifi = json.loads(wifi_file.read_text())
    ssid, password = wifi['ssid'], wifi['password']
    if not isinstance(ssid, str) or not 1 <= len(ssid.encode()) <= 32:
        raise ValueError('SSID must contain 1–32 UTF-8 bytes')
    if not isinstance(password, str) or not 8 <= len(password.encode()) <= 63:
        raise ValueError('Test Wi-Fi password must contain 8–63 UTF-8 bytes')
    if '\0' in ssid or '\0' in password:
        raise ValueError('NUL is not allowed in Wi-Fi strings')
    if raw[0x9000:0xd000] != b'\xff' * 0x4000:
        raise ValueError('Source must contain erased NVS, never saved device settings')
    if len(raw) >= int(full['identity_partition_offset'], 16):
        raise ValueError('Source overlaps product identity')
    app = source['application_image']
    app_offset = int(full['factory_application_offset'], 16)
    if sha(raw[app_offset:app_offset + app['size_bytes']]) != app['sha256']:
        raise ValueError('Embedded application checksum mismatch')
    generator = idf_path / 'components/nvs_flash/nvs_partition_generator/nvs_partition_gen.py'
    parser = idf_path / 'components/nvs_flash/nvs_partition_tool/nvs_tool.py'
    output.mkdir(parents=True, exist_ok=False)
    base = app['file'][:-4] + '-testwifi'
    filename, archive_name = base + '-full.bin', base + '.zip'
    with tempfile.TemporaryDirectory(prefix='magicbox-testwifi-') as temp:
        temp = Path(temp)
        with (temp / 'wifi.csv').open('w', newline='') as handle:
            writer = csv.writer(handle)
            writer.writerows([['key', 'type', 'encoding', 'value'], ['wifi', 'namespace', '', ''],
                              ['ssid', 'data', 'string', ssid],
                              ['password', 'data', 'string', password], ['channel', 'data', 'u8', 0]])
        run(sys.executable, generator, 'generate', temp / 'wifi.csv', temp / 'wifi.bin', '0x4000')
        nvs = (temp / 'wifi.bin').read_bytes()
        decoded = json.loads(run(sys.executable, parser, temp / 'wifi.bin', '-d', 'minimal', '-f', 'json', '--color', 'never'))
        expected = {('wifi', 'ssid'): ssid, ('wifi', 'password'): password, ('wifi', 'channel'): 0}
        if len(decoded) != 3 or {(v['namespace'], v['key']): v['data'] for v in decoded} != expected:
            raise ValueError('SDK NVS readback did not match the Wi-Fi reader schema')
        integrity = run(sys.executable, parser, temp / 'wifi.bin', '-i', '-d', 'none', '--color', 'never')
        if 'CRC32: OK' not in integrity or any(word in integrity.lower() for word in ('wrong', 'fail', 'mismatch', 'error')):
            raise ValueError('NVS integrity check failed')
        # Reconstruct every original component; holes must remain FF. Add only NVS.
        pieces, args = [], []
        check = bytearray(b'\xff' * len(raw))
        for i, component in enumerate(full['components']):
            off = int(component['offset'], 16)
            data = raw[off:off + component['size_bytes']]
            if sha(data) != component['sha256']:
                raise ValueError('Source component checksum mismatch')
            check[off:off + len(data)] = data
            pieces.append((off, data))
        if bytes(check) != raw:
            raise ValueError('Unexplained bytes in source image')
        pieces.append((0x9000, nvs))
        for i, (off, data) in enumerate(sorted(pieces)):
            path = temp / (str(i) + '.bin')
            path.write_bytes(data)
            args.extend([hex(off), path])
        run(sys.executable, '-m', 'esptool', '--chip', profile['target'], 'merge-bin',
            '--target-offset', '0x0', '--flash-mode', 'keep', '--flash-size', 'keep',
            '--flash-freq', 'keep', '-o', output / filename, *args)
    merged = (output / filename).read_bytes()
    if (merged[:0x9000] != raw[:0x9000] or merged[0xd000:] != raw[0xd000:]
            or merged[0x9000:0xd000] != nvs or len(merged) != len(raw)):
        raise ValueError('Test installer changed bytes outside the Wi-Fi NVS partition')
    manifest = {
        'schema_version': 1, 'product': 'MagicBox', 'profile': board, 'chip': profile['target'],
        'package_variant': 'testwifi', 'application_version': '0.1.0',
        'file': filename, 'kind': 'merged-installation', 'write_offset': '0x0',
        'size_bytes': len(merged), 'sha256': sha(merged),
        'base_installer': {'file': full['file'], 'sha256': full['sha256']},
        'application_image': app, 'application_rebuilt': False,
        'preconfigured_wifi': {'ssid': ssid, 'method': 'SDK-generated NVS wifi namespace',
                               'offset': '0x9000', 'size_bytes': len(nvs), 'sha256': sha(nvs)},
        'server_url': source['defaults']['ota_url'],
        'identity_partition_included': False, 'initial_boot': full['initial_boot'],
        'written_range': full['written_range'],
        'validation': {'sdk_nvs_decode_and_integrity': 'passed',
                       'all_bytes_outside_nvs_unchanged': True,
                       'application_matches_existing_release': True,
                       'new_physical_flash_or_network_test': False}}
    manifest_text = json.dumps(manifest, indent=2, ensure_ascii=False) + '\n'
    (output / 'manifest.json').write_text(manifest_text)
    (output / (base + '-manifest.json')).write_text(manifest_text)
    cmd = f'python -m esptool --chip {profile["target"]} --port PORT write-flash 0x0 {filename}'
    (output / 'README.zh-CN.md').write_text(f'''# MagicBox 测试 Wi-Fi 完整固件\n\n[English](README.en.md)\n\n型号：`{board}`。\n\n- SSID：`{ssid}`\n- 密码：`{password}`\n- 默认服务器：`{source['defaults']['ota_url']}`\n\n刷入后直接尝试连接上述网络，无需手机配网；约 60 秒内无法连接则进入 `FoloToy-XXXX` 配网。需要现场有可用的对应网络。云端激活/绑定仍按原有流程，预设 Wi-Fi 不会替你激活设备。\n\n```sh\n{cmd}\n```\n\n仅适用于此型号、16 MB Flash、未启用 Secure Boot/Flash Encryption 的设备。先私下备份，不需要全片擦除。此完整包会替换原厂应用和本机配置，产品身份分区不在写入范围，云端已有绑定不会删除。两个型号不可混刷。\n\n本包仅在原完整固件的 NVS 配置区预置测试网络；应用、bootloader、分区表与其余字节完全不变。只有完整安装包可预置此配置；仅刷应用不能获得这组预设 Wi-Fi。后续手动改网会保存新配置；清除网络配置后需重新配网或重新刷入本测试包。\n\n已通过 SDK NVS 解码/完整性、镜像对比、应用哈希和 ZIP 检查；本次未烧录或进行无线连接真机验收。\n''')
    (output / 'README.en.md').write_text(f'''# MagicBox test Wi-Fi installer\n\n[简体中文](README.zh-CN.md)\n\nProfile: `{board}`.\n\n- SSID: `{ssid}`\n- Password: `{password}`\n- Server: `{source['defaults']['ota_url']}`\n\nAfter installation, the device attempts this network without phone provisioning. If unavailable, it falls back to `FoloToy-XXXX` provisioning after approximately 60 seconds. Activation and cloud binding remain separate.\n\n```sh\n{cmd}\n```\n\nOnly for the exact profile, 16 MB flash and disabled Secure Boot/Flash Encryption. Back up privately first; no full-chip erase is needed. Replaces the factory application and local settings; product identity lies outside the write range. Existing cloud bindings are unchanged. Never mix profiles.\n\nOnly the original installer's NVS region changes; the application, bootloader, partition table and all remaining bytes are identical. Application-only flashing does not install this Wi-Fi preset. Later manual provisioning is saved normally; clearing saved networks requires provisioning or reinstalling this test package.\n\nSDK NVS decode/integrity, binary comparison, application hashes and archive checks pass. No new physical flashing or Wi-Fi acceptance is claimed.\n''')
    license_path = source_manifest.parent / 'LICENSE'
    (output / 'LICENSE').write_bytes(license_path.read_bytes())
    files = [filename, 'manifest.json', 'README.zh-CN.md', 'README.en.md', 'LICENSE']
    (output / 'SHA256SUMS').write_text(''.join(sha((output / n).read_bytes()) + '  ' + n + '\n' for n in files))
    with zipfile.ZipFile(output / archive_name, 'w', zipfile.ZIP_DEFLATED) as archive:
        for name in files + ['SHA256SUMS']:
            archive.write(output / name, base + '/' + name)
    with zipfile.ZipFile(output / archive_name) as archive:
        if archive.testzip() is not None:
            raise ValueError('ZIP CRC failed')
        for name in files:
            if archive.read(base + '/' + name) != (output / name).read_bytes():
                raise ValueError('ZIP member mismatch')
    (output / (base + '-SHA256SUMS.txt')).write_text(''.join(
        sha((output / n).read_bytes()) + '  ' + n + '\n' for n in [filename, archive_name, base + '-manifest.json']))
    print(json.dumps({'profile': board, 'file': filename, 'sha256': sha(merged),
                      'bytes': len(merged), 'application_unchanged': True}, ensure_ascii=False))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--board', required=True)
    parser.add_argument('--source-manifest', type=Path, required=True)
    parser.add_argument('--wifi-file', type=Path, required=True, help='Private JSON with ssid and password')
    parser.add_argument('--idf-path', type=Path, default=os.environ.get('IDF_PATH'))
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    if not args.idf_path:
        parser.error('--idf-path or IDF_PATH is required')
    package(args.board, args.source_manifest, args.wifi_file, args.idf_path, args.output)


if __name__ == '__main__':
    main()
