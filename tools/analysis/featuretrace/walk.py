"""The walk: from the starting point down every link to the effects and back (forward), and up every link to the
entry points the engine or the player fire (back). Each definition is opened once; a second visit points to the first.

Forward follows what runs: calls, names carried as strings, emits and the handlers connected to them, constructors,
overrides, threads, and callables handed to the engine or the library. Writes to state are shown where they happen; who reads that state is
listed after the walk. The walk returns (the `return` values) are shown where each definition ends.
"""
from __future__ import annotations

from dataclasses import dataclass, field

from .model import FUNCTION_KINDS

# What runs: a function or a lambda used as a value does not run where it is written; it runs where it is called
# ("call"), where a signal fires it ("signal"), or in the engine or library function it was handed to ("handed").
RUNS = frozenset({"call", "name", "emit", "signal", "new", "override", "thread", "handed"})


@dataclass
class Row:
    depth: int
    gid: int
    edge: object = None      # the edge that led here (None for a seed)
    status: str = "open"     # open | seen | entry | returns | writes | external | cut
    note: str = ""
    number: int = 0


@dataclass
class Walk:
    rows: list = field(default_factory=list)
    reached: set = field(default_factory=set)
    entries: list = field(default_factory=list)
    writes: dict = field(default_factory=dict)   # state gid -> [(writer gid, edge)]


def keep(edge) -> bool:
    return edge.kind in RUNS


def ordered(edges, side: str) -> list:
    """One edge per (other end, kind), in the order they are written; an override is not shown again for an end the
    same definition already reaches by another link that runs (the graph keeps both)."""
    seen, out = set(), []
    direct = {e.dst if side == "dst" else e.src for e in edges if e.kind != "override"}
    for edge in sorted(edges, key=lambda e: (e.file, e.line)):
        other = edge.dst if side == "dst" else edge.src
        if (other, edge.kind) in seen or (edge.kind == "override" and other in direct):
            continue
        seen.add((other, edge.kind))
        out.append(edge)
    return out


def forward(rs, seeds: list[int], max_depth: int | None = None) -> Walk:
    ix, walk = rs.ix, Walk()
    numbers = {}
    stack = [("visit", gid, 0, None) for gid in reversed(seeds)]
    while stack:
        action, gid, depth, edge = stack.pop()
        if action == "returns":
            for line, text, _ in ix.units[gid].returns:
                walk.rows.append(Row(depth, gid, None, "returns", f"{text}  (line {line})"))
            continue
        if gid in numbers:
            walk.rows.append(Row(depth, gid, edge, "seen", number=numbers[gid]))
            continue
        if max_depth is not None and depth > max_depth:
            walk.rows.append(Row(depth, gid, edge, "cut"))
            continue
        numbers[gid] = len(numbers) + 1
        walk.reached.add(gid)
        walk.rows.append(Row(depth, gid, edge, "open", number=numbers[gid]))
        outs = rs.out.get(gid, [])
        for e in ordered([e for e in outs if e.kind == "write"], "dst"):
            walk.writes.setdefault(e.dst, []).append((gid, e))
            walk.rows.append(Row(depth + 1, e.dst, e, "writes"))
        if rs.external.get(gid):
            walk.rows.append(Row(depth + 1, gid, None, "external", ", ".join(sorted(rs.external[gid]))))
        stack.append(("returns", gid, depth + 1, None))
        for e in reversed(ordered([e for e in outs if keep(e)], "dst")):
            stack.append(("visit", e.dst, depth + 1, e))
    return walk


def entry_reason(rs, gid: int) -> str:
    ix = rs.ix
    unit = ix.units[gid]
    if unit.kind == "engine_signal":
        return "the engine emits this signal"
    if ix.is_engine_callback(unit):
        return "the engine calls this callback"
    if unit.bound_as:
        return f"bound for the engine as {', '.join(sorted(set(unit.bound_as)))}; no caller in the code"
    if unit.lang == "js" and unit.name == "<module>":
        return "a module's top-level code: runs when it is first imported, or as an entry of the page or bundler"
    if unit.lang == "js" and unit.name == "<static>":
        return "runs when its class is evaluated"
    cls = ix.classes.get(unit.owner)
    if unit.lang == "js" and cls is not None and len(ix.base_keys(cls)) < len(cls.bases):
        return "a member of a class whose base the code read does not define: the library may call it"
    return "no caller in the code"


def back(rs, seeds: list[int], max_depth: int | None = None) -> Walk:
    ix, walk = rs.ix, Walk()
    numbers = {}
    stack = [(gid, 0, None) for gid in reversed(seeds)]
    while stack:
        gid, depth, edge = stack.pop()
        if gid in numbers:
            walk.rows.append(Row(depth, gid, edge, "seen", number=numbers[gid]))
            continue
        if max_depth is not None and depth > max_depth:
            walk.rows.append(Row(depth, gid, edge, "cut"))
            continue
        numbers[gid] = len(numbers) + 1
        walk.reached.add(gid)
        callers = ordered([e for e in rs.inn.get(gid, []) if keep(e)], "src")
        unit = ix.units[gid]
        is_entry = not callers or unit.kind == "engine_signal" or ix.is_engine_callback(unit)
        note = ""
        if unit.kind in FUNCTION_KINDS:
            sites = [e for e in rs.inn.get(gid, []) if e.kind == "connect"]
            if sites:
                note = "connected at " + ", ".join(f"{e.file}:{e.line}" for e in sites[:3])
        if is_entry:
            walk.entries.append(gid)
            walk.rows.append(Row(depth, gid, edge, "entry", entry_reason(rs, gid) + ("; " + note if note else ""),
                                 numbers[gid]))
            if not callers:
                continue
        else:
            walk.rows.append(Row(depth, gid, edge, "open", note, numbers[gid]))
        for e in reversed(callers):
            stack.append((e.src, depth + 1, e))
    return walk


def readers(rs, writes: dict, inside: set) -> list:
    """For each piece of state written on the way: who else reads it (the consumers) and who reads it inside."""
    out = []
    for state, writers in writes.items():
        reading = ordered([e for e in rs.inn.get(state, []) if e.kind == "read"], "src")
        out.append((state, writers, [e for e in reading if e.src not in inside], [e for e in reading if e.src in inside]))
    return sorted(out, key=lambda item: (rs.ix.units[item[0]].file, rs.ix.units[item[0]].line))
