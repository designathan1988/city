"""What the tree states is broken, read without running the game: every link the code and the data declare, followed to its end.

The game is a chain of declared links, each with one owner:
  a control or a tool (scripts, data/ui) -> a command or a question by name (game/store/store_api.gd COMMANDS, QUERIES)
  -> the core's bound method (native/src/api, D_METHOD) ; a text shown -> its key in data/i18n, in every language ;
  a tunable value (an X-macro field list, a key of data/**.json) -> the code that reads it.
A link whose other end is missing is a defect of its owner, found here by name, the way the code itself carries it:
  - a command or question whose core method is not bound, or sent by a script and not declared (fails when used);
  - a command nothing outside the tests sends (a feature the player cannot reach), a bound method nothing names (dead API);
  - a text key shown and not in the tables, a text with an empty translation, a text nothing shows;
  - a tunable field the code never reads (an option that changes nothing), a data key no code names.
Names are matched as whole identifiers over the game's sources; a name built at run time counts through its literal prefix
(texts) or through the data that carries it (commands named in data/**). Nothing is exempted by a list kept here.
"""
from __future__ import annotations

import csv
import json
import re
from pathlib import Path

from .findings import Finding

STORE_API = "project/game/store/store_api.gd"
GAME_SCRIPTS = ("project/game", "project/ui", "project/render")
TEST_SCRIPTS = ("project/tests",)
CORE = "native/src"
DATA = "project/data"
I18N = "project/data/i18n"
TABLE = re.compile(r"^const (COMMANDS|QUERIES) := \{")
ENTRY = re.compile(r'&"(\w+)":\s*"([\w/]+)"')
BOUND = re.compile(r'D_METHOD\("(\w+)"')
SENT = re.compile(r'\b(dispatch|query|ask)\(\s*&"(\w+)"')
SHOWN = re.compile(r'\btr\(\s*&?"([^"%]+)"\s*[,)]')
LITERAL = re.compile(r'"((?:[^"\\]|\\.)*)"')
WORD = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")
FIELDS = re.compile(r"#define\s+(CITY_\w+_FIELDS)\(X\)((?:.*\\\n)*.*)")
FIELD = re.compile(r"\bX\((\w+)\)")
KEY_FIELD = re.compile(r"_key$")
PREFIX_LEAST = 3  # a text prefix shorter than this names nothing


def _files(root: Path, folders: tuple[str, ...] | str, suffixes: tuple[str, ...]) -> list[Path]:
    out: list[Path] = []
    for folder in (folders,) if isinstance(folders, str) else folders:
        base = root / folder
        if base.is_dir():
            out.extend(p for p in sorted(base.rglob("*")) if p.suffix in suffixes and p.is_file())
    return out


def _text(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="replace")


def _rel(root: Path, path: Path) -> str:
    return path.relative_to(root).as_posix()


class Tree:
    """The sources read once: each file's text by its path, and what the checks ask of them."""

    def __init__(self, root: Path) -> None:
        self.root = root
        self.game = {p: _text(p) for p in _files(root, GAME_SCRIPTS, (".gd",))}
        self.tests = {p: _text(p) for p in _files(root, TEST_SCRIPTS, (".gd",))}
        self.core = {p: _text(p) for p in _files(root, CORE, (".cpp", ".h"))}
        self.shaders = {p: _text(p) for p in _files(root, "project/render", (".gdshader", ".gdshaderinc"))}
        self.data = {p: _text(p) for p in _files(root, DATA, (".json",))}
        self.read = len(self.game) + len(self.tests) + len(self.core) + len(self.shaders) + len(self.data)

    def words(self, *groups: dict[Path, str]) -> set[str]:
        found: set[str] = set()
        for group in groups:
            for text in group.values():
                found.update(WORD.findall(text))
        return found

    def literals(self, joint: str) -> tuple[set[str], set[str]]:
        """Every quoted string of the game's scripts, core and data, and the prefixes a name is built from at run time: what
        stands before a `%` of a format, and a string that ends in `joint` (the separator the names of that kind are joined by)."""
        found: set[str] = set()
        for group in (self.game, self.core, self.data):
            for text in group.values():
                found.update(LITERAL.findall(text))
        prefixes = {s.split("%")[0] for s in found if "%" in s} | {s for s in found if s.endswith(joint)}
        return found, {p for p in prefixes if len(p) >= PREFIX_LEAST and p.endswith(joint)}

    def place(self, path: Path, text: str, offset: int) -> str:
        return "%s:%d" % (_rel(self.root, path), text.count("\n", 0, offset) + 1)


