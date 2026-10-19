# Firmware downloads

English · [简体中文](FIRMWARE_RELEASES.zh_CN.md)

Download firmware from [GitHub Releases](https://github.com/FoloToy/MagicBox/releases).
Match the complete hardware profile before choosing an image.

## MagicBox v0.1 · M09

| Item | Value |
|---|---|
| Profile | `m09-s3-hw210` |
| Hardware | MagicBox M09 · ESP32-S3 · PCB HW2.1.0 · 16 MB flash |
| Source branch | `demo/xiaozhi` |
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
a new build is not new physical acceptance. C5's sales model remains unconfirmed
and its XiaoZhi port is not released. Other S3 PCB revisions need their own
profiles. See [validation](VALIDATION.md) and [build instructions](XIAOZHI.md).
