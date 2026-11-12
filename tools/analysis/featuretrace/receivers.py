"""The object a function runs on (log/trace-rules.md, J2', J2'''; MDN `this`, ISO C++ [expr.prim.this], GDScript
`self`; ECMAScript OrdinaryCallBindThis).

`this` in a method holds the objects the method is called on: each link of a call that reaches it carries its receiver
(a member call's receiver, the object `new` makes, the caller's own `this` for `super(...)`, `super.m()` and a call
written without a receiver), kept when its class is the method's class or below it, so an override gets only its own
class's objects. A static method's `this` is its class; a lambda, and a JavaScript arrow, read their enclosing
function's. A JavaScript function with a `this` of its own (a declaration or an expression, J2''') holds what its calls
give, whatever object: a member call's receiver, the object `new` makes (its prototype's class), the object `call`,
`apply` or `bind` gives, `undefined` for a plain call in strict code or the global object (the engine's value) in
sloppy code, the engine's value when the engine or a library is handed it. A function no call with a receiver reaches
(the engine's or a library's callback, a function no code calls) holds its class's objects (a method) or the engine's
value (a function), decided at a fixpoint (`settle`), when every call that could reach it has.
"""
from __future__ import annotations

from . import js_transfer, templates
from .solver import CLASS, ENGINE, FN, UNDEFINED, fn_of

THIS = "#this"
RECEIVING = ("call", "override", "new")           # the links of a call: what they reach runs on the call's
                                                   # receiver
HANDED = "handed"                                  # a link to a function the engine or a library runs


def own_this(unit) -> bool:
    """A JavaScript function whose `this` is what its calls give, whatever object (J2'''): no arrow, no method."""
    return unit.lang == "js" and unit.this_of == "function" and not unit.static


def lexical(unit) -> bool:
    """A function that reads its enclosing function's `this`: a lambda (C++ captures it, GDScript's is its script's),
    a JavaScript arrow."""
    return unit.this_of == "arrow" if unit.lang == "js" else unit.kind == "lambda"


def runs_on(t, unit) -> bool:
    """Whether what `this` holds in a function depends on its calls: a method (not static) of a class, never a
    module's or a namespace's; a JavaScript function with a `this` of its own; never a lambda or an arrow inside a
    function."""
    base = unit.base
    if lexical(base) and base.outer >= 0:
        return False
    if own_this(base):
        return True
    if base.kind not in ("func", "method") or base.static or base.owner not in t.ix.classes:
        return False
    return not (base.lang == "js" and (t.ix.js.is_module(base.owner) or base.owner in t.ix.js.namespaces))


def this_node(t, unit):
    """The node `this` is read from in `unit`: its own for a method or a function, its enclosing function's for a
    lambda or an arrow, its class for anything else (a static method, a class's own code)."""
    base = unit.base
    if lexical(base) and base.outer >= 0:
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
            t.unreceived.append((node, base))           # what it holds if no call reaches it (`settle`)
        else:
            key = t.class_of(base)
            t.add(node, [(CLASS + key, base)] if key else [])
    return node


def carry(rs, edge) -> None:
    """What a link of a call gives `this` of the function it reaches (J2', J2''')."""
    t, units = rs.typer, rs.ix.units
    method, caller = units[edge.dst], units[edge.src]
    if edge.kind == HANDED:                            # the engine or a library calls it: its `this` is theirs
        if own_this(method):
            t.add(this_node(t, method), [(ENGINE, method)])
        return
    if edge.kind not in RECEIVING or not runs_on(t, method):
        return
    if edge.kind == "new" and (edge.base is None or method.lang == "js"):     # the object `new` makes: its class's
        key = method.constructs if own_this(method) else method.owner          # (a function's: its prototype's, N1'')
        t.add(this_node(t, method), [(CLASS + key, method)] if key else [])
        return
    if own_this(method) and edge.base is None:         # a plain call: undefined in strict code, the global object else
        t.add(this_node(t, method), [(UNDEFINED if method.strict else ENGINE, method)])
        return
    source = this_node(t, caller) if edge.base in (None, ("super",)) else t.node_of(edge.base, caller)
    if not own_this(method):
        t.subscribe(source, this_node(t, method), ("this", method.gid), below(t, method.owner))
    elif edge.name and edge.base not in (None, ("super",)):         # `x.f()`: the x whose `f` holds it
        t.subscribe(source, this_node(t, method), ("this", method.gid, edge.name), holders(t, method.gid, edge.name))
    else:                                              # `f.call(o)`, `f.apply(o)`, `f.bind(o)`: the object given
        t.follow(source, this_node(t, method))


def holders(t, gid: int, name: str):
    """The receivers of `x.name()` whose `name` can hold function `gid` (J2'''; MDN `this`: a function called as a
    property runs on the object the property belongs to), each kept once its member holds the function."""
    def transfer(target, values):
        for text, ctx in values:
            t.subscribe(t.member_node(text, ctx, name), target, ("holds", gid, text, ctx.gid), holding(t, gid, text, ctx))
    return transfer


def holding(t, gid: int, text: str, ctx):
    """A receiver kept once its member holds function `gid`."""
    def transfer(target, values):
        if any(v.startswith(FN) and fn_of(v)[0] == gid for v, _ in values):
            t.add(target, [(text, ctx)])
    return transfer


def below(t, owner: str):
    """The values that are objects of class `owner` or of a class below it."""
    def transfer(target, values):
        t.add(target, [(text, ctx) for text, ctx in values
                       if (key := t.type_class(text, ctx)) is not None and owner in js_transfer.chain(t.ix, key)])
    return transfer


def settle(t) -> bool:
    """At a fixpoint, a method no call with a receiver reached runs on its class's objects, a JavaScript function on
    the engine's value (the host calls it). Returns whether a value was added (the solver then runs to its fixpoint
    again)."""
    added, waiting = False, []
    for node, unit in t.unreceived:
        if t.value.get(t.find(node)):
            continue
        if own_this(unit):
            t.add(node, [(ENGINE, unit)])
            added = True
            continue
        key = t.class_of(unit)
        if key:
            t.add(node, [(CLASS + key, unit)])
            added = True
    t.unreceived = waiting
    return added
