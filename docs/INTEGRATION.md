# Use the BSP in your ESP-IDF project

1. Select the complete hardware profile from boards/manifest.json and activate
   its SDK. Match the actual board, not just ESP32-S3 or ESP32-C5.
2. Keep this BSP repository as a dependency of your application, for example
   under `third_party/MagicBox`. Add exactly one component path before the IDF
   project include in your own root CMakeLists.txt:

```cmake
cmake_minimum_required(VERSION 3.16)
set(EXTRA_COMPONENT_DIRS
    "${CMAKE_CURRENT_LIST_DIR}/third_party/MagicBox/boards/m09-s3-hw210/bsp")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(my_product)
```

3. Declare the component dependency in your application's main/CMakeLists.txt:

```cmake
idf_component_register(SRCS "main.cpp" INCLUDE_DIRS "." REQUIRES bsp)
```

4. Use the selected profile's minimal example as the initialization reference.
   For M09, `bsp::begin()` prepares the board; audio initialization is explicitly
   opt-in through `bsp::audio_begin()`. M301 has its own C5 interfaces such as
   bsp_i2c_init(), bsp_power_init(), bsp_led_init() and bsp_audio_*.
   Their peripheral APIs are not assumed interchangeable.
5. Review the example's sdkconfig.defaults for the matching hardware, then
   define your own application's memory, partition and feature configuration.
   Example partition files are not a production migration plan.
6. Add your application tasks, interaction, networking and service protocol.
   The BSP does not prescribe them. Keep credentials and product identity in
   your application's provisioning/storage policy, never the public drivers.

## Ownership and events

Use the BSP's initialized I2C bus and audio lifecycle. M09 optional audio owns
both I2S controllers at 16 kHz mono PCM; returned counts may be partial. Apply
application-specific gain/volume policy in your application. C5 uses its codec
and PMU contracts; read its header and source-board notes before controlling
rails or wake sources. Avoid blocking work in callbacks and interrupt handlers.

## Validate hardware independently

```sh
python tools/build.py --board m09-s3-hw210 --app minimal
python tools/build.py --board m09-s3-hw210 --app hardware-demo
```

These commands build only. Build/host tests, inherited hardware evidence and
fresh device acceptance are separate. Check existing device partitions and a
restorable backup before installing a diagnostic on a production device.
