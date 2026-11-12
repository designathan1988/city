"""A write through a key whose value is read with that same key (Sridharan, Dolby, Chandra, Schafer, Tip, "Correlation
Tracking for Points-To Analysis of JavaScript", ECOOP 2012): `for (k in src) dst[k] = src[k]` writes, under each name,
the value read with that name — never every value under every name, which joins all the properties of `src` in every
property of `dst` and, from there, everything the program copies. The paper extracts the code between the correlated
read and write into a function analysed once per key; here the written value's tree, with the locals of its function
whose values read the key put in place, is evaluated once per name with the key read as that name's text. A value that
reads no key is the same for every name and is written as it is.
"""
from __future__ import annotations

from ..vocab import CAST, NAME, STRNAME, UNION

CORRELATED = 68           # a source of a write: (CORRELATED, gid, value tree, key tree, ctx), its value per name


def describe(an, unit, tree: tuple, key_tree: tuple, ctx):
    """The source of a write of `tree` through `key_tree` in `unit` running in `ctx`: a correlated one when the value
    reads the key, directly or through the locals of the function that hold what it reads; else None."""
    expanded = expand(an, unit, tree, key_tree, set())
    return (CORRELATED, unit.gid, expanded[0], key_tree, ctx) if expanded[1] else None


def expand(an, unit, tree, key_tree, seen: set) -> tuple:
    """`tree` with each local whose values read the key replaced by those values (a union); whether it reads the key."""
    if tree == key_tree:
        return tree, True
    if not isinstance(tree, tuple) or not tree:
        return tree, False
    if tree[0] == NAME and len(tree) == 2 and tree[1] in unit.types and tree[1] not in seen:
        seen.add(tree[1])
        parts = [expand(an, unit, value, key_tree, seen) for value in unit.types[tree[1]] if holds(an, value)]
        seen.discard(tree[1])
        if not any(reads for _, reads in parts):
            return tree, False
        union = parts[0][0]
        for part, _ in parts[1:]:
            union = (UNION, union, part)
        return union, True
    items, reads = [], False
    for part in tree:
        if isinstance(part, tuple):
            part, found = expand(an, unit, part, key_tree, seen)
            reads = reads or found
        items.append(part)
    return tuple(items), reads


def holds(an, tree) -> bool:
    """Whether a tree of a local's declarations is a value it holds (a declared type's cast is none: `trees`)."""
    from .trees import is_made
    return bool(tree) and (tree[0] != CAST or is_made(an, tree[1]))


def substitute(tree, key_tree, replacement):
    """`tree` with the key read as `replacement`."""
    if tree == key_tree:
        return replacement
    if not isinstance(tree, tuple):
        return tree
    return tuple(substitute(part, key_tree, replacement) for part in tree)


def source(an, given, name: str | None = None):
    """The node of a write's value: the value itself, or — a correlated one — its value with the key read as `name`
    (every name's together where `name` is None: a key that may name any property)."""
    if not (isinstance(given, tuple) and given and given[0] == CORRELATED):
        return given
    _, gid, tree, key_tree, ctx = given
    return an.node(tree if name is None else substitute(tree, key_tree, (STRNAME, name)), an.ix.units[gid], ctx)
