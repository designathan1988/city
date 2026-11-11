"""Values through branch conditions (log/trace-rules.md, "Narrowing at branch conditions": V1, V2, G1, G2, R1, R2,
N1): the language's constants, properties written outside their class, and what a name holds inside the regions a
condition proves something in."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))
sys.path.insert(0, str(HERE))

from support import FixtureCase  # noqa: E402
from featuretrace.solver import CLASS, ENGINE, NULL, TRUE, UNDEFINED  # noqa: E402

BOX, TILE, V3 = CLASS + "types.ts#Box", CLASS + "helpers.ts#Tile", CLASS + "guards.js#V3"


class Narrowing(FixtureCase):
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"

    def held(self, spec: str, name: str) -> set:
        return {text for text, _ in self.rs.typer.evaluate(("name", name), self.ix.units[self.gid(spec)])}

    def returned(self, spec: str) -> set:
        typer = self.rs.typer
        return {text for text, _ in typer.current(typer.ret_node(self.ix.units[self.gid(spec)]))}

    def test_literal_constants(self):
        self.assertEqual({TRUE}, self.returned("yes"))
        self.assertEqual({NULL}, self.returned("none"))
        self.assertEqual({UNDEFINED}, self.returned("gone"))

    def test_constants_are_the_languages_values(self):
        self.assertEqual({ENGINE}, self.returned("lengthOfYes"))

    def test_prototype_property(self):
        self.assertEqual({TRUE}, self.returned("flagOf"))

    def test_property_written_outside_its_class(self):
        self.assertIn("Box.open", self.names("useBag"))

    def test_property_found_nowhere_is_undefined(self):
        self.assertEqual({UNDEFINED}, self.returned("missing"))

    def test_member_truthiness_overload(self):
        self.assertNotIn(V3, self.held("V3.set", "x"))           # `x.isV3` false: never a V3
        self.assertIn(V3, self.held("V3.copy", "v"))

    def test_instanceof_branches(self):
        self.assertTrue({"Box.open", "Tile.lay"} <= self.names("pickBranch"))
        self.assertEqual({TILE}, self.returned("branchValue"))   # the `else` of `c instanceof Box`

    def test_assignment_in_region_keeps_the_name_whole(self):
        self.assertIn("Tile.lay", self.names("kept"))

    def test_closure_write_keeps_the_name_whole(self):
        self.assertIn("Tile.lay", self.names("laterWrite"))

    def test_nested_regions_compose(self):
        self.assertEqual({BOX}, self.returned("nested"))

    def test_narrowed_key_parameter(self):
        self.assertEqual({NULL}, self.returned("useKeyed"))      # the Tile's template: no Box, no Tile


if __name__ == "__main__":
    unittest.main()
