"""The values every expression can hold, wired from the facts: a name read holds what its definition holds, a member
read what the object's class declares under that name, a call what the function called returns. No object is told
from another of its class. What the scopes and the declared types cannot say is then found by name, said so in every
link, and carries functions only: a member of an object nothing is known of is a function of that name of some class
(`scopes.by_name`) or one written under that name.
"""
from __future__ import annotations

from .calls import Calls
from .model import spelled, text, unquoted
from .reads import Reads
from .solver import Solver, marked
from .stores import Stores
from .vocab import (ARGS, AS_NAME, ASSIGN, CALL, CALLEE, CAST, CHAIN, CLASS, CLS, CONSTRUCT, EXACT, EXPR, FN,
                    FUNCTION, IDENT, IMPORT, INDEX, INST, KEY, LEFT, LITERAL, MEMBER, NAME, NEW, OBJECT, OUT,
                    OUTSIDE, PART, PROP, PROPERTY, PROPS, RET, RETURN, RIGHT, SCOPES, SELF, SOURCE, STEP, STORED,
                    SUPER, TYPE, VALUE, VAR)


class Flow:
    def __init__(self, program, scopes):
        self.p, self.sc, self.s = program, scopes, Solver()
        self.st, self.calls, self.rd = Stores(self), Calls(self), Reads(self)
        self.seen, self.queue = set(), []
        self.closed = set()          # nodes holding what the code read cannot define (a base it does not write)
        self.callees = set()         # the nodes read to be called
        self.wires = {IDENT: self.ident, SELF: self.this, SUPER: self.base, LITERAL: self.literal, MEMBER: self.member,
                      INDEX: self.index, CHAIN: self.chain, CALL: self.call, NEW: self.new, IMPORT: self.imported,
                      CAST: self.cast}

    # ---- expressions ---------------------------------------------------------------------------------------------
    def ev(self, f, node) -> tuple:
        """The flow node of an expression, wired the first time it is asked for."""
        key = (EXPR, f.index, node.id)
        if key not in self.seen:
            self.seen.add(key)
            self.queue.append((f, node))
        return key

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
        ids, how = self.sc.lookup(f, node, text(node))
        for d in ids:
            self.give(d, key, how, f, node)

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
        """A value written in place: an object of a type the language defines, holding its elements."""
        self.s.add(key, OUTSIDE)
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
        """An entry read: by a written name, the member of that name; by anything else, what the object holds."""
        name, target = unquoted(parts[KEY]), self.ev(f, parts[OBJECT])
        if name is None:
            self.s.follow(target, key)
        else:
            self.rd.read(f, node, target, name, key)

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
                    self.s.follow(mid, at)
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
        if role[0] == MEMBER:
            receiver, places = self.ev(f, role[1][OBJECT]), self.st.place(f, role[1][OBJECT])
            direct = f.is_a(role[1][OBJECT], MEMBER) or f.is_a(role[1][OBJECT], INDEX)
            name = self.p.word(f, role[1][PROPERTY])
        self.calls.site(f, node, CALL, self.ev(f, callee), self.given(parts), key, text(callee), receiver, places,
                        parts.get(PROPS), role[0] == SUPER, direct, name)

    def new(self, f, node, parts, key) -> None:
        self.calls.site(f, node, NEW, self.ev(f, parts[CALLEE]), self.given(parts), key, text(parts[CALLEE]))

    @staticmethod
    def given(parts) -> list:
        """The arguments a call writes, in order."""
        return parts[ARGS].named_children if ARGS in parts else []

    def imported(self, f, node, parts, key) -> None:
        for m in self.sc.sources.resolve(f, spelled(parts[SOURCE])) if SOURCE in parts else ():
            self.s.add(key, (CLS, m, EXACT))

    # ---- the whole program ---------------------------------------------------------------------------------------
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
            names = [one.named_children[0] for one in parts[ARGS].named_children]      # each one name: a value
            self.calls.site(f, d.node, NEW, self.ev(f, parts[TYPE]), names, at, text(parts[TYPE]))
            self.s.follow(at, target)
        if TYPE in parts:
            self.typed(f, parts[TYPE], target)

    def typed(self, f, node, target) -> None:
        """A declared type: an object of each class it names, of what each variable standing for a type holds, and,
        where a name in it is none the code read defines, an object of a type not defined here."""
        classes, stored, missing = self.sc.types(f, node)
        for c, how in classes:
            self.s.add(target, (INST, c, how))
        for d in stored:
            self.s.watch((VAR, d), lambda v: self.s.add(target, (INST, v[1], v[2]) if v[0] == CLS else v)
                         if v[0] in (CLS, INST, OUT) and (v[0] != CLS or self.p.is_class(v[1])) else None)
        if missing:
            self.s.add(target, OUTSIDE)

    def build(self) -> None:
        for d in self.p.defs:
            self.define(d)
        for f in self.p.files:
            for node, parts in f.facts.get(ASSIGN, ()):
                self.st.store(self.st.place(f, parts[LEFT]), self.ev(f, parts[RIGHT]), f, node)
            for node, parts in f.facts.get(RETURN, ()):
                fn = self.sc.function_at(f, node)
                if fn is not None and VALUE in parts:
                    self.s.follow(self.ev(f, parts[VALUE]), (RET, fn))
            for parts in f.pairs.values():       # a literal's named entry is kept under its name
                self.s.follow(self.ev(f, parts[VALUE]), self.prop(f, spelled(parts[KEY])), AS_NAME)
            for role in (CALL, NEW, CHAIN):
                for node, _ in f.facts.get(role, ()):
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
        self.st.unknown()
        while True:
            waiting = self.rd.unknown(self.closed)
            if not waiting:
                break
            self.drain()
            say(f"{len(waiting)} member reads of objects nothing is known of, found by name: {count()}")
        for site in list(self.calls.sites):
            if not site.exact:
                self.calls.hand(site)
        self.drain()
        self.st.unknown()                # what the calls handed over was written too
        self.drain()
        return self.calls.result()
