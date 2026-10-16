import sys,tempfile,unittest
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from staging import sync_tree
class Staging(unittest.TestCase):
    def test_deleted_source_is_removed_but_generated_state_survives(self):
        with tempfile.TemporaryDirectory() as temp:
            src=Path(temp)/'src';dst=Path(temp)/'dst';src.mkdir();dst.mkdir()
            (src/'main').mkdir();(dst/'main').mkdir()
            (src/'main/code.cpp').write_text('updated')
            (dst/'main/code.cpp').write_text('old')
            (dst/'main/deleted.cpp').write_text('obsolete')
            (dst/'build').mkdir();(dst/'build/cache').write_text('keep')
            (src/'build').mkdir();(src/'build/cache').write_text('not a source')
            (dst/'main/generated.h').write_text('keep')
            sync_tree(src,dst,['build/','main/generated.h'])
            self.assertEqual((dst/'main/code.cpp').read_text(),'updated')
            self.assertFalse((dst/'main/deleted.cpp').exists())
            self.assertEqual((dst/'build/cache').read_text(),'keep')
            self.assertEqual((dst/'main/generated.h').read_text(),'keep')
