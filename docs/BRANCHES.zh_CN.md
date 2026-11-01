# 分支与服务

重构后的主产品工程位于仓库根目录，直接接入 Magic Server。公开版的固件基线
来自原 demo/magicbox；私有版原本已在 main 包含产品固件。两个仓库分别保留
自己的代码基线，见 [迁移记录](LAYOUT_MIGRATION.md)。

历史 demo/magicbox 和 demo/xiaozhi 分支不改写；后者保留原小智服务的历史移植。
当前产品按完整硬件 profile 构建，不再依赖切换分支来选择芯片。M301 的物理
PCB 版本仍待确认，旧 HW1.2 参考与其电源配置分别保留。

[English](BRANCHES.md) · [构建入口](../README.zh_CN.md)
