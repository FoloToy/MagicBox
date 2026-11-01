#!/usr/bin/env python3
"""Validate explicit board ownership and a service-independent public BSP tree."""
import json
import re
from pathlib import Path
from profiles import ROOT, profiles


def check(root=ROOT):
    errors=[]; ids=set(); pins=set()
    entries=json.loads((root/'boards/manifest.json').read_text())['profiles']
    for p in entries:
        board=p['id']
        if board in ids: errors.append('Duplicate profile '+board)
        ids.add(board)
        if p['pins'] in pins: errors.append('Shared pin authority '+board)
        pins.add(p['pins'])
        if p['bsp']!=f'boards/{board}/bsp' or p['pins']!=f'boards/{board}/bsp/include/bsp_pins.h':
            errors.append('Noncanonical BSP/pin ownership '+board)
        for key in ['pins','project','hardware_demo','bsp']:
            if not (root/p[key]).exists(): errors.append('Missing '+key+' for '+board)
        if 'firmware' in p or 'xiaozhi' in p:
            errors.append('Product selection in BSP registry '+board)
        if p['target'] not in ['esp32s3','esp32c5']:errors.append('Unknown chip '+board)
        for project in [p['project'],p['hardware_demo']]:
            cmake=root/project/'CMakeLists.txt'
            if not cmake.exists() or p['bsp'] not in cmake.read_text():
                errors.append('Example does not use canonical BSP '+project)
        main=root/p['project']/'main/main.cpp'
        if main.exists() and (len(main.read_text().splitlines())>24 or re.search(r'\b(audio_begin|bsp_audio_init|esp_wifi_init|xTaskCreate)\s*\(',main.read_text())):
            errors.append('Minimal example starts application services '+board)
    for folder in ['boards','examples']:
        for f in (root/folder).rglob('*'):
            if any(part in ['build','managed_components','__pycache__'] for part in f.parts):continue
            if f.suffix not in ['.cpp','.cc','.c','.h','.hpp','.yml']:continue
            text=f.read_text()
            if re.search(r'Application::GetInstance|esp_mqtt_client_|PubSubClient|esp_ota_|esp_wifi_init|magic\.folotoy\.cn|78/esp-wifi-connect',text):
                errors.append('Application/network/service coupling: '+str(f.relative_to(root)))
            if re.search(r'(?i)\b(password|passwd|api_key|access_token|ssid)\b\s*[=:]\s*"[^"\n]+"',text):
                errors.append('Credential in public BSP/example: '+str(f.relative_to(root)))
    for name in ['main','apps','scripts','partitions']:
        if (root/name).exists():errors.append('Product tree in BSP repository: '+name)
    return errors


if __name__=='__main__':
    failures=check()
    if failures:raise SystemExit('\n'.join(failures))
    print('PASS: canonical BSP profiles, minimal examples, no product/service/credential coupling')
