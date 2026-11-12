"""What a declared type gives and takes (log/trace-core-plan.md, Task 4): a declared result, property or variable gives
the values its type describes — a type parameter's place, the language's values of a primitive or a literal type, an
object of a named or written object type made at that site (its type arguments in its places), a function of a
function type, the receiver for `this`; an argument given to a parameter goes where the parameter's type says — into a
type parameter's place, through the places of an object of a named type it is (nominally) or the members its type
declares (structurally, where a function or a type parameter is), and a function given for a function type is called
back with the values its parameters' types give, its result taken as the function type's result. An environment is
(the object whose type parameters the types read, that type, the call whose own type parameters they read); a site is
(where, as a static key, the heap context) so every object made here is finite like the program's own.
"""
from __future__ import annotations

from ..languages import ARRAY_TYPE, INDEX_SIGNATURE
from ..solver import ENGINE, LANG, NUMBER, TEXT
from ..vocab import (ELEMENTS_FIELD, LEVEL_ALIAS, LEVEL_OWN, LEVEL_TYPE, SITE_DECLARED_FN, SITE_TEXT, SITE_UNKNOWN,
                     T_BOTH,
                     T_FN, T_INDEX, T_NAMED, T_OBJECT, T_PARAM, T_QUERY, T_THIS, T_TUPLE, T_UNION, T_VALUES)
from . import copies, members, names, summaries, typesys

PRODUCED, CONSUMED, OWN_PLACE, RESULT, MEMBER, BOUND = 20, 21, 22, 23, 24, 25   # node tags; a site's parts
NO_ENV = (None, None, None)          # the environment of a type that reads none


def sub(site, part) -> tuple:
    """A site within a site: a part of the same declaration's type (bounded by the type as written)."""
    return ((site[0], part), site[1])


def member_site(gid: int, site) -> tuple:
    """The site of what a member of a declared type gives: the member itself and the heap context — never the site
    of the object it is read on, so a type that contains itself makes one object per member, not one per level."""
    return ((MEMBER, gid), site[1])


def own_place(env, index: int):
    return (OWN_PLACE, env[2], index)


def level_place(an, env, level: int, index: int):
    """The place a type parameter of `level` names in `env` (None where the environment has none)."""
    if env is None:
        return None
    if level == LEVEL_TYPE and env[0] is not None:
        return typesys.place(an, env[0], env[1], index)
    if level == LEVEL_OWN and env[2] is not None:
        return own_place(env, index)
    return None


def produce(an, tree: tuple, env, site):
    """The node holding the values a declared type gives in `env` at `site`. The language's values are one node for
    all; a type parameter is its place itself; `this` the node of the receiver itself (F19); a type that reads nothing
    of the environment (no type parameter, no `this`) is keyed without it, so one node and one object serve every
    receiver (log/trace-research.md F12)."""
    kind = tree[0] if tree else None
    if kind == T_VALUES:
        node = (PRODUCED, tree)
    elif kind == T_PARAM:
        held = level_place(an, env, tree[1], tree[2])
        if held is not None:
            return held
        node = (PRODUCED, ())
    elif kind == T_THIS:                  # the receiver itself: one node per object, never one per call and site
        if env is not None and env[0] is not None:
            from .declared import an_node
            return an_node(an, env[0])
        node = (PRODUCED, ())
    else:
        node = (PRODUCED, tree, env if reads(tree) else NO_ENV, site)
    if node not in an.wired:
        an.wired.add(node)
        wire(an, node, tree, env if reads(tree) else NO_ENV, site)
    return node


def reads(tree) -> bool:
    """Whether a declared type reads its environment: a type parameter or `this` in it."""
    memo = READS.get(tree)
    if memo is None:
        tagged = bool(tree) and isinstance(tree[0], str)
        memo = READS[tree] = bool(tree) and (tagged and tree[0] in (T_PARAM, T_THIS) or any(
            reads(part) for part in (tree[1:] if tagged else tree) if isinstance(part, tuple)))
    return memo


READS: dict = {}


def wire(an, node, tree, env, site) -> None:
    kind = tree[0] if tree else None
    solver, objects = an.solver, an.objects
    if kind == T_PARAM:
        held = level_place(an, env, tree[1], tree[2])
        if held is not None:
            solver.follow(held, node)
    elif kind == T_UNION:
        for i, part in enumerate(tree[1]):
            solver.follow(produce(an, part, env, sub(site, i)), node)
    elif kind == T_VALUES:
        solver.add(node, language_objects(an, tree[1]))
    elif kind == T_BOTH:
        both(an, node, tree, env, site)
    elif kind == T_NAMED:
        named(an, node, tree, env, site)
    elif kind == T_OBJECT:
        made = objects.declared(tree[1], site, env)
        solver.add(node, [made])
        indexes(an, made, tree[1], env, site)
    elif kind == T_FN:
        solver.add(node, [objects.make(SITE_DECLARED_FN, tree, env, site[0], site[1])])
    elif kind == T_TUPLE:
        array = objects.array(None, site, ())
        solver.add(node, [array])
        for i, part in enumerate(tree[1]):
            solver.follow(produce(an, part, env, sub(site, i)), members.field(an, array, ELEMENTS_FIELD))
    elif kind == T_INDEX:
        texts = [v[len(TEXT):] for v in typesys.values(tree[2]) if v.startswith(TEXT)]
        solver.subscribe(produce(an, tree[1], env, sub(site, 0)), node, (T_INDEX,), lambda t, objs: [
            members.read(an, t, o, text) for o in objs for text in texts] if texts else [
            members.read_all(an, t, o) for o in objs])
    elif kind == T_QUERY:
        for gid in names.definitions(an, Scope(tree[2], tree[3]), tree[1]):
            names.value_of(an, gid, node)


