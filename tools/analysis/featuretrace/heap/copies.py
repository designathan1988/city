"""Copies of an object's properties by name (log/trace-research.md F34; ECMA-262 CopyDataProperties, for an object
literal's spread `{ ...x }`; a declared intersection `A & B`, one object that is both, as an assign declares its
result): each own property of the source into the target's property of the same name — those written to it so far and
those written later (`members.field` hands each new property of a source on to its copies) — and what its dictionary
part and its elements hold into the target's. A declared object's properties are what its declared types give (its
property members: a method signature describes a prototype's method, which is no own property); a
string's characters are its indexed properties (ToObject); null, undefined and the other primitives copy nothing; the
unknown object's properties are unknown, so the target's dictionary holds it. Never every property into every one.
"""
from __future__ import annotations

from ..languages import TYPEOF_STRING
from ..model import STATE_KINDS
from ..vocab import ELEMENTS_FIELD, SITE_DECLARED, SITE_UNKNOWN
from . import members, typesys, verdicts

SIGNATURES = verdicts.SIGNATURES


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
