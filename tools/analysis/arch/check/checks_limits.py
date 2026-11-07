"""Checks of what the program must not do: a trace that gives back the whole program, a parameter of an algorithm that moves the result."""
from .. import atoms, groups, layers

WHOLE = 0.5
SAMPLE = 32


def explodes(sizes, total):
    """A trace is useless when it brings most of the program with it. No trace at all proves nothing either."""
    return not sizes or any(s >= total * WHOLE for s in sizes)


def closure(ctx):
    """The rings of features of every size: what a trace gives must stay far from the whole, even when all that is reachable is nearly all of it."""
    run, a = ctx.plain()
    graph = run.graph
    files = {u["file"] for u in graph.units.values()}
    spread = sorted((g for g in a["groups"] if len({graph.units[m]["file"] for m in g["members"]}) > 1), key=lambda g: (-len(g["members"]), g["id"]))
    rows = []
    for g in spread[::max(1, len(spread) // SAMPLE)]:
        r = layers.rings(graph, a["flow"], a["idom"], g["members"])
        trace = set(g["members"]) | set(r["own"]) | set(r["executed"]) | set(r["data"]) | set(r["link"])
        rows.append((len({graph.units[t]["file"] for t in trace if t in graph.units}), len(trace), r["reachable"]))
    sizes = [r[0] for r in rows]
    return {"pass": not explodes(sizes, len(files)), "detail": {"traces": len(rows), "files": len(files), "largest_trace_files": max(sizes, default=0), "median_trace_files": sorted(sizes)[len(sizes) // 2] if sizes else 0,
                                                                "largest_reachable_units": max((r[2] for r in rows), default=0), "units": len(graph.units)}}


def _partition(found):
    return sorted(sorted(g["members"]) for g in found)


def inert(ctx):
    """R3: what is left of the parameters of an algorithm, moved. Only the tree of the map reads it, and every mark follows from the map."""
    _, plain = ctx.plain()
    base = _partition(plain["groups"])
    out = {}
    for value in (atoms.PRECISION - 1, atoms.PRECISION + 1):
        old = atoms.PRECISION
        atoms.PRECISION = value
        try:
            found = groups.discover(plain["atoms"])[0]
        finally:
            atoms.PRECISION = old
        out[f"PRECISION={value}"] = _partition(found) == base
    return {"pass": bool(out) and all(out.values()), "detail": out}
