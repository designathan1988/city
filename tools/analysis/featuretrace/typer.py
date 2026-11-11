"""VALUE and FLOW (log/trace-rules.md): the types an expression tree can hold, as value nodes of the solver.

Each (tree, unit) is a node wired once by its shape (the EXPR rows): a name to its sources (`places`: a local's values,
a parameter's arguments, a capture, a variable's or field's held values, a class, a function), a member, a call, a step,
an operator to the transfer of each value of its base (`transfer`), a union to both sides, a constant to itself. A tree
read in a template of a function is the template's own node only where the template's key reaches it, else the unit's
(`templates`, C2'''). Values only grow and only new ones propagate ("Difference propagation on every value"); a shape
whose value is not distributive over its sources (a callable or a class made from a name held in a value) is a
consumer computed again whole.

Type texts: a type as written, CLASS + key (a class of the program), FN + gid (a callable of a function), ELEM + type
(a value put into a container: what its elements hold), ENGINE (a value of the engine or the language).
"""
from __future__ import annotations

from . import closures, js_library, js_transfer, names, narrow_values, receivers, templates, transfer, truth
from .js_library import EACH
from .library import DEREF, ITEM, ITER, passes_through
from .places import Places
from .rules import CALLABLE_CONSTRUCTORS, NAME_CONSTRUCTORS
from .solver import ALIAS, CLASS, ENGINE, FN, TEXT, Solver
from .types_of import TypeNames

CALLABLE_CALLS = transfer.CALLABLE_CALLS
COMPUTED = "#computed"
STEPS = {"index": ITEM, "elements": ITER, "deref": DEREF, "each": EACH}
CONSTANT_SHAPES = ("cast", "new", "callable", "super", "strname", "module")


