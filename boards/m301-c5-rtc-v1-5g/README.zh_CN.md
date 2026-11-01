# M301 / ESP32-C5 源板 BSP

公开头文件位于 [bsp/include](bsp/include)，引脚定义见
[bsp_pins.h](bsp/include/bsp_pins.h)。详细硬件、电源轨与来源记录见
[硬件说明](HARDWARE.zh_CN.md)。本 profile 的物理 PCB 版本仍待确认。

最小示例按顺序初始化 I2C、电源与 LED；可通过 bsp_audio_*、bsp_rtc_*、
bsp_wakeup_* 等接口实现自己的应用。旧 HW1.2 参考使用不同的电源轨别名，不可混用。
SDK 为 ESP-IDF 6.1。[English](README.md)
