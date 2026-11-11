"""Properties ECMAScript's own functions define (log/trace-rules.md, J14, J14'; MDN Object.defineProperty,
Object.defineProperties).

`Object.defineProperty(o, k, d)` and `Object.defineProperties(o, m)`, read on the language's own `Object`, define o's
properties: a descriptor's `value` is written to the property (as `o.p = value`), its `get` gives what it returns when
the property is read (written as the value of its call), and its accessor functions are handed to the language, which
runs them on each read and write. A descriptor's members are read as the values it holds, whatever writes them (a
pair, shorthand, a method, a reference to a function, a spread); a descriptor no literal writes gives what its
`value`, `get` and `set` members hold. A key no literal names puts the value into o, as `o[k] = v` does
(`js_body.target`), and so does a map of descriptors no literal writes, each of its descriptors' values.
"""
from __future__ import annotations

from . import js_body
from .ir import put
from .ir_js import member_name
from .source import field_of, named_children, string_value

DEFINERS = ("defineProperty", "defineProperties")
OBJECT = "Object"
VALUE, GET, SET = "value", "get", "set"
MEMBERS = (VALUE, GET, SET)


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
        members = described(r, argv[2], unit)
        if key is not None:
            written(r, node, target, key, members, unit, cond)
        else:
            computed(r, argv[0], members, unit, cond)
    elif which == "defineProperties" and len(argv) >= 2:
        if argv[1].type == "object":
            for item in named_children(argv[1]):
                if item.type == "pair":
                    key = member_name(r.src, field_of(item, "key"))
                    written(r, node, target, key, described(r, field_of(item, "value"), unit), unit, cond)
        else:                                   # a map no literal writes: each descriptor (`Object.values(m)`)
            each = ("elements", ("call", ("member", ("name", OBJECT), "values", False), (r.ir(argv[1], unit),)))
            computed(r, argv[0], {name: [("member", each, name, False)] for name in MEMBERS}, unit, cond)


def described(r, desc, unit) -> dict:
    """A descriptor's `value`, `get` and `set` as the value trees it holds (each a list)."""
    found = {name: [] for name in MEMBERS}
    if desc is None:
        return found
    if desc.type != "object":                   # a descriptor no literal writes here: what its members hold
        tree = r.ir(desc, unit)
        for name in MEMBERS:
            found[name].append(("member", tree, name, False))
        return found
    for item in named_children(desc):
        if item.type == "pair":
            name, value = member_name(r.src, field_of(item, "key")), field_of(item, "value")
            tree = r.ir(value, unit) if value is not None else None
        elif item.type == "method_definition":
            name, tree = member_name(r.src, field_of(item, "name")), ("callable", r.lambda_id(item, unit))
        elif item.type == "shorthand_property_identifier":
            name, tree = r.src.text(item), r.ir(item, unit)
        elif item.type == "spread_element":     # `{ ...base }`: what base's members hold
            inner = r.ir((named_children(item) or [None])[0], unit)
            for member in MEMBERS:
                found[member].append(("member", inner, member, False))
            continue
        else:
            continue
        if name in found and tree is not None:
            found[name].append(tree)
    return found


def written(r, node, target, key: str, members: dict, unit, cond) -> None:
    """One property's descriptor: its value written to it, its getter's result written as what reading gives, its
    accessors handed to the language."""
    for tree in members[VALUE]:
        r.add(unit, "write", key, node, cond, base=target, args=(tree,))
    for tree in members[GET]:                   # reading the property gives what the getter returns
        r.add(unit, "write", key, node, cond, base=target, args=(("call", tree, ()),))
    accessors = tuple(members[GET] + members[SET])
    if accessors:
        r.add(unit, "hand", key, node, cond, args=accessors)        # the language runs them


def computed(r, obj, members: dict, unit, cond) -> None:
    """A property whose key no literal names: its value and its getter's result put into the object (`o[k] = v`)."""
    for tree in members[VALUE] + [("call", g, ()) for g in members[GET]]:
        js_body.target(r, obj, unit, cond, put(tree))
    accessors = tuple(members[GET] + members[SET])
    if accessors:
        r.add(unit, "hand", "", obj, cond, args=accessors)
