# MagicBox development

Start with git status and preserve local work. Read main/boards/manifest.json,
docs/ARCHITECTURE.md, docs/ADDING_BOARDS.md and main/AGENTS.md. The root is the
native IDF product project. Select complete model/chip/PCB profiles explicitly.
Each main/boards/<profile>/bsp owns its canonical pins; diagnostics reference
that same BSP. Never reuse M09 pins/power sequences for another S3 PCB or the
historical C5 rails for M301. Product logic/credentials do not belong in BSPs.

Run python3 tools/check_repo.py, root tests and scripts/tests. Build the affected
products and baseline/diagnostics with the SDK declared per profile. Product
builds use tools/build.py --board <profile>; diagnostics require --app. IDF 6.1
is used for products, while the legacy reference retains 5.5.3. M09 product
owns its buses; do not initialize a second BSP audio/bus owner.

Keep runtime sources from this repository's own baseline. Preserve independent
variants, board/OTA identities, partition layouts and source/license notices.
Do not transfer private implementation into the public repository. Distinguish
build, host, inherited device evidence and new device acceptance. Raw logs,
backups, real credentials and identities remain outside Git. No automatic
flash: exact profile, image/range authorization and restorable backup required.
