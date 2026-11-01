# Dedicated MagicBox layout migration

Date: 2026-11-01. Repository: `MagicBox`. Runtime baseline: `cea0fc91d31dc586f31565e62024881fcc2c548b`.
The product was lifted from its own apps/xiaozhi import; application, audio,
protocol, board driver and BSP implementations are retained from this repository.
The root descriptor is now `magicbox` version 0.1.0. Original upstream source
lineage/license is retained in docs/upstream/UPSTREAM.md and LICENSE.

Moved product adapters:
- folotoy/magicbox -> main/boards/m09-s3-hw210
- folotoy/magicbox-c5 -> main/boards/m301-c5-rtc-v1-5g

Canonical BSPs now live alongside each profile. Baseline/diagnostic projects
moved to diagnostics/, with component paths updated to that single BSP.
Removed unrelated upstream boards, all-products build/release matrices and
marketing files; kept supported board tests, shared client tests and source
attribution. Local and experimental variants remain as declared in each
repository's own config.json.

This is structural/build validation. No hardware was flashed for this task.
Earlier device evidence in VALIDATION.md is inherited and is not acceptance
of newly built hashes. Build and test results are recorded separately below.

## Validation

- 19 repository/build-selection/packaging tests passed.
- Application suite: 16 tests run, zero failures,
  two host-development-dependency skips; CI installs cJSON/mbedTLS explicitly.
- Both C5 button adapters passed failure/retry/idempotence/bounds host checks.
- 3 declared product variants built on ESP-IDF 6.1, including explicit
  development/experimental variants present in this repository.
- Six diagnostic builds passed: baseline and hardware-demo for M09, M301 and
  legacy C5. The reference uses its original IDF 5.5.3; products and current
  diagnostics use IDF 6.1.
- Product checksums, every generated flash component (including C5 srmodels),
  merged-image offsets, factory-slot fit and identity-partition exclusion passed.

Build records: [layout-validation.json](layout-validation.json). Source/config
comparison: [layout-source-audit.json](layout-source-audit.json). Raw logs and
build outputs remain outside Git. Development manifests retain their real dirty
source flag; these are not published release artifacts or device acceptance.

The build tools isolate each board/variant's Component Manager cache and let
Component Manager download/verify dependencies rather than trusting incomplete
legacy managed trees. The product exporter follows the complete generated flash
graph, validates merged offsets and includes the C5 model partition file. The
SDK/container workflow trusts only its own checked-out workspace for Git
provenance after activation.
