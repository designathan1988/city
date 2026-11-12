"""A trace: from the definitions a start names, the links followed forward (what runs) or backward (what reaches
it), each definition gone into once.
"""
from __future__ import annotations

from dataclasses import dataclass

from .sources import ends
from .vocab import RUNNING


@dataclass
class Step:
    depth: int
    link: object | None              # the link followed to get here (none at a start)
    at: int                          # the definition reached
    again: bool                      # shown above already: not gone into again


def find(result, start: str) -> list:
    """The definitions a start names: `path:line` (the innermost one written there), a label, or a label's end."""
    p = result.program
    running = [d for d in p.defs if d.kind in RUNNING]
    path, _, line = start.rpartition(":")
    if path and line.isdigit():
        path = path.replace("\\", "/")
        inside = [d for d in running
                  if (ends(p.files[d.file].display, path) or ends(path, p.files[d.file].display))
                  and d.node.start_point[0] < int(line) <= d.node.end_point[0] + 1]
        return [min(inside, key=lambda d: d.node.end_byte - d.node.start_byte).id] if inside else []
    found = [d.id for d in running if p.label(d.id) == start] or \
        [d.id for d in running if p.label(d.id).endswith("." + start)]
    return p.bodies(found)


def walk(starts: list, links: dict, end, depth: int | None, keep) -> list:
    """The steps of a walk from the starts along the links kept, to that depth (none: to the end)."""
    out, seen = [], set()
    todo = [(0, None, start) for start in reversed(starts)]
    while todo:
        level, link, at = todo.pop()
        again = at in seen
        out.append(Step(level, link, at, again))
        if again or (depth is not None and level >= depth):
            continue
        seen.add(at)
        for one in reversed([one for one in links.get(at, ()) if keep(one)]):
            todo.append((level + 1, one, end(one)))
    return out


def forward(result, starts: list, depth: int | None = None, keep=lambda link: True) -> list:
    return walk(starts, result.out, lambda link: link.dst, depth, keep)


def backward(result, starts: list, depth: int | None = None, keep=lambda link: True) -> list:
    return walk(starts, result.into, lambda link: link.src, depth, keep)
