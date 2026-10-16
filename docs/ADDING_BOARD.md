# Add another hardware revision

1. Assign a confirmed model, silicon target and PCB revision. If sales mapping is unknown, use an explicitly named reference profile rather than guessing the model.
2. Create a new `boards/<model>-<chip>-<revision>/esp-idf` project and a separate canonical pin header. Do not edit M09's pins to accommodate another S3 PCB.
3. Add its capabilities, SDK contract, source provenance and evidence to `boards/manifest.json`. The SDK target must match that profile; chip detection alone must not select it.
4. Build both minimal and diagnostic projects. Validate rail sequencing, audio directions/format, optional devices, buttons, volume and wake pins on the actual unit. Unsupported capabilities must be absent or explicitly fail, never return fake success.
5. Add a CI matrix row and wrong-target/profile tests. Label artifacts and firmware identity with the full profile ID.
6. XiaoZhi support is separate: add a uniquely named board/build/OTA identity and compile/runtime acceptance on `demo/xiaozhi`. A working BSP does not prove a working voice application.

Do not copy device secrets, raw production partitions or private application implementations into a profile. A development partition layout is never permission to overwrite a production factory image or identity.
