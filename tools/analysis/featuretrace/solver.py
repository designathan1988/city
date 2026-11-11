"""The solver's core (log/trace-rules.md: S1-S5, "Difference propagation on every value", "Collapsing cycles of
identical values").

A value node is a set that only grows: an expression tree in a unit, what a definition holds or returns, a parameter
or a variable (a place). Its contributions are subscriptions: a source node and a transfer applied to each value of the
source. When a source gains values, only the new ones go through its subscriptions (difference propagation, PKH 2003;
semi-naive evaluation): each transfer is distributive over the source's values, so the result is the least fixpoint.
A consumer (a reference being linked, a shape that is not distributive) reads nodes whole; when one of them grows it is
computed again, after every pending difference has propagated.

Nodes on a cycle of identity subscriptions (`follow`) always hold the same set: `cycles` merges them into one node, and
every node key given to the solver is read as the node that holds its values (`find`, union-find).
"""
from __future__ import annotations

from collections import defaultdict, deque

from . import cycles
from .library import ENGINE

CLASS, FN, ELEM, TEXT = "#", "<fn>", "<elem>", "<text>"   # value forms; TEXT + s: a string value carrying s (G7)
LANG = "<lang>"            # a constant of the language (V1): `true`, `false`, `null`, `undefined`
TRUE, FALSE, NULL, UNDEFINED = LANG + "true", LANG + "false", LANG + "null", LANG + "undefined"
SAME = ("same",)           # the key of an identity subscription: the target holds every value of the source


def is_language(text: str) -> bool:
    """A value of the language or the engine: the engine's value, or one of the language's constants (V1), which
    every rule that does not test it treats as the engine's."""
    return text == ENGINE or text.startswith(LANG)


def identity(text: str, ctx) -> tuple:
    """What makes two values the same: a class, a function, a text, a constant or an engine value is itself; a type as
    written is itself in the scope it is read in (the class and file, not each unit); a JavaScript value is itself, its
    names resolved when it was made (T3)."""
    core = text[len(ELEM):] if text.startswith(ELEM) else text
    if core == ENGINE or core.startswith((CLASS, FN, TEXT, LANG)) or ctx.lang == "js":
        return (text,)
    return (text, ctx.lang, ctx.owner, ctx.file)


class Solver:
    def __init__(self):
        self.value = {}                    # value node -> its values so far, in the order found
        self.known = defaultdict(set)      # value node -> the identities of its values
        self.subs = defaultdict(dict)      # source node -> {(target, transfer key): transfer}
        self.pending = deque()             # (node, its new values, the subscriptions they go to: None, all of node's)
        self.readers = defaultdict(set)    # node -> the consumers that read it whole
        self.stack = []                    # the consumers being computed, innermost last
        self.dirty = set()                 # consumers that read something that grew since they were computed
        self.work = deque()                # dirty consumers waiting to be computed again
        self.marks = {}                    # (consumer, node) -> how many of the node's values it has read
        self.grown = defaultdict(set)      # consumer -> the nodes it read that grew since
        self.extra = defaultdict(dict)     # consumer -> {node: values a merge owes it, read before the node's tail}
        self.substitution = None           # (node, its new values, read position) while a consumer is linked anew
        self.substitution_reads = 0
        self.parent = {}                   # a node merged into another -> that node (union-find)
        self.same_edges = 0                # identity subscriptions made
        self.sweep_at = 1                  # the identity subscriptions made when cycles are next looked for (`cycles`)

    # ---- value nodes -------------------------------------------------------------------------------------------
    def canon(self, text: str, ctx) -> tuple:
        return text, ctx

    def find(self, node):
        """The node that holds `node`'s values: itself, or the node of the cycle it was merged into."""
        parent = self.parent
        if node not in parent:
            return node
        root = node
        while root in parent:
            root = parent[root]
        while node != root:                # every node on the way points at the root (path compression)
            parent[node], node = root, parent[node]
        return root

    def add(self, node, values) -> None:
        """Adds values to a node; only those it did not hold are kept, queued for its subscriptions, and wake the
        consumers that read it."""
        node = self.find(node)
        known = self.known[node]
        held = self.value.setdefault(node, [])
        new = []
        for text, ctx in values:
            if not text:
                continue
            text, ctx = self.canon(text, ctx)
            if not text:
                continue                   # a type with no runtime value (erased, T1)
            key = identity(text, ctx)
            if key not in known:
                known.add(key)
                held.append((text, ctx))
                new.append((text, ctx))
        if new:
            if self.subs.get(node):
                self.pending.append((node, new, None))
            self.grew(node)

    def subscribe(self, source, target, key: tuple, transfer) -> None:
        """`target` takes `transfer` of every value of `source`: those it holds now, and each new one later."""
        source, target = self.find(source), self.find(target)
        subs = self.subs[source]
        if (target, key) in subs:
            return
        if key == SAME:
            if target == source:
                return                     # a node into itself (two nodes of one cycle): it holds its values already
            self.same_edges += 1
        subs[(target, key)] = transfer
        if self.value.get(source):
            transfer(target, list(self.value[source]))

    def follow(self, source, target) -> None:
        """`target` holds every value of `source` (an identity subscription: an argument into a parameter, a value
        into a local or a variable, a return into its call)."""
        self.subscribe(source, target, SAME, self.add)

    def propagate(self) -> None:
        """Passes every pending difference through the subscriptions until none is left; the cycles of identity
        subscriptions are merged first whenever enough of them were made since they were last looked for."""
        while self.pending:
            if self.same_edges >= self.sweep_at and not self.stack:
                cycles.sweep(self)         # between consumers only: a running consumer's read marks stay valid
            node, new, only = self.pending.popleft()
            for (target, _), transfer in (only if only is not None else list(self.subs.get(node, {}).items())):
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
        node = self.find(node)
        held = self.value.get(node, ())
        if self.stack:
            self.marks[(self.stack[-1], node)] = len(held)
        if self.substitution is not None and self.substitution[0] == node:
            reads = self.substitution_reads
            self.substitution_reads += 1
            if reads == self.substitution[2]:
                return list(self.substitution[1])
        return list(held)

    def held(self, node) -> list:
        """A node's values as a transfer reads another node beside its source (no consumer reads it)."""
        return list(self.value.get(self.find(node), ()))

    def take_deltas(self, consumer) -> dict:
        """The values each node the consumer read gained since it read it (values only grow: the tail of the list),
        after those a merge owes it (`cycles.collapse`)."""
        out = {}
        owed = self.extra.pop(consumer, {})
        for node in [*self.grown.pop(consumer, ()), *owed]:
            node = self.find(node)
            if node in out:
                continue
            new = list(owed.get(node, ()))
            start = self.marks.get((consumer, node))
            if start is not None and len(self.value.get(node, ())) > start:
                new += self.value[node][start:]
            if new:
                out[node] = new
        return out

    def read(self, node) -> None:
        """The consumer being computed reads `node` whole: it is computed again when `node` grows."""
        if self.stack:
            self.readers[self.find(node)].add(self.stack[-1])

    def grew(self, node) -> None:
        """`node` grew: every consumer that read it is computed again (over what grew)."""
        for reader in self.readers.get(node, ()):
            self.wake(reader, node)

    def wake(self, reader, node) -> None:
        """`reader` is computed again over what `node` holds beyond what it read."""
        self.grown[reader].add(node)
        if reader not in self.dirty:
            self.dirty.add(reader)
            self.work.append(reader)
