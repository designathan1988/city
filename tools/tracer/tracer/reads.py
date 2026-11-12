"""The member reads: of what an object holds, the member of a name. What each class of the object declares; of a
class declaring none and of an object of a type not defined here, what was kept under the name. A member of an
object nothing is known of is found by name: what was kept under it and, where the member is called, the functions
such an object may hold under it.
"""
from __future__ import annotations

from dataclasses import dataclass

from .vocab import BY_NAME, FN, INST, KEPT, OUT, TOLD, TXT


@dataclass
class Read:
    """A member read: the node holding the object, the name, the node the member's values go to."""
    target: tuple
    name: str
    key: tuple
    f: object
    node: object
    called: bool = False             # the member is called where it is read
    typed: bool = False              # the object is one of a class, a scope, or of a type not defined here
    opened: bool = False             # reads what was kept under its name
    wide: bool = False               # found by name among the classes


class Reads:
    def __init__(self, flow):
        self.fl = flow
        self.all = []

    def read(self, f, node, target, name: str, key) -> None:
        """The member of that name of what `target` holds, into `key`. Where the language looks a member up in the
        object as it runs, a type not defined here says nothing of the object."""
        fl = self.fl
        r = Read(target, name, key, f, node, key in fl.callees)
        self.all.append(r)

        def seen(v: tuple) -> None:
            kind, c, how = v
            if kind in (FN, KEPT, TXT, TOLD) or (kind == OUT and f.lang.duck_typed):
                return
            r.typed = True
            found = [] if kind == OUT else fl.sc.members(c, name, kind == INST)
            for d in found:
                fl.give(d, key, how, f, node)
            if not found:
                self.open(r)
        fl.s.watch(target, seen)

    def open(self, r: Read) -> None:
        if not r.opened:
            r.opened = True
            self.fl.s.follow(self.fl.prop(r.f, r.name), r.key)

    def widen(self, r: Read) -> None:
        """A member of an object nothing is known of: what was kept under its name and, where the member is called,
        the functions such an object may hold under it, a class's function or one kept in a member a class declares
        (read and not called, it is most often no function at all)."""
        r.wide = True
        self.open(r)
        for d in self.fl.sc.by_name(r.name, r.f.lang) if r.called else ():
            self.fl.give(d, r.key, BY_NAME, r.f, r.node)

    def unknown(self, closed: set) -> list:
        """The reads whose object is still unknown, the innermost first: widened; the list of those left waiting."""
        waiting = [r for r in self.all if not r.typed and not r.wide and r.target not in closed]
        keys = {r.key for r in waiting}
        for r in [r for r in waiting if r.target not in keys] or waiting:
            self.widen(r)
        return waiting
