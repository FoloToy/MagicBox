# Branch and service roles

The reorganized default product tree puts the Magic Server application at root.
The public repository's original product baseline comes from demo/magicbox;
the internal repository already carried that product on main. Current source
identity and validation are recorded in LAYOUT_MIGRATION.md.

Historical demo/magicbox and demo/xiaozhi branches remain historical branches;
this refactor does not rewrite them. The latter contains the original upstream
service port. New work targets the dedicated root product structure. Select
hardware with tools/build.py, not by switching a chip-only branch.

Standard MagicBox variants retain Magic Server discovery. The explicitly named
-local development variant and internal experimental variants keep their own
configuration and opt-in requirements. Restructuring does not switch accounts,
servers, credentials, board identities or published release files.
