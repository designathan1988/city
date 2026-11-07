"""Entry points of the program, each the root of one feature slice, enumerated from what the compiler and the formats say: methods overriding a
declaration of the outside (the engine calls them), functions a call files under a name (bound for scripts), lambdas started as threads or kept as
jobs, and script callbacks of the engine with what they reach on the native side."""
import collections

from .graph import FLOW


def display(prog, fid):
    f = prog.fn.get(fid)
    if f is None:
        return fid
    if f["kind"] == "lambda":
        return f"lambda {f['file']}:{f['line']}"
    owner = prog.cls.get(f["cls"], {}).get("name") if f["cls"] else None
    return f"{owner}::{f['name']}" if owner else f["name"]


def _loops_forever(prog, fid):
    """A body that keeps going until state tells it to stop: a thread's tick."""
    seen, todo = set(), [fid]
    while todo:
        x = todo.pop()
        if x in seen or x not in prog.fn:
            continue
        seen.add(x)
        if any(lp[3][0] == "open" for lp in prog.fn[x]["loops"]):
            return True
        if len(seen) > 1:
            continue
        todo.extend(prog.succ.get(x, ()))
    return False


def enumerate_entries(prog, api, graph):
    out = []
    for fid, f in sorted(prog.fn.items()):
        if f["foreign_over"]:
            names = {n for _, n in f["foreign_over"]}
            freq = "frame" if names & api.repeating else "input" if names & api.input else "event"
            out.append({"id": fid, "kind": "engine", "freq": freq, "why": [f"overrides {o}.{n}" for o, n in f["foreign_over"]]})
    for name, fid in prog.binds:
        out.append({"id": fid, "kind": "bound", "freq": "event", "why": [f"bound as '{name}'"]})
    for lid, (kind, owner, e) in sorted(prog.deferred().items()):
        freq = "tick" if kind == "thread" and _loops_forever(prog, lid) else kind
        out.append({"id": lid, "kind": kind, "freq": freq, "why": [f"{'started' if kind == 'thread' else 'kept'} at {prog.fn[owner]['file']}:{e[1]} by {display(prog, owner)}"]})
    merged = {}
    for e in out:
        known = merged.get(e["id"])
        if known is None:
            merged[e["id"]] = dict(e)
        else:
            known["why"] = sorted(set(known["why"]) | set(e["why"]))
    _scripts(merged, graph)
    for e in merged.values():
        e["name"] = display(prog, e["id"])
        f = prog.fn[e["id"]]
        e["file"], e["line"] = f["file"], f["line"]
    return [merged[k] for k in sorted(merged)]


def _scripts(merged, graph):
    """Script callbacks the engine runs, followed through scripts to the native entries they reach: a bound method a frame callback reaches runs
    every frame."""
    if graph is None:
        return
    rank = {"frame": 3, "input": 2, "event": 1}
    reach = collections.defaultdict(set)
    for uid, u in sorted(graph.units.items()):
        attrs = u["attrs"]
        if u["kind"] != "method" or not attrs.get("callback"):
            continue
        freq = "frame" if attrs.get("repeating") else "input" if attrs.get("input") else "event"
        seen, todo = {uid}, [(uid, (f"{u['file']}:{u['line']}",))]
        while todo:
            x, path = todo.pop()
            for e in graph.out.get(x, ()):
                if e.kind not in FLOW or e.generic or e.dst in seen:
                    continue
                seen.add(e.dst)
                step = (*path, f"{e.sites[0][0]}:{e.sites[0][1]}")
                if e.dst in merged:
                    reach[e.dst].add((freq, u["name"], step))
                elif not e.dst.startswith("cpp:"):
                    todo.append((e.dst, step))
    for fid, hits in reach.items():
        e = merged[fid]
        best = max(hits, key=lambda h: (rank[h[0]], h[1], h[2]))
        e["via"] = [{"script": h[1], "freq": h[0], "path": list(h[2])} for h in sorted(hits)][:8]
        if rank.get(best[0], 0) > rank.get(e["freq"], 0):
            e["freq"] = best[0]
