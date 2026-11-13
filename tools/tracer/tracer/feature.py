"""A functionality read off what it produces: given points of output (`file:line`, or the label of a function whose
outputs they are), everything that decides whether and how they happen (the backward slice: the functions on the way,
the conditions, the state read and who writes it, the constants and the data that enter) and what sets it off (the
roots and the way from each). Nothing is chosen by name. Its sheet: each output, its condition and the value of each
argument in the inputs of each root; the constants and data in them; the state read and written.
"""
from __future__ import annotations

from dataclasses import dataclass, field
from fractions import Fraction
from pathlib import Path

from .conditions import state_of
from .decide import render, satisfy, symbols
from .situations import lifted, lifted_value, reach
from .vocab import AT, CLASS, DATA, MODULE, NAMESPACE, OUTCALL, PARAM, SHAPE, SYMBOL, TEXT, WRITE

DEEP = 6                         # how many writers back the state a functionality reads is followed


@dataclass
class Sheet:
    outputs: list                # effect indices
    roots: list = field(default_factory=list)        # (root, why it runs, the way)
    functions: list = field(default_factory=list)    # labels on the way
    conditions: list = field(default_factory=list)   # where each condition is written
    reads: list = field(default_factory=list)        # state read (labels)
    writes: list = field(default_factory=list)       # state written (labels)
    writers: list = field(default_factory=list)      # (state, where it is written, by which function)
    constants: list = field(default_factory=list)    # the numbers and texts that enter
    data: list = field(default_factory=list)         # the data files and keys that enter
    rows: list = field(default_factory=list)         # (output, root, its condition, its arguments, an example)


def same_file(display: str, written: str) -> bool:
    """A file's path as the analysis shows it and as a point writes it name the same file: equal, or one the other's
    end at a folder's boundary (`roads/x.cpp` is not `myroads/x.cpp`)."""
    display, written = display.replace("\\", "/"), written.replace("\\", "/")
    return display == written or written.endswith("/" + display) or display.endswith("/" + written)


def points(result, wanted: list) -> list:
    """The effects at those points: `file:line`; a file alone (every output and write in it); or a function's label
    (its outputs and writes)."""
    from .languages import BY_EXTENSION
    p, out = result.program, []
    for want in wanted:
        path, _, line = want.rpartition(":")
        whole = not line.isdigit() and Path(want).suffix.lower() in BY_EXTENSION
        for i, e in enumerate(result.cases.effects):
            display = p.files[e.file].display
            if line.isdigit() and path and int(line) == e.line and same_file(display, path):
                out.append(i)
            elif whole and e.kind in (OUTCALL, WRITE) and same_file(display, want):
                out.append(i)
            elif not line.isdigit() and not whole and e.kind in (OUTCALL, WRITE) \
                    and (p.label(e.unit) == want or p.label(e.unit).endswith("." + want)):
                out.append(i)
    return list(dict.fromkeys(out))


def units_of(result, wanted: list) -> set:
    """The definitions of the functionality's slice: the functions its outputs are in and every one on the way to them
    (what its graph shows, not the whole of the roots)."""
    from .walk import backward
    effects = result.cases.effects
    units = list(dict.fromkeys(effects[i].unit for i in points(result, wanted)))
    return set(units) | {step.at for step in backward(result, units)}


def kept(p, d: int) -> bool:
    """Whether a definition is state a functionality reads or writes: a variable of a file, a namespace, a class or a
    record, not one of a function (a local carries a value on the way and is followed back, but is no state)."""
    it = p.defs[d]
    return it.kind != PARAM and it.owner >= 0 and p.defs[it.owner].kind in (CLASS, MODULE, NAMESPACE, SHAPE)


def gather(x, conditions: set, states: set, constants: set, data: set) -> None:
    if isinstance(x, Fraction):
        constants.add(str(x))
        return
    if not isinstance(x, tuple) or not x:
        return
    if x[0] == AT:
        conditions.add(x[1])
        gather(x[2], conditions, states, constants, data)
    elif x[0] == SYMBOL:
        if isinstance(x[1], int):
            states.add(x[1])
    elif x[0] == DATA:
        data.add(f"{x[1]} {list(x[2])}")
    elif x[0] != TEXT:
        for y in x[1:]:
            gather(y, conditions, states, constants, data)


