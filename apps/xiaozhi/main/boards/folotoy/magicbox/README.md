# FoloToy MagicBox HWv2.1.0 (ESP32-S3)

Independent XiaoZhi board identity: manufacturer `folotoy`, type `magicbox-m09-s3-hw210`, release variant `folotoy-magicbox-m09-s3-hw210`.

The adapter shares hardware facts with the outer repository's `boards/m09-s3-hw210/esp-idf/components/bsp/include/bsp_pins.h`. Mode toggles chat (hold three seconds for provisioning); Voice is push-to-talk. GPIO8 controls playback volume without repeated NVS writes. GPIO46 is the XiaoZhi state LED. No display, camera, wake-word model or echo reference is claimed.

Build from the outer repository root with ESP-IDF 6.1 (the wrapper handles paths with spaces):

```sh
./tools/build_xiaozhi.sh
```

See the outer repository's `docs/MAGICBOX.md` and `docs/BRANCHES.zh_CN.md` for preservation requirements, limitations and validation. Never flash the merged release image over a production device without reviewing its bootloader, partition and data impact.

This variant is M09 / ESP32-S3 / HW2.1.0 only. The original M09 application used Arduino; this application uses ESP-IDF 6.1. Another S3 PCB needs a separately named variant. Motion hardware is supplied by the shared BSP `bsp_motion.h`; audio and sleep/network policy are application-owned.
