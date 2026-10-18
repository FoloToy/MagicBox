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

## New repository XiaoZhi variant

PASS: native ESP-IDF 6.1 build of `folotoy-magicbox-m09-s3-hw210` with board type `magicbox-m09-s3-hw210`. Local application size 2,196,464 bytes, SHA256 `da64e9ad5fad53aa25e2128836c6f368aab9f02132a4f8703881090d72d76f9e`; fits the preserved 4MB application slot. Generated sdkconfig selects ESP32-S3 and the explicit M09 HW2.1.0 board.

PASS: all 129 XiaoZhi host tests (one conditional skip), plus 8 repository/profile/staging/CLI tests. Missing board selection, C5 XiaoZhi selection and upstream variant override are rejected. The build wrapper now uses Python staging, preserving generated dependencies while removing obsolete sources. Shared M09 pins and LIS3DH primitive come from the BSP.

No newly named image was flashed during repository migration. Earlier hardware acceptance remains inherited evidence from the original port; C5 XiaoZhi is not implemented.

## Cloud provisioning update — 2026-10-18

PASS: ESP-IDF 6.1 builds of both M09 XiaoZhi variants. Standard cloud image: 2,196,464 bytes, SHA256 `1e1c0130603d31cf0bade829b18a903018f0bc807e88546dad3e2c95505e92d6`. Local variant: 2,196,736 bytes, SHA256 `cda087837435fefb0238b9ac883d9e8d090449186273992fcd1cd10499039d00`. Repository checks and all 8 repository tests passed; XiaoZhi ran 126 tests successfully with 2 dependency-related skips. Touched C++ formatting passed.

New physical-device evidence: the connected, previously identified M09 / ESP32-S3 / HW2.1.0 had the expected production partition table and running app0 slot. The current application/settings/product identity were backed up privately. Only app0 at `0x410000` was updated, with written-data hash verification; application NVS at `0x9000` (16 KiB) was cleared for a fresh setup. Product identity and partition-table hashes matched afterward; factory and other application slots were not written. Serial confirmed the explicit M09 board and transition to Wi-Fi provisioning with an AP and DHCP server. No demo Wi-Fi was seeded.

The cloud account's sole synthetic deployment-test device was removed and the device list was verified empty. User Wi-Fi provisioning, account registration/binding and this physical device's cloud conversation remain for manual acceptance. Audio/buttons/knob/motion acceptance from earlier tests is inherited; those functions were not retested by this update.

## FoloToy provisioning prefix — 2026-10-18

PASS: both M09 XiaoZhi variants build with `CONFIG_WIFI_AP_SSID_PREFIX="FoloToy"`; other boards retain their existing default. The same prefix configures the provisioning AP and station DHCP hostname. Repository checks and 8 repository tests passed; XiaoZhi ran 126 tests successfully with 2 dependency-related skips. Formatting of the changed C++ line passed.

The standard cloud image (2,196,464 bytes, SHA256 `6f764e4af8e5f46cfbf1ae415f9beda7675f15c6915906ce3d11cb91156cfa42`) was flashed to the verified M09 app0 slot and its written-data hash verified. Current app/settings/identity were backed up privately. Application NVS, OTA selection, partition table and product identity hashes were unchanged after writing; factory and other slots were not written. Serial confirmed the `FoloToy-` station hostname. The device had saved Wi-Fi at this point, so an active provisioning hotspot was not forced for this check. Cloud service endpoint is unchanged.
