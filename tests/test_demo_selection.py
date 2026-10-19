import subprocess,sys,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
class DemoSelection(unittest.TestCase):
    def test_c5_has_its_own_target_and_variant(self):
        sys.path.insert(0, str(ROOT / 'tools'))
        from build_xiaozhi import selection
        profile, directory, config, variant = selection('m301-c5-rtc-v1-5g')
        self.assertEqual(profile['target'], 'esp32c5')
        self.assertEqual(directory, 'folotoy/magicbox-c5')
        self.assertEqual(config['type'], 'magicbox-m301-c5-rtc-v1-5g')
        self.assertEqual(variant, 'folotoy-magicbox-m301-c5-rtc-v1-5g')
        with self.assertRaisesRegex(ValueError, 'Variant does not belong'):
            selection('m301-c5-rtc-v1-5g', 'folotoy-magicbox-m09-s3-hw210')
        with self.assertRaisesRegex(ValueError, 'Variant does not belong'):
            selection('m09-s3-hw210', variant)
    def test_legacy_c5_reference_does_not_select_new_power_profile(self):
        sys.path.insert(0, str(ROOT / 'tools'))
        from build_xiaozhi import selection
        with self.assertRaisesRegex(ValueError, 'not ported'):
            selection('c5-reference-m301-hw120')
    def test_board_is_mandatory(self):
        p=subprocess.run([sys.executable,str(ROOT/'tools/build_xiaozhi.py')],capture_output=True,text=True)
        self.assertEqual(p.returncode,2)
        self.assertIn('--board',p.stderr)
    def test_upstream_variant_cannot_override_profile(self):
        p=subprocess.run([sys.executable,str(ROOT/'tools/build_xiaozhi.py'),'--board','m09-s3-hw210','--name','other-board'],capture_output=True,text=True)
        self.assertEqual(p.returncode,2)
        self.assertIn('unrecognized arguments',p.stderr)
