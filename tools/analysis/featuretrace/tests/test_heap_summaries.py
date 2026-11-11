"""What no declaration states, summarized from the declarations in the new core (log/trace-core-plan.md, Task 5): the
values a declared call takes in positions whose types say nothing of where they go reach the fields of its open object
arguments (the one a string it passed names) and its open result, the functions among them are called back, a member a
string names is read and called (reflection by name); a precisely typed function keeps its precise summary; an
intersection result gives its object the other part's members (fixture `heap/summaries`)."""
from __future__ import annotations

import unittest

from .support import HeapCase


class HeapSummaries(HeapCase):
    ROOT = "summaries"

    def test_a_descriptor_value_reaches_the_field_its_key_names(self):
        self.assertEqual({"Apple"}, self.classes("defined"))

    def test_a_callback_in_an_argument_field_is_called(self):
        self.assertTrue(any(kind == "handed" and dst.endswith(".get") for kind, dst in self.links("gotten")),
                        self.links("gotten"))
        self.assertEqual({"Apple"}, self.classes("gotten"))

    def test_a_source_argument_fields_reach_the_target(self):
        self.assertEqual({"Apple"}, self.classes("merged"))

    def test_an_open_result_holds_what_the_call_took(self):
        self.assertEqual({"Apple"}, self.classes("passed"))

    def test_a_precisely_typed_function_keeps_its_precise_summary(self):
        self.assertEqual({"Apple"}, self.classes("kept"))

    def test_an_intersection_result_gives_the_target_the_source_members(self):
        self.assertEqual({"Apple"}, self.classes("joined"))

    def test_a_member_a_string_names_is_called(self):
        self.assertTrue(any(kind == "call" and dst.endswith(".run") for kind, dst in self.links("invoked")),
                        self.links("invoked"))
        self.assertEqual({"Apple"}, self.classes("invoked"))


if __name__ == "__main__":
    unittest.main()
