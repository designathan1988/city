"""FLOW (log/trace-rules.md; "Difference propagation on every value"): what the links carry, as subscriptions between
value nodes. A link that carries arguments subscribes each argument's tree to the parameter it reaches, and a JS
parameter its call writes no argument for holds `undefined` (MDN Functions: unless it has a default); a write
subscribes the value written to the variable or field, unless the write runs the property's setter, which takes it as
its argument (G8'); a signal link subscribes what the signal carries to each parameter of its handler; the engine's
calls (a callable handed to it, a handler of an engine signal) add the engine's value to the function's parameters.
Only values new to a source pass on. The resolver (`resolve.Resolver`) is built on this: the links are its, the values
these.
"""
from __future__ import annotations

from . import receivers, templates
from .model import FUNCTION_KINDS
from .places import ARG, ENGINE_CALLED, HELD
from .solver import ENGINE, UNDEFINED

CARRYING = ("call", "override", "name", "thread", "emit", "new", "handed", "bound")   # links whose arguments reach
                                                                                  # parameters
WRITTEN_CALLS = ("call", "override", "new")      # links of a call whose arguments are written at the site
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
                if tree is not None:                # a place a `bind` filled (J2''): its own link carried it
                    typer.follow(typer.node_of(tree, source), (ARG, edge.dst, position))
            self.omitted(edge)
            receivers.carry(self, edge)                              # the object the method runs on (J2')
        elif edge.kind == "write" and edge.args:
            target = units[edge.dst]
            if target.kind in FUNCTION_KINDS:       # a bound property's setter (BIND): the value is its last argument
                typer.follow(typer.node_of(edge.args[0], source), (ARG, edge.dst, max(len(target.params) - 1, 0)))
            elif target.runs("set", source) is None:                 # a setter takes the value instead (G8')
                typer.follow(typer.node_of(edge.args[0], source), (HELD, edge.dst))
        elif edge.kind == "signal":
            for position in range(len(units[edge.dst].params)):
                typer.follow((ARG, edge.src, position), (ARG, edge.dst, position))
            if source.kind == "engine_signal":
                self.engine_calls(edge.dst)
        if edge.kind == "handed":
            self.engine_calls(edge.dst)

    def omitted(self, edge) -> None:
        """A JS call that writes fewer arguments than its function has parameters: each one past them holds
        `undefined`, unless written with a default (it holds that) or a rest parameter (an empty array). A call with a
        spread argument writes an unknown count."""
        callee = self.ix.units[edge.dst]
        if edge.kind not in WRITTEN_CALLS or edge.argc < 0 or any(tree is not None and tree[0] == "elements" for tree in edge.args):
            return
        for position in range(edge.argc, len(callee.params)):
            if templates.omitted_is_undefined(callee, position):
                self.typer.add((ARG, edge.dst, position), [(UNDEFINED, callee)])

    def engine_calls(self, gid: int) -> None:
        """The engine or a library calls the function: its parameters hold the engine's values."""
        self.typer.add((ENGINE_CALLED, gid), [(ENGINE, self.ix.units[gid])])
