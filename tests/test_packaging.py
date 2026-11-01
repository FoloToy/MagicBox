"""A complete flash graph must include model files and match the merged image."""
import hashlib
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import firmware_build as firmware

class PackagingTest(unittest.TestCase):
    def fixture(self,root):
        project=root/'project';build=project/'build';build.mkdir(parents=True)
        for name in ['magicbox.bin','magicbox.elf','magicbox.map','partition_table/partition-table.bin',
                     'bootloader/bootloader.bin','ota_data_initial.bin','flash_args','srmodels/srmodels.bin']:
            p=build/name;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(name.encode())
        flash={'0x0':'bootloader/bootloader.bin','0x100':'partition_table/partition-table.bin',
               '0x200':'ota_data_initial.bin','0x300':'magicbox.bin','0x900':'srmodels/srmodels.bin'}
        (build/'flasher_args.json').write_text(json.dumps({'flash_files':flash}))
        merged=bytearray(b'\xff'*0x1000)
        for off,name in flash.items():
            data=(build/name).read_bytes();start=int(off,16);merged[start:start+len(data)]=data
        (build/'merged-binary.bin').write_bytes(merged)
        (project/'sdkconfig').write_text('generated hardware settings')
        (project/'dependencies.lock').write_text('generated dependency lock')
        return project

    def test_model_component_is_exported_hashed_and_matches_merged(self):
        with tempfile.TemporaryDirectory() as directory:
            root=Path(directory);project=self.fixture(root)
            with patch.object(firmware,'ROOT',root),patch.object(firmware.subprocess,'check_output',side_effect=['a'*40,'']):
                firmware.package_artifacts({'id':'test-c5','target':'esp32c5','model':'M301','hardware_revision':None},
                                           {'type':'test-ota'},'test-variant',project,'test-sdk')
            output=root/'dist/test-variant';manifest=json.loads((output/'manifest.json').read_text())
            self.assertEqual((output/'srmodels/srmodels.bin').read_bytes(),b'srmodels/srmodels.bin')
            self.assertEqual(manifest['sha256']['srmodels/srmodels.bin'],hashlib.sha256(b'srmodels/srmodels.bin').hexdigest())
            self.assertEqual((output/'merged-binary.bin').read_bytes(),(project/'build/merged-binary.bin').read_bytes())

    def test_mismatched_model_prevents_packaging(self):
        with tempfile.TemporaryDirectory() as directory:
            root=Path(directory);project=self.fixture(root)
            (project/'build/srmodels/srmodels.bin').write_bytes(b'changed')
            with patch.object(firmware,'ROOT',root):
                with self.assertRaisesRegex(ValueError,'Merged image does not match flash component'):
                    firmware.package_artifacts({'id':'test'}, {}, 'test-variant',project,'test-sdk')
