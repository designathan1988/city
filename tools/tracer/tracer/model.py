"""What the program is made of once read: a definition, a file, and the text a node writes."""
from __future__ import annotations

import re
from collections import defaultdict
from dataclasses import dataclass, field

QUOTES = "\"'`"                                        # the marks a text is written between
PREFIX = 3                                              # how far into a text its opening mark may be (`u8"..."`)
WORD = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")
BRACKETS = {"<": ">"}                                   # and the pairs a source's path may be written between
LAMBDA = "<lambda:{}>"
OBJECT = "<object:{}>"


@dataclass(slots=True)
class Definition:
    id: int
    kind: str
    name: str
    file: int
    line: int
    node: object
    owner: int = -1                  # the definition it is inside (a module's: none)
    scopes: tuple = ()               # the scopes its name is written under (`A::B::f`: A, B)
    constructor: bool = False
    getter: bool = False
    setter: bool = False
    body: bool = False               # written with a body (a declaration alone has none)
    adopted: bool = False            # written out of its scope and given to it
    named: bool = False              # a file its own words name (known by that name everywhere)
    remade: bool = False             # read first as a function declared, then as an object made with arguments
    holder: int = -1                 # the class it is also a member of (a parameter property)
    position: int = 0                # a parameter's place among its function's
    parts: dict = field(default_factory=dict)


@dataclass
class File:
    index: int
    display: str
    lang: object
    tree: object
    path: object = None              # where it is (the language's own files that apply to it are found from here)
    module: int = -1
    span: range = range(0)           # its definitions
    roles: dict = field(default_factory=dict)        # node id -> (role, parts), where a pattern with parts says it
    named: dict = field(default_factory=dict)        # node id -> the node holding its name
    pairs: dict = field(default_factory=dict)        # node id -> the parts of the named entry it is
    passes: dict = field(default_factory=dict)       # node id -> the parts of it whose values it holds
    facts: dict = field(default_factory=lambda: defaultdict(list))   # anchor -> [(node, parts)]
    imports: list = field(default_factory=list)      # (local name, source, name in the source | WHOLE | DEFAULT)
    includes: list = field(default_factory=list)     # the sources whose every name is a name of this file
    exports: dict = field(default_factory=dict)      # exported name -> name in the file
    default: list = field(default_factory=list)      # the nodes exported by default
    open: set = field(default_factory=set)           # the types written there that may be any value
    effects: set = field(default_factory=set)        # the calls whose value nothing takes
    uniforms: tuple = ()                             # the uniforms a shader declares (the code outside sets them)

    def role(self, node):
        """What a node is as an expression, (role, parts): by a pattern that matched it, else by its kind alone."""
        return self.roles.get(node.id) or self.lang.by_type.get(node.type)

    def is_a(self, node, role: str) -> bool:
        found = self.role(node)
        return found is not None and found[0] == role


def child_nodes(node, named: bool = True) -> list:
    """A node's children (the named ones, by default), read with a cursor: no list of them is kept on the node
    (py-tree-sitter keeps a node's `children` on it for as long as the node lives; the tracer holds hundreds of
    thousands of nodes, and kept so their trees would be held whole as objects)."""
    out = []
    cursor = node.walk()
    if cursor.goto_first_child():
        while True:
            child = cursor.node
            if not named or child.is_named:
                out.append(child)
            if not cursor.goto_next_sibling():
                break
    return out


def text(node) -> str:
    return node.text.decode(errors="replace")


def unquoted(node) -> str | None:
    """The text a node writes between marks (a string), None for any other node."""
    raw = text(node)
    return raw[1:-1] if len(raw) >= 2 and raw[0] in QUOTES and raw[-1] == raw[0] else None


def named(node) -> str | None:
    """The name a text names: the text itself where it is one name, the name it ends with where it is names joined
    by marks (`a/b`, the way a name is written under its scopes); of any other node, none."""
    raw = text(node)
    at = next((i for i, mark in enumerate(raw[:PREFIX + 1]) if mark in QUOTES), -1)
    if at < 0 or len(raw) < at + 2 or raw[-1] != raw[at] or any(mark.isspace() for mark in raw):
        return None
    last = WORD.search(raw[at + 1:-1][::-1])
    return last.group()[::-1] if last and last.start() == 0 else None


def spelled(node) -> str:
    """What a node writes, without the marks or the brackets around it."""
    raw = text(node)
    inside = unquoted(node)
    if inside is None and len(raw) >= 2 and BRACKETS.get(raw[0]) == raw[-1]:
        inside = raw[1:-1]
    return raw if inside is None else inside
