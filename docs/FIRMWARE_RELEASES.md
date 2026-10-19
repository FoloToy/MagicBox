# Firmware builds for the original XiaoZhi service

English · [简体中文](FIRMWARE_RELEASES.zh_CN.md)

This `demo/xiaozhi` branch builds the original-service M09 variant
`folotoy-magicbox-m09-s3-hw210-xiaozhi` (upstream application version 2.5.3).
See the [build guide](XIAOZHI.md). No binary release for this new variant is
published by this branch split.

[MagicBox v0.1](https://github.com/FoloToy/MagicBox/releases/tag/v0.1) is the
separate Magic Server firmware from `demo/magicbox`. Use its
[branch-specific instructions](https://github.com/FoloToy/MagicBox/blob/demo/magicbox/docs/FIRMWARE_RELEASES.md).
An application build does not authorize overwriting device partitions, factory
firmware or product identity; verify actual hardware/layout and back up privately
before any separately authorized flash.
