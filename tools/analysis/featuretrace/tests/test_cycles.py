"""Cycles of identity subscriptions kept as one node (log/trace-rules.md, "Collapsing cycles of identical values"):
the solver alone, its values and its consumers checked against the fixpoint without merging."""
from __future__ import annotations

import random
import sys
import unittest
from collections import defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))

from featuretrace.solver import ELEM, SAME, Solver  # noqa: E402


class _Where:
    lang, owner, file, gid = "js", "", "", 0


WHERE = _Where()


def value(name: str) -> tuple:
    return "#" + name, WHERE


def elements(s):
    """A transfer that is not an identity: the values put into a container (as `transfer.elements`)."""
    def transfer(target, values):
        s.add(target, [(ELEM + text, ctx) for text, ctx in values if not text.startswith(ELEM)])
    return transfer


def texts(values) -> set:
    return {text for text, _ in values}


def read(s, consumer, node) -> set:
    """`consumer` reads `node` whole, as a reference being linked does."""
    s.stack.append(consumer)
    try:
        s.read(node)
        return texts(s.current(node))
    finally:
        s.stack.pop()


def drain(s, seen: dict) -> None:
    """The build loop's drain: every difference first, then each consumer over what grew of what it read (each
    value it is given appended to `seen[consumer]`, a list: a value given twice shows)."""
    while True:
        s.propagate()
        if not s.work:
            return
        consumer = s.work.popleft()
        if consumer not in s.dirty:
            continue
        s.dirty.discard(consumer)
        for node, new in s.take_deltas(consumer).items():
            seen[consumer] += [text for text, _ in new]
            read(s, consumer, node)


def closure(nodes, same_edges, element_edges, seeds) -> dict:
    """The least fixpoint computed plainly, by iterating every edge until nothing changes."""
    held = {n: set() for n in nodes}
    for node, text in seeds:
        held[node].add(text)
    changed = True
    while changed:
        changed = False
        for a, b in same_edges:
            if not held[a] <= held[b]:
                held[b] |= held[a]
                changed = True
        for a, b in element_edges:
            put = {ELEM + t for t in held[a] if not t.startswith(ELEM)}
            if not put <= held[b]:
                held[b] |= put
                changed = True
    return held


class Cycles(unittest.TestCase):
    def test_follow_is_an_identity_subscription(self):
        s = Solver()
        s.follow("a", "b")
        s.add("a", [value("X")])
        s.propagate()
        self.assertEqual([text for text, _ in s.current("b")], ["#X"])
        self.assertIn(("b", SAME), s.subs[s.find("a")])

    def test_a_cycle_of_identity_edges_is_one_node(self):
        s = Solver()
        s.follow("a", "b")
        s.follow("b", "c")
        s.follow("c", "a")
        s.add("a", [value("X")])
        s.propagate()
        self.assertEqual(len({s.find(n) for n in "abc"}), 1)
        self.assertEqual([n for n in "abc" if n in s.value], [s.find("a")])
        self.assertEqual(texts(s.current("c")), {"#X"})

    def test_values_reach_every_member_subscription(self):
        # each member's subscription gets the values it lacked, and the differences waiting for it, exactly once
        s = Solver()
        got = defaultdict(list)

        def recorder(name):
            def transfer(target, values):
                got[name].extend(text for text, _ in values)
            return transfer

        s.follow("a", "b")
        s.follow("b", "a")
        s.subscribe("a", "ta", ("record",), recorder("a"))
        s.subscribe("b", "tb", ("record",), recorder("b"))
        s.add("a", [value("X")])
        s.add("b", [value("Y")])
        s.propagate()
        self.assertEqual(s.find("a"), s.find("b"))
        self.assertEqual(sorted(got["a"]), ["#X", "#Y"])
        self.assertEqual(sorted(got["b"]), ["#X", "#Y"])

    def test_a_reader_of_a_merged_node_reads_the_union(self):
        s = Solver()
        s.follow("a", "b")
        s.follow("b", "a")
        s.add("b", [value("Y")])
        read(s, "c", "b")
        s.add("a", [value("X"), value("Z")])
        s.propagate()
        self.assertEqual(s.find("b"), "a")             # the member holding the most values
        self.assertIn("c", s.dirty)
        self.assertEqual([text for text, _ in s.take_deltas("c")[s.find("b")]], ["#X", "#Z"])   # #Y it had read

    def test_only_identity_edges_join_a_cycle(self):
        s = Solver()
        s.follow("a", "b")
        s.subscribe("b", "a", ("elem",), elements(s))
        s.add("a", [value("X")])
        s.propagate()
        self.assertNotEqual(s.find("a"), s.find("b"))

    def test_no_sweep_while_a_consumer_runs(self):
        s = Solver()
        s.stack.append("c")
        s.follow("a", "b")
        s.follow("b", "a")
        s.add("a", [value("X")])
        s.propagate()
        self.assertNotEqual(s.find("a"), s.find("b"))
        s.stack.pop()
        s.add("a", [value("Y")])
        s.propagate()
        self.assertEqual(s.find("a"), s.find("b"))
        self.assertEqual(texts(s.current("b")), {"#X", "#Y"})

    def test_a_long_cycle_is_collapsed_without_recursion(self):
        s = Solver()
        ring = [f"n{i}" for i in range(20000)]
        for i, node in enumerate(ring):
            s.follow(node, ring[(i + 1) % len(ring)])
        s.add(ring[0], [value("X")])
        s.propagate()
        self.assertEqual(len({s.find(n) for n in ring}), 1)
        self.assertEqual(texts(s.current(ring[-1])), {"#X"})

    def test_collapsed_values_equal_the_plain_fixpoint(self):
        merging = 0
        for seed in range(20):
            rng = random.Random(seed)
            s = Solver()
            nodes = [f"n{i}" for i in range(40)]
            same_edges = [(rng.choice(nodes), rng.choice(nodes)) for _ in range(100)]
            element_edges = [(rng.choice(nodes), rng.choice(nodes)) for _ in range(20)]
            seeds = [(rng.choice(nodes), f"#V{k}") for k in range(15)]
            steps = [("same", e) for e in same_edges] + [("elem", e) for e in element_edges] \
                + [("value", x) for x in seeds] + [("read", rng.choice(nodes)) for _ in range(10)] \
                + [("drain", None)] * 15
            rng.shuffle(steps)
            seen, reads = defaultdict(list), {}
            for kind, item in steps:
                if kind == "same":
                    s.follow(*item)
                elif kind == "elem":
                    s.subscribe(item[0], item[1], ("elem",), elements(s))
                elif kind == "value":
                    s.add(item[0], [(item[1], WHERE)])
                elif kind == "read":
                    consumer = ("c", len(reads))
                    reads[consumer] = item
                    seen[consumer] += sorted(read(s, consumer, item))
                else:
                    drain(s, seen)
            drain(s, seen)
            expected = closure(nodes, same_edges, element_edges, seeds)
            for node in nodes:
                self.assertEqual(texts(s.current(node)), expected[node], (seed, node))
            for consumer, node in reads.items():
                self.assertEqual(set(seen[consumer]), expected[node], (seed, consumer))
                self.assertEqual(len(seen[consumer]), len(set(seen[consumer])), (seed, consumer))   # none twice
            merging += bool(s.parent)
        self.assertGreater(merging, 0)                 # the graphs drawn do have cycles, and they were merged


if __name__ == "__main__":
    unittest.main()
