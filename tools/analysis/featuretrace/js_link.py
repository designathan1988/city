"""JavaScript / TypeScript links (log/trace-rules.md: CALL, SIGNAL, ENTRY for JS; J3, J4, J6, J10): `new C` to C's
constructor (inherited up the bases), an imported module's top-level code, events by type (a subscription links the
handler to the event, firing links to it), JSX components, and what a callback handed to the language's containers or
promises receives.
"""
from __future__ import annotations

from . import names
from .ir import LITERAL
from .rules import (ACCUMULATOR, AWAITED, CONSTRUCTOR, ELEMENTS, ENGINE_VALUE, HIGHER_ORDER)

MODULE_UNIT = "<module>"
HANDLE_EVENT = "handleEvent"       # an object listener's method (EventTarget, J6)
RENDER = "render"                  # a class component's method the JSX runtime calls (J10)


class Site:
    """A place a link is written with the values it carries (none, or a callback's arguments)."""

    def __init__(self, ref, args=()):
        self.line, self.cond, self.args = ref.line, ref.cond, tuple(args)


def classes_of(rs, tree, unit) -> list[str]:
    keys = []
    for text, ctx in rs.typer.evaluate(tree, unit):
        key = rs.typer.type_class(text, ctx)
        if key and key not in keys:
            keys.append(key)
    return keys


def constructors(rs, unit, ref) -> list[int]:
    """`new C(...)`: C's `constructor`, or the nearest base's (the default constructor calls it, J4)."""
    return [g for key in classes_of(rs, ref.base, unit) for g in rs.keep(rs.ix.lookup(key, CONSTRUCTOR), "func", unit.file)]


def module_run(rs, unit, ref) -> None:
    """`import ... from "m"`, `import("m")`, `require("m")`: m's top-level code runs (J3)."""
    target = rs.ix.js.resolve(unit.file, ref.text)
    for gid in rs.ix.by_qual.get(f"{target}:{MODULE_UNIT}", []) if target else []:
        rs.link(unit.gid, gid, "call", ref, "exact")


def handlers(rs, unit, tree) -> list[int]:
    """The functions a listener value can be: a callable, or an object's `handleEvent` (J6)."""
    keys, _, callables = rs.typer.receiver(tree, unit)
    found = list(callables)
    for key in keys:
        found += rs.keep(rs.ix.lookup(key, HANDLE_EVENT), "func", unit.file)
    return list(dict.fromkeys(found))


def event_types(rs, unit, name: str, tree) -> list[str]:
    if name:
        return [name]
    return names.texts(rs.typer, tree, unit) if tree is not None else []


def listen(rs, unit, ref) -> None:
    """A subscription: the handler is connected to the event of each type the call can name."""
    for kind in event_types(rs, unit, ref.name, ref.args[0] if ref.args else None):
        signal = rs.ix.engine_signal(kind)
        for handler in handlers(rs, unit, ref.handler):
            rs.link(unit.gid, handler, "connect", ref, "exact")
            rs.link(signal, handler, "signal", ref, "exact", unit.file)


def fire(rs, unit, ref) -> None:
    """Firing an event: the event of each type the call can name, with the values it carries."""
    for kind in event_types(rs, unit, ref.name, ref.handler):
        rs.link(unit.gid, rs.ix.engine_signal(kind), "emit", ref, "exact")


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


def callback_args(unit, ref) -> tuple:
    """What a callback handed to an array or promise method receives (J1, J6): the receiver's elements, the
    accumulator, the value awaited; nothing known for anything else."""
    spec = HIGHER_ORDER.get(ref.name) if unit.lang == "js" and ref.base is not None else None
    if spec is None:
        return ()
    values = {ELEMENTS: ("elements", ref.base), AWAITED: ("await", ref.base), ENGINE_VALUE: LITERAL}
    out = []
    for part in spec:
        if part == ACCUMULATOR:
            out.append(("union", ref.args[1], ("elements", ref.base)) if len(ref.args) > 1 else ("elements", ref.base))
        else:
            out.append(values[part])
    return tuple(out)
