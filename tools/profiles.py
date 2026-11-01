"""Explicit hardware registry: chip equality never implies PCB compatibility."""
import json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def profiles():
    return json.loads((ROOT/'boards/manifest.json').read_text())['profiles']
def select(name, target=None):
    matches=[p for p in profiles() if p['id']==name]
    if len(matches)!=1: raise ValueError('Unknown hardware profile: '+str(name))
    result=matches[0]
    if target and result['target']!=target:
        raise ValueError('Profile '+name+' requires '+result['target']+', not '+target)
    return result
