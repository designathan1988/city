"""The transfers of the JavaScript value rules (log/trace-rules.md: N1, N1'', T4', P1-P3, J5, V2), one value at a
time, as `transfer` has the language-independent ones: `new X(...)` evaluated, `o[k]` with a computed key, the
iteration of an object of the program, `for (k in o)` and `Object.keys/values/entries(o)` over its enumerable
properties, a property no class declares (an index signature's, written outside the class, on its prototype, or
nowhere), a function's own properties and prototype, `Object.create(p)`.
"""
from __future__ import annotations

from . import js_primitives
from . import transfer as common          # the language-independent transfers
from .library import ITEM
from .model import ALIAS_KIND
from .rules import INDEX_MEMBER, ITERATOR_MEMBER
from .solver import CLASS, ELEM, ENGINE, FN, OBJECT, TEXT, UNDEFINED, fn_of

PROTOTYPE = "prototype"
# What every object has from the language (MDN Object.prototype), and every function besides (MDN Function.prototype,
# a function's own `length` and `name`): a class value is either its objects or the class itself, so neither is ever
# `undefined` on it.
OBJECT_MEMBERS = frozenset({"constructor", "hasOwnProperty", "isPrototypeOf", "propertyIsEnumerable", "toLocaleString",
                            "toString", "valueOf", "__proto__", "__defineGetter__", "__defineSetter__",
                            "__lookupGetter__", "__lookupSetter__"})
FUNCTION_MEMBERS = frozenset({"apply", "bind", "call", "length", "name", "arguments", "caller"})
BUILT_IN = OBJECT_MEMBERS | FUNCTION_MEMBERS


def prop_node(key: str, name: str) -> tuple:
    """What JS code writes to property `name` of class `key`'s objects where no class of its chain declares it (V2)."""
    return ("#prop", key, name)


def fn_prop_node(gid: int, name: str) -> tuple:
    """What JS code writes to property `name` of function `gid` (a function is an object; `f.prototype = o`, N1'')."""
    return ("#fnprop", gid, name)


def function_property(t, target, unit, ctx, name: str, called: bool, caller=None, args: tuple = ()) -> None:
    """A property of a JS function of the program other than `call`, `apply`, `bind` (N1''; MDN Function): what code
    writes on it under that name; `prototype` its prototype object (an ordinary function's class of its own, a
    generator's the language's, `undefined` on an arrow, a method or an async function); one every function has the
    language's value; any other `undefined` (calling it throws: nothing)."""
    written = name in written_names(t)
    if written:
        source = fn_prop_node(unit.gid, name)
        if called:
            t.subscribe(source, target, ("call",), common.calls(t, caller, args))
        else:
            t.follow(source, target)
    if name == PROTOTYPE and not called:
        t.add(target, [(CLASS + unit.constructs if unit.constructs else ENGINE if unit.generator else UNDEFINED, ctx)])
    elif name in BUILT_IN:
        t.add(target, [(ENGINE, ctx)])
    elif not written and not called:
        t.add(target, [(UNDEFINED, ctx)])


def prototypes(t, key: str) -> list:
    """The prototype objects code wrote for the functions whose objects class `key` or a base is (`f.prototype = o`,
    N1''): where a member none of them declares is looked up next."""
    if PROTOTYPE not in written_names(t):
        return []
    if t.constructors is None:
        t.constructors = {u.constructs: u.gid for u in t.ix.units if u.constructs}
    return [fn_prop_node(t.constructors[k], PROTOTYPE) for k in chain(t.ix, key) if k in t.constructors]


def chain(ix, key: str) -> list[str]:
    """A class and every base above it (the prototype chain)."""
    found, todo = [], [key]
    while todo:
        k = todo.pop()
        if k not in found and k in ix.classes:
            found.append(k)
            todo += ix.base_keys(ix.classes[k])
    return found


def chain_read(ix, key: str) -> bool:
    """Whether the tracer read every base of the chain (a base it cannot resolve may hold any property)."""
    return all(len(ix.base_keys(ix.classes[k])) == len(ix.classes[k].bases) for k in chain(ix, key))


def written_names(t) -> frozenset:
    """The property names JS code writes on a receiver (`o.p = v`): only these can be created outside a class (V2)."""
    if t.js_written is None:
        t.js_written = frozenset(r.name for u in t.ix.units if u.lang == "js" for r in u.refs
                                 if r.kind == "write" and r.base is not None)
    return t.js_written


