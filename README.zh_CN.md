# MagicBox

MagicBox 的原生 ESP-IDF 产品固件，直接接入 Magic Server。根目录是唯一产品主工程，
板型按型号、芯片和电路板版本区分，不通过芯片名称猜测接线。

[English](README.md) · [架构](docs/ARCHITECTURE.md) · [新增板型](docs/ADDING_BOARDS.md)

- `main/application.*`、`protocols/`、`audio/`：公共应用、协议和音频服务。
- `main/boards/common/`：公共板级接口。
- `main/boards/m09-s3-hw210/`：M09 / ESP32-S3 / HW2.1.0，适配与 BSP 共置。
- `main/boards/m301-c5-rtc-v1-5g/`：M301 / ESP32-C5 源板配置，物理 PCB 版本待确认。
- `main/boards/c5-reference-m301-hw120/`：旧 C5 HW1.2 参考，不能作为 M301 产品配置。
- `diagnostics/<profile>/baseline` 与 `hardware-demo`：独立最小入口与硬件诊断。
- `partitions/`、`scripts/`、`tools/`、`tests/`、`docs/`：分区、资源、构建、测试与文档。

```sh
python3 tools/build.py --list-boards
source /path/to/esp-idf-v6.1/export.sh
python tools/build.py --board m09-s3-hw210
python tools/build.py --board m301-c5-rtc-v1-5g
python tools/build.py --board m301-c5-rtc-v1-5g --app baseline
python tools/build.py --board m301-c5-rtc-v1-5g --app hardware-demo
```

当前产品使用 IDF 6.1；旧 HW1.2 参考的诊断仍需要 IDF 5.5.3。
`--board` 必填，默认构建产品固件；`--variant` 只能选择该板已有变体。
产物为 `dist/<variant>/magicbox.bin`、合并镜像、ELF、分区表和校验记录。
OTA 型号、变体名、分区与服务地址保持原有兼容边界。

M09 与 M301 的引脚、音频、电源和唤醒方案分别管理；同芯片的其他 PCB 也必须
新增完整 profile。公开版与私有版分别保留各自源码与功能变体，来源和许可证见
[结构迁移记录](docs/LAYOUT_MIGRATION.md)。构建工具不自动刷机或发布。
