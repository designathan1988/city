"""Where a value is kept: the place an expression names when it is written to (a variable, a member of an object, an
entry of a container) and the flow of a value into it. A member written goes to the member the object's class
declares; where no class of the object declares it (or nothing is known of the object), its functions are kept under
the property's name, for the code of that language that reads the name knowing no more.
"""
from __future__ import annotations

from dataclasses import dataclass

from .model import text, unquoted, child_nodes
from .solver import marked
from .vocab import (ARGS, AS_NAME, CALL, CHAIN, CLS, CONSTRUCT, ELEM, EXPR, IDENT, INDEX, INST, KEY, MEMBER, OBJECT, OUT,
                    PART, PROP, PROPERTY, STEP, STORED, VAR, WRITES, HANDED, FN)


@dataclass(slots=True)
class Write:
    """A member written: the node holding the object, the name, the node holding what is written."""
    target: tuple
    name: str
    value: tuple
    f: object
    mark: str | None = None          # what the edge into the place carries (`vocab`)
    typed: bool = False              # the object is one of a class or a scope
    opened: bool = False             # kept under its name


class Stores:
    def __init__(self, flow):
        self.fl = flow
        self.writes = []

    def place(self, f, node) -> list:
        """The places an expression names: (VAR, definition) or (PROP, the object's node key, the property)."""
        fl = self.fl
        while node.id in f.named:
            node = f.named[node.id]
        role = f.role(node)
        if role is None:
            kids = child_nodes(node)
            return self.place(f, kids[0]) if len(kids) == 1 else []
        kind, parts = role
        if kind == IDENT:
            return [(VAR, i) for i in fl.sc.lookup(f, node, text(node))[0] if fl.p.defs[i].kind in STORED]
        if kind == MEMBER:
            return [(PROP,) + fl.spine(f, node, parts)]
        if kind == INDEX:
            name = unquoted(parts[KEY])
            if name is None:                 # an element: of a shape, its elements; of anything else, the object
                return [(ELEM, fl.ev(f, parts[OBJECT]), tuple(self.place(f, parts[OBJECT])))]
            return [(PROP, fl.ev(f, parts[OBJECT]), name)]
        return self.upto(f, node, len(child_nodes(node)) - 2) if kind == CHAIN else []

    def upto(self, f, node, j: int) -> list:
        """The place a chain names up to its step `j` (before its first step: its object's)."""
        kids = child_nodes(node)
        if j < 0:
            return self.place(f, kids[0])
        parts = f.role(kids[j + 1])[1] if f.is_a(kids[j + 1], STEP) else {}
        if ARGS in parts or CONSTRUCT in parts or PROPERTY not in parts:
            return []
        before = self.fl.ev(f, kids[0]) if j == 0 else (EXPR, f.index, kids[j].id)
        name = unquoted(parts[KEY]) if KEY in parts else None
        if name is not None:
            return [(PROP, (PART, f.index, kids[j + 1].id), name)]
        return [(PROP, before, text(parts[PROPERTY]))]

    def store(self, places: list, value, f, node, mark=None) -> None:
        """Every value of a node is kept in those places, as the edge `mark` carries it (no value: the places are
        written, as a step up or down writes them)."""
        for place in places:
            if place[0] == VAR:
                if mark is None and self.fl.is_state(place[1]):      # written by the program (not kept by it)
                    self.fl.touch(self.fl.sc.unit(f, node), place[1], WRITES)
                if value is not None:
                    self.fl.s.follow(value, (VAR, place[1]), mark)
            elif place[0] == ELEM:
                self.element(place[1], list(place[2]), value, f, node, mark)
            else:
                self.write(place[1], place[2], value, f, node, mark)

    def element(self, target, places: list, value, f, node, mark) -> None:
        """An element written by a key not written there: in a shape, among its elements; in anything else, in the
        places the object is kept (a list holds what is put in it)."""
        fl, done = self.fl, []

        def seen(v: tuple) -> None:
            if value is None:
                return
            if v[0] == INST and fl.p.is_shape(v[1]):
                fl.s.follow(value, (ELEM, v[1]), mark)
            elif not done:
                done.append(v)
                self.store(places, value, f, node, mark)
        fl.s.watch(target, seen)

    def write(self, target, name: str, value, f, node, mark) -> None:
        """A member written: in the member each class of the object declares (a setter runs); under its name where
        a class of the object declares none, or the object is of a type not defined here."""
        fl = self.fl
        w = Write(target, name, value, f, mark)
        self.writes.append(w)

        def seen(v: tuple) -> None:
            if value is not None and v[0] == OUT:      # a function written into the platform's object: registered
                fl.calls.handed_to_platform(f, value)
                unit = fl.sc.unit(f, node)
                fl.s.watch(value, lambda u, unit=unit: fl.calls.link(unit, u[1], HANDED, u[2], f, node)
                           if u[0] == FN else None)
            if value is not None and fl.calls.stands_for_file(v) and v[1] != f.module:
                fl.calls.handed_to_file(v[1], value)
                return
            if v[0] == OUT:
                entry = fl.built(fl.holder_of(target), f, node, name)
                if entry is not None:            # a record built member by member: its entry
                    w.typed = True
                    if mark is None:
                        fl.touch(fl.sc.unit(f, node), entry, WRITES)
                    if value is not None:
                        fl.s.follow(value, (VAR, entry), mark)
                    return
                self.loose(w)
            if v[0] not in (INST, CLS):
                return
            w.typed, declared = True, False
            for d in fl.sc.members(v[1], name):
                it = fl.p.defs[d]
                if it.kind in STORED:
                    declared = True
                    if mark is None and (fl.is_state(d) or fl.p.is_shape(it.owner)):
                        fl.touch(fl.sc.unit(f, node), d, WRITES)
                    if value is not None:
                        fl.s.follow(value, (VAR, d), mark or marked(v[2]))
                elif it.setter:
                    declared = True
                    fl.calls.link(fl.sc.unit(f, node), d, CALL, v[2], f, node)
                    for p in fl.sc.params[d].get(0, ()):
                        if value is not None:
                            fl.s.follow(value, (VAR, p), mark or marked(v[2]))
            if not declared:
                self.loose(w)
        fl.s.watch(target, seen)

    def loose(self, w: Write) -> None:
        """Keeps what is written under the property's name."""
        if not w.opened and w.value is not None:
            w.opened = True
            value = w.value
            if w.mark is not None:           # as the edge carries it first, then as found by name
                value = (PART, PROP, len(self.writes), id(w))
                self.fl.s.follow(w.value, value, w.mark)
            self.fl.s.follow(value, self.fl.prop(w.f, w.name), AS_NAME)

    def unknown(self) -> None:
        """The members written to objects nothing is known of are kept under their names."""
        for w in self.writes:
            if not w.typed:
                self.loose(w)
