# M09 / ESP32-S3 / HW2.1.0 BSP

Public API: [bsp.h](bsp/include/bsp.h), [bsp_motion.h](bsp/include/bsp_motion.h).
Pins: [bsp_pins.h](bsp/include/bsp_pins.h). SDK: ESP-IDF 6.1.

Digital I2S mic: BCLK42/DATA2/WS1. Speaker: BCLK40/DATA41/WS39.
Knob GPIO8, RGB46, keys4/3, external input6. LIS3DH bus SDA12/SCL13,
interrupt14. These pins are specific to this board.

bsp::begin() initializes the base board. Optional audio uses 16 kHz, 16-bit mono
PCM through audio_begin/read/write/end; one owner must serialize lifecycle/I/O,
and transfers may be partial. Hardware gain/interaction policy belongs to the
consumer application. Motion is explicitly initialized through MagicBoxMotion.

[Minimal example](../../examples/m09-s3-hw210/minimal) ·
[Hardware diagnostics](../../examples/m09-s3-hw210/hardware-demo) ·
[Integration](../../docs/INTEGRATION.md)
