"""LOOK, CALL, NAME (log/trace-rules.md): every reference linked to the definitions it reaches.

- a plain name: the local scope (a local holding a callable reaches that function), then the class and its bases,
  outer classes or enclosing namespaces, `using namespace`, globals; a C++ name with internal linkage only inside the
  translation unit that declares it (its file and the files it includes); C++ argument-dependent lookup adds the
  namespaces of the arguments' classes;
- a member `x.f`: the classes of every value that flows into `x` (`typer`), then their bases; `c.call()` on a callable
  reaches the function it holds; a receiver no value of the code reaches is recorded as such (a fact, no guess);
- a qualified `A::f`: the scope named, seen from the enclosing scopes and the translation unit;
- a name carried as a string: on the receiver's classes, or (handed to a call) every script method or signal and every
  bound method with that name — what the engine resolves by it.
A parameter holds the arguments written at every call site, a field what every assignment puts in it: the values come
from the links found so far; a reader of them is computed again when one more arrives (`typer`, `link.build`).
"""
from __future__ import annotations

from collections import defaultdict
from dataclasses import dataclass, field

from .model import ENGINE_CALLBACKS, FUNCTION_KINDS, STATE_KINDS
from .typer import CALLABLE_CALLS, ENGINE, Typer

SIGNAL_KINDS = ("signal", "engine_signal")
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
    args: tuple = field(default_factory=tuple)   # the trees written at the site (arguments, the value written)


