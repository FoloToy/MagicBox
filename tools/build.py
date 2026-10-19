#!/usr/bin/env python3
"""Build only: no implicit board selection, upload, erase or production layout change."""
import argparse,hashlib,json,os,re,shutil,subprocess,sys,tempfile
from pathlib import Path
from profiles import ROOT,profiles,select
from staging import sync_tree

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--list',action='store_true')
    parser.add_argument('--board')
    parser.add_argument('--target')
    parser.add_argument('--app',choices=['baseline','hardware-demo'],default='baseline')
    args=parser.parse_args()
    if args.list:
        for p in profiles(): print(p['id'],p['target'],'ESP-IDF '+p['idf_series'],p['model_mapping'])
        return 0
    if not args.board: parser.error('--board is required; a chip alone does not identify hardware')
    try: profile=select(args.board,args.target)
    except ValueError as error: parser.error(str(error))
    sdk=Path(os.environ.get('IDF_PATH',''))
    if not (sdk/'tools/idf.py').is_file(): parser.error('Source the profile ESP-IDF export.sh first')
    version=subprocess.check_output([sys.executable,str(sdk/'tools/idf.py'),'--version'],text=True)
    if not re.search(r'v'+re.escape(profile['idf_series'])+r'(?:[.\s-]|$)',version):
        parser.error(profile['id']+' requires ESP-IDF '+profile['idf_series']+'; found '+version.strip())
    key=hashlib.sha256(str(ROOT).encode()).hexdigest()[:12]
    cache=Path(tempfile.gettempdir())/('magicbox-bsp-'+key)/profile['id']/('idf-'+profile['idf_series'])
    owner=cache/'.source.json'
    if cache.exists() and (not owner.exists() or json.loads(owner.read_text())!={'source':str(ROOT)}):
        parser.error('Refusing unowned build cache '+str(cache))
    cache.mkdir(parents=True,exist_ok=True);owner.write_text(json.dumps({'source':str(ROOT)}))
    stage=cache/'project';stage.mkdir(exist_ok=True)
    sync_tree(ROOT/profile['project'],stage,
        ['build/','build-*/','managed_components/','sdkconfig','sdkconfig.old','dependencies.lock','__pycache__/'])
    project=stage if args.app=='baseline' else stage/'examples/hardware-demo'
    build=cache/('build-'+args.app)
    subprocess.run([sys.executable,str(sdk/'tools/idf.py'),'-C',str(project),'-B',str(build),
        '-DIDF_TARGET='+profile['target'],'-DSDKCONFIG='+str(cache/('sdkconfig-'+args.app)),
        '-DSDKCONFIG_DEFAULTS='+str(stage/'sdkconfig.defaults'),'build'],check=True)
    description=json.loads((build/'project_description.json').read_text())
    image=build/(description['project_name']+'.bin')
    out=ROOT/'build'/profile['id']/args.app;out.mkdir(parents=True,exist_ok=True)
    shutil.copy2(image,out/'application.bin')
    for name in ['bootloader/bootloader.bin','partition_table/partition-table.bin']:
        target=out/name;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(build/name,target)
    result={'profile':profile['id'],'target':profile['target'],'app':args.app,
            'sdk':version.strip(),'sha256':hashlib.sha256(image.read_bytes()).hexdigest(),
            'size':image.stat().st_size,'device_validation':'not performed by build tool'}
    (out/'build.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(result,indent=2));print('Artifacts:',out)
    return 0
if __name__=='__main__': raise SystemExit(main())
