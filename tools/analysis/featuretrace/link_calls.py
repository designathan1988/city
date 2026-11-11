"""How a call reaches what it runs (log/trace-rules.md: CALL, J2'', J6, J6', J11, G7): a value called (the functions
it holds, a bound function after its bound arguments, `operator()` of its classes), the site one function receives
for `call`, `apply` and `bind`, the callables handed to the engine or a library, and a method named by a string.
"""
from __future__ import annotations

from . import js_link, names
from .resolve import EXACT
from .rules import FUNCTION_FORMS


def invoked_leads(rs, unit, tree) -> list[tuple[int, int]]:
    """Calling a value: the functions it can hold, each with the arguments a `bind` put first (J2''), and
    `operator()` of the classes it can be."""
    found = rs.typer.callables(tree, unit)
    for key in rs.typer.receiver(tree, unit)[0]:
        found += [(g, 0) for g in rs.keep(rs.ix.lookup(key, "operator()"), "func", unit.file)]
    return list(dict.fromkeys(found))


class _FormSite:
    """A call site as one function receives it (J2''): the arguments a `bind` put first stand before the written ones
    (they reach the first parameters where the function was bound), `apply`'s array in every parameter, and `this`
    the object written for it."""

    def __init__(self, ref, args: tuple, argc: int, base):
        self.line, self.cond, self.args, self.argc, self.base = ref.line, ref.cond, args, argc, base


def site_of(rs, ref, gid: int, lead: int):
    """The site of a call, `call`, `apply` or `bind` of function `gid` holding `lead` bound arguments."""
    args = ref.args
    if ref.kind == "invoke" and ref.text == "apply":
        args = args[:1] * max(len(rs.ix.units[gid].params) - lead, 0)
    base = ref.receiver if ref.receiver is not None else ref.base
    if not lead and args is ref.args and base is ref.base:
        return ref
    argc = ref.argc + lead if ref.argc >= 0 else -1
    return _FormSite(ref, (None,) * lead + tuple(args), argc, base)


def form_on_functions(rs, unit, ref) -> bool:
    """`f.call(...)`, `f.apply(...)`, `f.bind(...)` on functions of the program alone: Function.prototype's members,
    whose own reference runs or binds f (J2''): nothing is handed to the engine there."""
    if unit.lang != "js" or ref.base is None or ref.name not in FUNCTION_FORMS:
        return False
    keys, engine, _ = rs.typer.receiver(ref.base, unit)
    return not keys and not engine


def handed(rs, unit, ref) -> None:
    """A callable given to a function of the engine or the library is run by it (a comparator, a deferred call, a
    thread's body); given to a function of the program it runs only where the program calls it."""
    site = js_link.Site(ref, js_link.callback_args(unit, ref))   # what the callback receives, when the language says
    for arg in ref.args:
        for gid in rs.typer.receiver(arg, unit)[2]:
            rs.link(unit.gid, gid, "handed", site, EXACT)


def engine_receiver(rs, unit, ref) -> bool:
    """Whether a member call's receiver can be an object of the engine or a library (it runs what it is handed),
    besides any object of the program it can be: in a dynamically typed language (JS, GDScript). In C++ an expression
    of a class of the program is an object of that class ([expr.ref]): the engine values the tracer's receiver also
    holds do not change the method it runs."""
    return unit.lang != "cpp" and ref.kind == "call" and ref.base not in (None, ("super",)) \
        and rs.typer.receiver(ref.base, unit)[1]


def by_value(rs, unit, ref) -> None:
    """`obj.call(name, ...)` with the name held in a value (G7): each text it can hold names a method."""
    for text in names.texts(rs.typer, ref.args[0], unit):
        targets, how = rs.by_string(unit, ref.base if ref.base is not None else ("self",), text)
        for gid in targets:
            rs.link(unit.gid, gid, "name", _Shifted(ref), how)


class _Shifted:
    """A by-name call site: its arguments after the name."""

    def __init__(self, ref):
        self.line, self.cond, self.args = ref.line, ref.cond, ref.args[1:]
