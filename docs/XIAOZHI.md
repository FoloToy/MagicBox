# M09 XiaoZhi original-service port

English · [简体中文](XIAOZHI.zh_CN.md)

Use `demo/xiaozhi` for the pinned upstream XiaoZhi application (2.5.3), connected
to `https://api.tenclass.net/xiaozhi/ota/` and the [original console](https://xiaozhi.me).
The Magic Server product application is on `demo/magicbox`.

```sh
source /path/to/esp-idf-v6.1/export.sh
./tools/build_xiaozhi.sh --board m09-s3-hw210 \
  --variant folotoy-magicbox-m09-s3-hw210-xiaozhi
python3 tools/check_repo.py
python -m unittest discover -s tests -v
(cd apps/xiaozhi && python -m unittest discover -s scripts/tests -v)
```

The build selects ESP32-S3 / M09 HW2.1.0, the shared canonical BSP pins and the
M09 production-layout partition CSV. Its OTA type is
`magicbox-m09-s3-hw210-xiaozhi`, independent of the Magic Server variant.
The original upstream provisioning behavior uses `Xiaozhi-XXXX`. No custom
server URL or demo Wi-Fi is seeded. Application/OTA/protocol/MCP and common
Wi-Fi code are checked against a pinned upstream file-hash manifest.

The hardware adapter owns its I2S/motion bus. Voice is push-to-talk; Mode and
external GPIO6 toggle conversation mode; long-press Mode enters provisioning.
Knob volume, microphone conversion gain and LIS3DH motion wake are retained.
No screen, camera, wake-word or AEC acceptance is claimed on this hardware.

Existing `wifi/ota_url` in NVS overrides the compiled default. Moving between
services requires reviewing that setting and activating/binding separately on
the target service. Do not erase product identity or overwrite factory firmware
and partition/OTA-selection data. Build tools do not flash; this task does not
change any connected device. The original service may provide its own OTA
policy: review returned board identity before accepting a firmware update.

The new variant requires fresh physical original-service activation/conversation
acceptance. Prior Magic Server device evidence does not establish that result.
See [branch roles](BRANCHES.md) and [validation](VALIDATION.md).
