"""A feature tracer: reads the code under some folders with each language's grammar and says, for every function, what
it calls, what it makes and what it hands to code it did not read. `analyse` is the whole of it; nothing here knows a
program, a library or a platform by name.
"""
from __future__ import annotations

import gc
import os
from pathlib import Path

from .compilers import answers
from .flow import Flow
from .program import Program
from .read import read
from .scopes import Scopes


AMONG = 8                        # the most functions found by name alone one call is linked to


def analyse(roots, base=None, say=lambda words: None, among: int | None = AMONG, keep: bool = False,
            ceiling=None, compilers: bool = True):
    """The links of the code under the roots (each file shown by its path from `base`, by default the folder the
    roots share); `say` is told each step as it ends. A call found by name alone among more functions than `among`
    is linked to none and said ambiguous (None: every one). `keep`: the files' facts are kept (what evaluating the
    program reads); `ceiling` (`limits.Ceiling`): past its time or memory the analysis stops, said in `result.cut`. `compilers`: what
    each language's compiler says a call runs is its answer (`compilers`); without, the flow's alone."""
    roots = [Path(root).resolve() for root in roots]
    if base is None:
        base = Path(os.path.commonpath([root if root.is_dir() else root.parent for root in roots]))
    base = Path(base).resolve()
    collecting = gc.isenabled()          # the analysis keeps what it makes: no collection of cycles while it runs
    gc.disable()
    try:
        parsed = read(roots, base, ceiling)
        say(f"read {len(parsed)} files")
        program = Program(parsed, base)
        parsed = None                    # what was read is the program's now: nothing else holds it
        say(f"{len(program.defs)} definitions")
        scopes = Scopes(program)
        say("scopes and bases settled")
        compiled = answers(program, ceiling, say) if compilers else {}
        result = Flow(program, scopes, among, keep, ceiling, compiled).solve(say)
    finally:
        if collecting:
            gc.enable()
    if result.cut:
        say(f"stopped at the ceiling ({result.cut}): the analysis is partial")
    for note in program.configs.notes:           # the files each language read to resolve names, or why not
        say(note)
    return result
