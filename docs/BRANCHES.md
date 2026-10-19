# Branches and device services

English · [简体中文](BRANCHES.zh_CN.md)

| Branch | Purpose | Default service | Provisioning AP |
|---|---|---|---|
| `main` | BSPs, minimal entry points, separate hardware diagnostics | None | None |
| `demo/magicbox` | MagicBox product firmware, independent v0.1 versioning | `https://magic.folotoy.cn/xiaozhi/ota/` | `FoloToy-XXXX` |
| `demo/xiaozhi` | M09 port retaining the pinned upstream XiaoZhi application and original service | `https://api.tenclass.net/xiaozhi/ota/` | `Xiaozhi-XXXX` |

The former product branch `demo/xiaozhi` was renamed to `demo/magicbox`.
The new `demo/xiaozhi` has a different purpose. MagicBox v0.1 release downloads
belong to `demo/magicbox`; immutable release source commits remain authoritative.

Both applications support M09 S3/HW2.1.0. The product branch additionally contains the independent [M301 C5 source-profile port](../apps/xiaozhi/main/boards/folotoy/magicbox-c5/README.md), with physical acceptance pending. The original-service branch remains M09-only and uses a build/OTA identity ending in `-xiaozhi`. The older C5 HW1.2.0 reference does not select the new M301 application.

The original-service demo retains the upstream application, OTA activation,
WebSocket/MQTT-UDP transports and MCP functionality from upstream commit
`6cb01531c2ae961eb3fd64c5e97f65d20a597f7e`. It does not claim an upgrade to the
latest upstream revision. M09-specific buttons, audio, volume and motion
adaptations remain necessary. Display, camera, wake-word and AEC capabilities
are limited by this board's actual hardware/configuration.

Build tools never flash hardware. Existing NVS `wifi/ota_url` overrides the
compiled default. When moving a device between services, explicitly review that
saved override and provision the intended service. Accounts, activation codes
and bindings belong to their respective services and are not migrated by a
branch switch. Preserve product identity and existing flash layout. Original
service activation and conversation on this new variant still require physical
acceptance; compilation alone does not establish service acceptance.
