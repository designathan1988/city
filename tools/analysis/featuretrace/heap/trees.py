"""The value of each shape of expression tree (log/trace-core-plan.md, Task 1; `ir`): a name what it names, a member
what each object's property holds, a call what each function it can run returns, `new` the object made at that site,
a literal its object (an object literal, an array, a function, a primitive of its kind, a string's text), either side of
a union. A declared type is no value: a cast of a class no code makes there is the declarations' (`log/trace-core-
plan.md`, Task 4). The tree shapes are the tracer's own (`vocab`); no name of a program or platform is read here.
"""
from __future__ import annotations

from .. import truth
from ..ir_js import CLASS_VALUE
from ..js_functions import ARROW as ARROW_THIS
from ..solver import LANG, STRING
from ..vocab import (AWAIT, CALL, CALLABLE, CAST, COLLECTION, CONSTRUCT, EACH, ELEMENTS, ELEMENTS_FIELD, INDEX,
                     KEYS, LITERAL, MEMBER, NAME, NARROW, OP, SELF, SITE_CLASS, SITE_LITERAL, SITE_NEW, SITE_TEXT,
                     STRNAME, SUPER, UNARY, UNION)
from . import members, names

NAME_SHAPE = NAME
LITERAL_MADE, CLASS_MADE = 1, 2           # `ClassFact.made` (js_class): an object literal, a class expression
STRING_KIND = truth.KINDS[STRING]


def wire(an, node, tree, unit) -> None:
    kind = tree[0]
    solver = an.solver
    if kind == NAME:
        names.wire_name(an, node, unit, tree[1])
    elif kind == MEMBER:
        if tree[1] == (SUPER,):
            return                                               # the base class's member (Task 2)
        name = tree[2]
        solver.subscribe(an.node(tree[1], unit), node, (MEMBER, name), lambda t, objs: [
            members.read(an, t, o, name) for o in objs])
    elif kind == INDEX:
        wire_index(an, node, tree, unit)
    elif kind == CALL:
        wire_call(an, node, tree, unit)
    elif kind == CONSTRUCT:
        site = tree[2] if len(tree) > 2 else -1
        solver.subscribe(an.node(tree[1], unit), node, (CONSTRUCT,), lambda t, objs: solver.add(t, [
            an.objects.make(SITE_NEW, k, unit.file, site) for k in map(an.objects.class_itself, objs) if k]))
    elif kind == CALLABLE:
        solver.add(node, [an.function_object(an.ix.offset[unit.file] + tree[1])])
    elif kind == CAST:
        wire_cast(an, node, tree[1])
    elif kind == COLLECTION:
        array = an.objects.array(unit.file, tree[2] if len(tree) > 2 else id(tree))
        solver.add(node, [array])
        for value in tree[1]:
            solver.follow(an.node(value, unit), members.field(an, array, ELEMENTS_FIELD))
    elif kind in (ELEMENTS, EACH):
        solver.subscribe(an.node(tree[1], unit), node, (ELEMENTS,), lambda t, objs: [
            solver.follow(members.field(an, o, ELEMENTS_FIELD), t) for o in objs])
    elif kind in (UNION,):
        solver.follow(an.node(tree[1], unit), node)
        solver.follow(an.node(tree[2], unit), node)
    elif kind in (NARROW, AWAIT):
        solver.follow(an.node(tree[1], unit), node)
    elif kind == SELF:
        solver.follow(this_of(an, unit), node)
    elif kind == LITERAL:
        solver.add(node, [an.objects.primitive(v) for v in literal_kinds(tree[1])])
    elif kind == STRNAME:
        solver.add(node, [an.objects.make(SITE_TEXT, tree[1])])
    elif kind in (OP, UNARY):
        solver.add(node, [an.objects.primitive(v[len(LANG):]) for v in truth.operator_values(tree[1], kind == UNARY)
                          if v.startswith(LANG)])
    elif kind == KEYS:
        solver.add(node, [an.objects.primitive(STRING_KIND)])


