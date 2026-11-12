"""The entry points (log/trace-core-plan.md, Task 3; log/trace-research.md F32, F36): module code runs first, in ROOT;
then a function no call of the program reaches is called by the unknown caller, once for each environment a closure
of it was made in, with an object of its declared type (else the unknown object) in each parameter and, in `this`,
an object of its class for a method (TypeScript: `this` in a method is of its class) and every one the program made,
else the unknown object. A module's code initializes each of its variables, read or not (ECMA-262 module evaluation).
Entries start in waves, the solver run between them, so that a function the program calls is found
called before it would be entered: first the functions no other code names (a read or a call of their name), then
those that stay uncalled; a function written in another waits until that one has run.
"""
from __future__ import annotations

from ..js_read import MODULE_UNIT
from ..model import STATE_KINDS
from ..vocab import CALL, HANDED, NEW, READ, SITE_UNKNOWN, T_OPEN
from . import contexts, values

ROOT = contexts.ROOT


def entries(an, modules: bool) -> bool:
    """Module code (modules), else the next wave of entry points: the runs to start — each uncalled function in each
    environment a closure of it was made in before this pass (ROOT when its enclosing function ran and made none) —
    those of functions no other code names first. Returns whether any run started."""
    if modules:
        return any([enter(an, u.gid, ROOT, None) for u in an.ix.units
                    if an.is_function_unit(u.gid) and not u.decl_only and u.name == MODULE_UNIT])
    unknown = an.objects.make(SITE_UNKNOWN)
    known = {gid: list(envs) for gid, envs in an.closures.items() if envs}
    ran = {gid for gid, ctxs in an.active.items() if ctxs}
    runs = []
    for unit in an.ix.units:
        gid = unit.gid
        if not an.is_function_unit(gid) or unit.decl_only or unit.name == MODULE_UNIT:
            continue                         # a declaration has no body to run
        if any(e.kind in (CALL, NEW, HANDED) for e in an.inn.get(gid, ())):    # a caller, or the platform
            continue
        outer = an.ix.offset[unit.file] + unit.outer if unit.outer >= 0 else -1
        if gid not in known and outer >= 0 and an.is_function_unit(outer) and outer not in ran:
            continue                         # its enclosing function has not run yet
        runs += [(gid, contexts.of_call(env, unknown)) for env in known.get(gid) or [ROOT]
                 if (gid, contexts.of_call(env, unknown)) not in an.entered]
    first = [(gid, ctx) for gid, ctx in runs if an.ix.units[gid].name not in named(an)]
    return any([enter(an, gid, ctx, unknown) for gid, ctx in first or runs])


def named(an) -> frozenset:
    """The names the program's code reads or calls (by name or as a member)."""
    if named not in an.memo:
        an.memo[named] = frozenset(ref.name for unit in an.ix.units for ref in unit.refs
                                   if ref.kind in (CALL, READ) and ref.name)
    return an.memo[named]


def enter(an, gid: int, ctx, unknown) -> bool:
    """A unit run in `ctx` by no call of the program (its arguments and `this` the unknown object, if given)."""
    if (gid, ctx) in an.entered:
        return False
    an.entered.add((gid, ctx))
    an.activate(gid, ctx)
    unit = an.ix.units[gid]
    if unknown is None:                      # a module's code: each variable it declares is initialized
        for gids in an.ix.members.get(unit.owner, {}).values():
            for variable in gids:
                if an.ix.units[variable].kind in STATE_KINDS:
                    an.hold(variable)
    else:                                    # each parameter: an object of its declared type, else the unknown one
        for index in range(len(unit.params)):
            tree = unit.param_types[index] if index < len(unit.param_types) else ()
            if tree and tree[0] != T_OPEN:
                site = ((gid, index), ROOT)
                an.solver.follow(values.produce(an, tree, (None, None, None), site), an.param(gid, index, ctx))
            else:
                an.solver.add(an.param(gid, index, ctx), [unknown])
        if unit.owner in an.ix.classes and not unit.static:      # a method: an object of its class, and any made
            an.solver.add(an.this(gid, ctx), [an.objects.from_outside(unit.owner)])
            an.solver.follow(an.instances(unit.owner), an.this(gid, ctx))
        else:
            an.solver.add(an.this(gid, ctx), [unknown])
        an.ret(gid, ctx)                     # what it returns is made, in this run (a closure, a literal's methods)
    return True
