"""Members of one object (log/trace-core-plan.md, Tasks 1-2; ECMA-262 [[Get]] and [[Set]] through the prototype
chain): a read gives what was written to that object's property, what its class's chain declares under the name (a
method's function object, an instance field's initializers, an accessor's getter run on the object), what was written
on the prototype objects of its chain (a class's, a constructor function's own or the object its `prototype` was set
to), what a write through an unknown key put in it; `constructor` its class; a class or a function as a value its own
properties (a class's statics, its prototype object). A write puts the value in that object's property, or runs the
setter its class declares. A read through a key no text names reads every property.
"""
from __future__ import annotations

from ..js_transfer import chain
from ..languages import GETTER, PROTOTYPE, PROTOTYPE_CONSTRUCTOR, SETTER
from ..model import STATE_KINDS
from ..vocab import CALL, ELEMENTS_FIELD, READ, SITE_CLASS, SITE_PROTOTYPE, WRITE

ALL, ACCESSOR, PROTOTYPES = 9, 11, 12        # nodes: every property of one object; an accessor's call; a prototype


def read(an, target, obj: int, name: str) -> None:
    """Member `name` of object `obj` into `target`."""
    ix, objects = an.ix, an.objects
    key = objects.class_of(obj)
    if key is not None:
        declared_read(an, target, obj, key, name)
    for k in chain(ix, key) if key is not None and key in ix.classes else []:
        an.solver.follow(field(an, prototype(an, k), name), target)
    if key is not None:
        made_from(an, target, obj, key, name)
        if name == PROTOTYPE_CONSTRUCTOR:
            an.solver.add(target, [constructor_value(an, key)])
    statics(an, target, obj, name)
    an.solver.follow(field(an, obj, name), target)
    an.solver.follow(an.any_field(obj), target)


def declared_read(an, target, obj: int, key: str, name: str) -> None:
    """What class `key`'s chain declares under `name`, read on `obj` (an instance, or `this` for `super.name`): a
    method's function object, an instance field's initializers, its getter run on `obj`."""
    ix = an.ix
    for gid in ix.lookup(key, name) if key in ix.classes else []:
        unit = ix.units[gid]
        if an.is_function_unit(gid):
            an.solver.add(target, [an.function_object(gid)])
        elif unit.kind in STATE_KINDS and not unit.static:
            getter = unit.runs(GETTER, None)
            if getter is not None:
                accessor(an, target, obj, unit, getter, [])
            else:
                an.solver.follow(an.initial(gid), target)


def statics(an, target, obj: int, name: str) -> None:
    """A class or a function as a value: a class's static members and its prototype object, a constructor function's
    prototype object."""
    ix, objects = an.ix, an.objects
    cls = objects.class_itself(obj)
    for gid in ix.lookup(cls, name) if cls is not None and cls in ix.classes else []:
        if not ix.units[gid].static:
            continue
        if an.is_function_unit(gid):
            an.solver.add(target, [an.function_object(gid)])
        elif ix.units[gid].kind in STATE_KINDS:
            an.solver.follow(an.hold(gid), target)
    if name != PROTOTYPE:
        return
    gid = objects.function(obj)
    made = cls if cls is not None else ix.units[gid].constructs if gid is not None else ""
    if made:
        an.solver.add(target, [prototype(an, made)])


def made_from(an, target, obj: int, key: str, name: str) -> None:
    """An object a constructor function made: the members of the objects its `prototype` was set to."""
    gid = an.constructed().get(key)
    if gid is None:
        return
    held = field(an, an.function_object(gid), PROTOTYPE)
    an.solver.subscribe(held, target, (PROTOTYPES, name), lambda t, protos: [read(an, t, p, name) for p in protos])


def constructor_value(an, key: str) -> int:
    """An object's `constructor`: its class, or the function whose `new` made it."""
    gid = an.constructed().get(key)
    return an.function_object(gid) if gid is not None else an.objects.make(SITE_CLASS, key)


def accessor(an, target, obj: int, prop, local_id, args: list) -> None:
    """A property's accessor (`get`, `set`) run on `obj`: its result into `target`."""
    gid = an.ix.offset[prop.file] + local_id if isinstance(local_id, int) else None
    if gid is None:
        return
    node = (ACCESSOR, obj, gid)
    an.call(node, an.function_object(gid), obj, args, CALL)
    an.solver.follow(node, target)


def read_all(an, target, obj: int) -> None:
    """Every property of `obj` and its elements: what a key no text names can read."""
    an.solver.follow(every(an, obj), target)
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


def prototype(an, key: str) -> int:
    """The prototype object of a class or of a constructor function's objects."""
    return an.objects.make(SITE_PROTOTYPE, key)


def field(an, obj: int, name: str):
    """A named property of one object; every named property also reaches the object's `every` node."""
    node = an.field(obj, name)
    if node not in an.wired:
        an.wired.add(node)
        an.solver.follow(node, every(an, obj))
    return node


def every(an, obj: int):
    return (ALL, obj)
