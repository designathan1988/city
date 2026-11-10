"""The solver's core (log/trace-rules.md, S1-S5): nodes with values that only grow, who read each node, and the
worklist of nodes to compute again. A node is a value (an expression tree in a unit), a reference being linked, or one
value carried by one link; a place (a parameter, a variable) is what flows accumulate into.
"""
from __future__ import annotations

from collections import defaultdict, deque

from .library import ENGINE

CLASS, FN, ELEM, TEXT = "#", "<fn>", "<elem>", "<text>"   # value forms; TEXT + s: a string value carrying s (G7)


def identity(text: str, ctx) -> tuple:
    """What makes two values the same: a class, a function, a text or an engine value is itself; a type as written
    is itself in the scope it is read in (the class and file, not each unit)."""
    core = text[len(ELEM):] if text.startswith(ELEM) else text
    if core == ENGINE or core.startswith((CLASS, FN, TEXT)):
        return (text,)
    return (text, ctx.lang, ctx.owner, ctx.file)


class Solver:
    def __init__(self):
        self.value = {}                    # node -> its value so far (values are unions: they only grow)
        self.readers = defaultdict(set)    # node -> the nodes whose computation read it
        self.stack = []                    # the nodes being computed, innermost last
        self.busy = set()
        self.dirty = set()                 # nodes that read something that grew since they were computed
        self.work = deque()                # dirty nodes waiting to be computed again

    def read(self, node) -> None:
        """The node being computed depends on `node`: it is computed again when `node` grows."""
        if self.stack:
            self.readers[node].add(self.stack[-1])

    def grew(self, node) -> None:
        """`node` grew: every node that read it is computed again."""
        for reader in self.readers.get(node, ()):
            if reader not in self.dirty:
                self.dirty.add(reader)
                self.work.append(reader)
