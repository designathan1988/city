"""CALL, SIGNAL, BIND, ENTRY (log/trace-rules.md): every reference of every unit turned into edges.

A call to a method also reaches the overrides below its class (every GDScript method; a C++ method declared `virtual`
or overriding one; never a qualified or `super` call). A `connect` links the signal to every function its handler tree
can hold; a signal no code declares is the engine's and an entry point. Scene connections link the same way. Names no
code defines are kept per unit as its external uses; members on a receiver no value reaches, as unreached.

Values flow along the links (an argument into a parameter, an assignment into a field) and the links depend on the
values: both are nodes of one worklist solver (`typer`). A reference whose values grew is linked again over the
difference only: each grown value node's new values in its place, one read position at a time, the others whole
(semi-naive evaluation); what depends on the whole set (anything found by lookup, an overload that fits) uses what the
reference already found (`Resolver.exact`, `Resolver.fitted`).
"""
from __future__ import annotations

import time

from . import cpp_lifetime, js_link, names, narrow_values
from .cpp_implicit import IMPLICIT
from .cpp_lifetime import Site as _Site
from .model import FUNCTION_KINDS
from .rules import NAME_CALLS
from .resolve import EXACT, EXTERNAL, LOCAL, UNREACHED, Resolver
from .typer import CLASS

REF = "#ref"
PROGRESS_SECONDS = 5.0      # a line of progress this often, however long the work
JS_KINDS = {"import": js_link.module_run, "listen": js_link.listen, "fire": js_link.fire, "jsx": js_link.jsx}
WANT = {"call": "func", "value": "func", "thread": "func", "read": "read", "write": "state", "emit": "signal",
        "operator": "func"}


def build(index, progress=None) -> Resolver:
    """Links every reference once, then computes again only what read something that grew (a value, or the links
    into a unit), until nothing grows: the least fixpoint of links and values together (S1-S3)."""
    rs = Resolver(index)
    typer = rs.typer
    for unit in list(index.units):
        for number in range(len(unit.refs)):
            link_node(rs, (REF, unit.gid, number))
    cpp_lifetime.implicit_construction(rs)
    cpp_lifetime.implicit_destruction(rs)
    for signal, src_key, dst_key, method, file, line in index.connections:
        handlers = rs.keep(index.lookup(dst_key, method), "func", "")
        signals = rs.keep(index.lookup(src_key, signal), "signal", "") or [index.engine_signal(signal)]
        for s in signals:
            for h in handlers:
                rs.link(s, h, "signal", _Site(line), EXACT, file)
    if progress:
        progress(f"linked every reference once: {len(rs.edges)} links; {len(typer.work)} to compute again")
    done, told = 0, time.monotonic()
    while True:
        typer.propagate()                  # every pending difference first: a consumer then reads settled values
        if not typer.work:
            if narrow_values.settle(typer):    # at the fixpoint: a property no value reached is `undefined` (N1)
                continue
            break
        node = typer.work.popleft()
        if node not in typer.dirty:
            continue
        if node[0] == REF:
            link_node(rs, node, typer.take_deltas(node))
        else:
            typer.recompute(node)
        done += 1
        if progress and time.monotonic() - told >= PROGRESS_SECONDS:
            told = time.monotonic()
            progress(f"computed again {done}; {len(typer.work)} waiting; {len(rs.edges)} links")
    rs.settle()
    if progress:
        progress(f"fixpoint: {len(rs.edges)} links, {done} computed again")
    return rs


def link_node(rs, node, deltas: dict | None = None) -> None:
    """Links one reference as a node of the solver: what it reads is recorded, so it is linked again if that grows.
    The first time over every value; after that over what grew: each grown node's new values in its place, at each
    position the reference reads it, the other nodes whole (semi-naive evaluation)."""
    _, gid, number = node
    unit = rs.ix.units[gid]
    typer = rs.typer
    typer.dirty.discard(node)
    typer.stack.append(node)
    try:
        if deltas is None:                  # the first time: every value
            rs.status.pop(node, None)
            link_ref(rs, unit, unit.refs[number])
            return
        if deltas and rs.status.get(node, ("",))[0] == UNREACHED:
            rs.status.pop(node)             # values reached what it reads: "no value reaches it" no longer holds
        for grown, new in deltas.items():
            position = 0
            while True:
                reads = typer.substituted(grown, new, position, lambda: link_ref(rs, unit, unit.refs[number]))
                position += 1
                if position >= reads:
                    break
    finally:
        typer.stack.pop()