class Typer(Places, TypeNames, Solver):
    def __init__(self, index, resolver):
        self.ix = index
        self.rs = resolver
        super().__init__()
        self._classes = {}                 # (type text, scope) -> the class it names
        self.wired = set()                 # value nodes whose sources are wired
        self.computed = {}                 # consumer -> (value node, its computation)
        self.template_keys = {}            # gid -> the parameters whose values key its templates (`templates`, C1)
        self.templates = {}                # (gid, tuple) -> the template (`templates.Frame`)
        self.template_depends = {}         # (gid, tree) -> whether its key parameters reach the tree (C2''')
        self.js_written = None             # the property names JS code writes on a receiver (`js_transfer`, V2)
        self.undecided = []                # a property guard on an object whose property no value reached yet (N1)
        self.joined = set()                # (class, interface) whose properties are one (`structural`, S1)
        self.unreceived = []               # methods' `this` nodes no call with a receiver reached yet (`receivers`)
        self.bound_members = {}            # class or interface key -> the members it requires (`bounds`, S2)
        self.constructors = None           # a function's prototype class -> the function (`js_transfer`, N1'')
        self.free = {}                     # gid -> a function's free variables (`closures.free_names`, C4)
        self.envs = [None]                 # environment id -> the template a closure was made in (C4'); 0: none
        self.env_ids = {}                  # a template's key -> its environment id

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

    # ---- wiring ------------------------------------------------------------------------------------------------
    def environment(self, frame) -> int:
        """The id of the template a closure made in it closes over (C4')."""
        if frame.key not in self.env_ids:
            self.env_ids[frame.key] = len(self.envs)
            self.envs.append(frame)
            closures.closure_writes(self, frame, self.env_ids[frame.key])     # its closures' own properties
        return self.env_ids[frame.key]

    def closure_frame(self, unit, env: int):
        """A function run as the closure made in environment `env`: its free variables read that template (C4')."""
        key = (unit.gid, (templates.CLOSED, env))
        if key not in self.templates:
            self.templates[key] = templates.Frame(unit, (templates.CLOSED, env), env=self.envs[env])
        return self.templates[key]

    def node_of(self, tree, unit):
        while unit.key != unit.gid and not templates.depends(self, unit, tree):
            unit = unit.base                      # its key does not reach the tree: the unit's own, shared (C2''')
        node = (tree, unit.key)                   # the unit's gid, or a template's (gid, tuple)
        if node not in self.wired:
            self.wired.add(node)
            self.value.setdefault(node, [])
            self.wire(node, tree, unit)
        return node

    def wire(self, node, tree, unit) -> None:
        kind = tree[0]
        if kind == "name":
            self.wire_name(node, tree[1], unit)
        elif kind == "member":                    # read where it is written: in its own accessors or not (G8')
            base = self.node_of(tree[1], unit)
            base = self.stepped(base, DEREF) if tree[3] else base
            self.subscribe(base, node, ("member", tree[2], False), transfer.members(self, tree[2], False, unit))
        elif kind == "call":
            self.wire_call(node, tree[1], tree[2], unit)
        elif kind == "#plain":                    # a by-name call's ordinary reading, its fallback
            self.wire_plain_call(node, tree[1], tree[2], unit)
        elif kind == "index" and unit.lang == "js":   # `o[k]`: the object's and the key's values together (T4')
            base, key = self.node_of(tree[1], unit), self.node_of(tree[2], unit)
            self.subscribe(base, node, ("index", key), js_transfer.indexed(self, key, True))
            self.subscribe(key, node, ("index", base), js_transfer.indexed(self, base, False))
        elif kind in STEPS:
            self.subscribe(self.node_of(tree[1], unit), node, ("step", STEPS[kind]), transfer.steps(self, STEPS[kind]))
        elif kind in ("op", "unary") and unit.lang == "js":    # a number, a BigInt or a string (O1, T1'')
            self.add(node, [(v, unit) for v in truth.operator_values(tree[1], kind == "unary")])
        elif kind in ("op", "unary"):
            self.subscribe(self.node_of(tree[2], unit), node, ("op", tree[1]), transfer.operators(self, tree[1]))
        elif kind == "construct":                 # JS `new X(...)`: X evaluated (N1)
            self.subscribe(self.node_of(tree[1], unit), node, ("construct",), js_transfer.constructs(self))
        elif kind == "keys":                      # JS `for (k in o)`: o's enumerable property names (P2)
            self.subscribe(self.node_of(tree[1], unit), node, ("own", "in"), js_transfer.owns(self, "in"))
        elif kind == "union":
            self.follow(self.node_of(tree[1], unit), node)
            self.follow(self.node_of(tree[2], unit), node)
        elif kind == "collection":                # JS: an array of the language holding what is written (T2''); else
            self.add(node, [(js_library.EMPTY_ARRAY if unit.lang == "js" else ENGINE, unit)])   # the engine's
            for value in tree[1]:
                self.subscribe(self.node_of(value, unit), node, ("elem",), transfer.elements(self))
        elif kind == "put":
            self.subscribe(self.node_of(tree[1], unit), node, ("elem",), transfer.elements(self))
        elif kind == "addr":
            self.follow(self.node_of(tree[1], unit), node)
        elif kind == "await":                     # what the value resolves to (a JS promise's type argument, T2)
            self.subscribe(self.node_of(tree[1], unit), node, ("await",), transfer.awaits(self))
        elif kind in CONSTANT_SHAPES:
            self.add(node, self.constant(tree, unit))
        elif kind == "self":                      # the objects the method is called on (J2')
            self.follow(receivers.this_node(self, unit), node)
        elif kind == "narrow":                    # a name read where a condition proved something of it (N1)
            narrow_values.wire(self, node, tree, unit)
        elif kind == "literal" and unit.lang == "js" and tree[1] in truth.JS_CONSTANTS:
            self.add(node, [(truth.JS_CONSTANTS[tree[1]], unit)])    # `true`, `false`, `null`, `undefined` (V1)
        elif kind == "literal" and unit.lang == "js" and tree[1] in truth.LITERAL_VALUES:
            self.add(node, [(v, unit) for v in truth.LITERAL_VALUES[tree[1]]])     # a number, a string, ... (T1'')
        else:
            self.add(node, [(ENGINE, unit)])      # literal, node: the engine's or the language's values

    def constant(self, tree, unit) -> list[tuple]:
        kind = tree[0]
        if kind in ("cast", "new"):
            return [(tree[1], unit)] if tree[1] else [(ENGINE, unit)]
        if kind == "callable":                    # in a template its free variables read: that template's (C4')
            return [(self.closure_value(self.ix.offset[unit.file] + tree[1], unit), unit)]
        if kind == "super":
            key = self.class_of(unit)
            bases = self.ix.base_keys(self.ix.classes[key]) if key else []
            return [(CLASS + bases[0], unit)] if bases else [(ENGINE, unit)]
        if kind == "strname":
            return [(TEXT + tree[1], unit)]
        key = self.ix.js.resolve(unit.file, tree[1])                     # import("m"): the namespace (J3)
        return [(CLASS + key, unit)] if key else [(ENGINE, unit)]

    def expand(self, text: str, node) -> None:
        """A type alias where a value would be: the values its type admits, read where it is declared (T1')."""
        self.follow(self.hold_node(self.ix.units[int(text[len(ALIAS):])]), node)

    def stepped(self, base, step: str):
        node = ("#step", base, step)
        if node not in self.wired:
            self.wired.add(node)
            self.subscribe(base, node, ("step", step), transfer.steps(self, step))
        return node

    def wire_call(self, node, callee, args, unit) -> None:
        read = js_library.object_read(callee) if unit.lang == "js" and args else None
        if read and "Object" not in unit.types and not self.rs.state_targets(unit, "Object"):
            if read == js_library.OBJECT_CREATE:            # an object whose members are p's (N1'')
                self.subscribe(self.node_of(args[0], unit), node, ("created",), js_transfer.created(self))
            else:
                self.subscribe(self.node_of(args[0], unit), node, ("own", read), js_transfer.owns(self, read))  # P3
            return
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
            self.subscribe(base, node, ("member", callee[2], True), transfer.members(self, callee[2], True, unit, args))
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
        self.subscribe(self.node_of(callee, unit), node, ("call",), transfer.calls(self, unit, args))

    def compute_whole(self, node, compute) -> None:
        consumer = (COMPUTED, node)
        self.computed[consumer] = (node, compute)
        self.recompute(consumer)

    def recompute(self, consumer) -> None:
        """A consumer value computed again whole from what it reads (its value only grows)."""
        node, compute = self.computed[consumer]
        self.dirty.discard(consumer)
        self.grown.pop(consumer, None)         # computed again whole: nothing is owed to it
        self.extra.pop(consumer, None)
        outer, self.substitution = self.substitution, None    # whole, even inside a reference linked by difference
        self.stack.append(consumer)
        try:
            values = compute()
        finally:
            self.stack.pop()
            self.substitution = outer
        self.add(node, values)

    def member(self, text: str, ctx, name: str, called: bool, depth: int = 0) -> list[tuple]:
        """A member of one value (for the consumers that need it whole)."""
        return self.values_of(self.member_node(text, ctx, name, called))

    def member_node(self, text: str, ctx, name: str, called: bool = False):
        """The node of a member of one value, wired once."""
        node = ("#member", text, ctx.gid, name, called)
        if node not in self.wired:
            self.wired.add(node)
            self.value.setdefault(node, [])
            transfer.member_one(self, node, text, ctx, name, called)
        return node
