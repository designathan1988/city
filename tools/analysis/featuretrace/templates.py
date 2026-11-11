"""Call results per argument values (log/trace-rules.md, "Call results per argument values: templates", C1-C3).

A function whose parameters reach its result (directly through locals, unions and `await`, through a member or element
read of them, or through a call by name of such a function on them) is answered per tuple of those parameters' values
at each call: Agesen's Cartesian product on the parameters Zipper's flow patterns select. The template of a tuple is
the unit read under that tuple (a `Frame`): its key parameters hold the tuple, its result is the call's; the call holds
the union over its tuples, as values only grow. Only the trees its key reaches (`depends`, by the same flows) are the
template's own nodes: every other tree is the unit's, shared by all its templates and delivered once (C2'''). The call
graph keeps the unit's own links, fed by every call (a superset): a template's closures read the unit's locals (C3).
"""
from __future__ import annotations

from .solver import UNDEFINED, identity

PASS_SHAPES = ("member", "index", "elements", "each", "await", "addr", "put", "deref", "narrow")
MISSING = ("<missing>",)          # a key parameter its call does not write: its default, one template


class Frame:
    """A unit read under a key: every attribute is the unit's but `key`, which keys its value nodes."""

    def __init__(self, unit, key: tuple):
        self._unit = unit
        self.key = (unit.gid, key)
        self.fixed = key[0] if key[1] != MISSING else None   # the parameter that holds its key value alone

    @property
    def base(self):
        """The unit it is a template of."""
        return self._unit

    def __getattr__(self, name):
        return getattr(self._unit, name)


def key_params(t, unit) -> tuple:
    """The parameters (indices) that reach the function's result (C1); () when none does."""
    memo = t.template_keys
    if unit.gid not in memo:
        memo[unit.gid] = ()                     # read as none while it is worked out (recursion)
        found = set()
        if unit.params and unit.returns:
            seen = set()
            for _, _, tree in unit.returns:
                found |= reaches(t, unit, tree, seen)
        memo[unit.gid] = tuple(sorted(found))
    return memo[unit.gid]


def reaches(t, unit, tree, seen: set) -> set:
    """The parameters whose value a tree can hold, or read a member or element of (C1)."""
    if not isinstance(tree, tuple) or not tree:
        return set()
    kind = tree[0]
    if kind == "name":
        name, out = tree[1], set()
        if name in unit.params:
            out.add(unit.params.index(name))
        if name in unit.types and name not in seen:
            seen.add(name)
            for part in unit.types[name]:
                out |= reaches(t, unit, part, seen)
        return out
    if kind in PASS_SHAPES:
        return reaches(t, unit, tree[1], seen)
    if kind == "union":
        return reaches(t, unit, tree[1], seen) | reaches(t, unit, tree[2], seen)
    if kind == "collection":
        return set().union(*(reaches(t, unit, part, seen) for part in tree[1]))
    if kind == "call" and tree[1][0] == "name":     # a call by name of a function whose result its key gives
        out = set()
        for gid in t.rs.function_targets(unit, tree[1][1]):
            for index in key_params(t, t.ix.units[gid]):
                if index < len(tree[2]):
                    out |= reaches(t, unit, tree[2][index], seen)
        return out
    return set()


def depends(t, unit, tree) -> bool:
    """Whether a tree's value can differ between the function's templates: it reads a key parameter, through any shape
    (an operand, a receiver, an argument, a local holding one). Only those trees are a template's own nodes; any other
    reads nothing a template binds, so it is the unit's, the same in all its templates (C2''')."""
    memo = t.template_depends
    key = (unit.gid, tree)
    if key not in memo:
        keys = set(t.template_keys.get(unit.gid, ()))
        memo[key] = bool(keys) and bool(reads(unit.base, tree, set()) & keys)
    return memo[key]