def _declared(tree: Tree) -> dict[str, dict[str, tuple[str, int]]]:
    """COMMANDS and QUERIES of StoreApi: name -> (core method, line)."""
    tables: dict[str, dict[str, tuple[str, int]]] = {"COMMANDS": {}, "QUERIES": {}}
    current = None
    for number, line in enumerate(_text(tree.root / STORE_API).splitlines(), 1):
        opened = TABLE.match(line)
        if opened:
            current = opened[1]
        elif line.startswith("}"):
            current = None
        if current:
            for name, method in ENTRY.findall(line):
                tables[current][name] = (method, number)
    return tables


def commands(tree: Tree) -> list[Finding]:
    out: list[Finding] = []
    tables = _declared(tree)
    if not tables["COMMANDS"] or not tables["QUERIES"]:
        return [Finding("links", "audit cannot read", STORE_API, "COMMANDS or QUERIES not found: the command chain was not checked")]
    bound: dict[str, str] = {}
    for path, text in tree.core.items():
        for m in BOUND.finditer(text):
            bound.setdefault(m[1], tree.place(path, text, m.start()))
    sent_game: dict[str, str] = {}
    sent_tests: set[str] = set()
    declared = {**tables["COMMANDS"], **tables["QUERIES"]}
    for group, by_tests in ((tree.game, False), (tree.tests, True)):
        for path, text in group.items():
            for m in SENT.finditer(text):
                name = m[2]
                if by_tests:
                    sent_tests.add(name)
                else:
                    sent_game.setdefault(name, tree.place(path, text, m.start()))
                if name not in declared and m[1] != "ask":
                    out.append(Finding("links", "sent and not declared", tree.place(path, text, m.start()),
                                       "%s(&\"%s\") names nothing StoreApi declares: it fails when it runs" % (m[1], name)))
    in_data = tree.words(tree.data)
    in_scripts = tree.words({p: t for p, t in tree.game.items() if _rel(tree.root, p) != STORE_API})  # the table itself names them all
    for kind, table in tables.items():
        for name, (method, line) in table.items():
            owner = "%s:%d" % (STORE_API, line)
            if method.rsplit("/", 1)[-1] not in bound:
                out.append(Finding("links", "no core method", owner, "%s '%s' names the core method '%s', which the core does not bind" % (kind[:-1].lower().replace("ie", "y"), name, method)))
            reached = name in sent_game or name in in_data or name in in_scripts
            if kind == "COMMANDS" and not reached:
                out.append(Finding("features", "command nothing sends" if name not in sent_tests else "command only tests send", owner,
                                   "the command '%s' is sent by no script, tool or data of the game: the player cannot reach what it does" % name))
            if kind == "QUERIES" and not reached and name not in sent_tests:
                out.append(Finding("features", "question nobody asks", owner, "the question '%s' is asked by nothing, not even a test" % name))
    named = {method.rsplit("/", 1)[-1] for method, _ in declared.values()}
    called = tree.words(tree.game, tree.tests)
    for name, where in bound.items():
        if name not in named and name not in called:
            out.append(Finding("features", "bound and never called", where, "the core binds '%s' and nothing names it: no command, no question, no script" % name))
    return out


