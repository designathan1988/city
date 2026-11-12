"""The files a language itself reads to resolve a name, read as that language reads them. They are part of each
language's rules, not of a project: the same reading serves any project written in the language. Each file is
found the way the language's own tools find it, the nearest one up the folders from the file being read, and is
only ever read.

- TypeScript and JavaScript (TypeScript handbook, Module Resolution; tsconfig reference): `tsconfig.json` or
  `jsconfig.json`; `extends` (a path, or a package's file under `node_modules`); `compilerOptions.baseUrl`, the
  folder a name not written as relative is looked for from; `compilerOptions.paths`, patterns with at most one `*`
  mapping a name to the places it is looked for, in order, the pattern with the longest prefix first, relative to
  `baseUrl`, else to the file that writes them. The file is JSON with comments and trailing commas.
- GDScript (Godot reference, Singletons (Autoload); File system, `res://`): `project.godot`; its `[autoload]` entries
  written `Name="*res://path"` are globals, each an object of the script the path names; `res://` is the folder of
  that file.
- C++ (the compilation database read by clang's tools): `compile_commands.json`, beside the file's folders or in a
  `build` folder there; the include folders each unit is compiled with (`-I`, `-isystem`, `-iquote`, `/I`).
"""
from __future__ import annotations

import json
import os
import re
import shlex
from dataclasses import dataclass, field
from pathlib import Path

# TypeScript and JavaScript
TS_FILES = ("tsconfig.json", "jsconfig.json")
TS_OPTIONS, TS_PATHS, TS_BASE, TS_EXTENDS = "compilerOptions", "paths", "baseUrl", "extends"
PACKAGES = "node_modules"
JSON_EXTENSION = ".json"
STAR = "*"
# GDScript
GODOT_FILES = ("project.godot",)
GODOT_GLOBALS = "autoload"
GODOT_ENABLED = "*"
GODOT_SCHEME = "res"
# C++
DATABASE_FILES = ("compile_commands.json", "build/compile_commands.json")
DATABASE_DIRECTORY, DATABASE_FILE, DATABASE_COMMAND, DATABASE_ARGUMENTS = "directory", "file", "command", "arguments"
INCLUDE_FLAGS = ("-I", "-isystem", "-iquote", "/I")

STRINGS = r'"(?:\\.|[^"\\])*"'
COMMENTS = re.compile(STRINGS + r"|//[^\n]*|/\*.*?\*/", re.S)
TRAILING = re.compile(STRINGS + r"|,(?=\s*[}\]])", re.S)
SECTION = re.compile(r"^\s*\[([^\]]+)\]\s*$")
ENTRY = re.compile(r'^\s*([A-Za-z_][A-Za-z0-9_]*)\s*=\s*"(.*)"\s*$')
SCHEME = "://"


@dataclass
class Project:
    """What one language file says about resolving names."""
    file: Path | None = None
    aliases: list = field(default_factory=list)     # (pattern, [places with the pattern's `*`]), longest prefix first
    base: Path | None = None                        # where a name not written as relative is looked for
    globals: dict = field(default_factory=dict)     # name -> the script an object of which it is
    schemes: dict = field(default_factory=dict)     # scheme -> the folder it stands for
    includes: dict = field(default_factory=dict)    # a unit's file -> its include folders
    every_include: list = field(default_factory=list)   # the include folders of every unit (for a header)
    problem: str = ""                               # why the file could not be read

    def places(self, written: str) -> list:
        """Where a name not written as relative may be (TypeScript `paths`, then `baseUrl`), in order."""
        out = []
        for pattern, targets in self.aliases:
            if STAR in pattern:
                head, tail = pattern.split(STAR, 1)
                if written.startswith(head) and written.endswith(tail) and len(written) >= len(head) + len(tail):
                    middle = written[len(head):len(written) - len(tail)]
                    out += [target.replace(STAR, middle, 1) for target in targets]
                    break
            elif pattern == written:
                out += list(targets)
                break
        if self.base is not None:
            out.append(str(self.base / written))
        return out

    def folders(self, path: Path) -> list:
        """The include folders of a file: its unit's, else every unit's."""
        return self.includes.get(norm(path), self.every_include)


EMPTY = Project()


def norm(path) -> str:
    return os.path.normcase(os.path.abspath(str(path)))


def jsonc(text: str):
    """JSON with comments and trailing commas, as TypeScript reads its configuration."""
    keep = lambda m: m.group() if m.group().startswith('"') else ""     # noqa: E731
    return json.loads(TRAILING.sub(keep, COMMENTS.sub(keep, text)))


