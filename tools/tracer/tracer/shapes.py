"""Objects written in place (a literal, a dictionary, an element's attributes), each of its own shape: whether a node
holds an object of a shape (once one of its entries holds a function or a function's name; one holding data alone is
told from no other, it would only join the flows of every object of data), and the functions an object of a shape hands
over (its entries', its entries' shapes'), gathered once for every call it is given to. Asked by the expressions
(`flow`) and by the calls (`calls`)."""
from __future__ import annotations

from .vocab import EXACT, FN, FUNCTION, INST, TXT, VAR


class Shapes:
    def __init__(self, flow):
        self.fl = flow
        self.active, self.waiting = set(), {}    # the shapes holding a function; the nodes waiting for one
        self.held, self.nested = {}, set()       # shape -> (its functions, who is told of them); shapes inside shapes

    def shaped(self, c: int, key) -> None:
        """`key` holds an object of the shape `c` once the shape holds a function or a function's name."""
        fl = self.fl
        if c in self.active:
            fl.s.add(key, (INST, c, EXACT))
            return
        if c in self.waiting:
            self.waiting[c].append(key)
            return
        self.waiting[c] = [key]
        for entry in fl.sc.entries(c):
            if fl.p.defs[entry].kind == FUNCTION:
                self.activate(c)
                return
        for entry in fl.sc.entries(c):
            fl.s.watch((VAR, entry), lambda v, c=c: self.activate(c) if v[0] in (FN, TXT) else None)

    def activate(self, c: int) -> None:
        if c in self.active:
            return
        self.active.add(c)
        for key in self.waiting.pop(c, ()):
            self.fl.s.add(key, (INST, c, EXACT))

    def functions_in(self, v: tuple, then, seen: set) -> None:
        """`then` of each function a value is, or an object written in place holds in its entries (the options an
        object of them hands over), as they come. A shape's functions are gathered once, for every call it is given
        to."""
        if v[0] == FN:
            then(v)
        elif v[0] == INST and self.fl.p.is_shape(v[1]):
            found, takers = self.gathered(v[1])
            takers.append(then)
            for one in list(found):
                then(one)

    def gathered(self, c: int) -> tuple:
        """A shape's functions as they come (its entries', its entries' shapes'), and who is told of each."""
        fl = self.fl
        if c not in self.held:
            self.held[c] = ([], [])
            for entry in fl.sc.entries(c):
                if fl.p.defs[entry].kind == FUNCTION:
                    self.gather(c, (FN, entry, EXACT))
                else:
                    fl.s.watch((VAR, entry), lambda w, c=c: self.gather(c, w))
        return self.held[c]

    def gather(self, c: int, v: tuple) -> None:
        if v[0] == INST and self.fl.p.is_shape(v[1]) and v[1] != c:
            if (c, v[1]) not in self.nested:
                self.nested.add((c, v[1]))
                self.functions_in(v, lambda w, c=c: self.gather(c, w), set())
            return
        if v[0] != FN:
            return
        found, takers = self.held[c]
        if v in found:
            return
        found.append(v)
        for then in list(takers):
            then(v)
