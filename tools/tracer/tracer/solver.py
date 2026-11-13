"""The flow's solver: values added to nodes, each value carried along the edges out of its node and shown to those
watching it, once, until nothing is new. A value is (kind, definition, how it was found). An edge says what it
carries (`vocab`): found by name, only the functions known exactly, marked so, for a guess is no ground for another.

Each node keeps its values once, in the order they came (the first `carried[node]` of them carried on already), and
every value is kept once for all the nodes holding it (memory). A node's values and its edges are a small list while
they are few (most nodes hold one or two), a table once they grow (memory: a table costs several times a list).
"""
from __future__ import annotations

from collections import deque
from itertools import islice

from .vocab import (AS_BUILT, AS_GUESS, AS_KEPT, AS_NAME, AS_OBJECTS, BY_NAME, CLS, EXACT, FN, INST, KEPT, OBJECTS,
                    OUTSIDE, PLATFORM, TOLD, TXT)

KEEPS = {FN: KEPT, KEPT: KEPT, TXT: TOLD, TOLD: TOLD}
LOOK = 512                               # how many values are carried between two looks at the ceiling
SMALL = 8                                # past so many values (or edges), a node keeps them in a table


def marked(how: str) -> str | None:
    """The edge a value found that way goes along: what a guess reaches is a guess, and carries the functions it
    finds, not the ones the objects it finds keep."""
    return AS_GUESS if how == BY_NAME else None


class Solver:
    def __init__(self):
        self.values = {}                     # node -> [value] or {value: None}, in the order added
        self.carried = {}                    # node -> how many of its values are carried on already
        self.edges = {}                      # node -> [(node after it, what the edge carries)] or {that: None}
        self.watchers = {}                   # node -> the functions shown each value
        self.work = deque()
        self.plain = set()                   # nodes that hold no object of a class (declared as plain values)
        self.same = {}                       # every value once: the nodes holding it share it (memory)
        self.check = None                    # asked now and then whether to stop (a ceiling reached): True stops
        self.stopped = False

    def add(self, key, value: tuple) -> None:
        if value[0] in (CLS, INST) and key in self.plain:
            return
        held = self.values.get(key)
        if held is None:
            value = self.same.setdefault(value, value)
            self.values[key] = [value]
            self.work.append((key, value))
            return
        if value in held:
            return
        value = self.same.setdefault(value, value)
        if type(held) is list:
            if len(held) < SMALL:
                held.append(value)
            else:
                held = self.values[key] = dict.fromkeys(held)
                held[value] = None
        else:
            held[value] = None
        self.work.append((key, value))

    def carry(self, value: tuple, dst, mark) -> None:
        kind, d, how = value
        if mark is None:
            self.add(dst, value)
        elif mark == AS_OBJECTS:                                  # the objects alone; what code not read gives
            if kind in OBJECTS:                                   # back of the platform's object is an object
                self.add(dst, OUTSIDE if value == PLATFORM else value)   # of a type not defined here
        elif how != EXACT:
            return
        elif (mark == AS_BUILT and kind in KEEPS) or (mark == AS_KEPT and kind in (FN, KEPT)):
            self.add(dst, (KEEPS[kind], d, EXACT))                # what an object is handed: kept by it
        elif (mark == AS_NAME and kind in (FN, KEPT)) or (mark == AS_GUESS and kind == FN):
            self.add(dst, (kind, d, BY_NAME))

    def of(self, key) -> list:
        """The values of a node carried on so far."""
        held = self.values.get(key)
        if not held:
            return []
        return list(islice(held, self.carried.get(key, 0)))

    def follow(self, src, dst, mark=None) -> None:
        """Every value of `src` is one of `dst`, as the edge carries it."""
        edge = (dst, mark)
        out = self.edges.get(src)
        if out is None:
            self.edges[src] = [edge]
        elif edge in out:
            return
        elif type(out) is list and len(out) < SMALL:
            out.append(edge)
        elif type(out) is list:
            out = self.edges[src] = dict.fromkeys(out)
            out[edge] = None
        else:
            out[edge] = None
        for value in self.of(src):
            self.carry(value, dst, mark)

    def watch(self, key, seen) -> None:
        """Calls `seen` with every value of the node, the ones to come as well."""
        self.watchers.setdefault(key, []).append(seen)
        for value in self.of(key):
            seen(value)

    def settle(self) -> None:
        """The flow is done: what carries values on is let go (memory); the values stay, read as they are."""
        self.edges, self.watchers, self.same, self.work = {}, {}, {}, deque()

    def run(self) -> None:
        if self.stopped:                         # stopped at the ceiling: nothing more is carried
            self.work.clear()
            return
        edges, watchers, carried = self.edges, self.watchers, self.carried
        done = 0
        while self.work:
            done += 1
            if self.check is not None and done % LOOK == 0 and self.check():
                self.work.clear()                # the ceiling reached: what is left is not carried (said)
                self.stopped = True
                return
            key, value = self.work.popleft()
            carried[key] = carried.get(key, 0) + 1
            out = edges.get(key)
            if out:
                for dst, mark in list(out):
                    self.carry(value, dst, mark)
            seen_by = watchers.get(key)
            if seen_by:
                for seen in list(seen_by):
                    seen(value)
