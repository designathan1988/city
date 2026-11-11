"""The values a guard lets through (log/trace-rules.md, "Narrowing at branch conditions": N1, N2): the typer's side of
`("narrow", name_tree, guard, positive)`. The narrowed node subscribes to the name's node and keeps each value the guard
can be true of (positive) or false of (negative); a value the tracer cannot decide is kept. A property's truthiness or
equality is decided by that property's values on that value, as they come; `instanceof` and a type predicate by the
classes the condition names, as they come (the name's held values tested again when they grow). Only values that
cannot be there at run time are removed (N2). Where `x is T`, `x instanceof C` or a type predicate holds, x is a T, as
a declared type is (EXPR, `is`; TypeScript narrows a name of no known type to the class tested): the class tested is
one of its values whatever flows in, and a value the tracer cannot tell is that class.
"""
from __future__ import annotations

from . import js_library, js_transfer, transfer, truth
from .solver import CLASS, ELEM, ENGINE, FN, LANG, OBJECT, TEXT, UNDEFINED, fn_of


def wire(t, node, tree, unit) -> None:
    _, name_tree, guard, positive = tree
    source = t.node_of(name_tree, unit)
    key = ("narrow", guard, positive)
    if guard[0] in ("member", "member-equals"):
        t.subscribe(source, node, key, by_member(t, guard, positive))
    elif guard[0] in ("instance", "predicate", "asserts"):
        named = t.node_of(guard[1], unit)
        t.subscribe(source, node, key, by_class(t, guard, positive, source, named, True, unit))
        t.subscribe(named, node, key + ("named",), by_class(t, guard, positive, source, named, False, unit))
    else:
        if guard[0] == "is" and positive:
            wanted = t.type_class(guard[1], unit)       # x is a T there: T declared (EXPR, `is`)
            t.add(node, [(CLASS + wanted, unit)] if wanted else [(guard[1], unit)])
        t.subscribe(source, node, key, by_value(t, guard, positive, unit))


def by_value(t, guard, positive: bool, unit):
    def transfer_(target, values):
        if guard[0] == "is":
            t.add(target, [kept for text, ctx in values for kept in is_of(t, guard[1], positive, text, ctx, unit)])
        else:
            t.add(target, [(text, ctx) for text, ctx in values if passes(t, guard, positive, text, ctx, unit)])
    return transfer_


def passes(t, guard, positive: bool, text: str, ctx, unit) -> bool:
    """Whether the guard can be true (positive) or false (negative) of one value decided by itself."""
    kind = guard[0]
    if kind == "truthy":
        return truth.can_be_truthy(text) if positive else truth.can_be_falsy(text)
    if kind == "typeof":
        results = truth.typeof_results(text)
        return guard[1] in results if positive else bool(results - {guard[1]})
    if kind == "equals":
        return truth.can_equal(text, guard[1]) if positive else truth.can_differ(text, guard[1])
    if kind == "nullish":
        nullish = truth.is_nullish(text)
        return nullish is not False if positive else nullish is not True
    if kind == "array":
        array = text.startswith(ELEM) or js_library.head_args(text)[0] == "Array"
        return (array or host_object(t, text, ctx)) if positive else not array
    if kind == "view":                  # `ArrayBuffer.isView`: a typed array or a DataView, the host's (G1'')
        return host_object(t, text, ctx) if positive else True
    if kind == "number":                # `Number.isInteger` and its kin: true of numbers alone (G1'')
        return "number" in truth.typeof_results(text) if positive else True
    if kind == "has":
        return has(t, guard[1], positive, text, ctx)
    return True


def host_object(t, text: str, ctx) -> bool:
    """Whether a value can be an object the program did not write: the engine's, an object of the language's, an object
    of a class of the program whose chain reaches a base the tracer did not read (it may extend the host's)."""
    if text in (ENGINE, OBJECT):
        return True
    if text.startswith(CLASS):
        key = t.type_class(text, ctx)
        return key is None or not js_transfer.chain_read(t.ix, key)
    return False


def has(t, name: str, positive: bool, text: str, ctx) -> bool:
    """`"p" in x` (MDN `in`): a property of the object or its prototype chain; a primitive throws."""
    if text.startswith((TEXT, LANG)):
        return False
    key = t.type_class(text, ctx) if text.startswith(CLASS) else None
    if key is None:
        return True
    declared = bool(t.ix.lookup(key, name)) or name in js_transfer.BUILT_IN
    maybe = name in js_transfer.written_names(t) or not js_transfer.chain_read(t.ix, key) \
        or bool(js_transfer.index_units(t.ix, key))          # an index signature holds any name (T2)
    return (declared or maybe) if positive else not declared


def is_of(t, type_text: str, positive: bool, text: str, ctx, unit) -> list:
    """GDScript `x is T` (G2): an object of a class of the program is or is not a T by its chain; a string, a callable
    or a container may be an engine T (`String`, `Callable`, `Array`), never a class of the program; a value the
    tracer cannot tell that passes is the T already declared there (`wire`). An engine T decides nothing of the
    program's classes (they may extend it)."""
    wanted = t.type_class(type_text, unit)
    key = t.type_class(text, ctx) if text.startswith(CLASS) else None
    if key is not None:
        if wanted is None:
            return [(text, ctx)]
        return [(text, ctx)] if (wanted in js_transfer.chain(t.ix, key)) == positive else []
    if not positive or wanted is None and text.startswith((TEXT, FN, ELEM)):
        return [(text, ctx)]
    return []


