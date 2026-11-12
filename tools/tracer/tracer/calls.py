"""The calls: at each place a function is called or an object made, what the callee holds says what runs. A function
runs with the arguments as its parameters and gives back its result; a class makes an object and runs what makes it.
Where no definition written with a body answers, the functions among the arguments (and the ones the object called on
holds) are handed to code the tracer did not read, which may run them, and stay with the object they were handed to.
"""
from __future__ import annotations

from collections import defaultdict
from dataclasses import dataclass

from .solver import marked
from .vocab import (AS_BUILT, AS_KEPT, BY_NAME, BY_TEXT, CALL, CLS, EXACT, FN, FUNCTION, HANDED, INST, KEPT, NEW, PART,
                    RET, STRENGTH, TOLD, TXT, VALUE, VAR)

SHOWN = 80                           # how much of a callee's text a report shows


@dataclass
class Link:
    src: int
    dst: int
    kind: str
    how: str
    file: int
    line: int
    texts: frozenset = frozenset()   # the functions' names given in texts where the link is written
    fns: frozenset = frozenset()     # the functions given there


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
    around: int | None = None        # the class whose member a callee written as a plain name may be
    fns: list | None = None          # the functions among its arguments, known exactly
    texts: set | None = None         # the functions' names written in the texts it is given
    targets: list | None = None      # what it links to: (definition, kind)
    given: set | None = None         # the functions among its arguments, however they were found
    told: set | None = None          # the ones the object it is called on was handed, and keeps
    objects: set | None = None       # the classes of the objects it is given or called on


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
             sup: bool = False, direct: bool = False, name: str = "", around=None) -> None:
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
        site.direct, site.name, site.around, site.texts, site.targets, site.given = direct, name, around, set(), [], set()

        def noted(v: tuple) -> None:
            if v[0] in (TXT, TOLD):
                site.texts.add(v[1])
            elif v[0] == FN:
                site.given.add(v[1])
        for arg in args:
            fl.s.watch(arg, noted)
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
        site.targets.append((d, NEW if site.kind == NEW else CALL))
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
        site.handed, site.kept, site.fns, site.told = True, [], [], set()
        site.objects = set() if site.around is None else {site.around}
        fl = self.fl
        if not site.found:
            self.outside[site.unit].add(site.shown)

        def given(v: tuple) -> None:
            if v[0] in (INST, CLS):
                site.objects.add(v[1])
            elif v[0] == FN:
                site.gave = True
                site.fns += [v[1]] if v[2] == EXACT else []
                self.link(site.unit, v[1], HANDED, v[2], site.f, site.node)

        def called_on(v: tuple) -> None:
            if v[0] == TOLD:
                site.told.add(v[1])
            elif v[0] in (INST, CLS):
                site.objects.add(v[1])
            elif v[0] in (FN, KEPT):
                site.kept.append(v)
        for arg in site.args:
            fl.s.watch(arg, given)
            if not site.answered:                        # what code not read is handed, what it gives back may keep
                fl.s.follow(arg, site.key, AS_BUILT)
            if site.places:
                fl.st.store(site.places, arg, site.f, site.node, AS_KEPT)
        if site.receiver is not None:
            fl.s.watch(site.receiver, called_on)

    def by_text(self) -> None:
        """A function handed over together with the text of its name is known by that text to the code not read: a
        call no definition answers, on an object that was handed that text and keeps it, may run it. A text given to
        such a call says nothing alone: with an object whose class has a method of that name, it names the method."""
        p, sc = self.fl.p, self.fl.sc
        known = defaultdict(set)
        for site in self.sites:
            for d in site.fns or ():
                if p.defs[d].name in site.texts:
                    known[p.defs[d].name].add(d)
        for site in self.sites:
            if not site.handed or site.answered:
                continue
            found = {m for name in site.texts for c in site.objects for m in sc.members(c, name)
                     if p.defs[m].kind == FUNCTION}
            for d in sorted(found.union(*(known.get(name, ()) for name in site.told))):
                if (site.unit, d, HANDED) not in self.links:         # where it is handed, the text says no more
                    self.link(site.unit, d, CALL, BY_TEXT, site.f, site.node)

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
        elif STRENGTH.index(how) > STRENGTH.index(old.how):
            old.how = how

    def result(self) -> Result:
        """The links by where they start and where they end; a call given no function hands over what the object it
        is called on holds (`runs`); making an object of a class also runs what the class writes at its own level."""
        p = self.fl.p
        keeping = {(site.f.lang.package, site.name) for site in self.sites if site.gave and site.name}
        for site in self.sites:
            for v in site.kept or () if not site.gave and not site.answered else ():
                if self.runs(site, v, keeping):
                    self.link(site.unit, v[1], HANDED, v[2], site.f, site.node)
        self.by_text()
        for site in self.sites:
            for d, kind in site.targets if site.texts or site.given else ():
                link = self.links[(site.unit, d, kind)]
                link.texts, link.fns = link.texts | site.texts, link.fns | site.given
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
