import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from check_repo import check
from profiles import profiles

class BspBoundaryTest(unittest.TestCase):
    def test_public_source_and_examples_have_no_application_coupling(self):
        self.assertEqual(check(),[])

    def test_list_works_without_sdk(self):
        p=subprocess.run([sys.executable,str(ROOT/'tools/build.py'),'--list-boards'],capture_output=True,text=True)
        self.assertEqual(p.returncode,0,p.stderr)
        self.assertIn('BSP + examples',p.stdout)

    def test_product_modes_and_variants_are_not_build_options(self):
        for args in [['--app','firmware'],['--variant','folotoy-magicbox-m09-s3-hw210']]:
            p=subprocess.run([sys.executable,str(ROOT/'tools/build.py'),'--board','m09-s3-hw210',*args],capture_output=True,text=True)
            self.assertEqual(p.returncode,2)

    def test_wrong_chip_is_rejected_before_sdk(self):
        p=subprocess.run([sys.executable,str(ROOT/'tools/build.py'),'--board','m09-s3-hw210','--target','esp32c5'],capture_output=True,text=True)
        self.assertNotEqual(p.returncode,0)
        self.assertIn('requires esp32s3',p.stderr)

    def test_service_coupling_in_added_driver_is_rejected(self):
        with tempfile.TemporaryDirectory() as directory:
            root=Path(directory);(root/'boards').mkdir();(root/'examples').mkdir()
            (root/'boards/manifest.json').write_text(json.dumps({'profiles':[]}))
            (root/'boards/driver.cpp').write_text('void connect() { esp_mqtt_client_start(client); }')
            self.assertTrue(any('coupling' in error for error in check(root)))

    def test_examples_reference_their_own_board_component(self):
        for p in profiles():
            for example in [p['project'],p['hardware_demo']]:
                self.assertIn(p['bsp'],(ROOT/example/'CMakeLists.txt').read_text())
