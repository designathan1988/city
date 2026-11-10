"""The trace written out: the starting point, the way back to the entry points, the way forward to the effects and
back, the state written and who reads it, and where data and scenes name what the trace passed through.
"""
from __future__ import annotations

import json

from .model import FUNCTION_KINDS
from .seeds import label
from .walk import readers

ARROW = {"call": "calls", "value": "hands over", "name": "by name", "emit": "emits", "signal": "fires handler",
         "lambda": "lambda", "new": "constructs", "override": "override", "thread": "runs on a thread"}


def where(ix, gid: int) -> str:
    unit = ix.units[gid]
    return f"{unit.file}:{unit.line}" if unit.line else unit.file


def site(edge) -> str:
    text = f"{edge.file}:{edge.line}"
    if edge.cond:
        text += "  when " + " > ".join(edge.cond)
    if edge.how not in ("exact",):
        text += f"  [{edge.how}]"
    return text


def row_text(ix, row, forward: bool) -> str:
    pad = "  " * row.depth
    name = label(ix, row.gid)
    if row.status == "returns":
        return f"{pad}<- returns {row.note}"
    if row.status == "writes":
        return f"{pad}writes {name}  ({where(ix, row.gid)})  at {site(row.edge)}"
    if row.status == "external":
        return f"{pad}engine/std: {row.note}"
    lead = ""
    if row.edge is not None:
        verb = ARROW.get(row.edge.kind, row.edge.kind)
        lead = f"--{verb}--> " if forward else f"<--{verb}-- "
    head = f"{pad}{lead}#{row.number} {name}  ({where(ix, row.gid)})" if row.number else f"{pad}{lead}{name}"
    if row.edge is not None:
        head += f"  @ {site(row.edge)}"
    if row.status == "seen":
        head += "  (opened above)"
    elif row.status == "cut":
        head += "  (depth limit)"
    elif row.status == "entry":
        head += f"  ENTRY: {row.note}"
    elif row.note:
        head += f"  ({row.note})"
    return head


def mentions(ix, gids: set) -> list[str]:
    out = []
    names = sorted({ix.units[g].name for g in gids if ix.units[g].kind in FUNCTION_KINDS | {"enumerator", "signal"}})
    for name in names:
        for file, line in ix.mentions.get(name, []):
            out.append(f"  '{name}'  {file}:{line}")
    return out


def text(ix, rs, spec: str, seeds: list[int], up, down) -> str:
    lines = [f"Feature trace from: {spec}", "", "Starting point:"]
    lines += [f"  {label(ix, g)}  ({where(ix, g)})" for g in seeds]
    lines += ["", "== Back: who reaches it, up to the entry points ==", ""]
    lines += [row_text(ix, r, False) for r in up.rows]
    lines += ["", "== Forward: what it reaches, down to its effects and back ==", ""]
    lines += [row_text(ix, r, True) for r in down.rows]
    lines += ["", "== State written on the way, and who reads it ==", ""]
    for state, writers, outside, inside in readers(rs, down.writes, down.reached):
        lines.append(f"  {label(ix, state)}  ({where(ix, state)})")
        lines += [f"    written by {label(ix, w)} @ {site(e)}" for w, e in writers]
        lines += [f"    read outside the trace by {label(ix, e.src)} @ {site(e)}" for e in outside]
        if inside:
            lines.append(f"    read inside the trace by {len(inside)} definition(s)")
    named = mentions(ix, up.reached | down.reached)
    if named:
        lines += ["", "== Named by data, scene and project files ==", ""] + named
    unreached = sorted({f"{label(ix, g)}: {name}" for g in down.reached for name in rs.unreached.get(g, ())})
    lines += ["", "== Totals ==",
              f"  entry points: {len(up.entries)}; definitions behind: {len(up.reached)}; ahead: {len(down.reached)}",
              f"  uses on a receiver no value of the code reaches: {len(unreached)}",
              *(f"    {u}" for u in unreached),
              f"  state written: {len(down.writes)}"]
    return "\n".join(lines) + "\n"


def to_json(ix, rs, spec: str, seeds: list[int], up, down) -> str:
    def unit(g):
        u = ix.units[g]
        return {"id": g, "name": label(ix, g), "kind": u.kind, "file": u.file, "line": u.line}

    def rows(walk):
        return [{"depth": r.depth, "unit": g_or_none(r), "status": r.status, "note": r.note,
                 "edge": None if r.edge is None else {"kind": r.edge.kind, "file": r.edge.file, "line": r.edge.line,
                                                      "when": list(r.edge.cond), "how": r.edge.how}}
                for r in walk.rows]

    def g_or_none(r):
        return unit(r.gid) if r.status != "returns" else {"id": r.gid}

    state = [{"state": unit(s), "writers": [unit(w) for w, _ in ws], "readers_outside": [unit(e.src) for e in out]}
             for s, ws, out, _ in readers(rs, down.writes, down.reached)]
    return json.dumps({"from": spec, "seeds": [unit(g) for g in seeds], "back": rows(up), "forward": rows(down),
                       "entries": [unit(g) for g in up.entries], "state": state}, indent=1)
