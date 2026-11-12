"""The solver: a value goes along every edge out of its node once; what is found by name carries functions only."""
from __future__ import annotations

import unittest

from support import AS_KEPT, AS_NAME, BY_NAME, EXACT, FN, INST, KEPT, Solver


class Edges(unittest.TestCase):
    def test_a_value_follows_the_edges_added_before_and_after_it(self):
        s = Solver()
        s.follow("a", "b")
        s.add("a", (FN, 1, EXACT))
        s.run()
        s.follow("b", "c")
        s.run()
        self.assertEqual([(FN, 1, EXACT)], list(s.of("c")))

    def test_an_edge_found_by_name_marks_functions_and_carries_nothing_else(self):
        s = Solver()
        s.follow("a", "b", AS_NAME)
        s.add("a", (FN, 1, EXACT))
        s.add("a", (INST, 2, EXACT))
        s.run()
        self.assertEqual([(FN, 1, BY_NAME)], list(s.of("b")))

    def test_what_was_found_by_name_goes_no_further_by_name(self):
        s = Solver()
        s.follow("a", "b", AS_NAME)
        s.follow("b", "c", AS_NAME)
        s.follow("b", "d")
        s.add("a", (FN, 1, EXACT))
        s.run()
        self.assertEqual(([], [(FN, 1, BY_NAME)]), (list(s.of("c")), list(s.of("d"))))

    def test_a_function_handed_to_an_object_is_kept_by_it(self):
        s = Solver()
        s.follow("given", "object", AS_KEPT)
        s.follow("object", "copy")
        s.add("given", (FN, 1, EXACT))
        s.add("given", (FN, 2, BY_NAME))
        s.run()
        self.assertEqual([(KEPT, 1, EXACT)], list(s.of("copy")))

    def test_a_watcher_sees_each_value_once(self):
        s, seen = Solver(), []
        s.add("a", (FN, 1, EXACT))
        s.watch("a", seen.append)
        s.run()
        s.watch("a", seen.append)
        self.assertEqual([(FN, 1, EXACT)] * 2, seen)


if __name__ == "__main__":
    unittest.main()
