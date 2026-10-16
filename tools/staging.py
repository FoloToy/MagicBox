"""Synchronize owned build caches without external rsync; preserve generated state."""
import fnmatch
import shutil
from pathlib import Path

def sync_tree(source, destination, excluded=(), relative=Path()):
    source, destination = Path(source), Path(destination)
    destination.mkdir(parents=True, exist_ok=True)
    def ignored(name):
        path = (relative / name).as_posix()
        return any(fnmatch.fnmatch(path, p.rstrip('/')) or
                   ('/' not in p.rstrip('/') and fnmatch.fnmatch(name, p.rstrip('/')))
                   for p in excluded)
    source_names = {p.name for p in source.iterdir() if not ignored(p.name)}
    for child in destination.iterdir():
        if child.name not in source_names and not ignored(child.name):
            if child.is_dir() and not child.is_symlink(): shutil.rmtree(child)
            else: child.unlink()
    for name in source_names:
        src, dst = source / name, destination / name
        if src.is_dir():
            if dst.exists() and not dst.is_dir(): dst.unlink()
            sync_tree(src, dst, excluded, relative / name)
        else:
            if dst.is_dir(): shutil.rmtree(dst)
            shutil.copy2(src, dst)
