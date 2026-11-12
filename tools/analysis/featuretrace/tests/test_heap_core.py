"""The heap analysis's core (log/trace-core-plan.md, Task 1): abstract objects by allocation site, fields per object,
calls resolved by the callee's objects, aliases through the heap (fixture `heap/core.ts`)."""
from __future__ import annotations

import unittest

from .support import ENGINE, HeapCase


class HeapCore(HeapCase):
    ROOT = "core.ts"

    def calls(self, spec: str) -> set:
        return self.names(spec, ("call", "new"))

    def test_a_method_through_a_local_and_a_field(self):
        self.assertIn("Box.open", self.calls("viaLocal"))
        self.assertIn("Holder.use", self.calls("viaField"))
        self.assertIn("Box.open", self.calls("Holder.use"))

    def test_two_new_are_two_objects(self):
        unit = self.gid("twoSites")
        self.assertNotEqual(self.result.points(unit, ("name", "a")), self.result.points(unit, ("name", "b")))

    def test_a_function_passed_and_called(self):
        self.assertTrue(any(c.endswith("seven") for c in self.calls("run")), self.calls("run"))
        self.assertEqual({"prim:number"}, self.returned("passed"))

    def test_a_field_written_through_an_alias(self):
        found = self.returned("alias")
        self.assertTrue(any(v.startswith("new:") and "Tile" in v for v in found), found)

    def test_an_element_written_through_an_alias(self):
        found = self.returned("arrayAlias")
        self.assertTrue(any(v.startswith("new:") and "Tile" in v for v in found), found)

    def test_a_computed_key_written_reaches_a_read_by_a_key(self):
        found = self.returned("computedAlias")
        self.assertTrue(any(v.startswith("new:") and "Tile" in v for v in found), found)

    def test_a_number_key_names_an_index_never_a_named_property(self):
        self.assertEqual({"Box"}, self.classes("indexKeepsNamesApart"))   # ECMA-262 ToPropertyKey
        self.assertEqual({"Tile"}, self.classes("indexReadBack"))         # "0" is a canonical numeric index
        # a key no text names writes the dictionary part, which a name does not read (log/trace-research.md F40)
        self.assertEqual({"Box"}, self.classes("unknownKeyKeepsNamesApart"))

    def test_each_instance_initializes_its_own_fields(self):
        self.assertEqual({"Box"}, self.classes("bagBoxes"))      # ECMA-262 InitializeInstanceElements
        self.assertEqual({"Tile"}, self.classes("bagTiles"))
        self.assertEqual({"Owned"}, self.classes("ownerOf"))     # `this` in an initializer: the instance
        self.assertEqual({"Owned"}, self.classes("holderOf"))    # a literal's property: the `this` around it

    def test_a_literal_property_is_evaluated_where_the_literal_is(self):
        self.assertEqual({"Box"}, self.classes("wrapped"))       # PropertyDefinitionEvaluation

    def test_a_module_initializes_its_variables_and_a_nested_literal_links_its_calls(self):
        self.assertIn(("call", "core.ts.tone"), self.links("warm"))

    def test_an_uncalled_method_runs_on_an_object_of_its_class(self):
        self.assertIn(("call", "Crate.lid"), self.links("Crate.open"))
        self.assertEqual({"Tile"}, self.classes("Crate.open"))

    def test_a_value_of_a_program_class_from_outside_is_an_instance_the_program_made(self):
        self.assertEqual({"Tile"}, self.classes("stocked"))

    def test_what_an_entry_point_returns_is_made_in_its_run(self):
        self.assertEqual({"Tile"}, self.classes("peek"))

    def test_the_instance_from_outside_is_one_object_for_its_class(self):
        self.assertIn(("call", "Plug.fit"), self.links("Socket.use"))

    def test_new_on_the_unknown_object_is_an_open_call(self):
        self.assertEqual({"unknown:"}, self.returned("forged"))
        self.assertTrue(any(kind == "handed" for kind, _ in self.links("forged")), self.links("forged"))

    def test_a_typed_position_takes_only_what_is_assignable_to_it(self):
        self.assertEqual({"Pot"}, self.classes("kept"))       # a parameter
        self.assertEqual({"Pot"}, self.classes("handed"))     # a return
        self.assertTrue(any(v.startswith("lit:") for v in self.returned("lidded")), self.returned("lidded"))

    def test_an_object_literal_property(self):
        found = self.returned("literalRead")
        self.assertTrue(any(v.startswith("new:") and "Box" in v for v in found), found)


class HeapCoreEngine(HeapCore):
    CORE = ENGINE


if __name__ == "__main__":
    unittest.main()
