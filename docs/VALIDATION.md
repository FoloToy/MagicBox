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

## 2026-10-19 M09 firmware prerelease

Historical packaging (subsequently superseded by MagicBox v0.1 and returned to draft): [M09 upstream-based 2.5.3 r1 source](https://github.com/FoloToy/MagicBox/tree/m09-s3-hw210-xiaozhi-v2.5.3-r1) was initially published as a prerelease, pinned to `d8acaff3c6d299050b3f74043ceea633915649ce` on `demo/magicbox`. The application and M09 BSP trees match the previously device-tested `ed8d2b5` source. This freshly built binary was not flashed during release creation; inherited hardware evidence is not fresh binary acceptance.

PASS: clean-source standard cloud variant built with ESP-IDF v6.1 for ESP32-S3, 16 MB flash, M09 HW2.1.0. Application is 2,196,464 bytes; SHA256 `ab32f858130488ade9c48d3b1604990a3d87ffffaf3651eb3854c17635d72197`. esptool v5.6.0 verified the chip, application descriptor, image checksum and appended validation hash. Generated configuration selects the M09 production partition CSV, `https://magic.folotoy.cn/xiaozhi/ota/`, `FoloToy` AP prefix and disabled demo Wi-Fi. Known private inference keys were absent from the binary.

PASS: repository checks and 8 tests; application host suite ran 126 tests successfully with 2 conditional skips in the SDK Python environment. An incomplete temporary component cache was preserved and regenerated through ESP-IDF before the successful build; no vendor component source was edited.

PASS: four published assets (application BIN, bilingual ZIP, manifest and SHA256SUMS) downloaded back from GitHub. All outer/inner checksums and ZIP integrity verified; remote tag resolved to the pinned source commit. Only an application update is packaged: no device dumps, identities, saved network/account settings, factory image or full-flash installer. No flashing or automatic OTA rollout occurred. C5 and other S3 revisions are outside this release. See [English release guide](FIRMWARE_RELEASES.md) and [中文升级说明](FIRMWARE_RELEASES.zh_CN.md).

## 2026-10-19 Independent MagicBox v0.1 release

Published [MagicBox v0.1](https://github.com/FoloToy/MagicBox/releases/tag/v0.1), pinned to `0e9fd3715568d51ab56f38e1088740936505b80c`. Public BIN/ZIP names are `magicbox-m09-s3-hw210-v0.1.*`; the application descriptor version is `0.1.0`. XiaoZhi is identified as the currently compatible protocol, not the MagicBox product/version. Existing protocol endpoints, port branch/directory and upstream attribution remain intact.

PASS: explicit M09 cloud variant built with ESP-IDF v6.1; 2,196,464-byte application SHA256 `68ea880307531d94adee944575d960f80ec84f6b508b3f6cfb8a4d5dd659e2a7`. esptool confirmed ESP32-S3/16 MB, version 0.1.0, valid checksum and validation hash. Repository checks and 8 tests passed; 126 port tests ran successfully with 2 conditional skips. All four GitHub attachments were downloaded back and their outer/inner SHA-256, ZIP integrity and manifest version verified.

The prior package is retained as a draft, with no asset deletion. Functional application/BSP code is unchanged from prior physical acceptance; the application version marker changed. This exact new binary was not flashed or newly device-tested. No automatic OTA rollout or existing-device reset occurred. The upstream-derived 2.5.3 and new independent 0.1.0 series require an explicit migration policy before any future version-ordered OTA rollout.

## 2026-10-19 Branch split: product firmware and upstream service

`demo/magicbox` is the renamed product branch. Its standard M09 build still emits the v0.1 application SHA256 `68ea880307531d94adee944575d960f80ec84f6b508b3f6cfb8a4d5dd659e2a7` (2,196,464 bytes), Magic Server URL and `FoloToy` AP prefix. Its nine repository/profile tests passed. Release v0.1 notes, manifest branch metadata and ZIP instructions now point to `demo/magicbox`; downloaded assets/checksums were verified and application bytes/source tag were preserved.

The recreated `demo/xiaozhi` retains pinned upstream 2.5.3 application/activation/protocol/MCP/common Wi-Fi behavior. Thirteen upstream core files are checked against hashes from commit `6cb01531c2ae961eb3fd64c5e97f65d20a597f7e`. Hardware adaptation remains M09 S3/HW2.1.0. It defaults to `https://api.tenclass.net/xiaozhi/ota/`, original `Xiaozhi` provisioning and no seeded Wi-Fi. Its independent build/OTA identities end in `-xiaozhi`.

PASS: ten original-service repository tests; 126 application host tests with two conditional skips; ESP-IDF 6.1 M09 build and esptool validation. New application size 2,196,480 bytes, SHA256 `72b5a635583ab82ce0fdf0d9ab56a9883ee52e6c906431267d2b38f40fe0ce9f`. Inspected generated config and binary for the original-service URL, upstream version 2.5.3 and independent board type; Magic Server URL and demo Wi-Fi were absent. A plain HTTPS request to the original OTA URL returned HTTP 200, without device identity or activation requests.

No flashing occurred. Original-service activation/binding/conversation, original-service OTA catalog support for the independent board ID, and physical hardware acceptance of the new binary remain unverified. Saved `wifi/ota_url` may override a new default and must be handled explicitly when moving an existing device between services. No transport/application/MCP core implementation was replaced.

## 2026-10-20 M301 C5 source profile

Owner confirms the connected ESP32-C5 device is **M301**. Added independent profile `m301-c5-rtc-v1-5g`, based on `magicbox_rtc_v1_5G` in folotoy-firmware-idf commit `8c013c923b59f1123e762880100d62ccf7005907`. The physical PCB revision remains null/pending; HW1.2.0 is not inferred. GPIOs, ES8311 clock/data directions, PMU flags and GX8002 ALDO4/BLDO1 rails are source-derived. The older `c5-reference-m301-hw120` retains its original implementation and SDK 5.5.3, including its different ALDO1/ALDO3 driver aliases. Matching GPIOs do not establish interchangeable power wiring.

The new BSP uses IDF 6.1, button 4.2.1 and esp_codec_dev 1.6.2. The pinned public FoloToy driver remains unmodified; its explicit I2C/SPI/LEDC dependencies and aggregate warning compatibility are supplied by BSP CMake. Build caches separate SDK series. No accelerometer driver or motion wake is claimed.

PASS: repository boundary checks, six profile/staging host tests and actual button adapter failure/cleanup tests. New profile ESP-IDF v6.1 builds:

| Application | Bytes | SHA256 |
|---|---|---|
| baseline | 255072 | `856185d3b3b427ce53d54f5abee143e642d372e055cb1e61413fabf8f12a0675` |
| hardware-demo | 357328 | `33f733b0e0a88d1b8ea95203d61d291dddae5de7448919b67ff06c31a2d8766a` |

Read-only ROM identification: ESP32-C5NR4 rev1.0, 16 MB Flash, 4 MB embedded PSRAM. Read-only partition table matches the cited source (factory 0x210000, app0 0x610000, app1 0xA10000, product NVS 0xF20000). No flash/settings/identity write occurred. PCB revision, peripheral/audio/RTC/GX8002/power behavior need separate physical acceptance. [M301 board notes](../boards/m301-c5-rtc-v1-5g/README.md).

## 2026-10-20 M301 MagicBox application port

Added a separate `folotoy/magicbox-c5` adapter and `magicbox-m301-c5-rtc-v1-5g` build/OTA identity on `demo/magicbox`. The new source profile, not the legacy C5 HW1.2.0 reference, selects this application. The BSP is the sole I2C/I2S/codec/PMU/button/ADC/RGB owner. Configuration uses C5 Quad PSRAM, 24 kHz mono ES8311 audio with 30 dB input gain, physical knob volume, Voice push-to-talk, Mode conversation/provisioning and AXP2101 battery reporting. No M09 microphone gain, motion sleep, external Mode pin or wake-chip model is reused.

Default service is Magic Server, provisioning AP prefix FoloToy, with no seeded network. The C5 partition CSV was compared entry-for-entry with the read-only device table. A local inspection of the server OTA response implementation found a placeholder version with an empty URL; the product client's version check now ignores empty-URL update offers rather than interrupting activation. This guard does not modify either transport implementation or the original-service branch. No live activation/conversation is claimed for this image.

PASS: repository boundary check; 11 profile/staging/CLI host tests, including cross-profile variant rejection and rejection of the old C5 reference; 126 application host tests with two conditional skips; touched C5 C++ formatting. ESP-IDF 6.1 builds of both product variants after the empty-URL guard:

| Profile | Bytes | SHA256 |
|---|---|---|
| m09-s3-hw210 | 2196480 | `97b54f438bd851f55226c197fe523639f1c7a4881bdb76e2da92db7c6751e8fa` |
| m301-c5-rtc-v1-5g | 1849104 | `19109e0b87ddedc8ebf7c74cb904c0fbe37b7fb7d410e3b2fa914d64eb9533b3` |

PASS: final C5 image chip/version/checksum validation with esptool; Magic Server endpoint and independent board identity present; demo Wi-Fi absent; image fits the 4 MB slot; generated production partition entries match the inspected device. Model is owner-confirmed M301, physical PCB revision remains null. No flashing, erasure, activation, factory/identity update or cloud rollout occurred. Boot/audio/controls/knob/network/activation/dialogue/reconnect/battery/RTC/wake-chip acceptance remains pending; M09 physical results are inherited evidence. [M301 application guide](../apps/xiaozhi/main/boards/folotoy/magicbox-c5/README.md).

## 2026-10-20 M301 app0 installation and first boot

User authorized installation of the newly prepared M301 firmware. A complete 16 MB flash backup was saved privately outside Git. Read-only security inspection reported Secure Boot and Flash Encryption disabled. The original boot selection was factory; both OTA application slots were empty. Installed the verified `m301-c5-rtc-v1-5g` product application from source `4f0d49c` into app0 at `0x610000`, then selected OTA slot 0 using the official SDK otatool. No factory image, bootloader, partition table, filesystem or product identity area was written.

Application: 1,849,104 bytes, SHA256 `19109e0b87ddedc8ebf7c74cb904c0fbe37b7fb7d410e3b2fa914d64eb9533b3`. esptool verified the written application. Separate post-write/before-first-boot verification confirmed the full factory partition, product identity NVS, partition table and original application settings unchanged against the private backup. OTA selection is the intentional metadata change; subsequent application startup may create its normal settings.

Fresh device evidence: the selected app boots on M301 C5, detects 4 MB PSRAM, reports AXP2101 initialization success, opens ES8311 at 24 kHz mono and starts the explicitly named M301 board/audio adapter. No panic was observed in the initial capture. Wi-Fi scanning started. Audible audio, physical controls, provisioning, activation, cloud dialogue, reconnect and power/sleep/RTC/wake-chip behavior remain separate acceptance items. This is application boot evidence, not acceptance of the standalone diagnostic or every BSP peripheral. Raw serial/security/flash logs and backups remain private outside Git.

## 2026-10-21 M301 v0.1 release package

Build: reuses the original ESP-IDF 6.1 application from `4f0d49c`; no new build
or flash was performed for publication. Application size is 1,849,104 bytes,
SHA256 `19109e0b87ddedc8ebf7c74cb904c0fbe37b7fb7d410e3b2fa914d64eb9533b3`.
Current esptool inspection validates ESP32-C5, 16 MB flash, version `0.1.0`,
image checksum and validation hash. Image fits the inspected 4 MB app0 slot at
`0x610000`; the published guide distinguishes it from M09.

Host: repository boundary checks and all 11 repository tests pass. All 126
application host tests complete successfully with 2 conditional skips after
activating the existing IDF Python environment; an initial run without the
venv PATH could not launch six CI-selection subprocesses. Package contents,
ZIP CRC, embedded application bytes and SHA-256 files are validated. Only the
application, bilingual instructions, manifest, hashes and MIT notices are in the
archive; device backups, settings, identity and factory images remain private.

Inherited device evidence for these exact bytes: the installation/boot record
above, plus the owner's confirmation that M301 was usable for cloud dialogue.
After a server-side reply fix, the owner confirmed a complete response without
an appended apology. This is not additional device testing during publication.
The remaining Mode/knob, fresh setup/activation, reconnect, battery/USB and
RTC/GX8002 limits are preserved in the [release guide](FIRMWARE_RELEASES.md#magicbox-v01--m301).

Publication: independent prerelease tag `v0.1-m301` on `demo/magicbox`; the tag
includes release documentation while application and BSP runtime source remain
unchanged from `4f0d49c`. No automatic OTA or device reset is configured.

Published at [MagicBox v0.1 · M301](https://github.com/FoloToy/MagicBox/releases/tag/v0.1-m301).
All four draft assets were downloaded and matched locally before publication;
all four published assets were downloaded again and matched byte-for-byte.
ZIP CRC/inner hashes and outer SHA-256 entries passed. The release is published
(not draft), marked prerelease, and its tag points to `a034023`. Existing M09
`v0.1` remains a separate published prerelease.


## 2026-10-21 M09 and M301 complete installation packages

No application rebuild or flash. Existing release application hashes remain
`68ea880307531d94adee944575d960f80ec84f6b508b3f6cfb8a4d5dd659e2a7`
(M09) and `19109e0b87ddedc8ebf7c74cb904c0fbe37b7fb7d410e3b2fa914d64eb9533b3`
(M301). Generated build bootloaders are ESP-IDF 6.1, chip IDs 9/23,
DIO/80 MHz/16 MB; esptool validates their checksums and image hashes.
The S3 bootloader is reused from the prior generated M09 build; C5 uses its
existing M301 build. Neither is read from a physical device backup.

Both binary partition tables exactly match their profile CSV regenerated by
the official SDK tool. Full images are merged with esptool 5.6.0 at target
offset zero. Every component and every FF padding byte is checked independently;
no unexplained bytes in the historical generated merges are accepted. Factory
contains the unchanged released application and OTA metadata is erased.
C5 also carries its generated four-byte empty model marker. NVS is FF; product
identity partitions are beyond both image ends and no identity/backups are used.
ZIP CRC, embedded-file hashes and inner/outer checksum files pass.

| Profile | Full image bytes | SHA-256 |
| --- | --- | --- |
| m09-s3-hw210 | 2262000 | `d0fe9293b35ec88cb7f6eebff606780be3f4f60cab92609cf9fa2de7a1117e4b` |
| m301-c5-rtc-v1-5g | 14745604 | `d82495126928257c2cdba072b49b12aade70e038aee888d8cba3a50685afd25a` |

Repository boundary checks and all 11 host tests pass. This is package
verification, not newly compiled firmware or new physical acceptance. Full
installation, first factory boot, blank-device provisioning and activation
remain untested for these complete installers; application device evidence
retains the earlier limitations. No connected device is flashed or erased.


## 2026-10-21 Test-network full installers

Created distinct `*-testwifi-full.bin` images for both supported product profiles.
No application rebuild or hardware flash. The existing Wi-Fi reader uses the
`wifi` NVS namespace and `ssid`/`password` string keys, with optional `channel`
uint8. The standard Wi-Fi board starts station mode when this list is populated
and retains its 60-second fallback to provisioning. The official ESP-IDF 6.1
NVS generator creates a fresh 0x4000-byte partition containing only this test
network; SDK JSON readback and NVS integrity checks pass. No device-saved
configuration, identity, tokens or other user data are used.

Every byte outside `0x9000`–`0xcfff` matches the corresponding published complete
installer. Existing released application, bootloader, production partition table,
OTA metadata and factory boot selection are unchanged. Product identity lies
outside the image; cloud activation is not bypassed. Full image hashes:

| Profile | Bytes | SHA-256 |
| --- | --- | --- |
| m09-s3-hw210 | 2262000 | `57a62272778735ea96c3a7f109f169a9baf4629a515baf5eb55dc9c2d8e4c948` |
| m301-c5-rtc-v1-5g | 14745604 | `3ae7fc7bc68f1cb8ba83b5c3a81e5c1c970ce20b88073bdf2be57e904f0d2bb0` |

Package contents, ZIP CRC and inner/outer checksums pass. Repository boundaries
and all 11 host tests pass. These tests do not establish an actual RF association
or cold-boot network acceptance; those remain a physical test for the owner.
The earlier application and full-installation limitations continue to apply.
