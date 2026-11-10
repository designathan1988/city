"""FLOW (log/trace-rules.md, S2): the values the links carry, as nodes of the solver. Each argument of each link is
carried to its parameter once, and again only when it grows (difference propagation); an assigned value to its
variable; what a signal carries to its handlers; what the language says a callback handed to a library receives.
The resolver (`resolve.Resolver`) is built on this: the links are its, the values these.
"""
from __future__ import annotations

from .model import ENGINE_CALLBACKS
from .typer import ENGINE, identity

CARRYING = ("call", "override", "name", "thread", "emit", "new")   # links whose arguments reach parameters
FLOW, ARG, HELD, ENGINE_CALLED = "#flow", "#arg", "#held", "#engine"


class Flows:
    def argument_values(self, unit, index: int, depth: int) -> list[tuple]:
        """A parameter's values: what the links into the function have carried to that position so far; the engine's
        values when the engine calls it (a callback, a bound method, a handler of an engine signal, a callable handed
        to the engine or the library)."""
        self.typer.read((ARG, unit.gid, index))
        self.typer.read((ENGINE_CALLED, unit.gid))
        out = list(self.carried.get((ARG, unit.gid, index), {}).values())
        if unit.name in ENGINE_CALLBACKS or unit.bound_as or unit.gid in self.engine_called:
            out.append((ENGINE, unit))
        return out

    def assigned_values(self, unit, depth: int) -> list[tuple]:
        """A variable's or field's values: what every assignment anywhere in the code has put into it so far."""
        self.typer.read((HELD, unit.gid))
        return list(self.carried.get((HELD, unit.gid), {}).values())

    def carry(self, place: tuple, values: list[tuple]) -> None:
        """Adds values to a place (a parameter, a variable); its readers are computed again only if it grew."""
        held = self.carried.setdefault(place, {})
        before = len(held)
        for text, ctx in values:
            held.setdefault(identity(text, ctx), (text, ctx))
        if len(held) != before:
            self.typer.grew(place)

    def flow(self, node: tuple) -> None:
        """One value carried by one link, as a node of the solver: an argument into its parameter, an assigned value
        into its variable, what a signal carries into its handler. It is computed again when its source grows, and
        adds only what is new."""
        _, number, position = node
        edge = self.edges[number]
        typer = self.typer
        typer.dirty.discard(node)
        typer.stack.append(node)
        try:
            if edge.kind == "signal":
                typer.read((ARG, edge.src, position))
                values = list(self.carried.get((ARG, edge.src, position), {}).values())
            else:
                values = typer.evaluate(edge.args[position], self.ix.units[edge.src])
        finally:
            typer.stack.pop()
        self.carry((HELD, edge.dst) if edge.kind == "write" else (ARG, edge.dst, position), values)

    def flows_of(self, number: int, edge) -> None:
        """The flow nodes a new link brings."""
        if edge.kind in CARRYING:
            count = len(edge.args)
        elif edge.kind == "write":
            count = min(1, len(edge.args))
        elif edge.kind == "signal":
            count = len(self.ix.units[edge.dst].params)
            if self.ix.units[edge.src].kind == "engine_signal":
                self.engine_calls(edge.dst)
        elif edge.kind == "handed":
            count = len(edge.args)          # a callback's arguments, when the language says what they are
            self.engine_calls(edge.dst)
        else:
            count = 0
        for position in range(count):
            node = (FLOW, number, position)
            self.typer.dirty.add(node)
            self.typer.work.append(node)

    def engine_calls(self, gid: int) -> None:
        if gid not in self.engine_called:
            self.engine_called.add(gid)
            self.typer.grew((ENGINE_CALLED, gid))
