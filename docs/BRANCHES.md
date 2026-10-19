# Branches and device services

English · [简体中文](BRANCHES.zh_CN.md)

| Branch | Purpose | Default M09 service | Provisioning AP |
|---|---|---|---|
| `main` | BSPs, minimal entry points, separate hardware diagnostics | None | None |
| `demo/magicbox` | MagicBox product firmware, independent v0.1 versioning | `https://magic.folotoy.cn/xiaozhi/ota/` | `FoloToy-XXXX` |
| `demo/xiaozhi` | M09 port retaining the pinned upstream XiaoZhi application and original service | `https://api.tenclass.net/xiaozhi/ota/` | `Xiaozhi-XXXX` |

The former product branch `demo/xiaozhi` was renamed to `demo/magicbox`.
The new `demo/xiaozhi` has a different purpose. MagicBox v0.1 release downloads
belong to `demo/magicbox`; immutable release source commits remain authoritative.

Both applications use the M09 S3/HW2.1.0 hardware profile. The original-service
variant has its own build name and OTA board type ending in `-xiaozhi`, so its
firmware identity cannot be confused with the Magic Server variant. BSP support
for C5 does not imply an application port for C5.

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
