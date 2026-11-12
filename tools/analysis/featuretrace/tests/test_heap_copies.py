"""Copies of an object's properties by name in the new core (log/trace-research.md F34): an object spread copies each
own property of the spread object into the property of the same name (ECMA-262 CopyDataProperties), through chains and
for properties written after the object was made; a declared intersection result (`T & U`, an assign's declaration)
gives its object the other part's properties by name, never into every property (fixture `heap/copies.ts`)."""
from __future__ import annotations

import unittest

from .support import ENGINE, HeapCase


class HeapCopies(HeapCase):
    ROOT = "copies.ts"

    def test_a_spread_copies_each_property_by_its_name(self):
        self.assertEqual({"Apple"}, self.classes("spread"))

    def test_a_property_written_beside_a_spread_is_its_own(self):
        self.assertEqual({"Pear"}, self.classes("spreadThenOwn"))

    def test_a_spread_of_a_spread(self):
        self.assertEqual({"Apple"}, self.classes("chained"))

    def test_a_spread_copies_a_property_written_on_the_object(self):
        self.assertEqual({"Apple"}, self.classes("spreadWritten"))

    def test_an_intersection_gives_the_other_part_its_properties_by_name(self):
        self.assertEqual({"Apple"}, self.classes("assigned"))
        self.assertEqual({"Pear"}, self.classes("assignedOther"))


class HeapCopiesEngine(HeapCopies):
    CORE = ENGINE


if __name__ == "__main__":
    unittest.main()
