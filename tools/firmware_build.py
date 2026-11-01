"""Build the selected MagicBox product; never access a connected device."""
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
from profiles import ROOT, select
from staging import sync_tree

FIRMWARE_BOARDS = {'m09-s3-hw210', 'm301-c5-rtc-v1-5g'}


def selection(board, variant=None):
    profile = select(board)
    if board not in FIRMWARE_BOARDS:
        raise ValueError('MagicBox application is not ported for ' + board)
    config = json.loads((ROOT / 'main/boards' / board / 'config.json').read_text())
    if config['target'] != profile['target']:
        raise ValueError('Application target does not match BSP profile')
    variants = [entry['name'] for entry in config['builds']]
    if any(not re.fullmatch(r'[a-z0-9][a-z0-9.-]*', name) for name in variants):
        raise ValueError('Invalid firmware variant identifier')
    variant = variant or variants[0]
    if variant not in variants:
        raise ValueError('Variant does not belong to ' + board + ': ' + variant)
    return profile, board, config, variant


def configuration(config, variant):
    build = next(entry for entry in config['builds'] if entry['name'] == variant)
    options = [*build['sdkconfig_append'], 'CONFIG_LANGUAGE_ZH_CN=y']
    result = {}
    for item in options:
        key, sep, value = item.partition('=')
        if not sep or not re.fullmatch(r'CONFIG_[A-Z0-9_]+', key) or '\n' in value:
            raise ValueError('Invalid hardware configuration: ' + repr(item))
        result[key] = value
    return result


def validate_sdkconfig(path, expected, target):
    actual = dict(line.split('=', 1) for line in path.read_text().splitlines()
                  if line.startswith('CONFIG_') and '=' in line)
    expected = {**expected, 'CONFIG_IDF_TARGET': json.dumps(target)}
    rejected = [f'{key}={value}: got {actual.get(key, "n")}' for key, value in expected.items()
                if actual.get(key, 'n') != value]
    if rejected:
        raise RuntimeError('Kconfig rejected profile settings:\n' + '\n'.join(rejected))


def build(profile, config, variant):
    sdk = Path(os.environ.get('IDF_PATH', ''))
    if not (sdk / 'tools/idf.py').is_file():
        raise ValueError('Activate ESP-IDF 6.1 with export.sh first')
    idf = [sys.executable, str(sdk / 'tools/idf.py')]
    version = subprocess.check_output([*idf, '--version'], text=True).strip()
    if not re.search(r'v6\.1(?:[.\s-]|$)', version):
        raise ValueError('Product firmware requires ESP-IDF 6.1; found ' + version)
    key = hashlib.sha256(str(ROOT).encode()).hexdigest()[:12]
    cache = Path(tempfile.gettempdir()) / ('magicbox-firmware-' + key) / profile['id'] / variant
    if any(c.isspace() for c in str(cache)):
        raise ValueError('Build cache must not contain whitespace; choose a different TMPDIR')
    owner = cache / '.source.json'
    identity = {'source': str(ROOT), 'board': profile['id'], 'variant': variant}
    if cache.exists() and (not owner.is_file() or json.loads(owner.read_text()) != identity):
        raise ValueError('Refusing unowned build cache: ' + str(cache))
    cache.mkdir(parents=True, exist_ok=True)
    owner.write_text(json.dumps(identity))
    project = cache / 'project'
    sync_tree(ROOT, project, ['.git', 'build', 'build-*', 'dist', 'managed_components', 'releases',
        'sdkconfig', 'sdkconfig.old', 'dependencies.lock', '__pycache__', '.DS_Store', 'local',
        'main/assets/lang_config.h', 'main/mmap_generate_*.h'])
    for filename in ['sdkconfig', 'sdkconfig.old']:
        (project / filename).unlink(missing_ok=True)
    options = configuration(config, variant)
    fragment = project / 'build/magicbox.sdkconfig.defaults'
    fragment.parent.mkdir(exist_ok=True)
    fragment.write_text('# Generated hardware/variant defaults\n' + ''.join(f'{k}={v}\n' for k,v in options.items()))
    env = os.environ.copy()
    env.pop('IDF_TARGET', None)
    env.setdefault('IDF_COMPONENT_CACHE_PATH', str(cache / 'component-cache'))
    print('Building', variant, 'in', project, flush=True)
    subprocess.run([*idf, '-DIDF_TARGET=' + profile['target'], '-DBOARD_NAME=' + variant, '-DMAGICBOX_PROFILE=' + profile['id'],
                    '-DSDKCONFIG_DEFAULTS=sdkconfig.defaults;build/magicbox.sdkconfig.defaults',
                    'reconfigure'], cwd=project, env=env, check=True)
    validate_sdkconfig(project / 'sdkconfig', options, profile['target'])
    subprocess.run([*idf, 'build', 'merge-bin'], cwd=project, env=env, check=True)
    return package_artifacts(profile, config, variant, project, version)


def package_artifacts(profile, config, variant, project, version):
    """Export every generated flash component, including optional speech models."""
    output = ROOT / 'dist' / variant
    output.mkdir(parents=True, exist_ok=True)
    hashes = {}
    files = ['magicbox.bin', 'magicbox.elf', 'magicbox.map', 'merged-binary.bin',
             'partition_table/partition-table.bin', 'bootloader/bootloader.bin',
             'ota_data_initial.bin', 'flasher_args.json', 'flash_args']
    flash_files = json.loads((project / 'build/flasher_args.json').read_text())['flash_files']
    files = list(dict.fromkeys([*files, *flash_files.values()]))
    merged = (project / 'build/merged-binary.bin').read_bytes()
    for offset, name in flash_files.items():
        component = project / 'build' / name
        if not component.resolve().is_relative_to((project / 'build').resolve()):
            raise ValueError('Artifact path escapes build directory')
        data = component.read_bytes()
        start = int(offset, 16)
        if merged[start:start + len(data)] != data:
            raise ValueError('Merged image does not match flash component ' + name)
    for name in files:
        source = project / 'build' / name
        if not source.resolve().is_relative_to((project / 'build').resolve()):
            raise ValueError('Artifact path escapes build directory')
        destination = output / name
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, destination)
        hashes[name] = hashlib.sha256(source.read_bytes()).hexdigest()
    for name in ['sdkconfig', 'dependencies.lock']:
        shutil.copy2(project / name, output / name)
    manifest = {'profile': profile['id'], 'target': profile['target'], 'model': profile['model'],
        'hardware_revision': profile['hardware_revision'], 'source_board': profile.get('source_board'),
        'hardware_source_commit': profile.get('source_commit'), 'variant': variant,
        'board_type': config['type'], 'language': 'zh-CN', 'version': '0.1.0', 'sdk': version,
        'source_commit': subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),
        'source_dirty': bool(subprocess.check_output(['git','status','--porcelain'],cwd=ROOT,text=True).strip()),
        'sha256': hashes, 'application_bytes': (output / 'magicbox.bin').stat().st_size,
        'device_test': 'not performed by build tool'}
    (output / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n')
    print('Artifacts:', output, flush=True)
    return 0
