"""Abstract objects (log/trace-core-plan.md, Tasks 1-3): every value the program can make is an allocation site — the
place in the source that makes it, or the definition it is the value of — interned as an int. An object the code makes
(`new`, a literal, an array, a function) carries the last part of its key from the context it was made in: one level of
heap context (`contexts.heap`), and a function its whole environment, what its free variables read; its site is its
key without it. An object
of a class knows its class, so its members are found through the class's chain; one a constructor function made knows
the function object that made it (its prototype is that function's `prototype`).
"""
from __future__ import annotations

from ..vocab import (SITE_ARRAY, SITE_CLASS, SITE_DECLARED, SITE_DECLARED_FN, SITE_FUNCTION, SITE_GENERATOR,
                     SITE_LITERAL, SITE_NEW, SITE_PRIMITIVE, SITE_PROMISE, SITE_TEXT)
from . import contexts

INSTANCE_SITE = (-1,)       # the site of a class's instance from outside (`Objects.from_outside`)
MADE = (SITE_NEW, SITE_LITERAL, SITE_ARRAY, SITE_FUNCTION, SITE_DECLARED, SITE_DECLARED_FN, SITE_GENERATOR,
        SITE_PROMISE)                                         # the kinds whose key ends with a heap context


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

    def kind(self, obj: int):
        return self.keys[obj][0]

    def site(self, obj: int) -> int:
        """The object's site without the context it was made in (what a method's context is keyed by)."""
        key = self.keys[obj]
        return self.make(*key[:-1]) if key[0] in MADE else obj

    def env(self, fn: int):
        """The context a function object was made in: its environment."""
        key = self.keys[fn]
        return key[-1] if key[0] == SITE_FUNCTION else contexts.ROOT

    def heap(self, obj: int):
        """The heap context an object was made in (a function's: its environment's)."""
        key = self.keys[obj]
        if key[0] == SITE_FUNCTION:
            return contexts.heap(key[-1])
        return key[-1] if key[0] in MADE else contexts.ROOT

    def class_of(self, obj: int) -> str | None:
        """The class an object is an instance of: a `new`'s class, an object literal's own class, a declared object's
        declared type."""
        key = self.keys[obj]
        return key[1] if key[0] in (SITE_NEW, SITE_LITERAL, SITE_DECLARED) else None

    def maker(self, obj: int) -> int | None:
        """The function object whose `new` made an object (a constructor function's), else None."""
        key = self.keys[obj]
        return key[4] if key[0] == SITE_NEW and key[4] >= 0 else None

    def class_value(self, key: str, env, holder: int) -> int:
        """A class evaluated in a run of the function `holder` (ECMA-262 ClassDefinitionEvaluation runs each time):
        its methods are closures of that run."""
        return self.make(SITE_CLASS, key, env, holder)

    def class_env(self, obj: int):
        """The run a class value was evaluated in: (its context, the function), None for a module's class."""
        key = self.keys[obj]
        return (key[2], key[3]) if key[0] == SITE_CLASS and len(key) > 3 else None

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
        """A readable name: its kind, then what identifies its site (its context left out)."""
        key = self.keys[obj]
        parts = key[1:-1] if key[0] in MADE else key[1:]
        return "{}:{}".format(key[0], ":".join(str(part) for part in parts))

    def primitive(self, kind: str) -> int:
        return self.make(SITE_PRIMITIVE, kind)

    def instance(self, key: str, file: str, site: int, maker: int, ctx) -> int:
        """The object `new` makes at a site in a context: of class `key`, by the function object `maker` (-1 for a
        class)."""
        return self.make(SITE_NEW, key, file, site, maker, contexts.heap(ctx))

    def literal(self, key: str, ctx) -> int:
        """An object literal's object, made in a context."""
        return self.make(SITE_LITERAL, key, contexts.heap(ctx))

    def array(self, file: str, site: int, ctx) -> int:
        return self.make(SITE_ARRAY, file, site, contexts.heap(ctx))

    def made_by_call(self, kind: str, gid: int, site) -> int:
        """The generator or the promise a call of `gid` makes at a site (where, the heap context)."""
        assert kind in (SITE_GENERATOR, SITE_PROMISE)
        return self.make(kind, gid, site[0], site[1])

    def from_outside(self, key: str) -> int:
        """The object standing for the instances of a class of the program that come from outside the code read
        (an entry method's `this`, a value of the class a declaration gives): one for the class, so what its
        constructor writes its methods read."""
        return self.make(SITE_DECLARED, key, INSTANCE_SITE, None, contexts.ROOT)

    def declared(self, key: str, site, env) -> int:
        """An object of the declared type `key` made at a site (where, the heap context); an object type written in
        place keeps the environment its member types are read in."""
        return self.make(SITE_DECLARED, key, site[0], env, site[1])

    def environment(self, obj: int):
        """The environment a declared object of an object type written in place reads its member types in."""
        key = self.keys[obj]
        return key[3] if key[0] == SITE_DECLARED else None
