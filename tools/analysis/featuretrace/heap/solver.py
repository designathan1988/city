"""The inclusion solver (log/trace-core-plan.md, Task 1; Andersen; difference propagation and the worklist discipline,
Pearce, Kelly and Hankin, SCAM 2003 — log/trace-research.md F1, Decision item 4): a node holds a set of objects that
only grows; an identity edge copies a node's objects into another, a subscription applies a transfer to each object new
to its source, so only differences propagate and the result is the least fixpoint. Every unit of work is an item of
the worklist — a node's new objects for all its edges and subscriptions, or one new edge's or subscription's first
application to what its source already holds — so no transfer runs inside another. Nodes are interned to integers the
first time they are named: the worklist, the edges and the sets are keyed by those integers, so a node keyed by a deep
expression tree is hashed once, not at every item.
"""
from __future__ import annotations

from collections import deque


class Solver:
    def __init__(self):
        self.ids: dict = {}                       # node -> its integer
        self.nodes: list = []                     # integer -> node
        self.sets: list = []                      # integer -> the objects it holds
        self.edges: list = []                     # integer -> the integers its objects are copied into
        self.subs: list = []                      # integer -> {(target, key): transfer}
        self.queue: deque = deque()               # (integer, new objects, None | the one edge or subscription)

    def id(self, node) -> int:
        found = self.ids.get(node)
        if found is None:
            found = self.ids[node] = len(self.nodes)
            self.nodes.append(node)
            self.sets.append(set())
            self.edges.append(None)
            self.subs.append(None)
        return found

    def held(self, node) -> set:
        """The objects a node holds (empty for a node never named)."""
        found = self.ids.get(node)
        return self.sets[found] if found is not None else set()

    def add(self, node, objects) -> None:
        self.add_id(self.id(node), objects)

    def add_id(self, i: int, objects) -> None:
        held = self.sets[i]
        new = [o for o in objects if o not in held]
        if new:
            held.update(new)
            if self.edges[i] or self.subs[i]:
                self.queue.append((i, new, None))

    def subscribe(self, source, target, key, transfer) -> None:
        """`target` takes `transfer` of every object `source` holds, now and later; once per (target, key). What the
        source already holds reaches the transfer as an item of the worklist."""
        i = self.id(source)
        subs = self.subs[i]
        if subs is None:
            subs = self.subs[i] = {}
        if (target, key) in subs:
            return
        subs[(target, key)] = transfer
        if self.sets[i]:
            self.queue.append((i, list(self.sets[i]), (target, key)))

    def follow(self, source, target) -> None:
        """`target` holds every object of `source` (an identity: a value into a variable, an argument into a
        parameter, a return into its call)."""
        if source == target:
            return
        i, j = self.id(source), self.id(target)
        edges = self.edges[i]
        if edges is None:
            edges = self.edges[i] = set()
        if j in edges:
            return
        edges.add(j)
        if self.sets[i]:
            self.queue.append((i, list(self.sets[i]), j))

    def run(self) -> None:
        queue, sets, edges, subs, add = self.queue, self.sets, self.edges, self.subs, self.add_id
        while queue:
            i, new, only = queue.popleft()
            if only is not None:
                if only.__class__ is int:
                    add(only, new)
                else:
                    subs[i][only](only[0], new)
                continue
            for j in list(edges[i] or ()):
                add(j, new)
            for (target, _), transfer in list((subs[i] or {}).items()):
                transfer(target, new)
