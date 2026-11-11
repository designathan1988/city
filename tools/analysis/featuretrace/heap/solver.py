"""The inclusion solver (log/trace-core-plan.md, Task 1; Andersen; difference propagation, Pearce, Kelly and Hankin):
a node holds a set of objects that only grows; a subscription applies a transfer to each object new to its source, so
only differences propagate and the result is the least fixpoint. A transfer may wire new nodes and subscriptions.
"""
from __future__ import annotations

from collections import defaultdict, deque


class Solver:
    def __init__(self):
        self.pts: dict = defaultdict(set)
        self.subs: dict = defaultdict(dict)       # source -> {(target, key): transfer}
        self.queue: deque = deque()

    def add(self, node, objects) -> None:
        held = self.pts[node]
        new = [o for o in objects if o not in held]
        if new:
            held.update(new)
            if self.subs.get(node):
                self.queue.append((node, new))

    def subscribe(self, source, target, key, transfer) -> None:
        """`target` takes `transfer` of every object `source` holds, now and later; once per (target, key)."""
        subs = self.subs[source]
        if (target, key) in subs:
            return
        subs[(target, key)] = transfer
        if self.pts.get(source):
            transfer(target, list(self.pts[source]))

    def follow(self, source, target) -> None:
        """`target` holds every object of `source` (an identity: a value into a variable, an argument into a
        parameter, a return into its call)."""
        if source != target:
            self.subscribe(source, target, None, self.add)

    def run(self) -> None:
        while self.queue:
            node, new = self.queue.popleft()
            for (target, _), transfer in list(self.subs[node].items()):
                transfer(target, new)
