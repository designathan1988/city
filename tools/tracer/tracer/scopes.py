"""What a name means where it is written, by the language's scopes: the function around it, the class around it (and
its bases) where the language reads members by a plain name, the namespaces, the file with what it imports and
includes, a file its own words name; a name no scope gives is none of the code read. A member of a class is found in
the class, then up its bases, and down in the classes below it (an override may be the one that runs).
"""
from __future__ import annotations

from collections import defaultdict

from .model import text, unquoted, child_nodes
from .vocab import (ARGS, CLASS, DEFAULT, EXACT, FUNCTION, IDENT, MEMBER, MODULE, NAMESPACE, OBJECT, OPTIONAL, PARAM,
                    PROPERTY, REST, SCOPES, SHAPE, STORED, TYPE, VALUE, VARIABLE, WHOLE)


class Scopes:
    def __init__(self, program):
        self.p = program
        self.sources = program.sources
        self.base_of, self.below_of, self.local_of = {}, {}, {}
        self.rooted_of, self.named_of, self.member_of = {}, {}, {}
        self.params = defaultdict(lambda: defaultdict(list))     # function -> place -> its parameters there
        self.subs = defaultdict(list)
        self.open = set()                # the classes extending one the code read does not define
        self.entries_of, self.named_classes, self.arity_of = {}, None, {}
        self.ready = False
        for c in list(program.base_nodes):
            for b in self.bases(c):
                self.subs[b].append(c)
        self.ready = True
        for d in program.defs:
            if d.kind == PARAM and program.defs[d.owner].kind == FUNCTION:
                self.params[d.owner][d.position].append(d.id)
        self.reread()

    # ---- a plain name --------------------------------------------------------------------------------------------
    def lookup(self, f, node, name: str) -> tuple:
        """The definitions a plain name written at a node means, and how they were found."""
        p = self.p
        for c in p.containers(f, node):
            found = self.inside(c, name, f)
            if found:
                return p.bodies(found), EXACT
        return list(p.top.get(name, ())), EXACT

    def inside(self, c: int, name: str, f):
        """A container's name as read from a file: a function's own; a class's member, where the language reads
        members by a plain name; a namespace's, written in a file that one reaches (else, for a value or a class, the
        one definition of that name the namespace has in the code read: a file reached through an include not read
        declares it, or the file declares the class ahead of its definition (`class A;`), as a compiled program's
        would: one definition a program, C++ [basic.def.odr]); a file's."""
        p = self.p
        kind = p.defs[c].kind
        if kind == FUNCTION:
            return p.held(c, name)
        if kind == SHAPE:                # an entry is no name of the code written in its object
            return ()
        if p.is_class(c):
            return self.members(c, name, True) if f.lang.implicit_members else ()
        if kind == NAMESPACE:
            reach = self.sources.reach(f.module)
            held = list(p.held(c, name))
            found = [i for i in held if p.files[p.defs[i].file].module in reach]
            if not found and len(held) == 1 and (p.defs[held[0]].kind in STORED or p.is_class(held[0])):
                return held
            return found
        return self.local(c, name) if kind == MODULE else ()

    def local(self, m: int, name: str, seen=None) -> list:
        """A name as a file reads it: its own or one it imports, else one of a file it includes."""
        if (m, name) in self.local_of:
            return self.local_of[(m, name)]
        seen = set() if seen is None else seen
        if (m, name) in seen:
            return []
        seen.add((m, name))
        found = []
        for t in self.sources.reach(m):
            found = list(self.p.held(t, name)) or self.brought(t, name, seen)
            if found:
                break
        self.local_of[(m, name)] = found
        return found

    def brought(self, m: int, name: str, seen: set) -> list:
        """A name a file imports: the file itself, what it exports with no name, or its name there."""
        f = self.p.files[self.p.defs[m].file]
        out = []
        for alias, source, what in f.imports:
            for t in self.sources.resolve(f, source) if alias == name else ():
                out += [t] if what == WHOLE else self.default(t) if what == DEFAULT else self.exported(t, what, seen)
        return out

    def exported(self, m: int, name: str, seen=None) -> list:
        """A name as another file reads it from that one."""
        f = self.p.files[self.p.defs[m].file]
        return self.local(m, f.exports.get(name, name), seen)

    def default(self, m: int) -> list:
        """What a file exports with no name."""
        f = self.p.files[self.p.defs[m].file]
        out = []
        for node in f.default:
            at = self.p.at.get((f.index, node.id))
            out += [at] if at is not None else [i for i, _ in self.static(f, node)]
        return out

    # ---- a member ------------------------------------------------------------------------------------------------
    def members(self, c: int, name: str, down: bool = False) -> list:
        """A scope's member of that name: a namespace's, a file's, a class's (its own, else its bases'; `down`, also
        the functions of that name in the classes below it)."""
        if (c, name, down) not in self.member_of:
            p = self.p
            kind = p.defs[c].kind
            if kind == NAMESPACE:
                found = p.held(c, name)
            elif not p.is_class(c):
                found = self.exported(c, name) if kind == MODULE else []
            else:
                found = self.up(c, name, set())
                if down:
                    found = found + [i for s in self.below(c) for i in p.held(s, name) if p.defs[i].kind == FUNCTION]
            if not self.ready:
                return p.bodies(found)
            self.member_of[(c, name, down)] = p.bodies(found)
        return self.member_of[(c, name, down)]

    def up(self, c: int, name: str, seen: set) -> list:
        if c in seen:
            return []
        seen.add(c)
        own = self.p.held(c, name)
        return list(own) if own else [i for b in self.bases(c) for i in self.up(b, name, seen)]

    def bases(self, c: int) -> list:
        """The classes a class extends, of those the code read defines."""
        if c not in self.base_of:
            self.base_of[c] = []
            found = []
            for fi, node in self.p.base_nodes.get(c, ()):
                named = [i for i, _ in self.static(self.p.files[fi], node) if self.p.is_class(i) and i != c]
                found += named
                if not named:
                    self.open.add(c)
            self.base_of[c] = list(dict.fromkeys(found))
        return self.base_of[c]

    def below(self, c: int) -> list:
        """Every class below a class."""
        if self.ready and c in self.below_of:
            return self.below_of[c]
        seen, todo = [], list(self.subs.get(c, ()))
        while todo:
            s = todo.pop()
            if s != c and s not in seen:
                seen.append(s)
                todo += self.subs.get(s, ())
        if self.ready:
            self.below_of[c] = seen
        return seen

    def rooted(self, c: int, seen=None) -> bool:
        """A class below one the code read does not define (its own base, or one up its bases; where the language
        gives every class a base, any class): an object of a type not defined here may be one of it."""
        if c not in self.rooted_of:
            seen = set() if seen is None else seen
            if c in seen:
                return False
            seen.add(c)
            lang = self.p.files[self.p.defs[c].file].lang
            self.rooted_of[c] = lang.implicit_base or c in self.open or any(self.rooted(b, seen) for b in self.bases(c))
        return self.rooted_of[c]

    def by_name(self, name: str, lang) -> list:
        """The members of that name an object nothing is known of may hold a function in (a function, or a variable a
        class declares), read from a file of a language: every class's of that language; where the language looks a
        member up in the object as it runs, also those of the other languages' classes below a type not defined
        here (what reaches one language from another)."""
        if (name, lang.package) not in self.named_of:
            p = self.p
            holder = lambda i: p.defs[i].owner if p.is_class(p.defs[i].owner) else p.defs[i].holder    # noqa: E731
            same = lambda i: p.files[p.defs[i].file].lang.package == lang.package                      # noqa: E731
            found = [i for i in p.bodies(p.member.get(name, ())) if p.defs[i].kind in (FUNCTION,) + STORED]
            self.named_of[(name, lang.package)] = [i for i in found
                                                   if same(i) or (lang.duck_typed and self.rooted(holder(i)))]
        return self.named_of[(name, lang.package)]

    def takes(self, d: int, count: int) -> bool:
        """Whether a function can be called with that many arguments: no fewer than the parameters that may not be
        left out, no more than all of them (any number past a parameter taking the rest)."""
        if d not in self.arity_of:
            least, most = 0, 0
            for place in sorted(self.params.get(d, {})):
                parts = self.p.defs[self.params[d][place][0]].parts
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
        owner = self.p.defs[i].owner
        return owner if owner >= 0 and self.p.is_class(owner) else self.p.defs[i].holder

    def makers(self, c: int, seen=None) -> list:
        """What runs when an object of a class is made: its constructors (the class itself where it writes none), and
        its bases'."""
        seen = set() if seen is None else seen
        if c in seen:
            return []
        seen.add(c)
        out = self.p.bodies(self.p.constructors.get(c, ())) or [c]
        for b in self.bases(c):
            out += self.makers(b, seen)
        return list(dict.fromkeys(out))

    # ---- a name read and not run ---------------------------------------------------------------------------------
    def static(self, f, node) -> list:
        """The definitions a node names where it is read as a name (a type, a base, a scope): [(definition, how)]."""
        while node.id in f.named:
            node = f.named[node.id]
        role = f.role(node) or ("", None)
        if role[0] == MEMBER:
            scopes, prop = self.static(f, role[1][OBJECT]), role[1][PROPERTY]
            while f.is_a(prop, MEMBER):
                inner = f.role(prop)[1]
                scopes, prop = self.step(scopes, self.p.word(f, inner[OBJECT])), inner[PROPERTY]
            return self.step(scopes, self.p.word(f, prop))
        if role[0] == IDENT:
            ids, how = self.lookup(f, node, text(node))
            return [(i, how) for i in ids]
        source = unquoted(node)
        if source is not None:
            return [(m, EXACT) for m in self.sources.resolve(f, source)]
        kids = child_nodes(node)
        return self.static(f, kids[0]) if len(kids) == 1 else []

    def step(self, scopes: list, name: str) -> list:
        return [(i, how) for c, how in scopes if self.p.defs[c].kind in SCOPES for i in self.members(c, name)]

    def classes(self, f, node) -> list:
        return [(i, how) for i, how in self.static(f, node) if self.p.is_class(i)]

    def given(self, f, node) -> list:
        """What a name is written with beside itself (its arguments), not the parts of the name."""
        out = []
        while True:
            if node.id in f.named:
                out += [c for c in child_nodes(node) if c.id != f.named[node.id].id]
                node = f.named[node.id]
            elif f.is_a(node, MEMBER):
                out += self.given(f, f.role(node)[1][OBJECT])
                node = f.role(node)[1][PROPERTY]
            else:
                return out

    def types(self, f, node) -> tuple:
        """What a written type names: the classes, the variables standing for a type (an alias, a type's parameter),
        whether some name in it is none the code read defines, and whether it may be any value."""
        classes, stored, missing, open_ = [], [], False, False
        todo = [node]
        while todo:
            node = todo.pop()
            if node.id in f.open:            # a type that may be any value
                open_ = True
                continue
            at = self.p.at.get((f.index, node.id))
            if at is not None and self.p.defs[at].kind == FUNCTION:
                continue
            if node.id not in f.named and not f.is_a(node, IDENT) and not f.is_a(node, MEMBER):
                todo += child_nodes(node)
                continue
            found = self.static(f, node)
            missing = missing or not found
            classes += [(i, how) for i, how in found if self.p.is_class(i)]
            stored += [i for i, _ in found if self.p.defs[i].kind in STORED]
            todo += self.given(f, node)
        return list(dict.fromkeys(classes)), list(dict.fromkeys(stored)), missing, open_

    def reread(self) -> None:
        """A function declared with no body inside a function's body, every parameter one name that is no class's: an
        object made with those names as its arguments (`languages`: ambiguous declarations)."""
        p = self.p
        for d in p.defs:
            f = p.files[d.file]
            if not f.lang.ambiguous_declarations or d.kind != FUNCTION or d.body or p.defs[d.owner].kind != FUNCTION:
                continue
            given = child_nodes(d.parts[ARGS]) if ARGS in d.parts and TYPE in d.parts else ()
            if given and all(self.is_value(f, one) for one in given):
                d.kind, d.remade = VARIABLE, True

    def is_value(self, f, written) -> bool:
        kids = child_nodes(written)
        return len(kids) == 1 and f.is_a(kids[0], IDENT) and not self.classes(f, kids[0])

    # ---- around a node -------------------------------------------------------------------------------------------
    def class_around(self, f, node) -> int | None:
        """The class code is written in (a shape is not one: `this` in it is the class's around it)."""
        return next((c for c in self.p.containers(f, node) if self.p.is_class(c) and not self.p.is_shape(c)), None)

    def function_at(self, f, node) -> int | None:
        return next((c for c in self.p.containers(f, node) if self.p.defs[c].kind == FUNCTION), None)

    def unit(self, f, node) -> int:
        """What a piece of code belongs to: the function around it, else the class, else the file (code written in
        an object written in place belongs to what is around that object)."""
        first = next(c for c in self.p.containers(f, node) if not self.p.is_shape(c))
        return first if self.p.defs[first].kind == FUNCTION or self.p.is_class(first) else f.module

    def entries(self, c: int) -> list:
        """A shape's members."""
        if c not in self.entries_of:
            self.entries_of[c] = [i for held in self.p.names.get(c, {}).values() for i in held]
        return self.entries_of[c]

    def classes_named(self, name: str) -> list:
        """Every class of that name, in any language read."""
        if self.named_classes is None:
            self.named_classes = {}
            for d in self.p.defs:
                if d.kind == CLASS and d.name:
                    self.named_classes.setdefault(d.name, []).append(d.id)
        return self.named_classes.get(name, [])

    def foreign(self, name: str, lang) -> list:
        """The classes of that name another language read defines below a type not defined here: what a name no
        scope gives may be, read where members are looked up as the program runs (a class the platform runs that
        code of another language registered)."""
        if not lang.duck_typed:
            return []
        if self.named_classes is None:
            self.named_classes = {}
            for d in self.p.defs:
                if d.kind == CLASS and d.name:
                    self.named_classes.setdefault(d.name, []).append(d.id)
        return [c for c in self.named_classes.get(name, ())
                if self.p.files[self.p.defs[c].file].lang.package != lang.package and self.rooted(c)]
