"""A unit's references lowered in one context (log/trace-core-plan.md, Tasks 1 and 3): a call is the node of its call
tree, which finds the functions it runs, and each reference written for it a link at its line; `new` runs the class's
constructor (or the constructor function object itself) on the object made at that site in that context; a write puts
its value in each receiver object's property (or the variable it names), a write through a key no literal names in the
property the key's text names or in every property; a read of a declared field and a function written as a value are
links too. No name of a program or platform is read here.
"""
from __future__ import annotations

from ..js_read import MODULE_UNIT
from ..model import STATE_KINDS
from ..vocab import (CALL, CONSTRUCT, IMPORT, INVOKE, JSX, LAMBDA, MEMBER, NAME, NEW,
                     READ, SITE_PRIMITIVE, SITE_UNKNOWN, STORE, WRITE)
from . import declared, jsx, members, names, summaries, texts, trees

WRITTEN = 11               # a node: the subscription a `new` reference writes its line through
ESCAPE = 14                # a node: the open call a write into the unknown object is


def lower(an, unit, ctx) -> None:
    for ref in unit.refs:
        kind = ref.kind
        if kind == CALL:
            callee = (MEMBER, ref.base, ref.name, False) if ref.base is not None else (NAME, ref.name)
            an.written(an.node((CALL, callee, ref.args), unit, ctx), unit.gid, ref.line)
        elif kind == INVOKE and ref.base is not None:
            an.written(an.node((CALL, ref.base, ref.args), unit, ctx), unit.gid, ref.line)
        elif kind == NEW and ref.base is not None:
            construct(an, unit, ref, ctx)
        elif kind == WRITE:
            write(an, unit, ref, ctx)
        elif kind == STORE:
            store(an, unit, ref, ctx)
        elif kind == READ:
            read(an, unit, ref, ctx)
        elif kind == LAMBDA and ref.target >= 0:
            an.link(unit.gid, an.ix.offset[unit.file] + ref.target, LAMBDA, ref.line)
        elif kind == JSX and ref.base is not None:
            jsx.written(an, unit, ref, ctx)
        elif kind == IMPORT and ref.text:            # the imported module's code runs first (J3)
            target = an.ix.js.resolve(unit.file, ref.text)
            for gid in an.ix.by_qual.get(f"{target}:{MODULE_UNIT}", []) if target else []:
                an.link(unit.gid, gid, CALL, ref.line)


def construct(an, unit, ref, ctx) -> None:
    """`new C(args)`: its tree does the construction (`trees.construct`); the reference writes its line on each
    constructor call the tree's node makes."""
    node = an.node((CONSTRUCT, ref.base, ref.site, ref.args), unit, ctx)
    an.solver.subscribe(an.node(ref.base, unit, ctx), (WRITTEN, node), (NEW,), lambda _, objs: [
        an.written(trees.constructed(node, an.made_by(o) or o), unit.gid, ref.line) for o in objs])


def write(an, unit, ref, ctx) -> None:
    if not ref.args:
        return
    value = an.node(ref.args[0], unit, ctx)
    if ref.base is None:
        if ref.name in unit.types or ref.name in unit.params:
            an.local(unit, ref.name, ctx)            # a local: its values are its trees (`trees.wire_local`)
            return
        if names.enclosing(an, unit, ref.name)[0] is not None:
            return
        for gid in names.definitions(an, unit, ref.name):
            if an.ix.units[gid].kind in STATE_KINDS:
                an.solver.follow(value, an.hold(gid))
                an.link(unit.gid, gid, WRITE, ref.line)
        return
    name = ref.name

    def by_object(_, objs):
        for obj in objs:
            if an.objects.kind(obj) == SITE_UNKNOWN:
                escape(an, unit, ref, value, ctx)
                continue
            members.write(an, value, obj, name)
            members.link_fields(an, unit, obj, name, ref.line, True)
    an.solver.subscribe(an.node(ref.base, unit, ctx), (WRITE, unit.gid, ref.line, name, ctx), (WRITE,), by_object)


def escape(an, unit, ref, value, ctx) -> None:
    """A value written into the unknown object is handed to code the program does not have (Averroes: what escapes may
    be called back): an open call at the write, its callbacks linked from the line that writes it."""
    node = (ESCAPE, unit.gid, ref.line, ctx)
    if node in an.wired:
        return
    an.wired.add(node)
    an.written(node, unit.gid, ref.line)
    summaries.open_call(an, node, [value], declared.call_site(an, node))


def store(an, unit, ref, ctx) -> None:
    """`o[k] = v`: the property each text k can have names (`texts`), else what each value of k names (an index: the
    elements; any other key: the elements and the dictionary part: `members.store_by_key`)."""
    if not ref.args or ref.key is None:
        return
    value = an.node(ref.args[0], unit, ctx)
    key = an.node(ref.key, unit, ctx)
    known = texts.of(an, ref.key, unit)

    def by_object(_, objs):
        for obj in objs:
            kind = an.objects.kind(obj)
            if kind == SITE_UNKNOWN:
                escape(an, unit, ref, value, ctx)
                continue
            if kind == SITE_PRIMITIVE:                 # a primitive keeps no property (`members.write`)
                continue
            if known is not None:
                for text in known:
                    members.write(an, value, obj, text)
            else:                                       # by what each value of the key is
                an.solver.subscribe(key, (STORE, unit.gid, ref.line, ctx, obj), (STORE, obj),
                                    lambda _, keys, obj=obj: [members.store_by_key(an, value, obj, k) for k in keys])
    an.solver.subscribe(an.node(ref.base, unit, ctx), (STORE, unit.gid, ref.line, ctx), (STORE,), by_object)


def read(an, unit, ref, ctx) -> None:
    """A read of a declared field (or a variable) is a link: the walk shows what writes it."""
    if ref.base is None:
        if ref.name in unit.types or ref.name in unit.params or ref.name in unit.lambdas:
            return
        for gid in names.definitions(an, unit, ref.name):
            if an.ix.units[gid].kind in STATE_KINDS:
                an.link(unit.gid, gid, READ, ref.line)
        return
    name = ref.name
    an.solver.subscribe(an.node(ref.base, unit, ctx), (READ, unit.gid, ref.line, name, ctx), (READ,),
                        lambda _, objs: [members.link_fields(an, unit, o, name, ref.line, False) for o in objs])
