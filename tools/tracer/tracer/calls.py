"""The calls: at each place a function is called or an object made, what the callee holds says what runs. A function
runs with the arguments as its parameters and gives back its result; a class makes an object and runs what makes it.
Where no definition written with a body answers, the functions among the arguments (and the ones the object called on
holds) are handed to code the tracer did not read, which may run them, and stay with the object they were handed to.
Where no definition answers at all, that code gives back the objects it was given and called on (and of the types the
call is written with), and runs the functions it is handed with them as their arguments (`through`).

Where the language's compiler answered a call (`compilers`), its answer is what the call runs: an exact one is linked
as written and nothing found otherwise is (a function of that name elsewhere, one hidden below its class); a
dispatched one runs what the object's class has of it, among the method and those of its name below its class, as the
flow finds the object (found by name alone: only those); one of code not read runs nothing here. A call through a
value holding a function has no answer: the flow follows what the value holds.

Each link keeps what the call wrote: the functions and functions' names it gave, and the texts written as its
arguments; a function that reads an entry of an object written in place by a key it was not written with keeps, for
each function or name an entry holds, the entries' keys (`Result.tables`), for a trace to follow only what its
caller's texts select; a function value made from a text and kept in such an object under a key not written there is
held under the keys of the entries whose value is that text. A call the language says makes a function value of an
object's method named by a text (`Callable(object, "method")`) runs nothing: the value runs where it is invoked.
"""
from __future__ import annotations

from collections import defaultdict
from dataclasses import dataclass

from .model import named, unquoted, child_nodes
from .solver import marked
from .vocab import (AS_BUILT, AS_KEPT, AS_OBJECTS, BY_NAME, BY_TEXT, CALL, CHANGES, CLS, ELEM, EXACT, EXPR, FIRES, FN,
                    FUNCTION, HANDED, INST, KEPT, MODULE, NEW, OUT, OUTSIDE, PART, READS, RET, SHAPE, STORED, STRENGTH,
                    TOLD, TXT, VALUE, VAR, WRITES, AMBIGUOUS)

SHOWN = 80                           # how much of a callee's text a report shows


@dataclass(slots=True)
class Link:
    src: int
    dst: int
    kind: str
    how: str
    file: int
    line: int
    texts: frozenset = frozenset()   # the functions' names given in texts where the link is written
    fns: frozenset = frozenset()     # the functions given there
    words: frozenset = frozenset()   # the texts written as its arguments there


@dataclass(slots=True)
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
    typed: list | None = None        # the types it is written with (`new A<B>()`)
    words: frozenset = frozenset()   # the texts written as its arguments
    guesses: set = None              # the functions it runs found by name alone
    platform: bool = False           # what is called, or the object it is called on, is the platform's
    passed: bool = False             # what code not read is given was given back (`through`)
    texted: set = None               # what its texts were found to name: functions run, entries read, classes made
    written: list = None             # the texts written as its arguments, whole
    compiled: object = None          # what the language's compiler says it runs (`compilers.Answer`), where it said
    allowed: set = None              # of a dispatched answer: the methods the object's class may run


@dataclass
class Result:
    program: object
    scopes: object
    out: dict                        # definition -> its links, in the order written
    into: dict                       # definition -> the links reaching it
    outside: dict                    # definition -> the callees it writes that no definition answers
    tables: dict = None              # definition -> {function, or function's name: the keys of the entries holding it}
    ambiguous: dict = None           # definition -> [(the callee as written, how many functions of its name)]
    state_out: dict = None           # unit -> {state definition, or a record's entry: what the unit does to it}
    state_in: dict = None            # state definition, or a record's entry -> {unit: what it does to it}
    built_by: dict = None            # unit -> the records it builds (shapes written in it), made when first asked
    unresolved: list = None          # (unit, file, line, callee as written, why no definition answers it)
    texts: set = None                # every name written as a text (what may be looked up by name as it runs)
    open_classes: set = None         # the classes below a type not defined here (the platform may run their members)
    imported: set = None             # every name some file imports from another
    cases: object = None             # when each effect happens, what it writes, and each call's condition (`conditions`)
    answers: dict = None             # the questions asked of it, answered (saved with it: asked again, read back)
    read_in: dict = None             # state definition -> the units that read it (one writing it too among them)
    cut: str = ""                    # why the analysis stopped before its end (a ceiling: time, memory), or nothing
    sites: dict = None               # (file, node) of a call -> what it runs [(definition, kind)] (kept: evaluating)


