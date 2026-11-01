# Add a board profile

1. Identify model, silicon, PCB revision, Flash/PSRAM, peripherals, buses, power
   rails and pin map. Leave unknown hardware facts unknown.
2. Create boards/<profile>/bsp with src/, include/, CMakeLists.txt and its pinned
   idf_component.yml dependencies. Keep bsp_pins.h as the unique pin authority.
   Never copy M09 pins onto another S3 PCB or reuse legacy C5 rail aliases for M301.
3. Register target, SDK, BSP, pin contract and example paths in boards/manifest.json.
4. Add examples/<profile>/minimal and hardware-demo, linking that same BSP.
   Minimal initializes basic hardware only; optional diagnostics exercise
   peripherals explicitly. Neither adds service clients or saved credentials.
5. Run tools/check_repo.py, host tests and both example builds using the declared
   SDK. Add a CI matrix entry. Record source facts, build results and separate
   physical acceptance; compilation does not prove electrical behavior.

Application protocols, account state, provisioning policy, OTA and product UI
belong to the consumer project, not a board-specific factory in this library.
