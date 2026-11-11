"""What no declaration states, summarized from the declarations themselves (log/trace-core-plan.md, Task 5; the
placeholder-library principle of Averroes, ECOOP 2013): a declared function, whatever its name, may do
with the values it takes in positions whose types say nothing of where they go (`any`, `unknown`, `object`, `{}`, a type
not written) whatever those types admit. Those values — each such argument, what reaches such a position inside the
argument's declared types (a property, a callback's result), each one's own fields one level down — are the call's
pool: they reach the fields of the call's open object arguments (the field a string the call passes names, any field
where none does), its result when the result is open, every function among them is called back with them, and a member
a string the call passes names is read on them and, a function, called with them (reflection by name). A position typed
precisely (a type parameter, a named type) keeps what Task 4 gives it.
"""
from __future__ import annotations

from ..library import ENGINE
from ..solver import OBJECT, STRING, TEXT
from ..vocab import (CALL, HANDED, SITE_PRIMITIVE, SITE_TEXT, SITE_UNKNOWN, T_OBJECT, T_OPEN, T_UNION, T_VALUES)
from . import members

POOL, TEXTS, RECEIVE, FIELDS, CALLED, REFLECT, MEMBER_OF, SUMMARY_SITE = 50, 51, 52, 53, 54, 55, 56, 57   # node tags


def is_open(an, tree: tuple) -> bool:
    """Whether a declared type says nothing of where a value given to it goes."""
    if not tree:
        return True
    kind = tree[0]
    if kind == T_OPEN:
        return True
    if kind == T_VALUES:
        return any(v in (ENGINE, OBJECT) for v in tree[1])
    if kind == T_UNION:
        return any(is_open(an, part) for part in tree[1])
    if kind == T_OBJECT:
        return not any(an.ix.members[tree[1]].values())
    return False


def keyed(tree: tuple) -> bool:
    """Whether a declared type admits a string (a property name a call may be handed)."""
    if not tree:
        return False
    if tree[0] == T_UNION:
        return any(keyed(part) for part in tree[1])
    return tree[0] == T_VALUES and any(v == STRING or v.startswith(TEXT) for v in tree[1])


def pool(origin):
    return (POOL, origin)


def summarize(an, node, unit, args: list, site, parameter) -> None:
    """The generic summary of the call `node` of the declared function `unit` (`parameter(unit, i)`: the type its
    argument at i takes)."""
    solver, objects = an.solver, an.objects
    held, texts = pool(node), (TEXTS, node)
    named = any(keyed(parameter(unit, i)) for i in range(len(args)))
    static = objects.make(SUMMARY_SITE, site[0])
    for i, arg in enumerate(args):
        if arg is None:
            continue
        solver.subscribe(arg, texts, (TEXTS, i), lambda t, objs: solver.add(t, [
            o for o in objs if objects.kind(o) == SITE_TEXT or objects.kind(o) == SITE_PRIMITIVE]))
        if is_open(an, parameter(unit, i)):
            solver.subscribe(arg, (RECEIVE, node, i), (RECEIVE,), lambda _, objs: [
                receive(an, node, o, named) for o in objs if is_object(an, o)])
    solver.subscribe(held, (CALLED, node), (CALLED,), lambda _, objs: [
        called(an, node, o, static) for o in objs])
    if is_open(an, unit.result_type):
        solver.follow(held, node)


def is_object(an, obj: int) -> bool:
    return an.objects.kind(obj) not in (SITE_PRIMITIVE, SITE_TEXT, SITE_UNKNOWN)


def receive(an, node, obj: int, named: bool) -> None:
    """An open object argument: its fields one level down join the pool; the pool reaches its fields (the one each
    string the call passes names; any one where the call passes no name or an unknown string), never itself."""
    solver = an.solver
    solver.follow(members.every(an, obj), pool(node))
    if not named:
        into(an, node, obj, an.any_field(obj))
        return
    solver.subscribe((TEXTS, node), (FIELDS, node, obj), (FIELDS,), lambda _, keys: [
        into(an, node, obj, members.field(an, obj, an.objects.text(k)) if an.objects.text(k) is not None
             else an.any_field(obj)) for k in keys])


def into(an, node, obj: int, field) -> None:
    an.solver.subscribe(pool(node), field, (FIELDS, obj), lambda t, objs: an.solver.add(t, [o for o in objs if o != obj]))


def called(an, node, obj: int, static) -> None:
    """A value in the pool: a function is called back with the pool; a member a string the call passes names is read
    on it, and called on it when it is a function; what they give joins the pool."""
    objects = an.objects
    gid = objects.function(obj)
    if gid is not None:
        back = (CALLED, static, obj)
        an.relay(node, back)
        an.call(back, obj, None, [pool(node)] * len(an.ix.units[gid].params), HANDED, site=static)
        an.solver.follow(back, pool(node))
    if not is_object(an, obj):
        return
    an.solver.subscribe((TEXTS, node), (REFLECT, node, obj), (REFLECT,), lambda _, keys: [
        reflect(an, node, obj, objects.text(k), static) for k in keys if objects.text(k) is not None])


def reflect(an, node, obj: int, name: str, static) -> None:
    read = (MEMBER_OF, node, obj, name)
    members.read(an, read, obj, name)
    call = (REFLECT, static, name)
    an.relay(node, call)
    an.solver.subscribe(read, pool(node), (MEMBER_OF, obj, name), lambda t, objs: [
        reflected(an, node, call, obj, o) for o in objs])


def reflected(an, node, call, obj: int, value: int) -> None:
    gid = an.objects.function(value)
    if gid is None:
        an.solver.add(pool(node), [value])
        return
    an.call(call, value, obj, [pool(node)] * len(an.ix.units[gid].params), CALL)
    an.solver.follow(call, pool(node))
