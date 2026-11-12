"""The calls: at each place a function is called or an object made, what the callee holds says what runs. A function
runs with the arguments as its parameters and gives back its result; a class makes an object and runs what makes it.
Where no definition written with a body answers, the functions among the arguments (and the ones the object called on
holds) are handed to code the tracer did not read, which may run them, and stay with the object they were handed to.
Where no definition answers at all, that code gives back the objects it was given and called on (and of the types the
call is written with), and runs the functions it is handed with them as their arguments (`through`).

Each link keeps what the call wrote: the functions and functions' names it gave, and the texts written as its
arguments; a function that reads an entry of an object written in place by a key it was not written with keeps, for
each function or name an entry holds, the entries' keys (`Result.tables`), for a trace to follow only what its
caller's texts select. The records (`Link`, `Site`, `Result`) are `links`'; what an object written in place hands over,
`shapes`'.
"""
from __future__ import annotations

from collections import defaultdict

from .links import Link, Result, Site
from .model import named
from .solver import marked
from .vocab import (AS_BUILT, AS_KEPT, AS_OBJECTS, BY_NAME, BY_TEXT, CALL, CLS, EXACT, EXPR, FN, FUNCTION, HANDED, INST, KEPT,
                    NEW, OUT, OUTSIDE, PART, RET, STRENGTH, TOLD, TXT, VAR)

SHOWN = 80                           # how much of a callee's text a report shows


