"""Declared types in the analysis (log/trace-core-plan.md, Task 4; TypeScript handbook: Declaration Merging, Generics,
Classes, Interfaces): what a type name names where it is written (a class or an interface, or an alias to expand), the
declarations merged under one global name as one type, a member looked up through them and their bases, the types an
object is of (its class, its declared type, the intrinsic its kind is made with), and an object's place of a type
parameter: a field of the object, linked to what the type's index signatures read and, for a type it has through its
heritage, to the heritage's type argument (`interface B<T> extends A<T>`: B's T is A's T).
"""
from __future__ import annotations

from ..languages import INDEX_SIGNATURE, INTRINSICS
from ..model import STATE_KINDS
from ..solver import NUMBER
from ..vocab import (ANY_FIELD, CLASS_UNIT, ELEMENTS_FIELD, LEVEL_TYPE, SITE_DECLARED, SITE_PRIMITIVE, T_NAMED,
                     T_PARAM, T_UNION, T_VALUES)
from . import contexts, members

ALIAS, CLASS = 1, 2                      # what a resolved name is


def resolve(an, text: str, file: str, owner: str):
    """A type name where it is written: (ALIAS, gid) for a type alias, (CLASS, identity) for a class or an interface,
    None for a name the program and its declarations do not declare."""
    memo = an.memo.setdefault(resolve, {})
    if (text, file, owner) not in memo:
        js = an.ix.js
        gid = js.alias_of(text, file, owner)
        key = js.class_of(text, file, owner) if gid is None else None
        memo[(text, file, owner)] = (ALIAS, gid) if gid is not None else \
            (CLASS, identity(an, key)) if key is not None and key in an.ix.classes else None
    return memo[(text, file, owner)]


def group(an, key: str) -> list[str]:
    """The declarations merged with `key`: every class and interface of its name in the global scope (Declaration
    Merging), else `key` alone."""
    memo = an.memo.setdefault(group, {})
    if key not in memo:
        cls = an.ix.classes.get(key)
        gids = an.ix.js.globals.get(cls.name, []) if cls is not None and cls.name else []
        keys = sorted({an.ix.units[g].qual for g in gids if an.ix.units[g].kind == CLASS_UNIT})
        memo[key] = keys if key in keys else [key]
    return memo[key]


def identity(an, key: str) -> str:
    """The one key of a merged type."""
    return group(an, key)[0] if key in an.ix.classes else key


def global_type(an, name: str) -> str | None:
    """The type the global scope declares under a name."""
    gids = [g for g in an.ix.js.globals.get(name, []) if an.ix.units[g].kind == CLASS_UNIT]
    return identity(an, an.ix.units[gids[0]].qual) if gids else None


def lookup(an, key: str, name: str, seen=None) -> list[int]:
    """A member by name in a type's merged declarations, then up their bases."""
    if seen is None:
        memo = an.memo.setdefault(lookup, {})
        if (key, name) not in memo:
            memo[(key, name)] = lookup(an, key, name, set())
        return memo[(key, name)]
    key = identity(an, key)
    if key in seen:
        return []
    seen.add(key)
    ix = an.ix
    found = [g for k in group(an, key) for g in ix.members[k].get(name, [])]
    if found:
        return found
    for k in group(an, key):
        for base in ix.base_keys(ix.classes[k]) if k in ix.classes else []:
            found += lookup(an, base, name, seen)
    return found


def own_types(an, obj: int) -> list[str]:
    """The types an object is of, its own first: its class or declared type, then the intrinsic its kind is made with."""
    objects, out = an.objects, []
    key = objects.class_of(obj)
    if key is not None and key in an.ix.classes:
        out.append(identity(an, key))
    kind = objects.kind(obj)
    name = INTRINSICS.get(objects.keys[obj][1] if kind == SITE_PRIMITIVE else kind)
    intrinsic = global_type(an, name) if name else None
    if intrinsic is not None and intrinsic not in out:
        out.append(intrinsic)
    return out


def has_member(an, obj: int, name: str) -> bool:
    return any(lookup(an, key, name) for key in own_types(an, obj))


def declared(an, gid: int) -> bool:
    """A definition only declared: written in a declaration file or under `declare`, or a member of an interface or an
    object type (its owner has no object of its own)."""
    unit = an.ix.units[gid]
    cls = an.ix.classes.get(unit.owner)
    return unit.ambient or cls is not None and cls.shape


def place(an, obj: int, key: str, index: int):
    """The place of the type parameter `index` of type `key` on `obj`."""
    node = an.field(obj, (T_PARAM, key, index))
    if node not in an.wired:
        an.wired.add(node)
        keyed(an, obj, key, index, node)
        inherited(an, obj, key, index, node)
    return node


def keyed(an, obj: int, key: str, index: int, node) -> None:
    """An index signature whose value is this type parameter: what the object holds under such keys is the place's,
    both ways (a number key: the elements; any other: every property)."""
    for gid in lookup(an, key, INDEX_SIGNATURE):
        for key_type, value in an.ix.units[gid].indexes:
            if (T_PARAM, LEVEL_TYPE, index) in parts(value):
                held = members.field(an, obj, ELEMENTS_FIELD) if NUMBER in values(key_type) else an.field(obj, ANY_FIELD)
                an.solver.follow(held, node)
                an.solver.follow(node, held)


def inherited(an, obj: int, key: str, index: int, node) -> None:
    """A type the object has through the heritage of one of its types: the heritage's type argument at `index`, read
    on the type that names it (a type parameter of that type: its place, both ways)."""
    from . import values
    seen, todo = set(), list(own_types(an, obj))
    while todo:
        sub = todo.pop()
        if sub in seen or sub == key:
            continue
        seen.add(sub)
        for k in group(an, sub):
            cls = an.ix.classes.get(k)
            for base in cls.heritage if cls is not None else []:
                found = resolve(an, base[1], base[3], base[4]) if base[0] == T_NAMED else None
                if found is None or found[0] != CLASS:
                    continue
                todo.append(found[1])
                if found[1] != key or index >= len(base[2]):
                    continue
                arg = base[2][index]
                if arg[0] == T_PARAM and arg[1] == LEVEL_TYPE:
                    other = place(an, obj, sub, arg[2])
                    an.solver.follow(other, node)
                    an.solver.follow(node, other)
                else:
                    site = ((T_PARAM, key, index), contexts.ROOT)
                    an.solver.follow(values.produce(an, arg, (obj, sub, None), site), node)


def state(an, gid: int) -> bool:
    return an.ix.units[gid].kind in STATE_KINDS


def parts(tree: tuple) -> list:
    """A type's parts: a union's, each; any other type itself."""
    return [p for part in tree[1] for p in parts(part)] if tree and tree[0] == T_UNION else [tree]


def values(tree: tuple) -> tuple:
    """The language's values a type admits by itself."""
    return tuple(v for p in parts(tree) if p and p[0] == T_VALUES for v in p[1])


def declared_type(an, obj: int) -> str | None:
    """The type a declared object is of."""
    key = an.objects.keys[obj]
    return key[1] if key[0] == SITE_DECLARED else None
