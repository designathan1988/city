"""Properties ECMAScript's own functions define (log/trace-rules.md, J14; MDN Object.defineProperty,
Object.defineProperties).

`Object.defineProperty(o, "p", d)` and `Object.defineProperties(o, { p: d, ... })`, read on the language's own `Object`,
with a descriptor written as an object literal, define o's property p: a data descriptor's `value` is written to it
(as `o.p = value`), an accessor's `get` gives what it returns when the property is read (written as the value of its
call), and the accessor functions are handed to the language, which runs them on each read and write.
"""
from __future__ import annotations

from .ir_js import member_name
from .source import field_of, named_children, string_value

DEFINERS = ("defineProperty", "defineProperties")
OBJECT = "Object"
ACCESSORS = ("get", "set")


def definer(r, fn, unit) -> str | None:
    """`Object.defineProperty` / `Object.defineProperties` on the language's `Object` (not a local of that name)."""
    if fn is None or fn.type != "member_expression":
        return None
    obj, prop = field_of(fn, "object"), field_of(fn, "property")
    if obj is None or prop is None or obj.type != "identifier" or r.src.text(obj) != OBJECT or OBJECT in unit.types:
        return None
    name = member_name(r.src, prop)
    return name if name in DEFINERS else None


def define(r, node, which: str, argv: list, unit, cond) -> None:
    """The properties a call of a definer defines on its first argument."""
    if not argv:
        return
    target = r.ir(argv[0], unit)
    if which == "defineProperty" and len(argv) >= 3:
        key = string_value(r.src, argv[1])
        if key is not None:
            descriptor(r, node, target, key, argv[2], unit, cond)
    elif which == "defineProperties" and len(argv) >= 2 and argv[1].type == "object":
        for pair in (c for c in named_children(argv[1]) if c.type == "pair"):
            descriptor(r, node, target, member_name(r.src, field_of(pair, "key")), field_of(pair, "value"), unit, cond)


def descriptor(r, node, target, key: str, desc, unit, cond) -> None:
    """One property's descriptor written as an object literal: its `value`, its `get`, its `set`."""
    if desc is None or desc.type != "object":
        return
    for item in named_children(desc):
        if item.type == "pair":
            name, value = member_name(r.src, field_of(item, "key")), field_of(item, "value")
        elif item.type == "method_definition":
            name, value = member_name(r.src, field_of(item, "name")), item
        else:
            continue
        if value is None:
            continue
        if name == "value":
            r.add(unit, "write", key, node, cond, base=target, args=(r.ir(value, unit),))
        elif name in ACCESSORS:
            accessor = ("callable", r.lambda_id(value, unit))
            if name == "get":                       # reading the property gives what the getter returns
                r.add(unit, "write", key, node, cond, base=target, args=(("call", accessor, ()),))
            r.add(unit, "hand", key, node, cond, args=(accessor,))    # the language runs it