def with_overrides(rs, gids: list[int]) -> list[int]:
    ix = rs.ix
    found = []
    for gid in gids:
        unit = ix.units[gid]
        if unit.kind not in FUNCTION_KINDS or unit.owner not in ix.classes:
            continue
        if unit.lang == "cpp" and unit.qual not in ix.virtual_quals:
            continue
        found += ix.overrides(unit.owner, unit.name)    # every override below it, whatever else the call reached
    return list(dict.fromkeys(found))


def link_ref(rs, unit, ref) -> None:
    ix = rs.ix
    kind = ref.kind
    if kind == "lambda":
        rs.link(unit.gid, ix.offset[unit.file] + ref.target, "lambda", ref, EXACT)
        return
    if kind == "load":
        return
    if kind in JS_KINDS:
        JS_KINDS[kind](rs, unit, ref)
        return
    if kind == "connect":
        connect(rs, unit, ref)
        return
    if kind == "destroy":
        cpp_lifetime.destructors(rs, unit, ref)
        return
    if kind == "invoke":
        for gid in invoked(rs, unit, ref.base):
            rs.link(unit.gid, gid, "call", ref, EXACT)
        return
    if kind == "name":
        targets, how = rs.by_string(unit, ref.base, ref.text or ref.name)
    elif kind == "operator":
        targets, how = rs.resolve(unit, ref, "func")
        if not targets and ref.args:
            targets = rs.adl(unit, ref.name, ((ref.base,) if ref.base is not None else ()) + ref.args)
            how = EXACT if targets else how
        if not targets:
            return
    elif kind == "new":
        targets, how = constructors(rs, unit, ref)
    else:
        targets, how = rs.resolve(unit, ref, WANT[kind])
        expando = kind == "write" and unit.lang == "js" and ref.base is not None
        foreign = js_link.expando_writes(rs, unit, ref) if expando else True    # a property no class declares (V2)
        if kind == "write" and unit.lang == "js" and how in (EXTERNAL, UNREACHED) and foreign:
            handed(rs, unit, ref)          # a callable written into a library's object: it calls it (J6, J11)
        if how == EXTERNAL and kind == "call" and unit.lang == "cpp" and ref.base is None and not ref.qualified:
            targets = rs.adl(unit, ref.name, ref.args)
            how = EXACT if targets else EXTERNAL
    if kind == "call" and ref.name in NAME_CALLS and ref.args:
        by_value(rs, unit, ref)
    node = rs.typer.stack[-1]
    if targets and how not in (EXTERNAL, LOCAL, UNREACHED):
        rs.exact.add(node)              # found by lookup: the fallbacks below no longer apply to this reference
    elif not targets and node in rs.exact:
        return
    if kind == "call" and ref.base is None and not ref.qualified and how in (EXTERNAL, LOCAL):
        targets = invoked(rs, unit, ("name", ref.name))      # the name holds a value that is called
        how = EXACT if targets else how
    elif kind == "call" and ref.base is not None and ref.base != ("super",) and how in (EXTERNAL, UNREACHED):
        held = invoked(rs, unit, ("member", ref.base, ref.name, False))   # a member holding a value that is called
        targets, how = (held, EXACT) if held else (targets, how)
    if how == EXTERNAL and kind in ("call", "new", "name"):
        handed(rs, unit, ref)
    if how in (EXTERNAL, LOCAL, UNREACHED):
        if how == EXTERNAL and kind in ("call", "new", "name", "emit") and ref.text != IMPLICIT:
            rs.note(EXTERNAL, ref.name)
        elif how == UNREACHED and kind in ("call", "name", "emit", "read", "write") and ref.text != IMPLICIT:
            rs.note(UNREACHED, ref.name)
        return
    if kind == "call" and ref.argc >= 0 and (len(targets) > 1 or rs.typer.substitution is not None):
        fitting = [g for g in targets if fits(ix.units[g], ref.argc)]
        if fitting:
            rs.fitted.add(node)         # an overload fits the arguments: the others are not called
            targets = fitting
        elif node in rs.fitted:
            targets = []
    for gid in targets:
        edge_kind = "value" if kind == "read" and ix.units[gid].kind in FUNCTION_KINDS else \
            "call" if kind == "operator" else kind
        rs.link(unit.gid, gid, edge_kind, ref, how)
        if kind in ("read", "write"):
            accessor(rs, unit, ix.units[gid], kind, ref)
    if kind in ("call", "name") and ref.base != ("super",) and not ref.qualified:
        for gid in with_overrides(rs, targets):
            rs.link(unit.gid, gid, "override", ref, "override")


