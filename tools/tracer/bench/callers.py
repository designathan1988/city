"""The tracer's callers of methods exposed across languages, against the pairs an oracle read from the code.

For every method of the oracle, the functions the backward trace reaches by exact links and links by text (each a
label, or the end of one) are compared with the method's real callers: found is the share of real pairs reached;
false is, of the functions linked straight to the method (not through a function that reads a table by key), the
share that are no real caller. Not part of the tracer.
"""
from __future__ import annotations

import collections
import json
import sys
from pathlib import Path

HOME = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(HOME))

from tracer.vocab import BY_TEXT, EXACT  # noqa: E402
from tracer.walk import backward  # noqa: E402

DEPTH = 4


def matches(label: str, wanted: str) -> bool:
    return label == wanted or label.endswith("." + wanted)


def measure(result, oracle: Path, language: str) -> dict:
    p = result.program
    pairs = json.loads(oracle.read_text(encoding="utf-8"))
    truth = collections.defaultdict(set)
    for row in pairs:
        truth[row["method"]].add(row["caller"])
    methods = collections.defaultdict(list)
    for d in p.defs:
        if d.kind == "function" and d.body and d.name in truth \
                and p.files[d.file].lang.name != language and p.is_class(d.owner):
            methods[d.name].append(d.id)
    tables = set((result.tables or {}).keys())
    found = total = false = presented = 0
    missed, wrong = collections.Counter(), collections.Counter()
    for name, callers in truth.items():
        ids = methods.get(name, [])
        if not ids:                          # a method of the platform's own class, not of the code read
            continue
        total += len(callers)
        steps = backward(result, ids, DEPTH, keep=lambda link: link.how in (EXACT, BY_TEXT)) if ids else []
        reached = {p.label(step.at) for step in steps}
        for caller in callers:
            if any(matches(label, caller) for label in reached):
                found += 1
            else:
                missed[name] += 1
        for d in ids:
            for link in result.into.get(d, ()):
                src = p.defs[link.src]
                if link.how not in (EXACT, BY_TEXT) or p.files[src.file].lang.name != language or link.src in tables:
                    continue
                presented += 1
                if not any(matches(p.label(link.src), caller) for caller in callers):
                    false += 1
                    wrong[f"{p.label(link.src)} -> {name}"] += 1
    return {"pairs": total, "found": round(found / total, 4) if total else None,
            "callers presented straight": presented, "false": round(false / presented, 4) if presented else 0.0,
            "most missed methods": missed.most_common(12), "some false callers": wrong.most_common(12)}
