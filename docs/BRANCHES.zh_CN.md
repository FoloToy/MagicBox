# 分支与设备服务

简体中文 · [English](BRANCHES.md)

| 分支 | 用途 | 默认服务 | 配网热点 |
|---|---|---|---|
| `main` | BSP、最小入口和独立硬件诊断 | 无 | 无 |
| `demo/magicbox` | MagicBox 产品固件，独立 v0.1 版本体系 | `https://magic.folotoy.cn/xiaozhi/ota/` | `FoloToy-XXXX` |
| `demo/xiaozhi` | 保留固定上游版本功能、连接原服务的 M09 小智移植 | `https://api.tenclass.net/xiaozhi/ota/` | `Xiaozhi-XXXX` |

原产品分支 `demo/xiaozhi` 已改名为 `demo/magicbox`，新建的同名分支用途不同。
MagicBox v0.1 固件下载对应 `demo/magicbox`；既有 Release 固定的源码提交仍是构建依据。

两个应用均支持 M09 / S3 / HW2.1.0。产品分支另新增独立的 [M301 C5 来源板型适配](../apps/xiaozhi/main/boards/folotoy/magicbox-c5/README.zh_CN.md)，实机验收待完成。原服务分支仍仅支持 M09，使用以 `-xiaozhi` 结尾的独立构建与 OTA 标识。旧 C5 HW1.2.0 参考配置不能用来选择这份新应用。

原服务分支保留上游提交 `6cb01531c2ae961eb3fd64c5e97f65d20a597f7e` 的应用、
OTA 激活、WebSocket、MQTT/UDP 和 MCP 功能；本次不升级到最新上游版本。
M09 的音频、按键、旋钮、运动适配继续保留。
屏幕、摄像头、唤醒词和 AEC 能力受实际硬件及配置限制。

构建不会自动烧录。NVS 内已保存的 `wifi/ota_url` 优先于编译默认值；切换服务时，
应明确检查旧地址覆盖并配到目标服务。两套服务的账号、激活码和绑定相互独立，
切换 Git 分支不会迁移设备数据。保留产品身份和已有分区布局。
新原服务版本的激活与对话仍需真机验收，不能仅凭编译通过宣称完成。
