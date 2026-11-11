"""JavaScript / TypeScript modules (J3, J7, J8): what a module exports, what an import binds, what a type name names
(a class, a type alias), and the global scope. Where a specifier leads is `js_specifiers`'s. An export is followed
through `export ... from` and `export *` to its definition. Scripts (files with no import or export) and `declare
global` put their names in the global scope.
"""
from __future__ import annotations

import posixpath
from collections import defaultdict

from . import templates
from .js_class import SHAPE
from .js_decl import GLOBAL_KEY
from .js_specifiers import Specifiers
from .model import ALIAS_KIND


def strip_generics(text: str) -> str:
    """`A.B<C<D>>` as `A.B`: the type arguments dropped."""
    out, depth = [], 0
    for c in text:
        if c == "<":
            depth += 1
        elif c == ">":
            depth -= 1
        elif depth == 0:
            out.append(c)
    return "".join(out).strip()


class JsModules(Specifiers):
    def __init__(self, all_facts: list, index):
        self.ix = index
        self.files = {f.path: f for f in all_facts if f.lang == "js"}
        self.configs = {posixpath.dirname(f.path): f.config for f in all_facts if f.lang == "tsconfig"}
        self.packages = {posixpath.dirname(f.path): f.config for f in all_facts if f.lang == "package"}
        self.ambient = {k for f in self.files.values() for k in f.config.get("ambient", [])}
        self.namespaces = {k for f in self.files.values() for k in f.config.get("namespaces", [])}
        self.globals = defaultdict(list)          # name -> gids of the global scope
        for path, facts in self.files.items():
            if facts.script:                      # a script's top-level declarations, never an inner function (L2)
                for unit in facts.units:
                    if unit.owner == path and unit.name != "<module>" and unit.kind != "lambda":
                        self.globals[unit.name].append(unit.gid)
        for name, gids in index.members.get(GLOBAL_KEY, {}).items():
            self.globals[name] += gids
        self._resolved, self._exports, self._classes = {}, {}, {}
        self.package_dirs = set(self.packages)    # node_modules/<pkg> folders holding files read
        for path in self.files:
            parts = path.split("/")
            for i, part in enumerate(parts[:-1]):
                if part == "node_modules" and i + 1 < len(parts) - 1:
                    count = 2 if parts[i + 1].startswith("@") else 1
                    self.package_dirs.add("/".join(parts[:i + 1 + count]))

    def is_module(self, key: str) -> bool:
        return key in self.files or key in self.ambient

    # ---- exports and bindings ------------------------------------------------------------------------------------
    def export(self, module: str, name: str, seen=None) -> tuple[list[int], list[str]]:
        """What a module exports under a name: (definitions, module namespaces), followed through re-exports."""
        if seen is None:
            if (module, name) not in self._exports:
                self._exports[(module, name)] = self.export(module, name, set())
            return self._exports[(module, name)]
        if (module, name) in seen:
            return [], []
        seen.add((module, name))
        if module in self.ambient:
            return list(self.ix.members[module].get(name, [])), []
        facts = self.files.get(module)
        if facts is None:
            return [], []
        local = facts.exports.get(name)
        if isinstance(local, tuple):
            target = self.resolve(module, local[1])
            if target is None:
                return [], []
            return ([], [target]) if local[2] == "*" else self.export(target, local[2], seen)
        if isinstance(local, str):
            gids = self.ix.members[module].get(local, [])
            return (list(gids), []) if gids else self.binding(module, local, seen)
        gids, mods = [], []
        if not facts.exports and not facts.star_exports and facts.script:
            gids = list(self.ix.members[module].get(name, []))
        for spec in facts.star_exports if name != "default" else []:
            target = self.resolve(module, spec)
            if target is not None:
                found = self.export(target, name, seen)
                gids += found[0]
                mods += found[1]
        return gids, mods

    def binding(self, file: str, local: str, seen=None) -> tuple[list[int], list[str]]:
        """What an imported name of a file is: (definitions, module namespaces)."""
        facts = self.files.get(file)
        imported = facts.imports.get(local) if facts is not None else None
        if imported is None:
            return [], []
        target = self.resolve(file, imported[0])
        if target is None:
            return [], []
        if imported[1] == "*":
            return [], [target]
        return self.export(target, imported[1], seen if seen is not None else set())

    def class_of(self, text: str, file: str, owner: str = "") -> str | None:
        """A class, interface, enum, namespace or module named in a file (a type annotation, `new X`, `extends X`,
        a name used as a value): the scopes outward, the imports, the module-level constants holding a class or an
        object literal, the global scope, type aliases; then each `.part` inside it."""
        memo = (text, file, owner)
        if memo not in self._classes:
            self._classes[memo] = None
            self._classes[memo] = self._class_of(text, file, owner)
        return self._classes[memo]

    def _class_of(self, text: str, file: str, owner: str) -> str | None:
        clean = strip_generics(text.replace("typeof ", "").strip())
        if not clean or not all(p.isidentifier() or p.startswith("#") for p in clean.split(".")):
            return None
        head, *rest = clean.split(".")
        key = None
        scopes = list(dict.fromkeys(self.scope_keys(owner) + [file] if owner else [file]))
        for scope in scopes:
            key = self.ix.inner[scope].get(head) or self.held_class(scope, head)
            alias = self.ix.aliases.get(f"{scope}#{head}") if key is None else None
            key = key or (self.class_of(alias, file, owner) if alias else None)
            if key:
                break
        if key is None:
            gids, mods = self.binding(file, head)
            key = next((self.ix.units[g].qual for g in gids if self.ix.units[g].kind == "class"), None) or \
                (mods[0] if mods else None) or next((k for g in gids for k in [self.held_class_unit(g)] if k), None)
        if key is None:
            key = next((self.ix.units[g].qual for g in self.globals.get(head, []) if self.ix.units[g].kind == "class"),
                       None)
        for part in rest:
            if key is None:
                return None
            if self.is_module(key):
                gids, mods = self.export(key, part)
                key = next((self.ix.units[g].qual for g in gids if self.ix.units[g].kind == "class"), None) or \
                    (mods[0] if mods else None)
            else:
                key = self.ix.inner[key].get(part)
        return key

    def alias_of(self, text: str, file: str, owner: str = "") -> int | None:
        """The type alias a name names where it is written (T1'): the scopes outward, the imports, the global scope;
        `A.B` the alias B of the namespace or module A names."""
        memo = ("alias", text, file, owner)
        if memo not in self._classes:
            self._classes[memo] = self._alias_of(text, file, owner)
        return self._classes[memo]

    def _alias_of(self, text: str, file: str, owner: str) -> int | None:
        clean = strip_generics(text.strip())
        if not clean or not all(p.isidentifier() for p in clean.split(".")):
            return None
        *outer, last = clean.split(".")
        if outer:
            scope = self.class_of(".".join(outer), file, owner)
            gids = (self.export(scope, last)[0] if self.is_module(scope) else self.ix.members[scope].get(last, [])) \
                if scope else []
        else:
            scopes = list(dict.fromkeys(self.scope_keys(owner) + [file] if owner else [file]))
            gids = next((self.ix.members[s][last] for s in scopes if self.ix.members[s].get(last)), None) \
                or self.binding(file, last)[0] or self.globals.get(last, [])
        return next((g for g in gids if self.ix.units[g].kind == ALIAS_KIND), None)

    def declares_type(self, text: str, file: str, owner: str = "") -> bool:
        """Whether the program declares a type of this name where it is written (H1): an alias in scope or global, or a
        name imported from a module read (its class, interface or alias). A class it names is asked first (class_of)."""
        head = strip_generics(text).split(".")[0]
        for scope in list(dict.fromkeys(self.scope_keys(owner) + [file] if owner else [file])) + [GLOBAL_KEY]:
            if f"{scope}#{head}" in self.ix.aliases or f"{scope}.{head}" in self.ix.aliases:
                return True
        facts = self.files.get(file)
        imported = facts.imports.get(head) if facts is not None else None
        return imported is not None and self.resolve(file, imported[0]) is not None

    def held_class(self, scope: str, name: str) -> str | None:
        """A constant of the scope that holds a class or an object literal (`const A = class {}`, `const o = {}`)."""
        return next((k for g in self.ix.members[scope].get(name, []) for k in [self.held_class_unit(g)] if k), None)

    def held_class_unit(self, gid: int) -> str | None:
        """The object literal or class a constant's initializer is (through `as T` too: the leaves of its value), never
        an object type its annotation writes (T1')."""
        unit = self.ix.units[gid]
        if unit.kind not in ("const", "var"):
            return None
        return next((leaf[1][1:] for v in unit.values for leaf in templates.leaves(v)
                     if leaf[0] == "cast" and leaf[1].startswith("#") and SHAPE not in leaf[1]), None)

    def scope_keys(self, owner: str) -> list[str]:
        """The namespaces a unit is written in, innermost first, then its module."""
        keys, key = [], owner
        while key:
            if key in self.namespaces or key in self.files or key in self.ambient:
                keys.append(key)
            cls = self.ix.classes.get(key)
            key = cls.outer if cls is not None and cls.outer != key else ""
        return keys
