"""Reading: the files under the roots that a language of `languages` reads, each parsed by its grammar and matched
against its pattern files. What comes out is the same for every language: a list of facts, each the capture that
anchors it, its node and its parts by name. Which files belong to a project is what its version control says (the
files git tracks or does not ignore); where there is none, every file outside hidden folders.
"""
from __future__ import annotations

import os
import subprocess
from dataclasses import dataclass, field
from pathlib import Path

from tree_sitter import QueryCursor

from .languages import BY_EXTENSION, Language
from .vocab import ANCHORS

LISTING = ("git", "ls-files", "--cached", "--others", "--exclude-standard")
LISTING_SECONDS = 60
HIDDEN, PRIVATE = ".", "_"


@dataclass
class Parsed:
    """One file read: where it is, its language, its tree and the facts its patterns found."""
    path: Path
    display: str
    lang: Language
    tree: object
    facts: list = field(default_factory=list)      # (anchor, node, {part: node})


def listed(root: Path) -> list[Path]:
    """The files of a folder: those its version control tracks or does not ignore, else all outside hidden folders."""
    try:
        done = subprocess.run(LISTING, cwd=root, capture_output=True, text=True, timeout=LISTING_SECONDS, check=True)
        return [root / line for line in done.stdout.splitlines() if line]
    except (OSError, subprocess.SubprocessError):
        found = []
        for folder, folders, names in os.walk(root):
            folders[:] = sorted(f for f in folders if not f.startswith(HIDDEN))
            found += [Path(folder) / name for name in sorted(names)]
        return found


def discover(roots: list[Path]) -> list[Path]:
    """Every file under the roots a language reads, once, in a stable order."""
    found = {}
    for root in roots:
        for path in ([root] if root.is_file() else listed(root)):
            if path.suffix.lower() in BY_EXTENSION and path.is_file():
                found[path.resolve()] = None
    return sorted(found)


def parse(path: Path, display: str) -> Parsed:
    """One file's tree and facts."""
    lang = BY_EXTENSION[path.suffix.lower()]
    tree = lang.parser().parse(path.read_bytes())
    parsed = Parsed(path, display, lang, tree)
    for query in lang.queries:
        for _, captures in QueryCursor(query).matches(tree.root_node):
            anchor = next((name for name in ANCHORS if name in captures), None)
            if anchor is None:
                continue
            parts = {name: nodes[0] for name, nodes in captures.items() if name != anchor and not name.startswith(PRIVATE)}
            parsed.facts.append((anchor, captures[anchor][0], parts))
    return parsed


def read(roots: list[Path], base: Path) -> list[Parsed]:
    """Every file under the roots, read; each shown by its path from `base`."""
    out = []
    for path in discover(roots):
        try:
            display = path.relative_to(base.resolve()).as_posix()
        except ValueError:
            display = path.as_posix()
        out.append(parse(path, display))
    return out
