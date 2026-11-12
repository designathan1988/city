"""A trace: from the definitions a start names, the links followed forward (what runs) or backward (what reaches
it), each definition gone into once. A function is followed as the call that entered it gave: by text, to the
function whose name that call gave, where there is one; of the functions its callers give it, to the ones that call
gave; of what it reads out of an object written in place by a key, to what the entries of the keys that call wrote
hold. Back from a function, not to the callers that gave another's name, another function or another key. What was
found by name alone is followed one step (`NEAR`), not on from there; `DEEP` follows it on, `NONE` not at all. A
function handed to code not read is gone into where it is written in the code handing it (that code runs it there:
`INLINE`); one written elsewhere (a handler registered, a function kept for later) is shown and not gone into,
unless asked (`EVERY`): it is where another flow starts.
"""
from __future__ import annotations

from dataclasses import dataclass

from .sources import ends
from .vocab import BY_NAME, BY_TEXT, DEEP, EVERY, HANDED, INLINE, NEAR, NONE, RUNNING


@dataclass
class Step:
    depth: int
    link: object | None              # the link followed to get here (none at a start)
    at: int                          # the definition reached
    again: bool                      # shown above already: not gone into again
    leaf: bool = False               # handed to code not read and written elsewhere: shown, not gone into


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


def walk(starts: list, links: dict, end, depth: int | None, after, leaf=lambda link: False) -> list:
    """The steps of a walk from the starts along the links `after` keeps (given the link that led to a
    definition and the links out of it), to that depth (none: to the end); a link `leaf` says is shown, its end not
    gone into."""
    out, seen = [], set()
    todo = [(0, None, start) for start in reversed(starts)]
    while todo:
        level, link, at = todo.pop()
        if link is not None and leaf(link):
            out.append(Step(level, link, at, False, True))
            continue
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


def guessed(guesses: str):
    """Which links a trace keeps, given the link that led to the definition they start from."""
    def keep(link, one) -> bool:
        if one.how != BY_NAME or guesses == DEEP:
            return True
        return guesses == NEAR and (link is None or link.how != BY_NAME)
    return keep


def keys_of(result, unit: int, one, name) -> set | None:
    """The keys of the entries of an object written in place that held what a link reaches, where the unit read it
    out of such an object by a key not written there."""
    table = (result.tables or {}).get(unit)
    if not table:
        return None
    found = table.get(one.dst)
    if found is None and one.how == BY_TEXT:
        found = table.get(name(one.dst))
    return found


def handed_away(result, handed: str):
    """Whether a link hands over a function written elsewhere than in the code handing it."""
    p = result.program

    def away(link) -> bool:
        return handed != EVERY and link.kind == HANDED and link.src not in p.chain(p.defs[link.dst].owner)
    return away


def forward(result, starts: list, depth: int | None = None, keep=lambda link: True, guesses: str = NEAR,
            handed: str = INLINE) -> list:
    name = lambda d: result.program.defs[d].name                                         # noqa: E731
    near = guessed(guesses)

    def after(link, found) -> list:
        """Entered with names given: by text, only the functions of those names, where any is. Entered with
        functions given: not the ones only its other callers give it. Entered with texts written that are keys of
        an object it reads by a key: only what those keys' entries hold."""
        found = [one for one in found if keep(one) and near(link, one)]
        if link is None:
            return found
        told = [one for one in found if one.how == BY_TEXT and name(one.dst) in link.texts]
        if told:
            found = [one for one in found if one.how != BY_TEXT or one in told]
        given = link.words | link.texts
        if given and (result.tables or {}).get(link.dst):
            keyed = [(one, keys_of(result, link.dst, one, name)) for one in found]
            if any(keys & given for _, keys in keyed if keys):
                found = [one for one, keys in keyed if keys is None or keys & given]
        others = handed_to(result, link.dst) - link.fns if link.fns else ()
        return [one for one in found if one.dst not in others]
    return walk(starts, result.out, lambda link: link.dst, depth, after, handed_away(result, handed))


def backward(result, starts: list, depth: int | None = None, keep=lambda link: True, guesses: str = NEAR) -> list:
    name = lambda d: result.program.defs[d].name                                         # noqa: E731
    near = guessed(guesses)

    def after(link, found) -> list:
        """Reached from a function that is given it: its callers that gave it, or gave no function. Reached by text:
        not its callers that gave the name of another function it runs by text. Reached from what an entry of an
        object it reads by a key holds: its callers that wrote one of those keys, or wrote no text."""
        found = [one for one in found if keep(one) and near(link, one)]
        if link is None:
            return found
        keys = keys_of(result, link.src, link, name)
        if keys:
            found = [one for one in found if (one.words | one.texts) & keys or not (one.words | one.texts)]
        if link.dst in handed_to(result, link.src):          # a function its callers give it: the ones that gave it
            found = [one for one in found if link.dst in one.fns or not one.fns]
        if link.how != BY_TEXT:
            return found
        others = {name(one.dst) for one in result.out.get(link.src, ()) if one.how == BY_TEXT} - {name(link.dst)}
        return [one for one in found if name(link.dst) in one.texts or not one.texts & others]
    return walk(starts, result.into, lambda link: link.src, depth, after)
