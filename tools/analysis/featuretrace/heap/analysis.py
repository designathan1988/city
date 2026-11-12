"""The analysis over abstract objects (log/trace-core-plan.md, Tasks 1-3): the places values are kept in are nodes of
the solver — an expression tree read in a unit, a local, a parameter, a function's `this` and its result, each in one
context; a variable; a field of one object — and calls are found by the objects a callee holds, each found call a link
of the form the walk and the report read (`resolve.Edge`).

Contexts are finite by construction (`contexts`): a function runs in (its closure's environment, the call's site), a
method called on a receiver in (its environment, the receiver's allocation site and heap context); an object made in a
context carries its heap context, a closure the whole context as its environment. A unit's references are lowered
once per context it runs in. Module code runs first, in ROOT; then the entry points (`entries`).
"""
from __future__ import annotations

from collections import defaultdict

from ..model import FUNCTION_KINDS
from ..resolve import EXACT, Edge
from ..model import STATE_KINDS as STATE
from ..vocab import (ANY_FIELD, CALL, ELEMENTS_FIELD, NEW, RESULT_FIELD, SITE_FUNCTION, SITE_GENERATOR,
                     SITE_PROMISE)
from . import contexts, declared, flows, members, protocols, refs, trees, typesys, values
from .entries import entries
from .objects import Objects
from .solver import Solver

TREE, LOCAL, PARAM, THIS, RET, HOLD, FIELD, INITIAL = range(8)       # what a node is
INSTANCES, MADE_BY_CALL = 60, 61               # every object of a class; the generator or promise a call makes
ROOT = contexts.ROOT


