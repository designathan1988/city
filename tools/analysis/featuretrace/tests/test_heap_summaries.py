"""What no declaration states, summarized from the declarations in the new core (log/trace-core-plan.md, Task 5;
log/trace-research.md, Decision): the values a declared call takes in positions whose types say nothing of where they
go reach the member a string it passes names of its open object arguments and its open result, the functions among
them are called back, a member a string names is read and called (reflection by name); no copy the declaration does not
state; a precisely typed function keeps its precise summary; an intersection result gives its object the other part's
members (fixture `heap/summaries`)."""
from __future__ import annotations

import unittest

from .support import ENGINE, HeapCase


class HeapSummaries(HeapCase):
    ROOT = "summaries"

    def test_a_descriptor_value_reaches_the_field_its_key_names(self):
        self.assertEqual({"Apple"}, self.classes("defined"))

    def test_a_callback_in_an_argument_field_is_called(self):
        self.assertTrue(any(kind == "handed" and dst.endswith(".get") for kind, dst in self.links("gotten")),
                        self.links("gotten"))
        self.assertEqual({"Apple"}, self.classes("gotten"))

    def test_an_undeclared_copy_is_not_assumed(self):
        # `merge(target: object, source: object): void` declares no copy; spreading every value of a call into every
        # open object's fields was Averroes's pool, which made every set large (log/trace-research.md F10, Decision)
        self.assertEqual(set(), self.classes("merged"))

    def test_an_open_result_holds_what_the_call_took(self):
        self.assertEqual({"Apple"}, self.classes("passed"))

    def test_a_precisely_typed_function_keeps_its_precise_summary(self):
        self.assertEqual({"Apple"}, self.classes("kept"))

    def test_an_intersection_result_gives_the_target_the_source_members(self):
        self.assertEqual({"Apple"}, self.classes("joined"))

    def test_a_text_that_is_no_property_key_names_no_member(self):
        self.assertFalse(any(dst.endswith(".run") for kind, dst in self.links("noted")), self.links("noted"))

    def test_a_member_a_string_names_is_called(self):
        self.assertTrue(any(kind == "call" and dst.endswith(".run") for kind, dst in self.links("invoked")),
                        self.links("invoked"))
        self.assertEqual({"Apple"}, self.classes("invoked"))


class HeapSummariesEngine(HeapSummaries):
    CORE = ENGINE


if __name__ == "__main__":
    unittest.main()
