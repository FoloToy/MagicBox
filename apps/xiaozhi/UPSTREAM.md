# Vendored upstream

- Source: https://github.com/78/xiaozhi-esp32
- Revision: 6cb01531c2ae961eb3fd64c5e97f65d20a597f7e
- Version: 2.5.3
- License: MIT, preserved in LICENSE.

Tracked upstream sources are included without their Git history. Local adaptation consists of `main/boards/folotoy/magicbox/`, `partitions/folotoy-magicbox.csv`, board selection additions in `main/Kconfig.projbuild` and `main/CMakeLists.txt`, `scripts/tests/test_folotoy_magicbox.py`, and this provenance file. Generated dependencies, sdkconfig, binaries and releases are excluded by the upstream ignore rules.

This repository imports the tested FoloToy port from `FoloToy/magicbox-arduino` revision `8bab8841109f7c891b70f4705a61473767495056`. The selected hardware is now explicitly M09 / ESP32-S3 / HW2.1.0, with an independent board/build/OTA identity. The pin authority and LIS3DH primitive are shared from `boards/m09-s3-hw210/esp-idf/components/bsp/include`; no Arduino driver is linked into the XiaoZhi application.