def sheet(result, wanted: list) -> Sheet:
    from .gaps import path_text, triggers
    from .walk import backward
    p = result.program
    rows = points(result, wanted)
    s = Sheet(rows)
    effects = result.cases.effects
    units = list(dict.fromkeys(effects[i].unit for i in rows))
    behind = backward(result, units)
    s.functions = sorted({p.label(step.at) for step in behind})
    s.roots = [(root, why, path_text(result, path)) for root, why, path in triggers(result, behind)]
    conditions, states, constants, data = set(), set(), set(), set()
    todo, seen = [effects[i] for i in rows], set()
    for depth in range(DEEP):                    # the state read, and the writes of it, back
        found = set()
        for e in todo:
            for x in (e.guard, e.value) + tuple(e.args):
                gather(x, conditions, found, constants, data)
        new = {d for d in found if d not in seen and p.defs[d].kind != PARAM}
        states |= found
        seen |= new
        todo = [e for e in effects if e.kind == WRITE and state_of(e.target) in new]
        for e in todo:
            s.writers.append((p.label(state_of(e.target)), f"{p.files[e.file].display}:{e.line}", p.label(e.unit)))
        if not todo:
            break
    s.conditions = sorted(conditions)
    s.reads = sorted({p.label(d) for d in states if 0 <= d < len(p.defs) and kept(p, d)})
    s.writes = sorted({p.label(state_of(effects[i].target)) for i in rows if effects[i].kind == WRITE
                       and state_of(effects[i].target) is not None and kept(p, state_of(effects[i].target))})
    s.constants = sorted(constants, key=lambda c: Fraction(c))
    s.data = sorted(data)
    for root, _, _ in s.roots[:4]:
        reached = reach(result, [root])
        for i in rows:
            e = effects[i]
            if e.unit not in reached:
                continue
            when = lifted(result, [root], e.unit, e.guard, reached)
            args = [render(lifted_value(result, [root], e.unit, a, reached)) for a in e.args] or \
                ([render(lifted_value(result, [root], e.unit, e.value, reached))] if e.value is not None else [])
            case = satisfy(when)
            s.rows.append((i, p.label(root), render(when), args,
                           case.text() if case not in (None,) and not isinstance(case, str) else "it never happens"))
    return s


def text_report(result, wanted: list) -> str:
    from .situations import effect_text
    s = sheet(result, wanted)
    lines = [f"The functionality of {', '.join(wanted)}:"]
    lines.append("  Outputs:")
    lines += [f"    {effect_text(result, i)}" for i in s.outputs]
    lines.append("  Set off by:")
    lines += [f"    {result.program.where(root)} [{why}] {way}" for root, why, way in s.roots[:10]]
    lines.append(f"  Functions on the way ({len(s.functions)}): {', '.join(s.functions[:40])}")
    lines.append(f"  Conditions at: {', '.join(s.conditions[:40])}")
    lines.append(f"  State read: {', '.join(s.reads[:40])}")
    lines.append(f"  State written: {', '.join(s.writes[:40])}")
    lines += ["  Written by:"] + [f"    {state} at {where} (in {unit})" for state, where, unit in s.writers[:40]]
    lines.append(f"  Constants: {', '.join(s.constants[:40])}")
    lines.append(f"  Data: {', '.join(s.data[:20])}")
    lines.append("  Each output from each root (its condition, its values, a case):")
    for i, root, when, args, case in s.rows:
        lines.append(f"    {effect_text(result, i)}  from {root}")
        lines.append(f"        when {when}")
        lines.append(f"        values {', '.join(args)}")
        lines.append(f"        e.g. {case}")
    return "\n".join(lines)


def data_of(result, wanted: list, s: Sheet | None = None) -> dict:
    """The sheet as data (`s`: one already made of these points)."""
    from .situations import effect_text
    s = s if s is not None else sheet(result, wanted)
    return {"the outputs": [effect_text(result, i) for i in s.outputs],
            "set off by": [f"{result.program.where(root)} [{why}] {way}" for root, why, way in s.roots],
            "the functions": s.functions, "conditions at": s.conditions, "state read": s.reads,
            "state written": s.writes, "written by": [list(w) for w in s.writers], "the constants": s.constants,
            "the data": s.data,
            "each output": [{"the output": effect_text(result, i), "from the root": root, "when it happens": when, "the values": args,
                             "a case": case} for i, root, when, args, case in s.rows]}
