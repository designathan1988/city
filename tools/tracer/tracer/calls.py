"""The calls: at each place a function is called or an object made, what the callee holds says what runs. A function
runs with the arguments as its parameters and gives back its result; a class makes an object and runs what makes it.
Where no definition written with a body answers, the functions among the arguments (and the ones the object called on
holds) are handed to code the tracer did not read, which may run them, and stay with the object they were handed to.
"""
from __future__ import annotations

from collections import defaultdict
from dataclasses import dataclass

from .solver import marked
from .vocab import AS_KEPT, BY_NAME, CALL, CLS, EXACT, FN, FUNCTION, HANDED, INST, KEPT, NEW, PART, RET, VALUE, VAR

SHOWN = 80                           # how much of a callee's text a report shows


@dataclass
class Link:
    src: int
    dst: int
    kind: str
    how: str
    file: int
    line: int


@dataclass
class Site:
    f: object
    node: object
    kind: str
    args: list
    key: tuple
    unit: int
    receiver: tuple | None = None    # the node holding the object the callee is a member of
    places: list | None = None       # where that object is kept
    shown: str = ""                  # the callee as written
    exact: bool = False              # a definition answers it by the scopes or the declared types
    found: bool = False              # a definition answers it, by them or by name
    answered: bool = False           # and one of them is written with a body
    handed: bool = False
    direct: bool = False             # the object called on is read as a member where it is called
    name: str = ""                   # the member called, where the callee is one
    gave: bool = False               # a function is among its arguments
    kept: list | None = None         # the functions the object called on holds


@dataclass
class Result:
    program: object
    scopes: object
    out: dict                        # definition -> its links, in the order written
    into: dict                       # definition -> the links reaching it
    outside: dict                    # definition -> the callees it writes that no definition answers


