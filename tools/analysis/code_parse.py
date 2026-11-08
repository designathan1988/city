# The code the analysers read: the files come from tools/tasks/inputs (git's tracked and untracked files: no copies,
# fixtures nor ignored outputs), and each C++ file is parsed into its functions once per content (tools/tasks memo), so
# every detector reads the same parse. Braces and heads are read on the code the gates' lexer leaves (tools/gates/lex_c.py:
# no comment, string or character), so a brace in a string or a comment shifts no function.
import os
import re
import sys
from dataclasses import dataclass
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
sys.path.insert(0, str(Path(__file__).resolve().parent.parent / "gates"))
import lex_c  # noqa: E402  the tools' one C++ lexer
from tasks import inputs, memo  # noqa: E402

PARSER = "tools/analysis/code_parse.py"  # its own source keys every parse: a change of the parser parses again
LEXER = "tools/gates/lex_c.py"  # and the lexer's
CPP_EXT = (".cpp", ".cc", ".cxx", ".h", ".hpp")
FUNC_HEAD = re.compile(r"^\s*[A-Za-z_][\w:<>,\s\*&~]*?\b([A-Za-z_]\w*(?:::~?\w+)*)\s*\(([^;{}]*)\)\s*(const)?[^;{}=]*\{\s*$")
CLASS_HEAD = re.compile(r"\b(?:class|struct)\s+(?:\w+\s+)*?([A-Za-z_]\w*)\s*(?:final\s*)?(?::[^;{]*)?\{")
SIGNATURE_LINES = 4  # the longest signature that wraps over lines
KEYWORDS = frozenset({"if", "for", "while", "switch", "catch", "return", "sizeof", "alignof", "decltype", "static_cast",
                      "dynamic_cast", "reinterpret_cast", "const_cast", "new", "delete", "throw"})


@dataclass
class Func:
    name: str  # qualified when the class is known (written A::f, or defined inside class A)
    line: int
    body: str
    const: bool

    @property
    def short(self) -> str:
        return self.name.split("::")[-1]

    @property
    def owner(self) -> str:
        return self.name.rsplit("::", 1)[0] if "::" in self.name else ""


@dataclass
class Source:
    path: str  # repo-relative
    text: str
    funcs: list


def code_only(text):
    """The text with every comment, string and character literal blanked, newlines kept: what braces and heads are read on."""
    chars = list(text)
    for t in lex_c.lex(text, lex_c.LANG_CPP):
        if t.kind in ("comment", "string", "char"):
            for k in range(t.start, t.end):
                if chars[k] != "\n":
                    chars[k] = " "
    return "".join(chars)


def functions(text):
    """(name, first line, body, const) of each function defined with its brace on the signature's last line; a function
    defined inside a class body gets that class's name in front. The body is the source as written."""
    source = text.splitlines()
    lines = code_only(text).splitlines()
    spans = class_spans(lines)
    i = 0
    while i < len(lines):
        head, k = lines[i], i  # a signature may run over several lines: joined until its parentheses close
        while head.count("(") > head.count(")") and k + 1 < len(lines) and k - i < SIGNATURE_LINES:
            k += 1
            head += " " + lines[k].strip()
        m = FUNC_HEAD.match(head)
        if not m or m.group(1) in KEYWORDS:
            i += 1
            continue
        depth, j = 0, i
        while j < len(lines):
            depth += lines[j].count("{") - lines[j].count("}")
            if depth <= 0 and j > i or (depth == 0 and j == i and "}" in lines[j]):
                break
            j += 1
        name = m.group(1)
        inside = [c for a, b, c in spans if a < i <= b]
        if inside and "::" not in name:
            name = f"{inside[-1]}::{name}"
        yield name, i + 1, "\n".join(source[i:j + 1]), bool(m.group(3))
        i = j + 1


def class_spans(lines):
    """(first line, last line, class name) of each class or struct body, by brace depth (innermost last)."""
    spans, stack, depth = [], [], 0
    for n, line in enumerate(lines):
        m = CLASS_HEAD.search(line)
        if m:
            stack.append((n, depth, m.group(1)))
        depth += line.count("{") - line.count("}")
        while stack and depth <= stack[-1][1] and n > stack[-1][0]:
            start, _, name = stack.pop()
            spans.append((start, n, name))
    return sorted(spans)


def read(path):
    with open(path, encoding="utf-8", errors="replace") as f:
        return f.read()


def load(root, patterns):
    """{repo-relative path: Source} of the files the patterns name, each C++ file parsed once per content."""
    out = {}
    found = inputs.files(patterns, root=root)
    inputs.content_ids(found + [PARSER, LEXER], root=root)  # one pass for every changed file, before the keys ask one by one
    for rel in found:
        text = read(os.path.join(root, rel))
        key = inputs.key_of([rel, PARSER, LEXER], root=root)
        parsed = memo("causes-parse", key, lambda t=text: list(functions(t)), root=root) if rel.endswith(CPP_EXT) else []
        out[rel] = Source(rel, text, [Func(n, ln, body, const) for n, ln, body, const in parsed])
    return out
