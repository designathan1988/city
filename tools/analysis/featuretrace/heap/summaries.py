"""What no declaration states, summarized from the declarations themselves (log/trace-core-plan.md, Task 5; narrowed by
log/trace-research.md F2, F10 and its Decision): a declared function, whatever its name, may do with the values it
takes in positions whose types say nothing of where they go (`any`, `unknown`, `object`, `{}`, the intrinsic object and
function types, a type not written) what unknown code does in Jelly and JAM: the values reaching such positions — each
open argument, what reaches an open position inside an argument's declared types (a property, a callback's result) —
are the call's pool; a string the call passes names the member of its open object arguments the pool reaches
(defineProperty's shape) and the member read and called on them (reflection by name); an open result holds the pool;
a function in the pool is called back with what its own declared parameters give. Nothing is spread into every field
and no callback is called with everything (Averroes's whole pool made every set large: F10).

The platform's kept state (Decision, item 3): a function a declared method takes under a declared function type is kept
on the receiver, and called back by a later declared call on that receiver that hands no function and passes a value of
a named, non-generic type the kept function's declared parameters name (an Event to an EventListener), with that value.
"""
from __future__ import annotations

from ..library import ENGINE
from ..solver import OBJECT, SYMBOL, TEXT
from ..vocab import (CALL, HANDED, PLATFORM_FIELD, SITE_KEY, SITE_PRIMITIVE, SITE_TEXT, SITE_UNKNOWN, T_FN, T_NAMED,
                     T_OBJECT, T_OPEN, T_UNION, T_VALUES)
from . import members, texts, typesys

POOL, TEXTS, RECEIVE, FIELDS, CALLED, REFLECT, MEMBER_OF, SUMMARY_SITE = 50, 51, 52, 53, 54, 55, 56, 57   # node tags
KEPT, KEPT_SITE, KEPT_ENTRY, GIVEN, UNKNOWN_ARG = 58, 59, 62, 63, 64


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
    if kind == T_NAMED:                           # what every object or every function is
        found = typesys.resolve(an, tree[1], tree[3], tree[4])
        return found is not None and found[0] == typesys.CLASS and found[1] in typesys.open_types(an)
    return False


def keyed(an, tree: tuple) -> bool:
    """Whether a declared type is a type of property keys (ECMA-262: a property key is a string or a symbol; TypeScript's
    `PropertyKey`, `keyof T`, a union of string literal types): a string given there names a member. A plain `string` is
    any text (a message, a path), no name."""
    kinds = set(key_values(an, tree))
    return bool(kinds) and (SYMBOL in kinds or all(v.startswith(TEXT) for v in kinds))


def key_values(an, tree: tuple, seen=()):
    """The language's values a declared type admits, through aliases and unions."""
    if not tree:
        return []
    if tree[0] == T_VALUES:
        return list(tree[1])
    if tree[0] == T_UNION:
        return [v for part in tree[1] for v in key_values(an, part, seen)]
    if tree[0] == T_NAMED:
        found = typesys.resolve(an, tree[1], tree[3], tree[4])
        if found is not None and found[0] == typesys.ALIAS and found not in seen:
            return key_values(an, an.ix.units[found[1]].result_type, seen + (found,))
    return []


def pool(origin):
    return (POOL, origin)


def summarize(an, node, unit, args: list, site, parameter) -> None:
    """The generic summary of the call `node` of the declared function `unit` (`parameter(unit, i)`: the type its
    argument at i takes). The pool is filled where the call's values reach open positions (`values.consume`)."""
    solver, objects = an.solver, an.objects
    where = (objects.make(SUMMARY_SITE, site[0]), site[1])
    names = set()
    for i in range(len(args)):                     # the texts the call's property-key arguments have, as written
        if args[i] is not None and keyed(an, parameter(unit, i)):
            names |= texts.argument(an, node, i) or set()
    for i, arg in enumerate(args):
        if arg is not None and names and is_open(an, parameter(unit, i)):
            solver.subscribe(arg, (RECEIVE, node, i), (RECEIVE,), lambda _, objs: [
                receive(an, node, o, names, where) for o in objs if is_object(an, o)])
    solver.subscribe(pool(node), (CALLED, node), (CALLED,), lambda _, objs: [
        called(an, node, o, where) for o in objs if objects.function(o) is not None])
    if is_open(an, unit.result_type):
        solver.follow(pool(node), node)


def open_call(an, node, args: list, site) -> None:
    """A call of what no declaration describes (the platform's unknown object, a JSX element no transform declares):
    what it is handed is called back (with what each function's declared parameters give); it gives the unknown."""
    where = (an.objects.make(SUMMARY_SITE, site[0]), site[1])
    for i, arg in enumerate(args):
        if arg is not None:
            an.solver.subscribe(arg, (CALLED, node), (CALLED, i), lambda _, objs: [
                called(an, node, o, where) for o in objs if an.objects.function(o) is not None])
    an.solver.add(node, [an.objects.make(SITE_UNKNOWN)])


