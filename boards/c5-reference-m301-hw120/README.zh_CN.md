# MagicBox C5 参考 BSP

[English](README.md) · [硬件注册表](../manifest.json)

配置名沿用原始来源的 **M301 / HW1.2.0**。用户已确认当前设备型号为 **M301**；实物 PCB 版本仍待核对，不归为 M09。

## 来源与硬件

已核对 FoloToy/folotoy-firmware-idf 的 `8c013c923b59f1123e762880100d62ccf7005907` 提交，其中 `magicbox_rtc_v1_5G` 系列与本参考配置的引脚、电源类型标志、ES8311 时钟来源及 24 kHz 音频格式一致。最初导入来源为 [FoloToy/magicbox-c5](https://github.com/FoloToy/magicbox-c5/tree/f889d36f721dce7c503cdacf67f18de12a082ca2)。指定提交包含多种共用引脚的应用版本，其业务行为不进入 BSP。

| 功能 | 配置 |
|---|---|
| I2C | SDA 1、SCL 0；AXP2101 0x34、ES8311 0x18、BM8563 0x51、GX8002 0x2F |
| I2S | MCLK 5、BCLK 25、WS 23、MCU 输出 4、MCU 输入 24 |
| 功放 / RGB | GPIO26 / GPIO27 |
| Mode / Voice | GPIO3 / GPIO2，低电平有效 |
| 旋钮 | GPIO6，ADC1 通道 5 |
| 中断 | 电源 10、RTC 8、GX8002 7；原配置另定义 IMU 9 |

引脚唯一来源为 `esp-idf/components/bsp/include/bsp_pins.h`。GX8002 电源按指定源码使用 **ALDO4 3.3 V 先上电，延迟 10 ms 后 BLDO1 1.0 V 上电**，下电顺序相反。通用驱动的 VddIO/VddCore 方法对应其他电源轨，本板不能使用。本 BSP 未初始化加速度传感器，仅有中断定义不能证明支持晃动唤醒。RTC 与 GX8002 API 可由应用显式调用。

## 构建

使用 ESP-IDF **6.1**，已从早期 5.5.3 参考导入版本迁移，与产品 demo 使用同一 SDK；历史构建记录保留在验证文档中。

```sh
source /path/to/esp-idf-v6.1/export.sh
python tools/build.py --board c5-reference-m301-hw120 --app baseline
python tools/build.py --board c5-reference-m301-hw120 --app hardware-demo
```

在仓库根目录执行。默认入口保持最小化；独立诊断示例覆盖按键、ADC、音频、电源、RTC 与唤醒芯片初始化。构建命令不会刷机。产品应用放在 `demo/magicbox`。

## 实机证据与分区

2026-10-20 只读识别结果：**ESP32-C5NR4 rev1.0、16 MB Flash、4 MB 内置 PSRAM**。设备分区表与指定源码一致：factory `0x210000`、app0 `0x610000`、app1 `0xA10000`，各 4 MB；产品 NVS 位于 `0xF20000`。此布局与 M09 S3 不同。本轮没有写入固件或设置；芯片与分区核对不能代替 PCB、外设、音频和电源的实机验收。

独立 BSP 的分区是开发布局，不能把其合并镜像或分区表覆盖到量产设备。后续更新必须明确硬件配置、应用镜像和实际启动分区，先做私有备份，并保留原厂固件、文件系统、身份和已保存设置。
