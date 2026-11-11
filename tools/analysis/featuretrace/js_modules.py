"""JavaScript / TypeScript modules (J3, J7, J8): where a specifier leads, what a module exports, what an import binds,
and the global scope.

A specifier resolves as TypeScript's resolution modes do (TypeScript handbook, Modules reference): a relative path with
extension substitution (`./a.js` -> `a.ts`), an extensionless path and a directory's `index` file; tsconfig `paths`
patterns and `baseUrl`; a package in `node_modules` by its package.json `exports` (conditions `types`, `import`,
`module`, `browser`, `default`), `module`, `main`, `types`; an ambient `declare module "m"`. An export is followed
through `export ... from` and `export *` to its definition. Scripts (files with no import or export) and `declare
global` put their names in the global scope.
"""
from __future__ import annotations

import posixpath
from collections import defaultdict

from .js_decl import AMBIENT, GLOBAL_KEY

SOURCE_EXTS = (".ts", ".tsx", ".d.ts", ".mts", ".cts", ".js", ".jsx", ".mjs", ".cjs")
SUBSTITUTE = {".js": (".ts", ".tsx", ".d.ts"), ".jsx": (".tsx",), ".mjs": (".mts", ".d.mts"),
              ".cjs": (".cts", ".d.cts")}
CONDITIONS = ("types", "import", "module", "browser", "default", "require", "node")
MAIN_FIELDS = ("module", "main", "types", "typings")


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


class JsModules:
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

    # ---- specifiers ---------------------------------------------------------------------------------------------
    def resolve(self, from_file: str, spec: str | None) -> str | None:
        """The module a specifier names, seen from a file (a file read, an ambient module), else None."""
        if not spec:
            return None
        key = (from_file, spec)
        if key not in self._resolved:
            self._resolved[key] = self._resolve(from_file, spec)
        return self._resolved[key]

    def _resolve(self, from_file: str, spec: str) -> str | None:
        folder = posixpath.dirname(from_file)
        if spec.startswith(("./", "../")) or spec in (".", ".."):
            return self.file_at(posixpath.normpath(posixpath.join(folder, spec)))
        for base, config in self.tsconfigs_of(folder):
            found = self.by_paths(base, config, spec)
            if found:
                return found
        found = self.package(folder, spec)
        if found:
            return found
        return AMBIENT + spec if AMBIENT + spec in self.ambient else None

    def file_at(self, path: str) -> str | None:
        """A path as written, with its extension substituted, with an extension added, or as a directory's index."""
        stem, ext = posixpath.splitext(path)
        tries = [path] + [stem + e for e in SUBSTITUTE.get(ext, ())] + [path + e for e in SOURCE_EXTS] + \
            [posixpath.join(path, "index" + e) for e in SOURCE_EXTS]
        return next((t for t in tries if t in self.files), None)

    def tsconfigs_of(self, folder: str):
        """The tsconfig/jsconfig files above a folder, nearest first, each with the folder its paths are based on."""
        while True:
            config = self.configs.get(folder)
            if config is not None:
                base = posixpath.normpath(posixpath.join(folder, config.get("baseUrl") or "."))
                yield base, config
            if not folder or folder == posixpath.dirname(folder):
                return
            folder = posixpath.dirname(folder)

    def by_paths(self, base: str, config: dict, spec: str) -> str | None:
        for pattern, targets in (config.get("paths") or {}).items():
            head, star, tail = pattern.partition("*")
            if not (spec.startswith(head) and spec.endswith(tail) and (star or spec == pattern)):
                continue
            middle = spec[len(head):len(spec) - len(tail)] if star else ""
            for target in targets if isinstance(targets, list) else [targets]:
                found = self.file_at(posixpath.normpath(posixpath.join(base, target.replace("*", middle))))
                if found:
                    return found
        return self.file_at(posixpath.normpath(posixpath.join(base, spec))) if config.get("baseUrl") else None

    def package(self, folder: str, spec: str) -> str | None:
        """`pkg` / `pkg/sub` / `@scope/pkg/sub` in the nearest node_modules holding the package (read as a root)."""
        parts = spec.split("/")
        count = 2 if spec.startswith("@") else 1
        name, sub = "/".join(parts[:count]), "/".join(parts[count:])
        while True:
            root = posixpath.join(folder, "node_modules", name) if folder else posixpath.join("node_modules", name)
            config = self.packages.get(root)
            if root in self.package_dirs:
                return self.package_entry(root, config or {}, sub)
            if not folder or folder == posixpath.dirname(folder):
                return None
            folder = posixpath.dirname(folder)

    def package_entry(self, root: str, config: dict, sub: str) -> str | None:
        exports = config.get("exports")
        if exports is not None:
            target = self.export_target(exports, "./" + sub if sub else ".")
            if target:
                return self.file_at(posixpath.normpath(posixpath.join(root, target)))
        if sub:
            return self.file_at(posixpath.normpath(posixpath.join(root, sub)))
        for field in MAIN_FIELDS:
            if isinstance(config.get(field), str):
                found = self.file_at(posixpath.normpath(posixpath.join(root, config[field])))
                if found:
                    return found
        return self.file_at(root)

    def export_target(self, exports, subpath: str) -> str | None:
        """package.json `exports` for a subpath: a string, a conditions object, a subpath map (with `*`)."""
        if isinstance(exports, str):
            return exports if subpath == "." else None
        if isinstance(exports, list):
            return next((t for t in (self.export_target(e, subpath) for e in exports) if t), None)
        if not isinstance(exports, dict):
            return None
        if not any(k.startswith(".") for k in exports):        # conditions at the top
            return self.condition(exports) if subpath == "." else None
        if subpath in exports:
            return self.condition(exports[subpath])
        for pattern, target in exports.items():
            head, star, tail = pattern.partition("*")
            if star and subpath.startswith(head) and subpath.endswith(tail):
                chosen = self.condition(target)
                return chosen.replace("*", subpath[len(head):len(subpath) - len(tail)]) if chosen else None
        return None

    def condition(self, target) -> str | None:
        if isinstance(target, str):
            return target
        if isinstance(target, list):
            return next((t for t in (self.condition(e) for e in target) if t), None)
        if isinstance(target, dict):
            for name in CONDITIONS:
                if name in target:
                    found = self.condition(target[name])
                    if found:
                        return found
        return None

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
        unit = self.ix.units[gid]
        if unit.kind not in ("const", "var"):
            return None
        return next((v[1][1:] for v in unit.values if v[0] == "cast" and v[1].startswith("#")), None)

    def scope_keys(self, owner: str) -> list[str]:
        """The namespaces a unit is written in, innermost first, then its module."""
        keys, key = [], owner
        while key:
            if key in self.namespaces or key in self.files or key in self.ambient:
                keys.append(key)
            cls = self.ix.classes.get(key)
            key = cls.outer if cls is not None and cls.outer != key else ""
        return keys
