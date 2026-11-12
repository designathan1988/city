"""A property read or written through a key (`o[k]`; ECMA-262 ToPropertyKey), for one value of the key: a string of a
known text is the property it names, an index the elements, a key enumerating an object (`for...in`) a property that
object has (`copies.put`), any other key the object as a dictionary — what such a write put is read through a key, not
by a name (log/trace-research.md F40).
"""
from __future__ import annotations

from ..vocab import ELEMENTS_FIELD, SITE_KEY, SITE_TEXT
from . import copies, correlation, members


def store_by_key(an, source, obj: int, key: int) -> None:
    """`obj[k] = value` for one value of k: a string of a known text writes the property it names (ECMA-262
    ToPropertyKey; a canonical numeric one, an index); a key enumerating an object (`for...in`) the property of each
    name that object has (`copies.put`); an index puts it in the elements; any other key may name any property: the
    elements and the dictionary part. A value read with the write's key is that name's (`correlation`)."""
    if an.objects.kind(key) == SITE_TEXT:
        text = an.objects.text(key)
        members.write(an, correlation.source(an, source, text), obj, text)
    elif an.objects.kind(key) == SITE_KEY:
        copies.put(an, source, an.objects.keys[key][1], obj)
    elif members.is_index(an, key):
        if an.objects.kind(obj) not in members.KEEP_NOTHING:
            an.solver.follow(correlation.source(an, source), members.field(an, obj, ELEMENTS_FIELD))
    else:
        members.put_dictionary(an, correlation.source(an, source), obj)


def load_by_key(an, target, obj: int, key: int) -> None:
    """`obj[k]` for one value of k: a string of a known text reads the property it names, as `obj.text` does; an index
    reads the elements; a key enumerating this same object (`for...in`) every property of it; any other key, the
    object as a dictionary (`read_all`)."""
    if an.objects.kind(key) == SITE_TEXT:
        members.read(an, target, obj, an.objects.text(key))
        return
    if an.objects.kind(obj) in members.KEEP_NOTHING:
        return
    if members.is_index(an, key):
        an.solver.follow(members.field(an, obj, ELEMENTS_FIELD), target)
        return
    if an.objects.kind(key) == SITE_KEY and an.objects.keys[key][1] == obj:
        an.solver.follow(members.every(an, obj), target)
    members.read_all(an, target, obj)
