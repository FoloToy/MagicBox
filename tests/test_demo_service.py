import json
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]

class DemoService(unittest.TestCase):
    def test_magic_server_and_local_variants_remain_separate(self):
        data = json.loads((ROOT/'apps/xiaozhi/main/boards/folotoy/magicbox/config.json').read_text())
        self.assertEqual(data['type'], 'magicbox-m09-s3-hw210')
        builds = data['builds']
        self.assertEqual([b['name'] for b in builds], ['folotoy-magicbox-m09-s3-hw210', 'folotoy-magicbox-m09-s3-hw210-local'])
        self.assertIn('CONFIG_OTA_URL="https://magic.folotoy.cn/xiaozhi/ota/"', builds[0]['sdkconfig_append'])
        self.assertIn('CONFIG_FOLOTOY_MAGICBOX_DEMO_WIFI=n', builds[0]['sdkconfig_append'])
        self.assertIn('CONFIG_FOLOTOY_MAGICBOX_DEMO_WIFI=y', builds[1]['sdkconfig_append'])
