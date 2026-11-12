"""The values every expression can hold, wired from the facts: a name read holds what its definition holds, a member
read what the object's class declares under that name, a call what the function called returns. No object is told
from another of its class. Where types are declared, a place declared as a value the language writes in place (a
number, a text) holds no object of a class, whatever reaches it. An object written in place is an object of its own shape; a name no scope gives is the
platform's object (none of its members is the code's), or, where the language's project file makes it a global, an
object of the file it names. What code not read is given comes back from it: a call no definition answers gives back
the objects it was given and called on, and runs the functions it is handed with them. What the scopes, the declared
types and these cannot say is then found by name, said so in every link, and carries functions only: a member of an
object nothing is known of is a function of that name of some class (`reads.by_name`) or one written under that
name to an object nothing was known of. The whole program's run is `run`'s; the objects written in place, `shapes`'.
"""
from __future__ import annotations

from collections import defaultdict

from .calls import Calls
from .model import named, spelled, text, unquoted
from .reads import Reads
from .shapes import Shapes
from .solver import Solver, marked
from .stores import Stores
from .vocab import (ARGS, BY_NAME, CALL, CALLEE, CAST, CHAIN, CLASS, CLS, CONSTRUCT, EXACT, EXPR, FN, FUNCTION, IDENT,
                    IMPORT, INDEX, INST, KEY, LITERAL, MEMBER, NEW, OBJECT, OPERATE, OUT, OUTSIDE, PART, PLATFORM, PROP,
                    PROPERTY, PROPS, RET, SCOPES, SELF, SHAPE, SOURCE, STEP, STORED, SUPER, TXT, TYPE, VALUE, VAR, ELEM)


class Flow:
    def __init__(self, program, scopes, among: int | None = None):
        self.p, self.sc, self.s = program, scopes, Solver()
        self.st, self.calls, self.rd, self.shapes = Stores(self), Calls(self), Reads(self, among), Shapes(self)
        self.seen, self.queue = set(), []
        self.node_of = {}            # an expression's flow node -> (its file, the node)
        self.closed = set()          # nodes holding what the code read cannot define (a base it does not write)
        self.callees = set()         # the nodes read to be called
        self.tables = defaultdict(set)   # unit -> the shapes it reads an entry of by a key not written there
        self.wires = {IDENT: self.ident, SELF: self.this, SUPER: self.base, LITERAL: self.literal, MEMBER: self.member,
                      INDEX: self.index, CHAIN: self.chain, CALL: self.call, NEW: self.new, IMPORT: self.imported,
                      CAST: self.cast, OPERATE: self.operate}

    # ---- expressions ---------------------------------------------------------------------------------------------
    def ev(self, f, node) -> tuple:
        """The flow node of an expression, wired the first time it is asked for."""
        key = (EXPR, f.index, node.id)
        if key not in self.seen:
            self.seen.add(key)
            self.queue.append((f, node))
            self.node_of[key] = (f, node)
        return key

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
        while self.queue or self.s.work:
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

    def shape(self, f, node, at, key) -> None:
        """An object written in place: an object of a type the language defines and, once one of its entries holds a
        function or a function's name (a method, a callback, a command's name), an object of its own shape too (one
        holding data alone is told from no other: it would only join the flows of every object of data). What it
        spreads in, it holds as well."""
        self.s.add(key, OUTSIDE)
        self.shapes.shaped(at, key)
        for child in node.named_children:
            if child.id in f.passes:
                self.s.follow(self.ev(f, child), key)

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
        functions' names written in it."""
        self.s.add(key, OUTSIDE)
        if named(node) in self.p.callable:
            self.s.add(key, (TXT, named(node), EXACT))
        for child in node.named_children:
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
        kids = node.named_children
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
            return [c for c in callee.named_children if c.id != inner.id]
        return []

    @staticmethod
    def given(parts) -> list:
        """The arguments a call writes, in order."""
        return parts[ARGS].named_children if ARGS in parts else []

    def imported(self, f, node, parts, key) -> None:
        for m in self.sc.sources.resolve(f, spelled(parts[SOURCE])) if SOURCE in parts else ():
            self.s.add(key, (CLS, m, EXACT))

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
