"""Golden traces: features of real projects whose trace is known, checked against what the tracer finds.

    python bench/golden.py <golden.json>... [--quiet]

A golden file names the roots to read and a list of checks, each from a start:

    {"name": "...", "roots": ["..."], "base": "...", "checks": [
      {"start": "Label", "direction": "forward" | "backward", "how": ["exact", "by text"],   (default: every link)
       "reach": ["Label", ...],          every one must be in the trace (a label, or the end of one after a dot)
       "reach_files": ["a.ts", ...],     a step past the start in each of these files (a path, or its end)
       "avoid": ["Label", ...],          none may be
       "reach_handed_to": "Label",       every function the callers of that definition hand it is in the trace
       "writes": ["Label", ...],         state the trace writes (or may change): each must be among it
       "readers": ["Label", ...],        each must read state the trace writes (straight, or a record such a reader
                                         builds)
       "max_fraction": 0.1,              at most this share of the program's definitions in the trace
       "max_over_exact": 1.3,            at most this many times the definitions the exact links and the ones by
                                         text alone reach (the guesses by name it adds are few)
       "oracle": "file.jsonl", "min_cover": 0.9}   at least this share of what the compiler reaches from the start
    ]}

Exit 1 when a check fails; what was missing or too much is printed. Not part of the tracer.
"""
from __future__ import annotations

import argparse
import collections
import inspect
import json
import sys
import time
from pathlib import Path

HOME = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(HOME))
sys.setrecursionlimit(100000)

from tracer import analyse  # noqa: E402
from tracer.vocab import RUNNING  # noqa: E402
from tracer.walk import backward, find, forward  # noqa: E402

_DONE = {}


def analysed(roots, base):
    key = (tuple(roots), base)
    if key not in _DONE:
        began = time.monotonic()
        _DONE[key] = analyse(roots, base)
        print(f"  analysed {len(roots)} roots in {time.monotonic() - began:.1f} s", file=sys.stderr)
    return _DONE[key]


def matches(label: str, wanted: str) -> bool:
    return label == wanted or label.endswith("." + wanted)


def oracle_reach(path: Path, start: tuple) -> set:
    """What the compiler's calls reach from a definition (file, line): the definitions, as (file, line)."""
    edges = collections.defaultdict(set)
    for line in path.read_text(encoding="utf-8").splitlines():
        if not line.strip():
            continue
        row = json.loads(line)
        caller = row.get("caller")
        if caller:
            for t in row["targets"]:
                edges[(caller["file"], caller["line"])].add((t["file"], t["line"]))
    seen, todo = set(), [start]
    while todo:
        at = todo.pop()
        for nxt in edges.get(at, ()):
            if nxt not in seen and nxt != start:
                seen.add(nxt)
                todo.append(nxt)
    return seen


