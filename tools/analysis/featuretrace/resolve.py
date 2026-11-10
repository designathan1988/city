"""Name resolution: every reference linked to the definitions it reaches, by the rule of its construct.

- a plain name: the local scope, then the class and its bases, then outer classes or enclosing namespaces,
  `using namespace`, globals (class_name, autoloads); an internal-linkage C++ name only inside its file;
- a member `x.f`: the class of `x` (see `typer`), then its bases; a virtual call also reaches every override below;
- a qualified `A::f`: the scope named, seen from the enclosing scopes;
- a name carried as a value (`&"f"`, `call("f")`, `Callable(o, "f")`, a bound name): the definitions with that name
  that the engine can call (script methods and signals, C++ methods bound with `D_METHOD`);
- a signal: `emit` reaches the signal, the signal reaches every handler `connect`ed to it (in code or in a scene).
When the receiver's class is declared nowhere, every definition of the name is a candidate, and the edge says so.
"""
from __future__ import annotations

from collections import defaultdict
from dataclasses import dataclass

from .model import FUNCTION_KINDS, STATE_KINDS
from .typer import Typer

SIGNAL_KINDS = ("signal", "engine_signal")
EXACT, CANDIDATES, LOCAL, EXTERNAL = "exact", "candidates", "local", "external"


@dataclass
class Edge:
    src: int
    dst: int
    kind: str          # call, value, name, emit, signal, connect, lambda, new, override, thread, read, write
    line: int
    cond: tuple
    how: str = EXACT   # exact | candidates (n) | by name
    file: str = ""     # where the link is written (the source unit's file unless a scene or a connect)


