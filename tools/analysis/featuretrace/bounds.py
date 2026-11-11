"""Declared types bound their places (log/trace-rules.md, S2; TypeScript handbook, Type Compatibility): whether a
value passes a bound (`js_bounds` tokens, resolved in the unit the type is written in), and the bounded place itself.

A place with a bound holds what flows into it only where the bound admits it: each source reaches it through a filter
subscription (no identity: the cycle sweep never merges a typed place with what flows past it). The engine's value
passes every bound (the tracer cannot tell it). A class of the program admits its objects and its subclasses', and an
object of any class that has all its members (structural); an interface or an object type an object that has its
required members, a primitive whose wrapper has them (its apparent type); a name the program does not declare (a host
type, H1) admits every value. Every value has the members the language gives it (Object.prototype's, a function's
Function.prototype's, an array's Array.prototype's) and the properties code writes on it (S2'').
"""
from __future__ import annotations

from . import js_library, js_primitives, js_transfer
from .model import ALIAS_KIND, FUNCTION_KINDS
from .rules import INDEX_MEMBER, ITERATOR_MEMBER
from .solver import ALIAS, CLASS, ELEM, ENGINE, FN, FUNCTION, NULL, OBJECT, STRING, TEXT, UNDEFINED

PRIMITIVE_TOKENS = ("number", "string", "boolean", "bigint", "symbol")
FUNCTION_MEMBERS = js_transfer.BUILT_IN | {"prototype"}         # MDN Function.prototype, a function's own properties
ARRAY_MEMBERS = js_transfer.OBJECT_MEMBERS | {
    "length", "at", "concat", "copyWithin", "entries", "every", "fill", "filter", "find", "findIndex", "findLast",
    "findLastIndex", "flat", "flatMap", "forEach", "includes", "indexOf", "join", "keys", "lastIndexOf", "map", "pop",
    "push", "reduce", "reduceRight", "reverse", "shift", "slice", "some", "sort", "splice", "toReversed", "toSorted",
    "toSpliced", "unshift", "values", "with", ITERATOR_MEMBER}                         # MDN Array.prototype
SKIPPED_MEMBERS = ("constructor", INDEX_MEMBER, "<static>")


def flow(t, source, place, tokens: tuple, unit) -> None:
    """What `source` holds flows into `place`: through the place's bound (S2), else as an identity (`follow`)."""
    if tokens:
        t.subscribe(source, place, ("bound",), admitting(t, tokens, unit))
    else:
        t.follow(source, place)


def admitting(t, tokens: tuple, unit):
    def transfer(target, values):
        t.add(target, [(text, ctx) for text, ctx in values if admits(t, tokens, unit, text, ctx)])
    return transfer


def admits(t, tokens: tuple, unit, text: str, ctx, seen: frozenset = frozenset()) -> bool:
    """Whether a value can be in a place declared with a type of these tokens."""
    if not tokens or text == ENGINE:
        return True
    return any(passes(t, token, unit, text, ctx, seen) for token in tokens)


def passes(t, token: tuple, unit, text: str, ctx, seen: frozenset) -> bool:
    kind = token[0]
    if kind == "any":
        return True
    if kind == "named":
        return named(t, token[1], unit, text, ctx, seen)
    primitive = js_primitives.kind(text)
    if kind in PRIMITIVE_TOKENS:
        return primitive == kind
    if kind == "text":
        return text in (TEXT + token[1], STRING)
    if kind == "null":
        return text == NULL
    if kind == "undefined":
        return text == UNDEFINED
    if kind == "nonnullish":
        return text not in (NULL, UNDEFINED)
    if primitive is not None:                   # a primitive against an object type: by its wrapper's members
        if primitive == js_primitives.NULLISH:
            return False
        return kind == "members" and all(js_primitives.apparent(text, name) for name in token[1])
    if text in (OBJECT, FUNCTION):              # an object or a function the tracer cannot tell
        return True
    if kind == "object":
        return True
    if kind == "function":
        return text.startswith(FN)
    if kind == "constructor":
        return text.startswith((FN, CLASS))
    if kind == "array":
        return is_array(text) or unread(t, text, ctx)
    if kind == "container":
        return js_library.head_args(text)[0] == token[1] or unread(t, text, ctx)
    return kind == "members" and has_members(t, text, ctx, token[1])


