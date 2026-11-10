"""What runs and what is only handed over; calling a value; the implicit calls added on the review (C7)."""
from __future__ import annotations

import unittest

from .support import FixtureCase


class Calls(FixtureCase):
    def test_a_callable_given_to_the_program_runs_where_it_is_called(self):
        # `keeps(cb)` never calls it; `call_deferred_thing(cb)` does: the lambda runs from there, not from `hands`
        lam = next(n for n in self.names("Controller.call_deferred_thing", ("call",)) if "lambda" in n)
        self.assertNotIn(lam, self.names("Controller.keeps", ("call", "handed")))
        self.assertIn("Controller.go_by_name", self.reached("Controller.call_deferred_thing"))

    def test_a_callable_given_to_the_engine_is_run_by_it(self):
        handed = self.names("Controller.hands", ("handed",))
        self.assertTrue(any("lambda" in n for n in handed), handed)

    def test_match_pattern_reads_the_names_it_compares(self):
        self.assertIn("Controller.WANTED", self.names("Controller.hands", ("read",)))

    def test_calling_an_object_calls_its_call_operator(self):
        self.assertIn("fx::Functor::operator()", self.names("fx::takes", ("call",)))
        self.assertIn("fx::Functor::operator()", self.names("fx::more_implicit", ("call",)))

    def test_assignment_and_increment_operators(self):
        calls = self.names("fx::more_implicit", ("call",))
        self.assertTrue({"fx::Functor::operator=", "fx::Functor::operator++"} <= calls, calls)

    def test_brace_construction(self):
        self.assertIn("fx::Vec::Vec", self.names("fx::more_implicit", ("new",)))

    def test_lambda_given_to_the_library_is_run_by_it(self):
        handed = self.names("fx::more_implicit", ("handed",))
        self.assertTrue(any("lambda" in n for n in handed), handed)
        self.assertTrue(any(n.endswith("helper") for n in self.reached("fx::more_implicit")))


if __name__ == "__main__":
    unittest.main()
