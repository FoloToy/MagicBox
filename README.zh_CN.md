# MagicBox M09 小智原服务移植

简体中文 · [English](README.md)

当前为 **`demo/xiaozhi`**：保留小智原有应用功能、连接原小智服务的 M09 硬件移植。
使用上游固定提交 `6cb01531c2ae961eb3fd64c5e97f65d20a597f7e` 的 **2.5.3** 版本，
保留 OTA 激活、WebSocket、MQTT/UDP、MCP 工具和会话模式。

| 分支 | 用途 |
|---|---|
| [`main`](https://github.com/FoloToy/MagicBox/tree/main) | 多代硬件 BSP、最小示例和硬件诊断 |
| [`demo/magicbox`](https://github.com/FoloToy/MagicBox/tree/demo/magicbox) | 连接 Magic Server 的 MagicBox v0.1 产品固件 |
| [`demo/xiaozhi`](https://github.com/FoloToy/MagicBox/tree/demo/xiaozhi) | 当前小智原服务移植 |

## 硬件与服务

- 硬件：**M09 / ESP32-S3 / HW2.1.0**，配置 `m09-s3-hw210`。
- 默认 OTA/激活服务：`https://api.tenclass.net/xiaozhi/ota/`。
- 配网热点：`Xiaozhi-XXXX`，配置页面 `http://192.168.4.1`。
- 激活后使用原 [xiaozhi.me 控制台](https://xiaozhi.me)管理。
- 独立构建名称：`folotoy-magicbox-m09-s3-hw210-xiaozhi`。
- 独立 OTA 机型：`magicbox-m09-s3-hw210-xiaozhi`。

M09 的音频、按键、外接 Mode、旋钮和运动适配继续保留。
上游可选功能仍保留在源码中；M09 没有屏幕、摄像头，本构建不启用唤醒词或 AEC。
C5 的 BSP 单独保留，不代表已完成 C5 应用移植。

## 构建

```sh
source /path/to/esp-idf-v6.1/export.sh
./tools/build_xiaozhi.sh --board m09-s3-hw210
```

本版本不注入 Magic Server 地址或演示 Wi-Fi。已保存的 `wifi/ota_url` 会覆盖编译默认地址，
切换服务前应检查该项。构建不会刷机，也不会迁移两套服务的账号与设备绑定。

[服务与构建说明](docs/XIAOZHI.zh_CN.md) · [分支说明](docs/BRANCHES.zh_CN.md) ·
[验证记录](docs/VALIDATION.md) · [上游文档](apps/xiaozhi/README_zh.md)

保留 MIT 许可证及上游署名。本次保留固定上游版本，不宣称升级到最新版本，
原服务的激活和对话仍需新版本真机验收。
