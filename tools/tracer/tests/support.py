"""What every test of the tracer reads it with: the tracer on the path and its words, a fixture analysed once, and
the links of a definition by the labels of what they reach."""
from __future__ import annotations

import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent))

from tracer import analyse, vocab  # noqa: E402
from tracer.languages import PATTERNS  # noqa: E402,F401
from tracer.solver import Solver  # noqa: E402,F401
from tracer.vocab import (AS_KEPT, AS_NAME, BY_NAME, BY_TEXT, CALL, EXACT, FIRES, FN, HANDED, INST,  # noqa: E402,F401
                          KEPT, NEW, RUNNING)

FIXTURE = HERE / "fixture"
WORDS = {value for value in vars(vocab).values() if isinstance(value, str)}      # every word of the vocabulary
_DONE = {}


def compiler() -> str | None:
    """A C++ compiler with its standard library: g++ on the path, else the one winget installed (a shell opened before
    it does not have it on its path yet), else clang++."""
    import os
    import shutil
    found = shutil.which("g++")
    if found:
        return found
    packages = Path(os.environ.get("LOCALAPPDATA", "")) / "Microsoft" / "WinGet" / "Packages"
    if packages.is_dir():
        for folder in sorted(packages.iterdir()):
            hit = next(iter(folder.rglob("g++.exe")), None) if "WinLibs" in folder.name else None
            if hit:
                return str(hit)
    return shutil.which("clang++")


def traced(folder: str):
    """The analysis of one fixture folder, made once."""
    if folder not in _DONE:
        _DONE[folder] = analyse([FIXTURE / folder], FIXTURE / folder)
    return _DONE[folder]


def reached(result, label: str, kinds=None, how=None) -> set:
    """The labels of what the code of that label links to (by those kinds, found that way)."""
    program = result.program
    sources = [d.id for d in program.defs if d.kind in RUNNING and program.label(d.id) == label]
    assert sources, f"nothing is labelled {label}: {sorted({program.label(d.id) for d in program.defs})}"
    return {program.label(link.dst) for src in sources for link in result.out.get(src, ())
            if (kinds is None or link.kind in kinds) and (how is None or link.how == how)}