def run_check(result, check: dict, folder: Path) -> list:
    """The failures of one check, as lines."""
    p = result.program
    starts = find(result, check["start"])
    if not starts:
        return [f"nothing is named {check['start']}"]
    allowed = check.get("how")
    keep = (lambda link: link.how in allowed) if allowed else (lambda link: True)
    walker = forward if check.get("direction", "forward") == "forward" else backward
    options = {"keep": keep}
    if "guesses" in check and "guesses" in inspect.signature(walker).parameters:
        options["guesses"] = check["guesses"]
    steps = walker(result, starts, **options)
    labels = {p.label(step.at) for step in steps}
    spots = {(p.files[p.defs[step.at].file].display, p.defs[step.at].line) for step in steps}
    failures = []
    missing = [want for want in check.get("reach", ()) if not any(matches(label, want) for label in labels)]
    if missing:
        failures.append(f"missing: {missing}")
    files = {display for display, _ in spots if display}
    absent = [want for want in check.get("reach_files", ())
              if not any(steps[i].depth > 0 and p.files[p.defs[steps[i].at].file].display.endswith(want)
                         for i in range(len(steps)))]
    if absent:
        failures.append(f"no step in: {absent} (files reached: {len(files)})")
    present = [bad for bad in check.get("avoid", ()) if any(matches(label, bad) for label in labels)]
    if present:
        failures.append(f"present though it must not be: {present}")
    if "max_fraction" in check:
        total = sum(1 for d in p.defs if d.kind in RUNNING)
        share = len({step.at for step in steps}) / total
        if share > check["max_fraction"]:
            failures.append(f"reaches {share:.1%} of the program's definitions (at most {check['max_fraction']:.0%})")
    if "reach_handed_to" in check:
        target = [d.id for d in p.defs if matches(p.label(d.id), check["reach_handed_to"]) and d.kind == "function"]
        handed = {fn for d in target for link in result.into.get(d, ()) for fn in link.fns}
        lost = sorted(p.label(fn) for fn in handed if p.label(fn) not in labels)
        if not handed:
            failures.append(f"nothing is handed to {check['reach_handed_to']}")
        elif lost:
            failures.append(f"{len(lost)} of the {len(handed)} functions handed to {check['reach_handed_to']} "
                            f"not reached: {lost[:8]}")
    if "writes" in check or "readers" in check:
        state = getattr(result, "state_out", None)
        if state is None:
            failures.append("the tracer records no state")
        else:
            from tracer.report import written, readers_of
            units = [step.at for step in steps if not step.again]
            wrote = written(result, units)
            labels_w = {p.label(d) for d in wrote}
            gone = [w for w in check.get("writes", ()) if not any(matches(label, w) for label in labels_w)]
            if gone:
                failures.append(f"state not written: {gone} (written: {sorted(labels_w)[:12]})")
            reading = {p.label(u) for d in wrote for u, _ in readers_of(result, d)}
            unread = [w for w in check.get("readers", ()) if not any(matches(label, w) for label in reading)]
            if unread:
                failures.append(f"not among the readers of what it writes: {unread}")
    if "max_over_exact" in check:
        exact = {step.at for step in walker(result, starts, keep=lambda link: link.how in ("exact", "by text"))}
        ratio = len({step.at for step in steps}) / max(len(exact), 1)
        if ratio > check["max_over_exact"]:
            failures.append(f"reaches {ratio:.2f} times what the exact links and the ones by text reach "
                            f"(at most {check['max_over_exact']})")
    if "oracle" in check:
        d = p.defs[starts[0]]
        reach = oracle_reach(folder / check["oracle"], (p.files[d.file].display, d.line))
        cover = len(reach & spots) / len(reach) if reach else 1.0
        if cover < check.get("min_cover", 0.9):
            lost = sorted(reach - spots)[:15]
            failures.append(f"covers {cover:.1%} of the {len(reach)} definitions the compiler reaches "
                            f"(at least {check.get('min_cover', 0.9):.0%}); some missed: {lost}")
    return failures


def run(paths, quiet=False) -> int:
    failed = 0
    for path in paths:
        path = Path(path).resolve()
        golden = json.loads(path.read_text(encoding="utf-8"))
        roots = [root for root in golden["roots"]]
        if not all(Path(root).exists() for root in roots):
            print(f"{golden['name']}: skipped, a root is missing", file=sys.stderr)
            continue
        result = analysed(roots, golden.get("base"))
        for check in golden["checks"]:
            what = f"{golden['name']}: {check.get('direction', 'forward')} {check['start']} {check.get('how') or 'all'}"
            failures = run_check(result, check, path.parent)
            failed += bool(failures)
            if failures:
                print(f"FAIL {what}\n" + "\n".join("     " + line for line in failures))
            elif not quiet:
                print(f"ok   {what}")
    return 1 if failed else 0


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("golden", nargs="+")
    parser.add_argument("--quiet", action="store_true")
    args = parser.parse_args(argv)
    return run(args.golden, args.quiet)


if __name__ == "__main__":
    sys.exit(main())
