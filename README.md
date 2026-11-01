# MagicBox

Native ESP-IDF MagicBox firmware for Magic Server. The repository root is the
product project; hardware is selected by **model + chip + board revision**.
[简体中文](README.zh_CN.md) · [Architecture](docs/ARCHITECTURE.md) ·
[Adding hardware](docs/ADDING_BOARDS.md) · [Validation](docs/VALIDATION.md)

```text
CMakeLists.txt           MagicBox IDF project, version 0.1.0
main/
  application.*          Shared conversation and lifecycle
  protocols/             Magic Server clients
  audio/                 Shared audio services and codecs
  boards/
    common/              Board/WifiBoard interfaces and helpers
    m09-s3-hw210/         M09 adapter and authoritative BSP
    m301-c5-rtc-v1-5g/    M301 adapter and authoritative BSP
    c5-reference-m301-hw120/  Legacy reference BSP; no product build
    manifest.json        Explicit hardware/SDK/diagnostic registry
partitions/              Product partition layouts
diagnostics/            Independent baseline and hardware-demo projects
scripts/                 Resource generators and application regression tests
tools/                   Explicit profile selection, build and packaging
tests/                   Profile and build-tool tests
docs/                    Hardware evidence, architecture and provenance
```

| Profile | Target | Firmware SDK | Status |
| --- | --- | --- | --- |
| `m09-s3-hw210` | ESP32-S3 | IDF 6.1 | M09 / HW2.1.0, product + diagnostics |
| `m301-c5-rtc-v1-5g` | ESP32-C5 | IDF 6.1 | M301 confirmed; physical PCB revision pending |
| `c5-reference-m301-hw120` | ESP32-C5 | IDF 5.5.3 diagnostics only | Historical power/pin reference; no product firmware |

## Build

```sh
python3 tools/build.py --list-boards
source /path/to/esp-idf-v6.1/export.sh
python tools/build.py --board m09-s3-hw210
python tools/build.py --board m301-c5-rtc-v1-5g
python tools/build.py --board m09-s3-hw210 --app baseline
python tools/build.py --board m09-s3-hw210 --app hardware-demo
```

A complete profile is mandatory. Product firmware is the default app; diagnostics
are explicit. Artifacts live in `dist/<variant>/` or `dist/<profile>/<diagnostic>/`,
with hashes and build metadata. Root images are named `magicbox.bin`; OTA board
and variant identities remain unchanged. Build tools never flash or publish.
The old `tools/build_xiaozhi.sh` remains a compatibility command.

The standard variants discover Magic Server at
`https://magic.folotoy.cn/xiaozhi/ota/`. Network provisioning uses the `FoloToy`
hotspot prefix. Local-network variants remain explicit opt-ins. They are listed
in each board's `config.json`; default firmware does not seed demo credentials.

## Hardware boundaries

M09 uses direct digital I2S audio, RGB, buttons, a knob and LIS3DH. Its application
owns the buses and only shares BSP headers; it must not initialize a second BSP
audio owner. M301 uses its own ES8311/AXP2101/BM8563/GX8002 BSP. The old HW1.2.0
reference has different PMU rail aliases and remains separate.

The public and internal repositories retain their own runtime baselines and
variants. Source notices and upstream license remain in `docs/upstream/`.
[Source and layout migration record](docs/LAYOUT_MIGRATION.md).

Historical released files keep their original names and validation scope.
Restructuring does not publish a release or establish new hardware acceptance.