def named(t, name: str, unit, text: str, ctx, seen: frozenset) -> bool:
    """A type the program names: its class (its chain, or its members: structural), its alias's bound; a name it does
    not declare (a host type, H1) admits the value."""
    key = t.type_class(name, unit)
    if key is None or key not in t.ix.classes:
        marker = t.alias_marker(name, unit)
        alias = t.ix.units[int(marker[len(ALIAS):])] if marker else None
        if alias is None or alias.kind != ALIAS_KIND or alias.gid in seen or not alias.bound:
            return True
        return admits(t, alias.bound, alias, text, ctx, seen | {alias.gid})
    primitive = js_primitives.kind(text)
    if primitive is not None:                   # a primitive: an interface its wrapper's members satisfy
        return primitive != js_primitives.NULLISH and t.ix.classes[key].shape \
            and all(js_primitives.apparent(text, n) for n in required(t, key))
    if text in (OBJECT, FUNCTION):
        return True
    own = t.type_class(text, ctx) if text.startswith(CLASS) else None
    if own is not None and key in js_transfer.chain(t.ix, own):
        return True
    return has_members(t, text, ctx, required(t, key))


def has_members(t, text: str, ctx, names) -> bool:
    """Whether a value has every member named: an object by its chain (or a base the tracer did not read), a function
    or an array by the language's prototypes, a primitive by its wrapper's; an object, a function or an array also by
    the language's Object.prototype and by the properties code writes on a receiver (an expando, V2)."""
    if js_primitives.kind(text) is not None:
        return text not in (NULL, UNDEFINED) and all(js_primitives.apparent(text, n) for n in names)
    written = js_transfer.written_names(t)
    if text.startswith(FN):
        return all(n in FUNCTION_MEMBERS or n in written for n in names)
    if is_array(text):
        return all(n in ARRAY_MEMBERS or n in written for n in names)
    key = t.type_class(text, ctx)
    if key is None:
        return True                             # a container or a value of the language: the tracer cannot tell
    return not js_transfer.chain_read(t.ix, key) or bool(js_transfer.index_units(t.ix, key)) \
        or all(t.ix.lookup(key, n) or n in js_transfer.BUILT_IN or n in written for n in names)


def required(t, key: str) -> tuple:
    """The members a class or interface requires of what is typed as it: its own and its bases' instance members,
    an optional one (`p?: T`, `m?(): T`) excepted."""
    cache = t.bound_members
    if key not in cache:
        names = set()
        for k in js_transfer.chain(t.ix, key):
            for name, gids in t.ix.members[k].items():
                if name in SKIPPED_MEMBERS or name.startswith("#"):
                    continue
                units = [t.ix.units[g] for g in gids]
                if any(u.kind in FUNCTION_KINDS | {"field"} and not u.static and not optional(u) for u in units):
                    names.add(name)
        cache[key] = tuple(sorted(names))
    return cache[key]


def optional(unit) -> bool:
    return unit.optional or ("cast", UNDEFINED) in unit.values or ("undefined",) in unit.bound


def is_array(text: str) -> bool:
    return text.startswith(ELEM) or js_library.head_args(text)[0] == "Array"


def unread(t, text: str, ctx) -> bool:
    """An object of a class whose chain reaches a base the tracer did not read (it may extend the host's)."""
    key = t.type_class(text, ctx) if text.startswith(CLASS) else None
    return key is not None and not js_transfer.chain_read(t.ix, key)
