"""The declaration files a JavaScript/TypeScript program has (log/trace-core-plan.md, Task 4), chosen as the TypeScript
compiler chooses them (TypeScript handbook, tsconfig reference: `lib`, `target`, `noLib`, `types`, `typeRoots`,
`extends`, `explainFiles`; Triple-Slash Directives; TypeScript 6.0 release notes for the defaults):

- the lib files: each name of `lib` (`lib.<name>.d.ts`), else the default library of `target` — TypeScript's own
  table (`DEFAULT_LIBS`), `lib.<target>.full.d.ts` for any other target; with no target, ES5's before TypeScript 6 and
  the newest ECMAScript year's full library from 6 on; none under `noLib`. They come from the `lib` folder of the
  program's own TypeScript package (the nearest enclosing `node_modules/typescript`);
- the packages of types: those `types` names, each resolved as a type reference directive is (the `@types` package of
  that name in the type roots, else the name as a module: `<name>.d.ts` or a package folder), else (before TypeScript
  6) every package visible in a `node_modules/@types` of an enclosing folder, or in `typeRoots`; a package's entry is
  its package.json `types` (or `typings`), else `index.d.ts`;
- every file a read declaration file references (`/// <reference lib | path | types="..." />`), transitively.
"""
from __future__ import annotations

import json
import re
from pathlib import Path

from .js_config import strip_jsonc

CONFIG, PACKAGE = "tsconfig.json", "package.json"
MODULES, COMPILER, TYPES_SCOPE, LIB_FOLDER = "node_modules", "typescript", "@types", "lib"
OPTIONS = "compilerOptions"
LIB, TARGET, NO_LIB, TYPES, TYPE_ROOTS, EXTENDS, VERSION = "lib", "target", "noLib", "types", "typeRoots", "extends", \
    "version"
ENTRY_FIELDS = ("types", "typings")              # package.json fields naming a package's declarations (npm, TypeScript)
INDEX = "index.d.ts"
# TypeScript's libMap aliases and targetToLibMap (src/compiler/commandLineParser.ts, utilities.ts getDefaultLibFileName)
LIB_ALIASES = {"es6": "es2015", "es7": "es2016"}
DEFAULT_LIBS = {"es3": "lib.d.ts", "es5": "lib.d.ts", "es6": "lib.es6.d.ts", "es2015": "lib.es6.d.ts"}
FULL = "lib.{}.full.d.ts"
YEAR_FULL = re.compile(r"^lib\.es(\d{4})\.full\.d\.ts$")
FLOATING_TARGET_MAJOR = 6                         # TypeScript 6.0: the default target is the newest ECMAScript year
REFERENCE = re.compile(r'^\s*///\s*<reference\s+(lib|path|types)\s*=\s*["\']([^"\']+)["\']', re.MULTILINE)


def declaration_files(root: Path) -> list[Path]:
    """The declaration files the program at `root` (a folder or a file) has, in the order they are found."""
    folder = root if root.is_dir() else root.parent
    options = config_options(nearest(folder, CONFIG))
    compiler = nearest_package(folder, COMPILER)
    major = version_major(compiler)
    found: list[Path] = []
    if compiler is not None and not options.get(NO_LIB):
        libs = compiler / LIB_FOLDER
        names = options.get(LIB)
        files = [lib_file(libs, n) for n in names] if isinstance(names, list) else [default_lib(libs, options, major)]
        found += [f for f in files if f is not None]
    found += type_packages(folder, options, major)
    return follow(found, compiler)


def lib_file(libs: Path, name: str) -> Path | None:
    name = str(name).lower()
    path = libs / f"lib.{LIB_ALIASES.get(name, name)}.d.ts"
    return path if path.is_file() else None


def default_lib(libs: Path, options: dict, major: int) -> Path | None:
    target = str(options.get(TARGET) or "").lower()
    if not target and major >= FLOATING_TARGET_MAJOR:
        years = sorted((int(m.group(1)), p) for p in libs.glob("lib.es*.full.d.ts") for m in [YEAR_FULL.match(p.name)]
                       if m)
        return years[-1][1] if years else None
    name = DEFAULT_LIBS.get(target or "es5", FULL.format(target))
    path = libs / name
    return path if path.is_file() else None


