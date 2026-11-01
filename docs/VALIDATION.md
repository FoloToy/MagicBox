# BSP validation record

Date: 2026-11-01. Scope: public reusable hardware components and their examples.

- 12 host tests passed, including profile/target rejection, removed product-mode
  rejection, service-coupling detection and correct example component references.
- Both C5 button adapters passed fault-injected cleanup, retry, idempotence and
  bounds checks against their actual source implementations.
- Six native example builds passed: minimal and hardware-demo on M09, M301 and
  legacy C5. M09/M301 used ESP-IDF 6.1; legacy C5 used ESP-IDF 5.5.3.
- Every exported example application matches its recorded SHA256.
- All 39 retained BSP C/C++ source/header files match public baseline 7b16c09.
  Example bodies, SDK defaults and partition CSVs also remain byte-identical.
  Only their component references/paths and repository scope changed.

[Build records](build-validation.json) · [Source comparison](source-audit.json)

The checker rejects product/client trees, server coupling, hardcoded network
credentials and initialization of application services in the minimal example.
The consuming application chooses all networking and product behavior.

Earlier model/pin/rail facts remain recorded beside each board. Compilation
and host fault tests are not electrical/peripheral acceptance. No hardware was
flashed or newly accepted in this scope correction. No release was published.
