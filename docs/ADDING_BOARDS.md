# Adding MagicBox hardware

1. Identify model, silicon, physical PCB revision, Flash/PSRAM, pinout, codec,
   speaker/microphone directions, power rails, keys/LEDs and partition/identity
   layout. Matching chip names do not imply matching hardware.
2. Create `main/boards/<profile>/` with independent authoritative BSP headers
   and drivers, and a product adapter only when a product port exists. Public
   interfaces stay under common/; application/protocol code must not depend
   on a concrete board.
3. Add the explicit profile to `main/boards/manifest.json`, keeping unknown
   physical facts unknown. `firmware: null` means diagnostics/reference only.
4. A product profile owns `config.json`, `Kconfig.board`, `board.cmake` and one
   `DECLARE_BOARD`. Source its Kconfig inside the root board choice; CMake
   discovers its board hook. Add its ID to firmware_build.FIRMWARE_BOARDS. The root CMake entry requires
   explicit profile/variant metadata; use the build tool instead of chip-only
   idf.py invocation.
   Preserve unique OTA identities and explicit target/partition/PSRAM settings.
5. Add baseline/hardware-demo projects under `diagnostics/<profile>/`, linking
   the same `main/boards/<profile>/bsp`. Never duplicate pin headers. Declare
   the required SDK per profile; do not silently upgrade reference hardware.
6. Run repository/profile checks, host tests and affected product/diagnostic
   builds. Inspect checksums and generated settings. Device flashing requires
   exact-profile/image/range authorization, a restorable backup and separate
   peripheral/network/voice acceptance.

```sh
python3 tools/check_repo.py
python3 -m unittest discover -s tests -v
python3 -m unittest discover -s scripts/tests -v
python tools/build.py --list-boards
python tools/build.py --board m09-s3-hw210
```

Seeded development Wi-Fi and experimental AEC remain explicitly named variants.
Do not enable them merely because another board uses the same chip.
