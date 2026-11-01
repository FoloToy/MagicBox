# MagicBox BSP

Reusable native ESP-IDF board support for FoloToy MagicBox hardware. This
repository provides **drivers, pin definitions and hardware examples** for
building your own applications. [简体中文](README.zh_CN.md)

```text
boards/
  manifest.json                    Explicit hardware / SDK registry
  m09-s3-hw210/bsp/                 M09 · ESP32-S3 · HW2.1.0
  m301-c5-rtc-v1-5g/bsp/            M301 · ESP32-C5 · source-board profile
  c5-reference-m301-hw120/bsp/      Separate legacy C5 reference
examples/<profile>/
  minimal/                         Minimal peripheral bring-up
  hardware-demo/                   Explicit hardware diagnostics
tools/                            Build-only selection and validation
tests/                             Profile, boundary and fault tests
docs/                              Integration and hardware guidance
```

| Profile | Target | ESP-IDF | Scope |
| --- | --- | --- | --- |
| `m09-s3-hw210` | ESP32-S3 | 6.1 | M09 / HW2.1.0, direct I2S, controls and LIS3DH |
| `m301-c5-rtc-v1-5g` | ESP32-C5 | 6.1 | M301 source board, ES8311, AXP2101, RTC and GX8002 |
| `c5-reference-m301-hw120` | ESP32-C5 | 5.5.3 | Historical reference; separate PMU rail wiring |

A chip name is not a board profile. M301's physical PCB revision remains
unconfirmed; the historical HW1.2.0 reference is not interchangeable with it.

## Start with an example

```sh
python3 tools/build.py --list-boards
source /path/to/esp-idf-v6.1/export.sh
python tools/build.py --board m09-s3-hw210
python tools/build.py --board m09-s3-hw210 --app hardware-demo
python tools/build.py --board m301-c5-rtc-v1-5g --app minimal
```

`--board` is mandatory. The default example is `minimal`; `--app baseline`
remains an alias. Use IDF 5.5.3 for the legacy reference. Outputs are in
`dist/<profile>/<example>/`, including the application, bootloader, table and
checksummed build record. Builds never open a device or flash it.

## Develop your application

Add **one** `boards/<profile>/bsp` directory to your application's
`EXTRA_COMPONENT_DIRS`, then declare `REQUIRES bsp` in the consuming component.
The examples demonstrate initialization and bus ownership. Board-specific
peripheral APIs live in each BSP's `include/` directory.

Your application owns networking, service protocols, credentials, activation,
OTA and interaction policy. The BSP initializes hardware only. Reuse its bus
owners rather than initializing a second I2C/I2S owner.

[Integration guide](docs/INTEGRATION.md) · [Architecture](docs/ARCHITECTURE.md) ·
[Add a board](docs/ADDING_BOARDS.md) · [Validation](docs/VALIDATION.md) ·
[Source provenance](docs/PROVENANCE.md)

Example partition layouts are development fixtures. Review your actual device
layout and backups before installing examples on an existing product. Historical
application branches/releases remain history; current BSP development starts here.
