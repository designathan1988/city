"""A trace as text and as data: each step its link's kind, the definition reached and where it is written, where the
link is written and whether it was found by name; under each definition, the callees it writes that no definition
answers, and the state it writes or may change (with the class or file that owns it) and, asked, the state it reads.
Asked, the state the whole trace writes, its owner, and who reads it: straight, or through a record such a reader
builds and gives back.
"""
from __future__ import annotations

from .vocab import AMBIGUOUS, CHANGES, CLASS, EXACT, MODULE, NAMESPACE, READS, SHAPE, TOUCHES, WRITES

INDENT = "  "
AGAIN = "(above)"
LEAF = "(handed over, not followed)"
OUTSIDE = "outside the code read:"
FOUND_BY_NAME = AMBIGUOUS + " (functions of the name):"
STATE = "State it writes, its owner, and who reads it:"
THROUGH = "through the record built by"


def is_state(result, d: int) -> bool:
    p = result.program
    it = p.defs[d]
    return it.owner >= 0 and p.defs[it.owner].kind in (CLASS, MODULE, NAMESPACE)


def owner_of(result, d: int) -> str:
    p = result.program
    return p.label(p.defs[d].owner)


def touches(result, unit: int, kinds) -> list:
    """The state a unit does those things to, as (definition, what), by label."""
    p = result.program
    found = [(d, how) for d, how in (result.state_out or {}).get(unit, {}).items()
             if how in kinds and is_state(result, d)]
    return sorted(found, key=lambda one: (TOUCHES.index(one[1]), p.label(one[0])))


def written(result, units) -> set:
    """The state those units write or may change."""
    return {d for u in units for d, _ in touches(result, u, (WRITES, CHANGES))}


def readers_of(result, d: int) -> list:
    """Who reads a definition: (unit, None) straight; (unit, builder) through a record `builder`, itself a reader,
    builds and gives back."""
    p = result.program
    if result.built_by is None:
        result.built_by = {}
        for one in p.defs:
            if one.kind == SHAPE and one.owner >= 0:
                result.built_by.setdefault(one.owner, []).append(one.id)
    state_in = result.state_in or {}
    out = []
    for unit, how in sorted(state_in.get(d, {}).items()):
        if how not in (READS, CHANGES):
            continue
        out.append((unit, None))
        for c in result.built_by.get(unit, ()):
            for entries in p.names.get(c, {}).values():
                for e in entries:
                    for other, how2 in sorted(state_in.get(e, {}).items()):
                        if how2 == READS and other != unit:
                            out.append((other, unit))
    return list(dict.fromkeys(out))


def state_lines(result, unit: int, depth: int, reads: bool) -> list:
    p = result.program
    out = []
    for kinds in ((WRITES, CHANGES), (READS,)) if reads else ((WRITES, CHANGES),):
        found = touches(result, unit, kinds)
        if found:
            words = ", ".join(f"{how} {p.label(d)} ({owner_of(result, d)})" for d, how in found)
            out.append(INDENT * (depth + 1) + "state: " + words)
    return out


def where(result, link) -> str:
    return f"{result.program.files[link.file].display}:{link.line}"


def row(result, step, at_end: bool) -> str:
    """One step: `kind label  file:line  @ where the link is written  [by name]  (above)`."""
    p = result.program
    words = [INDENT * step.depth + (f"{step.link.kind:<6} " if step.link else "") + p.label(step.at), p.where(step.at)]
    if step.link:
        words.append("@ " + where(result, step.link))
        if step.link.how != EXACT:
            words.append(f"[{step.link.how}]")
    if step.again and at_end:
        words.append(AGAIN)
    if step.leaf:
        words.append(LEAF)
    return "  ".join(words)


def as_text(result, title: str, steps: list, outside: bool, state: bool = False) -> str:
    lines = [title]
    for step in steps:
        lines.append(row(result, step, True))
        if outside and not step.again and not step.leaf:
            lines += state_lines(result, step.at, step.depth, state)
        names = result.outside.get(step.at) if outside and not step.again and not step.leaf else None
        if names:
            lines.append(INDENT * (step.depth + 1) + OUTSIDE + " " + ", ".join(names))
        many = (result.ambiguous or {}).get(step.at) if outside and not step.again and not step.leaf else None
        if many:
            lines.append(INDENT * (step.depth + 1) + FOUND_BY_NAME + " " +
                         ", ".join(f"{shown} ({count})" for shown, count in many))
    return "\n".join(lines)


def as_data(result, steps: list) -> list:
    """The steps as rows a program reads."""
    p = result.program
    out = []
    for step in steps:
        one = {"depth": step.depth, "label": p.label(step.at), "at": p.where(step.at), "again": step.again}
        if step.leaf:
            one[LEAF.strip("()")] = True
        if step.link:
            one.update(kind=step.link.kind, how=step.link.how, written=where(result, step.link))
        if not step.again and result.outside.get(step.at):
            one["outside"] = result.outside[step.at]
        if not step.again and (result.ambiguous or {}).get(step.at):
            one[AMBIGUOUS] = [list(pair) for pair in result.ambiguous[step.at]]
        if not step.again and not step.leaf:
            for how in TOUCHES:
                found = touches(result, step.at, (how,))
                if found:
                    one[how] = [{"label": p.label(d), "owner": owner_of(result, d), "at": p.where(d)} for d, _ in found]
        out.append(one)
    return out


def state_text(result, steps: list) -> str:
    """The state the steps write or may change, its owner, and who reads it."""
    p = result.program
    lines = [STATE]
    for d in sorted(written(result, [s.at for s in steps if not s.again and not s.leaf]), key=p.label):
        lines.append(f"{INDENT}{p.label(d)}  {p.where(d)}  owner {owner_of(result, d)}")
        for unit, builder in readers_of(result, d):
            via = f"  ({THROUGH} {p.label(builder)})" if builder is not None else ""
            lines.append(f"{INDENT * 2}{READS} {p.label(unit)}  {p.where(unit)}{via}")
    return "\n".join(lines)


def state_data(result, steps: list) -> list:
    p = result.program
    return [{"label": p.label(d), "at": p.where(d), "owner": owner_of(result, d),
             READS: [{"label": p.label(u), "at": p.where(u), THROUGH: p.label(b) if b is not None else None}
                     for u, b in readers_of(result, d)]}
            for d in sorted(written(result, [s.at for s in steps if not s.again and not s.leaf]), key=p.label)]


def listing(result, kinds: tuple) -> str:
    """Every definition of those kinds: its label and where it is written."""
    p = result.program
    rows = sorted((p.label(d.id), p.where(d.id)) for d in p.defs if d.kind in kinds)
    return "\n".join(f"{label}  {at}" for label, at in rows)