def undeclared(t, target, key: str, ctx, name: str, called: bool, caller=None, args: tuple = ()) -> None:
    """A property of an object of class `key` that no class of its chain declares (V2): an inner class or a module's
    member first; `prototype` is the class's objects; a property JS code writes is what was written to it on the class
    or a base (called: what those functions return); the language's value when it is one every object has or a base
    the tracer did not read may hold it, whatever is written under its name (V2'); else, when nothing is written,
    `undefined` (calling it throws: nothing)."""
    inner = t.ix.inner[key].get(name)
    if inner:
        t.add(target, [(CLASS + inner, ctx)])
        return
    if name == PROTOTYPE and not called:
        t.add(target, [(CLASS + key, ctx)])
        return
    indexes = index_nodes(t, key)
    sources = indexes + ([prop_node(k, name) for k in chain(t.ix, key)] if name in written_names(t) else [])
    for source in sources:
        if called:
            t.subscribe(source, target, ("call",), common.calls(t, caller, args))
        else:
            t.follow(source, target)
    protos = prototypes(t, key)
    for proto in protos:                            # the members of the prototype objects code wrote (N1'')
        t.subscribe(proto, target, ("proto", name, called), common.members(t, name, called, caller, args))
    if name in BUILT_IN or not chain_read(t.ix, key):
        t.add(target, [(ENGINE, ctx)])              # one every object has, or a base the tracer did not read holds it,
    elif not called and (indexes or not sources) and not protos:     # whatever code writes under its name (V2')
        t.add(target, [(UNDEFINED, ctx)])           # found nowhere, or a key an index signature does not hold


def index_nodes(t, key: str) -> list:
    """What an index signature of class `key` or a base declares (T2): the values any property name reads."""
    return [t.hold_node(t.ix.units[g]) for g in index_units(t.ix, key)]


def index_units(ix, key: str) -> list[int]:
    """The index signatures of class `key` and its bases: with one, any property name may be there."""
    return [g for k in chain(ix, key) for g in ix.members[k].get(INDEX_MEMBER, [])]


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
    def transfer(target, values):
        objects, keys = (values, t.held(other)) if from_base else (t.held(other), values)
        names = [text[len(TEXT):] for text, _ in keys if text.startswith(TEXT)]
        unnamed = any(not text.startswith(TEXT) for text, _ in keys)
        for text, ctx in objects:
            key = t.type_class(text, ctx)
            if key:                                     # an object of the program: the members the key names; a key
                for name in names:                      # no string names reads only what an index signature holds
                    common.member_one(t, target, text, ctx, name, False)
                for node in index_nodes(t, key) if unnamed else []:
                    t.follow(node, target)
            elif js_primitives.kind(text) is not None:  # a string, a number...: the members the key names, else an
                for name in names:                      # index (T5)
                    common.member_one(t, target, text, ctx, name, False)
                if unnamed:
                    common.step_one(t, target, ITEM, text, ctx)
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
        found += [ix.units[g] for name, gids in ix.members[k].items() if name != INDEX_MEMBER for g in gids
                  if literal and ix.units[g].kind not in ("class", ALIAS_KIND)
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
                for p in props:                         # an accessor property's value is its getter's (G8')
                    t.subscribe(t.read_node(p), target, ("own", mode, p.gid), wrapped(t, mode, p.name, ctx))
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


def created(t):
    """`Object.create(p)` (N1''; MDN Object.create): an object whose members are p's, read as p's objects (a write on
    it is a write on p: an over-approximation); with no prototype (`null`), an object of the language."""
    def transfer(target, values):
        t.add(target, [(OBJECT if js_primitives.kind(text) == js_primitives.NULLISH else text, ctx)
                       for text, ctx in values])
    return transfer


def constructs(t):
    """`new X(...)` in JS (N1): a class gives its instance, a function called as a constructor the objects it
    returns, anything else the host's constructor (the language's value)."""
    def transfer(target, values):
        for text, ctx in values:
            if text.startswith(FN):                     # what it returns, else the object it made (N1')
                unit = t.ix.units[fn_of(text)[0]]
                t.follow(t.ret_node(unit), target)
                t.add(target, [(CLASS + unit.constructs, ctx)] if unit.constructs else [])
            elif text.startswith(CLASS):
                t.add(target, [(text, ctx)])
            else:
                t.add(target, [(ENGINE, ctx)])
    return transfer
