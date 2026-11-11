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

from support import FixtureCase, Solved  # noqa: E402
from featuretrace.solver import (BIGINT, CLASS, ELEM, FALSE, NULL, NUMBER, OBJECT, STRING, SYMBOL,  # noqa: E402
                                 TEXT, TRUE, UNDEFINED)

BOX, TILE, V3 = CLASS + "types.ts#Box", CLASS + "helpers.ts#Tile", CLASS + "guards.js#V3"
CAN, SHAPE = CLASS + "guards.js#Can", CLASS + "guards.js#<object@"


class Narrowing(Solved, FixtureCase):
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"

    def test_literal_constants(self):
        self.assertEqual({TRUE}, self.returned("yes"))
        self.assertEqual({NULL}, self.returned("none"))
        self.assertEqual({UNDEFINED}, self.returned("gone"))

    def test_constants_are_the_languages_values(self):
        self.assertEqual({UNDEFINED}, self.returned("lengthOfYes"))     # `true.length`: Boolean has none (T5)

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
        self.assertEqual({FALSE, TILE}, self.returned("andRight"))     # the test's value where it is falsy (MDN `&&`)
        self.assertEqual({TRUE, TILE}, self.returned("orRight"))       # where it is truthy (MDN `||`)

    def test_typeof_and_equality(self):
        self.assertEqual({TEXT + "s", NULL}, self.returned("textOnly"))
        self.assertEqual({BOX, UNDEFINED}, self.returned("notNull"))
        self.assertEqual({BOX, UNDEFINED}, self.returned("looseNull"))

    def test_in_and_isarray(self):
        self.assertEqual({BOX, NULL}, self.returned("hasOpen"))
        found = self.returned("arrayOnly")
        self.assertNotIn(BOX, found)
        self.assertIn(ELEM + NUMBER, found)                       # `[1]`: an array of numbers (T1'')

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


