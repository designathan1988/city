"""What a name is where it is read (log/trace-core-plan.md, Task 1; the languages' scoping rules, as the readers keep
them): a local function, a local or parameter (and the enclosing function's, for a closure, hoisted or not), else a
definition of the module, a namespace, an import or the global scope — a function's object, a class's, or what a
variable holds.
"""
from __future__ import annotations

from ..model import STATE_KINDS
from ..vocab import CLASS_UNIT, LAMBDA, SITE_CLASS


def wire_name(an, node, unit, name: str) -> None:
    if name in unit.lambdas:
        an.solver.add(node, [an.function_object(an.ix.offset[unit.file] + unit.lambdas[name])])
        return
    if name in unit.types or name in unit.params:
        an.solver.follow(an.local(unit, name), node)
        return
    owner = enclosing(an, unit, name)
    if owner is not None:                         # declared by an enclosing function after the closure (J7)
        an.solver.follow(an.local(owner, name), node)
        return
    found = definitions(an, unit, name)
    for gid in found:
        value_of(an, gid, node)
    key = an.ix.js.class_of(name, unit.file, unit.owner) if not found else None
    if key is not None and key in an.ix.classes:            # a class no definition of the scope names (ambient)
        an.solver.add(node, [an.objects.make(SITE_CLASS, key)])


def enclosing(an, unit, name: str):
    """The function around `unit` that declares `name`, whatever the order it is written in (J7)."""
    while unit.outer >= 0:
        unit = an.ix.units[an.ix.offset[unit.file] + unit.outer]
        if name in unit.types or name in unit.params:
            return unit
    return None


def definitions(an, unit, name: str) -> list[int]:
    """The definitions a name reaches past the locals: the namespaces and the module it is written in, its imports,
    the global scope (never a class's member: those need `this.`)."""
    ix = an.ix
    for key in dict.fromkeys(ix.js.scope_keys(unit.owner) + [unit.file]):
        gids = [g for g in ix.members[key].get(name, []) if ix.units[g].kind != LAMBDA]
        if gids:
            return gids
    gids, _ = ix.js.binding(unit.file, name)
    return list(gids) or list(ix.js.globals.get(name, []))


def value_of(an, gid: int, node) -> None:
    """What a definition is as a value: a function's object, a class itself, what a variable holds."""
    unit = an.ix.units[gid]
    if an.is_function_unit(gid):
        an.solver.add(node, [an.function_object(gid)])
    elif unit.kind == CLASS_UNIT:
        an.solver.add(node, [an.objects.make(SITE_CLASS, unit.qual)])
    elif unit.kind in STATE_KINDS:
        an.solver.follow(an.hold(gid), node)
