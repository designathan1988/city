"""LOOK, block scope (log/trace-rules.md, "The pending rules found against a real run"): while a function is read, the
blocks it is in. A name declared in a block is local to it, from its declaration to the block's end ([basic.scope.block];
GDScript locals; JavaScript `let`, `const`, `class`, `catch`, loop variables); the same name declared in another block of
the same function is another local and gets its own key (`name#n`). A JavaScript `var` and the parameters belong to the
function's own scope. Names used while a block is open are read as the local they name there: in the trees built
(`scoped_tree`) and in the references made (`visible`).
"""
from __future__ import annotations

from . import narrowing


def reset(r) -> list:
    """A function starts its own scopes (a lambda keeps the enclosing ones: it sees them). Returns the old stack."""
    saved = r.scopes
    r.scopes = [{}]
    r.base = 0
    return saved


def open_scope(r) -> None:
    r.scopes.append({})


def close_scope(r) -> None:
    if len(r.scopes) > 1:
        r.scopes.pop()


def local_key(r, unit, name: str, function_scope: bool = False) -> str:
    """The key a local declared now is kept under: its name, unless the function already has another local of that
    name (declared in another block), then a key of its own. A name the function captures is shadowed in place."""
    if not r.scopes:
        return name
    scope = r.scopes[getattr(r, "base", 0)] if function_scope else r.scopes[-1]   # the function's own scope
    if name in scope:
        return scope[name]                     # declared again in the same scope: the same local (JS `var`)
    key = name
    if name in unit.types and name not in unit.captured:
        number = len(unit.types)
        while f"{name}#{number}" in unit.types:
            number += 1
        key = f"{name}#{number}"
    scope[name] = key
    return key


def visible(r, name: str) -> str:
    """The local a name names where the reader is: the innermost open scope that declares it."""
    for scope in reversed(r.scopes):
        if name in scope:
            return scope[name]
    return name


def scoped_tree(r, tree):
    """A tree built where the reader is: each name read as the local it names (`renamed`), through the facts the open
    regions prove about it (`narrowing`)."""
    return narrowing.apply(r, renamed(r, tree))


def renamed(r, tree):
    """A tree with each name that names a renamed local read as that local."""
    renamed = {}
    for scope in r.scopes:
        renamed.update(scope)
    renamed = {name: key for name, key in renamed.items() if key != name}
    return _rename(tree, renamed) if renamed else tree


def _rename(tree, renamed: dict):
    if not isinstance(tree, tuple) or not tree:
        return tree
    if tree[0] == "name" and len(tree) == 2 and tree[1] in renamed:
        return ("name", renamed[tree[1]])
    return tuple(_rename(t, renamed) if isinstance(t, tuple) else t for t in tree)
