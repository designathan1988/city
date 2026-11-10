"""Every reference of every unit turned into edges, construct by construct (the rules are in `resolve`).

A call to a method also reaches the overrides below its class (every GDScript method; a C++ method declared
`virtual` or overriding one). A `connect` makes an edge from the signal to the handler, so an `emit` of that signal
reaches it; a signal no code declares is the engine's, and becomes an entry point. Scene connections are linked the
same way. Names that no code defines (engine and standard-library calls) are kept per unit as its external uses.
"""
from __future__ import annotations

from .model import FUNCTION_KINDS
from .resolve import EXTERNAL, LOCAL, Resolver

WANT = {"call": "func", "value": "func", "thread": "func", "read": "read", "write": "state", "emit": "signal"}


def build(index) -> Resolver:
    rs = Resolver(index)
    for unit in list(index.units):
        for ref in unit.refs:
            link_ref(rs, unit, ref)
    for signal, src_key, dst_key, method, file, line in index.connections:
        handlers = rs.keep(index.lookup(dst_key, method), "func", "")
        signals = rs.keep(index.lookup(src_key, signal), "signal", "") or [index.engine_signal(signal)]
        for s in signals:
            for h in handlers:
                rs.link(s, h, "signal", _Site(line), "exact", file)
    return rs


class _Site:
    """A place a link is written that is no reference of a unit (a scene connection)."""

    def __init__(self, line: int):
        self.line = line
        self.cond = ()


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
        rs.link(unit.gid, ix.offset[unit.file] + ref.target, "lambda", ref, "exact")
        return
    if kind == "load":
        return
    if kind == "name":
        targets, how = rs.by_string(unit, ref.recv, ref.text or ref.name)
    elif kind == "new":
        targets, how = constructors(rs, unit, ref)
    elif kind == "connect":
        connect(rs, unit, ref)
        return
    else:
        targets, how = rs.resolve(unit, ref.name, ref.recv, ref.qualified, WANT[kind])
    if how in (EXTERNAL, LOCAL):
        if how == EXTERNAL and kind in ("call", "new", "name", "emit"):
            rs.external[unit.gid].add(ref.name)
        return
    if kind == "call" and ref.argc >= 0 and len(targets) > 1:
        targets = [g for g in targets if fits(ix.units[g], ref.argc)] or targets
    how_text = how if how != "candidates" else f"candidates ({len(targets)})"
    for gid in targets:
        edge_kind = kind
        if kind == "read" and ix.units[gid].kind in FUNCTION_KINDS:
            edge_kind = "value"
        rs.link(unit.gid, gid, edge_kind, ref, how_text)
    if kind in ("call", "name") and not (ref.recv and ref.recv[0] == "super"):
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
        key = ix.gd_class(".".join(ref.recv + (ref.name,)), unit.owner) if ref.name else None
        found = rs.keep(ix.lookup(key, "_init"), "func", unit.file) if key else []
    else:
        text = "::".join(ref.recv + (ref.name,))
        key = ix.cpp_class(text, unit.owner, unit.file)
        found = rs.keep(ix.members[key].get(ref.name, []), "func", unit.file) if key else []
    return (found, "exact") if found else ([], EXTERNAL)


def handler_targets(rs, unit, spec) -> list[int]:
    ix = rs.ix
    if not spec:
        return []
    if spec[0] == "bare":
        return rs.bare(unit, spec[1], "func")[0]
    if spec[0] == "unit":
        return [ix.offset[unit.file] + spec[1]]
    if spec[0] == "member":
        return rs.member(unit, spec[1], spec[2], "func")[0]
    return []


def connect(rs, unit, ref) -> None:
    ix = rs.ix
    signals, how = rs.resolve(unit, ref.name, ref.recv, False, "signal")
    if not signals:
        signals, how = [ix.engine_signal(ref.name)], "exact"
    elif how == "candidates":
        signals.append(ix.engine_signal(ref.name))   # an untyped receiver may as well be an engine object
    how_text = how if how != "candidates" else f"candidates ({len(signals)})"
    for handler in handler_targets(rs, unit, ref.handler):
        rs.link(unit.gid, handler, "connect", ref, "exact")
        for signal in signals:
            rs.link(signal, handler, "signal", ref, how_text, unit.file)
