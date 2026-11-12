"""What a test of the program proves where it holds (log/trace-research.md F31; the readers' facts, `js_guards`): a
value read where a guard is true (positive) or false of it keeps each object the guard can be so of, once and for all as
it arrives (`verdicts` says what each test can give of one value). The language's own tests are decided by the object's
kind; a property's truthiness or equality by that object's property as its values arrive (a property that may be
missing reads undefined); `instanceof` by each value its right side holds, as either grows; a declared type predicate
or assertion by the type its callee declares (TypeScript handbook, Narrowing). A value the analysis cannot decide
passes.
"""
from __future__ import annotations

from ..solver import UNDEFINED
from ..vocab import (G_ARRAY, G_ASSERTS, G_EQUALS, G_HAS, G_INSTANCE, G_MEMBER, G_MEMBER_EQUALS, G_NULLISH, G_NUMBER,
                     G_PREDICATE, G_TRUTHY, G_TYPEOF, G_VIEW, NARROW, SITE_DECLARED, SITE_LITERAL, SITE_NEW,
                     SITE_UNKNOWN)
from . import members, typesys, verdicts

GUARD_MEMBER = 80                    # a node: one object's property a guard reads


def wire(an, node, tree, unit, ctx) -> None:
    """`(NARROW, tree, guard, positive)`: the values of `tree` the guard can be true (false) of."""
    _, inner, guard, positive = tree
    source, kind, solver = an.node(inner, unit, ctx), guard[0], an.solver
    if kind in (G_MEMBER, G_MEMBER_EQUALS):
        by_member(an, source, node, guard, positive)
    elif kind == G_INSTANCE:
        by_instance(an, source, node, an.node(guard[1], unit, ctx), positive)
    elif kind in (G_PREDICATE, G_ASSERTS):
        by_predicate(an, source, node, an.node(guard[1], unit, ctx), guard[2], positive)
    else:
        solver.subscribe(source, node, (NARROW, guard, positive), lambda t, objs: solver.add(t, [
            o for o in objs if passes(an, o, guard, positive)]))


def passes(an, obj: int, guard, positive: bool) -> bool:
    """Whether a guard decided by the object alone can be true (positive) or false of it."""
    kind = guard[0]
    if kind == G_TYPEOF:
        results = verdicts.typeof(an, obj)
        return guard[1] in results if positive else bool(results - {guard[1]})
    if kind == G_TRUTHY:
        return verdicts.truthiness(an, obj)[0 if positive else 1]
    if kind == G_EQUALS:
        return verdicts.can_equal(an, obj, guard[1]) if positive else verdicts.can_differ(an, obj, guard[1])
    if kind == G_NULLISH:
        return positive in verdicts.nullish(an, obj)
    if kind == G_ARRAY:
        return positive in verdicts.array(an, obj)
    if kind in (G_VIEW, G_NUMBER):           # false of anything else; true of some views, some numbers
        return not positive or True in (verdicts.view(an, obj) if kind == G_VIEW else verdicts.number(an, obj))
    if kind == G_HAS:
        return not verdicts.primitive(an, obj)
    return True


def by_member(an, source, node, guard, positive: bool) -> None:
    """`x.p` truthy, `x.p === c`: an object passes once a value of its property p lets the guard be so; a property
    that may be missing reads undefined (falsy, equal to undefined, different from any other constant)."""
    kind, name = guard[0], guard[1]
    constant = guard[2] if kind == G_MEMBER_EQUALS else None
    if kind == G_MEMBER and not positive or positive and constant == UNDEFINED:
        an.solver.follow(source, node)
        return
    eager = not positive and constant != UNDEFINED

    def decide(target, objs):
        for obj in objs:
            if eager and not defined(an, obj, name):
                an.solver.add(target, [obj])
                continue
            held = (GUARD_MEMBER, obj, name)
            if held not in an.wired:
                an.wired.add(held)
                members.read(an, held, obj, name)
            an.solver.subscribe(held, target, (NARROW, guard, positive, obj), lambda t, values, obj=obj: [
                an.solver.add(t, [obj]) for _ in [0] if any(member_passes(an, v, constant, positive) for v in values)])
    an.solver.subscribe(source, node, (NARROW, guard, positive), decide)


def member_passes(an, value: int, constant, positive: bool) -> bool:
    if constant is None:
        return verdicts.truthiness(an, value)[0]
    return verdicts.can_equal(an, value, constant) if positive else verdicts.can_differ(an, value, constant)


def defined(an, obj: int, name: str) -> bool:
    """Whether the object always has property `name` with a value: a method, an accessor, a field its class or literal
    initializes; a declared object's property gives its declared values (`undefined` among them when optional)."""
    kind = an.objects.kind(obj)
    if kind in (SITE_DECLARED, SITE_UNKNOWN):
        return True
    if kind not in (SITE_NEW, SITE_LITERAL):
        return False
    for key in typesys.own_types(an, obj):
        for gid in typesys.lookup(an, key, name):
            unit = an.ix.units[gid]
            if an.is_function_unit(gid) or unit.accessors or unit.values:
                return True
    return False


def by_instance(an, source, node, targets, positive: bool) -> None:
    """`x instanceof C`: each value of x against each value of C, as either grows."""
    solver = an.solver

    def keep(obj, target):
        return positive in verdicts.outcomes(an, obj, target)
    solver.subscribe(source, node, (NARROW, G_INSTANCE, positive), lambda t, objs: solver.add(t, [
        o for o in objs if any(keep(o, c) for c in solver.held(targets))]))
    solver.subscribe(targets, node, (NARROW, G_INSTANCE, positive, targets), lambda t, cs: solver.add(t, [
        o for o in solver.held(source) if any(keep(o, c) for c in cs)]))


def by_predicate(an, source, node, callee, index: int, positive: bool) -> None:
    """A call of a function declared `x is T` (`asserts x is T`) where it is true: x's values that can be a T, for each
    function the callee can be; a callee that declares nothing of that argument proves nothing."""
    if not positive:
        an.solver.follow(source, node)
        return
    solver = an.solver

    def by_function(target, fns):
        for fn in fns:
            gid = an.objects.function(fn)
            found = an.ix.units[gid].predicate_type if gid is not None else ()
            if not found or found[0] != index:
                solver.follow(source, target)
                continue
            tree = found[1]
            solver.subscribe(source, target, (NARROW, G_PREDICATE, gid), lambda t, objs, tree=tree: solver.add(t, [
                o for o in objs if verdicts.admits(an, tree, o)]))
    solver.subscribe(callee, node, (NARROW, G_PREDICATE, index), by_function)
