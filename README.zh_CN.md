# MagicBox BSP

供二次开发使用的原生 ESP-IDF 板级支持库：提供驱动、引脚定义、最小示例和硬件诊断。
用户在 BSP 上实现自己的产品应用与服务对接。

[English](README.md) · [接入指南](docs/INTEGRATION.md) · [架构](docs/ARCHITECTURE.md)

- `boards/<profile>/bsp/`：每块电路板的独立驱动与公开头文件。
- `boards/manifest.json`：型号、芯片、硬件版本、SDK 与示例路径登记。
- `examples/<profile>/minimal/`：最小硬件初始化。
- `examples/<profile>/hardware-demo/`：音频、按键、灯、旋钮等硬件诊断。
- `tools/`、`tests/`、`docs/`：构建、边界检查、测试和二次开发说明。

| 板型 | 芯片 | SDK |
| --- | --- | --- |
| M09 / HW2.1.0 | ESP32-S3 | IDF 6.1 |
| M301 / RTC-v1-5G 源板配置 | ESP32-C5 | IDF 6.1 |
| 旧 C5 / HW1.2 参考 | ESP32-C5 | IDF 5.5.3 |

M301 的物理 PCB 版本仍待确认；旧 HW1.2 的电源轨配置单独保留。不能仅凭芯片相同混用板型。

```sh
python3 tools/build.py --list-boards
source /path/to/esp-idf-v6.1/export.sh
python tools/build.py --board m09-s3-hw210
python tools/build.py --board m09-s3-hw210 --app hardware-demo
python tools/build.py --board m301-c5-rtc-v1-5g --app minimal
```

默认构建最小示例，`--app baseline` 兼容映射到 `minimal`。产物放在
`dist/<profile>/<example>/`，构建工具不自动刷机。旧参考需切换至对应的 5.5.3 SDK。

在自己的 IDF 项目中，将选定的 `boards/<profile>/bsp` 加入
`EXTRA_COMPONENT_DIRS`，再通过 `REQUIRES bsp` 引用。具体步骤见
[接入指南](docs/INTEGRATION.md)。网络、协议、账号、OTA 与产品交互由用户应用负责；
BSP 仅负责硬件。各 BSP 只有一个引脚权威头文件，并复用既有 I2C/I2S 总线所有者。

示例分区用于开发；安装到已有产品前需核对实际分区和备份。历史应用分支与发布保留为历史资料，
当前 BSP 开发以这里的组件和示例为入口。
