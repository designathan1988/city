"""The stacks of a real run, kept raw: every sampled stack of every thread as (module, offset) frames, with how many times
it was seen. The sampling is the stack sampler's (`tools/analysis/sampler.py`, used as a library and left unchanged):
each thread frozen in turn, walked over each module's own unwind tables, let go. Nothing here knows a program.
"""
from __future__ import annotations

import json
import sys
from collections import Counter
from pathlib import Path

ANALYSIS = Path(__file__).resolve().parent.parent
START_MARGIN_S = 120            # past the sampling time, the started program's deadline: its start-up and the walk's lag


def _sampler():
    """The sampler module and the process runner it uses (imported on demand: they load Windows libraries)."""
    sys.path.insert(0, str(ANALYSIS))
    sys.path.insert(0, str(ANALYSIS.parent))
    import sampler                          # noqa: E402
    from tasks import proc                  # noqa: E402
    return sampler, proc


def record(command: str, seconds: float, interval_ms: float, hidden: bool, progress=None) -> dict:
    """Runs `command` (in a job of its own, killed with its tree at the end) and samples it for `seconds`.
    Returns {"modules": {path: [base, end]}, "stacks": [[count, [[module path, offset], ...]], ...]}, innermost frame
    first; a frame outside every module is kept as [None, address] (a lost frame: the chain is cut there)."""
    sampler, proc = _sampler()
    target = proc.Process(command, hidden=hidden, deadline_s=seconds + START_MARGIN_S)
    try:
        s = sampler.Sampler(target.pid)
        if progress:
            progress(f"sampling process {target.pid} for {seconds:.0f} s every {interval_ms:g} ms")
        samples = s.run(seconds, interval_ms / 1000.0)
    finally:
        target.close()
    modules = sorted((base, end, path) for base, (end, path) in s.modules.items())
    counts = Counter(stack for _, _, stack, _, _ in samples)
    stacks = []
    for stack, count in counts.most_common():
        frames = []
        for address in stack:
            hit = next(((base, path) for base, end, path in modules if base <= address < end), None)
            frames.append([hit[1], address - hit[0]] if hit else [None, address])
        stacks.append([count, frames])
    if progress:
        progress(f"{len(samples)} samples, {len(stacks)} distinct stacks, {len(modules)} modules")
    return {"modules": {path: [base, end] for base, end, path in modules}, "stacks": stacks}


def save(data: dict, path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(data), encoding="utf-8")


def load(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))
