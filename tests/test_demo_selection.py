import subprocess,sys,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
class DemoSelection(unittest.TestCase):
    def test_c5_cannot_build_s3_xiaozhi(self):
        p=subprocess.run([sys.executable,str(ROOT/'tools/build_xiaozhi.py'),'--board','c5-reference-m301-hw120'],capture_output=True,text=True)
        self.assertEqual(p.returncode,2)
        self.assertIn('XiaoZhi is not ported',p.stderr)
    def test_board_is_mandatory(self):
        p=subprocess.run([sys.executable,str(ROOT/'tools/build_xiaozhi.py')],capture_output=True,text=True)
        self.assertEqual(p.returncode,2)
        self.assertIn('--board',p.stderr)
    def test_upstream_variant_cannot_override_profile(self):
        p=subprocess.run([sys.executable,str(ROOT/'tools/build_xiaozhi.py'),'--board','m09-s3-hw210','--name','other-board'],capture_output=True,text=True)
        self.assertEqual(p.returncode,2)
        self.assertIn('unrecognized arguments',p.stderr)
