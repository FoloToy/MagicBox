import hashlib
import json
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]

class DemoService(unittest.TestCase):
    def test_original_server_has_independent_firmware_identity(self):
        data = json.loads((ROOT/'apps/xiaozhi/main/boards/folotoy/magicbox/config.json').read_text())
        self.assertEqual(data['type'], 'magicbox-m09-s3-hw210-xiaozhi')
        self.assertEqual(len(data['builds']), 1)
        b = data['builds'][0]
        self.assertEqual(b['name'], 'folotoy-magicbox-m09-s3-hw210-xiaozhi')
        self.assertIn('CONFIG_OTA_URL="https://api.tenclass.net/xiaozhi/ota/"', b['sdkconfig_append'])
        self.assertNotIn('magic.folotoy.cn', json.dumps(data))
        source = (ROOT/'apps/xiaozhi/main/boards/folotoy/magicbox/magicbox_board.cc').read_text()
        self.assertNotIn('AddSsid(', source)
        self.assertNotIn('FOLOTOY_LAND', source)

    def test_upstream_application_activation_transports_and_mcp_are_retained(self):
        manifest = json.loads((ROOT/'tests/upstream_core_sha256.json').read_text())
        for path, digest in manifest['sha256'].items():
            with self.subTest(path=path):
                self.assertEqual(hashlib.sha256((ROOT/'apps/xiaozhi'/path).read_bytes()).hexdigest(), digest)
