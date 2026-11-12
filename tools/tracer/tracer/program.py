"""The program as definitions: every file a definition (a module), every fact that defines something a definition
inside the one around it, each container with the names it holds. Built the same way for every language from the facts
its patterns found; what differs between languages is read from `languages` (a file that is a class, a method named as
its class). A definition whose name is written as a member of a scope (`A::f`) is given to that scope (`adopt`).

An object written in place (a shape) is a definition whose members are the entries written in it with a name. It is
no scope of a plain name, no class `this` is an object of, no unit code belongs to, and no part of a label; its
members are never offered by name to an object nothing is known of.
"""
from __future__ import annotations

import posixpath
from collections import defaultdict

from .adopt import adopt
from .configs import Configs
from .model import LAMBDA, OBJECT as SHAPED, Definition, File, named as text_named, spelled, text
from .sources import Sources
from .vocab import (ALIAS, BASE, BODY, CLASS, CLASSES, CONSTRUCTOR, CONTAINERS, DEFAULT, DEFINING, DRIVEN, EXPORT,
                    EXPORT_DEFAULT, FIELD, FUNCTION, GETTER, IMPORT, INCLUDE, KEY, KIND, MEMBER, MODULE, MODULE_NAME,
                    NAME, NAMED, NAMESPACE, OBJECT, OPEN, PAIR, PARAM, PASS, PROPERTY, ROLES, SETTER, SHAPE, SOURCE,
                    VALUE, WHOLE)


