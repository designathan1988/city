"""Copies of an object's properties by name (log/trace-research.md F34; ECMA-262 CopyDataProperties, for an object
literal's spread `{ ...x }`; a declared intersection `A & B`, one object that is both, as an assign declares its
result): each own property of the source into the target's property of the same name — those written to it so far and
those written later (`members.field` hands each new property of a source on to its copies) — and what its dictionary
part and its elements hold into the target's. A declared object's properties are what its declared types give (its
property members: a method signature describes a prototype's method, which is no own property); a
string's characters are its indexed properties (ToObject); null, undefined and the other primitives copy nothing; the
unknown object's properties are unknown, so the target's dictionary holds it. Never every property into every one.
A write by a key enumerating an object (`for (k in o) t[k] = v`; ECMA-262 EnumerateObjectProperties: k is the name of
a property o has) makes the target's property of each name o has, those made so far and those made later.
"""
from __future__ import annotations

from ..languages import TYPEOF_STRING
from ..model import STATE_KINDS
from ..vocab import ELEMENTS_FIELD, SITE_DECLARED, SITE_UNKNOWN
from . import correlation, members, typesys, verdicts

SIGNATURES = verdicts.SIGNATURES
KEYPUT = 67                # a node: the subscription a write by an enumerated key reads the dictionary part through


def copy(an, source: int, target: int) -> None:
    """Every own property of `source`, by name, into `target` (once per pair)."""
    kind = an.objects.kind(source)
    if source == target:
        return
    if kind == SITE_UNKNOWN:
        an.solver.add(an.any_field(target), [source])
        return
    if kind in members.PRIMITIVES:
        if TYPEOF_STRING in verdicts.typeof(an, source):
            an.solver.add(members.field(an, target, ELEMENTS_FIELD), [an.objects.primitive(verdicts.STRING_K)])
        return
    if target in an.copies[source]:
        return
    an.copies[source].add(target)
    for name in sorted(an.names_of[source]):
        an.solver.follow(members.field(an, source, name), members.field(an, target, name))
    an.solver.follow(an.any_field(source), an.any_field(target))
    if kind == SITE_DECLARED:
        for name in declared_names(an, source):
            members.read(an, members.field(an, target, name), source, name)


def put(an, source, enumerated: int, target: int) -> None:
    """`target[k] = value` for k a key enumerating `enumerated`: the value in the target's property of each name
    that object has (a declared object's: its declared properties); where its names are not known (the unknown
    object, a primitive's indices, a dictionary part written through unknown keys), in the target as a dictionary.
    A value read with the key is, under each name, the one read with that name (`correlation`)."""
    kind = an.objects.kind(enumerated)
    if kind == SITE_UNKNOWN or kind in members.PRIMITIVES:
        members.put_dictionary(an, correlation.source(an, source), target)
        return
    if (source, target) in an.key_puts[enumerated]:
        return
    an.key_puts[enumerated][(source, target)] = True
    for name in sorted(an.names_of[enumerated]) + (declared_names(an, enumerated) if kind == SITE_DECLARED else []):
        put_name(an, source, target, name)
    an.solver.subscribe(an.any_field(enumerated), (KEYPUT, source, target), (KEYPUT,), lambda _, objs: [
        members.put_dictionary(an, correlation.source(an, source), target)])


def put_name(an, source, target: int, name: str) -> None:
    """One name of a write by an enumerating key: the value for that name in the target's property of it."""
    members.write(an, correlation.source(an, source, name), target, name)


def declared_names(an, obj: int) -> list[str]:
    """The properties a declared object's types declare (no method), through their merged declarations and their
    heritage."""
    names = set()
    for key in typesys.own_types(an, obj):
        for k in typesys.ancestors(an, key):
            for g in typesys.group(an, k):
                names |= {n for n, gids in an.ix.members.get(g, {}).items()
                          if n not in SIGNATURES and any(an.ix.units[i].kind in STATE_KINDS for i in gids)}
    return sorted(names)
