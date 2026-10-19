# Profiles and ownership

A profile identifies model + silicon + PCB revision, for example `m09-s3-hw210`. The registry never chooses hardware from the detected chip alone. Missing/unknown profiles and wrong chip selections fail before a build. Add another S3 PCB as a new profile with its own pins, capabilities, provenance and acceptance evidence. No wildcard/default board is allowed.

`boards/<profile>/esp-idf` is an isolated BSP project with its own SDK/dependencies and minimal default main. Diagnostics live under that project's `examples/hardware-demo` and are built separately. M09 uses IDF 6.1; the imported C5 reference retains its tested IDF 5.5.3 dependency contract and power sequencing. SDK constraints are intentionally per profile, not silently widened. C5 sales model mapping is still unknown.

The original Arduino implementation is maintained as history in the archived [FoloToy/magicbox-arduino repository](https://github.com/FoloToy/magicbox-arduino); no legacy source tree is included here. The current M09 BSP and XiaoZhi port use native ESP-IDF 6.1.

`main` has no XiaoZhi application. `demo/magicbox` descends from main and adds `apps/xiaozhi`; BSP fixes can be merged from main. Its M09 application owns its I2S and motion bus; do not initialize a second BSP bus/audio owner in the same application. Only profiles explicitly marked XiaoZhi-supported can be selected; C5 BSP support is not a claim that a C5 XiaoZhi image exists.

C5 reference hardware source: FoloToy/magicbox-c5 M301 HW1.2.0. It uses ES8311, AXP2101, BM8563 and GX8002, distinct from M09's direct digital I2S microphone/speaker and LIS3DH. Do not copy S3 wake masks to C5. The C5 source mentions an accelerometer interrupt but no initialized driver; no C5 motion-wake API is claimed.
