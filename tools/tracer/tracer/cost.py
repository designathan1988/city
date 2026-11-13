"""What one run of a root costs, read off the code: every loop it runs (in its own code or in the functions it calls),
what the loop goes over and whether that grows with the inputs, the state or the data (else it is fixed); a loop
inside a loop (written inside it, or in a function called from inside it); what is made at each turn (an object, a
collection); a call of code not read at each turn (what it does is not read: its own cost is unknown, said). The order
of each: constant, linear, quadratic, ... in the loops that grow around it, with `file:line`.
"""
from __future__ import annotations

from dataclasses import dataclass, field

from .situations import reach
from .vocab import OUTCALL

from .vocab import ORDERS  # noqa: E402


@dataclass
class Item:
    where: str
    what: str                    # a loop, made at each turn, code not read at each turn
    shown: str
    loops: list = field(default_factory=list)        # the loops around it, outermost first: (where, grows)
    order: str = ""
    note: str = ""
    units: tuple = ()            # the functions it and the loops around it are in (what a feature's cost keeps)


def order_of(loops: list) -> str:
    n = sum(1 for _, grows in loops if grows)
    return ORDERS[n] if n < len(ORDERS) else f"n^{n}"


def report(result, roots: list) -> list:
    p, cases = result.program, result.cases
    reached = reach(result, roots)
    turnings = [t for t in cases.loops if t.unit in reached]
    by_unit = {}
    for t in turnings:
        by_unit.setdefault(t.unit, []).append(t)

    def around_in(unit, file, start) -> list:
        """The loops of a function around a point of it, outermost first."""
        return sorted((t for t in by_unit.get(unit, ()) if t.file == file and t.body[0] <= start < t.body[1]),
                      key=lambda t: t.start)

    known = {}

    def outside(unit, seen=frozenset()) -> list:
        """The loops a function is called from inside (by its callers on the way from the roots), outermost first."""
        if unit in roots or unit in seen:
            return []
        if unit in known:
            return known[unit]
        known[unit] = []                         # (a call back into it while it is asked: none more)
        best = []
        for (src, dst), rows in cases.passages.items():
            if dst != unit or src not in reached:
                continue
            for row in rows:
                here = outside(src, seen | {unit}) + around_in(src, row.file, row.start)
                if sum(t.grows for t in here) > sum(t.grows for t in best):
                    best = here
        known[unit] = best
        return best

    def where(t) -> str:
        return f"{p.files[t.file].display}:{t.line}"
    out = []
    for t in turnings:
        loops = outside(t.unit) + around_in(t.unit, t.file, t.start)
        mine = loops + [t]
        out.append(Item(where(t), "a loop", f"over {t.over}" + (" (grows)" if t.grows else " (fixed)"),
                        [(where(k), k.grows) for k in mine], order_of([(where(k), k.grows) for k in mine]),
                        f"{t.turns} turns" if t.turns is not None else "", tuple({k.unit for k in mine})))
    for unit, file, line, start, shown in cases.makes:
        if unit not in reached:
            continue
        loops = outside(unit) + around_in(unit, file, start)
        if loops:
            pairs = [(where(k), k.grows) for k in loops]
            out.append(Item(f"{p.files[file].display}:{line}", "made at each turn", shown, pairs, order_of(pairs),
                            "a new object or collection each turn", tuple({unit} | {k.unit for k in loops})))
    for unit, file, line, shown, _ in result.unresolved or ():     # every call no definition answers
        if unit not in reached:
            continue
        loops = outside(unit) + sorted((t for t in by_unit.get(unit, ()) if t.file == file and t.lines
                                        and t.lines[0] <= line <= t.lines[1]), key=lambda t: t.start)
        if loops:
            pairs = [(where(k), k.grows) for k in loops]
            out.append(Item(f"{p.files[file].display}:{line}", "code not read at each turn", shown, pairs,
                            order_of(pairs), "what it does is not read: its own cost is not known (if it goes "
                                             "over a collection, one order more)", tuple({unit} | {k.unit for k in loops})))
    return out


def text_report(result, roots: list) -> str:
    found = report(result, roots)
    labels = ", ".join(result.program.label(r) for r in roots)
    lines = [f"Cost of one run of {labels} ({len(found)}):"]
    for item in sorted(found, key=lambda i: (-ORDERS.index(i.order) if i.order in ORDERS else -9, i.where)):
        lines.append(f"  {item.order:9}  {item.where}  {item.what}: {item.shown}" +
                     (f"  ({item.note})" if item.note else ""))
        if len(item.loops) > 1:
            lines.append("             inside " + " > ".join(f"{w}{' (grows)' if g else ''}" for w, g in item.loops))
    return "\n".join(lines)


def data(result, roots: list, among: set | None = None) -> list:
    """The cost as data; with `among` (definition ids: a feature's slice), only what is in those functions or inside
    a loop of theirs."""
    return [{"where it is": i.where, "what is found": i.what, "as written": i.shown, "the order": i.order,
             "inside the loops": [{"where it is": w, "it grows": g} for w, g in i.loops], "the note": i.note}
            for i in report(result, roots) if among is None or among.intersection(i.units)]
