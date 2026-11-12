"""The program as definitions: every file a definition (a module), every fact that defines something a definition
inside the one around it, each container with the names it holds. Built the same way for every language from the facts
its patterns found; what differs between languages is read from `languages` (a file that is a class, a method named as
its class). A definition whose name is written as a member of a scope (`A::f`) is given to that scope.
"""
from __future__ import annotations

import posixpath
from collections import defaultdict

from .model import LAMBDA, Definition, File, spelled, text
from .sources import Sources
from .vocab import (ALIAS, BASE, BODY, CLASS, CONSTRUCTOR, CONTAINERS, DEFAULT, DEFINING, DRIVEN, EXPORT, EXPORT_DEFAULT,
                    FIELD, FUNCTION, GETTER, IMPORT, INCLUDE, KEY, KIND, MEMBER, MODULE, MODULE_NAME, NAME, NAMED,
                    NAMESPACE, OBJECT, PAIR, PARAM, PASS, PROPERTY, ROLES, SETTER, SOURCE, VALUE, WHOLE)

class Program:
    def __init__(self, parsed: list):
        self.files = [File(i, p.display, p.lang, p.tree) for i, p in enumerate(parsed)]
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
        self.adopt()
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

    def held_scopes(self, c: int, name: str) -> list:
        return [i for i in self.held(c, name) if self.defs[i].kind in (CLASS, NAMESPACE)]

    def move(self, d: Definition, owner: int) -> None:
        if d.id in self.names[d.owner].get(d.name, ()):
            self.names[d.owner][d.name].remove(d.id)
        d.owner, d.adopted = owner, True
        self.names[owner][d.name].append(d.id)

    def adopt(self) -> None:
        """Definitions written out of their scope go to it: a namespace under the namespaces its name writes (made
        where no file writes them alone), then every other one under the class or namespace its name's scopes name.
        A namespace written in several places is one scope: its parts keep one table of names."""
        for d in [d for d in self.defs if d.kind == NAMESPACE and d.scopes]:
            holder = d.owner
            for part in d.scopes:
                found = [i for i in self.names[holder].get(part, ()) if self.defs[i].kind == NAMESPACE]
                if not found:
                    made = self.new(NAMESPACE, part, self.files[d.file], d.node)
                    made.owner = holder
                    self.names[holder][part].append(made.id)
                    found = [made.id]
                holder = found[0]
            self.move(d, holder)
        groups, known = defaultdict(list), defaultdict(list)
        for d in self.defs:
            if d.kind == NAMESPACE:
                groups[self.label_of(d)].append(d.id)
            if d.kind in (CLASS, NAMESPACE) and d.name:
                known[d.name].append(d.id)
        for ids in groups.values():
            shared = self.names[ids[0]]
            for other in ids[1:]:
                for name, held in self.names[other].items():
                    shared[name] += held
                self.names[other] = shared
        for d in [d for d in self.defs if d.kind != NAMESPACE and d.scopes]:
            found = None
            for part in d.scopes:
                if found is None:
                    cands = [i for c in self.chain(d.owner) for i in self.held_scopes(c, part)] or known.get(part, [])
                else:
                    cands = self.held_scopes(found, part)
                seen = [i for i in cands if self.sources.sees(d.file, self.defs[i].file)]    # the one it can see first
                found = (seen or cands)[0] if cands else None
                if found is None:
                    break
            if found is not None:
                self.move(d, found)
        for d in self.defs:
            if d.kind != FUNCTION:
                continue
            owner = self.defs[d.owner]
            if self.files[d.file].lang.class_named_constructor and owner.kind == CLASS and d.name == owner.name:
                d.constructor = True
                if d.id in self.names[d.owner][d.name]:
                    self.names[d.owner][d.name].remove(d.id)
            if d.constructor and self.is_class(d.owner):
                self.constructors[d.owner].append(d.id)

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
        d = self.defs[i]
        return d.kind == CLASS or (d.kind == MODULE and self.files[d.file].lang.file_is_class)

    def name_of(self, d: Definition) -> str:
        return d.name or self.value_names.get((d.file, d.node.id)) or LAMBDA.format(d.line)

    def label_of(self, d: Definition) -> str:
        """A definition as it is shown: the names around it, then its own; a module that is no class, its path."""
        if d.kind == MODULE:
            return d.name if self.files[d.file].lang.file_is_class else self.files[d.file].display
        parts = []
        while d.kind != MODULE:
            parts.append(self.name_of(d))
            d = self.defs[d.owner]
        if self.files[d.file].lang.file_is_class:
            parts.append(d.name)
        return ".".join(reversed(parts))

    def label(self, i: int) -> str:
        return self.labels[i]

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