def type_packages(folder: Path, options: dict, major: int) -> list[Path]:
    roots = [Path(r) if Path(r).is_absolute() else (options[CONFIG] / r) for r in options.get(TYPE_ROOTS) or []] \
        if options.get(TYPE_ROOTS) and options.get(CONFIG) else \
        [f / MODULES / TYPES_SCOPE for f in [folder, *folder.parents] if (f / MODULES / TYPES_SCOPE).is_dir()]
    names = options.get(TYPES)
    if isinstance(names, list):
        return [e for e in (type_reference(folder, str(n), roots) for n in names) if e is not None]
    packages = [p for r in roots for p in sorted(r.iterdir()) if p.is_dir()] if major < FLOATING_TARGET_MAJOR else []
    return [e for e in (package_entry(p) for p in packages) if e is not None]


def type_reference(folder: Path, name: str, roots: list[Path]) -> Path | None:
    """A type reference directive's name: the `@types` package of that name in the type roots, else the name resolved
    as a module from `folder` (`node_modules/<name>.d.ts`, a package folder's entry, its `index.d.ts`)."""
    for root in roots:
        if (root / name).is_dir():
            return package_entry(root / name)
    for f in [folder, *folder.parents]:
        base = f / MODULES / name
        if base.with_name(base.name + ".d.ts").is_file():
            return base.with_name(base.name + ".d.ts").resolve()
        if base.is_dir():
            return package_entry(base)
    return None


def package_entry(package: Path) -> Path | None:
    data = read_json(package / PACKAGE)
    for field in ENTRY_FIELDS:
        if isinstance(data.get(field), str) and (package / data[field]).is_file():
            return (package / data[field]).resolve()
    return (package / INDEX).resolve() if (package / INDEX).is_file() else None


def follow(files: list[Path], compiler: Path | None) -> list[Path]:
    """The files and everything their triple-slash directives reference, each once."""
    seen, out, todo = set(), [], [f.resolve() for f in files]
    while todo:
        path = todo.pop(0)
        if path in seen or not path.is_file():
            continue
        seen.add(path)
        out.append(path)
        text = path.read_text(encoding="utf-8", errors="replace")
        for kind, value in REFERENCE.findall(text):
            if kind == LIB and compiler is not None:
                found = lib_file(compiler / LIB_FOLDER, value)
            elif kind == TYPES:
                roots = [f / MODULES / TYPES_SCOPE for f in [path.parent, *path.parents]
                         if (f / MODULES / TYPES_SCOPE).is_dir()]
                found = type_reference(path.parent, value, roots)
            else:
                found = (path.parent / value).resolve()
            if found is not None:
                todo.append(found.resolve())
    return out


def config_options(path: Path | None, seen=None) -> dict:
    """A tsconfig's compiler options, over those of the configs it extends (a path, or a package's config)."""
    if path is None:
        return {}
    seen = seen or set()
    if path in seen:
        return {}
    seen.add(path)
    data = read_json(path)
    parent = data.get(EXTENDS)
    options = {}
    for spec in parent if isinstance(parent, list) else [parent] if isinstance(parent, str) else []:
        base = (path.parent / spec) if spec.startswith(".") else next(
            (f / MODULES / spec for f in [path.parent, *path.parents] if (f / MODULES / spec).exists()), None)
        if base is not None and base.is_dir():
            base = base / CONFIG
        if base is not None and not base.is_file() and base.with_suffix(".json").is_file():
            base = base.with_suffix(".json")
        options.update(config_options(base if base is not None and base.is_file() else None, seen))
    options.update(data.get(OPTIONS) or {})
    options[CONFIG] = path.parent
    return options


def nearest(folder: Path, name: str) -> Path | None:
    return next((f / name for f in [folder, *folder.parents] if (f / name).is_file()), None)


def nearest_package(folder: Path, name: str) -> Path | None:
    return next((f / MODULES / name for f in [folder, *folder.parents] if (f / MODULES / name).is_dir()), None)


def version_major(package: Path | None) -> int:
    text = str(read_json(package / PACKAGE).get(VERSION, "")) if package is not None else ""
    head = text.split(".", 1)[0]
    return int(head) if head.isdigit() else 0


def read_json(path: Path) -> dict:
    try:
        data = json.loads(strip_jsonc(path.read_text(encoding="utf-8-sig", errors="replace")))
    except (OSError, ValueError):
        return {}
    return data if isinstance(data, dict) else {}
