#!/usr/bin/env python3
"""Build only: no implicit board selection, upload, erase or production layout change."""
import argparse,hashlib,json,os,re,shutil,subprocess,sys,tempfile
from pathlib import Path
from profiles import ROOT,profiles,select
from staging import sync_tree

def build_diagnostic(profile, app):
    def fail(message): raise ValueError(message)
    sdk=Path(os.environ.get('IDF_PATH',''))
    if not (sdk/'tools/idf.py').is_file(): fail('Source the profile ESP-IDF export.sh first')
    version=subprocess.check_output([sys.executable,str(sdk/'tools/idf.py'),'--version'],text=True)
    if not re.search(r'v'+re.escape(profile['idf_series'])+r'(?:[.\s-]|$)',version):
        fail(profile['id']+' requires ESP-IDF '+profile['idf_series']+'; found '+version.strip())
    key=hashlib.sha256(str(ROOT).encode()).hexdigest()[:12]
    cache=Path(tempfile.gettempdir())/('magicbox-diagnostics-'+key)/profile['id']/('idf-'+profile['idf_series'])
    owner=cache/'.source.json'
    if cache.exists() and (not owner.exists() or json.loads(owner.read_text())!={'source':str(ROOT)}):
        fail('Refusing unowned build cache '+str(cache))
    cache.mkdir(parents=True,exist_ok=True);owner.write_text(json.dumps({'source':str(ROOT)}))
    stage=cache/'project';stage.mkdir(exist_ok=True)
    sync_tree(ROOT,stage,
        ['.git','build','build-*','dist','managed_components','sdkconfig','sdkconfig.old','dependencies.lock','__pycache__','local'])
    project=stage/(profile['project'] if app=='baseline' else profile['hardware_demo'])
    build=cache/('build-'+app)
    env=os.environ.copy()
    env.pop('IDF_TARGET',None)
    env.setdefault('IDF_COMPONENT_CACHE_PATH',str(cache/('components-'+app)))
    subprocess.run([sys.executable,str(sdk/'tools/idf.py'),'-C',str(project),'-B',str(build),
        '-DIDF_TARGET='+profile['target'],'-DSDKCONFIG='+str(cache/('sdkconfig-'+app)),
        '-DSDKCONFIG_DEFAULTS='+str(project/'sdkconfig.defaults'),'build'],env=env,check=True)
    description=json.loads((build/'project_description.json').read_text())
    image=build/(description['project_name']+'.bin')
    out=ROOT/'dist'/profile['id']/app;out.mkdir(parents=True,exist_ok=True)
    shutil.copy2(image,out/'application.bin')
    for name in ['bootloader/bootloader.bin','partition_table/partition-table.bin']:
        target=out/name;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(build/name,target)
    result={'profile':profile['id'],'target':profile['target'],'app':app,
            'sdk':version.strip(),'sha256':hashlib.sha256(image.read_bytes()).hexdigest(),
            'size':image.stat().st_size,'device_validation':'not performed by build tool'}
    (out/'build.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(result,indent=2));print('Artifacts:',out)
    return 0
