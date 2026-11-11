"""Members of the language's values (log/trace-rules.md, T5): a JS primitive's property is its wrapper prototype's
(MDN Object coercion), an index of a string a character or `undefined`; a GDScript or C++ string is the engine's or
the language's value, never a type read from the text a literal carries."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))
sys.path.insert(0, str(HERE))

from support import FixtureCase, Solved  # noqa: E402
from featuretrace.solver import (BIGINT, CLASS, ENGINE, FALSE, FUNCTION, NULL, NUMBER, OBJECT, STRING,  # noqa: E402
                                 TRUE, UNDEFINED)

STRINGS = f"Array<{STRING}>"


class JsPrimitives(Solved, FixtureCase):
    """Fixture `jsvalues/primitives.js`."""
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"

    def test_string_members_by_kind(self):
        self.assertEqual({NUMBER}, self.returned("primLength"))
        self.assertEqual({STRING}, self.returned("primUpper"))
        self.assertEqual({TRUE, FALSE}, self.returned("primHas"))
        self.assertEqual({STRINGS}, self.returned("primSplit"))
        self.assertEqual({STRINGS, NULL}, self.returned("primMatch"))

    def test_string_index_and_iteration(self):
        self.assertEqual({STRING, UNDEFINED}, self.returned("primIndex"))    # bracket notation past the end: undefined
        self.assertEqual({STRING, UNDEFINED}, self.returned("primEach"))

    def test_string_method_read_and_missing_member(self):
        self.assertEqual({FUNCTION}, self.returned("primMethod"))
        self.assertEqual({UNDEFINED}, self.returned("primMissing"))

    def test_number_boolean_and_bigint_members(self):
        self.assertEqual({STRING}, self.returned("primFixed"))       # a BigInt has no `toFixed`: it throws
        self.assertEqual({NUMBER, BIGINT}, self.returned("primValueOf"))
        self.assertEqual({STRING}, self.returned("primBoolText"))
        self.assertEqual({NUMBER, UNDEFINED}, self.returned("primSumLength"))   # a string's length; a number has none

    def test_member_of_null_is_no_value(self):
        self.assertEqual(set(), self.returned("primNullish"))       # a TypeError

    def test_no_value_is_read_from_a_literals_text(self):
        for spec in ("primUpper", "primIndex", "primEach", "primMethod", "primMissing"):
            found = self.returned(spec)
            self.assertFalse(any("common" in v or v == "text" for v in found), (spec, found))


class LanguagePredicates(Solved, FixtureCase):
    """G1'': the predicates ECMAScript itself defines narrow (fixture `jsvalues/builtins.ts`)."""
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"
    GADGET = CLASS + "builtins.ts#Gadget"

    def test_array_buffer_is_view(self):
        self.assertEqual({ENGINE, NULL}, self.returned("viewOnly"))         # the typed array; no Gadget, no array
        self.assertNotIn("Gadget.constructor", self.names("viewMakes"))    # a view's constructor is the host's

    def test_number_predicates(self):
        self.assertEqual({NUMBER, NULL}, self.returned("intOnly"))

    def test_an_object_of_the_language_can_be_an_array(self):
        self.assertIn(OBJECT, self.returned("anyObjectArray"))


class OtherStrings(Solved, FixtureCase):
    """Fixtures `pending/strings.gd`, `pending/strings.cpp`."""
    ROOTS = ("pending",)
    BASE = "pending"

    def test_gdscript_string_is_the_engines(self):
        for spec in ("gd_text_method", "gd_text_index", "gd_text_plus"):
            self.assertEqual({ENGINE}, self.returned(spec), spec)

    def test_cpp_literal_element_is_no_type_text(self):
        for spec in ("cpp_text_index", "cpp_text_element"):
            found = self.returned(spec) - {"char"}                  # `cpp_text_index` declares its char
            self.assertEqual({ENGINE}, found, spec)


if __name__ == "__main__":
    unittest.main()