def invoked(rs, unit, tree) -> list[int]:
    """Calling a value: the functions it can hold, and `operator()` of the classes it can be."""
    keys, _, callables = rs.typer.receiver(tree, unit)
    found = list(callables)
    for key in keys:
        found += rs.keep(rs.ix.lookup(key, "operator()"), "func", unit.file)
    return list(dict.fromkeys(found))


def handed(rs, unit, ref) -> None:
    """A callable given to a function of the engine or the library is run by it (a comparator, a deferred call, a
    thread's body); given to a function of the program it runs only where the program calls it."""
    site = js_link.Site(ref, js_link.callback_args(unit, ref))   # what the callback receives, when the language says
    for arg in ref.args:
        for gid in rs.typer.receiver(arg, unit)[2]:
            rs.link(unit.gid, gid, "handed", site, EXACT)


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


def accessor(rs, unit, prop, kind: str, ref) -> None:
    """A property's getter on a read, its setter on a write (G8), except inside its own accessors."""
    which = "get" if kind == "read" else "set"
    target = prop.accessors.get(which)
    if target is None or (unit.accessor_of == prop.name and unit.owner == prop.owner):
        return
    if isinstance(target, int):
        gids = [rs.ix.offset[prop.file] + target]
    else:
        gids = rs.bare(prop, target, "func")[0]
    for gid in gids:
        rs.link(unit.gid, gid, "call", ref, EXACT)


def fits(unit, argc: int) -> bool:
    """Overload resolution by arity: the call's arguments fill the required parameters and no more than all."""
    if unit.required < 0:
        return True
    return unit.required <= argc and (unit.variadic or argc <= len(unit.params))


def constructors(rs, unit, ref) -> tuple[list[int], str]:
    """`X.new()` reaches `_init`; `new T` / `make_unique<T>` reach T's constructors."""
    ix = rs.ix
    if unit.lang == "js":
        found = js_link.constructors(rs, unit, ref)
        return (found, EXACT) if found else ([], EXTERNAL)
    if unit.lang == "gd":
        keys = [t[len(CLASS):] for t, _ in rs.typer.evaluate(ref.base, unit) if t.startswith(CLASS)] \
            if ref.base is not None else []
        keys += [k for t, c in rs.typer.evaluate(ref.base, unit) if not t.startswith(CLASS)
                 for k in [rs.typer.type_class(t, c)] if k] if ref.base is not None else []
        found = [g for k in dict.fromkeys(keys) for g in rs.keep(ix.lookup(k, "_init"), "func", unit.file)]
    else:
        found = cpp_lifetime.constructors(rs, unit, ref)
    return (found, EXACT) if found else ([], EXTERNAL)


def connect(rs, unit, ref) -> None:
    ix = rs.ix
    signals, how = rs.resolve(unit, ref, "signal")
    if how == EXTERNAL or (how == LOCAL and not signals):
        signals = [ix.engine_signal(ref.name)]     # the receiver is an engine object: the engine emits it
    elif how == UNREACHED:
        rs.note(UNREACHED, ref.name)
        return
    handlers = rs.typer.receiver(ref.handler, unit)[2] if ref.handler is not None else []
    for handler in handlers:
        rs.link(unit.gid, handler, "connect", ref, EXACT)
        for signal in signals:
            rs.link(signal, handler, "signal", ref, EXACT, unit.file)