class Resolver:
    def __init__(self, index):
        self.ix = index
        self.typer = Typer(index, self)
        self.edges: list[Edge] = []
        self.known = set()                  # the links found, by (src, dst, kind, line, file)
        self.out = defaultdict(list)
        self.inn = defaultdict(list)
        self.status = {}                    # reference node -> ("external" | "unreached", name), its last outcome
        self.external = defaultdict(set)    # unit gid -> names it uses that no code defines (engine, library)
        self.unreached = defaultdict(set)   # unit gid -> members used on a receiver no value of the code reaches

    def incoming(self, gid: int) -> list:
        """The links into a unit; the node being computed is computed again when one more arrives."""
        self.typer.read(("#in", gid))
        return list(self.inn.get(gid, ()))

    def keep(self, gids, want: str, file: str) -> list[int]:
        units, visible = self.ix.units, self.ix.visible(file) if file else set()
        # a macro use is a reference to the macro (`#define`, C1 preprocessor), as a call or as a value
        kinds = {"func": FUNCTION_KINDS | {"macro"}, "state": STATE_KINDS | {"enumerator"}, "signal": SIGNAL_KINDS,
                 "read": STATE_KINDS | {"enumerator", "macro"} | FUNCTION_KINDS}[want]
        found = [g for g in gids if units[g].kind in kinds and (not units[g].file_local or units[g].file in visible)]
        return self.ix.definitions(found) if want == "func" else found

    # ---- the rules ---------------------------------------------------------------------------------------------
    def bare(self, unit, name: str, want: str) -> tuple[list[int], str]:
        if name in unit.lambdas:
            return [self.ix.offset[unit.file] + unit.lambdas[name]], EXACT
        if name in unit.types:
            held = self.typer.receiver(("name", name), unit)[2] if want == "func" else []
            return (held, EXACT) if held else ([], LOCAL)
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
        for scope in ix.scopes(unit.owner) + ix.tu_usings(unit.file):
            qual = f"{scope}::{name}" if scope else name
            found = self.keep(ix.by_qual.get(qual, []), want, unit.file)
            if not found and qual in ix.aliases:
                found = self.keep(ix.by_qual.get(ix.aliases[qual], []), want, unit.file)
            if found:
                return found, EXACT
        cls = ix.cpp_class(name, unit.owner, unit.file)
        if cls and want == "func":
            return self.keep(ix.members[cls].get(name.split("::")[-1], []), want, unit.file), EXACT
        return [], EXTERNAL

    def qualified(self, unit, scope_text: str, name: str, want: str) -> tuple[list[int], str]:
        ix = self.ix
        for scope in ix.scopes(unit.owner) + ix.tu_usings(unit.file):
            qual = "::".join(p for p in (scope, scope_text, name) if p)
            found = self.keep(ix.by_qual.get(qual, []), want, unit.file)
            if found:
                return found, EXACT
        cls = ix.cpp_class(scope_text, unit.owner, unit.file)
        if cls:
            found = self.keep(ix.lookup(cls, name), want, unit.file)
            if found:
                return found, EXACT
        return [], EXTERNAL

    def adl(self, unit, name: str, args: tuple) -> list[int]:
        """Argument-dependent lookup: `f(x)` also looks in the namespaces of the classes of its arguments (C3)."""
        found = []
        for arg in args:
            for key in self.typer.receiver(arg, unit)[0]:
                namespace = key.rpartition("::")[0]
                while namespace and namespace in self.ix.classes:
                    namespace = namespace.rpartition("::")[0]
                found += self.keep(self.ix.by_qual.get(f"{namespace}::{name}" if namespace else name, []), "func",
                                   unit.file)
        return list(dict.fromkeys(found))

    def member(self, unit, base, name: str, want: str) -> tuple[list[int], str]:
        ix = self.ix
        if base == ("super",):
            key = self.typer.class_of(unit)
            bases = ix.base_keys(ix.classes[key]) if key else []
            found = self.keep(ix.lookup(bases[0], name), want, unit.file) if bases else []
            return (found, EXACT) if found else ([], EXTERNAL)
        keys, engine, callables = self.typer.receiver(base, unit)
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

    def by_string(self, unit, base, text: str) -> tuple[list[int], str]:
        """A name written as a string the engine resolves at run time (NAME)."""
        ix = self.ix
        if base is not None and base != ("self",):
            keys, engine, callables = self.typer.receiver(base, unit)
            if callables:
                return callables, EXACT        # Callable.call("x", ...): the callable runs, "x" is an argument
            if keys:
                return self.member(unit, base, text, "func")
            return ([], EXTERNAL) if engine else ([], UNREACHED)
        if base == ("self",):
            found, how = self.member(unit, base, text, "func")
            if found:
                return found, how
        pool = [g for g in ix.by_name.get(text, []) if ix.units[g].lang == "gd"
                and ix.units[g].kind in FUNCTION_KINDS | {"signal"}] + ix.bound.get(text, [])
        pool = list(dict.fromkeys(pool))
        return (pool, "by name") if pool else ([], EXTERNAL)

    # ---- values for the typer ----------------------------------------------------------------------------------
    def argument_values(self, unit, index: int, depth: int) -> list[tuple]:
        """A parameter's values: the argument at that position at every site that calls the function; the engine's
        values when the engine calls it (a callback, a handler of an engine signal, a lambda the library runs)."""
        ix, out = self.ix, []
        incoming = self.incoming(unit.gid)
        if unit.name in ENGINE_CALLBACKS or unit.bound_as:
            out.append((ENGINE, unit))
        for edge in incoming:
            if edge.kind in CALLING and index < len(edge.args):
                out += self.typer.evaluate(edge.args[index], ix.units[edge.src], depth + 1)
            elif edge.kind == "signal":
                signal = ix.units[edge.src]
                if signal.kind == "engine_signal":
                    out.append((ENGINE, unit))
                for emit in self.incoming(signal.gid):
                    if emit.kind == "emit" and index < len(emit.args):
                        out += self.typer.evaluate(emit.args[index], ix.units[emit.src], depth + 1)
        if unit.kind == "lambda" and not any(e.kind in CALLING for e in incoming):
            out.append((ENGINE, unit))     # handed to the library or the engine, which calls it
        return out

    def assigned_values(self, unit, depth: int) -> list[tuple]:
        """A variable's or field's values: what every assignment anywhere in the code puts into it."""
        return [v for e in self.incoming(unit.gid) if e.kind == "write" and e.args
                for v in self.typer.evaluate(e.args[0], self.ix.units[e.src], depth + 1)]

    def function_targets(self, unit, text: str) -> list[int]:
        if "::" in text:
            scope, _, name = text.rpartition("::")
            return self.qualified(unit, scope, name, "func")[0]
        return self.bare(unit, text, "func")[0]

    def state_targets(self, unit, name: str) -> list[int]:
        if "::" in name:
            scope, _, last = name.rpartition("::")
            return self.qualified(unit, scope, last, "state")[0]
        return self.bare(unit, name, "state")[0]

    def resolve(self, unit, ref, want: str) -> tuple[list[int], str]:
        if ref.qualified:
            return self.qualified(unit, ref.scope, ref.name, want)
        if ref.base is not None:
            return self.member(unit, ref.base, ref.name, want)
        return self.bare(unit, ref.name, want)

    def link(self, src, dst, kind, ref, how, file="") -> None:
        self.keep_edge(Edge(src, dst, kind, ref.line, ref.cond, how, file or self.ix.units[src].file,
                            getattr(ref, "args", ())))

    def keep_edge(self, edge: Edge) -> None:
        key = (edge.src, edge.dst, edge.kind, edge.line, edge.file)
        if key in self.known:
            return
        self.known.add(key)
        self.edges.append(edge)
        self.out[edge.src].append(edge)
        self.inn[edge.dst].append(edge)
        self.typer.grew(("#in", edge.dst))      # what flows into dst grew: its readers are computed again

    def note(self, outcome: str, name: str) -> None:
        """The outcome of the reference being linked when it reaches nothing of the program (replaced on a re-link)."""
        self.status[self.typer.stack[-1]] = (outcome, name)

    def settle(self) -> None:
        """The external and unreached uses, from the last outcome of each reference."""
        for (_, gid, _), (outcome, name) in self.status.items():
            (self.external if outcome == EXTERNAL else self.unreached)[gid].add(name)
