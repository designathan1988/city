"""Reading: the files under the roots that a language of `languages` reads, each parsed by its grammar and matched
against its pattern files. What comes out is the same for every language: a list of facts, each the capture that
anchors it, its node and its parts by name. Which files belong to a project is what its version control says (the
files git tracks or does not ignore); where there is none, every file outside hidden folders.

A shader is read as the C it is written like: what C does not write (its qualifiers, a uniform's hints, the file's own
declarations) is read as blanks, every line and column kept, and the uniforms it declares are kept by name. A shader
written in a text of another language (one that holds the shader's entry point, or the file's declaration) is read
the same way, on the lines of the file it is written in.
"""
from __future__ import annotations

import os
import re
import subprocess
from dataclasses import dataclass, field
from pathlib import Path

from tree_sitter import QueryCursor

from .languages import BY_EXTENSION, Language
from .model import unquoted, child_nodes
from .vocab import ANCHORS, SHADER

LISTING = ("git", "ls-files", "--cached", "--others", "--exclude-standard")
LISTING_SECONDS = 60
HIDDEN, PRIVATE = ".", "_"
EMBEDDED = "#"                   # what joins a file's path and the line a shader written in its text starts at
ESCAPES = {"n": "\n", "t": "\t", "r": "\n", "\\": "\\", "\"": "\"", "'": "'", "`": "`"}


@dataclass
class Parsed:
    """One file read: where it is, its language, its tree and the facts its patterns found."""
    path: Path
    display: str
    lang: Language
    tree: object
    facts: list = field(default_factory=list)      # (anchor, node, {part: node})
    uniforms: tuple = ()                           # the names of the uniforms a shader declares


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
            if path.suffix.lower() in BY_EXTENSION and path.is_file() and readable(BY_EXTENSION[path.suffix.lower()]):
                found[path.resolve()] = None
    return sorted(found)


def readable(lang: Language) -> bool:
    """Whether a language's grammar and patterns are there (a grammar not installed: its files are not read)."""
    try:
        lang.queries
        return True
    except Exception:            # an import or a pattern the grammar does not have: the language is not read
        return False


# ---- shaders ------------------------------------------------------------------------------------------------------
def blanked(text: str, spans) -> str:
    """The text with those spans read as blanks (lines and columns kept)."""
    out = list(text)
    for start, end in spans:
        for i in range(start, end):
            if out[i] != "\n":
                out[i] = " "
    return "".join(out)


def shader(lang: Language, text: str) -> tuple:
    """A shader as the C it is written like, and the names of its uniforms."""
    uniforms = []
    if lang.uniform_word:
        declared = re.compile(rf"\b{lang.uniform_word}\b[^;{{}}]*?\b(\w+)\s*(?=[:=;\[])")
        uniforms = [m.group(1) for m in declared.finditer(text)]
        hints = re.compile(rf"\b{lang.uniform_word}\b[^;{{}}=:]*?\w\s*(:[^;=]*)")
        text = blanked(text, [(m.start(1), m.end(1)) for m in hints.finditer(text)])
    for word in lang.declarations:                     # the file's own declarations: `shader_type x;`
        text = blanked(text, [(m.start(), m.end()) for m in re.finditer(rf"\b{word}\b[^;]*;", text)])
    if lang.placed:                                    # `layout(...)`
        text = blanked(text, [(m.start(), m.end()) for m in re.finditer(rf"\b{lang.placed}\s*\([^)]*\)", text)])
    for word in lang.leaves:                           # `discard;`: an output, and the shader leaves there
        text = re.sub(rf"\b{word}\s*;", f"{word}();return;", text)
    for word in lang.blanked:
        text = blanked(text, [(m.start(), m.end()) for m in re.finditer(rf"\b{word}\b", text)])
    return text, tuple(uniforms)


def parse(path: Path, display: str, lang: Language | None = None, source: bytes | None = None) -> Parsed:
    """One file's tree and facts (a text of its own given: that text, read in that language)."""
    lang = lang or BY_EXTENSION[path.suffix.lower()]
    raw = path.read_bytes() if source is None else source
    uniforms = ()
    if lang.prepare == SHADER:
        prepared, uniforms = shader(lang, raw.decode("utf-8", errors="replace"))
        raw = prepared.encode("utf-8")
    tree = lang.parser().parse(raw)
    parsed = Parsed(path, display, lang, tree, uniforms=uniforms)
    for query in lang.queries:
        for _, captures in QueryCursor(query).matches(tree.root_node):
            anchor = next((name for name in ANCHORS if name in captures), None)
            if anchor is None:
                continue
            parts = {name: nodes[0] for name, nodes in captures.items() if name != anchor and not name.startswith(PRIVATE)}
            parsed.facts.append((anchor, captures[anchor][0], parts))
    return parsed


def shaders() -> list:
    """The shader languages, each with what marks a text as one (its entry point defined, its file's declaration)."""
    out = []
    for lang in dict.fromkeys(BY_EXTENSION.values()):
        if lang.prepare == SHADER and readable(lang):
            marks = [re.compile(rf"\b\w+\s+{entry}\s*\(") for entry in lang.entries]
            marks += [re.compile(rf"\b{word}\b") for word in lang.declarations]
            out.append((lang, marks))
    return out


def unescaped(text: str) -> str:
    return re.sub(r"\\(.)", lambda m: ESCAPES.get(m.group(1), m.group(1)), text)


def embedded(parsed: Parsed, kinds: list) -> list:
    """The shaders written in the texts of a file: each read in its language, on the lines it is written on."""
    out = []
    raw = parsed.path.read_bytes().decode("utf-8", errors="replace")
    if not any(mark.search(raw) for _, marks in kinds for mark in marks):
        return out
    stack = [parsed.tree.root_node]
    while stack:
        node = stack.pop()
        inside = unquoted(node) if node.is_named else None
        if inside is not None and any(unquoted(k) is not None for k in child_nodes(node)):
            inside = None                        # texts joined: each read apart
        if inside is not None and len(inside) > 16:
            text = unescaped(inside)
            for lang, marks in kinds:
                if any(mark.search(text) for mark in marks):
                    lines = node.start_point[0]
                    source = ("\n" * lines + " " * node.start_point[1] + " " + text).encode("utf-8")
                    out.append(parse(parsed.path, f"{parsed.display}{EMBEDDED}{lines + 1}", lang, source))
                    break
            continue
        stack.extend(child_nodes(node, False))
    return out


def read(roots: list[Path], base: Path, ceiling=None) -> list[Parsed]:
    """Every file under the roots, read; each shown by its path from `base`; the shaders written in their texts too.
    Past the ceiling, the files left are not read (said)."""
    out = []
    kinds = shaders()
    for path in discover(roots):
        if ceiling is not None and ceiling.over():
            ceiling.skip(f"the file {path.name}: not read")
            continue
        try:
            display = path.relative_to(base.resolve()).as_posix()
        except ValueError:
            display = path.as_posix()
        parsed = parse(path, display)
        out.append(parsed)
        if kinds and parsed.lang.prepare != SHADER:
            out += embedded(parsed, kinds)
    return out
