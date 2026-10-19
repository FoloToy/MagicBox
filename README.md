# XiaoZhi for MagicBox M09

English · [简体中文](README.zh_CN.md)

This is **`demo/xiaozhi`**, the M09 hardware port of the original XiaoZhi
application and service. It retains upstream application version **2.5.3** from
commit `6cb01531c2ae961eb3fd64c5e97f65d20a597f7e`, including OTA activation,
WebSocket and MQTT/UDP transports, MCP tools and conversation modes.

| Branch | Purpose |
|---|---|
| [`main`](https://github.com/FoloToy/MagicBox/tree/main) | Multi-generation BSPs and minimal hardware examples |
| [`demo/magicbox`](https://github.com/FoloToy/MagicBox/tree/demo/magicbox) | MagicBox v0.1 product firmware for Magic Server |
| [`demo/xiaozhi`](https://github.com/FoloToy/MagicBox/tree/demo/xiaozhi) | This original-service M09 port |

## Hardware and service

- Hardware profile: **M09 / ESP32-S3 / HW2.1.0**, `m09-s3-hw210`.
- Default OTA/activation service: `https://api.tenclass.net/xiaozhi/ota/`.
- Provisioning hotspot: `Xiaozhi-XXXX`; open `http://192.168.4.1`.
- Manage the activated device in the original [xiaozhi.me console](https://xiaozhi.me).
- Independent build/OTA identity: `folotoy-magicbox-m09-s3-hw210-xiaozhi` / `magicbox-m09-s3-hw210-xiaozhi`.

M09 audio, buttons, external Mode button, volume knob and motion adaptations
are retained. Optional upstream features remain in the source, but M09 has no
screen/camera, and this build does not enable wake-word detection or AEC. C5's
BSP remains available separately; a C5 application port is not claimed.

## Build

```sh
source /path/to/esp-idf-v6.1/export.sh
./tools/build_xiaozhi.sh --board m09-s3-hw210
```

No Magic Server endpoint or demo Wi-Fi is seeded by this variant. Existing
saved `wifi/ota_url` overrides the build default: inspect it before moving a
device between services. Building does not flash or migrate bindings/accounts.

[Service/build guide](docs/XIAOZHI.md) · [Branch guide](docs/BRANCHES.md) ·
[Validation](docs/VALIDATION.md) · [Upstream documentation](apps/xiaozhi/README.md)

The MIT license and upstream attribution remain intact. This branch retains the
pinned source; it does not claim the latest upstream release or fresh physical
acceptance on the original service.
