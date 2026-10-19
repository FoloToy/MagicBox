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
C5 的销售型号尚未确认，小智移植尚未发布；其他 S3 PCB 也需独立配置。
详见[验证记录](VALIDATION.md)和[构建说明](MAGICBOX.md)。
