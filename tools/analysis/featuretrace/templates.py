"""Call results per argument values (log/trace-rules.md, "Call results per argument values: templates", C1-C3).

A function whose parameters reach its result (directly through locals, unions and `await`, through a member or element
read of them, or through a call by name of such a function on them) is answered per tuple of those parameters' values
at each call: Agesen's Cartesian product on the parameters Zipper's flow patterns select. The template of a tuple is
the unit read under that tuple (a `Frame`): its key parameters hold the tuple, its result is the call's; the call holds
the union over its tuples, as values only grow. Only the trees its key reaches (`depends`, by the same flows) are the
template's own nodes: every other tree is the unit's, shared by all its templates and delivered once (C2'''). The call
graph keeps the unit's own links, fed by every call (a superset). A closure whose free variables read a parameter
reaches the result as the parameter would (C4): its maker is answered per that parameter's values, and a closure made
in a template is the function with that template as its environment (C4', `Frame.env`): run, its free variables read
the template, its own templates nest in it.
"""
from __future__ import annotations

from . import receivers
from .closures import closed, free_names, function_of, functions, written_on
from .solver import FN, UNDEFINED, identity

PASS_SHAPES = ("member", "index", "elements", "each", "await", "addr", "put", "deref", "narrow")
MISSING = ("<missing>",)          # a key parameter its call does not write: its default, one template
RECEIVER = -1                     # the object a method runs on, a key when `this` reaches the result (Agesen: the
                                  # receiver is in the tuple; J2')
CLOSED = -2                       # a closure frame's key: the environment it runs in (C4')


class Frame:
    """A unit read under a key: every attribute is the unit's but `key`, which keys its value nodes. A template of a
    call's argument values (`keyed`), or a closure run in the template its maker made it in (`env`, C4'); a template of
    a closure's own call nests in its closure frame (its key keyed by that frame's)."""

    def __init__(self, unit, key: tuple, env=None):
        self._unit = unit
        self.key = (unit.key, key)
        self.keyed = env is None                            # keyed by its call's argument values
        self.env = env if env is not None else getattr(unit, "env", None)
        self.fixed = key[0] if self.keyed and key[1] != MISSING else None    # the parameter holding its key value

    @property
    def base(self):
        """The unit it is a template of."""
        return self._unit

    def __getattr__(self, name):
        return getattr(self._unit, name)


def key_params(t, unit) -> tuple:
    """The parameters (indices) that reach the function's result (C1), and `RECEIVER` when its `this` does; () when
    none does."""
    memo = t.template_keys
    if unit.gid not in memo:
        memo[unit.gid] = ()                     # read as none while it is worked out (recursion)
        found = set()
        if unit.returns:
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
            for part in unit.types[name] + written_on(unit, name):     # a closure's, its properties too (C4')
                out |= reaches(t, unit, part, seen)
        return out
    if kind == "self":                              # `this`: the receiver of a method's call (J2')
        return {RECEIVER} if receivers.runs_on(t, unit) else set()
    if kind == "callable":                          # a closure: the parameters its free variables read (C4)
        out = set()
        for name in free_names(t, closed(t, unit, tree[1])):
            out |= reaches(t, unit, ("name", name), seen)
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
                if 0 <= index < len(tree[2]):
                    out |= reaches(t, unit, tree[2][index], seen)
        return out
    return set()


def depends(t, unit, tree) -> bool:
    """Whether a tree's value can differ between the function's templates: it reads a key parameter, or (in a closure
    frame) a free variable its environment binds, through any shape (an operand, a receiver, an argument, a local
    holding one, a closure reading one). Only those trees are a template's own nodes; any other reads nothing a
    template binds, so it is the unit's, the same in all its templates (C2''', C4')."""
    memo = t.template_depends
    keyed, env = getattr(unit, "keyed", False), getattr(unit, "env", None) is not None
    key = (unit.gid, keyed, env, tree)
    if key not in memo:
        keys = set(t.template_keys.get(unit.gid, ())) if keyed else set()
        keys |= free_names(t, unit) if env else set()
        memo[key] = bool(keys) and bool(reads(t, unit, tree, set()) & keys)
    return memo[key]


def reads(t, unit, tree, seen: set) -> set:
    """The parameters (indices) and the free variables (names) a tree reads: a parameter or a captured name named, a
    local's every value, a closure's free variables, and every part of any shape (a tree is a tuple led by its kind;
    the other tuples and lists in it are sequences of trees)."""
    if isinstance(tree, list) or isinstance(tree, tuple) and not (tree and isinstance(tree[0], str)):
        return set().union(*(reads(t, unit, part, seen) for part in tree)) if tree else set()
    if not isinstance(tree, tuple):
        return set()
    if tree[0] == "name":
        name, out = tree[1], set()
        if name in unit.params:
            out.add(unit.params.index(name))
        if name in unit.captured:
            out.add(name)                       # a free variable: what the closure's environment binds (C4')
        if name in unit.types and name not in seen:
            seen.add(name)
            out |= reads(t, unit, unit.types[name] + written_on(unit, name), seen)
        return out
    if tree[0] == "self":
        return {RECEIVER}                       # `this`: the receiver (J2')
    if tree[0] == "callable":                   # a closure: what its free variables read here (C4)
        return reads(t, unit, [("name", n) for n in free_names(t, closed(t, unit, tree[1]))], seen)
    return set().union(*(reads(t, unit, part, seen) for part in tree[1:] if isinstance(part, (tuple, list))))


