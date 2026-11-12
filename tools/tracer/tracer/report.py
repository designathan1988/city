"""A trace as text and as data: each step its link's kind, the definition reached and where it is written, where the
link is written and whether it was found by name; under each definition, the callees it writes that no definition
answers.
"""
from __future__ import annotations

from .vocab import BY_NAME

INDENT = "  "
AGAIN = "(above)"
OUTSIDE = "outside the code read:"


def where(result, link) -> str:
    return f"{result.program.files[link.file].display}:{link.line}"


def row(result, step, at_end: bool) -> str:
    """One step: `kind label  file:line  @ where the link is written  [by name]  (above)`."""
    p = result.program
    words = [INDENT * step.depth + (f"{step.link.kind:<6} " if step.link else "") + p.label(step.at), p.where(step.at)]
    if step.link:
        words.append("@ " + where(result, step.link))
        if step.link.how == BY_NAME:
            words.append(f"[{BY_NAME}]")
    if step.again and at_end:
        words.append(AGAIN)
    return "  ".join(words)


def as_text(result, title: str, steps: list, outside: bool) -> str:
    lines = [title]
    for step in steps:
        lines.append(row(result, step, True))
        names = result.outside.get(step.at) if outside and not step.again else None
        if names:
            lines.append(INDENT * (step.depth + 1) + OUTSIDE + " " + ", ".join(names))
    return "\n".join(lines)


def as_data(result, steps: list) -> list:
    """The steps as rows a program reads."""
    p = result.program
    out = []
    for step in steps:
        one = {"depth": step.depth, "label": p.label(step.at), "at": p.where(step.at), "again": step.again}
        if step.link:
            one.update(kind=step.link.kind, how=step.link.how, written=where(result, step.link))
        if not step.again and result.outside.get(step.at):
            one["outside"] = result.outside[step.at]
        out.append(one)
    return out


def listing(result, kinds: tuple) -> str:
    """Every definition of those kinds: its label and where it is written."""
    p = result.program
    rows = sorted((p.label(d.id), p.where(d.id)) for d in p.defs if d.kind in kinds)
    return "\n".join(f"{label}  {at}" for label, at in rows)
