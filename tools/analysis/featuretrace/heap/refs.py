"""A unit's references lowered (log/trace-core-plan.md, Task 1): a call is the node of its call tree, which finds the
functions it runs, and each reference written for it a link at its line; `new` runs the class's constructor on the
object made at that site; a write puts its value in each receiver object's property (or the variable it names), a
write through a key no literal names in the property the key's text names or in every property; a read of a declared
field and a function written as a value are links too. No name of a program or platform is read here.
"""
from __future__ import annotations

from ..model import STATE_KINDS
from ..rules import CONSTRUCTOR
from ..vocab import (ANY_FIELD, CALL, CONSTRUCT, ELEMENTS_FIELD, INVOKE, LAMBDA, MEMBER, NAME, NEW, READ, SITE_NEW,
                     STORE, SUPER, WRITE)
from . import members, names

CONSTRUCTED = 10           # a node: what a constructor's call returns (the `new` itself is the object made)


def lower(an, unit) -> None:
    for ref in unit.refs:
        kind = ref.kind
        if kind == CALL:
            callee = (MEMBER, ref.base, ref.name, False) if ref.base is not None else (NAME, ref.name)
            if ref.base != (SUPER,):
                an.written(an.node((CALL, callee, ref.args), unit), unit.gid, ref.line)
        elif kind == INVOKE and ref.base is not None:
            an.written(an.node((CALL, ref.base, ref.args), unit), unit.gid, ref.line)
        elif kind == NEW and ref.base is not None:
            construct(an, unit, ref)
        elif kind == WRITE:
            write(an, unit, ref)
        elif kind == STORE:
            store(an, unit, ref)
        elif kind == READ:
            read(an, unit, ref)
        elif kind == LAMBDA and ref.target >= 0:
            an.link(unit.gid, an.ix.offset[unit.file] + ref.target, LAMBDA, ref.line)


def construct(an, unit, ref) -> None:
    """`new C(args)`: the object made at the site (the construct tree), and C's constructor run on it."""
    ctor = an.node(ref.base, unit)
    an.node((CONSTRUCT, ref.base, ref.site), unit)
    args = [an.node(a, unit) for a in ref.args]

    def by_class(_, objs):
        for key in (an.objects.class_itself(o) for o in objs):
            if key is None or key not in an.ix.classes:
                continue
            made = an.objects.make(SITE_NEW, key, unit.file, ref.site)
            call = (CONSTRUCTED, unit.gid, ref.site, key)
            an.written(call, unit.gid, ref.line)
            for gid in an.ix.lookup(key, CONSTRUCTOR):
                if an.is_function_unit(gid):
                    an.call(call, an.function_object(gid), made, args, NEW)
    an.solver.subscribe(ctor, (CONSTRUCTED, unit.gid, ref.site), (NEW,), by_class)


def write(an, unit, ref) -> None:
    if not ref.args:
        return
    value = an.node(ref.args[0], unit)
    if ref.base is None:
        if ref.name in unit.types or ref.name in unit.params or names.enclosing(an, unit, ref.name) is not None:
            return                                   # a local: its values are its trees (`trees.wire_local`)
        for gid in names.definitions(an, unit, ref.name):
            if an.ix.units[gid].kind in STATE_KINDS:
                an.solver.follow(value, an.hold(gid))
                an.link(unit.gid, gid, WRITE, ref.line)
        return
    name = ref.name

    def by_object(_, objs):
        for obj in objs:
            members.write(an, value, obj, name)
            members.link_fields(an, unit, obj, name, ref.line, True)
    an.solver.subscribe(an.node(ref.base, unit), (WRITE, unit.gid, ref.line, name), (WRITE,), by_object)


def store(an, unit, ref) -> None:
    """`o[k] = v`: the property each text of k names, else every property (and the elements)."""
    if not ref.args or ref.key is None:
        return
    value, key = an.node(ref.args[0], unit), an.node(ref.key, unit)

    def by_object(_, objs):
        for obj in objs:
            an.solver.subscribe(key, (STORE, obj), (STORE, unit.gid, ref.line), lambda __, keys, obj=obj: by_key(obj, keys))

    def by_key(obj, keys):
        for k in keys:
            text = an.objects.text(k)
            if text is not None:
                members.write(an, value, obj, text)
            else:
                an.solver.follow(value, members.field(an, obj, ELEMENTS_FIELD))
                an.solver.follow(value, an.field(obj, ANY_FIELD))
    an.solver.subscribe(an.node(ref.base, unit), (STORE, unit.gid, ref.line), (STORE,), by_object)


def read(an, unit, ref) -> None:
    """A read of a declared field (or a variable) is a link: the walk shows what writes it."""
    if ref.base is None:
        if ref.name in unit.types or ref.name in unit.params or ref.name in unit.lambdas:
            return
        for gid in names.definitions(an, unit, ref.name):
            if an.ix.units[gid].kind in STATE_KINDS:
                an.link(unit.gid, gid, READ, ref.line)
        return
    name = ref.name
    an.solver.subscribe(an.node(ref.base, unit), (READ, unit.gid, ref.line, name), (READ,), lambda _, objs: [
        members.link_fields(an, unit, o, name, ref.line, False) for o in objs])
