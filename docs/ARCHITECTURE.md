# MagicBox profiles and ownership

The root is the native IDF product project, following the same layout principles
as Rebot: shared services plus independent board implementations. There is no
nested apps/xiaozhi project. Public and internal repositories each retain their
own application baseline; restructuring does not transfer internal runtime code
into the public repository.

A profile means model + silicon + PCB revision. `main/boards/manifest.json`
registers board identity, target, SDK, pin authority, product configuration and
diagnostic paths. No wildcard board or chip-only selection is allowed.

| Location | Ownership |
| --- | --- |
| `main/application.*`, `ota.*` | Shared state, activation, conversation and lifecycle |
| `main/protocols/` | Shared Magic Server transports |
| `main/audio/` | Shared audio service, engines, codecs and bounded queues |
| `main/boards/common/` | Board interfaces, networking and reusable helpers |
| `main/boards/<profile>/` | Board factory, pins, codec composition, controls, power and BSP |
| `diagnostics/<profile>/` | Independent baseline and hardware-demo projects |
| `partitions/` | Verified product layouts |
| `tools/` | Selection, build-only staging and packaging |

Each supported product board owns config.json, Kconfig.board and board.cmake.
The main component includes board hooks and compiles exactly one factory.
`bsp/include/bsp_pins.h` is the single pin authority. M09 product code shares
headers without initializing a second BSP bus owner; the C5 product uses its
own local BSP component. Diagnostics link that same canonical BSP, not copies.

The historical C5 reference retains IDF 5.5.3, separate from the M301 source
profile on IDF 6.1. Their PMU rails are not interchangeable; unknown physical
PCB revision remains unknown. Firmware build availability is independent of
BSP/diagnostic availability.

`tools/build.py --board <profile>` builds the product. `--app baseline` and
`--app hardware-demo` select diagnostics. Every profile/variant is staged in
an owned whitespace-free cache. Targets, SDK version and generated hardware
Kconfig are checked. Product output uses the MagicBox descriptor/name;
compatibility-sensitive board/OTA identities, partition bytes and service
configuration remain unchanged. Build results are not device acceptance.
