"""A string is a value of its own text in the new core (log/trace-research.md F33; ECMA-262 ToPropertyKey: a key
computed from a string names the property of that text, a canonical numeric string an index): a key read from an array
of literals, passed as an argument or declared as a union of literal types names its member alone, on both cores
(fixture `heap/texts.ts`)."""
from __future__ import annotations

import unittest

from .support import ENGINE, HeapCase


class HeapTexts(HeapCase):
    ROOT = "texts.ts"

    def test_a_key_from_an_array_of_literals_names_those_members(self):
        self.assertEqual({"Apple", "Pear"}, self.classes("viaKeys"))

    def test_a_key_passed_as_an_argument_names_its_member(self):
        self.assertEqual({"Apple"}, self.classes("viaArgument"))

    def test_a_write_through_a_key_passed_as_an_argument_writes_that_member(self):
        self.assertEqual(set(), self.classes("written"))
        self.assertEqual({"Apple"}, self.classes("writtenNamed"))

    def test_a_key_of_a_declared_union_of_literals_names_those_members(self):
        self.assertEqual({"Apple", "Pear"}, self.classes("declaredKey"))

    def test_a_canonical_numeric_string_names_an_index(self):
        self.assertEqual({"Apple"}, self.classes("numbered"))


class HeapTextsEngine(HeapTexts):
    CORE = ENGINE


if __name__ == "__main__":
    unittest.main()
