"""The entry points (log/trace-core-plan.md, Task 3): module code runs first, in ROOT; then a function no call of the
program reaches is called by the unknown caller, once for each environment a closure of it was made in, with an object
of its declared type (else the unknown object) in each parameter and the unknown object in `this`.
"""
from __future__ import annotations

from ..js_read import MODULE_UNIT
from ..vocab import CALL, HANDED, NEW, SITE_UNKNOWN, T_OPEN
from . import contexts, values

ROOT = contexts.ROOT


def entries(an, modules: bool) -> bool:
    """Module code runs first, in ROOT; after it, every function no call reaches is called by the unknown caller, once
    for each environment a closure of it was made in before this pass, the unknown object in each parameter and `this`.
    One written in a function that had not run before this pass waits for the next: once the solver has run that
    function, a closure of it handed on is a call, not an entry (log/trace-research.md F32); one no run of its
    enclosing function made is entered in ROOT. Returns whether any run started."""
    unknown, started = an.objects.make(SITE_UNKNOWN), False
    known = {gid: list(envs) for gid, envs in an.closures.items() if envs}
    ran = {gid for gid, ctxs in an.active.items() if ctxs}
    for unit in an.ix.units:
        gid = unit.gid
        if not an.is_function_unit(gid) or unit.decl_only or (unit.name == MODULE_UNIT) != modules:
            continue                         # a declaration has no body to run
        if modules:
            started |= enter(an, gid, ROOT, None)
        elif not any(e.kind in (CALL, NEW, HANDED) for e in an.inn.get(gid, ())):   # no caller, nor the platform
            outer = an.ix.offset[unit.file] + unit.outer if unit.outer >= 0 else -1
            if gid not in known and outer >= 0 and an.is_function_unit(outer) and outer not in ran:
                continue                     # its enclosing function has not run yet
            for env in known.get(gid) or [ROOT]:
                started |= enter(an, gid, contexts.of_call(env, unknown), unknown)
    return started


def enter(an, gid: int, ctx, unknown) -> bool:
    """A unit run in `ctx` by no call of the program (its arguments and `this` the unknown object, if given)."""
    if (gid, ctx) in an.entered:
        return False
    an.entered.add((gid, ctx))
    an.activate(gid, ctx)
    if unknown is not None:                  # each parameter: an object of its declared type, else the unknown one
        unit = an.ix.units[gid]
        for index in range(len(unit.params)):
            tree = unit.param_types[index] if index < len(unit.param_types) else ()
            if tree and tree[0] != T_OPEN:
                site = ((gid, index), ROOT)
                an.solver.follow(values.produce(an, tree, (None, None, None), site), an.param(gid, index, ctx))
            else:
                an.solver.add(an.param(gid, index, ctx), [unknown])
        an.solver.add(an.this(gid, ctx), [unknown])
        if unit.owner in an.ix.classes and not unit.static:      # a method: any object of its class
            an.solver.follow(an.instances(unit.owner), an.this(gid, ctx))
    return True
