"""What a declared function does: what its declaration says (log/trace-core-plan.md, Task 4). A call of a function
that is only declared (a platform's, a library's) puts each argument where its parameter's type says and gives what
its result type gives, in the environment of the receiver it is called on and of this call's own type parameters; a
function the program hands to a function type is called back by it; `new` on an object of a declared type runs its
construct signature, a call of it its call signature; a function of a declared function type the platform gave, called
by the program, takes its arguments the same way; the function `await` hands a thenable takes the awaited value. A rest
parameter typed by a type parameter (`...args: A`) takes the arguments from it on as one array of type A, and gives a
function called back each element of it (TypeScript: a rest parameter's type is its array's; ECMA-262 makes the array
at the call).
"""
from __future__ import annotations

from ..languages import ARRAY_TYPE, CALL_SIGNATURE, CONSTRUCT_SIGNATURE
from ..solver import NUMBER
from ..vocab import (ELEMENTS_FIELD, HANDED, SITE_ARRAY, SITE_DECLARED_FN, SITE_FUNCTION, SITE_LITERAL, SITE_NEW,
                     SITE_RESOLVER, SITE_UNKNOWN, T_FN, T_INDEX, T_NAMED, T_OBJECT, T_OPEN, T_PARAM, T_TUPLE,
                     T_VALUES)
from . import contexts, members, summaries, typesys, values

CALLBACK, CALLBACK_SITE, READ_MEMBER, THIS, RECEIVERS, CALLBACK_ARG = 30, 31, 32, 33, 34, 35   # node tags, site parts
STRUCTURAL, REST = 36, 37
PROGRAM_MADE = (SITE_NEW, SITE_LITERAL, SITE_FUNCTION, SITE_ARRAY)     # the objects the program's own code makes


def call_site(an, node) -> tuple:
    """A call's site for what its declaration makes, for a node whose last part is its context and whose other parts
    are static (a tree's call, a protocol's, a JSX element's): the place without the context, and the heap context.
    A call whose node carries objects (a callback's) is given its static site explicitly (`analysis.call`)."""
    ctx = node[-1] if isinstance(node[-1], tuple) else contexts.ROOT
    return (an.objects.make(*node[:-1]), contexts.heap(ctx))


def apply(an, node, gid: int, holder, args: list, site) -> None:
    """A call `node` of the declared function `gid` on `holder` (whose type parameters its types read)."""
    unit = an.ix.units[gid]
    key = typesys.identity(an, unit.owner) if holder is not None else None
    env = (holder, key, (site, gid))
    for i, arg in enumerate(args):
        tree = parameter(unit, i)
        if tree and arg is not None:
            values.consume(an, tree, env, arg, values.sub(site, (gid, i)), node)
    rest_array(an, tuple(unit.param_types), len(unit.param_types) - 1 if unit.variadic else -1, args, env,
               values.sub(site, (gid, REST)), node)
    if unit.this_type and holder is not None:      # `this: T`: the receiver given to its declared type
        values.consume(an, unit.this_type, env, an_node(an, holder), values.sub(site, (gid, THIS)), node)
    if unit.result_type:
        an.solver.follow(values.produce(an, unit.result_type, env, values.sub(site, gid)), node)
    summaries.summarize(an, node, unit, args, site, parameter)     # what no declaration states (Task 5)
    if holder is not None:
        summaries.kept(an, node, holder, unit, args, site, parameter)   # the platform's state on its receiver


def parameter(unit, index: int) -> tuple:
    """The declared type a declared function's argument at `index` takes."""
    types = tuple(unit.param_types)
    return positional(types, len(types) - 1 if unit.variadic else -1, index)


def positional(types: tuple, rest: int, index: int) -> tuple:
    """The declared type an argument at `index` takes: its parameter's; from the rest parameter on, one element of
    the rest type (an array's element type, a tuple's part, a type parameter's element: `A[number]`)."""
    if 0 <= rest < len(types) and index >= rest:
        tree = types[rest]
        if tree and tree[0] == T_NAMED and tree[1] == ARRAY_TYPE and tree[2]:
            return tree[2][0]
        if tree and tree[0] == T_TUPLE:
            return tree[1][index - rest] if index - rest < len(tree[1]) else ()
        if tree and tree[0] == T_PARAM:
            return (T_INDEX, tree, (T_VALUES, (NUMBER,)))
        return tree
    return types[index] if index < len(types) else ()


def rest_array(an, types: tuple, rest: int, args: list, env, site, origin) -> None:
    """`...args: A` with A a type parameter: the arguments from the rest parameter on are one array made at the call,
    given to A."""
    if not 0 <= rest < len(types) or not types[rest] or types[rest][0] != T_PARAM:
        return
    array = an.objects.array(None, site, ())
    node = (REST, site)
    an.solver.add(node, [array])
    for arg in args[rest:]:
        if arg is not None:
            an.solver.follow(arg, members.field(an, array, ELEMENTS_FIELD))
    values.consume(an, types[rest], env, node, site, origin)


