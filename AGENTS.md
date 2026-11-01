# MagicBox public BSP development

Start with git status; preserve local work. This repository is a reusable BSP
library for secondary development, not a product or service client. Read
boards/manifest.json, docs/ARCHITECTURE.md and docs/INTEGRATION.md. Product
applications belong in their own repositories; do not add account, cloud,
activation, OTA or credential policy to these drivers/examples.

Select complete model/chip/PCB profiles explicitly. Keep each canonical BSP
under boards/<profile>/bsp and examples under examples/<profile>. Each board owns
its pin authority and SDK contract. M09 and M301 use IDF 6.1; legacy C5 HW1.2.0
uses 5.5.3. Do not infer unknown PCB revisions or share PMU rails by chip alone.

Run python tools/check_repo.py and python -m unittest discover -s tests -v.
Build affected minimal and hardware-demo examples via tools/build.py --board
<profile> --app <example>. The default is minimal; baseline is only an alias.
Reuse one I2C/I2S owner; callbacks/ISRs queue work. Keep minimal bring-up small,
without optional audio tasks, networking or application services by default.

Distinguish build, host, inherited and fresh device evidence. No automatic
flash: require exact-profile/image/range authorization and a restorable backup.
Raw logs, backups, real credentials and device identities remain outside Git.
