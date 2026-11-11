"""What a declared function does: what its declaration says (log/trace-core-plan.md, Task 4). A call of a function
that is only declared (a platform's, a library's) puts each argument where its parameter's type says and gives what
its result type gives, in the environment of the receiver it is called on and of this call's own type parameters; a
function the program hands to a function type is called back by it; `new` on an object of a declared type runs its
construct signature, a call of it its call signature; a function of a declared function type the platform gave, called
by the program, takes its arguments the same way; the function `await` hands a thenable takes the awaited value.
"""
from __future__ import annotations

from ..languages import ARRAY_TYPE, CALL_SIGNATURE, CONSTRUCT_SIGNATURE
from ..vocab import (ELEMENTS_FIELD, HANDED, SITE_DECLARED_FN, SITE_FUNCTION, SITE_RESOLVER, T_FN, T_NAMED, T_OBJECT,
                     T_OPEN, T_PARAM, T_TUPLE)
from . import contexts, members, typesys, values

CALLBACK, CALLBACK_SITE, READ_MEMBER = 30, 31, 32          # node tags


def call_site(an, node) -> tuple:
    """A call's site for what its declaration makes: the call's place without its context, and the heap context."""
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
    if unit.result_type:
        an.solver.follow(values.produce(an, unit.result_type, env, values.sub(site, gid)), node)


def parameter(unit, index: int) -> tuple:
    """The declared type an argument at `index` takes: its parameter's, past the rest parameter its element type."""
    types = unit.param_types
    if unit.variadic and types and index >= len(types) - 1:
        rest = types[-1]
        if rest and rest[0] == T_NAMED and rest[1] == ARRAY_TYPE and rest[2]:
            return rest[2][0]
        return rest[1][index - len(types) + 1] if rest and rest[0] == T_TUPLE and index - len(types) + 1 < \
            len(rest[1]) else ()
    return types[index] if index < len(types) else ()


def taken(an, obj: int, tree: tuple, env, site, origin) -> None:
    """One object given to a place of a function, named or object type."""
    kind = tree[0]
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
            bound = an.objects.declared(key, ((values.BOUND, key), site[1]), None)
            for i, arg in enumerate(tree[2]):
                held = typesys.place(an, bound, key, i)
                values.consume(an, arg, env, held, values.sub(site, i), origin)
                an.solver.follow(values.produce(an, arg, env, values.sub(site, i)), held)
            structural(an, obj, key, (bound, key, env[2]), site, origin)


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
    """An object given to a type it is not nominally: each method the type declares read on it and called back, each
    property through which values reach the program (a function type, a type parameter in its type) read on it and
    given to that type; a call signature, if it is a function, calls it back."""
    ix = an.ix
    for k in [key] + sorted(heritage(an, key)):
        for g in typesys.group(an, k):
            for name, gids in ix.members[g].items():
                for gid in gids:
                    unit = ix.units[gid]
                    if name == CALL_SIGNATURE:
                        if an.objects.kind(obj) == SITE_FUNCTION:
                            callback(an, obj, signature(unit), env, values.member_site(gid, site), origin, None)
                        continue
                    tree = signature(unit) if an.is_function_unit(gid) else unit.result_type
                    if not an.is_function_unit(gid) and not reaches(tree):
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
    result taken as the result type; a link from the call that handed it."""
    static = an.objects.make(CALLBACK_SITE, site[0])
    node = (CALLBACK, static, fn, env, site[1])
    args = [values.produce(an, p, env, values.sub(site, i)) for i, p in enumerate(tree[1])]
    an.relay(origin, node)
    an.call(node, fn, this, args, HANDED, site=static)
    if tree[2]:
        values.consume(an, tree[2], env, node, values.sub(site, values.RESULT), origin)


def call_object(an, node, fn: int, receiver, args: list) -> None:
    """A call of an object that is no function of the program: a declared function the platform gave, the function
    `await` hands a thenable, an object whose declared type has a call signature."""
    key, kind = an.objects.keys[fn], an.objects.kind(fn)
    if kind == SITE_DECLARED_FN:
        tree, env, site = key[1], key[2], (key[3], key[4])
        for i, arg in enumerate(args):
            if i < len(tree[1]) and arg is not None:
                values.consume(an, tree[1][i], env, arg, values.sub(site, i), node)
        if tree[2]:
            an.solver.follow(values.produce(an, tree[2], env, values.sub(site, values.RESULT)), node)
    elif kind == SITE_RESOLVER:
        if args and args[0] is not None:
            from . import protocols
            protocols.settle(an, key[1], args[0])
    else:
        signatures(an, node, fn, CALL_SIGNATURE, args)


def construct(an, node, obj: int, args: list) -> None:
    """`new` on an object of a declared type: its construct signatures."""
    signatures(an, node, obj, CONSTRUCT_SIGNATURE, args)


def signatures(an, node, obj: int, name: str, args: list) -> None:
    for key in typesys.own_types(an, obj):
        for gid in typesys.lookup(an, key, name):
            an.reached(node, gid, name == CONSTRUCT_SIGNATURE)
            apply(an, node, gid, obj, args, call_site(an, node))

