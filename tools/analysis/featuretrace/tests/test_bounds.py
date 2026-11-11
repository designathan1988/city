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
from featuretrace.solver import CLASS, ELEM, FN, NULL, NUMBER, TEXT, UNDEFINED  # noqa: E402

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

    def test_function_admits_a_class(self):
        found = self.returned("bFunction")                          # a class is a function (S2'')
        self.assertIn(BOX, found)
        self.assertTrue(any(v.startswith(FN) for v in found), found)
        self.assertNotIn(NUMBER, found)

    def test_members_every_value_has(self):
        self.assertIn(TILE, self.returned("bPrintable"))            # Object.prototype's `toString`
        found = self.returned("bTagged")                            # a function's property code writes
        self.assertTrue(any(v.startswith(FN) for v in found), found)

    def test_iterable_and_array_like_by_their_members(self):
        found = self.returned("bIterable")
        self.assertTrue(any(v.startswith("Set<") for v in found), found)
        self.assertIn(TEXT + "abc", found)
        self.assertTrue(any(v.startswith(("Array<", ELEM)) for v in found), found)
        self.assertNotIn(NUMBER, found)
        found = self.returned("bArrayLike")
        self.assertIn(TEXT + "abc", found)
        self.assertEqual(1, len(self.literals(found)), found)      # `{ length: 0 }`
        self.assertNotIn(NUMBER, found)
        found = self.returned("bThenable")                          # a thenable is a PromiseLike
        self.assertEqual(1, len(self.literals(found)), found)
        self.assertNotIn(NUMBER, found)


class JsDoc(Solved, FixtureCase):
    """S2': JSDoc in a JavaScript file (fixture `jsvalues/jsdoc.js`)."""
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"
    VEC = CLASS + "jsdoc.js#DocVec"

    def test_param_and_type_on_this(self):
        self.assertEqual({NUMBER}, self.returned("docX"))           # `@param {number} [x=0]`, `@type {number}`

    def test_closure_forms(self):
        self.assertEqual({NULL, NUMBER}, self.returned("docNullable"))     # `{?number}`
        self.assertEqual({UNDEFINED}, self.returned("docOptional"))        # `{number=}`
        self.assertEqual({TEXT + "s"}, self.returned("docAny"))             # `{*}`
        self.assertEqual({self.VEC}, self.returned("docBang"))              # `{!DocVec}`
        found = self.returned("docArray")                                   # `{Array.<number>}`
        self.assertNotIn(TEXT + "s", found)
        self.assertTrue(found, found)
        fns = self.returned("docFn")                                        # `{function(number): string}`
        self.assertTrue(fns and all(v.startswith("<fn>") for v in fns), fns)

    def test_returns_and_variable(self):
        self.assertEqual({self.VEC}, self.returned("docMake"))             # `@returns {DocVec}`
        self.assertEqual({NUMBER}, self.returned("docLetRead"))            # `@type {number}` on `let`

    def test_comments_are_the_trees(self):
        self.assertEqual({NUMBER}, self.returned("docAfterRead"))          # `/**` in a template is no comment


if __name__ == "__main__":
    unittest.main()
