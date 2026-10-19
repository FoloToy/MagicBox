import sys,unittest
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from profiles import profiles,select
class Profiles(unittest.TestCase):
    def test_m09_is_not_all_s3_boards(self):
        self.assertEqual(select('m09-s3-hw210','esp32s3')['model'],'M09')
        for guessed in ['esp32s3','s3','m09','m09-s3-hw220','unknown']:
            with self.assertRaises(ValueError):select(guessed)
    def test_wrong_silicon_is_rejected(self):
        with self.assertRaises(ValueError):select('m09-s3-hw210','esp32c5')
        with self.assertRaises(ValueError):select('c5-reference-m301-hw120','esp32s3')
    def test_m301_c5_is_not_m09(self):
        p=select('c5-reference-m301-hw120')
        self.assertEqual(p['model'],'M301')
        self.assertEqual(p['source_model'],'M301')
        self.assertIsNone(p['xiaozhi'])
    def test_pin_sources_are_independent(self):
        p=profiles();self.assertEqual(len({x['pins'] for x in p}),len(p))
