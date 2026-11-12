"""The flow's solver: values added to nodes, each value carried along the edges out of its node and shown to those
watching it, once, until nothing is new. A value is (kind, definition, how it was found). An edge says what it
carries (`vocab`): found by name, only the functions known exactly, marked so, for a guess is no ground for another.
"""
from __future__ import annotations

from collections import defaultdict, deque

from .vocab import AS_BUILT, AS_KEPT, AS_NAME, BY_NAME, EXACT, FN, KEPT, TOLD, TXT

KEEPS = {FN: KEPT, KEPT: KEPT, TXT: TOLD, TOLD: TOLD}


def marked(how: str) -> str | None:
    """The edge a value found that way goes along."""
    return AS_NAME if how == BY_NAME else None


class Solver:
    def __init__(self):
        self.values = defaultdict(set)       # node -> every value added to it
        self.shown = defaultdict(list)       # node -> the values already carried on
        self.edges = defaultdict(dict)       # node -> {(node after it, what the edge carries): None}
        self.watchers = defaultdict(list)    # node -> the functions shown each value
        self.work = deque()

    def add(self, key, value: tuple) -> None:
        held = self.values[key]
        if value not in held:
            held.add(value)
            self.work.append((key, value))

    def carry(self, value: tuple, dst, mark) -> None:
        kind, d, how = value
        if mark is None:
            self.add(dst, value)
        elif how != EXACT:
            return
        elif (mark == AS_BUILT and kind in KEEPS) or (mark == AS_KEPT and kind in (FN, KEPT)):
            self.add(dst, (KEEPS[kind], d, EXACT))                # what an object is handed: kept by it
        elif mark == AS_NAME and kind in (FN, KEPT):
            self.add(dst, (kind, d, BY_NAME))

    def follow(self, src, dst, mark=None) -> None:
        """Every value of `src` is one of `dst`, as the edge carries it."""
        edge = (dst, mark)
        if edge not in self.edges[src]:
            self.edges[src][edge] = None
            for value in list(self.shown[src]):
                self.carry(value, dst, mark)

    def watch(self, key, seen) -> None:
        """Calls `seen` with every value of the node, the ones to come as well."""
        self.watchers[key].append(seen)
        for value in list(self.shown[key]):
            seen(value)

    def run(self) -> None:
        while self.work:
            key, value = self.work.popleft()
            self.shown[key].append(value)
            for dst, mark in list(self.edges[key]):
                self.carry(value, dst, mark)
            for seen in list(self.watchers[key]):
                seen(value)

    def of(self, key) -> list:
        return self.shown.get(key, ())
