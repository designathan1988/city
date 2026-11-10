"""One source file as the gates see it: its text, its language, its tokens from that language's lexer and the views
the rules read (comment tokens, code tokens, the text with comments blanked). Parsed once per run."""
import os

import lex_c
import lex_gd
import lex_other
from tokens import CODE_KINDS

LANGUAGES = {
    ".cpp": "cpp", ".h": "cpp", ".hpp": "cpp", ".c": "cpp", ".cc": "cpp", ".inl": "cpp",
    ".gdshader": "glsl", ".gdshaderinc": "glsl", ".glsl": "glsl",
    ".js": "js", ".mjs": "js", ".cjs": "js", ".ts": "js", ".mts": "js", ".cts": "js", ".tsx": "js", ".jsx": "js",
    ".gd": "gd", ".py": "py", ".sh": "sh", ".ps1": "ps1",
    ".tscn": "godot_res", ".tres": "godot_res", ".import": "godot_res", ".cfg": "godot_res", ".godot": "godot_res",
    ".svg": "xml", ".xml": "xml",
    ".md": "prose", ".txt": "prose",
    ".json": "data", ".csv": "data", ".uid": "data", ".gltf": "data",
}
_LEXERS = {
    "cpp": lambda t: lex_c.lex(t, lex_c.LANG_CPP),
    "glsl": lambda t: lex_c.lex(t, lex_c.LANG_GLSL),
    "js": lambda t: lex_c.lex(t, lex_c.LANG_JS),
    "gd": lex_gd.lex,
    "py": lex_other.lex_python,
    "sh": lex_other.lex_shell,
    "ps1": lambda t: lex_other.lex_shell(t, powershell=True),
    "godot_res": lex_other.lex_godot_resource,
    "xml": lex_other.lex_xml,
    "prose": lex_other.lex_prose,
    "data": lex_other.lex_data,
}
_BINARY_PROBE = 8192


def language_of(path):
    name = os.path.basename(path)
    if name == ".gitignore":
        return "sh"
    return LANGUAGES.get(os.path.splitext(path)[1].lower())


class Source:
    """Lazily lexed view of `root/path`. `binary` files have no text; `lang` None means no lexer knows the format."""

    def __init__(self, root, path):
        self.path = path
        with open(os.path.join(root, path), "rb") as fh:
            raw = fh.read()
        self.binary = b"\0" in raw[:_BINARY_PROBE]
        self.text = ""
        if not self.binary:
            try:
                self.text = raw.decode("utf-8-sig")
            except UnicodeDecodeError:
                self.binary = True
        self.lang = language_of(path)
        self._tokens = None
        self._error = None

    @property
    def lines(self):
        return self.text.splitlines()

    @property
    def tokens(self):
        """All tokens; raises lex_other.LexError when the format is unknown or the text cannot be tokenized."""
        if self._tokens is None:
            if self.binary:
                self._tokens = []
            elif self.lang is None:
                raise lex_other.LexError(f"no lexer for '{os.path.splitext(self.path)[1] or self.path}'")
            else:
                self._tokens = _LEXERS[self.lang](self.text)
        return self._tokens

    def comments(self):
        return [t for t in self.tokens if t.kind == "comment"]

    def code(self):
        return [t for t in self.tokens if t.kind in CODE_KINDS]

    def blanked(self, keep_strings=True):
        """Lines of the text with comment tokens (and, unless keep_strings, string bodies) replaced by spaces; line
        numbers are kept, so a pattern match on line n is a match in code on line n."""
        chars = list(self.text)
        for t in self.tokens:
            if t.kind == "comment" or (not keep_strings and t.kind in ("string", "char")):
                for k in range(t.start, t.end):
                    if chars[k] != "\n":
                        chars[k] = " "
        return "".join(chars).splitlines()


class Sources:
    """Each file read and lexed at most once per run."""

    def __init__(self, root):
        self.root = root
        self._cache = {}

    def get(self, path):
        s = self._cache.get(path)
        if s is None:
            s = self._cache[path] = Source(self.root, path)
        return s
