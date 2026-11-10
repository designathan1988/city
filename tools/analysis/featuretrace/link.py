"""CALL, SIGNAL, BIND, ENTRY (log/trace-rules.md): every reference of every unit turned into edges.

A call to a method also reaches the overrides below its class (every GDScript method; a C++ method declared `virtual`
or overriding one; never a qualified or `super` call). A `connect` links the signal to every function its handler tree
can hold; a signal no code declares is the engine's and an entry point. Scene connections link the same way. Names no
code defines are kept per unit as its external uses; members on a receiver no value reaches, as unreached.

Values flow along the links (an argument into a parameter, an assignment into a field) and the links depend on the
values: the program is linked again with the links known so far, and every link found stays (values only grow), until
a pass adds none. The links are a finite set, so the passes end.
"""
from __future__ import annotations

from itertools import count

from .model import FUNCTION_KINDS
from .rules import NAME_CALLS
from .resolve import EXACT, EXTERNAL, LOCAL, UNREACHED, Resolver
from .typer import CLASS

WANT = {"call": "func", "value": "func", "thread": "func", "read": "read", "write": "state", "emit": "signal",
        "operator": "func"}


def build(index, progress=None) -> Resolver:
    previous = None
    for number in count(1):
        rs = Resolver(index, previous)
        for unit in list(index.units):
            for ref in unit.refs:
                link_ref(rs, unit, ref)
        implicit_construction(rs)
        for signal, src_key, dst_key, method, file, line in index.connections:
            handlers = rs.keep(index.lookup(dst_key, method), "func", "")
            signals = rs.keep(index.lookup(src_key, signal), "signal", "") or [index.engine_signal(signal)]
            for s in signals:
                for h in handlers:
                    rs.link(s, h, "signal", _Site(line), EXACT, file)
        known = {key(e) for e in rs.edges}
        for edge in previous.edges if previous is not None else []:
            if key(edge) not in known:
                rs.keep_edge(edge)                 # a link found once stays: the values that made it still flow
        if progress:
            progress(f"link pass {number}: {len(rs.edges)} links")
        if previous is not None and len(rs.edges) == len(previous.edges):
            return rs
        previous = rs


def key(edge) -> tuple:
    return (edge.src, edge.dst, edge.kind, edge.line, edge.file)


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
    if kind == "destroy":
        destructors(rs, unit, ref)
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
        if how == EXTERNAL and kind == "call" and unit.lang == "cpp" and ref.base is None and not ref.qualified:
            targets = rs.adl(unit, ref.name, ref.args)
            how = EXACT if targets else EXTERNAL
    if kind == "call" and ref.name in NAME_CALLS and ref.args:
        by_value(rs, unit, ref)
    if how in (EXTERNAL, LOCAL, UNREACHED):
        if how == EXTERNAL and kind in ("call", "new", "name", "emit"):
            rs.external[unit.gid].add(ref.name)
        elif how == UNREACHED and kind in ("call", "name", "emit", "read", "write"):
            rs.unreached[unit.gid].add(ref.name)
        return
    if kind == "call" and ref.argc >= 0 and len(targets) > 1:
        targets = [g for g in targets if fits(ix.units[g], ref.argc)] or targets
    for gid in targets:
        edge_kind = "value" if kind == "read" and ix.units[gid].kind in FUNCTION_KINDS else \
            "call" if kind == "operator" else kind
        rs.link(unit.gid, gid, edge_kind, ref, how)
        if kind in ("read", "write"):
            accessor(rs, unit, ix.units[gid], kind, ref)
    if kind in ("call", "name") and ref.base != ("super",) and not ref.qualified:
        for gid in with_overrides(rs, targets):
            rs.link(unit.gid, gid, "override", ref, "override")


def by_value(rs, unit, ref) -> None:
    """`obj.call(name, ...)` with the name held in a value (G7): each text it can hold names a method."""
    for text in rs.typer.texts(ref.args[0], unit):
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


def implicit_construction(rs) -> None:
    """A constructor runs its bases' constructors and its object members' constructors (C7)."""
    ix = rs.ix
    for unit in list(ix.units):
        cls = ix.classes.get(unit.owner)
        if cls is None or cls.lang != "cpp" or unit.name != cls.name or unit.decl_only:
            continue
        site = _Site(unit.line)
        for base in ix.base_keys(cls):
            for gid in rs.keep(ix.members[base].get(ix.classes[base].name, []), "func", unit.file):
                rs.link(unit.gid, gid, "call", site, EXACT)
        for gids in ix.members[cls.key].values():
            for member in (ix.units[g] for g in gids):
                key = ix.cpp_class(member.type_text, cls.key, member.file) if member.kind == "field" and                     member.type_text and "*" not in member.type_text and "&" not in member.type_text else None
                for gid in rs.keep(ix.members[key].get(ix.classes[key].name, []), "func", unit.file) if key else []:
                    rs.link(unit.gid, gid, "call", site, EXACT)


def destructors(rs, unit, ref) -> None:
    """An object's destructor at the end of its scope, or `delete` (C7)."""
    ix = rs.ix
    if ref.base is not None:
        keys = rs.typer.receiver(ref.base, unit)[0]
    else:
        key = ix.cpp_class("::".join(p for p in (ref.scope, ref.name) if p), unit.owner, unit.file)
        keys = [key] if key else []
    for key in keys:
        simple = ix.classes[key].name
        for gid in rs.keep(ix.members[key].get("~" + simple, []), "func", unit.file):
            rs.link(unit.gid, gid, "call", ref, EXACT)


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