def leaves(tree) -> list:
    """A result's parts: a union opened (each side delivered on its own, so the shared ones are delivered once)."""
    if isinstance(tree, tuple) and tree and tree[0] == "union":
        return leaves(tree[1]) + leaves(tree[2])
    return [tree]


def route(t, target, unit, args: list, keys: tuple, deliver, receiver=None) -> None:
    """A call of `unit` whose argument nodes are `args` (None where not written) on `receiver` (the one value it is
    called on, J2'): for each key parameter (`RECEIVER` among them), each value it holds at the call is a template (that
    parameter fixed, the others what the call writes); the call's result is the intersection over the key parameters
    of each one's templates' results (C2'), `deliver(node)` bringing it into `target`. One key parameter: Agesen's
    templates exactly. The call's result is one node per call and function, whatever reads it: a second reading of the
    same call (another receiver with the same method) delivers it again."""
    made = {}                                   # (key parameter, value identity) -> Frame, for this call
    single = len(keys) == 1
    families = {k: ("#family", target, unit.key, k) for k in keys}
    nodes = {i: a for i, a in enumerate(args) if a is not None}
    if receiver is not None:                    # the receiver as a node of its one value
        nodes[RECEIVER] = ("#receiver", identity(*receiver))
        t.add(nodes[RECEIVER], [receiver])

    def make(k: int, value) -> None:
        tkey = (k, MISSING if value is None else key_of(value))
        if tkey in made:
            return
        frame = t.templates.get((unit.key, tkey))
        if frame is None:
            frame = t.templates[(unit.key, tkey)] = Frame(unit, tkey)
        made[tkey] = frame
        if value is not None:
            t.add(t.arg_node(frame, k), [value])
        elif k == RECEIVER:                     # called with no receiver written: what the method's `this` holds
            t.follow(receivers.this_node(t, unit), t.arg_node(frame, k))
        elif omitted_is_undefined(unit, k):
            t.add(t.arg_node(frame, k), [(UNDEFINED, unit)])     # JS: an argument left out (MDN Functions)
        for index in keys:                      # the other key parameters: what this call writes (C2')
            if index != k and index in nodes:
                t.add(t.arg_node(frame, index), t.held(nodes[index]))
        for part in t.ret_parts(frame):         # the template's own parts, and the unit's shared ones (bounded, S2)
            t.result_flow(part, families[k], frame)

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
        if k not in nodes:
            make(k, None)
        else:
            t.subscribe(nodes[k], target, ("route", unit.key, "key", k), keyed(k))
    if single:
        deliver(families[keys[0]])
        return
    for index in keys:                          # a key parameter's later values: the templates keyed by another
        if index in nodes:
            t.subscribe(nodes[index], target, ("route", unit.key, "arg", index), other(index))
    joined = ("#meet", target, unit.key)
    for k in keys:
        t.subscribe(families[k], joined, ("meet", k), meets(t, families, k))
    deliver(joined)


def key_of(value) -> tuple:
    """What a value keys a template by: itself, a closure by its function (Agesen: a closure's type is the function
    written, not the environment it was made in), so the templates a closure argument makes are finite (C4')."""
    text, ctx = value
    return identity(function_of(text), ctx) if text.startswith(FN) else identity(text, ctx)


def omitted_is_undefined(unit, index: int) -> bool:
    """Whether a JS parameter a call writes no argument for holds `undefined`: not when written with a default, nor a
    rest parameter (an empty array)."""
    rest = unit.variadic and index == len(unit.params) - 1
    return unit.lang == "js" and index not in unit.defaults and not rest


def meets(t, families: dict, k: int):
    """A value of key parameter k's templates' results reaches the call when every other key parameter's family holds
    it too: each family is a superset of the call's result, so is their intersection (C2'). A closure is the same
    function in every family, each made in that family's template (C4'): the function is what they must all hold, and
    then each family's closure of it (its environment binding its own key) reaches the call."""
    def transfer(target, values):
        others = [t.known.get(t.find(node), set()) for j, node in families.items() if j != k]
        kept = [(text, ctx) for text, ctx in values
                if not text.startswith(FN) and all(identity(text, ctx) in seen for seen in others)]
        made = {function_of(text) for text, _ in values if text.startswith(FN)}
        shared = {f for f in made if all(f in functions(t, node) for j, node in families.items() if j != k)}
        if shared:
            for node in families.values():          # every family's closure of a function they all hold
                kept += [(text, ctx) for text, ctx in t.held(node) if text.startswith(FN) and function_of(text) in shared]
        t.add(target, kept)
    return transfer
