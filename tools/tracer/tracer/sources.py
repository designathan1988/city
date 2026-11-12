"""Which file a source names (an import's, an include's, a base written as a path). A source written as relative is
the file at that path from the one naming it; any other is first looked for where the language's own project file
says (`configs`: a path alias, the base folder, the folder a scheme stands for, an include folder), then as the file
whose path ends as the source is written, the nearest to the one naming it. A source may leave out the file's
extension, or name a folder for the file a language says stands for it (`languages`).
"""
from __future__ import annotations

import os
import posixpath
from collections import defaultdict
from pathlib import Path

from .configs import norm

SCHEME = "://"                   # what ends the scheme of a source written as a locator (RFC 3986)
RELATIVE = "."                   # what a source written from the file's own folder starts with


def ends(path: str, tail: str) -> bool:
    return path == tail or path.endswith("/" + tail)


class Sources:
    def __init__(self, program):
        self.p = program
        self.by_path, self.by_stem, self.by_tail = {}, defaultdict(list), defaultdict(list)
        self.by_file = {}                        # a file's whole path, as the system writes it -> its module
        self.cache, self.reach_of = {}, {}
        for f in program.files:
            stem = posixpath.splitext(f.display)[0]
            self.by_path[f.display] = f.module
            self.by_stem[stem].append(f.module)
            self.by_tail[posixpath.basename(stem)].append(f.module)
            if f.path is not None:
                self.by_file[norm(f.path)] = f.module

    def module_at(self, path) -> int | None:
        return self.by_file.get(norm(path))

    def shown(self, path) -> str | None:
        """A whole path as the files read are shown (from the base they share), None where it is outside it."""
        base = self.p.base
        if base is None:
            return None
        try:
            rel = os.path.relpath(str(path), str(base))
        except ValueError:                       # another drive
            return None
        rel = rel.replace(os.sep, "/")
        return None if rel.startswith("..") else rel

    def resolve(self, f, source: str) -> list:
        """The modules a source written in that file names."""
        key = (posixpath.dirname(f.display), f.lang.name, source)
        if key not in self.cache:
            self.cache[key] = self.find(f, source)
        return self.cache[key]

    def reach(self, m: int) -> dict:
        """The files whose names are a file's own, in the order met: itself, each one it includes, and theirs."""
        if m not in self.reach_of:
            out, order = {m: None}, [m]
            for t in order:                          # grows as it is gone through
                f = self.p.files[self.p.defs[t].file]
                for u in [u for source in f.includes for u in self.resolve(f, source) if u not in out]:
                    out[u] = None
                    order.append(u)
            self.reach_of[m] = out
        return self.reach_of[m]

    def sees(self, file: int, other: int) -> bool:
        """Whether the names a file writes at its own level are names of another file (by their indexes)."""
        return self.p.files[other].module in self.reach(self.p.files[file].module)

    def find(self, f, source: str) -> list:
        here = posixpath.dirname(f.display)
        scheme, _, rest = source.partition(SCHEME)
        written = rest if rest else source
        relative = written.startswith(RELATIVE)
        if relative or (f.lang.beside and not rest):
            found = self.at(posixpath.normpath(posixpath.join(here, written)), f.lang)
            if found or relative:
                return found
        for place in self.places(f, scheme if rest else "", written):
            shown = self.shown(place)
            found = self.at(posixpath.normpath(shown), f.lang) if shown is not None else []
            if found:
                return found
        return self.nearest(here, self.ending(written, f.lang))

    def places(self, f, scheme: str, written: str) -> list:
        """Where the language's own project file says a source may be: whole paths, in order."""
        if f.path is None or not f.lang.project:
            return []
        project = self.p.configs.of(f.lang.project, f.path)
        if scheme:
            folder = project.schemes.get(scheme)
            return [str(Path(folder) / written)] if folder is not None else []
        return project.places(written) + [str(Path(folder) / written) for folder in project.folders(f.path)]

    def at(self, path: str, lang) -> list:
        """The file at a path: as written, with an extension it leaves out, without the one it writes, or the file
        standing for the folder it names."""
        if path in self.by_path:
            return [self.by_path[path]]
        for stem in (path, posixpath.splitext(path)[0]) + tuple(f"{path}/{name}" for name in lang.folder_file):
            if stem in self.by_stem:
                return list(self.by_stem[stem])
        return []

    def ending(self, written: str, lang) -> list:
        """The files whose path ends as written: with that very name first, else as `at` reads a path."""
        bare = posixpath.splitext(written)[0]
        shown = lambda m: self.p.files[self.p.defs[m].file].display                      # noqa: E731
        exact = [m for m in self.by_tail.get(posixpath.basename(bare), ()) if ends(shown(m), written)]
        if exact:
            return exact
        out = []
        for tail, wanted in [(posixpath.basename(bare), (written, bare))] + \
                [(name, (f"{written}/{name}",)) for name in lang.folder_file]:
            for m in self.by_tail.get(tail, ()):
                if any(ends(posixpath.splitext(shown(m))[0], w) for w in wanted):
                    out.append(m)
        return list(dict.fromkeys(out))

    def nearest(self, here: str, modules: list) -> list:
        """Of several files, the ones sharing the longest path with the folder naming them."""
        if len(modules) < 2:
            return modules
        parts = here.split("/") if here else []

        def shared(m: int) -> int:
            other = posixpath.dirname(self.p.files[self.p.defs[m].file].display).split("/")
            count = 0
            while count < min(len(parts), len(other)) and parts[count] == other[count]:
                count += 1
            return count
        best = max(shared(m) for m in modules)
        return [m for m in modules if shared(m) == best]
