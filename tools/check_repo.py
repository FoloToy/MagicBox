#!/usr/bin/env python3
import json,re
from pathlib import Path
from profiles import ROOT,profiles
errors=[];ids=set();pins=set()
for p in profiles():
    if p['id'] in ids: errors.append('Duplicate profile '+p['id'])
    ids.add(p['id'])
    if p['pins'] in pins: errors.append('Profiles must have independent pin authority')
    pins.add(p['pins'])
    if not (ROOT/p['pins']).is_file():errors.append('Missing pins '+p['id'])
    if not (ROOT/p['project']/'CMakeLists.txt').is_file():errors.append('Missing project '+p['id'])
    if p['target'] not in ('esp32s3','esp32c5'):errors.append('Unknown target '+p['id'])
    for f in (ROOT/p['project']/'components/bsp').rglob('*'):
        if f.suffix not in ('.cpp','.cc','.h','.hpp'):continue
        text=f.read_text()
        if re.search(r'Application::GetInstance|esp_mqtt_client_|PubSubClient|esp_ota_',text):
            errors.append('Product logic in BSP: '+str(f.relative_to(ROOT)))
        if re.search(r'(?i)\b(password|passwd|api_key|access_token|ssid)\b\s*[=:]\s*"[^"\n]+"',text):
            errors.append('Possible credential: '+str(f.relative_to(ROOT)))
    main=(ROOT/p['project']/'main/main.cpp').read_text()
    if len(main.splitlines())>24 or re.search(r'\b(audio_begin|bsp_audio_init|esp_wifi_init|xTaskCreate)\s*\(',main):
        errors.append('Default main must remain minimal: '+p['id'])
if errors:raise SystemExit('\n'.join(errors))
print('PASS: independent explicit profiles, minimal BSP entries, product/credential boundaries')
