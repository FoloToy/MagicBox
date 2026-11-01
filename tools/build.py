#!/usr/bin/env python3
"""Build explicit MagicBox BSP examples without selecting a product or service."""
import argparse
import subprocess
from profiles import profiles, select
from build_bsp import build_example


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--list', '--list-boards', dest='list_boards', action='store_true')
    parser.add_argument('--board')
    parser.add_argument('--target')
    parser.add_argument('--app', choices=['minimal', 'baseline', 'hardware-demo'], default='minimal',
                        help='baseline is a compatibility alias for minimal')
    args = parser.parse_args()
    if args.list_boards:
        for p in profiles():
            print(p['id'], p['target'], 'ESP-IDF ' + p['idf_series'], 'BSP + examples')
        return 0
    if not args.board:
        parser.error('--board is required; a chip alone does not identify a PCB')
    try:
        profile = select(args.board, args.target)
        return build_example(profile, 'minimal' if args.app == 'baseline' else args.app)
    except (ValueError, RuntimeError, subprocess.CalledProcessError) as error:
        parser.exit(1, str(error) + '\n')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
