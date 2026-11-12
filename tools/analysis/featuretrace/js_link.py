"""JavaScript / TypeScript links (log/trace-rules.md: CALL, ENTRY for JS; J3, J4, J10): `new C` to C's constructor
(inherited up the bases), an imported module's top-level code, JSX components, and what a callback handed to the
language's containers or promises receives.
"""
from __future__ import annotations

from . import js_transfer
from .ir import LITERAL
from .solver import FN, env_of, fn_of
from .rules import (ACCUMULATOR, AWAITED, CONSTRUCTOR, ELEMENTS, ENGINE_VALUE, HIGHER_ORDER)

MODULE_UNIT = "<module>"
RENDER = "render"                  # a class component's method the JSX runtime calls (J10)


class Site:
    """A place a link is written with the values it carries (none, or a callback's arguments)."""

    def __init__(self, ref, args=()):
        self.line, self.cond, self.args = ref.line, ref.cond, tuple(args)


def constructors(rs, unit, ref) -> list[int]:
    """`new C(...)`: C's `constructor`, or the nearest base's (the default constructor calls it, J4); a function
    used with `new` is called as a constructor (N1)."""
    keys, _, functions = rs.typer.receiver(ref.base, unit)
    found = [g for key in keys for g in rs.keep(rs.ix.lookup(key, CONSTRUCTOR), "func", unit.file)]
    return list(dict.fromkeys(found + functions))


def module_run(rs, unit, ref) -> None:
    """`import ... from "m"`, `import("m")`, `require("m")`: m's top-level code runs (J3)."""
    target = rs.ix.js.resolve(unit.file, ref.text)
    for gid in rs.ix.by_qual.get(f"{target}:{MODULE_UNIT}", []) if target else []:
        rs.link(unit.gid, gid, "call", ref, "exact")


def jsx(rs, unit, ref) -> None:
    """A component tag: a function component is handed to the JSX runtime with the props; a class component is
    constructed and its `render` called by it (J10)."""
    keys, _, callables = rs.typer.receiver(ref.base, unit)
    for gid in callables:
        rs.link(unit.gid, gid, "handed", Site(ref, ref.args), "exact")
    for key in keys:
        for gid in rs.keep(rs.ix.lookup(key, CONSTRUCTOR), "func", unit.file):
            rs.link(unit.gid, gid, "new", Site(ref, ref.args), "exact")
        for gid in rs.keep(rs.ix.lookup(key, RENDER), "func", unit.file):
            rs.link(unit.gid, gid, "handed", Site(ref), "exact")


def expando_writes(rs, unit, ref) -> bool:
    """`o.p = v` (V2): for each class of the program the receiver can be that declares no `p`, v is what `p` holds on
    that class's objects (`js_transfer.prop_node`); for each function of the program it can be, what `p` holds on that
    function (`f.prototype = o` among them, N1''). Returns whether the receiver can be a value the program does not
    define: only then is a callable written into it handed to a library (J6, J11). A receiver that holds nothing yet
    decides nothing: the write is linked again when its values arrive, and one no value ever reaches is an unreached
    use, as a call on it is."""
    typer = rs.typer
    keys, engine, functions = typer.receiver(ref.base, unit)
    for key in keys:
        if ref.args and not rs.ix.lookup(key, ref.name):
            typer.follow(typer.node_of(ref.args[0], unit), js_transfer.prop_node(key, ref.name))
    made = {(fn_of(text)[0], env_of(text)) for text, _ in typer.evaluate(ref.base, unit) if text.startswith(FN)} \
        if functions and ref.args else set()
    for gid, env in made:                         # each closure's own (C4')
        typer.follow(typer.node_of(ref.args[0], unit), js_transfer.fn_prop_node(gid, ref.name, env))
    return engine


def callback_args(unit, ref) -> tuple:
    """What a callback handed to an array or promise method receives (J1, J6): the receiver's elements, the
    accumulator, the value awaited; nothing known for anything else."""
    spec = HIGHER_ORDER.get(ref.name) if unit.lang == "js" and ref.base is not None else None
    if spec is None:
        return ()
    values = {ELEMENTS: ("each", ref.base), AWAITED: ("await", ref.base), ENGINE_VALUE: LITERAL}
    out = []
    for part in spec:                     # `each`: the element a container's own iteration hands (a Map's value, T2)
        if part == ACCUMULATOR:
            out.append(("union", ref.args[1], ("each", ref.base)) if len(ref.args) > 1 else ("each", ref.base))
        else:
            out.append(values[part])
    return tuple(out)
