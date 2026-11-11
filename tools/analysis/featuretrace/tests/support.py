"""The fixture project loaded once, and the lookups every test of the tracer makes on it."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))

from featuretrace import narrow_values, program, receivers, seeds, walk  # noqa: E402

FIXTURE = HERE / "fixture"
_LOADED = {}


def loaded(roots: tuple = ("project", "native"), base: str = "") -> tuple:
    """The fixture read from these roots (relative to the fixture folder), once per set of roots."""
    if (roots, base) not in _LOADED:
        _LOADED[(roots, base)] = program.load([FIXTURE / r for r in roots], FIXTURE / base, None)
    return _LOADED[(roots, base)]


class FixtureCase(unittest.TestCase):
    ROOTS = ("project", "native")      # what the case reads, from the fixture folder
    BASE = ""                          # where the files' displayed paths start

    @classmethod
    def setUpClass(cls):
        cls.ix, cls.rs, cls.files, cls.broken = loaded(cls.ROOTS, cls.BASE)

    def gid(self, spec: str) -> int:
        found = seeds.find(self.ix, spec)
        self.assertEqual(len(found), 1, f"{spec} -> {[self.ix.units[g].qual for g in found]}")
        return found[0]

    def out(self, spec: str, kinds=("call",)) -> dict:
        """target label -> how, for the links of those kinds leaving a definition."""
        return {seeds.label(self.ix, e.dst): e.how for e in self.rs.out[self.gid(spec)] if e.kind in kinds}

    def names(self, spec: str, kinds=("call", "new")) -> set:
        """The labels of what a definition links to by those kinds."""
        return set(self.out(spec, kinds))

    def reached(self, spec: str) -> set:
        return {seeds.label(self.ix, g) for g in walk.forward(self.rs, [self.gid(spec)]).reached}


class Solved:
    """Values read once the solver reached its fixpoint (mixed into a `FixtureCase`)."""

    def solved(self, node) -> set:
        """A node's values once the solver reached its fixpoint again (a node first wired after the solve, such as a
        templated function's own result, is run to its own: pending differences, then the guards' `undefined` and the
        receivers no call reached)."""
        typer = self.rs.typer
        while True:
            typer.propagate()
            if not narrow_values.settle(typer) | receivers.settle(typer):
                return {text for text, _ in typer.current(node)}

    def held(self, spec: str, name: str) -> set:
        return self.solved(self.rs.typer.node_of(("name", name), self.ix.units[self.gid(spec)]))

    def returned(self, spec: str) -> set:
        return self.solved(self.rs.typer.ret_node(self.ix.units[self.gid(spec)]))
