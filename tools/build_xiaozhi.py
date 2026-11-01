#!/usr/bin/env python3
"""Compatibility entry point; use tools/build.py for root MagicBox firmware."""
from firmware_build import selection
from build import main
if __name__ == '__main__':
    raise SystemExit(main())
