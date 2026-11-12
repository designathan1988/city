"""The languages the tracer reads: for each, its grammar (a tree-sitter grammar installed with Python), the pattern
files that say where each fact is in that grammar (`patterns/*.scm`), and the rules of the language no pattern can
say, each from the language's own reference. Adding a language is a row here and its pattern file.

A pattern that is one kind of node and nothing else (`(identifier) @ident`) says what every node of that kind is: it
is kept as a table by kind and never matched node by node.
"""
from __future__ import annotations

import importlib
import re
from dataclasses import dataclass
from functools import cached_property
from pathlib import Path

from tree_sitter import Language as Grammar
from tree_sitter import Parser, Query

from .vocab import BY_KIND, ROLES

PATTERNS = Path(__file__).resolve().parent / "patterns"
COMMENT = re.compile(r"^\s*;.*$", re.MULTILINE)
ALONE = re.compile(r"^(?:\(([\w.]+)\)|\[((?:\s*\([\w.]+\))+\s*)\])\s*@([\w.]+)$")    # `(kind) @role`, `[(kind) ...] @role`
KIND = re.compile(r"\(([\w.]+)\)")


@dataclass(frozen=True)
class Language:
    name: str
    package: str                       # the Python package holding the grammar
    entry: str                         # the function of it that gives the grammar
    patterns: tuple                    # its pattern files
    implicit_members: bool = False     # a plain name reads a member of the class around it (GDScript, C++)
    file_is_class: bool = False        # a file is a class: its functions are its methods (GDScript reference)
    implicit_base: bool = False        # a class that extends nothing extends one the language defines (GDScript)
    duck_typed: bool = False           # a member is looked up in the object as it runs, whatever type is declared
    #                                    for it (GDScript reference: duck typing); elsewhere it is the declared type's
    class_named_constructor: bool = False   # a method named as its class makes its objects (C++ [class.ctor])
    super_names_method: bool = False   # the base called alone runs the base's function of this one's name (GDScript)
    ambiguous_declarations: bool = False    # a declaration in a body that reads as a function's or an object's is an
    #                                         object's unless its parameters name types (C++ [dcl.ambig.res])
    beside: bool = False               # a source not written as relative is still looked for beside the file
    folder_file: tuple = ()            # the file a folder stands for in an import (TypeScript module resolution)
    project: str = ""                  # the file the language reads to resolve names (`configs.READERS`)
    operators: str = ""                # what an operator applied to an object of a class calls: the function of
    #                                    this name followed by the operator (C++ [over.oper])

    @cached_property
    def grammar(self) -> Grammar:
        return Grammar(getattr(importlib.import_module(self.package), self.entry)())

    @cached_property
    def compiled(self) -> tuple:
        """The pattern files as queries, and what a node's kind alone says it is: {kind: (role, no parts)}."""
        queries, by_type = [], {}
        for name in self.patterns:
            source = (PATTERNS / name).read_bytes()
            query = Query(self.grammar, source.decode("utf-8"))
            for i in range(query.pattern_count):
                written = source[query.start_byte_for_pattern(i):query.end_byte_for_pattern(i)].decode("utf-8")
                alone = ALONE.match(COMMENT.sub("", written).strip())
                if not alone or alone.group(3) not in BY_KIND:
                    continue
                query.disable_pattern(i)
                for kind in ([alone.group(1)] if alone.group(1) else KIND.findall(alone.group(2))):
                    known = by_type.get(kind)
                    if known is None or ROLES.index(alone.group(3)) < ROLES.index(known[0]):
                        by_type[kind] = (alone.group(3), {})
            queries.append(query)
        return tuple(queries), by_type

    @property
    def queries(self) -> tuple:
        return self.compiled[0]

    @property
    def by_type(self) -> dict:
        return self.compiled[1]

    def parser(self) -> Parser:
        return Parser(self.grammar)

    def __hash__(self) -> int:
        return hash(self.name)


GDSCRIPT = Language("gdscript", "tree_sitter_gdscript", "language", ("gdscript.scm",), implicit_members=True,
                    file_is_class=True, implicit_base=True, duck_typed=True, super_names_method=True, beside=True,
                    project="godot")
CPP = Language("cpp", "tree_sitter_cpp", "language", ("cpp.scm",), implicit_members=True,
               class_named_constructor=True, ambiguous_declarations=True, beside=True, project="database",
               operators="operator")
TYPESCRIPT = Language("typescript", "tree_sitter_typescript", "language_typescript",
                      ("typescript.scm", "typescript_only.scm"), folder_file=("index",), project="typescript")
TSX = Language("tsx", "tree_sitter_typescript", "language_tsx", ("typescript.scm", "jsx.scm"), folder_file=("index",),
               project="typescript")
BY_EXTENSION = {
    ".gd": GDSCRIPT,
    ".cpp": CPP, ".cc": CPP, ".cxx": CPP, ".h": CPP, ".hpp": CPP, ".hh": CPP, ".hxx": CPP, ".inl": CPP,
    ".ts": TYPESCRIPT, ".mts": TYPESCRIPT, ".cts": TYPESCRIPT,
    ".tsx": TSX, ".js": TSX, ".jsx": TSX, ".mjs": TSX, ".cjs": TSX,
}
