# MagicBox M301 · ESP32-C5

[简体中文](README.zh_CN.md) · [BSP and source notes](../../../../../../boards/m301-c5-rtc-v1-5g/README.md)

M301 is owner-confirmed. The profile identifies the source board; the physical PCB revision remains unconfirmed. The older HW1.2.0 reference is a separate power configuration. The GPIO map and power configuration match the `magicbox_rtc_v1_5G` family at folotoy-firmware-idf commit `8c013c923b59f1123e762880100d62ccf7005907`.

## Build

From the repository root, using ESP-IDF 6.1:

```sh
./tools/build_xiaozhi.sh --board m301-c5-rtc-v1-5g
```

| Setting | Value |
|---|---|
| Variant | `folotoy-magicbox-m301-c5-rtc-v1-5g` |
| Board / OTA identity | `magicbox-m301-c5-rtc-v1-5g` |
| Target | ESP32-C5, 16 MB Flash, 4 MB Quad PSRAM on the identified device |
| Service | `https://magic.folotoy.cn/xiaozhi/ota/` |
| Provisioning AP | `FoloToy-XXXX`, no seeded Wi-Fi |
| Application version | MagicBox `0.1.0` |

The wrapper isolates each hardware profile's staged sources and generated build cache. It rejects an M09 variant with a C5 profile and the reverse. Build metadata records the selected profile, variant, board identity and image checksum. Build output does not authorize a write to the device.

## Controls and implementation

| Control | Behavior |
|---|---|
| Voice (GPIO2) | Hold to talk, release to send |
| Mode (GPIO3) | Click to toggle a conversation; hold one second for Wi-Fi provisioning |
| Knob (GPIO6) | Increasing ADC value increases volume; filtered updates, no continuous NVS writes |
| RGB (GPIO27) | Green idle, red listening, blue speaking, yellow network setup |

The BSP is the sole owner of I2C, I2S/ES8311, AXP2101, buttons, ADC and RGB. The application adapts these APIs to the existing protocol and audio services. It uses 24 kHz, 16-bit mono audio and 30 dB codec microphone gain; it does not apply the M09 digital microphone's 8x conversion. Startup audio uses the physical knob position. Battery status is supplied by AXP2101.

No automatic PMU sleep, motion wake, external Mode mapping, wake-word model, AEC, display or camera is enabled for this initial C5 port. RTC and GX8002 remain available as opt-in BSP APIs; the demo does not program alarms or update the wake chip. Their acceptance must not be inferred from an interrupt pin or a successful build.

## Production layout and acceptance

`partitions/folotoy-magicbox-m301-c5.csv` matches the cited source and the connected device's read-only partition inspection: factory `0x210000`, app0 `0x610000`, app1 `0xA10000`, each 4 MB; product identity is in the separate `folotoy` NVS at `0xF20000`. The M09 application offset is not valid for this C5 layout.

No flashing, erasure, factory replacement or device/account activation is performed by this change. Before a separately authorized update: verify the physical PCB and boot slot, back up the existing app/settings/identity privately, validate the C5 image checksum/size and update only the approved application slot. Preserve the partition table, factory app, filesystem and product NVS. Never use a merged image on the production unit.

Physical acceptance remains pending: boot/PSRAM/peripheral detection, audible prompts, Voice capture/playback, Mode, knob direction/endpoints, Wi-Fi provisioning, activation, dialogue, reconnect, battery/USB behavior and restart persistence. The M09's earlier hardware acceptance does not apply to M301.
