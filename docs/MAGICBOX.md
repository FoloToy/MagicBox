# MagicBox M09 firmware / MagicBox 固件

This branch inherits all BSP profiles from main and adds the previously tested M09 port of XiaoZhi 2.5.3. Original source: FoloToy/magicbox-arduino at `8bab8841109f7c891b70f4705a61473767495056`; upstream 78/xiaozhi-esp32 at `6cb01531c2ae961eb3fd64c5e97f65d20a597f7e`. MIT notices remain in `apps/xiaozhi`.

当前小智仅支持 **M09 / ESP32-S3 / HW2.1.0**。原始 M09 固件为 Arduino，本应用是原生 ESP-IDF 6.1。C5 在 main 的 BSP 可独立构建；C5 小智尚未移植，命令会明确拒绝，不能拿 S3 固件刷 C5。另一版 S3 同样需单独配置。

```sh
source /path/to/esp-idf-v6.1/export.sh
./tools/build_xiaozhi.sh --board m09-s3-hw210
# Host tests:
cd apps/xiaozhi
python -m unittest discover -s scripts/tests -v
```

The canonical upstream builder selects `folotoy/magicbox` with variant `folotoy-magicbox-m09-s3-hw210`. Board/OTA type is `magicbox-m09-s3-hw210`, independent of silicon and other revisions. The wrapper requires an explicit profile and builds from an owned whitespace-free cache when needed. It shares the canonical M09 pins and motion primitive from main, without initializing a second BSP audio/I2C owner.

功能保留：Voice 按住录音、松开发送；Mode 和 GPIO6 外接 Mode 控制会话，长按三秒配网；旋钮控制音量；麦克风 8 倍转换增益；LIS3DH 晃动唤醒；闲置 60 秒启用保留 Wi-Fi 关联的自动浅睡眠。配网、录音、播放和未关闭的会话不进入睡眠。没有唤醒词、显示屏、摄像头或 AEC。

**Build does not flash.** New standalone BSP/SDK default layouts are not production update layouts. For the previously tested M09 unit only, the retained production layout has app0 at `0x410000`, a 4MB slot, factory plus two OTA slots and dedicated product identity. Confirm exact chip, hardware, current partition table, image size/SHA and selected boot slot; privately back up current app/settings/identity before any explicitly authorized write. Never flash a merged/default full image over a production device. The original repository migration did not change installed firmware; see the dated cloud update in `docs/VALIDATION.md` for the later authorized device update.

Earlier M09 acceptance covers audio/buttons/volume/Wi-Fi/online dialogue and user-observed shake-wake dialogue. The initial migration did not flash the newly named variant; the later cloud update is recorded separately in `docs/VALIDATION.md`. Prior serial collection did not capture the motion-wake instant, actual sleep counters, RTC continuity or wake latency. Current draw and C5 runtime are unverified. See `docs/VALIDATION.md`.

## Cloud service and provisioning / 云端服务与配网

The standard M09 variant defaults to `https://magic.folotoy.cn/xiaozhi/ota/`. With no saved Wi-Fi, it starts the `FoloToy-XXXX` provisioning hotspot; open `http://192.168.4.1` from a phone connected to that hotspot. After connecting to Wi-Fi, register or sign in at [Magic Server](https://magic.folotoy.cn) and bind the device with its activation code. Hold Mode for three seconds to reconfigure Wi-Fi.

M09 标准版本默认连接云端 Magic Server。没有已保存网络时直接开启 `FoloToy-XXXX` 配网热点；手机连接热点后访问 `http://192.168.4.1` 配置 Wi-Fi，再到 [Magic Server](https://magic.folotoy.cn) 注册或登录，使用设备激活码绑定。长按 Mode 三秒可重新配网。

Only the separate `-local` variant enables `CONFIG_FOLOTOY_MAGICBOX_DEMO_WIFI` and seeds `FOLOTOY_LAND` (password: `makeitanything`) when no Wi-Fi is saved. Existing networks take precedence. The standard cloud variant does not seed these credentials.

仅独立的 `-local` 版本启用演示 Wi-Fi：SSID 为 `FOLOTOY_LAND`，密码为 `makeitanything`，且仅在没有已保存网络时填入。标准云端版本不自动填入演示网络。

## Local MagicBox-Server test / 本地服务联调

Use the separately named M09 local variant for the current test machine:

```sh
source /path/to/esp-idf-v6.1/export.sh
./tools/build_xiaozhi.sh --board m09-s3-hw210 --variant folotoy-magicbox-m09-s3-hw210-local
```

This variant uses `http://192.168.1.172:8988/xiaozhi/ota/`; the server supplies
`ws://192.168.1.172:8988/xiaozhi/v1/` after account enrollment. The computer and
device must be on a mutually reachable LAN. This is an HTTP/WS development
endpoint; use TLS for deployment beyond the trusted test LAN. A changed computer
IP requires updating the variant and the server's advertised URL together.
The standard `folotoy-magicbox-m09-s3-hw210` variant uses the HTTPS cloud endpoint above.
A saved provisioning `wifi/ota_url` takes precedence over the build default.

The local variant retains Voice/Mode/external Mode, knob volume, microphone gain,
and motion wake. It changes the service endpoint without clearing saved Wi-Fi
or product identity. Build metadata records the selected variant and image SHA.
For the previously tested unit, update the verified running app0 slot only;
never use `merged-binary.bin` for this operation. Device backups and raw logs
belong outside the repository.
