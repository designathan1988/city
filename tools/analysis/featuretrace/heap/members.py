"""Members of one object (log/trace-core-plan.md, Tasks 1-3; ECMA-262 [[Get]] and [[Set]] through the prototype
chain): a read gives what was written to that object's property, what its class's chain declares under the name (a
method's function object, an instance field's initializers, an accessor's getter run on the object), what was written
on the prototype objects of its chain (a class's, the prototype object of the function object that made it or the
object its `prototype` was set to: each function object its own), what a write through an unknown key put in it;
`constructor` its class or that function object; a class or a function as a value its own
properties (a class's statics, its prototype object). A write puts the value in that object's property, or runs the
setter its class declares. A read through a key no text names reads every property.
"""
from __future__ import annotations

from ..js_transfer import chain
from ..languages import GETTER, PROTOTYPE, PROTOTYPE_CONSTRUCTOR, SETTER
from ..model import STATE_KINDS
from ..vocab import (CALL, ELEMENTS_FIELD, READ, SITE_CLASS, SITE_DECLARED, SITE_LITERAL, SITE_MODULE, SITE_PROTOTYPE,
                     SITE_UNKNOWN, WRITE)

from . import contexts, names, typesys, values  # noqa: E402  (they read members: imported once it is defined)

ALL, ACCESSOR, PROTOTYPES, PROPERTY = 9, 11, 12, 13    # nodes: every property of one object; an accessor's call; a
                                                     # prototype; a site: a declared property's


def read(an, target, obj: int, name: str) -> None:
    """Member `name` of object `obj` into `target`: a module namespace's export; anything the platform's unknown
    object holds is the unknown object again."""
    ix, objects = an.ix, an.objects
    kind = objects.kind(obj)
    if kind == SITE_MODULE:
        gids, modules = ix.js.export(objects.keys[obj][1], name)
        for gid in gids:
            names.value_of(an, gid, target)
        an.solver.add(target, [objects.make(SITE_MODULE, m) for m in modules])
        return
    if kind == SITE_UNKNOWN:
        an.solver.add(target, [obj])
        return
    key, maker = objects.class_of(obj), objects.maker(obj)
    for own in typesys.own_types(an, obj):        # its class's or declared type's members, else its intrinsic's
        if declared_read(an, target, obj, own, name):
            break
    for k in chain(ix, key) if key is not None and key in ix.classes else []:
        an.solver.follow(field(an, prototype(an, k, maker if k == key else None), name), target)
    if maker is not None:
        made_from(an, target, maker, name)
    if key is not None and name == PROTOTYPE_CONSTRUCTOR:
        an.solver.add(target, [maker if maker is not None else an.objects.make(SITE_CLASS, key)])
    statics(an, target, obj, name)
    an.solver.follow(field(an, obj, name), target)
    an.solver.follow(an.any_field(obj), target)


def declared_read(an, target, obj: int, key: str, name: str) -> bool:
    """What class `key`'s chain declares under `name`, read on `obj` (an instance, or `this` for `super.name`): a
    method's function object, an instance field's initializers, its getter run on `obj`, a declared property what
    its type gives. Whether the chain declares the name."""
    ix = an.ix
    found = typesys.lookup(an, key, name) if key in ix.classes else []
    own = an.objects.kind(obj) == SITE_LITERAL and an.objects.class_of(obj)
    for gid in found:
        unit = ix.units[gid]
        if an.is_function_unit(gid):
            if unit.owner != own:                     # an object literal's own methods are its properties
                an.solver.add(target, [an.function_object(gid, method_env(an, obj, gid))])
        elif unit.kind in STATE_KINDS and not unit.static:
            getter = unit.runs(GETTER, None)
            if getter is not None:
                accessor(an, target, obj, unit, getter, [])
            elif typesys.declared(an, gid) and unit.result_type:
                site = ((PROPERTY, gid), an.objects.heap(obj))
                an.solver.follow(values.produce(an, unit.result_type, environment(an, obj, unit.owner), site), target)
            else:
                an.solver.follow(an.initial(gid), target)
    return bool(found)


def method_env(an, obj: int, gid: int):
    """The environment of a class's method read on `obj` (an instance, or the class itself): the run its class was
    evaluated in, seen from the function the method is written in; ROOT for a class of a module."""
    unit, objects = an.ix.units[gid], an.objects
    cls = obj if objects.kind(obj) == SITE_CLASS else objects.maker(obj)
    found = objects.class_env(cls) if cls is not None and unit.outer >= 0 else None
    if found is None:
        return contexts.ROOT
    env, holder = found
    return names.scope(an, an.ix.units[holder], env, unit.outer)


def environment(an, obj: int, owner: str):
    """The environment a member of `owner` read on `obj` reads its types in: an object type written in place keeps
    the one it was made in; any other, the object's places of `owner`'s type parameters."""
    env = an.objects.environment(obj)
    if env is not None and an.objects.kind(obj) == SITE_DECLARED and an.objects.class_of(obj) == owner:
        return env
    return (obj, typesys.identity(an, owner), None)


