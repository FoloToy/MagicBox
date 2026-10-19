# MagicBox development

Read `boards/manifest.json` and `docs/ARCHITECTURE.md`. Start with git status and preserve local changes. Explicitly select a complete hardware profile; a chip type alone is not a board identifier. Never reuse M09 pins for another S3 PCB. Each profile owns its canonical `bsp_pins.h`; framework adapters reference it.

`main` contains BSPs, minimal entry points and opt-in hardware diagnostics. Product/network applications belong on demo branches. Never put cloud credentials, device identity, OTA policy or account state in a BSP. Reuse bus owners; callbacks/ISRs only queue work. PMU rail sequencing and codec directions are board-specific.

Run `python3 tools/check_repo.py` and `python3 -m unittest discover -s tests`. Build the affected baseline and diagnostic with the SDK listed in the manifest. Test results must distinguish build, host, inherited device evidence and newly verified device evidence. Do not claim C5 hardware acceptance from compilation. No automatic flash: require authorization for exact profile, image and partition impact; preserve production identity and factory images.

On `demo/magicbox` or `demo/xiaozhi`, read `apps/xiaozhi/AGENTS.md` before modifying the application. Explicitly select an independently named board/OTA variant. Keep raw logs and backups outside this repository.
