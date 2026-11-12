"""The member reads: of what an object holds, the member of a name. What each class of the object declares; of a
class declaring none and of an object of a type not defined here, what was kept under the name. A member of an
object nothing is known of is found by name: what was kept under it and, where the member is called, the functions
such an object may hold under it, narrowed by what is known of the call and of the object: the function must take
that many arguments, and of the classes holding one, the ones declaring the most of the other members read of the
same object are kept. More than a trace follows (`among`), and the call is ambiguous: no link, said so.
"""
from __future__ import annotations

from collections import defaultdict
from dataclasses import dataclass

from .vocab import BY_NAME, FN, FUNCTION, GLOBAL, INST, KEPT, OPTIONAL, OUT, OUTSIDE, REST, STORED, TOLD, TXT, VALUE


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
    holder: object = None            # what holds the object (the variable read), its members read counted together


class Reads:
    """Of an object of a type not defined here, a member is the platform's: nothing is read under its name."""
    def __init__(self, flow, among: int | None = None):
        self.fl = flow
        self.all = []
        self.among = among                   # the most functions found by name a call is linked to
        self.on = defaultdict(set)           # the node holding an object -> the members read of it
        self.named_of, self.arity_of = {}, {}    # (name, language) -> its candidates; function -> (least, most)

    def read(self, f, node, target, name: str, key) -> None:
        """The member of that name of what `target` holds, into `key`. Where the language looks a member up in the
        object as it runs, a type not defined here says nothing of the object."""
        fl = self.fl
        r = Read(target, name, key, f, node, key in fl.callees)
        self.all.append(r)
        r.holder = self.fl.holder_of(target)
        self.on[r.holder].add(name)

        def seen(v: tuple) -> None:
            kind, c, how = v
            if kind in (FN, KEPT, TXT, TOLD) or (kind == OUT and c != GLOBAL and f.lang.duck_typed):
                return
            r.typed = True
            found = [] if kind == OUT else fl.sc.members(c, name, kind == INST)
            for d in found:
                fl.give(d, key, how, f, node)
            if found:
                return
            if kind == OUT or (not f.lang.duck_typed and fl.sc.rooted(c)):
                fl.s.add(key, OUTSIDE)       # a member of the platform's object, or one a class takes from the
                #                              platform's, is the platform's: an object of a type not defined here
            else:
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
        if not r.called:
            return
        found = self.narrowed(r, self.by_name(r.name, r.f.lang))
        site = self.fl.calls.by_callee.get(r.key)
        if self.among is not None and len(found) > self.among:
            if site is not None:
                self.fl.calls.ambiguous[site.unit].add((site.shown, len(found)))
            return
        for d in found:
            self.fl.give(d, r.key, BY_NAME, r.f, r.node)

    def narrowed(self, r: Read, found: list) -> list:
        """The functions found by name that take the call's arguments, of the classes declaring the most of the other
        members read of the same object."""
        sc, site = self.fl.sc, self.fl.calls.by_callee.get(r.key)
        if site is not None:
            found = [d for d in found if self.fl.p.defs[d].kind != FUNCTION or self.takes(d, len(site.args))]
        others = self.on.get(r.holder, set()) - {r.name}
        if not others or len(found) < 2:
            return found
        score = {d: sum(1 for name in others if sc.members(self.holder(d), name, True)) if self.holder(d) >= 0 else 0
                 for d in found}
        best = max(score.values())
        return [d for d in found if score[d] == best] if best else found

    def by_name(self, name: str, lang) -> list:
        """The members of that name an object nothing is known of may hold a function in (a function, or a variable a
        class declares), read from a file of a language: every class's of that language; where the language looks a
        member up in the object as it runs, also those of the other languages' classes below a type not defined
        here (what reaches one language from another)."""
        if (name, lang.package) not in self.named_of:
            p, sc = self.fl.p, self.fl.sc
            holder = lambda i: p.defs[i].owner if p.is_class(p.defs[i].owner) else p.defs[i].holder    # noqa: E731
            same = lambda i: p.files[p.defs[i].file].lang.package == lang.package                      # noqa: E731
            found = [i for i in p.bodies(p.member.get(name, ())) if p.defs[i].kind in (FUNCTION,) + STORED]
            self.named_of[(name, lang.package)] = [i for i in found
                                                   if same(i) or (lang.duck_typed and sc.rooted(holder(i)))]
        return self.named_of[(name, lang.package)]

    def takes(self, d: int, count: int) -> bool:
        """Whether a function can be called with that many arguments: no fewer than the parameters that may not be
        left out, no more than all of them (any number past a parameter taking the rest)."""
        if d not in self.arity_of:
            params, least, most = self.fl.sc.params, 0, 0
            for place in sorted(params.get(d, {})):
                parts = self.fl.p.defs[params[d][place][0]].parts
                if REST in parts:
                    most = None
                    break
                most += 1
                if VALUE not in parts and OPTIONAL not in parts:
                    least = most
            self.arity_of[d] = (least, most)
        least, most = self.arity_of[d]
        return count >= least and (most is None or count <= most)

    def holder(self, i: int) -> int:
        """The class a member is written in (or is also a member of)."""
        p = self.fl.p
        owner = p.defs[i].owner
        return owner if owner >= 0 and p.is_class(owner) else p.defs[i].holder

    def unknown(self, closed: set) -> list:
        """The reads whose object is still unknown, the innermost first: widened; the list of those left waiting."""
        waiting = [r for r in self.all if not r.typed and not r.wide and r.target not in closed]
        keys = {r.key for r in waiting}
        for r in [r for r in waiting if r.target not in keys] or waiting:
            self.widen(r)
        return waiting
