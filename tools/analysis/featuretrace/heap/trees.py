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
                     KEYS, LITERAL, MEMBER, NAME, NARROW, OP, SELF, SITE_CLASS, SITE_TEXT, STRNAME, SUPER, UNARY,
                     UNION)
from . import contexts, members, names, protocols

LITERAL_MADE, CLASS_MADE = 1, 2           # `ClassFact.made` (js_class): an object literal, a class expression
STRING_KIND = truth.KINDS[STRING]


def wire(an, node, tree, unit, ctx) -> None:
    kind = tree[0]
    solver = an.solver
    if kind == NAME:
        names.wire_name(an, node, unit, tree[1], ctx)
    elif kind == MEMBER:
        name = tree[2]
        if tree[1] == (SUPER,):                                  # `super.name`: the base classes' member, on `this`
            bases = base_keys(an, unit)
            solver.subscribe(this_of(an, unit, ctx), node, (SUPER, name), lambda t, objs: [
                members.declared_read(an, t, o, k, name) for o in objs for k in bases])
            return
        solver.subscribe(an.node(tree[1], unit, ctx), node, (MEMBER, name), lambda t, objs: [
            members.read(an, t, o, name) for o in objs])
    elif kind == INDEX:
        wire_index(an, node, tree, unit, ctx)
    elif kind == CALL:
        wire_call(an, node, tree, unit, ctx)
    elif kind == CONSTRUCT:
        site = tree[2] if len(tree) > 2 else -1
        solver.subscribe(an.node(tree[1], unit, ctx), node, (CONSTRUCT,), lambda t, objs: solver.add(t, [
            made(an, o, unit.file, site, ctx) for o in objs if an.made_by(o)]))
    elif kind == CALLABLE:
        gid = an.ix.offset[unit.file] + tree[1]                  # made here: this run is its environment
        solver.add(node, [an.function_object(gid, names.scope(an, unit, ctx, an.ix.units[gid].outer))])
    elif kind == CAST:
        wire_cast(an, node, tree[1], unit, ctx)
    elif kind == COLLECTION:
        array = an.objects.array(unit.file, tree[2] if len(tree) > 2 else id(tree), ctx)
        solver.add(node, [array])
        for value in tree[1]:
            solver.follow(an.node(value, unit, ctx), members.field(an, array, ELEMENTS_FIELD))
    elif kind in (ELEMENTS, EACH):
        protocols.iterate(an, node, an.node(tree[1], unit, ctx))
    elif kind in (UNION,):
        solver.follow(an.node(tree[1], unit, ctx), node)
        solver.follow(an.node(tree[2], unit, ctx), node)
    elif kind == AWAIT:
        protocols.await_(an, node, an.node(tree[1], unit, ctx))
    elif kind == NARROW:
        solver.follow(an.node(tree[1], unit, ctx), node)
    elif kind == SELF:
        solver.follow(this_of(an, unit, ctx), node)
    elif kind == LITERAL:
        solver.add(node, [an.objects.primitive(v) for v in literal_kinds(tree[1])])
    elif kind == STRNAME:
        solver.add(node, [an.objects.make(SITE_TEXT, tree[1])])
    elif kind in (OP, UNARY):
        solver.add(node, [an.objects.primitive(v[len(LANG):]) for v in truth.operator_values(tree[1], kind == UNARY)
                          if v.startswith(LANG)])
    elif kind == KEYS:
        solver.add(node, [an.objects.primitive(STRING_KIND)])


def wire_index(an, node, tree, unit, ctx) -> None:
    """`o[k]`: each object with each key: a text names its property, any other key reads every property."""
    solver = an.solver
    base, key = an.node(tree[1], unit, ctx), an.node(tree[2], unit, ctx)

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


def wire_call(an, node, tree, unit, ctx) -> None:
    """A call: each function the callee can be, on the receiver it was read from (a member call)."""
    callee, args = tree[1], [an.node(a, unit, ctx) for a in tree[2]]
    solver = an.solver
    if callee[0] == MEMBER and callee[1] != (SUPER,):
        name = callee[2]

        def by_receiver(target, objs):
            for obj in objs:
                held = (MEMBER, obj, name)
                members.read(an, held, obj, name)
                solver.subscribe(held, target, (CALL, obj), lambda t, fns, obj=obj: [
                    an.call(t, fn, obj, args, CALL) for fn in fns])
        solver.subscribe(an.node(callee[1], unit, ctx), node, (CALL, name), by_receiver)
    elif callee[0] == MEMBER:                                    # `super.name(...)`, `super(...)`: on `this`
        name, bases = callee[2], base_keys(an, unit)

        def by_this(target, objs):
            for obj in objs:
                held = (SUPER, obj, name, unit.gid)
                for k in bases:
                    members.declared_read(an, held, obj, k, name)
                solver.subscribe(held, target, (CALL, obj), lambda t, fns, obj=obj: [
                    an.call(t, fn, obj, args, CALL) for fn in fns])
        solver.subscribe(this_of(an, unit, ctx), node, (SUPER, name), by_this)
    else:
        solver.subscribe(an.node(callee, unit, ctx), node, (CALL,), lambda t, fns: [
            an.call(t, fn, None, args, CALL) for fn in fns])


