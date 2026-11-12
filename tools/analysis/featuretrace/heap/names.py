"""What a name is where it is read (log/trace-core-plan.md, Tasks 1 and 3; the languages' scoping rules, as the
readers keep them): a local function, a local or parameter (and the enclosing function's, for a closure, hoisted or
not), else a definition of the module, a namespace, an import or the global scope — a function's object, a class's, or
what a variable holds. A local is read in the run it belongs to: one step outward is one step to the environment
(`contexts.environment`), so a closure reads the variables of the run that made it.
"""
from __future__ import annotations

from ..model import STATE_KINDS
from ..vocab import CLASS_UNIT, LAMBDA, SITE_CLASS, SITE_MODULE
from . import contexts


def wire_name(an, node, unit, name: str, ctx) -> None:
    if name in unit.lambdas:
        gid = an.ix.offset[unit.file] + unit.lambdas[name]
        an.solver.add(node, [an.function_object(gid, scope(an, unit, ctx, an.ix.units[gid].outer))])
        return
    if name in unit.types or name in unit.params:
        an.solver.follow(an.local(unit, name, ctx), node)
        return
    owner, owner_ctx = enclosing(an, unit, name, ctx)
    if owner is not None:                         # declared by an enclosing function after the closure (J7)
        an.solver.follow(an.local(owner, name, owner_ctx), node)
        return
    found = definitions(an, unit, name)
    for gid in found:
        value_of(an, gid, node)
    if found:
        return
    modules = an.ix.js.binding(unit.file, name)[1]          # a module's namespace an import binds (`import * as m`)
    an.solver.add(node, [an.objects.make(SITE_MODULE, m) for m in modules])
    key = an.ix.js.class_of(name, unit.file, unit.owner)
    if key is not None and key in an.ix.classes:            # a class no definition of the scope names (ambient)
        an.solver.add(node, [an.objects.make(SITE_CLASS, key)])


def scope(an, unit, ctx, local_id: int):
    """The run of the function `local_id` — `unit` itself or one written around it — as seen from `unit` running in
    `ctx`: the environment of each step outward. A function object made from a declaration is made in that run."""
    while unit.local_id != local_id and unit.outer >= 0:
        unit, ctx = an.ix.units[an.ix.offset[unit.file] + unit.outer], contexts.environment(ctx)
    return ctx


def enclosing(an, unit, name: str, ctx=contexts.ROOT):
    """The function around `unit` that declares `name`, whatever the order it is written in (J7), and its run as seen
    from `unit` running in `ctx`; (None, ctx) when no enclosing function declares it."""
    while unit.outer >= 0:
        unit, ctx = an.ix.units[an.ix.offset[unit.file] + unit.outer], contexts.environment(ctx)
        if name in unit.types or name in unit.params:
            return unit, ctx
    return None, ctx


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
    elif unit.kind == CLASS_UNIT and not (unit.qual in an.ix.classes and an.ix.classes[unit.qual].shape):
        an.solver.add(node, [an.objects.make(SITE_CLASS, unit.qual)])     # an interface is no value
    elif unit.kind in STATE_KINDS:
        an.solver.follow(an.hold(gid), node)
