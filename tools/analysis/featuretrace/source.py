"""Source files: which ones are read, in which language, parsed by which grammar, and the facts kept per file content.

The language of a file comes from its extension. Code is parsed by the tree-sitter grammar of its language (the
language's own rules, as a compiler reads them). Lines are counted from byte offsets: py-tree-sitter 0.26 corrupts
memory when `Point.row` is read (issue #505), so no Point is ever touched. Facts are cached by the file's content and
the reader's own source, so a file is read again only when it or the reader changed.
"""
from __future__ import annotations

import bisect
import hashlib
import os
import pickle
from pathlib import Path

import tree_sitter
import tree_sitter_cpp
import tree_sitter_gdscript
import tree_sitter_typescript

LANG_BY_EXT = {
    ".gd": "gd", ".cpp": "cpp", ".cc": "cpp", ".cxx": "cpp", ".h": "cpp", ".hpp": "cpp", ".hh": "cpp",
    ".inl": "cpp", ".inc": "cpp", ".tscn": "scene", ".json": "data",
    ".ts": "js", ".mts": "js", ".cts": "js", ".tsx": "js", ".js": "js", ".jsx": "js", ".mjs": "js", ".cjs": "js",
}
# JavaScript and TypeScript (J12): TypeScript's grammar for .ts/.mts/.cts; the TSX grammar (TypeScript's, with JSX) for
# .tsx and every JavaScript file. A `.d.ts` file only declares.
TYPESCRIPT_EXTS = (".ts", ".mts", ".cts")
CONFIG_FILES = ("tsconfig.json", "jsconfig.json")
PACKAGE_FILE = "package.json"
LIBRARY_DIRS = {"node_modules"}       # read only when a root points inside them
PROJECT_FILE = "project.godot"
SKIP_DIRS = {".git", ".godot", "__pycache__", ".import"} | LIBRARY_DIRS

_GRAMMARS = {
    "gd": tree_sitter.Language(tree_sitter_gdscript.language()),
    "cpp": tree_sitter.Language(tree_sitter_cpp.language()),
    "ts": tree_sitter.Language(tree_sitter_typescript.language_typescript()),
    "tsx": tree_sitter.Language(tree_sitter_typescript.language_tsx()),
}
_COMMENTS = {}                        # grammar -> the query of its comment nodes


def comments(root, grammar: str) -> list:
    """The comment nodes of a parsed tree, in the order they are written (a comment is a node of its own, never part
    of a string, a template or a regular expression)."""
    if grammar not in _COMMENTS:
        _COMMENTS[grammar] = tree_sitter.Query(_GRAMMARS[grammar], "(comment) @comment")
    found = tree_sitter.QueryCursor(_COMMENTS[grammar]).captures(root).get("comment", [])
    return sorted(found, key=lambda node: node.start_byte)


def language_of(path: Path) -> str:
    if path.name == PROJECT_FILE:
        return "project"
    if path.name in CONFIG_FILES or path.name.startswith("tsconfig.") and path.suffix == ".json":
        return "tsconfig"
    if path.name == PACKAGE_FILE:
        return "package"
    return LANG_BY_EXT.get(path.suffix.lower(), "")


def grammar_of(path: Path) -> str:
    """The grammar a JavaScript or TypeScript file is parsed with (J12)."""
    return "ts" if path.suffix.lower() in TYPESCRIPT_EXTS else "tsx"


def discover(roots: list[Path]) -> list[tuple[Path, str]]:
    """Every file under the roots that some reader understands, in a stable order."""
    found = []
    for root in roots:
        if root.is_file():
            lang = language_of(root)
            if lang:
                found.append((root.resolve(), lang))
            continue
        for dirpath, dirnames, filenames in os.walk(root):
            dirnames[:] = sorted(d for d in dirnames if d not in SKIP_DIRS)
            for name in sorted(filenames):
                path = Path(dirpath) / name
                lang = language_of(path)
                if lang:
                    found.append((path.resolve(), lang))
    return found


class Source:
    """One file's bytes, with the helpers every reader uses."""

    def __init__(self, path: Path, display: str, data: bytes):
        self.path = path
        self.display = display
        self.data = data
        self._newlines = [i for i, b in enumerate(data) if b == 0x0A]

    def line(self, node_or_byte) -> int:
        offset = node_or_byte if isinstance(node_or_byte, int) else node_or_byte.start_byte
        return bisect.bisect_right(self._newlines, offset - 1) + 1

    def end_line(self, node) -> int:
        return self.line(max(node.start_byte, node.end_byte - 1))

    def text(self, node) -> str:
        return self.data[node.start_byte:node.end_byte].decode("utf-8", "replace")

    def parse(self, lang: str):
        parser = tree_sitter.Parser(_GRAMMARS[lang])
        return parser.parse(self.data).root_node


def field_of(node, name: str):
    return node.child_by_field_name(name)


def named_children(node) -> list:
    return [c for c in node.children if c.is_named]


def string_value(src: Source, node) -> str | None:
    """The text inside a string literal node (GDScript string or &"name", C++ string_literal), else None."""
    if node is None or node.type not in ("string", "string_name", "string_literal", "raw_string_literal",
                                         "template_string"):
        return None
    text = src.text(node)
    quote = next((q for q in ('"', "'", "`") if text.rstrip().endswith(q)), '"')   # JS: ' and ` quote too
    if node.type == "template_string" and any(c.is_named and c.type != "string_fragment" for c in node.children):
        return None                                                            # a template with substitutions
    start = text.find(quote)
    end = text.rfind(quote)
    if start < 0 or end <= start:
        return None
    return text[start + 1:end]


def is_identifier(text: str | None) -> bool:
    return bool(text) and (text[0].isalpha() or text[0] == "_") and all(c.isalnum() or c == "_" for c in text)


def reader_version() -> str:
    """A digest of the readers' own source: changing a reader invalidates every cached fact."""
    digest = hashlib.sha1()
    for path in sorted(Path(__file__).parent.glob("*.py")):
        digest.update(path.read_bytes())
    return digest.hexdigest()


def cached_facts(path: Path, display: str, lang: str, read, cache_dir: Path | None, version: str, context: str):
    """The file's facts from the cache when its content, the readers and the context are unchanged; else read anew."""
    data = path.read_bytes()
    key = hashlib.sha1(data + version.encode() + context.encode() + display.encode()).hexdigest()
    cache_file = cache_dir / f"{key}.pkl" if cache_dir else None
    if cache_file and cache_file.exists():
        with cache_file.open("rb") as handle:
            return pickle.load(handle)
    facts = read(Source(path, display, data), lang)
    if cache_file:
        cache_file.parent.mkdir(parents=True, exist_ok=True)
        tmp = cache_file.with_suffix(".tmp")
        with tmp.open("wb") as handle:
            pickle.dump(facts, handle, protocol=pickle.HIGHEST_PROTOCOL)
        os.replace(tmp, cache_file)
    return facts