class Calls:
    def __init__(self, flow):
        self.fl = flow
        self.sites, self.links, self.outside = [], {}, defaultdict(set)
        self.by_callee = {}                  # the node read to be called -> its call
        self.guessed = defaultdict(set)      # a link found by name -> the calls it was found at
        self.ambiguous = defaultdict(set)    # unit -> (callee as written, how many functions of its name)

    def site(self, f, node, kind: str, callee, given: list, key, shown: str, receiver=None, places=None, props=None,
             sup: bool = False, direct: bool = False, name: str = "", around=None, typed=None) -> None:
        """One place of the code that calls (or makes): its arguments wired, its callee watched."""
        fl = self.fl
        args = [fl.ev(f, one) for one in given]
        if props is not None:                    # the one argument: the object of the attributes' shape
            one = (PART, f.index, props.id)
            at = fl.p.at.get((f.index, props.id))
            if at is not None and fl.p.is_shape(at):
                fl.shapes.shaped(at, one)
            args = [one]
        site = Site(f, node, kind, args, key, fl.sc.unit(f, node), receiver, places, " ".join(shown.split())[:SHOWN])
        site.direct, site.name, site.around, site.texts, site.targets, site.given = direct, name, around, set(), [], set()
        site.typed, site.words, site.guesses = typed, frozenset(w for w in (named(one) for one in given) if w), set()
        self.by_callee.setdefault(callee, site)

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
        if receiver is not None:
            fl.s.watch(receiver, lambda v: self.on_platform(site, v))

    @staticmethod
    def on_platform(site: Site, v: tuple) -> None:
        if v[0] == OUT:
            site.platform = True

    def reach(self, site: Site, v: tuple) -> None:
        kind, d, how = v
        if kind == OUT:
            site.platform = True
        elif kind == FN:
            self.run(site, d, how)
        elif kind == KEPT:                   # what is called keeps the function: it may be the one that runs
            self.run(site, d, BY_NAME)
        elif (kind == CLS and self.fl.p.is_class(d)) or (kind == INST and site.kind == NEW):
            self.make(site, d, how)

    def run(self, site: Site, d: int, how: str) -> None:
        """A function runs: linked, given the arguments, its result the call's value."""
        self.link(site.unit, d, NEW if site.kind == NEW else CALL, how, site.f, site.node)
        site.targets.append((d, NEW if site.kind == NEW else CALL))
        if how == BY_NAME:
            site.guesses.add(d)
            self.guessed[(site.unit, d, NEW if site.kind == NEW else CALL)].add(id(site))
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

        def handed(v: tuple) -> None:
            site.gave = True
            site.fns += [v[1]] if v[2] == EXACT else []
            self.link(site.unit, v[1], HANDED, v[2], site.f, site.node)

        def given(v: tuple) -> None:
            if v[0] in (INST, CLS):
                site.objects.add(v[1])
            fl.shapes.functions_in(v, handed, set())

        def called_on(v: tuple) -> None:
            if v[0] == TOLD:
                site.told.add(v[1])
            elif v[0] in (INST, CLS):
                site.objects.add(v[1])
            elif v[0] in (FN, KEPT):
                site.kept.append(v)
        for arg in site.args:
            fl.s.watch(arg, given)
            written = fl.p.at.get((arg[1], arg[2])) if arg[0] == EXPR else None
            if not site.answered and (written is None or fl.p.defs[written].kind != FUNCTION):
                fl.s.follow(arg, site.key, AS_BUILT)     # what code not read is handed, what it gives back may keep
                #                                          (a function written there it runs: `through`)
            if site.places:
                fl.st.store(site.places, arg, site.f, site.node, AS_KEPT)
        if site.receiver is not None:
            fl.s.watch(site.receiver, called_on)

    def through(self) -> bool:
        """Each call no definition answers, once: what the object it is called on holds (the objects), and what the
        functions written in place as its arguments give back, it gives back; with those objects and the ones it is
        given it runs those functions (a function written elsewhere and handed to many such calls is not made to
        carry one's objects to another's); called on the platform's object or made of a class not defined here, it
        gives an object of a type not defined here. True when a call was gone through."""
        fl, done = self.fl, False
        for site in self.sites:
            if site.found or site.passed:
                continue
            site.passed = done = True
            sources = list(site.args) + ([site.receiver] if site.receiver is not None else [])
            if site.receiver is not None:            # what an object of code not read holds, it gives back
                fl.s.follow(site.receiver, site.key, AS_OBJECTS)
            for written in site.typed or ():
                fl.typed(site.f, written, site.key)
            if site.platform or site.kind == NEW:
                fl.s.add(site.key, OUTSIDE)
            for arg in site.args:
                d = fl.p.at.get((arg[1], arg[2])) if arg[0] == EXPR else None
                if d is None or fl.p.defs[d].kind != FUNCTION:
                    continue
                for params in fl.sc.params[d].values():
                    for param in params:
                        for one in sources:
                            if one != arg:
                                fl.s.follow(one, (VAR, param), AS_OBJECTS)
                fl.s.follow((RET, d), site.key, AS_OBJECTS)
        return done

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
        self.unguess()
        keeping = {(site.f.lang.package, site.name) for site in self.sites if site.gave and site.name}
        for site in self.sites:
            for v in site.kept or () if not site.gave and not site.answered else ():
                if self.runs(site, v, keeping):
                    self.link(site.unit, v[1], HANDED, v[2], site.f, site.node)
        self.by_text()
        for site in self.sites:
            for d, kind in site.targets if site.texts or site.given or site.words else ():
                link = self.links.get((site.unit, d, kind))
                if link is None:
                    continue
                link.texts, link.fns, link.words = link.texts | site.texts, link.fns | site.given, link.words | site.words
        starts = {src for src, _, _ in self.links}
        for link in list(self.links.values()):
            owner = p.defs[link.dst].owner
            if link.kind == NEW and p.defs[link.dst].kind == FUNCTION and owner in starts and p.is_class(owner):
                self.links.setdefault((link.src, owner, NEW), Link(link.src, owner, NEW, link.how, link.file, link.line))
        out, into = defaultdict(list), defaultdict(list)
        for link in sorted(self.links.values(), key=lambda one: (one.src, one.file, one.line, one.dst, one.kind)):
            out[link.src].append(link)
            into[link.dst].append(link)
        return Result(p, self.fl.sc, dict(out), dict(into), {unit: sorted(names) for unit, names in self.outside.items()},
                      self.tables(), {unit: sorted(calls) for unit, calls in self.ambiguous.items()})

    def unguess(self) -> None:
        """A call that runs more functions found by name alone than a trace follows is ambiguous: its guesses are
        no links (where no other call found them), and it is said so."""
        among = self.fl.rd.among
        if among is None:
            return
        for site in self.sites:
            if len(site.guesses) <= among:
                continue
            self.ambiguous[site.unit].add((site.shown, len(site.guesses)))
            kind = NEW if site.kind == NEW else CALL
            for d in site.guesses:
                key = (site.unit, d, kind)
                self.guessed[key].discard(id(site))
                if not self.guessed[key] and key in self.links and self.links[key].how == BY_NAME:
                    del self.links[key]
            site.targets = [(d, k) for d, k in site.targets if (site.unit, d, k) in self.links]

    def tables(self) -> dict:
        """For each unit reading an entry of a shape by a key it was not written with: what each entry holds (a
        function, or a function's name) and the entries' keys."""
        fl, out = self.fl, {}
        for unit, shapes in fl.tables.items():
            held = defaultdict(set)
            for c in shapes:
                for entry in fl.sc.entries(c):
                    d = fl.p.defs[entry]
                    if d.kind == FUNCTION:
                        held[entry].add(d.name)
                    for v in fl.s.of((VAR, entry)):
                        if v[0] == FN:
                            held[v[1]].add(d.name)
                        elif v[0] in (TXT, TOLD):
                            held[v[1]].add(d.name)
            out[unit] = dict(held)
        return out
