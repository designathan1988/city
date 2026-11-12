"""A trace: from the definitions a start names, the links followed forward (what runs) or backward (what reaches
it), each definition gone into once. A function is followed as the call that entered it gave: by text, to the
function whose name that call gave, where there is one; and, of the functions its callers give it, to the ones that
call gave. Back from a function, not to the callers that gave another's name or another function.
"""
from __future__ import annotations

from dataclasses import dataclass

from .sources import ends
from .vocab import BY_TEXT, RUNNING


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


def walk(starts: list, links: dict, end, depth: int | None, after) -> list:
    """The steps of a walk from the starts along the links `after` keeps (given the link that led to a
    definition and the links out of it), to that depth (none: to the end)."""
    out, seen = [], set()
    todo = [(0, None, start) for start in reversed(starts)]
    while todo:
        level, link, at = todo.pop()
        again = at in seen
        out.append(Step(level, link, at, again))
        if again or (depth is not None and level >= depth):
            continue
        seen.add(at)
        for one in reversed(after(link, links.get(at, ()))):
            todo.append((level + 1, one, end(one)))
    return out


def handed_to(result, d: int) -> frozenset:
    """Every function the callers of a definition give it."""
    return frozenset().union(*(one.fns for one in result.into.get(d, ())))


def forward(result, starts: list, depth: int | None = None, keep=lambda link: True) -> list:
    name = lambda d: result.program.defs[d].name                                         # noqa: E731

    def after(link, found) -> list:
        """Entered with names given: by text, only the functions of those names, where any is. Entered with
        functions given: not the ones only its other callers give it."""
        found = [one for one in found if keep(one)]
        if link is None:
            return found
        told = [one for one in found if one.how == BY_TEXT and name(one.dst) in link.texts]
        if told:
            found = [one for one in found if one.how != BY_TEXT or one in told]
        others = handed_to(result, link.dst) - link.fns if link.fns else ()
        return [one for one in found if one.dst not in others]
    return walk(starts, result.out, lambda link: link.dst, depth, after)


def backward(result, starts: list, depth: int | None = None, keep=lambda link: True) -> list:
    name = lambda d: result.program.defs[d].name                                         # noqa: E731

    def after(link, found) -> list:
        """Reached from a function that is given it: its callers that gave it, or gave no function. Reached by text:
        not its callers that gave the name of another function it runs by text."""
        found = [one for one in found if keep(one)]
        if link is None:
            return found
        if link.dst in handed_to(result, link.src):          # a function its callers give it: the ones that gave it
            found = [one for one in found if link.dst in one.fns or not one.fns]
        if link.how != BY_TEXT:
            return found
        others = {name(one.dst) for one in result.out.get(link.src, ()) if one.how == BY_TEXT} - {name(link.dst)}
        return [one for one in found if name(link.dst) in one.texts or not one.texts & others]
    return walk(starts, result.into, lambda link: link.src, depth, after)
