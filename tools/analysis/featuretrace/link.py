"""CALL, SIGNAL, BIND, ENTRY (log/trace-rules.md): every reference of every unit turned into edges.

A call to a method also reaches the overrides below its class (every GDScript method; a C++ method declared `virtual`
or overriding one; never a qualified or `super` call). A `connect` links the signal to every function its handler tree
can hold; a signal no code declares is the engine's and an entry point. Scene connections link the same way. Names no
code defines are kept per unit as its external uses; members on a receiver no value reaches, as unreached.

Values flow along the links (an argument into a parameter, an assignment into a field) and the links depend on the
values: the program is linked again with the previous pass's values until a pass changes no link.
"""
from __future__ import annotations

from .model import FUNCTION_KINDS
from .resolve import EXACT, EXTERNAL, LOCAL, UNREACHED, Resolver
from .typer import CLASS

WANT = {"call": "func", "value": "func", "thread": "func", "read": "read", "write": "state", "emit": "signal"}
MAX_PASSES = 8


def build(index, progress=None) -> Resolver:
    previous = None
    for number in range(1, MAX_PASSES + 1):
        rs = Resolver(index, previous)
        for unit in list(index.units):
            for ref in unit.refs:
                link_ref(rs, unit, ref)
        for signal, src_key, dst_key, method, file, line in index.connections:
            handlers = rs.keep(index.lookup(dst_key, method), "func", "")
            signals = rs.keep(index.lookup(src_key, signal), "signal", "") or [index.engine_signal(signal)]
            for s in signals:
                for h in handlers:
                    rs.link(s, h, "signal", _Site(line), EXACT, file)
        if progress:
            progress(f"link pass {number}: {len(rs.edges)} links")
        if previous is not None and edge_set(rs) == edge_set(previous):
            return rs
        previous = rs
    return rs


def edge_set(rs) -> set:
    return {(e.src, e.dst, e.kind, e.line) for e in rs.edges}


class _Site:
    """A place a link is written that is no reference of a unit (a scene connection)."""

    def __init__(self, line: int):
        self.line = line
        self.cond = ()
        self.args = ()


def with_overrides(rs, gids: list[int]) -> list[int]:
    ix = rs.ix
    found = []
    for gid in gids:
        unit = ix.units[gid]
        if unit.kind not in FUNCTION_KINDS or unit.owner not in ix.classes:
            continue
        if unit.lang == "cpp" and unit.qual not in ix.virtual_quals:
            continue
        found += [g for g in ix.overrides(unit.owner, unit.name) if g not in gids]
    return list(dict.fromkeys(found))


def link_ref(rs, unit, ref) -> None:
    ix = rs.ix
    kind = ref.kind
    if kind == "lambda":
        rs.link(unit.gid, ix.offset[unit.file] + ref.target, "lambda", ref, EXACT)
        return
    if kind == "load":
        return
    if kind == "connect":
        connect(rs, unit, ref)
        return
    if kind == "name":
        targets, how = rs.by_string(unit, ref.base, ref.text or ref.name)
    elif kind == "new":
        targets, how = constructors(rs, unit, ref)
    else:
        targets, how = rs.resolve(unit, ref, WANT[kind])
        if how == EXTERNAL and kind == "call" and unit.lang == "cpp" and ref.base is None and not ref.qualified:
            targets = rs.adl(unit, ref.name, ref.args)
            how = EXACT if targets else EXTERNAL
    if how in (EXTERNAL, LOCAL, UNREACHED):
        if how == EXTERNAL and kind in ("call", "new", "name", "emit"):
            rs.external[unit.gid].add(ref.name)
        elif how == UNREACHED and kind in ("call", "name", "emit", "read", "write"):
            rs.unreached[unit.gid].add(ref.name)
        return
    if kind == "call" and ref.argc >= 0 and len(targets) > 1:
        targets = [g for g in targets if fits(ix.units[g], ref.argc)] or targets
    for gid in targets:
        edge_kind = "value" if kind == "read" and ix.units[gid].kind in FUNCTION_KINDS else kind
        rs.link(unit.gid, gid, edge_kind, ref, how)
    if kind in ("call", "name") and ref.base != ("super",) and not ref.qualified:
        for gid in with_overrides(rs, targets):
            rs.link(unit.gid, gid, "override", ref, "override")


def fits(unit, argc: int) -> bool:
    """Overload resolution by arity: the call's arguments fill the required parameters and no more than all."""
    if unit.required < 0:
        return True
    return unit.required <= argc and (unit.variadic or argc <= len(unit.params))


def constructors(rs, unit, ref) -> tuple[list[int], str]:
    """`X.new()` reaches `_init`; `new T` / `make_unique<T>` reach T's constructors."""
    ix = rs.ix
    if unit.lang == "gd":
        keys = [t[len(CLASS):] for t, _ in rs.typer.evaluate(ref.base, unit) if t.startswith(CLASS)] \
            if ref.base is not None else []
        keys += [k for t, c in rs.typer.evaluate(ref.base, unit) if not t.startswith(CLASS)
                 for k in [rs.typer.type_class(t, c)] if k] if ref.base is not None else []
        found = [g for k in dict.fromkeys(keys) for g in rs.keep(ix.lookup(k, "_init"), "func", unit.file)]
    else:
        key = ix.cpp_class("::".join(p for p in (ref.scope, ref.name) if p), unit.owner, unit.file)
        found = rs.keep(ix.members[key].get(ref.name, []), "func", unit.file) if key else []
    return (found, EXACT) if found else ([], EXTERNAL)


def connect(rs, unit, ref) -> None:
    ix = rs.ix
    signals, how = rs.resolve(unit, ref, "signal")
    if how == EXTERNAL or (how == LOCAL and not signals):
        signals = [ix.engine_signal(ref.name)]     # the receiver is an engine object: the engine emits it
    elif how == UNREACHED:
        rs.unreached[unit.gid].add(ref.name)
        return
    handlers = rs.typer.receiver(ref.handler, unit)[2] if ref.handler is not None else []
    for handler in handlers:
        rs.link(unit.gid, handler, "connect", ref, EXACT)
        for signal in signals:
            rs.link(signal, handler, "signal", ref, EXACT, unit.file)
