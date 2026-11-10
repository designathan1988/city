"""Name resolution: every reference linked to the definitions it reaches, by the rule of its construct.

- a plain name: the local scope (a local holding a callable reaches that function), then the class and its bases,
  outer classes or enclosing namespaces, `using namespace`, globals (class_name, autoloads); a C++ name with internal
  linkage only inside the translation unit that declares it (its file and the files it `#include`s);
- a member `x.f`: the classes of every value that flows into `x` (`typer`), then their bases; a virtual call also
  reaches every override below; `c.call()` on a callable reaches the function it holds;
- a qualified `A::f`: the scope named, seen from the enclosing scopes and the translation unit;
- a name carried as a string (`&"f"`, `call("f")`, `Callable(o, "f")`, a bound name): what the engine calls by it;
- a signal: `emit` reaches the signal, the signal reaches every handler `connect`ed to it (in code or in a scene).
A parameter holds the arguments written at every call site, a field what every assignment puts in it: the values come
from the links of the previous pass (`link.build` repeats until no link is added).
"""
from __future__ import annotations

from collections import defaultdict
from dataclasses import dataclass, field

from .model import ENGINE_CALLBACKS, FUNCTION_KINDS, STATE_KINDS
from .typer import Typer

SIGNAL_KINDS = ("signal", "engine_signal")
CALLABLE_CALLS = ("call", "callv", "call_deferred")
CALLING = ("call", "override", "name", "thread")
EXACT, LOCAL, EXTERNAL, UNREACHED = "exact", "local", "external", "unreached"


@dataclass
class Edge:
    src: int
    dst: int
    kind: str          # call, value, name, emit, signal, connect, lambda, new, override, thread, read, write
    line: int
    cond: tuple
    how: str = EXACT   # exact | by name | override
    file: str = ""     # where the link is written (the source unit's file unless a scene or a connect)
    args: tuple = field(default_factory=tuple)   # the value markers written at the site (arguments, assigned value)


