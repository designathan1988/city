"""The inclusion solver (log/trace-core-plan.md, Task 1; Andersen; difference propagation and the worklist discipline,
Pearce, Kelly and Hankin, SCAM 2003 — log/trace-research.md F1): a node holds a set of objects that only grows; a
subscription applies a transfer to each object new to its source, so only differences propagate and the result is the
least fixpoint. Every unit of work is an item of the worklist — a node's new objects for all its subscriptions, or a new
subscription's first application to what its source already holds — so no transfer runs inside another: wiring stays
shallow whatever the program, and a transfer may wire new nodes and subscriptions freely.
"""
from __future__ import annotations

from collections import defaultdict, deque


class Solver:
    def __init__(self):
        self.pts: dict = defaultdict(set)
        self.subs: dict = defaultdict(dict)       # source -> {(target, key): transfer}
        self.queue: deque = deque()               # (node, new objects, the one subscription or None for all)

    def add(self, node, objects) -> None:
        held = self.pts[node]
        new = [o for o in objects if o not in held]
        if new:
            held.update(new)
            if self.subs.get(node):
                self.queue.append((node, new, None))

    def subscribe(self, source, target, key, transfer) -> None:
        """`target` takes `transfer` of every object `source` holds, now and later; once per (target, key). What the
        source already holds reaches the transfer as an item of the worklist."""
        subs = self.subs[source]
        if (target, key) in subs:
            return
        subs[(target, key)] = transfer
        if self.pts.get(source):
            self.queue.append((source, list(self.pts[source]), (target, key)))

    def follow(self, source, target) -> None:
        """`target` holds every object of `source` (an identity: a value into a variable, an argument into a
        parameter, a return into its call)."""
        if source != target:
            self.subscribe(source, target, None, self.add)

    def run(self) -> None:
        while self.queue:
            node, new, only = self.queue.popleft()
            if only is not None:
                self.subs[node][only](only[0], new)
                continue
            for (target, _), transfer in list(self.subs[node].items()):
                transfer(target, new)