def both(an, node, tree, env, site) -> None:
    """An intersection: the objects of each part, each one with the properties of the others' objects by name (a
    value of `A & B` is one object that is both: `copies`)."""
    parts = [produce(an, part, env, sub(site, i)) for i, part in enumerate(tree[1])]
    for i, held in enumerate(parts):
        an.solver.follow(held, node)
        for j, other in enumerate(parts):
            if i != j:
                an.solver.subscribe(held, (T_BOTH, node, i, j), (T_BOTH,), lambda _, objs, other=other: [
                    an.solver.subscribe(other, an.any_field(o), (T_BOTH, o), lambda _, srcs, o=o: [
                        copies.copy(an, s, o) for s in srcs]) for o in objs])


def named(an, node, tree, env, site) -> None:
    """A named type: an alias's type with its arguments, or an object of the class or interface made at the site, its
    type arguments in its places and what its index signatures give in its elements; a class of the program gives
    every instance of it the program made as well."""
    found = typesys.resolve(an, tree[1], tree[3], tree[4])
    solver = an.solver
    if found is None:
        if tree[1] == ARRAY_TYPE and tree[2]:                 # `T[]` with no lib read: an array of the language
            array = an.objects.array(None, site, ())
            solver.add(node, [array])
            solver.follow(produce(an, tree[2][0], env, sub(site, 0)), members.field(an, array, ELEMENTS_FIELD))
        else:
            solver.add(node, [an.objects.make(SITE_UNKNOWN)])
        return
    if found[0] == typesys.ALIAS:
        if any(part == found for part in path(site)):         # an alias inside its own expansion: nothing more
            return
        alias = an.ix.units[found[1]]
        solver.follow(produce(an, substitute(alias.result_type, arguments(tree[2], alias.tparams)), env,
                              sub(site, found)), node)
        return
    key = found[1]
    cls = an.ix.classes.get(key)
    own = cls is not None and not cls.shape and not cls.ambient   # a class of the program: only the program makes
    made = an.objects.from_outside(key) if own else an.objects.declared(key, site, None)
    solver.add(node, [made])
    if own:                                                       # its objects: one from outside is one it made
        solver.follow(an.instances(key), node)
    params = max((len(an.ix.classes[k].tparams) for k in typesys.group(an, key)), default=0)
    for i in range(params):
        arg = tree[2][i] if i < len(tree[2]) else None
        if arg is not None:
            solver.follow(produce(an, arg, env, sub(site, i)), typesys.place(an, made, key, i))
    indexes(an, made, key, (made, key, None), site)


def indexes(an, made: int, key: str, env, site) -> None:
    """What a declared object's index signatures give, in its elements (a number key) or every property."""
    for gid in typesys.lookup(an, key, INDEX_SIGNATURE):
        for key_type, value in an.ix.units[gid].indexes:
            if any(p and p[0] == T_PARAM and p[1] == LEVEL_TYPE for p in typesys.parts(value)):
                continue                                       # a place: `typesys.keyed` links it
            field = members.field(an, made, ELEMENTS_FIELD) if NUMBER in typesys.values(key_type) else \
                an.any_field(made)
            an.solver.follow(produce(an, value, env, member_site(gid, site)), field)


def consume(an, tree: tuple, env, source, site, origin) -> None:
    """The values of `source` given to a place of a declared type in `env` (`origin`: the call they were given to)."""
    kind = tree[0] if tree else None
    if summaries.is_open(an, tree) and origin is not None:     # a position no type describes: the call's pool
        an.solver.follow(source, summaries.pool(origin))
    if kind == T_PARAM:
        held = level_place(an, env, tree[1], tree[2])
        if held is not None:
            an.solver.follow(source, held)
    elif kind in (T_UNION, T_BOTH):
        for i, part in enumerate(tree[1]):
            consume(an, part, env, source, sub(site, i), origin)
    elif kind in (T_FN, T_NAMED, T_OBJECT, T_TUPLE):
        from . import declared
        an.solver.subscribe(source, (CONSUMED, tree, env, site, origin), (CONSUMED,), lambda _, objs: [
            declared.taken(an, o, tree, env, site, origin) for o in objs])


def language_objects(an, values: tuple) -> list[int]:
    """The objects of the language's values: a primitive of its kind, a known string, the unknown object for any."""
    out = []
    for v in values:
        if v == ENGINE:
            out.append(an.objects.make(SITE_UNKNOWN))
        elif v.startswith(TEXT):                      # a string literal type: the value of its text (F33)
            out.append(an.objects.make(SITE_TEXT, v[len(TEXT):]))
        elif v.startswith(LANG):
            out.append(an.objects.primitive(v[len(LANG):]))
    return out


def arguments(args: tuple, defaults: tuple) -> tuple:
    """Type arguments, a parameter's default where none is written."""
    return tuple(args[i] if i < len(args) else defaults[i] for i in range(max(len(args), len(defaults))))


def substitute(tree, args: tuple):
    """An alias's type with its type parameters replaced by the arguments written."""
    if not isinstance(tree, tuple) or not tree:
        return tree
    if tree[0] == T_PARAM and tree[1] == LEVEL_ALIAS:
        return args[tree[2]] if tree[2] < len(args) else ()
    return tuple(substitute(part, args) if isinstance(part, tuple) else part for part in tree)


def path(site) -> list:
    """The parts a site is made of, outermost last."""
    out, where = [], site[0]
    while isinstance(where, tuple) and len(where) == 2:
        out.append(where[1])
        where = where[0]
    return out


class Scope:
    """Where a name in a declared type is written: what `names.definitions` reads."""

    def __init__(self, file: str, owner: str):
        self.file, self.owner = file, owner

