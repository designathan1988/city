"""The whole program as tables: every definition by name, by qualified name and by class; classes with their bases
resolved; the names the engine knows (class_name, autoloads, C++ bindings); scene connections and data mentions.
"""
from __future__ import annotations

from collections import defaultdict

from . import cpp_ambig
from .cpp_names import strip_templates
from .js_modules import JsModules
from .model import ENGINE_CALLBACKS, FUNCTION_KINDS, Unit

ENGINE_SIGNAL = "engine_signal"


class Index:
    def __init__(self, all_facts: list):
        self.units: list[Unit] = []
        self.offset: dict[str, int] = {}
        self.by_name = defaultdict(list)
        self.by_qual = defaultdict(list)
        self.members = defaultdict(lambda: defaultdict(list))
        self.classes = {}
        self.inner = defaultdict(dict)            # class key -> inner name -> key (gd inner classes and enums)
        self.gd_globals = {}                      # class_name and autoload name -> gd class key
        self.cpp_simple = defaultdict(list)       # simple class name -> cpp keys
        self.usings = defaultdict(list)
        self.aliases = {}
        self.bound = defaultdict(list)            # bound name -> cpp gids
        self.bound_in = defaultdict(lambda: defaultdict(list))  # cpp class key -> bound name -> gids
        self.props = defaultdict(list)            # property name -> (class key, setter name, getter name)
        self.connections = []                     # (signal, from key, to key, method, file, line)
        self.mentions = defaultdict(list)
        self.subclasses = defaultdict(set)
        self.bases = {}
        self.engine_signals = {}
        self.virtual_quals = set()
        self.includes = {}                        # file -> files its `#include "..."` names (resolved)
        self._visible = {}
        self._lookups = {}
        self._overrides = {}
        autoloads, bindings = {}, []
        for facts in all_facts:
            self.offset[facts.path] = len(self.units)
            for unit in facts.units:
                unit.gid = len(self.units)
                self.units.append(unit)
                self.by_name[unit.name].append(unit.gid)
                self.by_qual[unit.qual].append(unit.gid)
                if unit.kind != "lambda":         # a function written in a function is its local, no member (L1)
                    self.members[unit.owner][unit.name].append(unit.gid)
                if unit.lang == "cpp" and unit.virtual:
                    self.virtual_quals.add(unit.qual)
            for cls in facts.classes:
                self.classes.setdefault(cls.key, cls)
                if cls.lang == "gd" and cls.outer:
                    self.inner[cls.outer][cls.name] = cls.key
                elif cls.lang == "gd" and cls.name:
                    self.gd_globals[cls.name] = cls.key
                elif cls.lang == "cpp":
                    self.cpp_simple[cls.name].append(cls.key)
                elif cls.lang == "js" and cls.outer and cls.name:
                    self.inner[cls.outer].setdefault(cls.name, cls.key)
            self.usings[facts.path] = facts.usings
            self.includes[facts.path] = facts.includes
            self.aliases.update(facts.aliases)
            autoloads.update(facts.autoloads)
            bindings += [(b, facts.path) for b in facts.bindings]
            for signal, src, dst, method, line in facts.connections:
                self.connections.append((signal, src, dst, method, facts.path, line))
            if facts.lang in ("data", "project", "scene"):
                for text, line in facts.mentions:
                    self.mentions[text].append((facts.path, line))
        known = set(self.offset)
        for path, names in self.includes.items():
            self.includes[path] = [f for f in (self.include_file(path, n, known) for n in names) if f]
        self.js = JsModules(all_facts, self)
        for name, path in autoloads.items():
            if path in self.classes:
                self.gd_globals[name] = path
        for key, cls in self.classes.items():
            for base in self.base_keys(cls):
                self.subclasses[base].add(key)
        for key, cls in self.classes.items():
            if cls.lang == "cpp" and self.members[key].get("_bind_methods"):
                cls.engine_visible = True
        for binding, path in bindings:
            self.bind(binding, path)
        cpp_ambig.decide(self)                    # `T x(a, b);`: a function or an object, now that names are known

    # ---- the translation unit (C6) -----------------------------------------------------------------------------
    @staticmethod
    def include_file(path: str, name: str, known: set) -> str | None:
        """`#include "x"`: searched next to the including file, then from each folder above it."""
        folders = path.split("/")[:-1]
        for depth in range(len(folders), -1, -1):
            parts = []
            for piece in folders[:depth] + name.split("/"):
                if piece == "..":
                    parts = parts[:-1]
                elif piece not in (".", ""):
                    parts.append(piece)
            if "/".join(parts) in known:
                return "/".join(parts)
        return None

    def visible(self, path: str) -> set:
        """A file and everything it includes, transitively."""
        if path not in self._visible:
            seen, todo = set(), [path]
            while todo:
                f = todo.pop()
                if f not in seen:
                    seen.add(f)
                    todo += self.includes.get(f, [])
            self._visible[path] = seen
        return self._visible[path]

    def tu_usings(self, path: str) -> list[str]:
        """The `using namespace` directives the translation unit carries (its own and its headers')."""
        return list(dict.fromkeys(u for f in self.visible(path) for u in self.usings.get(f, [])))

    # ---- classes -----------------------------------------------------------------------------------------------
    def base_keys(self, cls) -> list[str]:
        if cls.key in self.bases:
            return self.bases[cls.key]
        self.bases[cls.key] = []
        found = []
        for spec in cls.bases:
            key = self.gd_class(spec, cls.key) if cls.lang == "gd" else \
                self.js.class_of(spec, cls.file, cls.outer) if cls.lang == "js" else \
                self.cpp_class(spec, cls.outer, cls.file)
            if key:
                found.append(key)
        self.bases[cls.key] = found
        return found

    def gd_path(self, path: str, from_key: str) -> str | None:
        if path.startswith("res://"):
            return path if path in self.classes else None
        folder = from_key.split(".gd", 1)[0].rsplit("/", 1)[0]
        parts = []
        for piece in f"{folder}/{path}".replace("res://", "").split("/"):
            if piece == "..":
                parts = parts[:-1]
            elif piece not in (".", ""):
                parts.append(piece)
        key = "res://" + "/".join(parts)
        return key if key in self.classes else None

    def gd_class(self, spec: str, from_key: str) -> str | None:
        """A GDScript type name or path, seen from a class: inner classes outward, then globals, then const aliases."""
        if not spec:
            return None
        if spec.startswith("path:"):
            path, *inner = spec[5:].split("|")
            key = self.gd_path(path, from_key)
            for part in inner:
                key = self.inner[key].get(part) if key else None
            return key
        spec = spec.split("[", 1)[0].strip()
        head, *rest = spec.split(".")
        key = None
        scope = from_key
        while scope and key is None:
            key = self.inner[scope].get(head) or self.const_alias(scope, head)
            scope = self.classes[scope].outer if scope in self.classes else ""
        key = key or self.gd_globals.get(head)
        if key is None:
            engine = self.engine_class(head)
            return engine
        for part in rest:
            key = self.inner[key].get(part) if key else None
        return key

    def const_alias(self, key: str, name: str) -> str | None:
        for gid in self.members[key].get(name, []):
            unit = self.units[gid]
            for value in unit.values if unit.kind == "const" else []:
                if value[0] == "cast" and value[1].startswith("path:"):
                    return self.gd_path(value[1][5:], key)
        return None

    def engine_class(self, name: str) -> str | None:
        """A C++ class the engine knows by name (it binds methods), seen from GDScript."""
        keys = [k for k in self.cpp_simple.get(name, []) if self.classes[k].engine_visible]
        return keys[0] if len(keys) == 1 else None

    def scopes(self, prefix: str) -> list[str]:
        parts = prefix.split("::") if prefix else []
        return ["::".join(parts[:i]) for i in range(len(parts), -1, -1)]

    def cpp_class(self, text: str, prefix: str, file: str, depth: int = 0) -> str | None:
        """A C++ class named in a scope: enclosing scopes outward, aliases, `using namespace`, then a unique name."""
        name = strip_templates(text).replace("struct ", "").replace("class ", "").lstrip(":")
        if not name or depth > len(self.aliases):
            return None
        for scope in self.scopes(prefix) + self.tu_usings(file):
            key = f"{scope}::{name}" if scope else name
            if key in self.classes and self.classes[key].lang == "cpp":
                return key
            if key in self.aliases:
                return self.cpp_class(self.aliases[key], scope, file, depth + 1)
        # declared in the translation unit under a namespace the code reaches through its own scopes
        visible = self.visible(file) if file else set()
        keys = [k for k in self.cpp_simple.get(name.split("::")[-1], []) if (k == name or k.endswith("::" + name))
                and self.classes[k].file in visible]
        return keys[0] if len(keys) == 1 else None

    def lookup(self, key: str, name: str, seen=None) -> list[int]:
        """A member by name in a class, then up its bases (the language's inheritance rule)."""
        if seen is None:                      # the answer for a class and a name never changes: kept
            if (key, name) not in self._lookups:
                self._lookups[(key, name)] = self.lookup(key, name, set())
            return self._lookups[(key, name)]
        if not key or key in seen:
            return []
        seen.add(key)
        if self.js.is_module(key):            # a module namespace: what it exports (J3)
            return list(self.js.export(key, name)[0])
        found = list(self.members[key].get(name, []))
        if found:
            return found
        cls = self.classes.get(key)
        for base in self.base_keys(cls) if cls else []:
            found += self.lookup(base, name, seen)
        if not found and cls is not None and cls.lang == "cpp" and cls.engine_visible:
            found = list(self.bound_in[key].get(name, []))
        return found

    def overrides(self, key: str, name: str) -> list[int]:
        """Definitions of the same name in every class that derives from `key`: what a virtual call may reach."""
        if (key, name) in self._overrides:
            return self._overrides[(key, name)]
        found, todo, seen = [], list(self.subclasses.get(key, ())), set()
        while todo:
            sub = todo.pop()
            if sub in seen:
                continue
            seen.add(sub)
            found += [g for g in self.members[sub].get(name, []) if self.units[g].kind in FUNCTION_KINDS]
            todo += list(self.subclasses.get(sub, ()))
        self._overrides[(key, name)] = found
        return found

    # ---- the engine's names ------------------------------------------------------------------------------------
    def bind(self, binding, path) -> None:
        name, target, owner, line, kind = binding
        if kind == "signal":
            unit = Unit("signal", "cpp", name, f"{owner}::{name}", owner, path, line, line, gid=len(self.units))
            self.units.append(unit)
            self.by_name[name].append(unit.gid)
            self.members[owner][name].append(unit.gid)
            return
        if kind == "property":
            setter, getter = target.split("|")
            self.props[name].append((owner, setter, getter))
            return
        quals = [q for q in (f"{s}::{target}" if s else target for s in self.scopes(owner)) if q in self.by_qual]
        for gid in self.definitions(self.by_qual[quals[0]]) if quals else []:
            self.units[gid].bound_as.append(name)
            self.bound[name].append(gid)
            self.bound_in[owner][name].append(gid)

    def definitions(self, gids: list[int]) -> list[int]:
        """A C++ function's bodies when it has any, else its declarations."""
        bodies = [g for g in gids if not self.units[g].decl_only]
        return bodies or list(gids)

    def engine_signal(self, name: str) -> int:
        """A signal the engine emits (declared by no code): one entry point per name."""
        if name not in self.engine_signals:
            unit = Unit(ENGINE_SIGNAL, "gd", name, f"engine:{name}", "", "<engine>", 0, 0, gid=len(self.units))
            self.units.append(unit)
            self.engine_signals[name] = unit.gid
        return self.engine_signals[name]

    def is_engine_callback(self, unit: Unit) -> bool:
        return unit.name in ENGINE_CALLBACKS and unit.kind in FUNCTION_KINDS
