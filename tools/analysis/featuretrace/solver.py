"""The solver's core (log/trace-rules.md: S1-S5, "Difference propagation on every value").

A value node is a set that only grows: an expression tree in a unit, what a definition holds or returns, a parameter
or a variable (a place). Its contributions are subscriptions: a source node and a transfer applied to each value of the
source. When a source gains values, only the new ones go through its subscriptions (difference propagation, PKH 2003;
semi-naive evaluation): each transfer is distributive over the source's values, so the result is the least fixpoint.
A consumer (a reference being linked, a shape that is not distributive) reads nodes whole; when one of them grows it is
computed again, after every pending difference has propagated.
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
        self.value = {}                    # value node -> its values so far, in the order found
        self.known = defaultdict(set)      # value node -> the identities of its values
        self.subs = defaultdict(dict)      # source node -> {(target, transfer key): transfer}
        self.pending = deque()             # (node, its new values) still to pass through its subscriptions
        self.readers = defaultdict(set)    # node -> the consumers that read it whole
        self.stack = []                    # the consumers being computed, innermost last
        self.dirty = set()                 # consumers that read something that grew since they were computed
        self.work = deque()                # dirty consumers waiting to be computed again
        self.marks = {}                    # (consumer, node) -> how many of the node's values it has read
        self.grown = defaultdict(set)      # consumer -> the nodes it read that grew since
        self.substitution = None           # (node, its new values, read position) while a consumer is linked anew
        self.substitution_reads = 0

    # ---- value nodes -------------------------------------------------------------------------------------------
    def canon(self, text: str, ctx) -> tuple:
        return text, ctx

    def add(self, node, values) -> None:
        """Adds values to a node; only those it did not hold are kept, queued for its subscriptions, and wake the
        consumers that read it."""
        known = self.known[node]
        held = self.value.setdefault(node, [])
        new = []
        for text, ctx in values:
            if not text:
                continue
            text, ctx = self.canon(text, ctx)
            key = identity(text, ctx)
            if key not in known:
                known.add(key)
                held.append((text, ctx))
                new.append((text, ctx))
        if new:
            if self.subs.get(node):
                self.pending.append((node, new))
            self.grew(node)

    def subscribe(self, source, target, key: tuple, transfer) -> None:
        """`target` takes `transfer` of every value of `source`: those it holds now, and each new one later."""
        subs = self.subs[source]
        if (target, key) in subs:
            return
        subs[(target, key)] = transfer
        if self.value.get(source):
            transfer(target, list(self.value[source]))

    def propagate(self) -> None:
        """Passes every pending difference through the subscriptions until none is left."""
        while self.pending:
            node, new = self.pending.popleft()
            for (target, _), transfer in list(self.subs[node].items()):
                transfer(target, new)

    # ---- consumers ---------------------------------------------------------------------------------------------
    def substituted(self, node, new: list, position: int, run) -> int:
        """Runs a consumer with `node` read as only its new values at its `position`-th read (the others whole).
        Returns how many times the consumer read `node`."""
        outer = (self.substitution, self.substitution_reads)
        self.substitution, self.substitution_reads = (node, new, position), 0
        try:
            run()
            return self.substitution_reads
        finally:
            self.substitution, self.substitution_reads = outer

    def current(self, node) -> list:
        """A node's values as the consumer being run reads them (its difference in its place when substituted); how
        many it has read is kept, so the next difference is what came after."""
        held = self.value.get(node, ())
        if self.stack:
            self.marks[(self.stack[-1], node)] = len(held)
        if self.substitution is not None and self.substitution[0] == node:
            reads = self.substitution_reads
            self.substitution_reads += 1
            if reads == self.substitution[2]:
                return list(self.substitution[1])
        return list(held)

    def take_deltas(self, consumer) -> dict:
        """The values each node the consumer read gained since it read it (values only grow: the tail of the list)."""
        out = {}
        for node in self.grown.pop(consumer, ()):
            start = self.marks.get((consumer, node))
            if start is not None and len(self.value.get(node, ())) > start:
                out[node] = self.value[node][start:]
        return out

    def read(self, node) -> None:
        """The consumer being computed reads `node` whole: it is computed again when `node` grows."""
        if self.stack:
            self.readers[node].add(self.stack[-1])

    def grew(self, node) -> None:
        """`node` grew: every consumer that read it is computed again (over what grew)."""
        for reader in self.readers.get(node, ()):
            self.grown[reader].add(node)
            if reader not in self.dirty:
                self.dirty.add(reader)
                self.work.append(reader)