def taken(an, obj: int, tree: tuple, env, site, origin) -> None:
    """One object given to a place of a function, named or object type (a function given to a declared function type
    by a method is kept on the method's receiver: `summaries.keep`)."""
    kind = tree[0]
    if env[0] is not None and an.objects.function(obj) is not None:
        for kept in function_types(an, tree):
            summaries.keep(an, env[0], obj, kept)
    if kind == T_FN:
        callback(an, obj, tree, env, site, origin, None)
    elif kind == T_TUPLE:
        for i, part in enumerate(tree[1]):
            values.consume(an, part, env, members.field(an, obj, ELEMENTS_FIELD), values.sub(site, i), origin)
    elif kind == T_OBJECT:
        structural(an, obj, tree[1], env, site, origin)
    elif kind == T_NAMED:
        found = typesys.resolve(an, tree[1], tree[3], tree[4])
        if found is None:
            return
        if found[0] == typesys.ALIAS:
            alias = an.ix.units[found[1]]
            if found not in values.path(site):
                values.consume(an, values.substitute(alias.result_type, values.arguments(tree[2], alias.tparams)),
                               env, an_node(an, obj), values.sub(site, found), origin)
            return
        key = found[1]
        if key in typesys.own_types(an, obj) or any(key in heritage(an, k) for k in typesys.own_types(an, obj)):
            for i, arg in enumerate(tree[2]):                    # nominally: its places give to the arguments
                values.consume(an, arg, env, typesys.place(an, obj, key, i), values.sub(site, i), origin)
        else:                                                    # structurally, through what the type declares
            bound = an.objects.declared(key, values.sub(site, (values.BOUND, key)), None)   # one per site and type
            for i, arg in enumerate(tree[2]):
                held = typesys.place(an, bound, key, i)
                values.consume(an, arg, env, held, values.sub(site, i), origin)
                an.solver.follow(values.produce(an, arg, env, values.sub(site, i)), held)
            structural(an, obj, key, (bound, key, env[2]), site, origin)


def function_types(an, tree: tuple) -> list:
    """The declared function types a type gives a function: itself, or a named type's call signatures."""
    if tree[0] == T_FN:
        return [tree]
    if tree[0] != T_NAMED:
        return []
    found = typesys.resolve(an, tree[1], tree[3], tree[4])
    if found is None or found[0] != typesys.CLASS:
        return []
    return [signature(an.ix.units[g]) for g in typesys.lookup(an, found[1], CALL_SIGNATURE)]


def an_node(an, obj: int):
    """A node holding one object."""
    node = (READ_MEMBER, obj)
    an.solver.add(node, [obj])
    return node


def heritage(an, key: str) -> set:
    """Every type `key` has through its heritage."""
    memo = an.memo.setdefault(heritage, {})
    if key not in memo:
        memo[key], todo = set(), [key]
        while todo:
            k = todo.pop()
            for g in typesys.group(an, k):
                for base in an.ix.base_keys(an.ix.classes[g]) if g in an.ix.classes else []:
                    base = typesys.identity(an, base)
                    if base not in memo[key]:
                        memo[key].add(base)
                        todo.append(base)
    return memo[key]


def structural(an, obj: int, key: str, env, site, origin) -> None:
    """An object the program made, given to a type it is not nominally (once per object, type and environment —
    log/trace-research.md F4): each method the type declares that the object has, read on it and called back; each
    such property through which values reach the program (a function type, a type parameter in its type) or which no
    type describes (the call's pool, Task 5), read on it and given to that type; its index signatures' values; a call
    signature, if it is a function, calls it back. An object the platform made is the platform's own."""
    ix = an.ix
    done = (STRUCTURAL, obj, key, env)
    if an.objects.kind(obj) not in PROGRAM_MADE or done in an.wired:
        return
    an.wired.add(done)
    own = an.objects.class_of(obj)
    for k in [key] + sorted(heritage(an, key)):
        for g in typesys.group(an, k):
            for name, gids in ix.members[g].items():
                for gid in gids:
                    unit = ix.units[gid]
                    for key_type, value in unit.indexes:        # an index signature: what the object holds
                        held = members.field(an, obj, ELEMENTS_FIELD) if typesys.NUMBER in typesys.values(key_type) \
                            else members.every(an, obj)
                        values.consume(an, value, env, held, values.member_site(gid, site), origin)
                    if unit.indexes:
                        continue
                    if name == CALL_SIGNATURE:
                        if an.objects.kind(obj) == SITE_FUNCTION:
                            callback(an, obj, signature(unit), env, values.member_site(gid, site), origin, None)
                        continue
                    if own is None or own not in ix.classes or not typesys.lookup(an, own, name):
                        continue                         # a member the object does not have
                    tree = signature(unit) if an.is_function_unit(gid) else unit.result_type
                    if not an.is_function_unit(gid) and not reaches(tree) and not summaries.is_open(an, tree):
                        continue                         # a method is called back; a property, where values reach
                    read = (READ_MEMBER, obj, name)
                    members.read(an, read, obj, name)
                    if an.is_function_unit(gid):
                        an.solver.subscribe(read, (READ_MEMBER, obj, name, gid), (T_FN,), lambda _, fns, u=unit, g=gid:
                                            [callback(an, f, signature(u), env, values.member_site(g, site), origin, obj)
                                             for f in fns])
                    else:
                        values.consume(an, tree, env, read, values.member_site(gid, site), origin)


