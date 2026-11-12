"""Narrowing in the new core (log/trace-research.md F31): a name read where a test proved something of its value keeps
only the values the test can be so of — ECMA-262 `typeof` (13.5.3.1), ToBoolean (7.1.2), IsStrictlyEqual, `== null`,
`instanceof` (13.10.2, OrdinaryHasInstance 7.3.21, a class's own @@hasInstance), `in` (13.10.1: a primitive throws),
a property's truthiness and equality, the operands of `||`, a declared type predicate and assertion — the program's
or the platform's (`isArray(arg: any): arg is any[]` as the fixture declares it): no function known by its name
(fixture `heap/narrowing.ts`)."""
from __future__ import annotations

import unittest

from .support import ENGINE, HeapCase


class HeapNarrowing(HeapCase):
    ROOT = "narrowing.ts"

    def kinds(self, spec: str) -> set:
        """What a function returns, each value by its kind (`new:` by its class, a primitive or a text whole)."""
        out = set()
        for v in self.returned(spec):
            kind = v.split(":", 1)[0]
            out.add(v.split(":")[1].rsplit("#", 1)[-1] if kind == "new" else v if kind in ("prim", "str") else kind)
        return out

    def test_typeof_object_keeps_the_objects_alone(self):
        self.assertEqual({"Apple"}, self.kinds("typeofObject"))

    def test_typeof_function_keeps_the_functions_alone(self):
        self.assertEqual({"fn"}, self.kinds("typeofFunction"))

    def test_typeof_string_keeps_the_strings_alone(self):
        self.assertEqual({"str:a"}, self.kinds("typeofString"))

    def test_truthiness_both_ways(self):
        self.assertEqual({"prim:null"}, self.kinds("falsy"))
        self.assertEqual({"Apple"}, self.kinds("truthy"))

    def test_nullish_both_ways(self):
        self.assertEqual({"prim:null"}, self.kinds("nullish"))
        self.assertEqual({"Apple"}, self.kinds("notNullish"))

    def test_strict_equality_with_a_constant(self):
        self.assertEqual({"prim:null"}, self.kinds("strictNull"))
        self.assertEqual({"str:a"}, self.kinds("sameText"))

    def test_instanceof_by_the_prototype_chain(self):
        self.assertEqual({"Apple", "Seed"}, self.kinds("instance"))
        self.assertEqual({"Pear"}, self.kinds("notInstance"))

    def test_a_class_with_its_own_has_instance_decides_alone(self):
        self.assertEqual({"prim:number", "Pear"}, self.kinds("ownInstanceTest"))

    def test_a_platform_predicate_by_its_declaration_both_ways(self):
        self.assertEqual({"arr"}, self.kinds("isArray"))
        self.assertEqual({"Pear"}, self.kinds("notArray"))

    def test_in_throws_on_a_primitive(self):
        self.assertEqual({"Apple"}, self.kinds("has"))
        self.assertEqual({"Apple"}, self.kinds("lacks"))

    def test_a_property_equal_to_a_constant_tells_the_objects_apart(self):
        self.assertEqual({"Apple"}, self.kinds("tagged"))
        self.assertEqual({"Pear"}, self.kinds("otherTag"))

    def test_a_property_truthy_tells_the_objects_apart(self):
        self.assertEqual({"Apple"}, self.kinds("ready"))

    def test_the_left_operand_of_or_is_the_value_when_truthy(self):
        self.assertEqual({"Apple", "Pear"}, self.kinds("either"))

    def test_a_declared_type_predicate(self):
        self.assertEqual({"Apple"}, self.kinds("predicated"))

    def test_a_declared_assertion(self):
        self.assertEqual({"Apple"}, self.kinds("asserted"))


class HeapNarrowingEngine(HeapNarrowing):
    CORE = ENGINE


if __name__ == "__main__":
    unittest.main()
