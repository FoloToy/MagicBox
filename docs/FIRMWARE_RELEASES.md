# Firmware downloads

English · [简体中文](FIRMWARE_RELEASES.zh_CN.md)

Download firmware from [GitHub Releases](https://github.com/FoloToy/MagicBox/releases).
Match the complete hardware profile before choosing an image.

## Test Wi-Fi installers

For setup-free connection to the test network, use the separate complete
installers below. Both write at **`0x0`** and connect to `FOLOTOY_LAND`
(password `makeitanything`), using Magic Server. The network must be available;
connection failure falls back to provisioning after approximately 60 seconds.
Device activation/cloud binding remains a separate step.

| Model | Test Wi-Fi installer bundle |
| --- | --- |
| M09 / S3 HW2.1.0 | [Download ZIP](https://github.com/FoloToy/MagicBox/releases/download/v0.1/magicbox-m09-s3-hw210-v0.1-testwifi.zip) |
| M301 / C5 rtc-v1-5g | [Download ZIP](https://github.com/FoloToy/MagicBox/releases/download/v0.1-m301/magicbox-m301-c5-rtc-v1-5g-v0.1-testwifi.zip) |

These packages preset only the Wi-Fi NVS region of the verified full image;
application, bootloader and all other bytes remain unchanged. They reset local
settings and replace the original factory application like the standard full
installer. No product identity or existing cloud binding is included or reset.
An application-only update cannot install this preset. Later manual Wi-Fi
changes are retained; clearing saved networks requires provisioning or
reinstalling this test package. Standard images below still have no seeded Wi-Fi.

To reproduce, use `tools/package_test_wifi.py --board PROFILE
--source-manifest PATH --wifi-file PRIVATE_JSON --idf-path IDF_PATH --output DIR`
with the matching verified complete-installer manifest. The private JSON holds
`ssid` and `password`; the tool uses the SDK NVS generator/parser and checks that
no byte outside NVS changes. Keep local configuration outside Git. Only use a
network intended for the recipients of the generated test package. Offline
checks passed; no new physical Wi-Fi test has been performed.

## Installation and update packages

Both released models now provide two formats. Each bundle contains both BINs,
English/Chinese instructions, a manifest and SHA-256 checksums.

| Model | Complete installer, write at `0x0` | Existing application update |
| --- | --- | --- |
| M09 / S3 HW2.1.0 | [Full BIN](https://github.com/FoloToy/MagicBox/releases/download/v0.1/magicbox-m09-s3-hw210-v0.1-full.bin) · [Bundle ZIP](https://github.com/FoloToy/MagicBox/releases/download/v0.1/magicbox-m09-s3-hw210-v0.1-bundle.zip) | Existing BIN unchanged; verified app0 `0x410000` |
| M301 / C5 rtc-v1-5g | [Full BIN](https://github.com/FoloToy/MagicBox/releases/download/v0.1-m301/magicbox-m301-c5-rtc-v1-5g-v0.1-full.bin) · [Bundle ZIP](https://github.com/FoloToy/MagicBox/releases/download/v0.1-m301/magicbox-m301-c5-rtc-v1-5g-v0.1-bundle.zip) | Existing BIN unchanged; verified app0 `0x610000` |

Full installers include the matching generated ESP-IDF 6.1 bootloader and
production partition table, erased OTA metadata and the unchanged released
application in the factory slot. Initial boot uses factory (`0x10000` on M09,
`0x210000` on M301). They replace the original factory application and reset
local settings in their written range. No full-chip erase is needed; product
identity partitions lie beyond the image and are not included. eFuses and
server-side device bindings remain unchanged. These are not original factory
recovery images. Use only with the exact profile, 16 MB flash and disabled
Secure Boot/Flash Encryption.

Application-only updates retain their original bytes and requirements below.
The `*-full-manifest.json` separately records component offsets, hashes and the
written range. Segment/padding, partition-table and archive checks passed; full
installers have not received new blank-device hardware testing.

## MagicBox v0.1 · M09

| Item | Value |
|---|---|
| Profile | `m09-s3-hw210` |
| Hardware | MagicBox M09 · ESP32-S3 · PCB HW2.1.0 · 16 MB flash |
| Source branch | `demo/magicbox` |
| Build variant | `folotoy-magicbox-m09-s3-hw210` |
| SDK | ESP-IDF 6.1 |
| Server | `https://magic.folotoy.cn/xiaozhi/ota/` |
| Provisioning | `FoloToy-XXXX`, then `http://192.168.4.1` |

The release includes an application-only `.bin`, a ZIP containing that image,
bilingual instructions, a source/build manifest and SHA-256 checksums. The public release is `v0.1`; the ESP-IDF application descriptor uses
`0.1.0`. MagicBox versions are independent of upstream firmware versions.
This firmware currently supports the XiaoZhi protocol. Protocol names identify
compatibility rather than the product or release name.
No saved Wi-Fi, account credentials, device identity, flash dumps or factory
images are included. The cloud variant does not seed the development Wi-Fi.

### Existing-device update

The application is under 4 MB. On the previously tested M09 production layout,
`app0` occupies `0x410000`–`0x80ffff` and is selected for boot. Before a serial
update, independently verify the hardware, actual partition table, selected boot
slot, image size and SHA-256. Privately back up application/settings/identity.
Only after these checks, writing the application to the verified app0 slot is:

```sh
python -m esptool --chip esp32s3 --port PORT write-flash 0x410000 magicbox-m09-s3-hw210-v0.1.bin
```

This command assumes the installed esptool uses the hyphenated subcommand syntax.
It does not select an OTA boot slot. If a different slot/layout is active, stop
and obtain the correct update plan. Do not erase flash or overwrite the
bootloader, partition table, NVS, factory app, OTA-selection data or product
identity. An application-only image is not a blank-device factory installer.
Release creation does not flash a connected device or configure automatic OTA.

### Setup and validation

With no saved network, connect a phone to `FoloToy-XXXX` and configure Wi-Fi at
`http://192.168.4.1`. Sign in or register at [Magic Console](https://magic.folotoy.cn)
and bind the device using its activation code. Existing Wi-Fi and bindings are
retained by an application-only update. Hold Mode for three seconds to enter
Wi-Fi provisioning. Voice is push-to-talk; Mode changes the conversation mode.

The source version has prior M09 device acceptance for provisioning, activation,
voice, buttons and volume, and was used in the Magic Server conversation tests.
Each release manifest separately records whether that exact binary was flashed;
a new build is not new physical acceptance. M301 C5 has a separate release
below. Other S3 PCB revisions need their own profiles. See [validation](VALIDATION.md) and [build instructions](MAGICBOX.md).

## MagicBox v0.1 · M301

[Download the M301 prerelease](https://github.com/FoloToy/MagicBox/releases/tag/v0.1-m301).
The hardware suffix distinguishes this release tag from the existing M09 `v0.1`.
Both use application version `0.1.0`.

| Item | Value |
| --- | --- |
| Profile | `m301-c5-rtc-v1-5g` |
| Hardware | MagicBox M301 · ESP32-C5 · 16 MB flash · 4 MB Quad PSRAM |
| Source board | `magicbox_rtc_v1_5G`; physical PCB revision unconfirmed |
| Source branch | `demo/magicbox` |
| Firmware source | `4f0d49c4e0cb2a3a3cbb5652fc8cf75f32545c02` |
| SDK | ESP-IDF 6.1 |
| Application BIN | `magicbox-m301-c5-rtc-v1-5g-v0.1.bin` |
| Server | `https://magic.folotoy.cn/xiaozhi/ota/` |
| Provisioning | `FoloToy-XXXX`, then `http://192.168.4.1` |
| SHA-256 | `19109e0b87ddedc8ebf7c74cb904c0fbe37b7fb7d410e3b2fa914d64eb9533b3` |

The BIN is the exact 1,849,104-byte application previously installed on the tested
M301. Boot/PSRAM/AXP2101/ES8311 initialization was observed, and the owner confirmed
cloud dialogue. Publication reuses those bytes rather than rebuilding them.
The ZIP contains the application, bilingual instructions, manifest, checksums and
MIT notices. It contains no bootloader, partition table, factory image, saved
Wi-Fi, product identity or device backup.

On the previously inspected M301 layout, app0 is `0x610000` with a 4 MB capacity.
This differs from M09. Independently verify the complete profile, actual partition
map and active boot slot, and privately back up the device before updating.
Only for a device with the verified app0 layout and boot selection:

```sh
python -m esptool --chip esp32c5 --port PORT write-flash 0x610000 magicbox-m301-c5-rtc-v1-5g-v0.1.bin
```

Writing an application does not select its boot slot. Preserve factory, identity,
NVS, partition table and OTA selection; an unprovisioned chip needs a separate
installation plan. This release does not flash a device or enable automatic OTA.

Voice is push-to-talk. Mode toggles conversation; hold it for one second for
Wi-Fi provisioning. The knob controls volume. M301 does not enable M09's motion
wake, automatic sleep, wake words or AEC. RTC/GX8002 APIs, battery behavior,
Mode/knob range, fresh activation and reconnect remain separate acceptance items.
The older `c5-reference-m301-hw120` profile is not interchangeable with this
source-derived M301 power configuration.