def reads(unit, tree, seen: set) -> set:
    """The parameters (indices) a tree reads: a parameter named, a local's every value, and every part of any shape
    (a tree is a tuple led by its kind; the other tuples and lists in it are sequences of trees)."""
    if isinstance(tree, list) or isinstance(tree, tuple) and not (tree and isinstance(tree[0], str)):
        return set().union(*(reads(unit, part, seen) for part in tree)) if tree else set()
    if not isinstance(tree, tuple):
        return set()
    if tree[0] == "name":
        name, out = tree[1], set()
        if name in unit.params:
            out.add(unit.params.index(name))
        if name in unit.types and name not in seen:
            seen.add(name)
            out |= reads(unit, unit.types[name], seen)
        return out
    return set().union(*(reads(unit, part, seen) for part in tree[1:] if isinstance(part, (tuple, list))))


def leaves(tree) -> list:
    """A result's parts: a union opened (each side delivered on its own, so the shared ones are delivered once)."""
    if isinstance(tree, tuple) and tree and tree[0] == "union":
        return leaves(tree[1]) + leaves(tree[2])
    return [tree]


def route(t, target, unit, args: list, keys: tuple, deliver) -> None:
    """A call of `unit` whose argument nodes are `args` (None where not written): for each key parameter, each value
    it holds at the call is a template (that parameter fixed, the others what the call writes); the call's result is
    the intersection over the key parameters of each one's templates' results (C2'), `deliver(node)` bringing it into
    `target`. One key parameter: Agesen's templates exactly. The call's result is one node per call and function,
    whatever reads it: a second reading of the same call (another receiver with the same method) delivers it again."""
    made = {}                                   # (key parameter, value identity) -> Frame, for this call
    single = len(keys) == 1
    families = {k: ("#family", target, unit.gid, k) for k in keys}

    def make(k: int, value) -> None:
        tkey = (k, MISSING if value is None else identity(*value))
        if tkey in made:
            return
        frame = t.templates.get((unit.gid, tkey))
        if frame is None:
            frame = t.templates[(unit.gid, tkey)] = Frame(unit, tkey)
        made[tkey] = frame
        if value is not None:
            t.add(t.arg_node(frame, k), [value])
        elif omitted_is_undefined(unit, k):
            t.add(t.arg_node(frame, k), [(UNDEFINED, unit)])     # JS: an argument left out (MDN Functions)
        for index in keys:                      # the other key parameters: what this call writes (C2')
            if index != k and index < len(args) and args[index] is not None:
                t.add(t.arg_node(frame, index), t.held(args[index]))
        for part in t.ret_parts(frame):         # the template's own parts, and the unit's shared ones
            t.follow(part, families[k])

    def keyed(k: int):
        def transfer(_, values):
            for value in values:
                make(k, value)
        return transfer

    def other(index: int):
        def transfer(_, values):            # the call's new values of a parameter: every template not keyed by it
            for (k, _), frame in list(made.items()):
                if k != index:
                    t.add(t.arg_node(frame, index), values)
        return transfer

    for k in keys:
        if k >= len(args) or args[k] is None:
            make(k, None)
        else:
            t.subscribe(args[k], target, ("route", unit.gid, "key", k), keyed(k))
    if single:
        deliver(families[keys[0]])
        return
    for index in keys:                          # a key parameter's later values: the templates keyed by another
        if index < len(args) and args[index] is not None:
            t.subscribe(args[index], target, ("route", unit.gid, "arg", index), other(index))
    joined = ("#meet", target, unit.gid)
    for k in keys:
        t.subscribe(families[k], joined, ("meet", k), meets(t, families, k))
    deliver(joined)


def omitted_is_undefined(unit, index: int) -> bool:
    """Whether a JS parameter a call writes no argument for holds `undefined`: not when written with a default, nor a
    rest parameter (an empty array)."""
    rest = unit.variadic and index == len(unit.params) - 1
    return unit.lang == "js" and index not in unit.defaults and not rest


def meets(t, families: dict, k: int):
    """A value of key parameter k's templates' results reaches the call when every other key parameter's family holds
    it too: each family is a superset of the call's result, so is their intersection (C2')."""
    def transfer(target, values):
        others = [t.known.get(t.find(node), set()) for j, node in families.items() if j != k]
        t.add(target, [(text, ctx) for text, ctx in values if all(identity(text, ctx) in seen for seen in others)])
    return transfer
