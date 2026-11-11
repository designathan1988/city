"""Declared types bound their places (log/trace-rules.md, S2, S2'): a value the declared type does not admit stops at
the place (fixtures `jsvalues/bounded.ts`, `jsvalues/jsdoc.js`)."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))
sys.path.insert(0, str(HERE))

from support import FixtureCase, Solved  # noqa: E402
from featuretrace.solver import CLASS, ELEM, NULL, NUMBER, TEXT, UNDEFINED  # noqa: E402

BOX, BIG, TILE = CLASS + "bounded.ts#BoundBox", CLASS + "bounded.ts#BoundBig", CLASS + "bounded.ts#BoundTile"
LITERAL = CLASS + "bounded.ts#<object@"


class Bounds(Solved, FixtureCase):
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"

    def literals(self, found: set) -> list:
        return [v for v in found if v.startswith(LITERAL)]

    def test_primitive_bound(self):
        self.assertEqual({NUMBER}, self.returned("bNum"))

    def test_class_bound_by_chain_and_by_members(self):
        found = self.returned("bBox")
        self.assertTrue({BOX, BIG} <= found, found)
        self.assertNotIn(TILE, found)
        self.assertEqual(1, len(self.literals(found)), found)       # `{ open() {...} }` has Box's members

    def test_object_type_bound(self):
        found = self.returned("bShape")
        self.assertEqual(1, len(self.literals(found)), found)       # `{ w: 1 }`; `{ h: 2 }` lacks w
        self.assertFalse(found & {NUMBER, TEXT + "text"}, found)
        self.assertIn(TEXT + "text", self.returned("bLength"))      # a string has a length (its apparent type)
        self.assertNotIn(NUMBER, self.returned("bLength"))

    def test_optional_parameter(self):
        self.assertEqual({NUMBER, UNDEFINED}, self.returned("bOpt"))

    def test_field_bound(self):
        self.assertEqual({NUMBER}, self.returned("BoundHolder.get"))

    def test_result_bound(self):
        self.assertEqual({NUMBER}, self.returned("bRet"))
        self.assertNotIn(TEXT + "s", self.returned("bAsync"))       # bounded by its Promise's argument
        self.assertIn(NUMBER, self.returned("bAsync"))

    def test_record_and_union(self):
        found = self.returned("bRec")
        self.assertEqual(1, len(self.literals(found)), found)
        self.assertNotIn(TEXT + "s", found)
        self.assertEqual({BOX, NULL}, self.returned("bUnion"))

    def test_any_and_unknown_admit_everything(self):
        found = self.returned("bAny")
        self.assertTrue({ELEM + TEXT + "s", ELEM + TILE} <= found, found)


if __name__ == "__main__":
    unittest.main()
