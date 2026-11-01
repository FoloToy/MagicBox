# XiaoZhi client provenance and historical branch

MagicBox's shared client derives from the pinned native XiaoZhi import recorded
in docs/upstream/UPSTREAM.md; its license and source notices remain intact.
The current repository root is the Magic Server product, built with
`python tools/build.py --board <complete-profile>`; see [the firmware guide](MAGICBOX.md).

The public demo/xiaozhi branch retains the historical original-service port
and its separately named OTA identity. This structural refactor does not rewrite
that branch or change its service. It must not be selected by copying M09 pins
onto an arbitrary S3 board. Current root profiles use their own declared
hardware/variant identities and service settings.
