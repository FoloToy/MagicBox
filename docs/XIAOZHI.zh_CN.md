# M09 小智原服务移植

简体中文 · [English](XIAOZHI.md)

`demo/xiaozhi` 使用固定的小智上游应用版本 2.5.3，默认连接
`https://api.tenclass.net/xiaozhi/ota/`，使用[原控制台](https://xiaozhi.me)。
连接 Magic Server 的产品应用在 `demo/magicbox`。

```sh
source /path/to/esp-idf-v6.1/export.sh
./tools/build_xiaozhi.sh --board m09-s3-hw210 \
  --variant folotoy-magicbox-m09-s3-hw210-xiaozhi
python3 tools/check_repo.py
python -m unittest discover -s tests -v
(cd apps/xiaozhi && python -m unittest discover -s scripts/tests -v)
```

构建明确选择 M09 / S3 / HW2.1.0、共享 BSP 引脚和 M09 量产布局分区表。
独立 OTA 机型为 `magicbox-m09-s3-hw210-xiaozhi`。
原配网行为使用 `Xiaozhi-XXXX`，不注入 Magic Server 地址或演示 Wi-Fi。
应用、OTA、协议、MCP 和公共 Wi-Fi 代码通过固定上游文件哈希核对。

硬件适配层独占 I2S 和运动总线。Voice 按住说话，Mode 和 GPIO6 外接 Mode 切换会话，
长按 Mode 配网；保留旋钮音量、麦克风转换增益和 LIS3DH 运动唤醒。
本硬件不宣称屏幕、摄像头、唤醒词或 AEC 验收。

已保存的 NVS `wifi/ota_url` 优先于编译默认值。跨服务使用需要检查该项，
并在目标服务分别激活、绑定。不要清除产品身份，或覆盖原厂应用、分区及 OTA 选择数据。
构建不会刷机，本次不改变已连接设备。原服务有自己的 OTA 策略，接受更新前需核对返回机型。

新版本还需真机完成原服务的激活与对话验收；以前 Magic Server 的验证不等于此项已完成。
详见[分支职责](BRANCHES.zh_CN.md)和[验证记录](VALIDATION.md)。