def statics(an, target, obj: int, name: str) -> None:
    """A class or a function as a value: a class's static members and its prototype object, a constructor function's
    prototype object."""
    ix, objects = an.ix, an.objects
    cls = objects.class_itself(obj)
    for gid in typesys.lookup(an, cls, name) if cls is not None and cls in ix.classes else []:
        if not ix.units[gid].static:
            continue
        if an.is_function_unit(gid):
            an.solver.add(target, [an.function_object(gid, method_env(an, obj, gid))])
        elif ix.units[gid].kind in STATE_KINDS:
            an.solver.follow(an.hold(gid), target)
    if name != PROTOTYPE:
        return
    gid = objects.function(obj)
    if cls is not None:
        an.solver.add(target, [prototype(an, cls)])
    elif gid is not None and ix.units[gid].constructs:
        an.solver.add(target, [prototype(an, ix.units[gid].constructs, obj)])


def made_from(an, target, maker: int, name: str) -> None:
    """An object the function object `maker` made: the members of the objects its `prototype` was set to."""
    held = field(an, maker, PROTOTYPE)
    an.solver.subscribe(held, target, (PROTOTYPES, name), lambda t, protos: [read(an, t, p, name) for p in protos])


def accessor(an, target, obj: int, prop, local_id, args: list) -> None:
    """A property's accessor (`get`, `set`) run on `obj`: its result into `target`."""
    gid = an.ix.offset[prop.file] + local_id if isinstance(local_id, int) else None
    if gid is None:
        return
    node = (ACCESSOR, obj, gid)
    an.call(node, an.function_object(gid), obj, args, CALL)
    an.solver.follow(node, target)


def read_all(an, target, obj: int) -> None:
    """What a key no text names reads (log/trace-research.md F14; T4' of the old core): the object as a dictionary —
    its elements and what writes through unknown keys put in it — never its named members, which code reaches by name
    (ACG and Jelly leave such reads unresolved; reading every member of every object a generic helper is handed made
    every set of the program large)."""
    an.solver.follow(field(an, obj, ELEMENTS_FIELD), target)
    an.solver.follow(an.any_field(obj), target)


def write(an, source, obj: int, name: str) -> None:
    """`obj.name = value`: the value in that object's property, or its class's setter run with it (a class's static
    field: what it holds)."""
    ix, objects = an.ix, an.objects
    cls = objects.class_itself(obj)
    for gid in ix.lookup(cls, name) if cls is not None and cls in ix.classes else []:
        if ix.units[gid].static and ix.units[gid].kind in STATE_KINDS:
            an.solver.follow(source, an.hold(gid))
    key = objects.class_of(obj)
    for gid in ix.lookup(key, name) if key is not None and key in ix.classes else []:
        setter = ix.units[gid].runs(SETTER, None) if ix.units[gid].kind in STATE_KINDS else None
        if setter is not None:
            accessor(an, (ACCESSOR, obj, -gid), obj, ix.units[gid], setter, [source])
    an.solver.follow(source, field(an, obj, name))


def declared(an, obj: int, name: str) -> list[int]:
    """The fields a member read or written on `obj` reaches, as its class's chain declares them."""
    ix = an.ix
    key = an.objects.class_of(obj) or an.objects.class_itself(obj)
    if key is None or key not in ix.classes:
        return []
    return [g for g in ix.lookup(key, name) if ix.units[g].kind in STATE_KINDS]


def link_fields(an, unit, obj: int, name: str, line: int, writing: bool) -> None:
    """Links to the fields a read or a write reaches, and to the accessor it runs."""
    for gid in declared(an, obj, name):
        an.link(unit.gid, gid, WRITE if writing else READ, line)
        prop = an.ix.units[gid]
        runs = prop.runs(SETTER if writing else GETTER, unit)
        if isinstance(runs, int):
            an.link(unit.gid, an.ix.offset[prop.file] + runs, CALL, line)


def prototype(an, key: str, maker: int | None = None) -> int:
    """The prototype object of a class, or of the objects one constructor function object makes (each function
    object has its own: ECMAScript MakeConstructor)."""
    return an.objects.make(SITE_PROTOTYPE, key) if maker is None else an.objects.make(SITE_PROTOTYPE, key, maker)


def field(an, obj: int, name: str):
    """A named property of one object; every named property also reaches the object's `every` node."""
    node = an.field(obj, name)
    if node not in an.wired:
        an.wired.add(node)
        an.solver.follow(node, every(an, obj))
    return node


def every(an, obj: int):
    """Every property of `obj`: what was written to its fields, and the initializers of the instance fields its class
    chain declares (an object literal's properties are its class's fields)."""
    node = (ALL, obj)
    if node not in an.wired:
        an.wired.add(node)
        key, ix = an.objects.class_of(obj), an.ix
        for k in chain(ix, key) if key is not None and key in ix.classes and not an.objects.environment(obj)                 and an.objects.kind(obj) != SITE_DECLARED else []:
            for gids in ix.members[k].values():
                for gid in gids:
                    if ix.units[gid].kind in STATE_KINDS and not ix.units[gid].static:
                        an.solver.follow(an.initial(gid), node)
    return node
