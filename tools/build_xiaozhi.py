#!/usr/bin/env python3
"""Build the downstream application, isolating vendor linkers from paths with spaces."""
import argparse
from profiles import select
from staging import sync_tree
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]
PROJECT = ROOT / "apps/xiaozhi"
DEMO_BOARDS = {
    "m09-s3-hw210": "folotoy/magicbox",
    "m301-c5-rtc-v1-5g": "folotoy/magicbox-c5",
}


def selection(board, variant=None):
    profile = select(board)
    if board not in DEMO_BOARDS:
        raise ValueError("MagicBox application is not ported for " + board)
    directory = DEMO_BOARDS[board]
    config = json.loads((PROJECT / "main/boards" / directory / "config.json").read_text())
    if config["target"] != profile["target"]:
        raise ValueError("Application target does not match BSP profile")
    variants = [entry["name"] for entry in config["builds"]]
    variant = variant or variants[0]
    if variant not in variants:
        raise ValueError("Variant does not belong to " + board + ": " + variant)
    return profile, directory, config, variant


def main():
    parser = argparse.ArgumentParser(description="Explicit MagicBox XiaoZhi profile")
    parser.add_argument("--board", required=True)
    parser.add_argument("--variant")
    args = parser.parse_args()
    try:
        profile, directory, board_config, variant = selection(args.board, args.variant)
    except ValueError as error:
        parser.error(str(error))
    project = PROJECT
    if any(character.isspace() for character in str(project)):
        key = hashlib.sha256(str(ROOT).encode()).hexdigest()[:12]
        cache = Path(tempfile.gettempdir()) / ("folotoy-magicbox-" + key + "-" + profile["id"])
        if any(character.isspace() for character in str(cache)):
            raise SystemExit("Use a temporary directory without whitespace (set TMPDIR).")
        owner = cache / ".magicbox-source.json"
        expected = {"source": str(ROOT)}
        if cache.exists():
            if not owner.is_file() or json.loads(owner.read_text()) != expected:
                raise SystemExit("Refusing to reuse an unowned build cache: " + str(cache))
        else:
            cache.mkdir(parents=True)
            owner.write_text(json.dumps(expected))
        project = cache / "apps/xiaozhi"
        project.mkdir(parents=True, exist_ok=True)
        sync_tree(PROJECT, project, ["build/", "managed_components/", "releases/", "sdkconfig", "sdkconfig.old",
                         "dependencies.lock", "__pycache__/", ".git/", "main/assets/lang_config.h",
                         "main/mmap_generate_emoji.h", "main/mmap_generate_*.h"])
        if profile["target"] == "esp32c5":
            bsp = cache / profile["project"]
            bsp.mkdir(parents=True, exist_ok=True)
            sync_tree(ROOT / profile["project"], bsp,
                      ["build/", "build-*/", "managed_components/", "sdkconfig", "sdkconfig.old",
                       "dependencies.lock", "__pycache__/"])
        pins = cache / profile["pins"]
        pins.parent.mkdir(parents=True, exist_ok=True)
        for header in (ROOT / profile["pins"]).parent.glob("*.h"):
            shutil.copy2(header, pins.parent / header.name)
        # Reuse already-downloaded components, without changing their generated sources.
        if (PROJECT / "managed_components").is_dir() and not (project / "managed_components").exists():
            shutil.copytree(PROJECT / "managed_components", project / "managed_components")
        print("Building in whitespace-free cache:", project, flush=True)
    command = [sys.executable, "scripts/build.py", directory, "--name",
               variant, "--language", "zh-CN"]
    result = subprocess.run(command, cwd=project)
    if result.returncode:
        return result.returncode
    if project != PROJECT:
        outputs = ["xiaozhi.bin", "xiaozhi.elf", "xiaozhi.map", "merged-binary.bin",
                   "partition_table/partition-table.bin", "bootloader/bootloader.bin",
                   "project_description.json", "flasher_args.json"]
        for relative in outputs:
            source = project / "build" / relative
            if source.is_file():
                target = PROJECT / "build" / relative
                target.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(source, target)
        for source in (project / "releases").glob("*.zip"):
            target = PROJECT / "releases" / source.name
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source, target)
        print("Artifacts copied to:", PROJECT / "build", flush=True)
    image = PROJECT / "build/xiaozhi.bin"
    if image.is_file():
        checksum = hashlib.sha256(image.read_bytes()).hexdigest()
        image.with_suffix(".sha256").write_text(checksum + "  " + image.name + "\n")
        (PROJECT / "build/magicbox-build.json").write_text(json.dumps({
            "profile": profile["id"], "target": profile["target"],
            "model": profile["model"], "hardware_revision": profile["hardware_revision"],
            "source_board": profile.get("source_board"),
            "source_commit": profile.get("source_commit"),
            "variant": variant, "board_type": board_config["type"], "sha256": checksum,
            "size": image.stat().st_size, "device_test": "not performed by build tool"
        }, indent=2) + "\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
