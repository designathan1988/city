"""Closures valued by the call that makes them (log/trace-rules.md, C4, C4'): a maker answered per the values its
closures' free variables read, each closure the function with its maker's template (fixture `jsvalues/closures.js`)."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))
sys.path.insert(0, str(HERE))

from support import FixtureCase, Solved  # noqa: E402
from featuretrace.solver import CLASS, FN  # noqa: E402

APPLE, PEAR = CLASS + "closures.js#Apple", CLASS + "closures.js#Pear"
ENV = "~"


class Closures(Solved, FixtureCase):
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"

    def test_each_memo_runs_its_own_function(self):
        self.assertEqual({APPLE}, self.returned("memoApple"))
        self.assertEqual({PEAR}, self.returned("memoPear"))

    def test_a_local_made_from_the_parameter(self):
        self.assertEqual({APPLE}, self.returned("aliasApple"))

    def test_a_closure_inside_a_closure(self):
        self.assertEqual({APPLE}, self.returned("nestedApple"))

    def test_a_closure_that_reads_no_parameter_is_one_value(self):
        found = self.returned("plainValue")
        self.assertEqual(1, len(found), found)
        self.assertTrue(all(v.startswith(FN) and ENV not in v for v in found), found)


if __name__ == "__main__":
    unittest.main()
