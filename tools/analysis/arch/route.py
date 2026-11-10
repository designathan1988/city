"""The trace command: from what a scan published (slices.json), which entries of the program run a function, how often, by which calls, and
the route of the entry that runs it most often: its functions in program order with their files, and the state the whole run writes.
A function is named as the code names it (`Class::method`, a bare name) or by where it is (`file:line`, a file)."""
import collections
import json
import os
import re

from .contracts import FREQ_RANK
from .core import read_json

LINE_SPEC = re.compile(r"(.+):(\d+)$")
SHOWN = 12   # entries listed; the rest are counted
ROUTE_FILES = 60  # files of the route printed; the whole goes to --json


def wanted(table, spec):
    """Indices of the functions a spec names."""
    found = LINE_SPEC.match(spec)
    if found:
        path, line = found.group(1).replace("\\", "/"), int(found.group(2))
        inside = [(end - start, i) for i, (_, file, start, end) in enumerate(table) if file.endswith(path) and start <= line <= end]
        return [min(inside)[1]] if inside else []
    if "/" in spec or "\\" in spec:
        path = spec.replace("\\", "/")
        return [i for i, row in enumerate(table) if row[1].endswith(path)]
    return [i for i, row in enumerate(table) if row[0] == spec or row[0].endswith("::" + spec)]


def chain(sl, position):
    """The calls from the entry down to the function at a position of the slice: (function index, line of the call into the next)."""
    place = {f: i for i, f in enumerate(sl["order"])}
    out, at = [], position
    while sl["reached"][at] is not None:
        caller, line = sl["reached"][at]
        out.append((caller, line))
        at = place[caller]
    return out[::-1]


def below(sl, position):
    """The functions a function of the slice runs, in program order: every one whose way down from the entry passes through it."""
    place = {f: i for i, f in enumerate(sl["order"])}
    under = {position}
    for i in range(position + 1, len(sl["order"])):
        reached = sl["reached"][i]
        if reached is not None and place[reached[0]] in under:
            under.add(i)
    return [sl["order"][i] for i in sorted(under)]


def by_file(table, functions):
    files = collections.OrderedDict()
    for f in functions:
        files.setdefault(table[f][1], []).append(f)
    return files


def main(base, spec, out_json=None):
    data = read_json(os.path.join(base, "slices.json"))
    if data is None:
        print(f"no slices in {base}: run scan first (with --worktree for the code on disk)")
        return 1
    table, slices = data["functions"], data["slices"]
    targets = set(wanted(table, spec))
    if not targets:
        print(f"no function matches {spec}")
        return 1
    hits = []
    for sl in slices:
        where = [i for i, f in enumerate(sl["order"]) if f in targets]
        if where:
            hits.append((sl, where[0]))
    hits.sort(key=lambda h: (-FREQ_RANK.get(h[0]["freq"], 0), len(h[0]["order"]), h[0]["entry"], h[0]["id"]))
    print(f"{spec}: {len(targets)} function(s): " + "; ".join(f"{table[i][0]} ({table[i][1]}:{table[i][2]})" for i in sorted(targets)[:6]))
    if not hits:
        print("no entry of the program runs it: dead code, or reached only by a name the scan could not resolve")
        return 0
    print(f"run by {len(hits)} entr{'y' if len(hits) == 1 else 'ies'} ({dict(collections.Counter(h[0]['freq'] for h in hits))}):")
    for sl, position in hits[:SHOWN]:
        steps = " -> ".join(f"{table[f][0]} ({os.path.basename(table[f][1])}:{line})" for f, line in chain(sl, position))
        print(f"  [{sl['freq']}, {sl['kind']}] {sl['entry']}: {steps or 'the entry itself'} -> {table[sl['order'][position]][0]}")
    best = hits[0][0]
    files = by_file(table, best["order"])
    position = hits[0][1]
    down = by_file(table, below(best, position))
    print(f"what it runs itself: {sum(len(v) for v in down.values())} functions in {len(down)} files (the whole is in --json)")
    print(f"\nroute of {best['entry']} [{best['freq']}]: {len(best['order'])} functions in {len(files)} files, {best['jobs']} job(s) submitted; in program order:")
    for n, (file, fns) in enumerate(files.items(), 1):
        if n > ROUTE_FILES:
            print(f"  ... {len(files) - ROUTE_FILES} more files (--json has them all)")
            break
        mark = " *" if targets & set(fns) else ""
        print(f"  {n}. {file}{mark}: " + ", ".join(f"{table[f][0].rsplit('::', 1)[-1]}@{table[f][2]}" for f in fns[:8]) + (" ..." if len(fns) > 8 else ""))
    print("state the run writes: " + (", ".join(best["writes"][:24]) or "none") + (" ..." if len(best["writes"]) > 24 else ""))
    if out_json:
        listed = lambda group: [{"file": file, "functions": [[table[f][0], table[f][2], table[f][3]] for f in fns]} for file, fns in group.items()]  # noqa: E731
        route = {"spec": spec, "entry": best["entry"], "freq": best["freq"], "entries": [[sl["entry"], sl["freq"], sl["kind"]] for sl, _ in hits],
                 "chain": [[table[f][0], table[f][1], line] for f, line in chain(best, position)], "below": listed(down),
                 "targets": [[table[i][0], table[i][1], table[i][2], table[i][3]] for i in sorted(targets)],
                 "files": [{"file": file, "functions": [[table[f][0], table[f][2], table[f][3]] for f in fns]} for file, fns in files.items()],
                 "writes": best["writes"]}
        with open(out_json, "w", encoding="utf8", newline="\n") as f:
            f.write(json.dumps(route, ensure_ascii=False, sort_keys=True, indent=1) + "\n")
    return 0
