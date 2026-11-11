"""VALUE, LOOK and FLOW (log/trace-rules.md): the types an expression tree can hold, as value nodes of the solver.

Each (tree, unit) is a node wired once by its shape (the EXPR rows): a name to its sources (a local's values, a
parameter's arguments, a capture, a variable's or field's held values, a class, a function), a member, a call, a step,
an operator to the transfer of each value of its base (`transfer`), a union to both sides, a constant to itself. What a
definition holds (`#hold`) and returns (`#ret`) are nodes too; parameters, variables and the engine's calls are the
places the links carry into (`flows`). Values only grow and only new ones propagate ("Difference propagation on every
value"); a shape whose value is not distributive over its sources (a callable or a class made from a name held in a
value) is a consumer computed again whole.

Type texts: a type as written, CLASS + key (a class of the program), FN + gid (a callable of a function), ELEM + type
(a value put into a container: what its elements hold), ENGINE (a value of the engine or the language).
"""
from __future__ import annotations

from . import names, transfer
from .library import DEREF, ITEM, ITER, passes_through
from .model import ENGINE_CALLBACKS, FUNCTION_KINDS
from .rules import CALLABLE_CONSTRUCTORS, NAME_CONSTRUCTORS
from .solver import CLASS, ELEM, ENGINE, FN, TEXT, Solver
from .types_of import TypeNames

CALLABLE_CALLS = transfer.CALLABLE_CALLS
NO_VALUE = ("void", "Variant", "auto", "decltype(auto)", "")
ARG, HELD, ENGINE_CALLED, HOLD, RET = "#arg", "#held", "#engine", "#hold", "#ret"
COMPUTED = "#computed"
STEPS = {"index": ITEM, "elements": ITER, "deref": DEREF}
CONSTANT_SHAPES = ("cast", "new", "callable", "self", "super", "strname", "module")


def same(t):
    def transfer_same(target, values):
        t.add(target, values)
    return transfer_same


