"""The transfers of the JavaScript value rules (log/trace-rules.md: N1, T4', P1-P3, J5), one value at a time, as
`transfer` has the language-independent ones: `new X(...)` evaluated, `o[k]` with a computed key, the iteration of an
object of the program, `for (k in o)` and `Object.keys/values/entries(o)` over its enumerable properties.
"""
from __future__ import annotations

from . import transfer as common          # the language-independent transfers
from .library import ITEM
from .rules import ITERATOR_MEMBER
from .solver import CLASS, ELEM, ENGINE, FN, TEXT


def iterated(t, key: str):
    """What iterating a JS object of class `key` gives (J5): its `[Symbol.iterator]()`'s `next().value`."""
    node = ("#iterated", key)
    if node not in t.wired:
        t.wired.add(node)
        t.value.setdefault(node, [])
        iterator = ("#iterator", key)
        for gid in t.ix.definitions(t.ix.lookup(key, ITERATOR_MEMBER)):
            t.subscribe(t.ret_node(t.ix.units[gid]), iterator, ("member", "next", True), common.members(t, "next", True))
        t.subscribe(iterator, node, ("member", "value", False), common.members(t, "value", False))
    return node


def indexed(t, other, from_base: bool):
    """JS `o[k]` with a computed key (T4'): each object value with each string value of the key gives that member;
    a container gives its elements. `from_base`: the new values are the object's (`other` the key), else the key's."""
    def named(values) -> list[str]:
        return [text[len(TEXT):] for text, _ in values if text.startswith(TEXT)]

    def transfer(target, values):
        objects, names = (values, named(t.held(other))) if from_base else (t.held(other), named(values))
        if not from_base and not names:
            return                                      # a key that is no known string reads no property
        for text, ctx in objects:
            if t.type_class(text, ctx):                 # an object of the program: the members the key names
                for name in names:
                    common.member_one(t, target, text, ctx, name, False)
            elif from_base:                             # a container: its elements, whatever the key
                common.step_one(t, target, ITEM, text, ctx)
    return transfer


def enumerable(ix, key: str) -> list:
    """The enumerable properties of a JS object of the program (P1): an object literal's every property and its spread
    bases'; a class instance's fields, its bases' too (methods, accessors and statics are not enumerable on it)."""
    found, todo, seen = [], [key], set()
    while todo:
        k = todo.pop()
        if k in seen or k not in ix.classes:
            continue
        seen.add(k)
        literal = "#<object@" in k
        found += [ix.units[g] for gids in ix.members[k].values() for g in gids
                  if literal and ix.units[g].kind != "class"
                  or ix.units[g].kind in ("field", "var") and not ix.units[g].static and not ix.units[g].accessors]
        todo += ix.base_keys(ix.classes[k])
    return found


def owns(t, mode: str):
    """`for (k in o)` ("in": o's enumerable property names as strings, P2); `Object.keys/values/entries(o)` (P3): an
    array of the names, of the properties' values, of `[name, value]` arrays. Over an array: its indices (the
    language's values), its elements, `[index, element]`."""
    def transfer(target, values):
        for text, ctx in values:
            key = t.type_class(text, ctx)
            props = enumerable(t.ix, key) if key else []
            if mode in ("in", "keys"):
                names = [TEXT + p.name for p in props] or [ENGINE]
                t.add(target, [(ELEM + n if mode == "keys" else n, ctx) for n in names])
            elif props:
                for p in props:
                    t.subscribe(t.hold_node(p), target, ("own", mode, p.gid), wrapped(t, mode, p.name, ctx))
            elif text.startswith(ELEM):
                t.add(target, [(text, ctx)] if mode == "values" else [(ELEM + ELEM + ENGINE, ctx), (ELEM + text, ctx)])
            else:
                t.add(target, [(ELEM + ENGINE, ctx)])
    return transfer


def wrapped(t, mode: str, name: str, ctx):
    """One property's values as `Object.values` (each an element) or `Object.entries` (each in a `[name, value]`
    element) give them; a value that is itself a container is not wrapped again (as `elements`)."""
    def transfer(target, values):
        plain = [(v, c) for v, c in values if not v.startswith(ELEM)]
        if mode == "values":
            t.add(target, [(ELEM + v, c) for v, c in plain])
        else:
            t.add(target, [(ELEM + ELEM + TEXT + name, ctx)] + [(ELEM + ELEM + v, c) for v, c in plain])
    return transfer


def constructs(t):
    """`new X(...)` in JS (N1): a class gives its instance, a function called as a constructor the objects it
    returns, anything else the host's constructor (the language's value)."""
    def transfer(target, values):
        for text, ctx in values:
            if text.startswith(FN):
                t.follow(t.ret_node(t.ix.units[int(text[len(FN):])]), target)
            elif text.startswith(CLASS):
                t.add(target, [(text, ctx)])
            else:
                t.add(target, [(ENGINE, ctx)])
    return transfer
