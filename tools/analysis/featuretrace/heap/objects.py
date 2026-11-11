"""Abstract objects (log/trace-core-plan.md, Task 1): every value the program can make is an allocation site — the
place in the source that makes it, or the definition it is the value of — interned as an int. An object of a class
knows its class, so its members are found through the class's chain.
"""
from __future__ import annotations

from ..vocab import SITE_ARRAY, SITE_CLASS, SITE_FUNCTION, SITE_LITERAL, SITE_NEW, SITE_PRIMITIVE, SITE_TEXT


class Objects:
    def __init__(self):
        self.keys: list[tuple] = []
        self.ids: dict[tuple, int] = {}

    def make(self, *key) -> int:
        """The object of a site (made once, the same id each time it is asked for)."""
        if key not in self.ids:
            self.ids[key] = len(self.keys)
            self.keys.append(key)
        return self.ids[key]

    def kind(self, obj: int) -> str:
        return self.keys[obj][0]

    def class_of(self, obj: int) -> str | None:
        """The class an object is an instance of: a `new`'s class, an object literal's own class."""
        key = self.keys[obj]
        return key[1] if key[0] in (SITE_NEW, SITE_LITERAL) else None

    def class_itself(self, obj: int) -> str | None:
        """The class an object is (a class as a value: its constructor and its static members)."""
        key = self.keys[obj]
        return key[1] if key[0] == SITE_CLASS else None

    def function(self, obj: int) -> int | None:
        """The definition a function object runs."""
        key = self.keys[obj]
        return key[1] if key[0] == SITE_FUNCTION else None

    def text(self, obj: int) -> str | None:
        """The text of a string the analysis knows."""
        key = self.keys[obj]
        return key[1] if key[0] == SITE_TEXT else None

    def label(self, obj: int) -> str:
        """A readable name: its kind, then what identifies its site."""
        key = self.keys[obj]
        return "{}:{}".format(key[0], ":".join(str(part) for part in key[1:]))

    def primitive(self, kind: str) -> int:
        return self.make(SITE_PRIMITIVE, kind)

    def array(self, file: str, site: int) -> int:
        return self.make(SITE_ARRAY, file, site)
