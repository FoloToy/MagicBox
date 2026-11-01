#!/usr/bin/env python3
"""Build one complete MagicBox hardware profile; no flash, publish or implicit chip selection."""
import argparse
import subprocess
from profiles import profiles, select
from firmware_build import selection, build


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--list', '--list-boards', dest='list_boards', action='store_true')
    parser.add_argument('--board')
    parser.add_argument('--target')
    parser.add_argument('--variant')
    parser.add_argument('--app', choices=['firmware', 'baseline', 'hardware-demo'], default='firmware')
    args = parser.parse_args()
    if args.list_boards:
        for p in profiles():
            print(p['id'], p['target'], 'IDF ' + p['idf_series'],
                  'firmware + diagnostics' if p.get('firmware') else 'reference diagnostics only')
        return 0
    if not args.board:
        parser.error('--board is required; a chip does not identify a PCB')
    try:
        profile = select(args.board, args.target)
        if args.app != 'firmware':
            if args.variant: parser.error('--variant applies only to product firmware')
            from build_bsp import build_diagnostic
            return build_diagnostic(profile, args.app)
        profile, _, config, variant = selection(args.board, args.variant)
        return build(profile, config, variant)
    except (ValueError, RuntimeError, subprocess.CalledProcessError) as error:
        parser.exit(1, str(error) + '\n')


if __name__ == '__main__':
    raise SystemExit(main())