def reaches(tree) -> bool:
    """Whether a type has a function type or a type parameter in it: a place values reach the program through."""
    if not isinstance(tree, tuple) or not tree:
        return False
    if tree[0] in (T_FN, T_PARAM):
        return True
    return any(reaches(part) for part in tree[1:] if isinstance(part, tuple))


def signature(unit) -> tuple:
    """A declared function as a function type."""
    rest = len(unit.param_types) - 1 if unit.variadic else -1
    return (T_FN, tuple(unit.param_types), unit.result_type or (T_OPEN,), rest, (T_OPEN,))


def callback(an, fn: int, tree: tuple, env, site, origin, this) -> None:
    """The platform calls `fn` back as a function of type `tree`: its arguments what the parameters' types give, its
    result taken as the result type; a link from the call that handed it. One call node for the function at a site
    whatever receiver the declared call had (its run is the same for all: log/trace-research.md F35): each receiver's
    environment gives its arguments into the node's own, and the result is taken into each receiver's places."""
    static = an.objects.make(CALLBACK_SITE, site[0])
    node = (CALLBACK, static, fn, this, site[1])
    where = (static, site[1])
    gid = an.objects.function(fn)
    count = len(an.ix.units[gid].params) if gid is not None else len(tree[1])
    args = [argument(an, node, positional(tree[1], tree[3], i), env, values.sub(site, i), origin) for i in range(count)]
    an.relay(origin, node)
    if node not in an.wired:                                    # called once; later receivers only add arguments
        an.wired.add(node)
        an.call(node, fn, this, args, HANDED, site=where)
    if this is None and tree[4] and tree[4][0] != T_OPEN:       # the `this` its declared type gives
        receivers = values.produce(an, tree[4], env, values.sub(site, THIS))
        an.solver.subscribe(receivers, (RECEIVERS, node), (RECEIVERS,), lambda _, objs: [
            an.call(node, fn, o, args, HANDED, site=where) for o in objs])
    if tree[2]:
        values.consume(an, tree[2], env, node, values.sub(site, values.RESULT), origin)


def argument(an, node, tree: tuple, env, site, origin):
    """What the platform passes a callback's parameter, in the call node's own argument: what its declared type gives
    in this environment and, where that type is open, whatever the call handing the callback took (its pool:
    Averroes's placeholder principle)."""
    held = (CALLBACK_ARG, node, site)
    an.solver.follow(values.produce(an, tree, env, site), held)
    if origin is not None and summaries.is_open(an, tree):
        an.solver.follow(summaries.pool(origin), held)
    return held


def call_object(an, node, fn: int, receiver, args: list, where) -> None:
    """A call of an object that is no function of the program, at the static site `where`: a declared function the
    platform gave, the function `await` hands a thenable, an object whose declared type has a call signature."""
    key, kind = an.objects.keys[fn], an.objects.kind(fn)
    if kind == SITE_DECLARED_FN:
        tree, env, site = key[1], key[2], (key[3], key[4])
        for i, arg in enumerate(args):
            part = positional(tree[1], tree[3], i)
            if part and arg is not None:
                values.consume(an, part, env, arg, values.sub(site, i), node)
        rest_array(an, tree[1], tree[3], args, env, values.sub(site, REST), node)
        if tree[2]:
            an.solver.follow(values.produce(an, tree[2], env, values.sub(site, values.RESULT)), node)
    elif kind == SITE_UNKNOWN:                     # what no declaration describes: an open call
        summaries.open_call(an, node, args, where)
    elif kind == SITE_RESOLVER:
        if args and args[0] is not None:
            from . import protocols
            protocols.settle(an, key[1], args[0])
    else:
        signatures(an, node, fn, CALL_SIGNATURE, args, where)


def construct(an, node, obj: int, args: list) -> None:
    """`new` on an object of a declared type: its construct signatures."""
    signatures(an, node, obj, CONSTRUCT_SIGNATURE, args, call_site(an, node))


def signatures(an, node, obj: int, name: str, args: list, where) -> None:
    for key in typesys.own_types(an, obj):
        for gid in typesys.lookup(an, key, name):
            an.reached(node, gid, name == CONSTRUCT_SIGNATURE)
            apply(an, node, gid, obj, args, where)

