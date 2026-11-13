"""The values every expression can hold, wired from the facts: a name read holds what its definition holds, a member
read what the object's class declares under that name, a call what the function called returns. No object is told
from another of its class. Where types are declared, a place declared as a value the language writes in place (a
number, a text) holds no object of a class, whatever reaches it. An object written in place is an object of its own shape; a name no scope gives is the
platform's object (none of its members is the code's), or, where the language's project file makes it a global, an
object of the file it names. What code not read is given comes back from it: a call no definition answers gives back
the objects it was given and called on, and runs the functions it is handed with them. What the scopes, the declared
types and these cannot say is then found by name, said so in every link, and carries functions only: a member of an
object nothing is known of is a function of that name of some class (`scopes.by_name`) or one written under that
name to an object nothing was known of.
"""
from __future__ import annotations

from collections import defaultdict

from .calls import Calls
from .conditions import weigh
from .configs import norm
from .sources import RELATIVE
from .model import named, spelled, text, unquoted, child_nodes
from .reads import Reads
from .solver import Solver, marked
from .stores import Stores
from .vocab import (ARGS, ASSIGN, BY_NAME, BY_TEXT, CALL, CALLEE, CAST, CHAIN, CLASS, CLS, CONSTRUCT, EXACT, EXPR, FN,
                    FUNCTION, IDENT, IMPORT, INDEX, INST, KEY, LEFT, LITERAL, MEMBER, MODULE, NAME, NAMESPACE, NEW,
                    NODE, OBJECT, OPERATE, OUT, OUTSIDE, PARAM, PART, PLATFORM, PROP, PROPERTY, PROPS, READS, RET, RETURN,
                    RIGHT, SCOPES, SELF, SHAPE, SOURCE, STEP, STORED, SUPER, TOUCHES, TXT, TYPE, VALUE, VAR, VARIABLE,
                    ELEM, HANDLERS)