def wire_index(an, node, tree, unit) -> None:
    """`o[k]`: each object with each key: a text names its property, any other key reads every property."""
    solver = an.solver
    base, key = an.node(tree[1], unit), an.node(tree[2], unit)

    def by_object(target, objs):
        for obj in objs:
            solver.subscribe(key, target, (INDEX, obj), lambda t, keys, obj=obj: by_key(t, obj, keys))

    def by_key(target, obj, keys):
        for k in keys:
            text = an.objects.text(k)
            if text is not None:
                members.read(an, target, obj, text)
            else:
                members.read_all(an, target, obj)
    solver.subscribe(base, node, (INDEX,), by_object)


def wire_call(an, node, tree, unit) -> None:
    """A call: each function the callee can be, on the receiver it was read from (a member call)."""
    callee, args = tree[1], [an.node(a, unit) for a in tree[2]]
    solver = an.solver
    if callee[0] == MEMBER and callee[1] != (SUPER,):
        name = callee[2]

        def by_receiver(target, objs):
            for obj in objs:
                held = (MEMBER, obj, name)
                members.read(an, held, obj, name)
                solver.subscribe(held, target, (CALL, obj), lambda t, fns, obj=obj: [
                    an.call(t, fn, obj, args, CALL) for fn in fns])
        solver.subscribe(an.node(callee[1], unit), node, (CALL, name), by_receiver)
    elif callee[0] != MEMBER:
        solver.subscribe(an.node(callee, unit), node, (CALL,), lambda t, fns: [
            an.call(t, fn, None, args, CALL) for fn in fns])


def wire_cast(an, node, text: str) -> None:
    """A value written in the code of a class made there: an object literal's object, a class expression's class;
    any other cast is a declared type, no value of its own."""
    if not text.startswith(CLASS_VALUE):
        return
    key = text[len(CLASS_VALUE):]
    made = an.ix.classes[key].made if key in an.ix.classes else -1
    if made == LITERAL_MADE:
        an.solver.add(node, [an.objects.make(SITE_LITERAL, key)])
    elif made == CLASS_MADE:
        an.solver.add(node, [an.objects.make(SITE_CLASS, key)])


def this_of(an, unit):
    """`this` where it is read: an arrow's or a lambda's is its enclosing function's (lexical), any other its own."""
    while unit.this_of == ARROW_THIS and unit.outer >= 0:
        unit = an.ix.units[an.ix.offset[unit.file] + unit.outer]
    return an.this(unit.gid)


def wire_local(an, node, unit, name: str) -> None:
    """A local or a parameter: every value its declarations and assignments give it (a declared type none), every
    argument of a call for a parameter, the enclosing function's for a captured name, a sharing closure's writes."""
    solver, ix = an.solver, an.ix
    if name in unit.params:
        solver.follow(an.param(unit.gid, unit.params.index(name)), node)
    for tree in unit.types.get(name, ()):
        if tree and tree[0] != CAST or tree and is_made(an, tree[1]):
            solver.follow(an.node(tree, unit), node)
    if name in unit.captured and unit.outer >= 0:
        solver.follow(an.local(ix.units[ix.offset[unit.file] + unit.outer], name), node)
    for writer in unit.written_by.get(name, ()):
        solver.follow(an.local(ix.units[ix.offset[unit.file] + writer], name), node)


def is_made(an, text: str) -> bool:
    key = text[len(CLASS_VALUE):] if text.startswith(CLASS_VALUE) else None
    return key is not None and key in an.ix.classes and an.ix.classes[key].made > 0



def literal_kinds(kind: str) -> list[str]:
    """The kinds of value a literal is, by the language's constants and literals (`truth`, V1, T1'')."""
    values = truth.LITERAL_VALUES.get(kind) or ([truth.JS_CONSTANTS[kind]] if kind in truth.JS_CONSTANTS else [])
    return [v[len(LANG):] for v in values if v.startswith(LANG)] or ([kind] if kind else [])