def member_node(t, text: str, ctx, name: str):
    """The values of property `name` on one value (shared with `Typer.member`)."""
    node = ("#member", text, ctx.gid, name, False)
    if node not in t.wired:
        t.wired.add(node)
        t.value.setdefault(node, [])
        transfer.member_one(t, node, text, ctx, name, False)
    return node


def by_member(t, guard, positive: bool):
    """`x.p` truthy, `x.p === v`: a value of x passes once one value of its `p` lets the guard be so."""
    def decide(text, ctx):
        def transfer_(target, values):
            for value, _ in values:
                if member_passes(guard, positive, value):
                    t.add(target, [(text, ctx)])
                    return
        return transfer_

    def transfer_(target, values):
        for text, ctx in values:
            member = member_node(t, text, ctx, guard[1])
            t.undecided.append((member, target, text, ctx, guard, positive))
            t.subscribe(member, target, ("narrow-by", text, guard, positive), decide(text, ctx))
    return transfer_


def settle(t) -> bool:
    """At a fixpoint, a property no value reached on an object reads `undefined` there (MDN: a property found nowhere
    on the chain): a guard that `undefined` lets through keeps that object. Returns whether a value was added (the
    solver then runs to its fixpoint again: values only grow)."""
    added, waiting = False, []
    for member, target, text, ctx, guard, positive in t.undecided:
        if t.value.get(t.find(member)):
            continue                                # decided by the property's values
        if member_passes(guard, positive, UNDEFINED):
            node = t.find(target)
            before = len(t.value.get(node, ()))
            t.add(node, [(text, ctx)])
            added = added or len(t.value.get(node, ())) > before
        else:
            waiting.append((member, target, text, ctx, guard, positive))
    t.undecided = waiting
    return added


def member_passes(guard, positive: bool, value: str) -> bool:
    if guard[0] == "member":
        return truth.can_be_truthy(value) if positive else truth.can_be_falsy(value)
    return truth.can_equal(value, guard[2]) if positive else truth.can_differ(value, guard[2])


def named_classes(t, guard, held: list) -> tuple[list, bool]:
    """The classes `instanceof C` or a type predicate names, and whether it can name one the tracer cannot tell."""
    keys, unknown = [], False
    if guard[0] == "instance":
        for text, ctx in held:
            key = t.type_class(text, ctx) if text.startswith(CLASS) else None
            keys += [key] if key else []
            unknown = unknown or key is None
        return keys, unknown
    asserts = guard[0] == "asserts"
    for text, _ in held:                  # the callee: a function declared `[asserts] x is T` at this argument
        unit = t.ix.units[fn_of(text)[0]] if text.startswith(FN) else None
        predicate = unit.predicate if unit is not None else ()
        if not predicate or predicate[0] != guard[2] or predicate[2] != asserts:
            return [], True
        for tree in predicate[1]:         # T as written, read where the function is declared
            key = t.type_class(tree[1], unit) if tree[0] == "cast" else None
            keys += [key] if key else []
            unknown = unknown or not key
    return keys, unknown or not held


def by_class(t, guard, positive: bool, source, named, from_source: bool, unit):
    """Where the condition holds, each class it names is one of the name's values (declared there); the name's values
    are tested by those classes. The classes named grow as the condition's class values do (`named` side)."""
    def transfer_(target, values):
        keys, unknown = named_classes(t, guard, t.held(named))
        tested = values if from_source else t.held(source)
        test = guard[0] == "instance"
        kept = [v for text, ctx in tested for v in instance(t, keys, unknown, positive, text, ctx, test)]
        declared = [(CLASS + k, unit) for k in keys] if positive and not from_source else []
        t.add(target, kept + declared)
    return transfer_


def instance(t, keys: list, unknown: bool, positive: bool, text: str, ctx, test: bool = True) -> list:
    """What `x instanceof C` (MDN: the prototype chain) or a type predicate keeps of one value: an object of the
    program by its chain; a primitive is never an instance (`instanceof` is false of it), but can be what a predicate
    names when the tracer cannot tell what it names (`x is string`, a call that is no predicate); a value the tracer
    cannot tell that passes is the class named (declared by `by_class`), and stays itself only when the condition can
    name a class the tracer cannot tell."""
    if text.startswith((TEXT, LANG)):
        return [] if positive and (test or not unknown) else [(text, ctx)]
    key = t.type_class(text, ctx) if text.startswith(CLASS) else None
    if key is None:
        return [(text, ctx)] if unknown or not positive else []
    lineage = js_transfer.chain(t.ix, key)
    if positive:
        keep = unknown or any(k in lineage for k in keys)
    else:
        keep = unknown or not keys or not all(k in lineage for k in keys)
    return [(text, ctx)] if keep else []