class Resolver:
    def __init__(self, index):
        self.ix = index
        self.typer = Typer(index, self)
        self.edges: list[Edge] = []
        self.out = defaultdict(list)
        self.inn = defaultdict(list)
        self.external = defaultdict(set)   # unit gid -> names it uses that no code defines (engine, std)

    # ---- filters -----------------------------------------------------------------------------------------------
    def keep(self, gids, want: str, file: str) -> list[int]:
        units = self.ix.units
        kinds = {"func": FUNCTION_KINDS, "state": STATE_KINDS | {"enumerator"}, "signal": SIGNAL_KINDS,
                 "read": STATE_KINDS | {"enumerator"} | FUNCTION_KINDS}[want]
        found = [g for g in gids if units[g].kind in kinds and (not units[g].file_local or units[g].file == file)]
        return self.ix.definitions(found) if want == "func" else found

    # ---- the rules -----------------------------------------------------------------------------------------------
    def bare(self, unit, name: str, want: str) -> tuple[list[int], str]:
        if name in unit.lambdas:
            return [self.ix.offset[unit.file] + unit.lambdas[name]], EXACT
        if name in unit.types:
            return [], LOCAL
        key = self.typer.class_of(unit)
        scope = key
        while scope:
            found = self.keep(self.ix.lookup(scope, name), want, unit.file)
            if found:
                return found, EXACT
            cls = self.ix.classes.get(scope)
            scope = cls.outer if cls is not None and cls.lang == "gd" else ""
        if unit.lang == "gd":
            cls_key = self.ix.gd_class(name, unit.owner)
            if cls_key and want == "func":
                return self.keep(self.ix.lookup(cls_key, "_init"), want, unit.file), EXACT
            return [], EXTERNAL
        return self.cpp_scoped(unit, name, want)

    def cpp_scoped(self, unit, name: str, want: str) -> tuple[list[int], str]:
        ix = self.ix
        for scope in ix.scopes(unit.owner) + ix.usings.get(unit.file, []):
            qual = f"{scope}::{name}" if scope else name
            found = self.keep(ix.by_qual.get(qual, []), want, unit.file)
            if not found and qual in ix.aliases:
                found = self.keep(ix.by_qual.get(ix.aliases[qual], []), want, unit.file)
            if found:
                return found, EXACT
        cls = ix.cpp_class(name, unit.owner, unit.file)
        if cls and want == "func":
            return self.keep(ix.members[cls].get(name.split("::")[-1], []), want, unit.file), EXACT
        free = [g for g in ix.by_name.get(name, []) if ix.units[g].owner not in ix.classes]
        free = self.keep(free, want, unit.file)
        return (free, CANDIDATES) if free else ([], EXTERNAL)

    def qualified(self, unit, recv: tuple, name: str, want: str) -> tuple[list[int], str]:
        ix = self.ix
        scope_text = "::".join(p for p in recv if p)
        for scope in ix.scopes(unit.owner) + ix.usings.get(unit.file, []):
            qual = "::".join(p for p in (scope, scope_text, name) if p)
            found = self.keep(ix.by_qual.get(qual, []), want, unit.file)
            if found:
                return found, EXACT
        cls = ix.cpp_class(scope_text, unit.owner, unit.file)
        if cls:
            found = self.keep(ix.lookup(cls, name), want, unit.file)
            if found:
                return found, EXACT
        suffix = f"::{scope_text}::{name}"
        found = [g for g in ix.by_name.get(name, []) if ix.units[g].qual.endswith(suffix)]
        found = self.keep(found, want, unit.file)
        return (found, CANDIDATES if len(found) > 1 else EXACT) if found else ([], EXTERNAL)

    def member(self, unit, recv: tuple, name: str, want: str) -> tuple[list[int], str]:
        ix = self.ix
        if recv and recv[0] == "super":
            key = self.typer.class_of(unit)
            bases = ix.base_keys(ix.classes[key]) if key else []
            found = self.keep(ix.lookup(bases[0], name), want, unit.file) if bases else []
            return (found, EXACT) if found else ([], EXTERNAL)
        key, engine_type = self.typer.receiver(recv, unit)
        if key:
            cls = ix.classes[key]
            if unit.lang == "gd" and cls.lang == "cpp":
                found = self.engine_members(key, name, want)
            else:
                found = self.keep(ix.lookup(key, name), want, unit.file)
            return (found, EXACT) if found else ([], EXTERNAL)
        if engine_type:
            # the receiver's type is declared and is the engine's or the library's: so is the member
            return [], EXTERNAL
        return self.by_name(unit, name, want)

    def engine_members(self, key: str, name: str, want: str) -> list[int]:
        """What GDScript reaches on a C++ object: its bound methods, signals and property accessors (with bases)."""
        ix = self.ix
        found, todo, seen = [], [key], set()
        while todo:
            k = todo.pop()
            if k in seen:
                continue
            seen.add(k)
            found += ix.bound_in[k].get(name, [])
            found += [g for g in ix.members[k].get(name, []) if ix.units[g].kind == "signal"]
            for owner, setter, getter in ix.props.get(name, []):
                if owner == k:
                    found += ix.bound_in[k].get(getter if want == "read" else setter, [])
            todo += ix.base_keys(ix.classes[k]) if k in ix.classes else []
        return [g for g in found if want != "signal" or ix.units[g].kind in SIGNAL_KINDS]

    def by_name(self, unit, name: str, want: str) -> tuple[list[int], str]:
        """No class declared for the receiver: every definition the construct can reach by this name."""
        ix = self.ix
        if unit.lang == "gd":
            pool = [g for g in ix.by_name.get(name, []) if ix.units[g].lang == "gd"]
            pool = self.keep(pool, want, unit.file)
            if want in ("func", "read"):
                pool += ix.bound.get(name, [])
            if want in ("read", "state"):
                pool += [g for _, s, gt in ix.props.get(name, []) for g in ix.bound.get(gt if want == "read" else s, [])]
            if want == "signal":
                pool += [g for g in ix.by_name.get(name, []) if ix.units[g].lang == "cpp" and ix.units[g].kind == "signal"]
        else:
            pool = [g for g in ix.by_name.get(name, []) if ix.units[g].lang == "cpp" and ix.units[g].owner in ix.classes]
            pool = self.keep(pool, want, unit.file)
        pool = list(dict.fromkeys(pool))
        if not pool:
            return [], EXTERNAL
        return pool, (EXACT if len(pool) == 1 else CANDIDATES)

    def by_string(self, unit, recv: tuple, text: str) -> tuple[list[int], str]:
        """A name written as a string the engine resolves at run time: script methods and signals, bound methods."""
        ix = self.ix
        if recv and recv != ("self",):
            key, engine_type = self.typer.receiver(recv, unit)
            if key:
                return self.member(unit, recv, text, "func")
            if engine_type == "Callable":
                return [], EXTERNAL     # Callable.call(args) runs the callable itself: the string is an argument
        if recv == ("self",) and unit.lang == "gd":
            found, how = self.member(unit, recv, text, "func")
            if found:
                return found, how
        pool = [g for g in ix.by_name.get(text, []) if ix.units[g].lang == "gd"
                and ix.units[g].kind in FUNCTION_KINDS | {"signal"}] + ix.bound.get(text, [])
        pool = list(dict.fromkeys(pool))
        return (pool, "by name") if pool else ([], EXTERNAL)

    # ---- entry for typer -------------------------------------------------------------------------------------------
    def function_targets(self, unit, text: str) -> list[int]:
        if "::" in text:
            parts = text.split("::")
            return self.qualified(unit, tuple(parts[:-1]), parts[-1], "func")[0]
        return self.bare(unit, text, "func")[0]

    def state_targets(self, unit, name: str) -> list[int]:
        if "::" in name:
            parts = name.split("::")
            return self.qualified(unit, tuple(parts[:-1]), parts[-1], "state")[0]
        found, _ = self.bare(unit, name, "state")
        return found

    def resolve(self, unit, name: str, recv: tuple, qualified: bool, want: str) -> tuple[list[int], str]:
        if qualified:
            return self.qualified(unit, recv, name, want)
        if recv:
            return self.member(unit, recv, name, want)
        return self.bare(unit, name, want)

    def link(self, src, dst, kind, ref, how, file="") -> None:
        edge = Edge(src, dst, kind, ref.line, ref.cond, how, file or self.ix.units[src].file)
        self.edges.append(edge)
        self.out[src].append(edge)
        self.inn[dst].append(edge)
