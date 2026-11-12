"""A feature tracer: reads the code under some folders with each language's grammar and says, for every function, what
it calls, what it makes and what it hands to code it did not read. `analyse` is the whole of it; nothing here knows a
program, a library or a platform by name.
"""
from __future__ import annotations

import os
from pathlib import Path

from .flow import Flow
from .program import Program
from .read import read
from .scopes import Scopes


def analyse(roots, base=None, say=lambda words: None):
    """The links of the code under the roots (each file shown by its path from `base`, by default the folder the
    roots share); `say` is told each step as it ends."""
    roots = [Path(root).resolve() for root in roots]
    if base is None:
        base = Path(os.path.commonpath([root if root.is_dir() else root.parent for root in roots]))
    parsed = read(roots, Path(base).resolve())
    say(f"read {len(parsed)} files")
    program = Program(parsed)
    say(f"{len(program.defs)} definitions")
    scopes = Scopes(program)
    say("scopes and bases settled")
    return Flow(program, scopes).solve(say)
