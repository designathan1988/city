"""The queue: one unit per feature slice, with its cost vector, the contract violations met inside it with the path from the entry to each, every
place, the candidate owner and what disappears with the fix; ordered by how often the slice runs, how much it breaks and what it cost in the past."""
import bisect
import collections

from .contracts import FREQ_RANK
from .core import digest


def finding_id(f):
    return digest(f"{f['class']}|{f['file']}|{f['line']}|{f['detail']}")


def _containers(prog):
    """Per file, the functions by line span, to say which function holds a place."""
    by_file = collections.defaultdict(list)
    for fid, f in prog.fn.items():
        by_file[f["file"]].append((f["line"], f["end"], fid))
    for spans in by_file.values():
        spans.sort()
    return by_file


def holder(by_file, file, line):
    spans = by_file.get(file, [])
    i = bisect.bisect_right(spans, (line, float("inf"), ""))
    best = None
    for start, end, fid in reversed(spans[:i]):
        if start <= line <= end and (best is None or end - start < best[0]):
            best = (end - start, fid)
    return best[1] if best else None


def _commits(history):
    touched = collections.defaultdict(set)
    for i, (_, changes) in enumerate(history):
        for change in changes:
            touched[change[1]].add(i)
    return touched


def build(prog, entries, slices, findings, history, cost_of):
    by_file = _containers(prog)
    touched = _commits(history)
    for f in findings:
        f["id"] = finding_id(f)
        f["function"] = holder(by_file, f["file"], f["line"])
    by_owner = collections.defaultdict(list)
    for f in findings:
        by_owner[f["owner"]].append(f["id"])
    in_slice = collections.defaultdict(list)
    for f in findings:
        if f["entry"] is not None:
            in_slice[f["entry"]].append(f)
    loose = [f for f in findings if f["entry"] is None and f["function"] is not None]
    queue = []
    for e in entries:
        sl = slices[e["id"]]
        met = list(in_slice[e["id"]]) + [f for f in loose if f["function"] in sl.functions]
        rows = []
        for f in met:
            path = f["proof"] or [{"file": prog.fn[x]["file"], "line": line, "what": x} for x, line, _ in sl.first.get(f["function"], ())]
            rows.append({"id": f["id"], "class": f["class"], "file": f["file"], "line": f["line"], "detail": f["detail"], "path": path})
        owners = collections.Counter(f["owner"] for f in met if f["owner"])
        owner = owners.most_common(1)[0][0] if owners else ""
        mine = {f["id"] for f in met}
        files = {p["file"] for f in met for p in f["places"]} | {e["file"]}
        queue.append({"id": digest(e["id"]), "entry": e["name"], "kind": e["kind"], "freq": e["freq"], "file": e["file"], "line": e["line"],
                      "why": e["why"], "via": e.get("via", []), "cost": cost_of(e, sl), "violations": sorted(rows, key=lambda r: (r["class"], r["file"], r["line"])),
                      "places": sorted({(p["file"], p["line"], p["what"]) for f in met for p in f["places"]}), "owner": owner,
                      "vanish": sorted(set(by_owner.get(owner, ())) - mine) if owner else [], "commits": len(set().union(*(touched.get(x, set()) for x in files)))})
    queue.sort(key=lambda q: (-len(q["violations"]) * (1 + FREQ_RANK.get(q["freq"], 0)), -q["cost"]["deepest"], -q["commits"], q["entry"], q["id"]))
    return queue
