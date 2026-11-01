"""Verify physical profile boundaries and product/diagnostic separation."""
import json
from pathlib import Path
import subprocess
import sys
import unittest
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from profiles import profiles
from firmware_build import selection,configuration,validate_sdkconfig
import tempfile
import shutil

class RootLayoutTest(unittest.TestCase):
    def test_all_diagnostics_use_one_canonical_bsp(self):
        for p in profiles():
            self.assertEqual(p['pins'],f"main/boards/{p['id']}/bsp/include/bsp_pins.h")
            for project in [p['project'],p['hardware_demo']]:
                self.assertTrue((ROOT/project/'CMakeLists.txt').is_file())
                self.assertIn(f"main/boards/{p['id']}/bsp",(ROOT/project/'CMakeLists.txt').read_text())
            if p.get('firmware'):
                cfg=json.loads((ROOT/p['firmware']).read_text())
                self.assertEqual(cfg['target'],p['target'])

    def test_legacy_profile_cannot_build_product_without_sdk(self):
        result=subprocess.run([sys.executable,str(ROOT/'tools/build.py'),'--board','c5-reference-m301-hw120'],text=True,capture_output=True)
        self.assertNotEqual(result.returncode,0)
        self.assertIn('not ported',result.stderr)

    def test_wrong_chip_cannot_use_same_named_profile(self):
        result=subprocess.run([sys.executable,str(ROOT/'tools/build.py'),'--board','m09-s3-hw210','--target','esp32c5'],text=True,capture_output=True)
        self.assertNotEqual(result.returncode,0)
        self.assertIn('requires esp32s3',result.stderr)

    def test_rejected_hardware_configuration_stops_build(self):
        with tempfile.TemporaryDirectory() as directory:
            path=Path(directory)/'sdkconfig';path.write_text('CONFIG_IDF_TARGET="esp32c5"\n')
            with self.assertRaisesRegex(RuntimeError,'Kconfig rejected'):
                validate_sdkconfig(path,{},'esp32s3')

    def test_list_does_not_require_an_sdk(self):
        result=subprocess.run([sys.executable,str(ROOT/'tools/build.py'),'--list-boards'],text=True,capture_output=True)
        self.assertEqual(result.returncode,0,result.stderr)
        self.assertIn('reference diagnostics only',result.stdout)

    def test_direct_cmake_cannot_guess_hardware_from_chip(self):
        if not shutil.which('cmake'): self.skipTest('CMake required')
        with tempfile.TemporaryDirectory() as directory:
            result=subprocess.run(['cmake','-S',str(ROOT),'-B',directory,'-DIDF_TARGET=esp32s3'],text=True,capture_output=True)
            self.assertNotEqual(result.returncode,0)
            self.assertIn('Select complete MagicBox hardware',result.stdout+result.stderr)
