"""Contexts and heap cloning in the new core (log/trace-core-plan.md, Task 3): closures valued by the call that made
them, objects by the call that made them, methods by their receiver, recursion through closures finite, a closure's
write the shared variable's, a callback no call reaches run in the environment it was made in, a prototype object per
function object (fixture `heap/contexts.js`)."""
from __future__ import annotations

import unittest

from .support import ENGINE, HeapCase


class HeapContexts(HeapCase):
    ROOT = "contexts.js"

    def test_each_memo_runs_its_own_function(self):
        self.assertEqual({"Apple"}, self.classes("memoApple"))

    def test_each_factory_call_makes_its_own_objects(self):
        self.assertEqual({"Apple"}, self.classes("factoryApple"))

    def test_a_method_keeps_each_receiver_apart(self):
        self.assertEqual({"Apple"}, self.classes("receiverApple"))

    def test_recursion_through_closures_ends(self):
        self.assertEqual({"Apple"}, self.classes("spun"))

    def test_a_closure_write_is_the_shared_variable(self):
        self.assertEqual({"Apple"}, self.classes("sharedApple"))

    def test_a_callback_no_call_reaches_reads_the_run_that_made_it(self):
        self.assertEqual({"Apple"}, self.classes("onEvent"))

    def test_each_function_object_has_its_own_prototype(self):
        self.assertEqual({"Apple"}, self.classes("kindApple"))

    def test_an_object_literal_method_is_a_closure_of_the_run_that_made_it(self):
        self.assertEqual({"Apple"}, self.classes("literalApple"))

    def test_a_class_method_is_a_closure_of_the_run_that_made_the_class(self):
        self.assertEqual({"Apple"}, self.classes("localApple"))

    def test_a_class_value_method_is_a_closure_of_the_run_that_made_it(self):
        self.assertEqual({"Apple"}, self.classes("classValueApple"))


    def test_a_run_carrying_no_object_to_its_outputs_runs_once(self):
        # each is also run once as an entry point (entries are chosen before `sized` and `both` run)
        self.assertEqual(2, self.runs("area"))                 # a number made from its arguments: one run for both
        self.assertEqual(3, self.runs("same"))                 # an identity: each call its own run
        self.assertEqual(3, self.runs("keep"))                 # a store into a parameter's object: likewise
        self.assertEqual({"Apple"}, self.classes("both"))


class HeapContextsEngine(HeapContexts):
    CORE = ENGINE


if __name__ == "__main__":
    unittest.main()
