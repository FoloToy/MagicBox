# Profiles and ownership

A profile identifies model + silicon + PCB revision, for example `m09-s3-hw210`. The registry never chooses hardware from the detected chip alone. Missing/unknown profiles and wrong chip selections fail before a build. Add another S3 PCB as a new profile with its own pins, capabilities, provenance and acceptance evidence. No wildcard/default board is allowed.

`boards/<profile>/esp-idf` is an isolated BSP project with its own SDK/dependencies and minimal default main. Diagnostics live under that project's `examples/hardware-demo` and are built separately. M09 uses IDF 6.1; the new M301 source profile uses IDF 6.1; the historical C5 HW1.2.0 reference retains IDF 5.5.3. SDK constraints are intentionally per profile, not silently widened. The owner confirms C5 model M301; the physical PCB revision is still pending.

The original Arduino implementation is maintained as history in the archived [FoloToy/magicbox-arduino repository](https://github.com/FoloToy/magicbox-arduino); no legacy source tree is included here. The current M09 BSP and XiaoZhi port use native ESP-IDF 6.1.

`main` contains BSPs, minimal entry points and separate diagnostics, without a product/network application. `demo/magicbox` adds the Magic Server product firmware; `demo/xiaozhi` adds the pinned upstream original-service port. Both retain the upstream source under `apps/xiaozhi`; BSP fixes can be merged from main separately into each demo. Their M09 applications own I2S and motion buses; do not initialize a second BSP bus/audio owner. Only explicitly supported hardware profiles may build an application. C5 BSP support does not establish C5 application support. See [branch roles](BRANCHES.md).

C5 reference hardware source: FoloToy/magicbox-c5 M301 HW1.2.0. It uses ES8311, AXP2101, BM8563 and GX8002, distinct from M09's direct digital I2S microphone/speaker and LIS3DH. Do not copy S3 wake masks to C5. The C5 source mentions an accelerometer interrupt but no initialized driver; no C5 motion-wake API is claimed.

The cited folotoy-firmware-idf commit `8c013c923b59f1123e762880100d62ccf7005907` uses the same C5 reference pin map. [C5 source reconciliation and production partition notes](../boards/m301-c5-rtc-v1-5g/README.md). GX8002 uses ALDO4/BLDO1 on this profile; generic driver rail aliases belong to another PCB.

`m301-c5-rtc-v1-5g` identifies the cited source board and owner-confirmed M301 model, with an unknown physical PCB revision. It owns independent pins and ALDO4/BLDO1 wake rails. The older `c5-reference-m301-hw120` retains its original ALDO1/ALDO3 aliases. Matching GPIOs do not establish interchangeable power wiring.