class Analysis:
    def __init__(self, index):
        self.ix = index
        self.objects = Objects()
        self.solver = Solver()
        self.wired = set()
        self.active = defaultdict(set)       # gid -> the contexts its references were lowered in
        self.lines = defaultdict(list)       # a call's node -> (caller gid, line) of each reference written for it
        self.targets = defaultdict(set)      # a call's node -> (callee gid, link kind) found so far
        self.edges, self.known = [], set()
        self.out, self.inn = defaultdict(list), defaultdict(list)
        self.entered = set()                 # (gid, context) of each run started as an entry point
        self.closures = defaultdict(set)     # gid -> the environments its function objects were made in
        self.relays = defaultdict(list)      # a call's node -> the nodes of the calls it hands functions to
        self.memo = {}                       # what a declared type resolves to, merges with, looks up

    # ---- nodes -------------------------------------------------------------------------------------------------
    def node(self, tree, unit, ctx):
        """The node of an expression tree read in `unit` in context `ctx`, wired once by its shape (`trees`)."""
        node = (TREE, unit.gid, tree, ctx)
        if node not in self.wired:
            self.wired.add(node)
            trees.wire(self, node, tree, unit, ctx)
        return node

    def local(self, unit, name: str, ctx):
        node = (LOCAL, unit.gid, name, ctx)
        if node not in self.wired:
            self.wired.add(node)
            trees.wire_local(self, node, unit, name, ctx)
        return node

    def param(self, gid: int, index: int, ctx):
        return (PARAM, gid, index, ctx)

    def this(self, gid: int, ctx):
        return (THIS, gid, ctx)

    def ret(self, gid: int, ctx):
        node = (RET, gid, ctx)
        if node not in self.wired:
            self.wired.add(node)
            unit = self.ix.units[gid]
            for _, _, tree in unit.returns:
                self.solver.follow(self.node(tree, unit, ctx), node)
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
                self.solver.follow(self.node(tree, unit, ROOT), node)
            if unit.ambient and unit.result_type and unit.kind in STATE:     # declared: an object of its type
                self.solver.follow(values.produce(self, unit.result_type, (None, None, None), ((gid,), ROOT)), node)
        return node

    def field(self, obj: int, name: str):
        return (FIELD, obj, name)

    def any_field(self, obj: int):
        return self.field(obj, ANY_FIELD)

    def function_object(self, gid: int, env=ROOT) -> int:
        """A function's object, made in context `env` (its environment: what its free variables read)."""
        self.closures[gid].add(env)
        return self.objects.make(SITE_FUNCTION, gid, env)

    # ---- running units -----------------------------------------------------------------------------------------
    def activate(self, gid: int, ctx) -> None:
        """A unit runs in a context: its references are lowered there (once)."""
        if ctx not in self.active[gid]:
            self.active[gid].add(ctx)
            refs.lower(self, self.ix.units[gid], ctx)

    def call(self, node, fn: int, receiver: int | None, args: list, kind: str, site=None, shared=False) -> None:
        """A call whose node is `node` runs the function object `fn` on `receiver` with `args`, in its context: its
        parameters take the arguments, its `this` the receiver, the call its result; a link for each reference.
        A plain call's site is its node without the caller's context, or `site`, (static site, heap context), when the
        caller gives it (a callback's node carries objects no site may hold); `shared`: every such call hands the
        function the same arguments (unknown code's), so one run in its environment serves them all. A function only
        declared does what its declaration says (`declared`); an object no function of the program is called as
        its kind says."""
        objects = self.objects
        where = site if site is not None else declared.call_site(self, node)    # (static site, heap context)
        gid = objects.function(fn)
        if gid is None:
            declared.call_object(self, node, fn, receiver, args, where)
            return
        unit = self.ix.units[gid]
        if unit.decl_only:                   # no body: nothing runs (no context, no parameter, no result of its own)
            self.reached(node, gid, kind)
            if typesys.declared(self, gid):
                declared.apply(self, node, gid, receiver, args, where)
            return
        env = objects.env(fn)
        if shared or not flows.contextual(self, gid):     # nothing that differs between callers reaches out
            ctx = contexts.of_call(env, contexts.ONCE)
        elif receiver is not None:
            ctx = contexts.of_method(env, objects.site(receiver), objects.heap(receiver))
        else:
            ctx = contexts.of_call(env, where[0])
        self.activate(gid, ctx)
        for index, arg in enumerate(args):
            if arg is not None and index < len(unit.params):
                self.solver.follow(arg, self.param(gid, index, ctx))
        if receiver is not None:
            self.solver.add(self.this(gid, ctx), [receiver])
        self.solver.follow(self.result(gid, ctx, where), node)
        self.reached(node, gid, kind)

    def result(self, gid: int, ctx, where):
        """What a call gives: what the function returns; a generator function's call, a generator object whose
        elements are its yields; an async function's, a promise whose result is what it returns, settled."""
        unit = self.ix.units[gid]
        if not unit.generator and not unit.asynchronous:
            return self.ret(gid, ctx)
        made = self.objects.made_by_call(SITE_GENERATOR if unit.generator else SITE_PROMISE, gid, where)
        held = (MADE_BY_CALL, made)
        if held not in self.wired:
            self.wired.add(held)
            self.solver.add(held, [made])
        if unit.generator:
            slot = members.field(self, made, ELEMENTS_FIELD)
            for tree in unit.yields:
                self.solver.follow(self.node(tree, unit, ctx), slot)
        else:
            protocols.settle(self, members.field(self, made, RESULT_FIELD), self.ret(gid, ctx))
        return held

    def reached(self, node, gid: int, kind) -> None:
        """The call `node` runs `gid`: a link for each reference written for it (`kind` True: a `new`)."""
        kind = NEW if kind is True else CALL if kind is False else kind
        if (gid, kind) not in self.targets[node]:
            self.targets[node].add((gid, kind))
            for caller, line in self.lines[node]:
                self.link(caller, gid, kind, line)

    def relay(self, origin, node) -> None:
        """The references written for the call `origin` are written for `node` too (a call it hands a function to)."""
        if origin is None or node in self.relays[origin]:
            return
        self.relays[origin].append(node)
        for caller, line in list(self.lines[origin]):
            self.written(node, caller, line)

    def written(self, node, caller: int, line: int) -> None:
        """A reference written for the call `node`: a link to each function it runs, found now or later."""
        if (caller, line) not in self.lines[node]:
            self.lines[node].append((caller, line))
            for gid, kind in list(self.targets[node]):
                self.link(caller, gid, kind, line)
            for other in list(self.relays.get(node, ())):
                self.written(other, caller, line)

    def link(self, src: int, dst: int, kind: str, line: int) -> None:
        key = (src, dst, kind, line)
        if key not in self.known:
            self.known.add(key)
            edge = Edge(src, dst, kind, line, (), EXACT, self.ix.units[src].file)
            self.edges.append(edge)
            self.out[src].append(edge)
            self.inn[dst].append(edge)

    # ---- classes -----------------------------------------------------------------------------------------------
    def made_by(self, obj: int) -> str | None:
        """The class of the objects `new` on `obj` makes: a class itself, or a constructor function's own (N1'')."""
        key = self.objects.class_itself(obj)
        if key is not None:
            return key if key in self.ix.classes else None
        gid = self.objects.function(obj)
        return self.ix.units[gid].constructs or None if gid is not None else None

    def instance_made(self, obj: int) -> None:
        """An object of a class: one of the instances of its class and of each class above it."""
        key = self.objects.class_of(obj)
        for k in typesys.ancestors(self, key) if key is not None else []:
            self.solver.add(self.instances(k), [obj])

    def instances(self, key: str):
        return (INSTANCES, key)

    def is_function_unit(self, gid: int) -> bool:
        return self.ix.units[gid].kind in FUNCTION_KINDS


class Result:
    """What the walk and the report read: the links, by source and by target, and the objects a tree can hold."""

    def __init__(self, analysis: Analysis):
        self.analysis = analysis
        self.ix = analysis.ix
        self.edges, self.out, self.inn = analysis.edges, analysis.out, analysis.inn
        self.external, self.unreached = defaultdict(set), defaultdict(set)

    def points(self, gid: int, tree) -> set[str]:
        an = self.analysis
        nodes = [an.node(tree, an.ix.units[gid], ctx) for ctx in an.active.get(gid) or {ROOT}]
        an.solver.run()
        return {an.objects.label(o) for node in nodes for o in an.solver.held(node)}

    def runs(self, gid: int) -> int:
        """How many runs (contexts) a function had."""
        return len(self.analysis.active.get(gid, ()))

    def returned(self, gid: int) -> set[str]:
        an = self.analysis
        nodes = [an.ret(gid, ctx) for ctx in an.active.get(gid) or {ROOT}]
        an.solver.run()
        return {an.objects.label(o) for node in nodes for o in an.solver.held(node)}


def analyse(index, progress=None) -> Result:
    """Module code, then the entry points, each solved to the least fixpoint, until no function starts."""
    an = Analysis(index)
    entries(an, modules=True)
    an.solver.run()
    while entries(an, modules=False):
        an.solver.run()
    if progress:
        progress(f"heap: {len(an.edges)} links, {len(an.objects.keys)} objects")
    return Result(an)
