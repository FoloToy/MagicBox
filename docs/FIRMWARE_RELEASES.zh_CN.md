# 固件下载

简体中文 · [English](FIRMWARE_RELEASES.md)

从 [GitHub Releases](https://github.com/FoloToy/MagicBox/releases) 下载固件。
选择时核对完整的型号、芯片和 PCB 版本。

## MagicBox v0.1 · M09

| 项目 | 配置 |
|---|---|
| 硬件配置 | `m09-s3-hw210` |
| 适配硬件 | MagicBox M09 · ESP32-S3 · PCB HW2.1.0 · 16 MB Flash |
| 源码分支 | `demo/magicbox` |
| 构建版本 | `folotoy-magicbox-m09-s3-hw210` |
| SDK | ESP-IDF 6.1 |
| 默认服务器 | `https://magic.folotoy.cn/xiaozhi/ota/` |
| 配网 | 热点 `FoloToy-XXXX`，浏览器打开 `http://192.168.4.1` |

附件包括应用固件 `.bin`、ZIP 压缩包、中英文说明、源码与构建清单、SHA-256 校验文件。
对外发布版本为 `v0.1`，ESP-IDF 应用版本为 `0.1.0`，采用 MagicBox 自己的版本体系。
当前固件兼容小智协议；协议名称表示兼容能力，不作为产品名或版本名。
包内不包含已保存的 Wi-Fi、账号密钥、设备身份、Flash 备份或原厂固件。
标准云端版本不自动填入开发测试用 Wi-Fi。

### 已有设备升级

应用小于 4 MB。此前真机验证的 M09 量产分区中，`app0` 位于
`0x410000`–`0x80ffff`，且已被选为启动槽。串口升级前，先独立确认硬件、
实际分区表、当前启动槽、固件大小和 SHA-256，并私下备份应用、配置及身份数据。
只有上述条件全部匹配时，才可按以下方式更新已确认的 app0：

```sh
python -m esptool --chip esp32s3 --port PORT write-flash 0x410000 magicbox-m09-s3-hw210-v0.1.bin
```

命令假定所用 esptool 支持连字符形式的子命令，且不会替你选择 OTA 启动槽。
实际启动槽或分区布局不同，应先取得对应升级方案。
不要全片擦除或覆盖 bootloader、分区表、NVS、原厂应用、OTA 选择数据及产品身份。
应用固件不是空白芯片的全量出厂包。创建 Release 不会自动烧录设备或启用自动 OTA。

### 配网、激活与验证

没有已保存网络时，用手机连接 `FoloToy-XXXX`，打开 `http://192.168.4.1` 配置 Wi-Fi。
到 [Magic Console](https://magic.folotoy.cn) 注册或登录，使用激活码绑定设备。
仅更新应用会保留已有 Wi-Fi 和绑定；长按 Mode 三秒进入重新配网。
Voice 用于按住说话，Mode 切换对话模式。

此源码版本已有 M09 配网、激活、语音、按键、音量及 Magic Server 对话的真机验证记录。
每个发布清单会另行注明该份二进制是否曾烧录，新构建不等于新增真机验收。
M301 C5 使用下方独立发布；其他 S3 PCB 也需独立配置。
详见[验证记录](VALIDATION.md)和[构建说明](MAGICBOX.md)。

## MagicBox v0.1 · M301

[下载 M301 预发布版](https://github.com/FoloToy/MagicBox/releases/tag/v0.1-m301)。
发布标签 `v0.1-m301` 用型号区分已有的 M09 `v0.1`，两者内部应用版本均为 `0.1.0`。

| 项目 | 配置 |
| --- | --- |
| 硬件配置 | `m301-c5-rtc-v1-5g` |
| 硬件 | MagicBox M301 · ESP32-C5 · 16 MB Flash · 4 MB Quad PSRAM |
| 来源板型 | `magicbox_rtc_v1_5G`；实物 PCB 版本待确认 |
| 源码分支 | `demo/magicbox` |
| 固件源码 | `4f0d49c4e0cb2a3a3cbb5652fc8cf75f32545c02` |
| SDK | ESP-IDF 6.1 |
| 应用 BIN | `magicbox-m301-c5-rtc-v1-5g-v0.1.bin` |
| 默认服务器 | `https://magic.folotoy.cn/xiaozhi/ota/` |
| 配网 | 热点 `FoloToy-XXXX`，打开 `http://192.168.4.1` |
| SHA-256 | `19109e0b87ddedc8ebf7c74cb904c0fbe37b7fb7d410e3b2fa914d64eb9533b3` |

该 BIN 与此前已烧录至测试 M301 的 1,849,104 字节应用完全一致。日志确认启动、
PSRAM、AXP2101 和 ES8311 初始化，用户确认可以进行云端对话。本次发布沿用相同
二进制，不重新构建。ZIP 包含应用、中英文说明、清单、校验文件及 MIT 声明；
不含 bootloader、分区表、原厂镜像、已保存网络、产品身份或设备备份。

此前检查的 M301 分区中，app0 位于 `0x610000`，容量 4 MB，与 M09 不同。
升级前须独立核对完整硬件配置、实际分区与当前启动槽，并私下备份设备。
仅当实际设备的 app0 布局和启动选择均匹配时使用：

```sh
python -m esptool --chip esp32c5 --port PORT write-flash 0x610000 magicbox-m301-c5-rtc-v1-5g-v0.1.bin
```

写入应用不会自动选择启动槽。保留原厂应用、产品身份、NVS、分区表和 OTA 选择；
空白芯片需要单独的安装方案。创建本次 Release 不会烧录设备或启用自动 OTA。

Voice 按住说话，Mode 切换会话、长按一秒配网，旋钮控制音量。M301 未启用 M09
的晃动唤醒、自动休眠、唤醒词或 AEC。RTC/GX8002、电池行为、Mode/旋钮全范围、
从零激活及重连仍需独立验收。旧 `c5-reference-m301-hw120` 配置不能与此来源板型
的供电配置混用。
