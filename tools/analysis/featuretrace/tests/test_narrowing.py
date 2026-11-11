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
from featuretrace import narrow_values  # noqa: E402
from featuretrace.solver import CLASS, ELEM, ENGINE, NULL, TEXT, TRUE, UNDEFINED  # noqa: E402

BOX, TILE, V3 = CLASS + "types.ts#Box", CLASS + "helpers.ts#Tile", CLASS + "guards.js#V3"
CAN, SHAPE = CLASS + "guards.js#Can", CLASS + "guards.js#<object@"


class Solved:
    """Values read once the solver reached its fixpoint."""

    def solved(self, node) -> set:
        """A node's values once the solver reached its fixpoint again (a node first wired after the solve, such as a
        templated function's own result, is run to its own: pending differences, then the guards' `undefined`)."""
        typer = self.rs.typer
        while True:
            typer.propagate()
            if not narrow_values.settle(typer):
                return {text for text, _ in typer.current(node)}

    def held(self, spec: str, name: str) -> set:
        return self.solved(self.rs.typer.node_of(("name", name), self.ix.units[self.gid(spec)]))

    def returned(self, spec: str) -> set:
        return self.solved(self.rs.typer.ret_node(self.ix.units[self.gid(spec)]))


class Narrowing(Solved, FixtureCase):
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"

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

    def test_early_return(self):
        self.assertEqual({BOX, UNDEFINED}, self.returned("early"))

    def test_ternary_and_logical(self):
        self.assertEqual({BOX, NULL}, self.returned("ternary"))
        self.assertEqual({ENGINE, TILE}, self.returned("andRight"))
        self.assertEqual({ENGINE, TILE}, self.returned("orRight"))

    def test_typeof_and_equality(self):
        self.assertEqual({TEXT + "s", NULL}, self.returned("textOnly"))
        self.assertEqual({BOX, UNDEFINED}, self.returned("notNull"))
        self.assertEqual({BOX, UNDEFINED}, self.returned("looseNull"))

    def test_in_and_isarray(self):
        self.assertEqual({BOX, NULL}, self.returned("hasOpen"))
        found = self.returned("arrayOnly")
        self.assertNotIn(BOX, found)
        self.assertIn(ELEM + ENGINE, found)

    def test_discriminant(self):
        for spec in ("discriminant", "bySwitch"):
            shapes = [v for v in self.returned(spec) if v.startswith(SHAPE)]
            self.assertEqual(1, len(shapes), spec)                 # the box's shape, not the can's

    def test_type_predicate(self):
        for spec in ("viaPredicate", "viaAssertion"):
            found = self.returned(spec)
            self.assertIn(BOX, found, spec)
            self.assertNotIn(CAN, found, spec)

    def test_while_body(self):
        self.assertEqual({BOX, NULL}, self.returned("looped"))

    def test_negated_conjunction_proves_nothing_when_true(self):
        self.assertEqual({BOX, TILE, NULL}, self.returned("notBoth"))

    def test_optional_chain_member_guard(self):
        self.assertEqual({V3, NULL}, self.returned("optional"))
        self.assertEqual({TILE, NULL}, self.returned("optionalElse"))   # Tile has no `isV3`: undefined

    def test_unknown_comparison_proves_nothing(self):
        self.assertEqual({BOX, TILE, NULL}, self.returned("unknownTest"))

    def test_unknown_value_is_the_tested_class(self):
        self.assertEqual({BOX, NULL}, self.returned("hostValue"))   # the host's value that is a Box is a Box


class GdNarrowing(Solved, FixtureCase):
    ROOTS = ("pending",)
    BASE = "pending"

    @staticmethod
    def has(values: set, cls: str) -> bool:
        return any(v.endswith("guards.gd." + cls) for v in values)

    def test_gd_is_branches(self):
        self.assertTrue({"Crate.open", "Plank.lay"} <= {n.split("guards.gd.")[-1] for n in self.names("gd_branches")})
        values = self.returned("gd_else_value")
        self.assertTrue(self.has(values, "Plank"), values)
        self.assertFalse(self.has(values, "Crate"), values)

    def test_gd_early_return(self):
        values = self.returned("gd_early_value")
        self.assertTrue(self.has(values, "Crate"), values)
        self.assertFalse(self.has(values, "Plank"), values)

    def test_gd_unknown_value_is_the_tested_class(self):
        self.assertTrue(self.has(self.returned("gd_typed"), "Crate"))


if __name__ == "__main__":
    unittest.main()