def wire_cast(an, node, text: str, unit, ctx) -> None:
    """A value written in the code of a class made there: an object literal's object, a class expression's class;
    any other cast is a declared type, no value of its own."""
    if not text.startswith(CLASS_VALUE):
        return
    key = text[len(CLASS_VALUE):]
    made = an.ix.classes[key].made if key in an.ix.classes else -1
    if made == LITERAL_MADE:
        obj = an.objects.literal(key, ctx)
        an.solver.add(node, [obj])
        literal_methods(an, obj, key, unit, ctx)
    elif made == CLASS_MADE:
        an.solver.add(node, [an.objects.make(SITE_CLASS, key)])


def literal_methods(an, obj: int, key: str, unit, ctx) -> None:
    """An object literal's methods are closures made where the literal is evaluated, each a property of the object
    (ECMA-262 PropertyDefinitionEvaluation, DefineMethod): their environment is the run that evaluates it."""
    for name, gids in an.ix.members[key].items():
        for gid in gids:
            method = an.ix.units[gid]
            if an.is_function_unit(gid) and not method.accessor_of:
                env = names.scope(an, unit, ctx, method.outer)
                an.solver.add(members.field(an, obj, name), [an.function_object(gid, env)])


def this_of(an, unit, ctx):
    """`this` where it is read: an arrow's or a lambda's is its enclosing function's (lexical), any other its own."""
    owner, owner_ctx = lexical(an, unit, ctx)
    return an.this(owner.gid, owner_ctx)


def lexical(an, unit, ctx=contexts.ROOT):
    """The function whose `this` and `super` a function reads, and its context: itself, or for an arrow its
    enclosing function's, read in the arrow's environment."""
    while unit.this_of == ARROW_THIS and unit.outer >= 0:
        unit, ctx = an.ix.units[an.ix.offset[unit.file] + unit.outer], contexts.environment(ctx)
    return unit, ctx


def base_keys(an, unit) -> list[str]:
    """The classes `super` names where it is written: the bases of the class its function belongs to."""
    owner = lexical(an, unit)[0].owner
    return an.ix.base_keys(an.ix.classes[owner]) if owner in an.ix.classes else []


def wire_local(an, node, unit, name: str, ctx) -> None:
    """A local or a parameter: every value its declarations and assignments give it (a declared type none), every
    argument of a call for a parameter; a captured name is the enclosing function's, in this run's environment, and
    where the language shares it (`written_by`) this closure's writes are that variable's too."""
    solver, ix = an.solver, an.ix
    if name in unit.params:
        solver.follow(an.param(unit.gid, unit.params.index(name), ctx), node)
    for tree in unit.types.get(name, ()):
        if tree and tree[0] != CAST or tree and is_made(an, tree[1]):
            solver.follow(an.node(tree, unit, ctx), node)
    if name in unit.captured and unit.outer >= 0:
        owner = ix.units[ix.offset[unit.file] + unit.outer]
        outer = an.local(owner, name, contexts.environment(ctx))
        solver.follow(outer, node)
        if unit.local_id in owner.written_by.get(name, ()):
            solver.follow(node, outer)


def made(an, obj: int, file: str, site: int, ctx) -> int:
    """The object `new` on `obj` makes at a site in a context (a constructor function's knows its function)."""
    return an.objects.instance(an.made_by(obj), file, site, obj if an.objects.function(obj) is not None else -1, ctx)


def is_made(an, text: str) -> bool:
    key = text[len(CLASS_VALUE):] if text.startswith(CLASS_VALUE) else None
    return key is not None and key in an.ix.classes and an.ix.classes[key].made > 0


def literal_kinds(kind: str) -> list[str]:
    """The kinds of value a literal is, by the language's constants and literals (`truth`, V1, T1'')."""
    values = truth.LITERAL_VALUES.get(kind) or ([truth.JS_CONSTANTS[kind]] if kind in truth.JS_CONSTANTS else [])
    return [v[len(LANG):] for v in values if v.startswith(LANG)] or ([kind] if kind else [])
