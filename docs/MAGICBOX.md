# MagicBox firmware guide

The root is the dedicated product IDF project. Source provenance is retained
in [the migration record](LAYOUT_MIGRATION.md) and docs/upstream/UPSTREAM.md.

```sh
source /path/to/esp-idf-v6.1/export.sh
python tools/build.py --list-boards
python tools/build.py --board m09-s3-hw210
python tools/build.py --board m301-c5-rtc-v1-5g
```

M09 and M301 keep independent OTA types, variants and partition layouts.
Artifacts are under dist/<variant>/ with magicbox.bin, ELF, merged image,
partition table, flash arguments and checksummed manifest. The build never
opens a serial port, flashes or publishes. The compatibility build_xiaozhi.sh
command selects this same root product.

The explicitly named M09 -local variant retains development network/server
settings. Use --variant folotoy-magicbox-m09-s3-hw210-local to request it;
standard firmware does not seed demo credentials. Internal experimental
variants are explicitly named in that repository's board config.json and do
not change the standard variant's AEC/interruption defaults.

[M09 implementation and controls](../main/boards/m09-s3-hw210/README.md) ·
[M301 implementation and controls](../main/boards/m301-c5-rtc-v1-5g/README.md) ·
[Hardware evidence](VALIDATION.md) · [Architecture](ARCHITECTURE.md)

Published releases remain historical artifacts with their original filenames.
A new build is not a replacement release or a physical hardware test. Any
future flash must match the exact board, security/partition state and owner
authorization, preserving restorable backups and product identity.
