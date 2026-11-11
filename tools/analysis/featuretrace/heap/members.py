"""Members of one object (log/trace-core-plan.md, Task 1): a read gives what was written to that object's property,
what its class's chain declares under the name (a method's function object, an instance field's initializers), what a
write through an unknown key put in it; a class as a value gives its static members. A write puts the value in that
object's property, and links to the field its class declares. A read through a key no text names reads every property.
"""
from __future__ import annotations

from ..model import STATE_KINDS
from ..vocab import ELEMENTS_FIELD, READ, WRITE

ALL = 9            # a node: every property of one object (what a read through an unknown key gives)


def read(an, target, obj: int, name: str) -> None:
    """Member `name` of object `obj` into `target`."""
    ix = an.ix
    key = an.objects.class_of(obj)
    for gid in ix.lookup(key, name) if key is not None and key in ix.classes else []:
        if an.is_function_unit(gid):
            an.solver.add(target, [an.function_object(gid)])
        elif ix.units[gid].kind in STATE_KINDS and not ix.units[gid].static:
            an.solver.follow(an.initial(gid), target)
    cls = an.objects.class_itself(obj)
    for gid in ix.lookup(cls, name) if cls is not None and cls in ix.classes else []:
        if not ix.units[gid].static:
            continue
        if an.is_function_unit(gid):
            an.solver.add(target, [an.function_object(gid)])
        elif ix.units[gid].kind in STATE_KINDS:
            an.solver.follow(an.hold(gid), target)
    an.solver.follow(field(an, obj, name), target)
    an.solver.follow(an.any_field(obj), target)


def read_all(an, target, obj: int) -> None:
    """Every property of `obj` and its elements: what a key no text names can read."""
    an.solver.follow(every(an, obj), target)
    an.solver.follow(field(an, obj, ELEMENTS_FIELD), target)
    an.solver.follow(an.any_field(obj), target)


def write(an, source, obj: int, name: str) -> None:
    """`obj.name = value`: the value in that object's property (a class's static field: what it holds)."""
    cls = an.objects.class_itself(obj)
    statics = [g for g in an.ix.lookup(cls, name) if an.ix.units[g].static] \
        if cls is not None and cls in an.ix.classes else []
    for gid in statics:
        an.solver.follow(source, an.hold(gid))
    an.solver.follow(source, field(an, obj, name))


def declared(an, obj: int, name: str) -> list[int]:
    """The fields a member read or written on `obj` reaches, as its class's chain declares them."""
    ix = an.ix
    key = an.objects.class_of(obj) or an.objects.class_itself(obj)
    if key is None or key not in ix.classes:
        return []
    return [g for g in ix.lookup(key, name) if ix.units[g].kind in STATE_KINDS]


def link_fields(an, unit, obj: int, name: str, line: int, writing: bool) -> None:
    for gid in declared(an, obj, name):
        an.link(unit.gid, gid, WRITE if writing else READ, line)


def field(an, obj: int, name: str):
    """A named property of one object; every named property also reaches the object's `every` node."""
    node = an.field(obj, name)
    if node not in an.wired:
        an.wired.add(node)
        an.solver.follow(node, every(an, obj))
    return node


def every(an, obj: int):
    return (ALL, obj)