class Calls:
    def __init__(self, flow):
        self.fl = flow
        self.sites, self.links, self.outside = [], {}, defaultdict(set)

    def site(self, f, node, kind: str, callee, given: list, key, shown: str, receiver=None, places=None, props=None,
             sup: bool = False, direct: bool = False, name: str = "") -> None:
        """One place of the code that calls (or makes): its arguments wired, its callee watched."""
        fl = self.fl
        args = [fl.ev(f, one) for one in given]
        if props is not None:
            one = (PART, f.index, props.id)
            for child in props.named_children:
                if child.id in f.pairs:
                    fl.s.follow(fl.ev(f, f.pairs[child.id][VALUE]), one)
            args = [one]
        site = Site(f, node, kind, args, key, fl.sc.unit(f, node), receiver, places, " ".join(shown.split())[:SHOWN])
        site.direct, site.name = direct, name
        self.sites.append(site)
        if sup and f.lang.super_names_method:
            self.same_name(site)
        else:
            fl.s.watch(callee, lambda v: self.reach(site, v))

    def reach(self, site: Site, v: tuple) -> None:
        kind, d, how = v
        if kind == FN:
            self.run(site, d, how)
        elif kind == KEPT:                   # what is called keeps the function: it may be the one that runs
            self.run(site, d, BY_NAME)
        elif (kind == CLS and self.fl.p.is_class(d)) or (kind == INST and site.kind == NEW):
            self.make(site, d, how)

    def run(self, site: Site, d: int, how: str) -> None:
        """A function runs: linked, given the arguments, its result the call's value."""
        self.link(site.unit, d, NEW if site.kind == NEW else CALL, how, site.f, site.node)
        self.bind(site, d, how)
        self.fl.s.follow((RET, d), site.key, marked(how))
        site.found, site.exact = True, site.exact or how == EXACT
        if self.fl.p.defs[d].body:
            site.answered = True
        else:
            self.hand(site)

    def make(self, site: Site, c: int, how: str) -> None:
        """An object of a class is made: what makes it runs, the call's value is the object."""
        for m in self.fl.sc.makers(c):
            self.link(site.unit, m, NEW, how, site.f, site.node)
            if self.fl.p.defs[m].kind == FUNCTION:
                self.bind(site, m, how)
        self.fl.s.add(site.key, (INST, c, how))
        site.found, site.answered, site.exact = True, True, site.exact or how == EXACT

    def bind(self, site: Site, d: int, how: str) -> None:
        for i, arg in enumerate(site.args):
            for p in self.fl.sc.params[d].get(i, ()):
                self.fl.s.follow(arg, (VAR, p), marked(how))

    def same_name(self, site: Site) -> None:
        """The base called alone: the base's function of the name of the one around the call."""
        sc = self.fl.sc
        fn, c = sc.function_at(site.f, site.node), sc.class_around(site.f, site.node)
        name = self.fl.p.defs[fn].name if fn is not None else ""
        for b in sc.bases(c) if c is not None and name else ():
            for d in sc.members(b, name):
                if self.fl.p.defs[d].kind == FUNCTION:
                    self.run(site, d, EXACT)

    def hand(self, site: Site) -> None:
        """No definition with a body answers the call by the scopes or the types: every function among its arguments
        is handed over and stays with the object called on. A call given no function, that none answers even by
        name, may be the one running what that object holds (`result`)."""
        if site.handed:
            return
        site.handed, site.kept = True, []
        fl = self.fl
        if not site.found:
            self.outside[site.unit].add(site.shown)

        def given(v: tuple) -> None:
            if v[0] == FN:
                site.gave = True
                self.link(site.unit, v[1], HANDED, v[2], site.f, site.node)
        for arg in site.args:
            fl.s.watch(arg, given)
            if site.places:
                fl.st.store(site.places, arg, site.f, site.node, AS_KEPT)
        if site.receiver is not None:
            fl.s.watch(site.receiver, lambda v: site.kept.append(v) if v[0] in (FN, KEPT) else None)

    @staticmethod
    def runs(site: Site, v: tuple, keeping: set) -> bool:
        """Whether a call no definition answers, given no function, may run a function the object it is called on
        holds. A function that is the object itself: yes (a member of a function runs it). One the object keeps:
        not through a member the code hands functions through (that member keeps them, it does not run them); and,
        found by name, only where the object is read as a member right there (what was found by name and travelled
        on is no ground for it)."""
        if v[0] == KEPT and (site.f.lang.package, site.name) in keeping:
            return False
        return v[2] == EXACT or site.direct

    def link(self, src: int, dst: int, kind: str, how: str, f, node) -> None:
        old = self.links.get((src, dst, kind))
        if old is None:
            self.links[(src, dst, kind)] = Link(src, dst, kind, how, f.index, node.start_point[0] + 1)
        elif how == EXACT:
            old.how = EXACT

    def result(self) -> Result:
        """The links by where they start and where they end; a call given no function hands over what the object it
        is called on holds (`runs`); making an object of a class also runs what the class writes at its own level."""
        p = self.fl.p
        keeping = {(site.f.lang.package, site.name) for site in self.sites if site.gave and site.name}
        for site in self.sites:
            for v in site.kept or () if not site.gave and not site.answered else ():
                if self.runs(site, v, keeping):
                    self.link(site.unit, v[1], HANDED, v[2], site.f, site.node)
        starts = {src for src, _, _ in self.links}
        for link in list(self.links.values()):
            owner = p.defs[link.dst].owner
            if link.kind == NEW and p.defs[link.dst].kind == FUNCTION and owner in starts and p.is_class(owner):
                self.links.setdefault((link.src, owner, NEW), Link(link.src, owner, NEW, link.how, link.file, link.line))
        out, into = defaultdict(list), defaultdict(list)
        for link in sorted(self.links.values(), key=lambda one: (one.src, one.file, one.line, one.dst, one.kind)):
            out[link.src].append(link)
            into[link.dst].append(link)
        return Result(p, self.fl.sc, dict(out), dict(into), {unit: sorted(names) for unit, names in self.outside.items()})
