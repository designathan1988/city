"""The fixture project loaded once, and the lookups every test of the tracer makes on it; the new core's analysis of a
fixture, once, by the Python semantics or the native engine (`CORE`), and the lookups its tests make (`HeapCase`)."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))

from featuretrace import engine_result, narrow_values, program, receivers, seeds, walk  # noqa: E402
from featuretrace.heap import analysis  # noqa: E402

FIXTURE = HERE / "fixture"
HEAP = FIXTURE / "heap"
_LOADED = {}
_ANALYSED = {}
ENGINE = "engine"                    # a HeapCase's CORE: the native engine
COLLAPSE_OFTEN = 4                   # the engine collapses its cycles every few items: every result must stay


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


def analysed(roots: tuple, base: str, declarations: bool, core: str = "python") -> tuple:
    """The new core's analysis of the fixture read from these roots (relative to the fixture folder), with the
    declaration files its TypeScript chooses when asked, by the Python semantics or the engine, once: (index,
    result)."""
    key = (roots, base, declarations, core)
    if key not in _ANALYSED:
        ix, _, _ = program.read([FIXTURE / r for r in roots], FIXTURE / base, None, declarations=declarations)
        run = (lambda: engine_result.analyse(ix, names=True, collapse_every=COLLAPSE_OFTEN)) if core == ENGINE \
            else (lambda: analysis.analyse(ix))
        _ANALYSED[key] = (ix, program.deep(run))
    return _ANALYSED[key]


class HeapCase(unittest.TestCase):
    """A case over the new core's analysis of a fixture: `ROOT` under `fixture/heap`, or `ROOTS` from the fixture
    folder with `BASE` and the declaration files (`DECLARATIONS`), by the core `CORE` names."""
    ROOT = ""
    ROOTS = ()
    BASE = "heap"
    DECLARATIONS = False
    CORE = "python"
    RUNS = ("call", "new", "handed")      # a function the platform runs for the caller is a call made through it

    def setUp(self):
        self.ix, self.result = analysed(self.ROOTS or ("heap/" + self.ROOT,), self.BASE, self.DECLARATIONS, self.CORE)

    def names(self, spec: str, kinds=RUNS) -> set:
        """The labels of what a definition links to by those kinds."""
        return {seeds.label(self.ix, e.dst) for e in self.result.out[self.gid(spec)] if e.kind in kinds}

    def reached(self, spec: str) -> set:
        """Every definition a definition reaches through the links that run code."""
        seen, todo = set(), [self.gid(spec)]
        while todo:
            gid = todo.pop()
            for e in self.result.out.get(gid, ()):
                if e.kind in self.RUNS and e.dst not in seen:
                    seen.add(e.dst)
                    todo.append(e.dst)
        return {seeds.label(self.ix, g) for g in seen}

    def gid(self, spec: str) -> int:
        found = seeds.find(self.ix, spec)
        self.assertEqual(1, len(found), found)
        return found[0]

    def returned(self, spec: str) -> set:
        return self.result.returned(self.gid(spec))

    def runs(self, spec: str) -> int:
        """How many runs (contexts) a function had."""
        return self.result.runs(self.gid(spec))

    def classes(self, spec: str) -> set:
        """The classes of the objects `new` made that a function returns."""
        return {v.split(":")[1].rsplit("#", 1)[-1] for v in self.returned(spec) if v.startswith("new:")}

    def links(self, spec: str) -> set:
        """(kind, target label) of every link leaving a definition."""
        return {(e.kind, seeds.label(self.ix, e.dst)) for e in self.result.out[self.gid(spec)]}

    def declared(self, spec: str, type_name: str) -> bool:
        """Whether a function returns an object of a declared type of that name."""
        return any(v.startswith("decl:") and f"#{type_name}:" in v for v in self.returned(spec))


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