def texts(tree: Tree) -> list[Finding]:
    out: list[Finding] = []
    keys: dict[str, str] = {}
    for path in _files(tree.root, I18N, (".csv",)):
        with path.open(encoding="utf-8", newline="") as handle:
            rows = list(csv.reader(handle))
        if not rows:
            continue
        languages = rows[0][1:]
        for number, row in enumerate(rows[1:], 2):
            if not row or not row[0].strip():
                continue
            where = "%s:%d" % (_rel(tree.root, path), number)
            if row[0] in keys:
                out.append(Finding("texts", "key twice", where, "the text key '%s' is also at %s" % (row[0], keys[row[0]])))
            keys[row[0]] = where
            for language, cell in zip(languages, row[1:] + [""] * len(languages)):
                if not cell.strip():
                    out.append(Finding("texts", "no translation", where, "the text '%s' is empty in %s" % (row[0], language)))
    literals, prefixes = tree.literals(".")
    for path, text in tree.game.items():
        for m in SHOWN.finditer(text):
            if m[1] not in keys:
                out.append(Finding("texts", "key not in the tables", tree.place(path, text, m.start()), "tr(\"%s\") shows a key no table of data/i18n holds: the player reads the key itself" % m[1]))
    for path, text in tree.data.items():
        if I18N in _rel(tree.root, path):
            continue
        for key, value in _walk(json.loads(text), ""):
            if KEY_FIELD.search(key.rsplit(".", 1)[-1]) and isinstance(value, str) and value and value not in keys:
                out.append(Finding("texts", "key not in the tables", "%s %s" % (_rel(tree.root, path), key), "the data names the text key '%s', which no table of data/i18n holds" % value))
    for key, where in keys.items():
        if key not in literals and not any(key.startswith(p) for p in prefixes):
            out.append(Finding("texts", "text nothing shows", where.rsplit(":", 1)[0], "the text '%s' is shown by no script and named by no data" % key, 1, [where]))
    return out


def _walk(node, path: str):
    """Every (dotted key path, value) of a JSON document, lists by index."""
    if isinstance(node, dict):
        for key, value in node.items():
            here = "%s.%s" % (path, key) if path else key
            yield here, value
            yield from _walk(value, here)
    elif isinstance(node, list):
        for index, value in enumerate(node):
            yield from _walk(value, "%s[%d]" % (path, index))


def tunables(tree: Tree) -> list[Finding]:
    """An X-macro list of fields declares values the data must give; a field nothing reads is an option that changes nothing."""
    out: list[Finding] = []
    scripted = tree.words(tree.game, tree.shaders)  # a list handed to the scripts by name (a dictionary made from it) is read there
    for path, text in tree.core.items():
        for m in FIELDS.finditer(text):
            for field in FIELD.finditer(m[2]):
                name = field[1]
                reader = re.compile(r"(?:\.|->)%s\b" % re.escape(name))
                if name not in scripted and not any(reader.search(other) for other in tree.core.values()):
                    out.append(Finding("data", "field nothing reads", tree.place(path, text, m.start(2) + field.start()),
                                       "%s declares '%s' and no code of the core reads it: the value in the data changes nothing" % (m[1], name)))
    return out


def data_keys(tree: Tree) -> list[Finding]:
    """A key of data/**.json no source names. A table of rows keyed by id (every value a dictionary, or every value a list) is read
    by walking it, so its ids are not names the code holds; `_doc` is the file's own note."""
    out: list[Finding] = []
    named = tree.words(tree.game, tree.core, tree.shaders)
    _, prefixes = tree.literals("_")
    for path, text in tree.data.items():
        rel = _rel(tree.root, path)
        if I18N in rel:
            continue

        def visit(node, at: str) -> None:
            if isinstance(node, list):
                for index, value in enumerate(node):
                    visit(value, "%s[%d]" % (at, index))
                return
            if not isinstance(node, dict):
                return
            values = [v for k, v in node.items() if not k.startswith("_")]
            rows = len(values) > 1 and (all(isinstance(v, dict) for v in values) or all(isinstance(v, list) for v in values))
            for key, value in node.items():
                if key.startswith("_"):
                    continue
                here = "%s.%s" % (at, key) if at else key
                if not rows and WORD.fullmatch(key) and key not in named and not any(key.startswith(p) for p in prefixes):
                    out.append(Finding("data", "key nothing reads", rel, "the key '%s' is named by no source of the game: its value changes nothing" % key, 1, [here]))
                visit(value, here)

        visit(json.loads(text), "")
    return out


def read_tree(root: Path) -> tuple[list[Finding], int]:
    tree = Tree(root)
    return commands(tree) + texts(tree) + tunables(tree) + data_keys(tree), tree.read