class Resolver:
    def __init__(self, index, previous=None):
        self.ix = index
        self.typer = Typer(index, self)
        self.edges: list[Edge] = []
        self.out = defaultdict(list)
        self.inn = defaultdict(list)
        self.flows = previous.inn if previous is not None else defaultdict(list)
        self.external = defaultdict(set)    # unit gid -> names it uses that no code defines (engine, library)
        self.unreached = defaultdict(set)   # unit gid -> members called on a receiver no value of the code reaches

    # ---- filters -----------------------------------------------------------------------------------------------
    def keep(self, gids, want: str, file: str) -> list[int]:
        units, visible = self.ix.units, self.ix.visible(file) if file else set()
        kinds = {"func": FUNCTION_KINDS, "state": STATE_KINDS | {"enumerator"}, "signal": SIGNAL_KINDS,
                 "read": STATE_KINDS | {"enumerator"} | FUNCTION_KINDS}[want]
        found = [g for g in gids if units[g].kind in kinds and (not units[g].file_local or units[g].file in visible)]
        return self.ix.definitions(found) if want == "func" else found

    # ---- the rules ---------------------------------------------------------------------------------------------
    def bare(self, unit, name: str, want: str) -> tuple[list[int], str]:
        if name in unit.lambdas:
            return [self.ix.offset[unit.file] + unit.lambdas[name]], EXACT
        if name in unit.types:
            if want == "func":
                held = self.typer.receiver((name,), unit)[2]
                return (held, EXACT) if held else ([], LOCAL)
            return [], LOCAL
        scope = self.typer.class_of(unit)
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
        # a free name declared in the translation unit (the file and what it includes), in any namespace it opens
        visible = ix.visible(unit.file)
        free = [g for g in ix.by_name.get(name, []) if ix.units[g].owner not in ix.classes
                and any(ix.units[d].file in visible for d in ix.by_qual[ix.units[g].qual])]
        free = self.keep(free, want, unit.file)
        return (free, EXACT) if free else ([], EXTERNAL)

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
        visible = ix.visible(unit.file)
        suffix = f"::{scope_text}::{name}"
        found = [g for g in ix.by_name.get(name, []) if ix.units[g].qual.endswith(suffix)
                 and any(ix.units[d].file in visible for d in ix.by_qual[ix.units[g].qual])]
        found = self.keep(found, want, unit.file)
        return (found, EXACT) if found else ([], EXTERNAL)

    def member(self, unit, recv: tuple, name: str, want: str) -> tuple[list[int], str]:
        ix = self.ix
        if recv and recv[0] == "super":
            key = self.typer.class_of(unit)
            bases = ix.base_keys(ix.classes[key]) if key else []
            found = self.keep(ix.lookup(bases[0], name), want, unit.file) if bases else []
            return (found, EXACT) if found else ([], EXTERNAL)
        keys, engine, callables = self.typer.receiver(recv, unit)
        if name in CALLABLE_CALLS and callables and want == "func":
            return callables, EXACT
        found = []
        for key in keys:
            if unit.lang == "gd" and ix.classes[key].lang == "cpp":
                found += self.engine_members(key, name, want)
            else:
                found += self.keep(ix.lookup(key, name), want, unit.file)
        found = list(dict.fromkeys(found))
        if found:
            return found, EXACT
        return ([], EXTERNAL) if keys or engine or callables else ([], UNREACHED)

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

    def by_string(self, unit, recv: tuple, text: str) -> tuple[list[int], str]:
        """A name written as a string the engine resolves at run time: on the receiver's classes when there is one,
        else every script method or signal and every bound method with that name (StringName handed over)."""
        ix = self.ix
        if recv:
            keys, engine, callables = self.typer.receiver(recv, unit)
            if callables:
                return callables, EXACT        # Callable.call("x", ...): the callable runs, "x" is an argument
            if keys:
                return self.member(unit, recv, text, "func")
            if engine:
                return [], EXTERNAL
        pool = [g for g in ix.by_name.get(text, []) if ix.units[g].lang == "gd"
                and ix.units[g].kind in FUNCTION_KINDS | {"signal"}] + ix.bound.get(text, [])
        pool = list(dict.fromkeys(pool))
        return (pool, "by name") if pool else ([], EXTERNAL)

    # ---- values for the typer ----------------------------------------------------------------------------------
    def argument_values(self, unit, index: int, depth: int) -> list[tuple]:
        """A parameter's values: the argument at that position at every site that calls the function; the engine's
        values when the engine calls it (a callback, a handler of an engine signal, a lambda the library runs)."""
        ix, out = self.ix, []
        incoming = self.flows.get(unit.gid, [])
        if unit.name in ENGINE_CALLBACKS:
            out.append(("<engine>", unit))
        for edge in incoming:
            if edge.kind in CALLING and index < len(edge.args):
                out += self.typer.settle(edge.args[index], ix.units[edge.src], depth + 1)
            elif edge.kind == "signal":
                signal = ix.units[edge.src]
                if signal.kind == "engine_signal":
                    out.append(("<engine>", unit))
                for emit in self.flows.get(signal.gid, []):
                    if emit.kind == "emit" and index < len(emit.args):
                        out += self.typer.settle(emit.args[index], ix.units[emit.src], depth + 1)
        if unit.kind == "lambda" and not any(e.kind in CALLING for e in incoming):
            out.append(("<engine>", unit))     # handed to the library or the engine, which calls it
        return out

    def assigned_values(self, unit, depth: int) -> list[tuple]:
        """A variable's or field's values: what every assignment anywhere in the code puts into it."""
        return [v for e in self.flows.get(unit.gid, []) if e.kind == "write" and e.args
                for v in self.typer.settle(e.args[0], self.ix.units[e.src], depth + 1)]

    def function_targets(self, unit, text: str) -> list[int]:
        if "::" in text:
            parts = text.split("::")
            return self.qualified(unit, tuple(parts[:-1]), parts[-1], "func")[0]
        return self.bare(unit, text, "func")[0]

    def state_targets(self, unit, name: str) -> list[int]:
        if "::" in name:
            parts = name.split("::")
            return self.qualified(unit, tuple(parts[:-1]), parts[-1], "state")[0]
        return self.bare(unit, name, "state")[0]

    def resolve(self, unit, name: str, recv: tuple, qualified: bool, want: str) -> tuple[list[int], str]:
        if qualified:
            return self.qualified(unit, recv, name, want)
        if recv:
            return self.member(unit, recv, name, want)
        return self.bare(unit, name, want)

    def link(self, src, dst, kind, ref, how, file="") -> None:
        edge = Edge(src, dst, kind, ref.line, ref.cond, how, file or self.ix.units[src].file, getattr(ref, "args", ()))
        self.edges.append(edge)
        self.out[src].append(edge)
        self.inn[dst].append(edge)
