"""The texts a key position can have, read from the code around it (log/trace-research.md F13, F33): a string is a
value of its own text in the heap (`by_key.load_by_key`), and where a string names a member — a computed key `o[k]`,
a write `o[k] = v`, an argument a property-key type takes — what its expression says is known before any value
arrives: a string literal its text; a union, a narrowing either side; a name, the values its declarations give
(a local's assignments, an enclosing function's for a captured name, a module constant's initializers), when every one
of them is such a text. Anything else (a computed string, a parameter) is no known text: every member, as before.
"""
from __future__ import annotations

from ..model import STATE_KINDS
from ..vocab import CALL, CONSTRUCT, NAME, NARROW, STRNAME, UNION
from . import names


def of(an, tree, unit, seen=frozenset()) -> frozenset | None:
    """The texts an expression read in `unit` can have; None where they are not all known."""
    if not isinstance(tree, tuple) or not tree:
        return None
    kind = tree[0]
    if kind == STRNAME:
        return frozenset((tree[1],))
    if kind == UNION:
        left, right = of(an, tree[1], unit, seen), of(an, tree[2], unit, seen)
        return left | right if left is not None and right is not None else None
    if kind == NARROW:
        return of(an, tree[1], unit, seen)
    if kind == NAME:
        return named(an, tree[1], unit, seen)
    return None


def named(an, name: str, unit, seen) -> frozenset | None:
    key = (unit.gid, name)
    if key in seen:
        return frozenset()
    seen = seen | {key}
    if name in unit.params:
        return None
    if name in unit.types:
        return every(an, unit.types[name], unit, seen) if unit.types[name] else None
    owner, _ = names.enclosing(an, unit, name)
    if owner is not None:
        return named(an, name, owner, seen)
    found = names.definitions(an, unit, name)
    if not found or any(an.ix.units[g].kind not in STATE_KINDS for g in found):
        return None
    out = frozenset()
    for gid in found:
        texts = every(an, an.ix.units[gid].values, an.ix.units[gid], seen)
        if texts is None:
            return None
        out |= texts
    return out


def every(an, trees, unit, seen) -> frozenset | None:
    out = frozenset()
    for tree in trees:
        texts = of(an, tree, unit, seen)
        if texts is None:
            return None
        out |= texts
    return out


def argument(an, node, index: int) -> frozenset | None:
    """The texts the argument at `index` of the call a tree's node is (a call or a `new`) can have."""
    if not isinstance(node, tuple) or len(node) < 4 or not isinstance(node[2], tuple) or not node[2]:
        return None
    tree, unit = node[2], an.ix.units[node[1]] if isinstance(node[1], int) else None
    args = tree[2] if tree[0] == CALL else tree[3] if tree[0] == CONSTRUCT and len(tree) > 3 else None
    if unit is None or args is None or index >= len(args):
        return None
    return of(an, args[index], unit)
