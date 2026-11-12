"""Which runs of each function get contexts (log/trace-research.md F44; Li, Tan, Moller, Smaragdakis, "Scalability-First
Pointer Analysis with Self-Tuning Context-Sensitivity", ESEC/FSE 2018): the choice of each function's most precise
kind of context within one bound, the largest bound whose costs together stay within the total; and the analyses,
both cores, honouring the choice — a wrapper's callers apart where the total allows it, together where it does not
(fixture `heap/contexts.js`)."""
from __future__ import annotations

import unittest

from featuretrace import engine_result, program
from featuretrace.heap import analysis, scaler

from .support import FIXTURE, ENGINE, HeapCase


class Choice(unittest.TestCase):
    COSTS = {1: [(scaler.OBJECT, 50), (scaler.CALL, 20), (scaler.ONCE, 5)],
             2: [(scaler.OBJECT, 8), (scaler.CALL, 6), (scaler.ONCE, 2)],
             3: [(scaler.ONCE, 1)]}

    def test_a_total_that_holds_every_most_precise_kind_gives_it_to_every_function(self):
        self.assertEqual({1: scaler.OBJECT, 2: scaler.OBJECT, 3: scaler.ONCE}, scaler.choose(self.COSTS, 59))

    def test_the_largest_bound_within_the_total_decides_each_function(self):
        # bound 20: function 1 takes its call sites (20), function 2 its objects (8), 3 runs once: 29 <= 30
        self.assertEqual({1: scaler.CALL, 2: scaler.OBJECT, 3: scaler.ONCE}, scaler.choose(self.COSTS, 30))

    def test_a_total_too_small_for_any_context_runs_each_function_once(self):
        self.assertEqual({1: scaler.ONCE, 2: scaler.ONCE, 3: scaler.ONCE}, scaler.choose(self.COSTS, 1))


class Honoured(HeapCase):
    """The engine solves the fixture without contexts, the choice is made from it, and the core analyses with it."""
    ROOT = "contexts.js"
    TOTAL = scaler.TOTAL

    def setUp(self):
        ix, _, _ = program.read([FIXTURE / "heap" / self.ROOT], FIXTURE / "heap", None)
        levels = program.deep(lambda: scaler.levels(ix, engine_result.analyse(ix, contexts=False), self.TOTAL))
        run = (lambda: engine_result.analyse(ix, levels=levels)) if self.CORE == ENGINE else \
            (lambda: analysis.analyse(ix, levels=levels))
        self.ix, self.result = ix, program.deep(run)

    def test_the_wrapper_s_callers_are_apart_or_together_as_the_total_allows(self):
        self.assertEqual({"Apple"} if self.TOTAL > 1 else {"Apple", "Pear"}, self.classes("wrappedApple"))


class HonouredEngine(Honoured):
    CORE = ENGINE


class HonouredWithin(Honoured):
    TOTAL = 1


class HonouredWithinEngine(HonouredWithin):
    CORE = ENGINE


if __name__ == "__main__":
    unittest.main()