class Typer(TypeNames, Solver):
    def __init__(self, index, resolver):
        self.ix = index
        self.rs = resolver
        super().__init__()
        self._classes = {}                 # (type text, scope) -> the class it names
        self.wired = set()                 # value nodes whose sources are wired
        self.computed = {}                 # consumer -> (value node, its computation)

    def class_of(self, unit) -> str | None:
        return unit.owner if unit.owner in self.ix.classes else None

    # ---- reading values (consumers) -------------------------------------------------------------------------
    def evaluate(self, tree, unit, depth: int = 0) -> list[tuple]:
        """Every type a tree can hold so far; the consumer reading it is computed again when it grows."""
        if not tree or unit is None:
            return []
        return self.values_of(self.node_of(tree, unit))

    def values_of(self, node) -> list[tuple]:
        self.read(node)
        return self.current(node)

    def follow(self, source, target) -> None:
        self.subscribe(source, target, ("same",), same(self))

    # ---- wiring ------------------------------------------------------------------------------------------------
    def node_of(self, tree, unit):
        node = (tree, unit.gid)
        if node not in self.wired:
            self.wired.add(node)
            self.value.setdefault(node, [])
            self.wire(node, tree, unit)
        return node

    def wire(self, node, tree, unit) -> None:
        kind = tree[0]
        if kind == "name":
            self.wire_name(node, tree[1], unit)
        elif kind == "member":
            base = self.node_of(tree[1], unit)
            base = self.stepped(base, DEREF) if tree[3] else base
            self.subscribe(base, node, ("member", tree[2], False), transfer.members(self, tree[2], False))
        elif kind == "call":
            self.wire_call(node, tree[1], tree[2], unit)
        elif kind == "#plain":                    # a by-name call's ordinary reading, its fallback
            self.wire_plain_call(node, tree[1], tree[2], unit)
        elif kind in STEPS:
            self.subscribe(self.node_of(tree[1], unit), node, ("step", STEPS[kind]), transfer.steps(self, STEPS[kind]))
        elif kind in ("op", "unary"):
            self.subscribe(self.node_of(tree[2], unit), node, ("op", tree[1]), transfer.operators(self, tree[1]))
        elif kind == "union":
            self.follow(self.node_of(tree[1], unit), node)
            self.follow(self.node_of(tree[2], unit), node)
        elif kind == "collection":
            self.add(node, [(ENGINE, unit)])
            for value in tree[1]:
                self.subscribe(self.node_of(value, unit), node, ("elem",), transfer.elements(self))
        elif kind == "put":
            self.subscribe(self.node_of(tree[1], unit), node, ("elem",), transfer.elements(self))
        elif kind in ("await", "addr"):
            self.follow(self.node_of(tree[1], unit), node)
        elif kind in CONSTANT_SHAPES:
            self.add(node, self.constant(tree, unit))
        else:
            self.add(node, [(ENGINE, unit)])      # literal, node: the engine's or the language's values

    def constant(self, tree, unit) -> list[tuple]:
        kind = tree[0]
        if kind in ("cast", "new"):
            return [(tree[1], unit)] if tree[1] else [(ENGINE, unit)]
        if kind == "callable":
            return [(FN + str(self.ix.offset[unit.file] + tree[1]), unit)]
        if kind == "self":
            key = self.class_of(unit)
            return [(CLASS + key, unit)] if key else []
        if kind == "super":
            key = self.class_of(unit)
            bases = self.ix.base_keys(self.ix.classes[key]) if key else []
            return [(CLASS + bases[0], unit)] if bases else [(ENGINE, unit)]
        if kind == "strname":
            return [(TEXT + tree[1], unit)]
        key = self.ix.js.resolve(unit.file, tree[1])                     # import("m"): the namespace (J3)
        return [(CLASS + key, unit)] if key else [(ENGINE, unit)]

    def stepped(self, base, step: str):
        node = ("#step", base, step)
        if node not in self.wired:
            self.wired.add(node)
            self.subscribe(base, node, ("step", step), transfer.steps(self, step))
        return node

    def wire_call(self, node, callee, args, unit) -> None:
        if self.by_name(callee, args, unit):
            plain = self.node_of(("#plain",) + (callee, args), unit)
            self.compute_whole(node, lambda: names.made_by_name(self, callee, args, unit)
                               or names.callable_of(self, callee, args, unit, 0) or self.values_of(plain))
            return
        self.wire_plain_call(node, callee, args, unit)

    def by_name(self, callee, args, unit) -> bool:
        """A call that makes a class or a callable from a name held in a value (G6, G7): not distributive."""
        if callee[0] == "member" and callee[1][0] == "name":
            return (callee[1][1], callee[2]) in NAME_CONSTRUCTORS
        return callee[0] == "name" and callee[1] in CALLABLE_CONSTRUCTORS

    def wire_plain_call(self, node, callee, args, unit) -> None:
        if callee[0] == "name" and args and unit.lang == "cpp" and passes_through(callee[1]) \
                and not self.rs.function_targets(unit, callee[1]):
            self.follow(self.node_of(args[0], unit), node)          # std::move(x), std::forward<T>(x): x itself
            return
        if callee[0] == "member":
            base = self.node_of(callee[1], unit)
            base = self.stepped(base, DEREF) if callee[3] else base
            self.subscribe(base, node, ("member", callee[2], True), transfer.members(self, callee[2], True))
            return
        if callee[0] == "name":
            name = callee[1]
            key = self.ix.gd_class(name, unit.owner) if unit.lang == "gd" else None if unit.lang == "js" else \
                self.ix.cpp_class(name, unit.owner, unit.file)
            if key:
                self.add(node, [(CLASS + key, unit)])               # a class called constructs it
                return
            if unit.lang == "gd" and name[:1].isupper() and not self.rs.function_targets(unit, name):
                self.add(node, [(name, unit)])                      # an engine type constructed (PascalCase)
                return
        self.subscribe(self.node_of(callee, unit), node, ("call",), transfer.calls(self))

    def compute_whole(self, node, compute) -> None:
        consumer = (COMPUTED, node)
        self.computed[consumer] = (node, compute)
        self.recompute(consumer)

    def recompute(self, consumer) -> None:
        """A consumer value computed again whole from what it reads (its value only grows)."""
        node, compute = self.computed[consumer]
        self.dirty.discard(consumer)
        self.grown.pop(consumer, None)         # computed again whole
        outer, self.substitution = self.substitution, None    # whole, even inside a reference linked by difference
        self.stack.append(consumer)
        try:
            values = compute()
        finally:
            self.stack.pop()
            self.substitution = outer
        self.add(node, values)

    # ---- names (LOOK, FLOW) -------------------------------------------------------------------------------------
    def wire_name(self, node, name: str, unit) -> None:
        """LOOK: local -> member (with bases) -> outer class / namespaces -> global; else the engine's."""
        if name in unit.lambdas:
            self.add(node, [(FN + str(self.ix.offset[unit.file] + unit.lambdas[name]), unit)])
            return
        if name in unit.types:
            self.wire_local(node, name, unit)
            return
        targets = self.rs.state_targets(unit, name)
        for gid in targets:
            self.follow(self.hold_node(self.ix.units[gid]), node)
        if unit.lang == "js":
            found = [(FN + str(f), unit) for f in self.rs.function_targets(unit, name)]
            key = self.ix.js.class_of(name, unit.file, unit.owner)
            found += [(CLASS + key, unit)] if key else []
            self.add(node, found or ([] if targets else [(ENGINE, unit)]))
            return
        if targets:
            return
        key = self.ix.gd_class(name, unit.owner) if unit.lang == "gd" else self.ix.cpp_class(name, unit.owner,
                                                                                                unit.file)
        functions = self.rs.function_targets(unit, name) if not key else []
        self.add(node, [(CLASS + key, unit)] if key else [(FN + str(f), unit) for f in functions] or [(ENGINE, unit)])

    def wire_local(self, node, name: str, unit) -> None:
        """A local or parameter: its declared type, every value written to it, for a parameter the argument of every
        link into the function (and the engine's when the engine calls it), for a capture the enclosing local."""
        for tree in unit.types.get(name, []):
            self.follow(self.node_of(tree, unit), node)
        if name in unit.params:
            self.follow((ARG, unit.gid, unit.params.index(name)), node)
            self.follow((ENGINE_CALLED, unit.gid), node)
            if unit.name in ENGINE_CALLBACKS or unit.bound_as:
                self.add(node, [(ENGINE, unit)])
        if name in unit.captured and unit.outer >= 0:
            outer = self.ix.units[self.ix.offset[unit.file] + unit.outer]
            self.follow(self.node_of(("name", name), outer), node)

    def named(self, name: str, unit, depth: int = 0) -> list[tuple]:
        return self.evaluate(("name", name), unit)

    def hold_node(self, unit):
        """What a variable or field holds: its declared type, its initializers, every value written to it."""
        node = (HOLD, unit.gid)
        if node not in self.wired:
            self.wired.add(node)
            self.value.setdefault(node, [])
            if unit.kind in FUNCTION_KINDS:
                self.add(node, [(FN + str(unit.gid), unit)])
                return node
            if unit.type_text and unit.type_text not in NO_VALUE:
                self.add(node, [(unit.type_text, unit)])
            for tree in unit.values:
                self.follow(self.node_of(tree, unit), node)
            self.follow((HELD, unit.gid), node)
        return node

    def ret_node(self, unit):
        """What a call of the function yields: its declared return type, else what its `return`s carry."""
        node = (RET, unit.gid)
        if node not in self.wired:
            self.wired.add(node)
            self.value.setdefault(node, [])
            if unit.type_text == "void":
                return node
            if unit.type_text and unit.type_text not in NO_VALUE:
                self.add(node, [(unit.type_text, unit)])
            for _, _, tree in unit.returns:
                self.follow(self.node_of(tree, unit), node)
        return node

    def member(self, text: str, ctx, name: str, called: bool, depth: int = 0) -> list[tuple]:
        """A member of one value (for the consumers that need it whole)."""
        node = ("#member", text, ctx.gid, name, called)
        if node not in self.wired:
            self.wired.add(node)
            self.value.setdefault(node, [])
            transfer.member_one(self, node, text, ctx, name, called)
        return self.values_of(node)

    def receiver(self, tree, unit) -> tuple[list[str], bool, list[int]]:
        """(classes of the program the receiver can be, whether it can be an engine value, callables it can be)."""
        keys, engine, functions = [], False, []
        for text, ctx in self.evaluate(tree, unit):
            if text.startswith(ELEM):
                engine = True              # a container's elements: the container is the language's or the library's
                continue
            if text.startswith(FN):
                functions.append(int(text[len(FN):]))
                continue
            key = self.type_class(text, ctx)
            if key:
                keys.append(key)
            else:
                engine = True
        return list(dict.fromkeys(keys)), engine, list(dict.fromkeys(functions))
