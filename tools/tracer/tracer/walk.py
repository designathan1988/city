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
from .vocab import BY_NAME, BY_TEXT, DEEP, EVERY, FUNCTION, HANDED, INLINE, NEAR, NONE, RUNNING


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


def walk(starts: list, links: dict, end, depth: int | None, after, leaf=lambda link: False,
         context=lambda before, link: None, key=lambda ctx: ctx) -> list:
    """The steps of a walk from the starts along the links `after` keeps (given the link that led to a definition,
    the links out of it and the context it was reached in), to that depth (none: to the end); a link `leaf` says is
    shown, its end not gone into. The context is carried along the way (`context(the one before, the link)`), and a
    definition is gone into once for each `key` of it (what `after` keeps depends on that), so a second way in with
    another context is followed too."""
    out, seen = [], set()
    todo = [(0, None, start, None) for start in reversed(starts)]
    while todo:
        level, link, at, ctx = todo.pop()
        if link is not None and leaf(link):
            out.append(Step(level, link, at, False, True))
            continue
        visit = (at, key(ctx))
        again = visit in seen
        out.append(Step(level, link, at, again))
        if again or (depth is not None and level >= depth):
            continue
        seen.add(visit)
        for one in reversed(after(link, links.get(at, ()), ctx)):
            todo.append((level + 1, one, end(one), context(ctx, one)))
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


def written_in(result, d: int) -> list:
    """The functions a function is written inside (innermost first)."""
    p = result.program
    return [c for c in p.chain(d)[1:] if p.defs[c].kind == FUNCTION]


def forward(result, starts: list, depth: int | None = None, keep=lambda link: True, guesses: str = NEAR,
            handed: str = INLINE) -> list:
    """Forward, each step in the context of the way it was reached: the functions the calls on the way gave (a
    function given by one caller is not run for another that gave something else, at any depth), the keys and texts
    the call gave, and the functions passed through (a function written inside another is reached only through it)."""
    name = lambda d: result.program.defs[d].name                                         # noqa: E731
    near = guessed(guesses)
    reached, todo = set(starts), list(starts)    # what the starts reach at all: a function written inside one
    while todo:                                  # of them is reached only where that one is
        at = todo.pop()
        for one in result.out.get(at, ()):
            if one.dst not in reached:
                reached.add(one.dst)
                todo.append(one.dst)

    def after(link, found, ctx) -> list:
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
        fns, path, _ = ctx
        others = handed_to(result, link.dst) - fns
        found = [one for one in found if one.dst not in others]
        return [one for one in found if all(c in reached for c in written_in(result, one.dst))]

    def context(before, link):
        """What the next step is in: the functions the call gives (those it was given itself, only where they are
        the ones its way gave), and the functions passed through."""
        fns, path, _ = before if before is not None else (None, frozenset(starts), None)
        received = handed_to(result, link.src)
        gives = frozenset(g for g in link.fns if fns is None or g not in received or g in fns)
        words = link.words | link.texts if (result.tables or {}).get(link.dst) else frozenset()
        return gives, path | {link.src, link.dst}, words

    def key(ctx):
        return (ctx[0], ctx[2]) if ctx is not None else None
    return walk(starts, result.out, lambda link: link.dst, depth, after, handed_away(result, handed), context, key)


def backward(result, starts: list, depth: int | None = None, keep=lambda link: True, guesses: str = NEAR) -> list:
    """Backward, each step in the context of the way it was reached: a function received as an argument is followed
    back only to the callers that gave it, at any depth; by text, not to the callers that gave the name of another
    function; from an entry read by a key, to the callers that wrote that key."""
    name = lambda d: result.program.defs[d].name                                         # noqa: E731
    near = guessed(guesses)

    def after(link, found, ctx) -> list:
        found = [one for one in found if keep(one) and near(link, one)]
        if link is None:
            return found
        keys = keys_of(result, link.src, link, name)
        if keys:
            found = [one for one in found if (one.words | one.texts) & keys or not (one.words | one.texts)]
        carried = ctx or frozenset()
        if carried:                              # functions it was given: only the callers that gave them
            found = [one for one in found if carried <= one.fns]
        if link.how != BY_TEXT:
            return found
        others = {name(one.dst) for one in result.out.get(link.src, ()) if one.how == BY_TEXT} - {name(link.dst)}
        return [one for one in found if name(link.dst) in one.texts or not one.texts & others]

    def context(before, link):
        """The functions carried back: the one reached, and the ones carried, where the caller was given them."""
        received = handed_to(result, link.src)
        return frozenset(g for g in (before or frozenset()) | {link.dst} if g in received)
    return walk(starts, result.into, lambda link: link.src, depth, after, context=context)
