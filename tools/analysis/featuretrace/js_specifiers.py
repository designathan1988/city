"""JavaScript / TypeScript module specifiers (J3): where `import ... from "m"` leads, as TypeScript's resolution modes
resolve it (TypeScript handbook, Modules reference): a relative path with extension substitution (`./a.js` -> `a.ts`),
an extensionless path and a directory's `index` file; tsconfig `paths` patterns and `baseUrl`; a package in
`node_modules` by its package.json `exports` (conditions `types`, `import`, `module`, `browser`, `default`), `module`,
`main`, `types`; an ambient `declare module "m"`. `js_modules.JsModules` is built on this.
"""
from __future__ import annotations

import posixpath

from .js_decl import AMBIENT

SOURCE_EXTS = (".ts", ".tsx", ".d.ts", ".mts", ".cts", ".js", ".jsx", ".mjs", ".cjs")
SUBSTITUTE = {".js": (".ts", ".tsx", ".d.ts"), ".jsx": (".tsx",), ".mjs": (".mts", ".d.mts"),
              ".cjs": (".cts", ".d.cts")}
CONDITIONS = ("types", "import", "module", "browser", "default", "require", "node")
MAIN_FIELDS = ("module", "main", "types", "typings")


class Specifiers:
    """Needs `files` (the modules read), `configs` (tsconfig by folder), `packages` (package.json by folder),
    `package_dirs`, `ambient` and the memo `_resolved`."""

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
