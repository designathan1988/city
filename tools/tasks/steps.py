# Small steps the suites run as tasks through call.py (the spec is the task's data): copying a result into its golden.
from __future__ import annotations

from pathlib import Path

from .cache import place
from .task import ROOT


def copy(spec: dict) -> int:
    """{"from": path, "to": path}: a file or a folder copied whole (repo-relative or absolute). A missing source fails."""
    root = Path(spec.get("root") or ROOT)
    src, dst = (Path(spec[k]) if Path(spec[k]).is_absolute() else root / spec[k] for k in ("from", "to"))
    if not src.exists():
        print(f"FAIL nothing to copy: {spec['from']} does not exist")
        return 1
    if src.is_dir() and not any(src.iterdir()):
        print(f"FAIL nothing to copy: {spec['from']} is empty")
        return 1
    place(src, dst, root)
    print(f"copied {spec['from']} -> {spec['to']}")
    return 0
