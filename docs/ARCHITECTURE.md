# BSP ownership and repository scope

MagicBox public is a reusable hardware support library. Drivers and pin
contracts live under boards/<profile>/bsp; independent consuming examples live
under examples/<profile>. There is no root product application or cloud client.

The board registry selects complete model + silicon + PCB profiles explicitly.
Each profile has a unique bsp_pins.h and SDK contract. Multiple boards may use
the same silicon without sharing wiring or power sequences. The old C5 HW1.2.0
reference keeps its original IDF 5.5.3/rail aliases; M301's source profile uses
IDF 6.1 without inventing a physical PCB revision.

| Layer | Responsibility |
| --- | --- |
| BSP | GPIO, buses, audio peripheral access, power, sensors, hardware events |
| Minimal example | Bring up the selected hardware with no application services |
| Hardware example | Exercise hardware explicitly; tone/audio, controls and sensors |
| User application | Networking, protocols, credentials, interaction, activation and OTA |

BSP callbacks/ISRs expose events and queue work; they do not decide account,
cloud, UI or product behavior. One application owns each initialization and bus
lifecycle. Consumers must not create competing I2C/I2S owners.

The public build tool selects examples only and checks SDK/target before build.
Its default is minimal; no product variant can be selected. The boundary checker
rejects service clients, embedded credentials and application trees in the
current source distribution. All retained BSP driver code is unchanged by this
scope correction. Separate product repositories remain outside this task.