class Calls:
    def __init__(self, flow):
        self.fl = flow
        self.sites, self.links, self.outside = [], {}, defaultdict(set)
        self.by_callee = {}                  # the node read to be called -> its call
        self.guessed = defaultdict(set)      # a link found by name -> the calls it was found at
        self.held, self.nested = {}, set()   # shape -> (its functions, who is told of them); shapes inside shapes
        # A file run by the platform (a worker made from its path): what it hands the platform, what the code outside
        # hands the object standing for it, the members called on that object from outside; each (found, takers).
        self.inside, self.outside_of = defaultdict(lambda: ([], [])), defaultdict(lambda: ([], []))
        self.called_from_outside = defaultdict(set)
        self.in_file = defaultdict(list)     # a file -> the calls on the platform's object written in it
        self.sending = set()                 # the calls of those found to send back
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
                fl.shaped(at, one)
            args = [one]
        site = Site(f, node, kind, args, key, fl.sc.unit(f, node), receiver, places, " ".join(shown.split())[:SHOWN])
        site.direct, site.name, site.around, site.texts, site.targets, site.given = direct, name, around, set(), [], set()
        site.typed, site.words, site.guesses = typed, frozenset(w for w in (named(one) for one in given) if w), set()
        site.written = [unquoted(one) for one in given]
        site.objects, site.told, site.kept, site.fns, site.texted = set(), set(), [], [], set()
        if kind == CALL and not sup:
            site.compiled = fl.compiled.get((f.index, node.start_byte, node.end_byte))
        if around is not None and site.compiled is not None and not any(
                t in fl.sc.members(around, fl.p.defs[t].name) for t in site.compiled.targets):
            around = site.around = None          # the compiler says it is no member of the class around
        if around is not None:
            site.objects.add(around)
        self.by_callee.setdefault(callee, site)

        def noted(v: tuple) -> None:         # what it is given: texts, functions, objects
            if v[0] in (TXT, TOLD):
                site.texts.add(v[1])
            elif v[0] == FN:
                site.given.add(v[1])
                if v[2] == EXACT and v[1] not in site.fns:
                    site.fns.append(v[1])
            elif v[0] in (INST, CLS):
                site.objects.add(v[1])
        for arg in args:
            fl.s.watch(arg, noted)
        self.sites.append(site)
        for d in site.compiled.targets if site.compiled is not None and not site.compiled.dispatched else ():
            self.run(site, d, EXACT)
        if sup and f.lang.super_names_method:
            self.same_name(site)
        else:
            fl.s.watch(callee, lambda v: self.reach(site, v))
        if receiver is not None:
            fl.s.watch(receiver, lambda v: self.called_on(site, v))

    @staticmethod
    def called_on(site: Site, v: tuple) -> None:
        """What the object it is called on is and keeps."""
        if v[0] == OUT:
            site.platform = True
        elif v[0] == TOLD:
            site.told.add(v[1])
        elif v[0] in (INST, CLS):
            site.objects.add(v[1])
        elif v[0] in (FN, KEPT):
            site.kept.append(v)

    def allows(self, site: Site, v: tuple) -> bool:
        """Whether what the callee holds is what the call runs, by the compiler's answer: of an exact one (or one
        of code not read) nothing but the platform's mark; of a dispatched one a function the flow found by the
        object, or one of the methods the object's class may run."""
        kind, d, how = v
        if kind == OUT:
            return True
        if not site.compiled.dispatched:
            return False
        if kind == FN:
            return how == EXACT or d in self.allowed(site)
        return kind == KEPT and how == EXACT

    def allowed(self, site: Site) -> set:
        if site.allowed is None:
            p, sc = self.fl.p, self.fl.sc
            site.allowed = set(site.compiled.targets)
            for t in site.compiled.targets:
                c = p.defs[t].owner
                if c >= 0 and p.is_class(c):
                    site.allowed.update(sc.members(c, p.defs[t].name, True))
        return site.allowed

    def reach(self, site: Site, v: tuple) -> None:
        kind, d, how = v
        if site.compiled is not None and not self.allows(site, v):
            return
        if kind == OUT:
            site.platform = True
        elif kind == FN:
            self.run(site, d, how)
        elif kind == KEPT and how == EXACT:  # what is called keeps the function: calling it runs it
            self.fire(site, d, how, 0)
        elif kind == KEPT:
            self.run(site, d, BY_NAME)
        elif (kind == CLS and self.fl.p.is_class(d)) or (kind == INST and site.kind == NEW):
            self.make(site, d, how)
        elif kind == INST and site.kind == CALL and site.f.lang.called:   # an object called: its call operator
            for m in self.fl.sc.members(d, site.f.lang.operators + site.f.lang.called, True):
                self.run(site, m, how)

    def run(self, site: Site, d: int, how: str, given: bool = True) -> None:
        """A function runs: linked, given the arguments (`given`), its result the call's value."""
        self.link(site.unit, d, NEW if site.kind == NEW else CALL, how, site.f, site.node)
        site.targets.append((d, NEW if site.kind == NEW else CALL))
        if how == BY_NAME:
            site.guesses.add(d)
            self.guessed[(site.unit, d, NEW if site.kind == NEW else CALL)].add(id(site))
        if given:
            self.bind(site, d, how)
        self.fl.s.follow((RET, d), site.key, marked(how))
        site.found, site.exact = True, site.exact or how == EXACT
        if self.fl.p.defs[d].body:
            site.answered = True
        else:
            self.hand(site)

    def fire(self, site: Site, d: int, how: str, start) -> None:
        """Code not read runs a function it was handed, or the one a value keeps, here: linked as fired, given the
        call's arguments from `start` on (None: the objects of a list of them, to every parameter), its result the
        call's value."""
        fl = self.fl
        self.link(site.unit, d, FIRES, how, site.f, site.node)
        site.targets.append((d, FIRES))
        if start is None:
            for params in fl.sc.params[d].values():
                for param in params:
                    for arg in site.args:
                        fl.s.follow(arg, (VAR, param), AS_OBJECTS)
        else:
            for i, arg in enumerate(site.args[start:]):
                for param in fl.sc.params[d].get(i, ()):
                    fl.s.follow(arg, (VAR, param), marked(how))
        fl.s.follow((RET, d), site.key, marked(how))
        site.found, site.exact = True, site.exact or how == EXACT
        site.answered = site.answered or bool(fl.p.defs[d].body)

    def function_called_on(self, site: Site, v: tuple) -> None:
        """A call no definition answers on a function value: one of the members the language says run it fires it;
        any other gives back the function (bound), which it is handed to."""
        if v[0] != FN:
            return
        invokes = site.f.lang.invokes_table
        if site.name in invokes:
            self.fire(site, v[1], v[2], invokes[site.name])
        else:
            self.fl.s.add(site.key, v)
            self.link(site.unit, v[1], HANDED, v[2], site.f, site.node)

    def make(self, site: Site, c: int, how: str) -> None:
        """An object of a class is made: what makes it runs, the call's value is the object; where the language
        makes a class with no constructor from values as its fields, they go to its fields in order."""
        fl = self.fl
        for m in fl.sc.makers(c):
            self.link(site.unit, m, NEW, how, site.f, site.node)
            if fl.p.defs[m].kind == FUNCTION:
                self.bind(site, m, how)
            elif m == c and site.args and fl.p.files[fl.p.defs[c].file].lang.aggregate:
                fields = sorted((i for held in fl.p.names.get(c, {}).values() for i in held
                                 if fl.p.defs[i].kind in STORED), key=lambda i: fl.p.defs[i].node.start_byte)
                for arg, field in zip(site.args, fields):
                    fl.s.follow(arg, (VAR, field), marked(how))
        fl.s.add(site.key, (INST, c, how))
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
        site.handed = True
        fl = self.fl
        if not site.found:
            self.outside[site.unit].add(site.shown)

        def handed(v: tuple) -> None:
            site.gave = True
            self.link(site.unit, v[1], HANDED, v[2], site.f, site.node)
            if site.platform:
                self.offer(self.inside, site.f.module, v)

        def given(v: tuple) -> None:
            self.functions_in(v, handed, set())
        for arg in site.args:
            fl.s.watch(arg, given)
            written = fl.p.at.get((arg[1], arg[2])) if arg[0] == EXPR else None
            if not site.answered and (written is None or fl.p.defs[written].kind != FUNCTION):
                fl.s.follow(arg, site.key, AS_BUILT)     # what code not read is handed, what it gives back may keep
                #                                          (a function written there it runs: `through`)
            if site.places:
                fl.st.store(site.places, arg, site.f, site.node, AS_KEPT)

    def stands_for_file(self, v: tuple) -> bool:
        """Whether a value is an object standing for a file run by the platform (a file that is no class)."""
        p = self.fl.p
        return v[0] == INST and p.defs[v[1]].kind == MODULE and not p.is_class(v[1])

    def offer(self, table, m: int, v: tuple) -> None:
        """A function into a file's list of `table`, told to who waits for it."""
        if v[0] != FN:
            return
        found, takers = table[m]
        if v in found:
            return
        found.append(v)
        for then in list(takers):
            then(v)

    def wait(self, table, m: int, then) -> None:
        found, takers = table[m]
        takers.append(then)
        for v in list(found):
            then(v)

    def handed_to_platform(self, f, value) -> None:
        """The functions code of a file writes into a member of the platform's object, or hands to code not read
        called on it: what the file asks the platform to run (its handlers of what is sent to it)."""
        self.fl.s.watch(value, lambda v, m=f.module: self.offer(self.inside, m, v))

    def handed_to_file(self, m: int, value) -> None:
        """The functions code outside a file hands the object standing for it: run when the file sends something."""
        self.fl.s.watch(value, lambda v, m=m: self.offer(self.outside_of, m, v))

    def file_called_on(self, site: Site, v: tuple) -> None:
        """A call no definition answers on an object standing for a file: given a function, it is handed to the
        object (run when the file sends); given values, they are sent: the file's handlers fire (by text)."""
        if not self.stands_for_file(v) or site.f.module == v[1]:
            return
        m = v[1]
        if site.name and site.name not in self.called_from_outside[m]:
            self.called_from_outside[m].add(site.name)
            for one in list(self.in_file.get(m, ())):
                self.platform_called_in_file(one)
        if site.given or site.gave:
            for arg in site.args:
                self.handed_to_file(m, arg)
        elif site.args:
            self.wait(self.inside, m, lambda w, site=site: self.fire(site, w[1], BY_TEXT, None))

    def platform_called_in_file(self, site: Site) -> None:
        """In a file run by the platform, a call on the platform's object by a member's name that code outside calls
        on the object standing for the file sends back: what code outside handed that object fires (by text)."""
        m = site.f.module
        if id(site) in self.sending:
            return
        if site.name and site.name in self.called_from_outside.get(m, ()) and site.args and not site.given:
            self.sending.add(id(site))
            self.wait(self.outside_of, m, lambda w, site=site: self.fire(site, w[1], BY_TEXT, None))

    def texts(self) -> bool:
        """The calls no definition with a body answers, read by the texts they are given (as the flow runs, before
        they are taken for unanswered): a text naming a method of an object it is given or called on runs it; one
        naming an entry of an object written in place reads it; one naming a class makes an object of it; a text the
        object called on keeps runs the functions handed over together with that text somewhere. Each by text. A call
        the language says makes a function value (`Language.makers`) runs nothing: its value is the function of the
        method the text names (it runs where it is invoked). True when something new was found."""
        fl, p, sc, done = self.fl, self.fl.p, self.fl.sc, False
        known = defaultdict(set)
        for site in self.sites:
            for d in site.fns:
                if p.defs[d].name in site.texts:
                    known[p.defs[d].name].add(d)
        for site in self.sites:
            if site.answered or not (site.texts or site.told):
                continue
            maker = site.shown in site.f.lang.makers
            for name in site.texts:
                for c in list(site.objects):
                    for m in sc.members(c, name, True):
                        if (CALL, m) in site.texted or m in site.given:    # where it is handed, the text says no
                            continue                                        # more
                        if maker and p.defs[m].kind == FUNCTION:            # a function value made: it runs nothing
                            site.texted.add((CALL, m))
                            fl.s.add(site.key, (FN, m, BY_TEXT))
                            site.found = site.answered = True    # answered: its value is the function, not what
                            done = True                          # it was given (`hand` gives that back otherwise)
                        elif p.defs[m].kind == FUNCTION:
                            site.texted.add((CALL, m))
                            self.run(site, m, BY_TEXT, False)
                            done = True
                        elif p.is_shape(c) and p.defs[m].kind in STORED:
                            site.texted.add((CALL, m))
                            fl.give(m, site.key, BY_TEXT, site.f, site.node)
                            fl.touch(site.unit, m, READS)
                            done = True
                        elif p.defs[m].kind in STORED:
                            site.texted.add((CALL, m))
                            self.member_by_text(site, m)
                            done = True
                for c in sc.classes_named(name):
                    if (NEW, c) not in site.texted:
                        site.texted.add((NEW, c))
                        fl.s.add(site.key, (INST, c, BY_TEXT))
                        done = True
            lookup = site.f.lang.node_lookup
            if lookup and site.shown.split(".")[-1] in lookup and site.written and site.written[0]:
                m = fl.scene_node(site.f, site.written[0])
                if m is not None and (NEW, m) not in site.texted:
                    site.texted.add((NEW, m))
                    fl.s.add(site.key, (INST, m, BY_TEXT))
                    done = True
            for name in site.told:
                for d in known.get(name, ()):
                    if (CALL, d) not in site.texted:
                        site.texted.add((CALL, d))
                        self.run(site, d, BY_TEXT, False)
                        done = True
        return done

    def member_by_text(self, site: Site, m: int) -> None:
        """A member of an object named by a text a call no definition answers is given (a signal emitted or
        connected by its name, a property set or got by its name): given a function, the member keeps it; given
        other values, they may be written to it; else what it keeps fires, and its value is the call's (by text)."""
        fl = self.fl
        if site.given:
            for arg in site.args:
                fl.s.follow(arg, (VAR, m), AS_KEPT)
            return
        if len(site.args) > 1:
            for arg in site.args[1:]:
                fl.s.follow(arg, (VAR, m), AS_OBJECTS)
            fl.touch(site.unit, m, WRITES)
        fl.give(m, site.key, BY_TEXT, site.f, site.node)
        fl.s.watch((VAR, m), lambda v, site=site: self.fire(site, v[1], BY_TEXT, 1)
                   if v[0] in (FN, KEPT) and v[2] == EXACT else None)

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
            if site.compiled is not None and site.compiled.dispatched:
                for d in sorted(self.allowed(site)):     # the object is not known: what its class may run, by type
                    self.run(site, d, EXACT)
                continue
            sources = list(site.args) + ([site.receiver] if site.receiver is not None else [])
            if site.receiver is not None:            # what an object of code not read holds, it gives back
                fl.s.follow(site.receiver, site.key, AS_OBJECTS)
            for written in site.typed or ():
                fl.typed(site.f, written, site.key)
            if site.platform or site.kind == NEW:
                fl.s.add(site.key, OUTSIDE)
            if site.receiver is not None:
                fl.s.watch(site.receiver, lambda v, site=site: self.function_called_on(site, v))
                fl.s.watch(site.receiver, lambda v, site=site: self.file_called_on(site, v))
                if site.name in site.f.lang.text_ends:      # a text's end names what the whole names (`a/b`: b)
                    fl.s.watch(site.receiver, lambda v, site=site: fl.s.add(site.key, v) if v[0] == TXT else None)
            if site.platform:
                self.in_file[site.f.module].append(site)
                self.platform_called_in_file(site)
            for arg in site.args:            # given a file (or an object standing for one): an object standing for it
                fl.s.watch(arg, lambda v, site=site: fl.s.add(site.key, (INST, v[1], BY_TEXT))
                           if v[0] == CLS and fl.p.defs[v[1]].kind == MODULE and not fl.p.is_class(v[1])
                           or self.stands_for_file(v) else None)
            if site.receiver is not None:    # code not read on the object state keeps, for its effect: may change it
                fires = any(v[0] in (FN, KEPT) for v in site.kept)    # (one firing what it keeps changes nothing;
                parent = site.node.parent                             # one whose value is taken reads it)
                effect = site.node.id in site.f.effects or (parent is not None and parent.id in site.f.effects
                                                            and child_nodes(parent)[-1].id == site.node.id)
                for d in fl.kept_in.get(site.receiver, ()) if effect and not fires and not site.given else ():
                    if fl.is_state(d):
                        fl.touch(site.unit, d, CHANGES)
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

    @staticmethod
    def runs(site: Site, v: tuple, keeping: set) -> bool:
        """Whether a call no definition answers, given no function, may run a function the object it is called on
        holds. A function that is the object itself: yes (a member of a function runs it). One the object keeps:
        not through a member the code hands functions through (that member keeps them, it does not run them); and,
        found by name, only where the object is read as a member right there (what was found by name and travelled
        on is no ground for it)."""
        if v[0] == FN:                   # a function value called on: its own members say (`function_called_on`)
            return False
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
                    self.link(site.unit, v[1], FIRES, v[2], site.f, site.node)
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
        from .gaps import FIRES_CALL, HANDS_CALL, PLATFORM_CALL, TEXT_CALL, UNKNOWN_CALL
        many = {(unit, shown) for unit, calls in self.ambiguous.items() for shown, _ in calls}
        target_of = {r.key: r.target for r in self.fl.rd.all}
        fired = {(link.src, link.file, link.line) for link in self.links.values() if link.kind == FIRES}

        def of_platform(key) -> bool:
            """Whether what a call is made on is, or is read off, an object of code not read."""
            seen = set()
            while key is not None and key not in seen:
                seen.add(key)
                if any(v[0] == OUT for v in self.fl.s.of(key)):
                    return True
                key = target_of.get(key)
            return False
        unresolved, texts = [], set()
        for site in self.sites:
            texts |= site.texts | site.words
            if site.found:
                continue
            line = site.node.start_point[0] + 1
            why = AMBIGUOUS if (site.unit, site.shown) in many \
                else FIRES_CALL if (site.unit, site.f.index, line) in fired \
                else HANDS_CALL if site.gave \
                else PLATFORM_CALL if site.platform or of_platform(site.receiver) \
                else TEXT_CALL if site.texts else UNKNOWN_CALL
            unresolved.append((site.unit, site.f.index, line, site.shown, why))
        open_classes = {d.id for d in p.defs if p.is_class(d.id) and not p.is_shape(d.id) and self.fl.sc.rooted(d.id)}
        state_out, state_in = defaultdict(dict), defaultdict(dict)
        for (unit, d), how in self.fl.touched.items():
            state_out[unit][d] = how
            state_in[d][unit] = how
        read_in = defaultdict(set)
        for unit, d in self.fl.read_by:
            read_in[d].add(unit)
        return Result(p, self.fl.sc, dict(out), dict(into), {unit: sorted(names) for unit, names in self.outside.items()},
                      self.tables(), {unit: sorted(calls) for unit, calls in self.ambiguous.items()},
                      dict(state_out), dict(state_in), None, unresolved, texts, open_classes,
                      {what for f in p.files for _, _, what in f.imports}, read_in=dict(read_in))

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
        function, or a function's name) and the entries' keys; and what the shape holds under keys not written where
        it was filled (its elements): a function value made from a text, under the keys of the entries whose value is
        that text (the table the text was read from: `&"key": "method"`)."""
        fl, out, chosen = self.fl, {}, None
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
                for v in fl.s.of((ELEM, c)):
                    if v[0] == FN and v[2] == BY_TEXT:
                        chosen = self.chosen() if chosen is None else chosen
                        held[v[1]] |= chosen.get(fl.p.defs[v[1]].name, set())
            out[unit] = dict(held)
        return out

    def chosen(self) -> dict:
        """Each text an entry of an object written in place holds as its value, and the names of the entries holding
        it: the keys that choose the function a value made from that text is."""
        fl, out = self.fl, defaultdict(set)
        for d in fl.p.defs:
            if d.kind == SHAPE:
                for entry in fl.sc.entries(d.id):
                    for v in fl.s.of((VAR, entry)):
                        if v[0] == TXT:
                            out[v[1]].add(fl.p.defs[entry].name)
        return out
