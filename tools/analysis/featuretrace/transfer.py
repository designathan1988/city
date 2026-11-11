"""The transfers of the value rules, one value at a time (log/trace-rules.md: EXPR, VALUE; "Difference propagation on
every value"). Each takes the values new to its source and, for each, adds what the rule gives to the target: constants
at once, and the values of the nodes the rule reads (a member's held values, a function's returns) by a subscription,
so their later values arrive as they come. Every rule here is distributive over its source's values.

`[]`, a range-for, `->`, `*`: an element of a container, what a pointer points at, or the operator of a class of the
program (its template parameters replaced); a member of a value: its field's values, its method's callable or result;
calling a callable: what its function returns.
"""
from __future__ import annotations

from . import js_library
from .cpp_names import template_args
from .library import (DEREF, HELD_CALLS, ITEM, ITER, ITERATOR_CALLS, PAIR_PARTS, clean, library_step)
from .rules import ITERATOR_MEMBER
from .solver import CLASS, ELEM, ENGINE, FN, TEXT

OPERATORS = {ITEM: "operator[]", DEREF: "operator->", ITER: "begin", js_library.EACH: "begin"}
COMPARISONS = ("==", "!=", "<", ">", "<=", ">=", "<=>", "&&", "||", "!", "and", "or", "not", "in", "is")
CALLABLE_CALLS = ("call", "callv", "call_deferred", "apply")
BIND_CALLS = ("bind", "unbind", "bindv")


def substituted(t, key: str, outer: str, ctx, iterate: bool):
    """The values of a class template's member, its parameters replaced by the arguments `outer` writes (and, for a
    range-for, what the iterator points at)."""
    def transfer(target, values):
        out = []
        for value, where in values:
            value, replaced = t.substitute(key, outer, value)
            if iterate:
                value = library_step(DEREF, value, False) if template_args(clean(value)) else value
            out.append((value, ctx if replaced else where))
        t.add(target, out)
    return transfer


def members(t, name: str, called: bool):
    def transfer(target, values):
        for text, ctx in values:
            member_one(t, target, text, ctx, name, called)
    return transfer


def member_one(t, target, text: str, ctx, name: str, called: bool) -> None:
    """A member of one value: its field's held values, its method's callable, or (called) the method's returns."""
    ix = t.ix
    if text.startswith(FN):
        unit = ix.units[int(text[len(FN):])]
        if not called and name in PAIR_PARTS:
            t.add(target, [(text if name == "second" else ENGINE, ctx)])       # (library.PAIR_PARTS)
        elif called and name in CALLABLE_CALLS:
            t.follow(t.ret_node(unit), target)
        else:
            t.add(target, [(text, ctx) if name in BIND_CALLS else (ENGINE, ctx)])
        return
    if text.startswith(ELEM):
        # what a container holds: its access calls give an element, its search calls an iterator over them (JS: the
        # element calls give an element, the calls that give the container give it again, T2)
        element = js_library.ELEMENT_CALLS if ctx.lang == "js" else HELD_CALLS + ("at",)
        same = js_library.SAME_CALLS if ctx.lang == "js" else ITERATOR_CALLS
        if called and name in element or ctx.lang == "js" and not called and name == js_library.RESULT_VALUE:
            t.add(target, [(text[len(ELEM):], ctx)])    # (an iterator's `next()` is the container: its `value` one)
        elif called and name in same:
            t.add(target, [(text, ctx)])
        return
    if ctx.lang == "js" and js_library.is_container(text):
        t.add(target, [(v, ctx) for v in js_library.member(name, text, called)])
        return
    key = t.type_class(text, ctx)
    if called and name == "new":
        t.add(target, [(CLASS + key, ctx) if key else (text, ctx)])            # `T.new()` constructs a T (EXPR: new)
        return
    if key is not None and ix.js.is_module(key):                              # a namespace's member: its export (J3)
        gids, modules = ix.js.export(key, name)
        t.add(target, [(CLASS + m, ctx) for m in modules] or ([] if gids else [(ENGINE, ctx)]))
        for gid in gids:
            unit = ix.units[gid]
            if unit.kind == "class":
                t.add(target, [(CLASS + unit.qual, ctx)])
            else:
                t.follow(t.ret_node(unit) if called else t.hold_node(unit), target)
        return
    if key is None:
        t.add(target, [(library_step(name, text, called), ctx) if text != ENGINE else (ENGINE, ctx)])
        return
    found = ix.lookup(key, name)
    if ctx.lang == "gd" and ix.classes[key].lang == "cpp":
        found = t.rs.engine_members(key, name, "func" if called else "read")
    if not found:
        if not called and name in PAIR_PARTS and ix.classes[key].lang == "cpp":
            t.add(target, [(text if name == "second" else ENGINE, ctx)])
            return
        inner = ix.inner[key].get(name)
        t.add(target, [(CLASS + inner, ctx) if inner else (ENGINE, ctx)])      # an engine or library base's member
        return
    for gid in ix.definitions(found):
        unit = ix.units[gid]
        source = t.ret_node(unit) if called else t.hold_node(unit)
        t.subscribe(source, target, ("subst", key, text, ctx.gid, False), substituted(t, key, text, ctx, False))


