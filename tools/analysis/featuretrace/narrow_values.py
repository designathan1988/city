"""The values a guard lets through (log/trace-rules.md, "Narrowing at branch conditions": N1, N2): the typer's side of
`("narrow", name_tree, guard, positive)`. The narrowed node subscribes to the name's node and keeps each value the guard
can be true of (positive) or false of (negative); a value the tracer cannot decide is kept. A property's truthiness or
equality is decided by that property's values on that value, as they come; `instanceof` and a type predicate by the
classes the condition names, as they come (the name's held values tested again when they grow). Only values that
cannot be there at run time are removed (N2).
"""
from __future__ import annotations

from . import js_library, js_transfer, transfer, truth
from .solver import CLASS, ELEM, FN, LANG, TEXT, is_language


def wire(t, node, tree, unit) -> None:
    _, name_tree, guard, positive = tree
    source = t.node_of(name_tree, unit)
    key = ("narrow", guard, positive)
    if guard[0] in ("member", "member-equals"):
        t.subscribe(source, node, key, by_member(t, guard, positive))
    elif guard[0] in ("instance", "predicate"):
        named = t.node_of(guard[1], unit)
        t.subscribe(source, node, key, by_class(t, guard, positive, source, named, True))
        t.subscribe(named, node, key + ("named",), by_class(t, guard, positive, source, named, False))
    else:
        t.subscribe(source, node, key, by_value(t, guard, positive, unit))


def by_value(t, guard, positive: bool, unit):
    def transfer_(target, values):
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
        return can_be_array(t, text, ctx) if positive else not array
    if kind == "has":
        return has(t, guard[1], positive, text, ctx)
    if kind == "is":
        return is_of(t, guard[1], positive, text, ctx, unit)
    return True


def can_be_array(t, text: str, ctx) -> bool:
    if text.startswith((TEXT, FN, LANG)):
        return False
    if text.startswith(CLASS):
        key = t.type_class(text, ctx)
        return key is None or not js_transfer.chain_read(t.ix, key)        # a base the tracer did not read
    return True


def has(t, name: str, positive: bool, text: str, ctx) -> bool:
    """`"p" in x` (MDN `in`): a property of the object or its prototype chain; a primitive throws."""
    if text.startswith((TEXT, LANG)):
        return False
    key = t.type_class(text, ctx) if text.startswith(CLASS) else None
    if key is None:
        return True
    declared = bool(t.ix.lookup(key, name)) or name in js_transfer.BUILT_IN
    maybe = name in js_transfer.written_names(t) or not js_transfer.chain_read(t.ix, key)
    return (declared or maybe) if positive else not declared


def is_of(t, type_text: str, positive: bool, text: str, ctx, unit) -> bool:
    """GDScript `x is T`: an object of a class of the program is or is not a T by its chain; anything else may be."""
    wanted = t.type_class(type_text, unit)
    key = t.type_class(text, ctx) if text.startswith(CLASS) else None
    if wanted is None or key is None:
        return True
    derives = wanted in js_transfer.chain(t.ix, key)
    return derives if positive else not derives


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
            t.subscribe(member_node(t, text, ctx, guard[1]), target, ("narrow-by", text, guard, positive),
                        decide(text, ctx))
    return transfer_


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
    for text, _ in held:                            # the callee: a function declared `x is T` at this argument
        unit = t.ix.units[int(text[len(FN):])] if text.startswith(FN) else None
        predicate = unit.predicate if unit is not None else ()
        if not predicate or predicate[0] != guard[2]:
            return [], True
        for tree in predicate[1]:
            key = tree[1][len(CLASS):] if tree[0] == "cast" and tree[1].startswith(CLASS) else None
            keys += [key] if key in t.ix.classes else []
            unknown = unknown or key not in t.ix.classes
    return keys, unknown or not held


def by_class(t, guard, positive: bool, source, named, from_source: bool):
    def transfer_(target, values):
        keys, unknown = named_classes(t, guard, t.held(named))
        tested = values if from_source else t.held(source)
        t.add(target, [(text, ctx) for text, ctx in tested if instance(t, keys, unknown, positive, text, ctx)])
    return transfer_


def instance(t, keys: list, unknown: bool, positive: bool, text: str, ctx) -> bool:
    """Whether `x instanceof C` can be true (positive) or false of a value (MDN `instanceof`: the prototype chain)."""
    if text.startswith((TEXT, LANG)):
        return not positive                         # a primitive is no instance
    key = t.type_class(text, ctx) if text.startswith(CLASS) else None
    if key is None:
        return True if not positive else unknown or is_language(text)
    lineage = js_transfer.chain(t.ix, key)
    if positive:
        return unknown or any(k in lineage for k in keys)
    return unknown or not keys or not all(k in lineage for k in keys)
