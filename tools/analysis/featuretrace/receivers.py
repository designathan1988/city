"""The object a method runs on (log/trace-rules.md, J2'; MDN `this`, ISO C++ [expr.prim.this], GDScript `self`).

`this` in a method holds the objects the method is called on: each link of a call that reaches it carries its receiver
(a member call's receiver, the object `new` makes, the caller's own `this` for `super(...)`, `super.m()` and a call
written without a receiver), kept when its class is the method's class or below it, so an override gets only its own
class's objects. A static method's `this` is its class; a lambda reads its enclosing function's. A method no call with a
receiver reaches (the engine's or a library's callback, a method no code calls) holds its class's objects, decided at a
fixpoint (`settle`), when every call that could reach it has.
"""
from __future__ import annotations

from . import js_transfer, templates
from .solver import CLASS

THIS = "#this"
RECEIVING = ("call", "override", "new", "bound")    # the links of a call (and of a `bind`, J2''): what they reach
                                                   # runs on the call's receiver


def runs_on(t, unit) -> bool:
    """Whether what `this` holds in a function depends on its calls: a method (not static) of a class, never a
    module's, a namespace's or a lambda."""
    base = unit.base
    if base.kind not in ("func", "method") or base.static or base.owner not in t.ix.classes:
        return False
    return not (base.lang == "js" and (t.ix.js.is_module(base.owner) or base.owner in t.ix.js.namespaces))


def this_node(t, unit):
    """The node `this` is read from in `unit`: its own for a method, its enclosing function's for a lambda, its class
    for anything else (a static method, a class's own code)."""
    base = unit.base
    if base.kind == "lambda" and base.outer >= 0:
        return this_node(t, t.ix.units[t.ix.offset[base.file] + base.outer])
    if unit.key != base.gid and templates.RECEIVER in templates.key_params(t, base):
        node = (THIS, unit.key)                         # a template's: its receiver, or the method's (`templates.route`)
        if node not in t.wired:
            t.wired.add(node)
            t.value.setdefault(node, [])
        return node
    node = (THIS, base.gid)
    if node not in t.wired:
        t.wired.add(node)
        t.value.setdefault(node, [])
        if runs_on(t, base):
            t.unreceived.append((node, base))           # its class's objects if no call reaches it (`settle`)
        else:
            key = t.class_of(base)
            t.add(node, [(CLASS + key, base)] if key else [])
    return node


def carry(rs, edge) -> None:
    """What a link of a call gives `this` of the method it reaches (J2')."""
    t, units = rs.typer, rs.ix.units
    method, caller = units[edge.dst], units[edge.src]
    if edge.kind not in RECEIVING or not runs_on(t, method):
        return
    if edge.kind == "new" and edge.base is None:      # a construction written as a declaration: its class's object
        t.add(this_node(t, method), [(CLASS + method.owner, method)])
        return
    source = this_node(t, caller) if edge.base in (None, ("super",)) else t.node_of(edge.base, caller)
    t.subscribe(source, this_node(t, method), ("this", method.gid), below(t, method.owner))


def below(t, owner: str):
    """The values that are objects of class `owner` or of a class below it."""
    def transfer(target, values):
        t.add(target, [(text, ctx) for text, ctx in values
                       if (key := t.type_class(text, ctx)) is not None and owner in js_transfer.chain(t.ix, key)])
    return transfer


def settle(t) -> bool:
    """At a fixpoint, a method no call with a receiver reached runs on its class's objects. Returns whether a value was
    added (the solver then runs to its fixpoint again)."""
    added, waiting = False, []
    for node, unit in t.unreceived:
        if t.value.get(t.find(node)):
            continue
        key = t.class_of(unit)
        if key:
            t.add(node, [(CLASS + key, unit)])
            added = True
    t.unreceived = waiting
    return added
