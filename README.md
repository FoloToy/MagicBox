# MagicBox

Multi-generation FoloToy MagicBox hardware BSPs. Select **model + chip + PCB revision**, not chip alone. [中文说明](README.zh_CN.md) · [Architecture](docs/ARCHITECTURE.md) · [Validation](docs/VALIDATION.md)

| Profile | Hardware | SDK | Status |
|---|---|---|---|
| `m09-s3-hw210` | M09 / ESP32-S3 / HW2.1.0 | ESP-IDF 6.1 | Owner-confirmed model; original firmware was Arduino |
| `c5-reference-m301-hw120` | ESP32-C5 / source M301 / HW1.2.0 | ESP-IDF 5.5.3 | Sales model mapping pending; reference hardware |

`main` contains BSPs, minimal entry points and separate hardware diagnostics. The [`demo/xiaozhi`](https://github.com/FoloToy/MagicBox/tree/demo/xiaozhi) branch adds the XiaoZhi application. M09's original Arduino BSP is preserved in `legacy/m09-arduino`; the current XiaoZhi firmware uses native ESP-IDF. C5 BSP availability does not imply a validated C5 XiaoZhi port.

```sh
python3 tools/build.py --list
# Activate the SDK required by the selected profile, then:
source /path/to/esp-idf-v6.1/export.sh
python tools/build.py --board m09-s3-hw210 --app baseline
python tools/build.py --board m09-s3-hw210 --app hardware-demo
# C5 uses its own ESP-IDF 5.5.3 environment:
python tools/build.py --board c5-reference-m301-hw120 --app baseline
python tools/build.py --board c5-reference-m301-hw120 --app hardware-demo
```

Builds stage into an owned whitespace-free cache and export profile-labelled artifacts under `build/<profile>/<app>`. Build tools never flash. Development partition layouts must not be applied to production units without checking identity/factory preservation. A new S3 PCB needs a new profile and independent validation.

M09: direct digital I2S microphone/speaker, RGB, Mode/Voice, external Mode, knob, LIS3DH. C5 reference: ES8311, AXP2101, BM8563 RTC, GX8002, RGB, buttons and knob. These devices/pins/power sequences are not interchangeable. See profile pin headers and upstream provenance in the validation document.
