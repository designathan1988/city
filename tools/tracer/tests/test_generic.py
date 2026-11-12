"""The tracer is generic: its code names no program, library, framework or platform, and leans on no other project.

- Every text its Python writes that reads as a name (letters, digits, `_`, `.`) is one of its own words: a constant
  of `vocab.py`, or one of the few words of its command and report listed here, each with what it is.
- The only modules it imports are Python's own, the grammar library and its own.
- The words its pattern files compare a node's text with are words of the language the file is for, listed here
  with where the language defines each. (A word written as a token of a pattern, `"as"`, is checked by the grammar
  itself: a pattern naming a token the grammar does not have does not compile.)
"""
from __future__ import annotations

import ast
import re
import sys
import unittest
from pathlib import Path

from support import PATTERNS, WORDS

HOME = Path(__file__).resolve().parent.parent
CODE = sorted((HOME / "tracer").glob("*.py")) + [HOME / "trace.py"]
NAME = re.compile(r"^[A-Za-z_][A-Za-z0-9_.]*$")

# The tracer's words that are no part of its vocabulary: what each is.
OWN = {
    "utf-8", "replace",                                  # how a file's bytes are read as text
    "git", "ls-files",                                   # the command asked which files a project holds (read.py)
    "getter", "setter",                                  # the two flags of a definition set by their anchors
    "start", "forward", "backward", "depth", "label", "at", "again", "kind", "how", "written", "outside",   # the
    #                                                      fields of a trace given as data (report.py, trace.py)
    "store_true", "append",                              # argparse's own actions
    "__main__",                                          # Python's name for the script being run
}
# Where each language's file names a language, a grammar package or a pattern file (languages.py), and the keys of
# the files each language itself reads to resolve names, each from the language's own reference (configs.py:
# tsconfig/jsconfig, project.godot, compile_commands.json). Neither names a program, a library or a platform.
REGISTRY = {"languages.py", "configs.py"}

# The words a pattern file compares a node's text with, each a word of its language.
LANGUAGE_WORDS = {
    "gdscript.scm": {"self", "super", "preload", "new"},          # GDScript reference: keywords, `preload`, `.new()`
    "typescript.scm": {"constructor", "any", "unknown", "object"},  # ECMA-262: ClassElement `constructor`;
    #                                                                TypeScript handbook, Everyday Types: the open types
    "typescript_only.scm": set(),
    "jsx.scm": set(),
    "cpp.scm": {"static_cast", "dynamic_cast", "const_cast", "reinterpret_cast"},   # ISO C++ [expr.post]
}
PREDICATE = re.compile(r"\(#[\w-]+\?[^)]*\)")
QUOTED = re.compile(r'"([^"]*)"')


def texts(tree: ast.AST):
    """Every text a module writes, its docstrings left out."""
    docs = set()
    for node in ast.walk(tree):
        if isinstance(node, (ast.Module, ast.ClassDef, ast.FunctionDef)) and node.body:
            first = node.body[0]
            if isinstance(first, ast.Expr) and isinstance(first.value, ast.Constant) and isinstance(first.value.value, str):
                docs.add(id(first.value))
    for node in ast.walk(tree):
        if isinstance(node, ast.Constant) and isinstance(node.value, str) and id(node) not in docs:
            yield node.value, node.lineno


class Generic(unittest.TestCase):
    def test_every_name_written_is_the_tracer_s_own(self):
        strangers = []
        for path in CODE:
            if path.name in REGISTRY or path.name == "vocab.py":
                continue
            for value, line in texts(ast.parse(path.read_text(encoding="utf-8"))):
                if NAME.match(value) and len(value) > 1 and value not in WORDS and value not in OWN:
                    strangers.append(f"{path.name}:{line} {value!r}")
        self.assertEqual([], strangers)

    def test_it_imports_python_the_grammar_library_and_itself(self):
        allowed = set(sys.stdlib_module_names) | {"tree_sitter", "tracer"}
        strangers = []
        for path in CODE:
            for node in ast.walk(ast.parse(path.read_text(encoding="utf-8"))):
                names = [alias.name for alias in node.names] if isinstance(node, ast.Import) else \
                    [node.module] if isinstance(node, ast.ImportFrom) and node.level == 0 and node.module else []
                strangers += [f"{path.name}: {name}" for name in names if name.split(".")[0] not in allowed]
        self.assertEqual([], strangers)

    def test_a_pattern_compares_only_with_words_of_its_language(self):
        files = sorted(path.name for path in PATTERNS.glob("*.scm"))
        self.assertEqual(sorted(LANGUAGE_WORDS), files)
        for name in files:
            compared = {word for found in PREDICATE.findall((PATTERNS / name).read_text(encoding="utf-8"))
                        for word in QUOTED.findall(found)}
            self.assertEqual(set(), compared - LANGUAGE_WORDS[name], name)


if __name__ == "__main__":
    unittest.main()