class Declared(Solved, FixtureCase):
    """T1', T1'', G8' and the argument a call leaves out (fixture `jsvalues/declared.ts`)."""
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"
    RUNNER, BAG = CLASS + "declared.ts#Runner", CLASS + "declared.ts#Bag"
    PACK, SCOPED = CLASS + "declared.ts#Pack", CLASS + "declared.ts#Scoped"

    def test_interface_object_keeps_what_its_types_admit(self):
        self.assertTrue({self.RUNNER, NULL} <= self.returned("declLive"))       # `done: boolean` can be false
        self.assertTrue({self.RUNNER, NULL} <= self.returned("declWalking"))    # `mode: 'walk' | 'run'`

    def test_optional_property(self):
        self.assertEqual({STRING, UNDEFINED}, self.returned("declNote"))

    def test_primitive_and_literal_types(self):
        self.assertEqual({NUMBER, STRING, TRUE, FALSE, NULL, UNDEFINED, SYMBOL, BIGINT, OBJECT},
                         self.returned("declKinds"))
        self.assertEqual(set(), self.returned("declNever"))
        self.assertEqual({self.PACK, UNDEFINED}, self.returned("declOptional"))

    def test_alias_lookup_and_conditional_types(self):
        found = self.returned("declLookup")                                     # `Runner['act']` through an alias
        self.assertIn(NULL, found)
        self.assertTrue(any(v.startswith(CLASS + "declared.ts#<type@") for v in found), found)
        self.assertEqual({self.PACK, NULL}, self.returned("declConditional"))

    def test_typeof_on_a_declared_number(self):
        self.assertEqual({NUMBER, NULL}, self.returned("declTypeof"))

    def test_literals_and_operators_by_kind(self):
        self.assertEqual({NUMBER}, self.returned("declNumber"))
        self.assertEqual({STRING}, self.returned("declTemplate"))
        self.assertEqual({TRUE, FALSE}, self.returned("declNot"))
        self.assertEqual({STRING}, self.returned("declTypeofText"))
        self.assertEqual({NUMBER, BIGINT}, self.returned("declMinus"))

    def test_getter_and_setter_values(self):
        self.assertEqual({self.SCOPED, NULL}, self.returned("declGetter"))      # the getter's `!==` can be true
        self.assertEqual({UNDEFINED}, self.returned("declSetterOnly"))

    def test_index_signature(self):
        self.assertTrue({self.BAG, NULL} <= self.returned("declIndexed"))
        self.assertEqual({self.PACK, UNDEFINED}, self.returned("declIndexValue"))

    def test_member_call_reaches_a_held_function_beside_a_method(self):
        self.assertTrue(any(n.endswith("flip") for n in self.names("declFieldCall")), self.names("declFieldCall"))

    def test_logical_operands_where_they_are_the_value(self):
        self.assertEqual({NULL, NUMBER}, self.returned("declAndValue"))
        self.assertEqual({self.PACK, TEXT + "none"}, self.returned("declOrValue"))
        self.assertEqual({self.PACK, TEXT + "none"}, self.returned("declNullishValue"))

    def test_literal_typed_as_interface_shares_its_properties(self):
        found = self.returned("declShadowRead")                     # `c.shadow` truthy: written through the Car
        self.assertTrue(any("<object@" in v for v in found), found)

    def test_this_is_the_receiver(self):
        self.assertEqual({CLASS + "declared.ts#ChainLeaf"}, self.returned("declFluent"))
        self.assertIn("ChainOther.otherOnly", self.names("declFluentOther"))
        self.assertEqual({CLASS + "declared.ts#Lone"}, self.returned("Lone.me"))   # called by nobody: its class

    def test_branded_number_is_a_number(self):
        self.assertEqual({NUMBER}, self.returned("declBranded"))

    def test_objects_typed_as_one_interface_stay_apart(self):
        self.returned("declSpotTyped")                              # rawB's object read as a Spot: joined
        self.assertIn(self.PACK, self.returned("declSpotA"))        # spotA's object joined too
        self.assertNotIn(self.PACK, self.returned("declSpotB"))     # spotA's own Pack is not rawB's

    def test_no_arity_overloads_in_javascript(self):
        found = self.names("declArity")
        self.assertTrue(any("<object@" in n and n.endswith(".emitted") for n in found), found)

    def test_satisfies_is_the_value_alone(self):
        found = self.returned("declSatisfies")
        self.assertEqual(1, len(found), found)
        self.assertIn("<object@", next(iter(found)))

    def test_satisfies_keeps_the_spread_base(self):
        written = self.names("declSpreadBase", ("write",))
        self.assertTrue(any("<object@" in label for label in written), written)

    def test_parameter_property(self):
        self.assertEqual({self.PACK}, self.returned("declParamProperty"))

    def test_optional_method_may_be_absent(self):
        self.assertIn("Reads.at", self.names("declOptionalMethod"))  # the else of `f.atInto ?` keeps the Reads

    def test_count_of_a_built_array(self):
        self.assertIn(self.PACK, self.returned("declCount"))          # `!all.length` leaves: all held Packs

    def test_plain_call_statement_proves_nothing(self):
        self.assertIn(TEXT + "a", self.returned("declPlainCall"))

    def test_omitted_argument_is_undefined(self):
        self.assertEqual({UNDEFINED}, self.returned("declLeftCall"))


class GdAccessors(Solved, FixtureCase):
    """G8': a read runs the getter, a write the setter (fixture `pending/accessors.gd`)."""
    ROOTS = ("pending",)
    BASE = "pending"

    def crates(self, spec: str) -> bool:
        return any(v.endswith("accessors.gd.Crate") for v in self.returned(spec))

    def test_read_through_the_getter(self):
        self.assertTrue(self.crates("gd_getter"))
        self.assertTrue(self.crates("gd_own"))          # its own getter reads the stored member, initialized

    def test_write_through_the_setter(self):
        self.assertFalse(self.crates("gd_setter"))      # the setter stores elsewhere
        self.assertTrue(self.crates("gd_stored"))


class GdReceivers(Solved, FixtureCase):
    """J2': `self` is the object the method runs on (fixture `pending/receivers.gd`)."""
    ROOTS = ("pending",)
    BASE = "pending"

    def test_self_is_the_receiver(self):
        for spec in ("gd_self_receiver", "gd_implicit_receiver"):    # through a member call; a call with no receiver
            found = self.returned(spec)
            self.assertTrue(any(v.endswith("receivers.gd.Leaf2") for v in found), (spec, found))
            self.assertFalse(any(v.endswith("receivers.gd.Base2") for v in found), (spec, found))


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
