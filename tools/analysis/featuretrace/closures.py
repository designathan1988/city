"""What a closure is (log/trace-rules.md, C4, C4'; ECMAScript: a function made in a run closes over that run's
variables): its free variables (the enclosing functions' variables it reads, its own closures' too), what its maker
writes on it (`fn.tag = x`, its own properties per template), and its function whatever template made it.
"""
from __future__ import annotations

from . import js_transfer
from .model import LOCAL_REFS
from .solver import ENV, FN


def free_names(t, unit) -> frozenset:
    """The variables of its enclosing functions a function reads (its free variables, the ones it closes over): the
    captured names its references and trees read, and those its own closures read through it."""
    memo = t.free
    if unit.gid not in memo:
        memo[unit.gid] = frozenset()                         # read as none while worked out (a closure of itself)
        found = {r.name for r in unit.refs if r.base is None and r.kind in LOCAL_REFS and r.name in unit.captured}
        trees = [r.base for r in unit.refs] + [a for r in unit.refs for a in r.args] \
            + [v for vs in unit.types.values() for v in vs] + [tree for _, _, tree in unit.returns] + list(unit.yields)
        found |= {n for n in names_in(trees) if n in unit.captured}
        inner = callables_in(trees) | set(unit.lambdas.values()) | {r.target for r in unit.refs if r.kind == "lambda"}
        for lid in inner:                                    # its own closures' free variables it closes over too
            found |= {n for n in free_names(t, closed(t, unit, lid)) if n in unit.captured}
        memo[unit.gid] = frozenset(found)
    return memo[unit.gid]


def names_in(tree) -> set:
    """The names a tree reads (any shape)."""
    if isinstance(tree, tuple) and len(tree) == 2 and tree[0] == "name" and isinstance(tree[1], str):
        return {tree[1]}
    if isinstance(tree, (tuple, list)):
        return set().union(*(names_in(part) for part in tree if isinstance(part, (tuple, list))))
    return set()


def callables_in(tree) -> set:
    """The functions written in a tree (their local ids)."""
    if isinstance(tree, tuple) and len(tree) == 2 and tree[0] == "callable" and isinstance(tree[1], int):
        return {tree[1]}
    if isinstance(tree, (tuple, list)):
        return set().union(*(callables_in(part) for part in tree if isinstance(part, (tuple, list))))
    return set()


def closed(t, unit, local_id: int):
    """The function written in `unit` with local id `local_id`."""
    return t.ix.units[t.ix.offset[unit.file] + local_id]


def written_on(unit, name: str) -> list:
    """What `unit` writes on the properties of a local holding a function it writes (`fn.tag = x`): part of the
    closure it makes (C4'); nothing for any other local."""
    if not any(tree[0] == "callable" for tree in unit.types.get(name, ())) and name not in unit.lambdas:
        return []
    return [r.args[0] for r in unit.refs if r.kind == "write" and r.base == ("name", name) and r.args]


def closure_writes(t, frame, env: int) -> None:
    """The properties a template writes on the closures it makes, each that closure's own (C4'): `fn.tag = x` in a
    template is `tag` of the closure made there."""
    unit = t.ix.units[frame.gid]
    for ref in unit.refs:
        if ref.kind != "write" or not ref.args or ref.base is None or ref.base[0] != "name":
            continue
        name = ref.base[1]
        lids = [tree[1] for tree in unit.types.get(name, ()) if tree[0] == "callable"]
        lids += [unit.lambdas[name]] if name in unit.lambdas else []
        for lid in dict.fromkeys(lids):
            t.follow(t.node_of(ref.args[0], frame), js_transfer.fn_prop_node(closed(t, unit, lid).gid, ref.name, env))


def function_of(text: str) -> str:
    """A function value's function, whatever template made it: its unit and bound arguments (C4')."""
    return text.partition(ENV)[0]


def functions(t, node) -> set:
    """The functions a node's function values are, whatever templates made them."""
    return {function_of(text) for text, _ in t.held(node) if text.startswith(FN)}
