# MagicBox C5 reference BSP

[简体中文](README.zh_CN.md) · [Registry](../manifest.json)

This profile identifies source board **magicbox_rtc_v1_5G** and model **M301**, without assigning a PCB revision. The owner confirms the connected device is **M301**; the physical PCB revision remains unconfirmed. It is not M09.

## Source and hardware

The C5 `magicbox_rtc_v1_5G` family in FoloToy/folotoy-firmware-idf commit `8c013c923b59f1123e762880100d62ccf7005907` defines this profile's GPIO assignments, PMU board-type flags, ES8311 clock source and 24 kHz audio format. The older, separate reference is [FoloToy/magicbox-c5](https://github.com/FoloToy/magicbox-c5/tree/f889d36f721dce7c503cdacf67f18de12a082ca2). The cited source has several application variants with the same hardware pins; their product behavior is not part of this BSP.

| Function | Configuration |
|---|---|
| I2C | SDA 1, SCL 0; AXP2101 0x34, ES8311 0x18, BM8563 0x51, GX8002 0x2F |
| I2S | MCLK 5, BCLK 25, WS 23, MCU output 4, MCU input 24 |
| Amplifier / RGB | GPIO26 / GPIO27 |
| Mode / Voice | GPIO3 / GPIO2, active low |
| Knob | GPIO6, ADC1 channel 5 |
| Interrupts | PMU 10, RTC 8, GX8002 7; source mentions IMU 9 |

`esp-idf/components/bsp/include/bsp_pins.h` is the only pin authority. GX8002 rails follow the cited source: **ALDO4 3.3 V first, BLDO1 1.0 V after 10 ms**; shutdown reverses that order. Generic driver VddIO/VddCore helpers select different rails and must not be used here. The BSP initializes no accelerometer: an interrupt define alone does not establish motion wake support. RTC and GX8002 APIs are available for explicit application use.

## Build

Use ESP-IDF **6.1**. The older HW1.2.0 reference remains separate on SDK 5.5.3. Identical GPIOs do not establish identical PMU rail wiring.

```sh
source /path/to/esp-idf-v6.1/export.sh
python tools/build.py --board m301-c5-rtc-v1-5g --app baseline
python tools/build.py --board m301-c5-rtc-v1-5g --app hardware-demo
```

Run commands from the repository root. Baseline is minimal; the opt-in diagnostic exercises buttons, ADC, audio, power, RTC and wake-chip initialization. Neither build command writes hardware. Product firmware belongs on `demo/magicbox`.

## Device evidence and partition safety

Read-only identification on 2026-10-20 found **ESP32-C5NR4 revision 1.0, 16 MB Flash and 4 MB embedded PSRAM**. The connected partition table matches the cited source: factory `0x210000`, app0 `0x610000`, app1 `0xA10000` (each 4 MB), product NVS `0xF20000`. This differs from M09 S3. No image or settings were written. Silicon and partition evidence does not verify the PCB, peripherals, audio or power behavior.

Standalone BSP partitions are development layouts. Never write their merged image or partition table over the connected production device. Any future update needs explicit selection of this profile, a verified application image, the actual boot slot, private backup and preservation of factory, filesystem, product identity and saved settings.
