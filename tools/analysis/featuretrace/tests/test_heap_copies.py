"""Copies of an object's properties by name in the new core (log/trace-research.md F34): an object spread copies each
own property of the spread object into the property of the same name (ECMA-262 CopyDataProperties), through chains and
for properties written after the object was made; a declared intersection result (`T & U`, an assign's declaration)
gives its object the other part's properties by name, never into every property; a write by a key enumerating an
object (`for...in`) makes the properties that object has, by name (fixture `heap/copies.ts`)."""
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

    def test_a_write_by_an_enumerated_key_makes_the_property_of_that_name(self):
        # `for (k in src) dst[k] = v`: the properties src has, by name (written before or after), and no other
        self.assertEqual({"Apple"}, self.classes("enumerated"))
        self.assertEqual(set(), self.classes("enumeratedOther"))
        self.assertEqual({"Pear"}, self.classes("enumeratedLater"))

    def test_a_value_read_with_the_write_s_key_is_that_name_s_value(self):
        self.assertEqual({"Apple"}, self.classes("cloneByKey"))
        self.assertEqual({"Pear"}, self.classes("cloneThroughLocal"))       # through a local the key's read sets

    def test_a_write_by_an_unknown_key_is_read_by_a_key_not_by_a_name(self):
        self.assertEqual(set(), self.classes("byUnknownKey"))
        self.assertEqual({"Apple"}, self.classes("byUnknownKeyRead"))

    def test_a_name_reads_a_declared_string_index_signature(self):
        self.assertTrue(self.declared("indexedByName", "Pear"), self.returned("indexedByName"))


class HeapCopiesEngine(HeapCopies):
    CORE = ENGINE


if __name__ == "__main__":
    unittest.main()
