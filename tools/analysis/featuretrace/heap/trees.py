"""The value of each shape of expression tree (log/trace-core-plan.md, Task 1; `ir`): a name what it names, a member
what each object's property holds, a call what each function it can run returns, `new` the object made at that site,
a literal its object (an object literal, an array, a function, a primitive of its kind, a string's text), either side of
a union. A declared type is no value: a cast of a class no code makes there is the declarations' (`log/trace-core-
plan.md`, Task 4). The tree shapes are the tracer's own (`vocab`); no name of a program or platform is read here.
"""
from __future__ import annotations

from .. import truth
from ..ir_js import CLASS_VALUE
from ..js_class import MADE_CLASS as CLASS_MADE, MADE_LITERAL as LITERAL_MADE
from ..js_functions import ARROW as ARROW_THIS
from ..solver import LANG, STRING
from ..rules import CONSTRUCTOR
from ..vocab import (AWAIT, CALL, CALLABLE, CAST, COLLECTION, CONSTRUCT, EACH, ELEMENTS, ELEMENTS_FIELD, INDEX,
                     JSX, KEYS, LITERAL, MEMBER, NAME, NARROW, NEW, OP, SELF, SITE_CLASS, SITE_PRIMITIVE, SITE_TEXT,
                     SITE_KEY, STRNAME, SUPER, UNARY, UNION)
from ..model import STATE_KINDS
from . import contexts, declared, guards, jsx, members, names, protocols, texts, typesys

CONSTRUCTED = 10           # a node: what a constructor's call returns (the `new` itself is the object made)

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
        construct(an, node, tree, unit, ctx)
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
    elif kind == NARROW:                          # what a test proves where it holds (`guards`)
        guards.wire(an, node, tree, unit, ctx)
    elif kind == SELF:
        solver.follow(this_of(an, unit, ctx), node)
    elif kind == LITERAL:
        solver.add(node, [an.objects.primitive(v) for v in literal_kinds(tree[1])])
    elif kind == STRNAME:                          # a string literal: the value of its own text (F33)
        solver.add(node, [an.objects.make(SITE_TEXT, tree[1])])
    elif kind in (OP, UNARY):
        solver.add(node, [an.objects.primitive(v[len(LANG):]) for v in truth.operator_values(tree[1], kind == UNARY)
                          if v.startswith(LANG)])
    elif kind == JSX:
        jsx.value(an, node, tree, unit, ctx)
    elif kind == KEYS:                             # `for (k in o)`: a key of each object o (a string)
        solver.add(node, [an.objects.primitive(STRING_KIND)])
        solver.subscribe(an.node(tree[1], unit, ctx), node, (KEYS,), lambda t, objs: solver.add(t, [
            an.objects.make(SITE_KEY, o) for o in objs]))


def wire_index(an, node, tree, unit, ctx) -> None:
    """`o[k]`: the property each text k can have names (`texts`); where its texts are not known, what each value of k
    names (`members.load_by_key`: an index the elements, a key enumerating that same object every member, any other
    key the object as a dictionary)."""
    key = an.node(tree[2], unit, ctx)
    known = texts.of(an, tree[2], unit)
    solver = an.solver

    def by_object(target, objs):
        for o in objs:
            if known is not None:
                for text in known:
                    members.read(an, target, o, text)
                continue
            solver.subscribe(key, target, (INDEX, o), lambda t, keys, o=o: [
                members.load_by_key(an, t, o, k) for k in keys])
    solver.subscribe(an.node(tree[1], unit, ctx), node, (INDEX,), by_object)


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
        an.instance_made(obj)
        literal_methods(an, obj, key, unit, ctx)
        members.initialize_literal(an, obj, key, ctx)
    elif made == CLASS_MADE:                       # evaluated in this run: its methods are closures of it
        an.solver.add(node, [an.objects.class_value(key, ctx, unit.gid)])


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
    """The function whose `this` and `super` a function reads, and its context: itself, or for an arrow or an
    object literal's property (whose value is evaluated around it) its enclosing function's, read in its
    environment."""
    while (unit.this_of == ARROW_THIS or literal_property(an, unit)) and unit.outer >= 0:
        unit, ctx = an.ix.units[an.ix.offset[unit.file] + unit.outer], contexts.environment(ctx)
    return unit, ctx


def literal_property(an, unit) -> bool:
    """Whether a unit is an object literal's property (its value is evaluated where the literal is)."""
    cls = an.ix.classes.get(unit.owner)
    return unit.kind in STATE_KINDS and cls is not None and cls.made == LITERAL_MADE


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


def construct(an, node, tree, unit, ctx) -> None:
    """`new C(args)` (ECMA-262 EvaluateNew, [[Construct]]): for each value C can be, the object made at the site
    with C's constructor (a class's declared one, or the constructor function itself) run on it, and an object the
    constructor returns instead; on an object of a declared type, its construct signatures."""
    solver = an.solver
    site = tree[2] if len(tree) > 2 else -1
    args = [an.node(a, unit, ctx) for a in (tree[3] if len(tree) > 3 else ())]

    def by_value(target, objs):
        for obj in objs:
            key = an.made_by(obj)
            call = constructed(node, key or obj)
            if not key:
                declared.construct(an, call, obj, args)
                solver.follow(call, target)
                continue
            instance = made(an, obj, unit.file, site, ctx)
            solver.add(target, [instance])
            ctors = [obj] if an.objects.function(obj) is not None else [
                an.function_object(g, members.method_env(an, obj, g)) for g in typesys.lookup(an, key, CONSTRUCTOR)
                if an.is_function_unit(g)]
            for fn in ctors:
                an.call(call, fn, instance, args, NEW)
            solver.subscribe(call, target, (CONSTRUCT, key), lambda t, returned: solver.add(t, [
                o for o in returned if an.objects.kind(o) not in (SITE_PRIMITIVE, SITE_TEXT)]))
    solver.subscribe(an.node(tree[1], unit, ctx), node, (CONSTRUCT,), by_value)


def constructed(node, made) -> tuple:
    """The call node of the constructor a `new` tree's node runs for what C is (a class key, a declared object)."""
    return (CONSTRUCTED, node[1], node[2], made, node[3])


def made(an, obj: int, file: str, site: int, ctx) -> int:
    """The object `new` on `obj` makes at a site in a context (one a constructor function or a class evaluated in a
    run made knows that function or class object)."""
    objects = an.objects
    maker = obj if objects.function(obj) is not None or objects.class_env(obj) is not None else -1
    made = objects.instance(an.made_by(obj), file, site, maker, ctx)
    an.instance_made(made)
    members.initialize(an, made)
    return made


def is_made(an, text: str) -> bool:
    key = text[len(CLASS_VALUE):] if text.startswith(CLASS_VALUE) else None
    return key is not None and key in an.ix.classes and an.ix.classes[key].made > 0


def literal_kinds(kind: str) -> list[str]:
    """The kinds of value a literal is, by the language's constants and literals (`truth`, V1, T1'')."""
    values = truth.LITERAL_VALUES.get(kind) or ([truth.JS_CONSTANTS[kind]] if kind in truth.JS_CONSTANTS else [])
    return [v[len(LANG):] for v in values if v.startswith(LANG)] or ([kind] if kind else [])