def steps(t, step: str):
    def transfer(target, values):
        for text, ctx in values:
            step_one(t, target, step, text, ctx)
    return transfer


def step_one(t, target, step: str, text: str, ctx) -> None:
    """`[]`, a range-for, `->`: an element of a container, what a pointer points at."""
    if text.startswith(ELEM):
        t.add(target, [(text[len(ELEM):], ctx)])        # an element: by index, by a walk, through an iterator
        return
    if text.startswith(FN) and step == DEREF:
        t.add(target, [(text, ctx)])                    # `*p` of a pointer to a function: the function ([expr.unary.op])
        return
    if text == ENGINE or text.startswith(FN):
        t.add(target, [(ENGINE, ctx)])
        return
    if ctx.lang == "js" and js_library.is_container(text):
        t.add(target, [(v, ctx) for v in js_library.step(step, text)])     # the language's containers (T2)
        return
    key = t.type_class(text, ctx)
    if key is None:
        t.add(target, [(library_step(step, text, False), ctx)])
        return
    if ctx.lang == "js":    # an object of the program: `o[k]` the members its key names (`indexed`, T4'); its
        if step == ITER:    # iteration what its own iterator gives (J5); a callback handed to its own forEach gets
            t.follow(iterated(t, key), target)      # what that method passes
        return
    found = t.ix.lookup(key, OPERATORS[step])
    if not found:
        t.add(target, [(text, ctx)])                    # a raw pointer's `->`, `*` on a pointer: the object itself
        return
    for gid in t.ix.definitions(found):
        t.subscribe(t.ret_node(t.ix.units[gid]), target, ("subst", key, text, ctx.gid, step == ITER),
                    substituted(t, key, text, ctx, step == ITER))


def iterated(t, key: str):
    """What iterating a JS object of class `key` gives (J5): its `[Symbol.iterator]()`'s `next().value`."""
    node = ("#iterated", key)
    if node not in t.wired:
        t.wired.add(node)
        t.value.setdefault(node, [])
        iterator = ("#iterator", key)
        for gid in t.ix.definitions(t.ix.lookup(key, ITERATOR_MEMBER)):
            t.subscribe(t.ret_node(t.ix.units[gid]), iterator, ("member", "next", True), members(t, "next", True))
        t.subscribe(iterator, node, ("member", "value", False), members(t, "value", False))
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
                    member_one(t, target, text, ctx, name, False)
            elif from_base:                             # a container: its elements, whatever the key
                step_one(t, target, ITEM, text, ctx)
    return transfer


def operators(t, op: str):
    def transfer(target, values):
        for text, ctx in values:
            operator_one(t, target, op, text, ctx)
    return transfer


def operator_one(t, target, op: str, text: str, ctx) -> None:
    """A binary or unary operator on a value (its left operand): a comparison is the language's; on a class of the
    program its operator's result (`*` too: multiplication, O2; dereference is the `deref` shape); else the value."""
    if op in COMPARISONS:
        t.add(target, [(ENGINE, ctx)])
        return
    key = t.type_class(text, ctx)
    if key and ctx.lang == "cpp":
        found = t.ix.lookup(key, "operator" + op)
        if found:
            t.follow(t.ret_node(t.ix.units[t.ix.definitions(found)[0]]), target)
            return
    t.add(target, [(text, ctx)])


def calls(t):
    def transfer(target, values):
        for text, ctx in values:
            if text.startswith(FN):
                t.follow(t.ret_node(t.ix.units[int(text[len(FN):])]), target)
            elif not text.startswith(ELEM):
                t.add(target, [(ENGINE, ctx)])          # calling what the engine gives: the engine's value
    return transfer


def elements(t):
    """A value put into a container: what its elements hold."""
    def transfer(target, values):
        t.add(target, [(ELEM + text, ctx) for text, ctx in values if not text.startswith(ELEM)])
    return transfer


def awaits(t):
    """`await x`, `then`'s callback: what each value resolves to (a JS promise's type argument, T2; else itself)."""
    def transfer(target, values):
        t.add(target, [(v, ctx) for text, ctx in values
                       for v in (js_library.awaited(text) if ctx.lang == "js" else [text])])
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