def read_typescript(path: Path, seen=None) -> Project:
    """A tsconfig/jsconfig file and the ones it extends: the aliases, the base folder."""
    seen = set() if seen is None else seen
    if norm(path) in seen:
        return Project(file=path)
    seen.add(norm(path))
    data = jsonc(path.read_text(encoding="utf-8-sig"))
    options = data.get(TS_OPTIONS) or {}
    parents = data.get(TS_EXTENDS) or []
    project = Project(file=path)
    for parent in [parents] if isinstance(parents, str) else parents:
        found = extended(path.parent, parent)
        if found is not None:
            inner = read_typescript(found, seen)
            project.aliases, project.base = inner.aliases or project.aliases, inner.base or project.base
    if TS_BASE in options:
        project.base = (path.parent / options[TS_BASE]).resolve()
    if TS_PATHS in options:
        root = project.base or path.parent
        pairs = [(pattern, [str((root / target).resolve()) if not Path(target).is_absolute() else target
                            for target in targets])
                 for pattern, targets in options[TS_PATHS].items() if isinstance(targets, list)]
        project.aliases = sorted(pairs, key=lambda pair: -len(pair[0].split(STAR, 1)[0]))
    return project


def extended(folder: Path, written: str) -> Path | None:
    """The file an `extends` names: a path from the file, or a package's file under `node_modules` up the folders."""
    tries = []
    if written.startswith(".") or Path(written).is_absolute():
        tries.append(folder / written)
    else:
        up = folder
        while True:
            tries += [up / PACKAGES / written, up / PACKAGES / written / TS_FILES[0]]
            if up.parent == up:
                break
            up = up.parent
    for one in tries:
        for candidate in (one, Path(str(one) + JSON_EXTENSION)):
            if candidate.is_file():
                return candidate.resolve()
    return None


def read_godot(path: Path) -> Project:
    """A project.godot: the globals its `[autoload]` enables, and the folder `res://` stands for."""
    project = Project(file=path, schemes={GODOT_SCHEME: path.parent.resolve()})
    section = ""
    for line in path.read_text(encoding="utf-8-sig").splitlines():
        head = SECTION.match(line)
        if head:
            section = head.group(1).strip()
            continue
        entry = ENTRY.match(line) if section == GODOT_GLOBALS else None
        if entry and entry.group(2).startswith(GODOT_ENABLED):
            written = entry.group(2)[len(GODOT_ENABLED):]
            scheme, _, rest = written.partition(SCHEME)
            if rest and scheme in project.schemes:
                project.globals[entry.group(1)] = (project.schemes[scheme] / rest).resolve()
    return project


def read_database(path: Path) -> Project:
    """A compilation database: each unit's include folders."""
    project = Project(file=path)
    every = {}
    for entry in json.loads(path.read_text(encoding="utf-8-sig")):
        folder = path.parent / entry.get(DATABASE_DIRECTORY, ".")      # a relative folder is from the database's
        words = entry.get(DATABASE_ARGUMENTS) or shlex.split(entry.get(DATABASE_COMMAND, "").replace("\\", "/"))
        found, take = [], False
        for word in words:
            if take:
                found.append(word)
                take = False
                continue
            for flag in INCLUDE_FLAGS:
                if word == flag:
                    take = True
                elif word.startswith(flag) and len(word) > len(flag):
                    found.append(word[len(flag):])
        dirs = [(folder / one).resolve() for one in found]
        unit = folder / entry.get(DATABASE_FILE, "")
        project.includes[norm(unit)] = dirs
        every.update(dict.fromkeys(dirs))
    project.every_include = list(every)
    return project


READERS = {"typescript": (TS_FILES, read_typescript), "godot": (GODOT_FILES, read_godot),
           "database": (DATABASE_FILES, read_database)}


class Configs:
    """The language files that apply to each folder, each read once."""

    def __init__(self):
        self.near = {}           # (reader, folder) -> Project
        self.read = {}           # file -> Project
        self.notes = []          # what was read, or why not: one line each

    def of(self, reader: str, path: Path) -> Project:
        if not reader or reader not in READERS:
            return EMPTY
        return self.folder(reader, Path(path).parent)

    def folder(self, reader: str, folder: Path) -> Project:
        key = (reader, norm(folder))
        if key not in self.near:
            names, parse = READERS[reader]
            found = next((folder / name for name in names if (folder / name).is_file()), None)
            if found is not None:
                self.near[key] = self.load(found, parse)
            elif folder.parent == folder:
                self.near[key] = EMPTY
            else:
                self.near[key] = self.folder(reader, folder.parent)
        return self.near[key]

    def load(self, path: Path, parse) -> Project:
        key = norm(path)
        if key not in self.read:
            try:
                project = parse(path)
                size = len(project.aliases) + len(project.globals) + len(project.every_include)
                self.notes.append(f"{path}: {size} names")
            except (OSError, ValueError, TypeError, AttributeError) as error:
                project = Project(file=path, problem=str(error))
                self.notes.append(f"{path}: unreadable ({error})")
            self.read[key] = project
        return self.read[key]
