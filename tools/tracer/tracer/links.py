"""What the calls find, as records: a link from one definition to another (how it was found, where it is written, what
the call wrote there), a place of the code that calls or makes (`Site`, what is known of it as the flow runs), and the
result of the whole analysis (the links by where they start and where they end, the callees no definition answers,
the tables read by a key, the calls said ambiguous). Built by `calls`, read by `walk` and `report`."""
from __future__ import annotations

from dataclasses import dataclass


@dataclass
class Link:
    src: int
    dst: int
    kind: str
    how: str
    file: int
    line: int
    texts: frozenset = frozenset()   # the functions' names given in texts where the link is written
    fns: frozenset = frozenset()     # the functions given there
    words: frozenset = frozenset()   # the texts written as its arguments there


@dataclass
class Site:
    f: object
    node: object
    kind: str
    args: list
    key: tuple
    unit: int
    receiver: tuple | None = None    # the node holding the object the callee is a member of
    places: list | None = None       # where that object is kept
    shown: str = ""                  # the callee as written
    exact: bool = False              # a definition answers it by the scopes or the declared types
    found: bool = False              # a definition answers it, by them or by name
    answered: bool = False           # and one of them is written with a body
    handed: bool = False
    direct: bool = False             # the object called on is read as a member where it is called
    name: str = ""                   # the member called, where the callee is one
    gave: bool = False               # a function is among its arguments
    kept: list | None = None         # the functions the object called on holds
    around: int | None = None        # the class whose member a callee written as a plain name may be
    fns: list | None = None          # the functions among its arguments, known exactly
    texts: set | None = None         # the functions' names written in the texts it is given
    targets: list | None = None      # what it links to: (definition, kind)
    given: set | None = None         # the functions among its arguments, however they were found
    told: set | None = None          # the ones the object it is called on was handed, and keeps
    objects: set | None = None       # the classes of the objects it is given or called on
    typed: list | None = None        # the types it is written with (`new A<B>()`)
    words: frozenset = frozenset()   # the texts written as its arguments
    guesses: set = None              # the functions it runs found by name alone
    platform: bool = False           # what is called, or the object it is called on, is the platform's
    passed: bool = False             # what code not read is given was given back (`through`)


@dataclass
class Result:
    program: object
    scopes: object
    out: dict                        # definition -> its links, in the order written
    into: dict                       # definition -> the links reaching it
    outside: dict                    # definition -> the callees it writes that no definition answers
    tables: dict = None              # definition -> {function, or function's name: the keys of the entries holding it}
    ambiguous: dict = None           # definition -> [(the callee as written, how many functions of its name)]