class Flow:
    def __init__(self, program, scopes, among: int | None = None, keep: bool = False, ceiling=None, compiled=None):
        self.p, self.sc, self.s = program, scopes, Solver()
        self.compiled = compiled or {}   # (file, start byte, end byte) of a call -> its compiler's answer
        self.keep = keep                 # keep the files' facts after the analysis (what evaluates the program reads)
        self.ceiling = ceiling
        self.s.check = ceiling.over if ceiling is not None else None
        self.st, self.calls, self.rd = Stores(self), Calls(self), Reads(self, among)
        self.seen, self.queue = set(), []
        self.node_of = {}            # an expression's flow node -> (its file, the node)
        self.active, self.waiting = set(), {}    # the shapes holding a function; the nodes waiting for one
        self.touched = {}            # (unit, definition) -> what the unit does to it (`TOUCHES`)
        self.read_by = set()         # (unit, definition) read, whatever else the unit does to it (a write is stronger)
        self.kept_in = defaultdict(set)  # an expression's node -> the state definitions its value is read from
        self.builts = {}             # a variable written member by member -> the shape it builds
        self.closed = set()          # nodes holding what the code read cannot define (a base it does not write)
        self.callees = set()         # the nodes read to be called
        self.tables = defaultdict(set)   # unit -> the shapes it reads an entry of by a key not written there
        self.wires = {IDENT: self.ident, SELF: self.this, SUPER: self.base, LITERAL: self.literal, MEMBER: self.member,
                      INDEX: self.index, CHAIN: self.chain, CALL: self.call, NEW: self.new, IMPORT: self.imported,
                      CAST: self.cast, OPERATE: self.operate, NODE: self.node}

    # ---- expressions ---------------------------------------------------------------------------------------------
    def ev(self, f, node) -> tuple:
        """The flow node of an expression, wired the first time it is asked for."""
        key = (EXPR, f.index, node.id)
        if key not in self.seen:
            self.seen.add(key)
            self.queue.append((f, node))
            self.node_of[key] = (f, node)
        return key

    def is_state(self, d: int) -> bool:
        """State: a variable or a field declared at the level of a class, a file or a namespace (not in a function,
        not an entry of an object written in place)."""
        it = self.p.defs[d]
        if it.kind not in STORED or it.kind == PARAM or it.owner < 0 or HANDLERS in it.parts:
            return False
        owner = self.p.defs[it.owner]
        return owner.kind in (CLASS, MODULE, NAMESPACE)

    def touch(self, unit: int, d: int, how: str) -> None:
        """A unit's code does that to a definition (state, or an entry of a record, which carries state on)."""
        key = (unit, d)
        old = self.touched.get(key)
        if old is None or TOUCHES.index(how) < TOUCHES.index(old):
            self.touched[key] = how
        if how == READS:
            self.read_by.add(key)

    def read_state(self, f, node, d: int, key) -> None:
        if self.is_state(d) or self.p.is_shape(self.p.defs[d].owner):
            self.touch(self.sc.unit(f, node), d, READS)
            self.kept_in[key].add(d)

    def built(self, holder, f, node, name: str) -> int | None:
        """The entry `name` of the record a variable of the function around it is built as, member by member with
        names written there (an object of a type not defined here, or a dictionary written in place, filled in)."""
        if holder[0] != VAR:
            return None
        d = self.p.defs[holder[1]]
        if d.kind not in STORED or d.owner < 0 or self.p.defs[d.owner].kind != FUNCTION:
            return None
        if holder not in self.builts:
            c = self.p.new(SHAPE, "", self.p.files[d.file], d.node)
            c.owner = d.owner
            self.p.labels.append(self.p.label_of(c))
            self.builts[holder] = c.id
            self.s.add(holder, (INST, c.id, EXACT))
        c = self.builts[holder]
        held = self.p.names[c].get(name)
        if held:
            return held[0]
        e = self.p.new(VARIABLE, name, f, node)
        e.owner = c
        self.p.names[c][name].append(e.id)
        self.p.labels.append(self.p.label_of(e))
        self.sc.entries_of.pop(c, None)
        return e.id

    def holder_of(self, key):
        """What holds the object a node holds, for the members read of it to be counted together: the variable a
        name reads, else the node itself."""
        found = self.node_of.get(key)
        if found is None or not found[0].is_a(found[1], IDENT):
            return key
        ids = [d for d in self.sc.lookup(found[0], found[1], text(found[1]))[0] if self.p.defs[d].kind in STORED]
        return (VAR, ids[0]) if len(ids) == 1 else key

    @staticmethod
    def prop(f, name: str) -> tuple:
        """The flow node of what is kept under a property's name, for the code of one language."""
        return (PROP, f.lang.package, name)

    def drain(self) -> None:
        while (self.queue or self.s.work) and not self.s.stopped:
            while self.queue:
                self.wire(*self.queue.pop())
            self.s.run()

    def wire(self, f, node) -> None:
        key = (EXPR, f.index, node.id)
        at = self.p.at.get((f.index, node.id))
        kind = self.p.defs[at].kind if at is not None else None
        role = f.role(node)
        if kind in (FUNCTION, CLASS):
            self.give(at, key, EXACT)
        elif kind == SHAPE and (role is None or role[0] == LITERAL):
            self.shape(f, node, at, key)
        elif role is not None and role[0] in self.wires:
            self.wires[role[0]](f, node, role[1], key)
        elif kind in STORED:
            self.s.follow((VAR, at), key)
        elif node.id in f.named:
            self.s.follow(self.ev(f, f.named[node.id]), key)
        elif node.id in f.pairs:
            self.s.follow(self.ev(f, f.pairs[node.id][VALUE]), key)
        else:                                            # any other node holds only what a pattern says it does
            for part in f.passes.get(node.id, ()):
                self.s.follow(self.ev(f, part), key)

    def give(self, d: int, key, how: str, f=None, node=None) -> None:
        """What reading a definition gives: a function, a scope, a variable's values; a getter read runs."""
        it = self.p.defs[d]
        if it.kind == FUNCTION and it.getter and node is not None:
            self.calls.link(self.sc.unit(f, node), d, CALL, how, f, node)
            self.s.follow((RET, d), key, marked(how))
        elif it.kind == FUNCTION:
            if not it.setter:
                self.s.add(key, (FN, d, how))
        elif it.kind in SCOPES:
            self.s.add(key, (CLS, d, how))
        else:
            self.s.follow((VAR, d), key, marked(how))

    def ident(self, f, node, parts, key) -> None:
        """A name: what its definition gives; else a global the language's project file declares, an object of the
        file it names; else, where members are looked up as the program runs, a class of that name another language
        read defines for the platform to run; else the platform's object."""
        name = text(node)
        ids, how = self.sc.lookup(f, node, name)
        for d in ids:
            self.give(d, key, how, f, node)
            self.read_state(f, node, d, key)
        if ids:
            return
        module = self.p.global_object(f, name)
        if module is not None:
            self.s.add(key, (INST, module, EXACT))
            return
        foreign = self.sc.foreign(name, f.lang)
        for c in foreign:
            self.s.add(key, (CLS, c, BY_NAME))
        if not foreign:
            self.s.add(key, PLATFORM)

    def node(self, f, node, parts, key) -> None:
        """A node of the scene by its path or unique name written in place: an object of the script the scene runs
        there (by text); else an object of a type not defined here."""
        written = text(node).lstrip()
        mark = written[:1]
        found = self.scene_node(f, written[1:].strip("\"'"), mark == f.lang.node_marks[-1:])
        self.s.add(key, (INST, found, BY_TEXT) if found is not None else OUTSIDE)

    def scene_node(self, f, path: str, unique: bool = False) -> int | None:
        """The module of the script a scene runs at a node, by its path from the node running this file's script
        (or its unique name in the scene)."""
        if f.path is None or not f.lang.project:
            return None
        project = self.p.configs.of(f.lang.project, f.path)
        table = (project.unique if unique else project.nodes).get(norm(f.path), {})
        script = table.get(path)
        return self.sc.sources.module_at(script) if script is not None else None

    def shape(self, f, node, at, key) -> None:
        """An object written in place: an object of a type the language defines and, once one of its entries holds a
        function or a function's name (a method, a callback, a command's name), an object of its own shape too (one
        holding data alone is told from no other: it would only join the flows of every object of data). What it
        spreads in, it holds as well."""
        self.s.add(key, OUTSIDE)
        self.shaped(at, key)
        for child in child_nodes(node):
            if child.id in f.passes:
                self.s.follow(self.ev(f, child), key)

    def shaped(self, c: int, key) -> None:
        """`key` holds an object of the shape `c` once the shape holds a function or a function's name: in an entry
        written there, or among its elements, written by a key not written there (a table of callables filled as the
        program runs)."""
        if c in self.active:
            self.s.add(key, (INST, c, EXACT))
            return
        if c in self.waiting:
            self.waiting[c].append(key)
            return
        self.waiting[c] = [key]
        for entry in self.sc.entries(c):
            if self.p.defs[entry].kind == FUNCTION:
                self.activate(c)
                return
        self.s.watch((ELEM, c), lambda v, c=c: self.activate(c)
                     if v[0] in (FN, TXT) or (v[0] == INST and self.p.is_shape(v[1])) else None)
        for entry in self.sc.entries(c):
            self.s.watch((VAR, entry), lambda v, c=c: self.activate(c)          # a function, a function's name,
                         if v[0] in (FN, TXT) or (v[0] == INST and self.p.is_shape(v[1])) else None)  # or an object
            #                                                                    carrying them (a shape is one once
            #                                                                    it holds one)

    def activate(self, c: int) -> None:
        if c in self.active:
            return
        self.active.add(c)
        for key in self.waiting.pop(c, ()):
            self.s.add(key, (INST, c, EXACT))

    def this(self, f, node, parts, key) -> None:
        c = self.sc.class_around(f, node)
        if c is not None:
            self.s.add(key, (INST, c, EXACT))

    def base(self, f, node, parts, key) -> None:
        c = self.sc.class_around(f, node)
        bases = self.sc.bases(c) if c is not None else []
        for b in bases:
            self.s.add(key, (CLS, b, EXACT))
        if not bases:
            self.closed.add(key)

    def literal(self, f, node, parts, key) -> None:
        """A value written in place: an object of a type the language defines, holding its elements; a text, the
        function's or class's name it writes, or the file read its relative path names (by text)."""
        self.s.add(key, OUTSIDE)
        if named(node) in self.p.callable:
            self.s.add(key, (TXT, named(node), EXACT))
        written = unquoted(node)
        if written is not None and written.startswith(RELATIVE):     # a file's path, written from this one
            for m in self.sc.sources.resolve(f, written):
                self.s.add(key, (CLS, m, BY_TEXT))
        for child in child_nodes(node):
            self.s.follow(self.ev(f, child), key)

    def cast(self, f, node, parts, key) -> None:
        """A value read as a type: what the value holds, and an object of that type."""
        if VALUE in parts:
            self.s.follow(self.ev(f, parts[VALUE]), key)
        if TYPE in parts:
            self.typed(f, parts[TYPE], key)

    def spine(self, f, node, parts) -> tuple:
        """A member read's object and name; a name written under scopes after the object (`a::b::c`) reads each."""
        target, prop = self.ev(f, parts[OBJECT]), parts[PROPERTY]
        while f.is_a(prop, MEMBER):
            inner, mid = f.role(prop)[1], (PART, f.index, prop.id)
            if mid not in self.seen:
                self.seen.add(mid)
                self.rd.read(f, node, target, self.p.word(f, inner[OBJECT]), mid)
            target, prop = mid, inner[PROPERTY]
        return target, self.p.word(f, prop)

    def member(self, f, node, parts, key) -> None:
        self.rd.read(f, node, *self.spine(f, node, parts), key)

    def index(self, f, node, parts, key) -> None:
        """An entry read: by a written name, the member of that name; by anything else, of a shape every entry (the
        read noted, for a trace to follow only the entries of the keys its caller gave), of anything else what the
        object holds."""
        name, target = unquoted(parts[KEY]), self.ev(f, parts[OBJECT])
        if name is not None:
            self.rd.read(f, node, target, name, key)
        else:
            self.element(f, node, target, key)

    def element(self, f, node, target, key) -> None:
        """An element read by a key not written there: of a shape every entry and element (the read noted, for a
        trace to follow only the entries of the keys its caller gave); of anything else what it holds."""
        unit = self.sc.unit(f, node)

        def seen(v: tuple) -> None:
            if v[0] == INST and self.p.is_shape(v[1]):
                self.tables[unit].add(v[1])
                for entry in self.sc.entries(v[1]):
                    self.give(entry, key, v[2], f, node)
                self.s.follow((ELEM, v[1]), key)
            else:
                self.s.add(key, v)
        self.s.watch(target, seen)

    def operate(self, f, node, parts, key) -> None:
        """An operator applied to an object: where the object's class writes the function the language names for
        that operator, a call of it with the other operand."""
        if not f.lang.operators or OBJECT not in parts or PROPERTY not in parts:
            return
        target, name, made = self.ev(f, parts[OBJECT]), f.lang.operators + text(parts[PROPERTY]), []

        def seen(v: tuple) -> None:
            if made or v[0] not in (INST, CLS) or not self.sc.members(v[1], name, v[0] == INST):
                return
            made.append(v)
            mid = (PART, f.index, node.id)
            self.callees.add(mid)
            self.rd.read(f, node, target, name, mid)
            self.calls.site(f, node, CALL, mid, [parts[ARGS]] if ARGS in parts else [], key, text(node), target,
                            name=name)
        self.s.watch(target, seen)

    def chain(self, f, node, parts, key) -> None:
        kids = child_nodes(node)
        before = self.ev(f, kids[0])
        for j, step in enumerate(kids[1:]):
            if not f.is_a(step, STEP):
                continue
            sp, at, mid = f.role(step)[1], (EXPR, f.index, step.id), (PART, f.index, step.id)
            self.seen.add(at)
            shown = text(node)[:sp[PROPERTY].end_byte - node.start_byte] if PROPERTY in sp else text(step)
            if CONSTRUCT in sp:
                self.calls.site(f, step, NEW, before, self.given(sp), at, shown)
            elif ARGS in sp:
                self.callees.add(mid)
                self.rd.read(f, step, before, text(sp[PROPERTY]), mid)
                self.calls.site(f, step, CALL, mid, self.given(sp), at, shown, before, self.st.upto(f, node, j - 1),
                                direct=j > 0 or f.is_a(kids[0], INDEX), name=text(sp[PROPERTY]))
            elif KEY in sp:                  # a property, then its entry of that name or one of its elements
                self.rd.read(f, step, before, text(sp[PROPERTY]), mid)
                if unquoted(sp[KEY]) is None:
                    self.element(f, step, mid, at)
                else:
                    self.rd.read(f, step, mid, unquoted(sp[KEY]), at)
            else:
                self.rd.read(f, step, before, text(sp[PROPERTY]), at)
            before = at
        self.s.follow(before, key)

    def call(self, f, node, parts, key) -> None:
        callee = parts[CALLEE]
        role = f.role(callee) or ("", None)
        self.callees.add((EXPR, f.index, callee.id))
        receiver = places = None
        direct, name = False, ""
        around = self.sc.class_around(f, node) if role[0] == IDENT and f.lang.implicit_members else None
        if role[0] == MEMBER:
            receiver, places = self.ev(f, role[1][OBJECT]), self.st.place(f, role[1][OBJECT])
            direct = f.is_a(role[1][OBJECT], MEMBER) or f.is_a(role[1][OBJECT], INDEX)
            name = self.p.word(f, role[1][PROPERTY])
        self.calls.site(f, node, CALL, self.ev(f, callee), self.given(parts), key, text(callee), receiver, places,
                        parts.get(PROPS), role[0] == SUPER, direct, name, around, self.type_of(f, callee, parts))

    def new(self, f, node, parts, key) -> None:
        self.calls.site(f, node, NEW, self.ev(f, parts[CALLEE]), self.given(parts), key, text(parts[CALLEE]),
                        typed=self.type_of(f, parts[CALLEE], parts))

    @staticmethod
    def type_of(f, callee, parts) -> list:
        """The types a call or a `new` is written with (`new A<B>()`, `f<B>()`): its type part, else the arguments a
        callee written as a name given arguments is written with."""
        if TYPE in parts:
            return [parts[TYPE]]
        while f.is_a(callee, MEMBER):
            callee = f.role(callee)[1][PROPERTY]
        if callee.id in f.named:
            inner = f.named[callee.id]
            return [c for c in child_nodes(callee) if c.id != inner.id]
        return []

    @staticmethod
    def given(parts) -> list:
        """The arguments a call writes, in order."""
        return child_nodes(parts[ARGS]) if ARGS in parts else []

    def imported(self, f, node, parts, key) -> None:
        for m in self.sc.sources.resolve(f, spelled(parts[SOURCE])) if SOURCE in parts else ():
            self.s.add(key, (CLS, m, EXACT))

    # ---- the whole program ---------------------------------------------------------------------------------------
    def declarator(self, d) -> None:
        """What lies between a variable's declaration and its name declares it: a call read there (a statement that
        can be a declaration is one, `T (*name)() = value` read as calls of T) runs nothing."""
        if d.kind not in STORED or NAME not in d.parts:
            return
        f, up = self.p.files[d.file], d.parts[NAME].parent
        while up is not None and up.id != d.node.id and d.node.start_byte <= up.start_byte:
            if f.is_a(up, CALL):
                self.seen.add((EXPR, f.index, up.id))
            up = up.parent

    def define(self, d) -> None:
        """What a definition holds from its own words: its value, the property it takes, its declared type."""
        f, parts = self.p.files[d.file], d.parts
        if d.kind not in STORED and d.kind != FUNCTION:
            return
        target = (RET if d.kind == FUNCTION else VAR, d.id)
        if d.kind in STORED and VALUE in parts:
            self.s.follow(self.ev(f, parts[VALUE]), target)
        if d.kind in STORED and OBJECT in parts and NAME in parts:
            self.rd.read(f, d.node, self.ev(f, parts[OBJECT]), spelled(parts.get(KEY, parts[NAME])), target)
        if d.remade:
            at = (EXPR, f.index, d.node.id)
            self.seen.add(at)
            names = [child_nodes(one)[0] for one in child_nodes(parts[ARGS])]      # each one name: a value
            self.calls.site(f, d.node, NEW, self.ev(f, parts[TYPE]), names, at, text(parts[TYPE]))
            self.s.follow(at, target)
        if TYPE in parts:
            if VALUE not in parts:           # declared with a value: its objects are the value's (the type bounds it)
                self.typed(f, parts[TYPE], target)
            if not f.lang.duck_typed:        # declared as values the language writes in place: no object of a class
                if not any(self.sc.types(f, parts[TYPE])):
                    self.s.plain.add(target)

    def typed(self, f, node, target) -> None:
        """A declared type: an object of each class it names, of what each variable standing for a type holds, and,
        where a name in it is none the code read defines, an object of a type not defined here."""
        classes, stored, missing, _ = self.sc.types(f, node)
        for c, how in classes:
            self.s.add(target, (INST, c, how))
        for d in stored:
            self.s.watch((VAR, d), lambda v: self.s.add(target, (INST, v[1], v[2]) if v[0] == CLS else v)
                         if v[0] in (CLS, INST, OUT) and (v[0] != CLS or self.p.is_class(v[1])) else None)
        if missing:
            self.s.add(target, OUTSIDE)

    def through(self) -> None:
        """What the calls no definition answers name by the texts they are given, give back and give the functions
        they are handed, run out (the texts first: a call answered by text is a call)."""
        while True:
            named = self.calls.texts()
            if named:
                self.drain()
                continue
            if not self.calls.through():
                break
            self.drain()

    def build(self) -> None:
        for d in self.p.defs:
            self.declarator(d)
        for d in self.p.defs:
            self.define(d)
        read_run = {d.name for d in self.p.defs if d.kind == FUNCTION and d.getter}
        for f in self.p.files:
            for node, parts in f.facts.get(ASSIGN, ()):      # (a step up or down writes what it names alone)
                value = self.ev(f, parts[RIGHT]) if RIGHT in parts else None
                self.st.store(self.st.place(f, parts[LEFT]), value, f, node)
            for node, parts in f.facts.get(RETURN, ()):
                fn = self.sc.function_at(f, node)
                if fn is not None and VALUE in parts:
                    self.s.follow(self.ev(f, parts[VALUE]), (RET, fn))
            for role in (CALL, NEW, CHAIN, OPERATE):        # (an operator applied for its effect calls too)
                for node, _ in f.facts.get(role, ()):
                    self.ev(f, node)
            for node, parts in f.facts.pop(MEMBER, ()):     # a read that runs a getter runs it, its value taken or not
                if PROPERTY in parts and text(parts[PROPERTY]) in read_run:
                    self.ev(f, node)

    def solve(self, say=lambda words: None):
        """Wires the program and runs it out; then, innermost first, widens each member read whose object is still
        unknown; then hands to the code not read what the calls no definition answers were given."""
        def count() -> str:
            return f"{sum(len(held) for held in self.s.values.values())} values, {len(self.calls.links)} links"
        self.build()
        say(f"wired {len(self.queue)} expressions to start from")
        self.drain()
        say(f"{len(self.seen)} expressions, {len(self.calls.sites)} calls, {len(self.rd.all)} member reads: {count()}")
        self.through()
        say(f"what code not read was given, given back: {count()}")
        self.st.unknown()
        while True:
            waiting = self.rd.unknown(self.closed)
            if not waiting:
                break
            self.drain()
            self.through()
            say(f"{len(waiting)} member reads of objects nothing is known of, found by name: {count()}")
        for site in list(self.calls.sites):
            if not site.exact:
                self.calls.hand(site)
        self.drain()
        self.through()
        self.st.unknown()                # what the calls handed over was written too
        self.drain()
        result = self.calls.result()
        say(f"handed over what was left: {count()}")
        result.cases = weigh(self)       # when each effect happens and what it writes, in its function's inputs
        say(f"weighed: {len(result.cases.effects)} effects, "
            f"{sum(len(rows) for rows in result.cases.passages.values())} passages")
        self.s.settle()                  # nothing more is carried: the edges and the watchers go (memory)
        for unit, d in result.cases.reads:       # state read where no value was asked of the flow: a read too
            result.state_out.setdefault(unit, {}).setdefault(d, READS)
            result.state_in.setdefault(d, {}).setdefault(unit, READS)
            result.read_in.setdefault(d, set()).add(unit)
        result.cut = self.ceiling.why if self.ceiling is not None and self.ceiling.why else ""
        result.sites = {(site.f.index, site.node.id): list(site.targets or ()) for site in self.calls.sites} \
            if self.keep else None
        if not self.keep:
            for f in self.p.files:       # what the files' facts were read for is done: let them go (memory)
                f.facts, f.roles, f.named, f.pairs, f.passes = {}, {}, {}, {}, {}
        return result