class Program:
    def __init__(self, parsed: list, base=None):
        self.base = base
        self.configs = Configs()                      # the files each language reads to resolve names
        self.files = [File(i, p.display, p.lang, p.tree, p.path) for i, p in enumerate(parsed)]
        self.defs: list[Definition] = []
        self.at = {}                                  # (file, node id) -> definition
        self.names = defaultdict(lambda: defaultdict(list))   # container -> name -> definitions
        self.value_names = {}                         # (file, node id) of a value -> the name it is given
        self.base_nodes = defaultdict(list)           # class (or module) -> the nodes naming what it extends
        self.constructors = defaultdict(list)         # class -> the functions that make its objects
        self.around_of = {}
        for f, p in zip(self.files, parsed):
            for anchor, node, parts in p.facts:
                f.facts[anchor].append((node, parts))
            self.settle(f)
            self.define(f)
        self.sources = Sources(self)
        for f in self.files:
            self.own(f)
        adopt(self)                                   # definitions written out of their scope, given to it
        self.labels = [self.label_of(d) for d in self.defs]
        self.index()
        for f in self.files:                          # what is left to read of the facts: the ones the flow starts from
            f.facts = {anchor: f.facts[anchor] for anchor in DRIVEN if anchor in f.facts}

    # ---- building ------------------------------------------------------------------------------------------------
    def settle(self, f: File) -> None:
        """What each node is as an expression (the first role that applies), the file's imports and exports."""
        for role in reversed(ROLES):
            for node, parts in f.facts.get(role, ()):
                same = f.roles.get(node.id)
                f.roles[node.id] = (role, {**same[1], **parts} if same and same[0] == role else parts)
        f.named = {node.id: parts[NAME] for node, parts in f.facts.get(NAMED, ()) if NAME in parts}
        f.pairs = {node.id: parts for node, parts in f.facts.get(PAIR, ()) if VALUE in parts and KEY in parts}
        for node, parts in f.facts.get(PASS, ()):
            f.passes.setdefault(node.id, []).append(parts[VALUE])
        for node, parts in f.facts.get(IMPORT, ()):
            source = spelled(parts[SOURCE]) if SOURCE in parts else ""
            if WHOLE in parts:
                f.imports.append((text(parts[WHOLE]), source, WHOLE))
            elif DEFAULT in parts:
                f.imports.append((text(parts[DEFAULT]), source, DEFAULT))
            elif NAME in parts:
                f.imports.append((text(parts.get(ALIAS, parts[NAME])), source, text(parts[NAME])))
        for node, parts in f.facts.get(INCLUDE, ()):
            if SOURCE in parts and spelled(parts[SOURCE]) not in f.includes:
                f.includes.append(spelled(parts[SOURCE]))
        for node, parts in f.facts.get(EXPORT, ()):
            if ALIAS in parts and NAME in parts:
                f.exports[text(parts[ALIAS])] = text(parts[NAME])
        f.default = [parts[VALUE] for node, parts in f.facts.get(EXPORT_DEFAULT, ()) if VALUE in parts]
        f.open = {node.id for node, _ in f.facts.get(OPEN, ())}

    def new(self, kind: str, name: str, f: File, node) -> Definition:
        d = Definition(len(self.defs), kind, name, f.index, node.start_point[0] + 1, node)
        self.defs.append(d)
        return d

    def word(self, f: File, node) -> str:
        """The name a node writes (of a name given arguments, the name inside it)."""
        while node.id in f.named:
            node = f.named[node.id]
        return text(node)

    def path(self, f: File, node) -> tuple:
        """A name as written: the scopes it is a member of, outermost first, then the name itself."""
        scopes = []
        while f.is_a(node, MEMBER):
            parts = f.role(node)[1]
            scopes.append(self.word(f, parts[OBJECT]))
            node = parts[PROPERTY]
        return tuple(scopes), self.word(f, node)

    def define(self, f: File) -> None:
        """The file's module and each definition its facts make (one for a node several patterns matched)."""
        start = len(self.defs)
        named = f.facts.get(MODULE_NAME)
        stem = posixpath.splitext(posixpath.basename(f.display))[0]
        module = self.new(MODULE, text(named[0][1][NAME]) if named else stem, f, f.tree.root_node)
        f.module, module.named = module.id, bool(named)
        for anchor in DEFINING:
            for node, parts in f.facts.get(anchor, ()):
                key = (f.index, node.id)
                if key not in self.at:
                    self.at[key] = self.new(KIND[anchor], "", f, node).id
                d = self.defs[self.at[key]]
                d.parts.update(parts)
                d.constructor = d.constructor or anchor == CONSTRUCTOR
                d.holder = 0 if anchor == FIELD else d.holder       # settled in `own`
                d.body = d.body or BODY in parts
                if NAME in parts:
                    d.scopes, d.name = self.path(f, parts[NAME])
                    d.name = text_named(parts[NAME]) or d.name      # a name written as a text (an entry's key)
                if VALUE in parts:
                    self.value_names[(f.index, parts[VALUE].id)] = d.name
        for anchor, flag in ((GETTER, "getter"), (SETTER, "setter")):
            for node, _ in f.facts.get(anchor, ()):
                if (f.index, node.id) in self.at:
                    setattr(self.defs[self.at[(f.index, node.id)]], flag, True)
        for parts in f.pairs.values():
            self.value_names.setdefault((f.index, parts[VALUE].id), spelled(parts[KEY]))
        f.span = range(start, len(self.defs))

    def above(self, f: File, node, kinds: tuple) -> int | None:
        """The nearest definition of those kinds at or above a node."""
        while node is not None:
            found = self.at.get((f.index, node.id))
            if found is not None and self.defs[found].kind in kinds:
                return found
            node = node.parent
        return None

    def own(self, f: File) -> None:
        """Each definition's owner: the nearest container around it (a parameter's, the nearest function)."""
        for i in f.span:
            d = self.defs[i]
            if d.kind == MODULE:
                continue
            up = self.above(f, d.node.parent, (FUNCTION,) if d.kind == PARAM else CONTAINERS)
            d.owner = up if up is not None else f.module
            if d.kind == PARAM and d.node.parent is not None:
                d.position = [c.id for c in d.node.parent.named_children].index(d.node.id)
            if d.name and not d.constructor and (up is not None or d.kind != PARAM):
                self.names[d.owner][d.name].append(i)
            if d.holder == 0:
                cls = self.above(f, d.node.parent, (CLASS,))
                d.holder = cls if cls is not None else -1
                if cls is not None and d.name and cls != d.owner:
                    self.names[cls][d.name].append(i)
        for node, parts in f.facts.get(BASE, ()):
            holder = self.above(f, node.parent, (CLASS,))
            if holder is not None or f.lang.file_is_class:
                self.base_nodes[holder if holder is not None else f.module].append((f.index, node))

    def index(self) -> None:
        """Where a name is found beyond its scope: a file its own words name, by that name anywhere; a member, by its
        name among every class's."""
        self.top, self.member = defaultdict(list), defaultdict(list)
        self.callable = {d.name for d in self.defs if d.kind == FUNCTION and d.name}      # the functions' names
        self.by_label = defaultdict(list)
        for d in self.defs:
            if d.kind == FUNCTION:
                self.by_label[self.labels[d.id]].append(d.id)
            if d.kind == MODULE:
                if d.named:
                    self.top[d.name].append(d.id)
                continue
            if not d.name or d.constructor:
                continue
            if self.defs[d.owner].kind == SHAPE:      # an entry is never a guess for an object nothing is known of
                continue
            if d.holder >= 0 or (d.kind != PARAM and self.is_class(d.owner)):
                self.member[d.name].append(d.id)

    # ---- reading -------------------------------------------------------------------------------------------------
    def chain(self, i: int) -> list:
        """A definition and each one around it, outward."""
        out = []
        while i != -1:
            out.append(i)
            i = self.defs[i].owner
        return out

    def containers(self, f: File, node) -> list:
        """The containers around a node, innermost first, the file's module last; one written out of its scope is
        followed by its scope's."""
        found = self.above(f, node, CONTAINERS)
        if found is None:
            return [f.module]
        if found not in self.around_of:
            d = self.defs[found]
            inner = [found] + (self.chain(d.owner) if d.adopted else [])
            self.around_of[found] = list(dict.fromkeys(inner + self.containers(f, d.node.parent)))
        return self.around_of[found]

    def held(self, c: int, name: str):
        """The definitions a container holds under a name."""
        names = self.names.get(c)
        return names.get(name, ()) if names else ()

    def is_class(self, i: int) -> bool:
        """Whether a definition's objects have members: a class, a shape, a file that is a class."""
        d = self.defs[i]
        return d.kind in CLASSES or (d.kind == MODULE and self.files[d.file].lang.file_is_class)

    def is_shape(self, i: int) -> bool:
        return self.defs[i].kind == SHAPE

    def name_of(self, d: Definition) -> str:
        unnamed = SHAPED if d.kind == SHAPE else LAMBDA
        return d.name or self.value_names.get((d.file, d.node.id)) or unnamed.format(d.line)

    def label_of(self, d: Definition) -> str:
        """A definition as it is shown: the names around it, then its own; a module that is no class, its path."""
        if d.kind == MODULE:
            return d.name if self.files[d.file].lang.file_is_class else self.files[d.file].display
        parts = [] if d.kind != SHAPE else [self.name_of(d)]
        while d.kind != MODULE:
            if d.kind != SHAPE:                       # a shape is no part of the labels written in it
                parts.append(self.name_of(d))
            d = self.defs[d.owner]
        if self.files[d.file].lang.file_is_class:
            parts.append(d.name)
        return ".".join(reversed(parts))

    def label(self, i: int) -> str:
        return self.labels[i]

    def global_object(self, f: File, name: str) -> int | None:
        """The file whose object a name is, where the language's own project file makes the name a global."""
        project = self.configs.of(f.lang.project, f.path) if f.path is not None else None
        path = project.globals.get(name) if project is not None else None
        return self.sources.module_at(path) if path is not None else None

    def where(self, i: int) -> str:
        d = self.defs[i]
        return f"{self.files[d.file].display}:{d.line}"

    def bodies(self, ids) -> list:
        """Each definition, a function declared with no body replaced by the ones of its name written with one: those
        whose file reaches the declaration's, where any does."""
        out = []
        for i in ids:
            d = self.defs[i]
            full = [j for j in self.by_label[self.labels[i]] if self.defs[j].body] \
                if d.kind == FUNCTION and not d.body else []
            if len(full) > 1:
                full = [j for j in full if self.sources.sees(self.defs[j].file, d.file)] or full
            out += full or [i]
        return list(dict.fromkeys(out))
