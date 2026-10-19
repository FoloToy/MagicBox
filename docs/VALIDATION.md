# Validation and provenance — 2026-10-16

## Build

PASS: each native profile's minimal baseline and standalone hardware diagnostic compiled with its declared SDK. Build metadata and hashes are exported under `build/<profile>/<app>/build.json` (generated/ignored).

| Profile | Application | SDK | Bytes | SHA256 |
|---|---|---|---|---|
| c5-reference-m301-hw120 | baseline | ESP-IDF v5.5.3 | 279984 | `6859f87250107d283beb24a087a347c624e512b4206761c9016dcf22e102f712` |
| c5-reference-m301-hw120 | hardware-demo | ESP-IDF v5.5.3 | 377040 | `bd2d6d9f7852c040afc28245c3a9f0861fb1afa1d087bc3f9dd19b8a41dfde1a` |
| m09-s3-hw210 | baseline | ESP-IDF v6.1 | 230272 | `6a309fe742f542817dd44ccf7eede545b900d517882826d632af48e4f4baa67e` |
| m09-s3-hw210 | hardware-demo | ESP-IDF v6.1 | 270432 | `cace71f5570e84bc5d4f76ce890ed28b8d0ea2e3eed398680c237a9fcd273ce3` |

PASS: preserved M09 Arduino baseline and hardware demo compile with PlatformIO espressif32 6.7.0 / Arduino. Their canonical M09 pin header is shared with the native project.

## Host

PASS: registry tests reject unknown/silicon-only/revision-guessed profiles and wrong silicon; C5 remains model-unconfirmed. PASS: actual imported C5 button adapter tests cover failure cleanup, retry, idempotence and bounds. PASS: repository BSP/application/credential boundary checks and new M09 source formatting.

## Device and limits

No new repository image was flashed during this migration. The previously installed M09 XiaoZhi firmware remains on the unit. Earlier Arduino/M09 XiaoZhi tests confirmed audio input/output, buttons including GPIO6, knob, Wi-Fi/online conversation and human-observed motion-wake conversation. That is inherited hardware evidence, not device acceptance for the new standalone ESP-IDF BSP. Motion-wake timing, actual sleep counts and RTC continuity were not captured in that earlier acceptance window; current draw was not measured.

C5 physical model mapping, audio/power/peripheral acceptance and motion-wake are unverified in this task. The C5 reference BSP includes BM8563 RTC/GX8002/AXP2101/ES8311; no C5 XiaoZhi application is claimed. Another S3 PCB is unsupported until a separately identified profile is added and validated.

## Source provenance / 来源

- Original public M09 Arduino BSP and working native XiaoZhi port: [FoloToy/magicbox-arduino at 8bab884](https://github.com/FoloToy/magicbox-arduino/tree/8bab8841109f7c891b70f4705a61473767495056). Owner identifies the tested HW2.1.0 / S3 device as M09. Native standalone M09 BSP is a new adaptation, not the old Arduino implementation.
- Public C5 reference BSP: [FoloToy/magicbox-c5 at f889d36](https://github.com/FoloToy/magicbox-c5/tree/f889d36f721dce7c503cdacf67f18de12a082ca2), source M301 HW1.2.0. Sales/model mapping is pending owner confirmation.
- XiaoZhi upstream (demo branch only): [78/xiaozhi-esp32 at 6cb0153](https://github.com/78/xiaozhi-esp32/tree/6cb01531c2ae961eb3fd64c5e97f65d20a597f7e), version 2.5.3, MIT. Original upstream licenses remain in place; no private product application, identity backup or credentials were imported.

## 2026-10-20 C5 source reconciliation and ESP-IDF 6.1 migration

The cited folotoy-firmware-idf commit `8c013c923b59f1123e762880100d62ccf7005907` (`magicbox_rtc_v1_5G` family) matches the existing C5 reference GPIO map and PMU flags. Reused `c5-reference-m301-hw120`; no model/PCB identity was inferred from the chip. Corrected GX8002 power rails from generic ALDO1/ALDO3 aliases to source-specific ALDO4 3.3 V / BLDO1 1.0 V with ordered 10 ms sequencing. Added provenance, the source IMU interrupt definition and bilingual board notes. No accelerometer driver or motion wake is claimed.

Migrated C5 BSP dependencies to IDF 6.1, button 4.2.1 and esp_codec_dev 1.6.2. The pinned public FoloToy driver remains unmodified; its explicit I2C/SPI/LEDC dependencies and zero-initialized aggregate warning compatibility are supplied by BSP CMake. Build caches now separate SDK series.

PASS: repository boundary check, five host tests and the actual C5 button adapter host test. ESP-IDF v6.1 builds:

| Application | Bytes | SHA256 |
|---|---|---|
| baseline | 255072 | `976a02baf2b8a3c7ea15cfcc1e3d701d6234efd2c7fe14701199ae7ea4102724` |
| hardware-demo | 357328 | `a695b98b34ceb9b56d2d812a946b9b9e54d4278531c97a4f322bd7a3d9a20c4a` |

Read-only ROM identification found ESP32-C5NR4 rev1.0, 16 MB Flash, 4 MB embedded PSRAM. Read-only partition-table inspection matched the cited source (factory 0x210000, app0 0x610000, app1 0xA10000, product NVS 0xF20000). No flash/settings/identity write occurred. PCB/model mapping, audio, buttons, knob, RTC, GX8002 and power behavior require physical acceptance of a separately authorized image. [C5 board notes](../boards/c5-reference-m301-hw120/README.md).
