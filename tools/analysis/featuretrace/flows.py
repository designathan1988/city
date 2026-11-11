"""FLOW (log/trace-rules.md; "Difference propagation on every value"): what the links carry, as subscriptions between
value nodes. A link that carries arguments subscribes each argument's tree to the parameter it reaches; a write
subscribes the value written to the variable or field; a signal link subscribes what the signal carries to each
parameter of its handler; the engine's calls (a callable handed to it, a handler of an engine signal) add the engine's
value to the function's parameters. Only values new to a source pass on. The resolver (`resolve.Resolver`) is built on
this: the links are its, the values these.
"""
from __future__ import annotations

from .places import ARG, ENGINE_CALLED, HELD
from .solver import ENGINE

CARRYING = ("call", "override", "name", "thread", "emit", "new", "handed")   # links whose arguments reach parameters
FLOW = "#flow"


class Flows:
    def carry(self, place: tuple, values: list[tuple]) -> None:
        """Adds values to a place (a parameter, a variable)."""
        self.typer.add(place, values)

    def flows_of(self, edge) -> None:
        """The subscriptions a new link, or a new call site of a known link, brings."""
        typer, units = self.typer, self.ix.units
        source = units[edge.src]
        if edge.kind in CARRYING:
            for position, tree in enumerate(edge.args):
                typer.follow(typer.node_of(tree, source), (ARG, edge.dst, position))
        elif edge.kind == "write" and edge.args:
            typer.follow(typer.node_of(edge.args[0], source), (HELD, edge.dst))
        elif edge.kind == "signal":
            for position in range(len(units[edge.dst].params)):
                typer.follow((ARG, edge.src, position), (ARG, edge.dst, position))
            if source.kind == "engine_signal":
                self.engine_calls(edge.dst)
        if edge.kind == "handed":
            self.engine_calls(edge.dst)

    def engine_calls(self, gid: int) -> None:
        """The engine or a library calls the function: its parameters hold the engine's values."""
        self.typer.add((ENGINE_CALLED, gid), [(ENGINE, self.ix.units[gid])])
