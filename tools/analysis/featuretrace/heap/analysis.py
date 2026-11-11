"""The analysis over abstract objects (log/trace-core-plan.md, Task 1): the places values are kept in are nodes of the
solver — an expression tree read in a unit, a local, a parameter, a function's `this` and its result, a variable, a
field of one object — and calls are found by the objects a callee holds, each found call a link of the form the walk
and the report read (`resolve.Edge`). `analyse(index)` lowers every unit's references and solves them.
"""
from __future__ import annotations

from collections import defaultdict

from ..model import FUNCTION_KINDS
from ..resolve import EXACT, Edge
from ..vocab import ANY_FIELD, CALL, NEW, SITE_FUNCTION, SITE_UNKNOWN
from . import refs, trees
from .objects import Objects
from .solver import Solver

TREE, LOCAL, PARAM, THIS, RET, HOLD, FIELD, INITIAL = range(8)       # what a node is


class Analysis:
    def __init__(self, index):
        self.ix = index
        self.objects = Objects()
        self.solver = Solver()
        self.wired = set()
        self.lines = defaultdict(list)       # a call's node -> (caller gid, line) of each reference written for it
        self.targets = defaultdict(set)      # a call's node -> (callee gid, link kind) found so far
        self.edges, self.known = [], set()
        self.out, self.inn = defaultdict(list), defaultdict(list)
        self.entered = set()                 # the functions given the unknown object as entry points

    # ---- nodes -------------------------------------------------------------------------------------------------
    def node(self, tree, unit):
        """The node of an expression tree read in `unit`, wired once by its shape (`trees`)."""
        node = (TREE, unit.gid, tree)
        if node not in self.wired:
            self.wired.add(node)
            trees.wire(self, node, tree, unit)
        return node

    def local(self, unit, name: str):
        node = (LOCAL, unit.gid, name)
        if node not in self.wired:
            self.wired.add(node)
            trees.wire_local(self, node, unit, name)
        return node

    def param(self, gid: int, index: int):
        return (PARAM, gid, index)

    def this(self, gid: int):
        return (THIS, gid)

    def ret(self, gid: int):
        node = (RET, gid)
        if node not in self.wired:
            self.wired.add(node)
            unit = self.ix.units[gid]
            for _, _, tree in unit.returns:
                self.solver.follow(self.node(tree, unit), node)
        return node

    def hold(self, gid: int):
        """What a variable or a static field holds: its initializers and every value written to it."""
        node = (HOLD, gid)
        if node not in self.wired:
            self.wired.add(node)
            self.solver.follow(self.initial(gid), node)
        return node

    def initial(self, gid: int):
        """What a variable's or a field's initializers give (an instance field's: every instance's, read on each)."""
        node = (INITIAL, gid)
        if node not in self.wired:
            self.wired.add(node)
            unit = self.ix.units[gid]
            for tree in unit.values:
                self.solver.follow(self.node(tree, unit), node)
        return node

    def field(self, obj: int, name: str):
        return (FIELD, obj, name)

    def function_object(self, gid: int) -> int:
        return self.objects.make(SITE_FUNCTION, gid)

    # ---- calls -------------------------------------------------------------------------------------------------
    def call(self, node, fn: int, receiver: int | None, args: list, kind: str) -> None:
        """A call whose node is `node` runs the function object `fn` on `receiver` with `args`: its parameters take the
        arguments, its `this` the receiver, the call its result; a link for each reference written for the call."""
        gid = self.objects.function(fn)
        if gid is None:
            return
        unit = self.ix.units[gid]
        for index, arg in enumerate(args):
            if arg is not None and index < len(unit.params):
                self.solver.follow(arg, self.param(gid, index))
        if receiver is not None:
            self.solver.add(self.this(gid), [receiver])
        self.solver.follow(self.ret(gid), node)
        if (gid, kind) not in self.targets[node]:
            self.targets[node].add((gid, kind))
            for caller, line in self.lines[node]:
                self.link(caller, gid, kind, line)

    def written(self, node, caller: int, line: int) -> None:
        """A reference written for the call `node`: a link to each function it runs, found now or later."""
        if (caller, line) not in self.lines[node]:
            self.lines[node].append((caller, line))
            for gid, kind in self.targets[node]:
                self.link(caller, gid, kind, line)

    def link(self, src: int, dst: int, kind: str, line: int) -> None:
        key = (src, dst, kind, line)
        if key not in self.known:
            self.known.add(key)
            edge = Edge(src, dst, kind, line, (), EXACT, self.ix.units[src].file)
            self.edges.append(edge)
            self.out[src].append(edge)
            self.inn[dst].append(edge)

    def any_field(self, obj: int):
        return self.field(obj, ANY_FIELD)

    def is_function_unit(self, gid: int) -> bool:
        return self.ix.units[gid].kind in FUNCTION_KINDS


def entries(an) -> bool:
    """The functions no call reaches get the unknown object in each parameter and `this` (once). Returns whether any
    did."""
    unknown, added = an.objects.make(SITE_UNKNOWN), False
    calling = (CALL, NEW)
    for unit in an.ix.units:
        gid = unit.gid
        if not an.is_function_unit(gid) or gid in an.entered or any(e.kind in calling for e in an.inn.get(gid, ())):
            continue
        an.entered.add(gid)
        for index in range(len(unit.params)):
            an.solver.add(an.param(gid, index), [unknown])
        an.solver.add(an.this(gid), [unknown])
        added = True
    return added


class Result:
    """What the walk and the report read: the links, by source and by target, and the objects a tree can hold."""

    def __init__(self, analysis: Analysis):
        self.analysis = analysis
        self.ix = analysis.ix
        self.edges, self.out, self.inn = analysis.edges, analysis.out, analysis.inn
        self.external, self.unreached = defaultdict(set), defaultdict(set)

    def points(self, gid: int, tree) -> set[str]:
        an = self.analysis
        node = an.node(tree, an.ix.units[gid])
        an.solver.run()
        return {an.objects.label(o) for o in an.solver.pts.get(node, ())}

    def returned(self, gid: int) -> set[str]:
        an = self.analysis
        node = an.ret(gid)
        an.solver.run()
        return {an.objects.label(o) for o in an.solver.pts.get(node, ())}


def analyse(index, progress=None) -> Result:
    """Every unit's references lowered, then solved to the least fixpoint; at the fixpoint, a function no call of the
    program reaches is an entry point: what calls it passes values the program does not make (the unknown object, in
    each parameter and `this`), and the solver runs to its fixpoint again."""
    an = Analysis(index)
    for unit in index.units:
        refs.lower(an, unit)
    an.solver.run()
    while entries(an):
        an.solver.run()
    if progress:
        progress(f"heap: {len(an.edges)} links, {len(an.objects.keys)} objects")
    return Result(an)
