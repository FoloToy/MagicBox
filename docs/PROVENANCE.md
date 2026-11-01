# Hardware source provenance

The retained BSP implementations and example bodies come from this public
repository at commit 7b16c09 (the merged layout reorganization). This correction
moves their paths without changing the drivers or pin definitions. Product
clients, adapters and release tools were removed from the current source tree;
historical commits, branches and releases are not rewritten.

M09 is owner-confirmed ESP32-S3 / HW2.1.0. M301 is owner-confirmed ESP32-C5;
its source board is magicbox_rtc_v1_5G in folotoy-firmware-idf commit
8c013c923b59f1123e762880100d62ccf7005907, without assigning an unknown PCB revision.
The separate C5 HW1.2.0 reference derives from magicbox-c5 commit
f889d36f721dce7c503cdacf67f18de12a082ca2 and retains its original rail aliases.
Detailed source/hardware notes remain beside each board.

The MIT license and notices in retained sources remain intact. External driver
versions and licenses are declared in each BSP's component manifest. No source
from the private product implementation is introduced by this correction.
