#!/usr/bin/env bash
set -euo pipefail
root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
if [[ -z "${IDF_PATH:-}" || ! -f "$IDF_PATH/tools/idf.py" ]]; then
  echo 'Source ESP-IDF 6.1 export.sh first; see docs/XIAOZHI.md.' >&2
  exit 1
fi
exec python "$root_dir/tools/build_xiaozhi.py" "$@"