def receive(an, node, obj: int, keys, where) -> None:
    """An open object argument: the pool reaches the member each text the call's property-key arguments have names
    (never the object itself), and that member is read and, a function, called on it (reflection by name)."""
    for name in keys:
        named(an, node, obj, name, where)


def named(an, node, obj: int, name: str, where) -> None:
    an.solver.subscribe(pool(node), members.field(an, obj, name), (FIELDS, obj), lambda t, objs: an.solver.add(t, [
        o for o in objs if o != obj]))
    read = (MEMBER_OF, node, obj, name)
    members.read(an, read, obj, name)
    call = (REFLECT, where[0], name)
    an.relay(node, call)
    an.solver.subscribe(read, pool(node), (MEMBER_OF, obj, name), lambda t, objs: [
        reflected(an, node, call, obj, o) for o in objs])


def reflected(an, node, call, obj: int, value: int) -> None:
    if an.objects.function(value) is None:
        an.solver.add(pool(node), [value])
        return
    an.call(call, value, obj, arguments(an, value), CALL)
    an.solver.follow(call, pool(node))


def called(an, node, fn: int, where) -> None:
    """A function in the pool or handed to unknown code: called back with what its declared parameters give."""
    back = (CALLED, where[0], fn)
    an.relay(node, back)
    an.call(back, fn, None, arguments(an, fn), HANDED, site=where)
    an.solver.follow(back, pool(node))


def arguments(an, fn: int) -> list:
    """What a function called back by unknown code receives: each parameter, an object of its declared type (`values`),
    else the unknown object."""
    from . import values
    unit = an.ix.units[an.objects.function(fn)]
    out = []
    for i in range(len(unit.params)):
        tree = unit.param_types[i] if i < len(unit.param_types) else ()
        if tree and not is_open(an, tree):
            out.append(values.produce(an, tree, (None, None, None), (((GIVEN, unit.gid, i),), ())))
        else:
            node = (UNKNOWN_ARG,)
            an.solver.add(node, [an.objects.make(SITE_UNKNOWN)])
            out.append(node)
    return out


def keep(an, holder: int, fn: int, tree: tuple) -> None:
    """A function a declared method of `holder` took under the declared function type `tree`: kept on the holder."""
    an.solver.add(an.field(holder, PLATFORM_FIELD), [an.objects.make(KEPT_ENTRY, fn, tree)])


def kept(an, node, holder: int, unit, args: list, site, parameter) -> None:
    """A declared call on `holder` that hands no function: each kept function whose declared parameters name a
    non-generic type this call's parameters name is called back with the arguments of that type."""
    if any(hands(an, parameter(unit, i)) for i in range(len(unit.param_types))):
        return
    offered = {}
    for i, arg in enumerate(args):
        key = plain_type(an, parameter(unit, i))
        if key is not None and arg is not None:
            offered.setdefault(key, []).append(arg)
    if not offered:
        return
    where = (an.objects.make(KEPT_SITE, site[0]), site[1])
    an.solver.subscribe(an.field(holder, PLATFORM_FIELD), (KEPT, node), (KEPT,), lambda _, entries: [
        kept_call(an, node, e, offered, where) for e in entries])


def kept_call(an, node, entry: int, offered: dict, where) -> None:
    _, fn, tree = an.objects.keys[entry]
    wanted = [plain_type(an, p) for p in tree[1]]
    if not any(k in offered for k in wanted if k is not None):
        return
    given = []
    for i, key in enumerate(wanted):
        arg = (GIVEN, node, entry, i)
        for source in offered.get(key, ()):
            an.solver.follow(source, arg)
        given.append(arg)
    back = (KEPT, where[0], fn)
    an.relay(node, back)
    an.call(back, fn, None, given, HANDED, site=where)


def plain_type(an, tree: tuple):
    """The class or interface a declared type names when it names exactly one with no type arguments, else None."""
    if not tree or tree[0] != T_NAMED or tree[2]:
        return None
    found = typesys.resolve(an, tree[1], tree[3], tree[4])
    if found is None or found[0] != typesys.CLASS:
        return None
    key = found[1]
    return key if not any(an.ix.classes[k].tparams for k in typesys.group(an, key) if k in an.ix.classes) else None


def hands(an, tree: tuple) -> bool:
    """Whether a declared parameter type takes a function (a function type, or a type with a call signature)."""
    if not tree:
        return False
    if tree[0] == T_FN:
        return True
    if tree[0] == T_UNION:
        return any(hands(an, part) for part in tree[1])
    if tree[0] == T_NAMED:
        found = typesys.resolve(an, tree[1], tree[3], tree[4])
        if found is not None and found[0] == typesys.ALIAS:
            return hands(an, an.ix.units[found[1]].result_type)
        from ..languages import CALL_SIGNATURE
        return found is not None and bool(typesys.lookup(an, found[1], CALL_SIGNATURE))
    return False


def is_object(an, obj: int) -> bool:
    return an.objects.kind(obj) not in (SITE_PRIMITIVE, SITE_TEXT, SITE_KEY, SITE_UNKNOWN)
