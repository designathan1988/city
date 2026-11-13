"""The links the tracer finds in each language it reads (fixtures `gd`, `ts`, `cpp`): calls by a plain name, through
`self`/`this`, through a declared type, an import and a base class; an override below a base; a function kept in a
variable, an object or a dictionary and called; a callback handed to code the fixture does not define, and run by a
later use of what it was handed to; a member of a receiver nothing is known of, found by its name and marked so."""
from __future__ import annotations

import unittest

from support import BY_NAME, CALL, EXACT, FIRES, HANDED, NEW, reached, traced


class GDScript(unittest.TestCase):
    def setUp(self):
        self.r = traced("gd")

    def test_calls_by_name_self_type_and_preload(self):
        found = reached(self.r, "Shop.open", (CALL,))
        self.assertLessEqual({"Crate.lift", "Shop.count", "tool.use"}, found)

    def test_a_base_by_its_path_and_the_override_below_it(self):
        self.assertEqual({"base.greet", "Shop.greet"}, reached(self.r, "base.visit", (CALL,)))

    def test_a_lambda_kept_in_a_variable_is_handed_to_what_calls_it(self):
        """`hook.call(a)`: `call` is the member of a function value that runs it (spec 002, D15/D16): fired."""
        self.assertIn("Shop.hook", reached(self.r, "Shop.open", (FIRES,)))
        self.assertEqual({"Shop.polish"}, reached(self.r, "Shop.hook", (CALL,)))

    def test_a_handler_handed_to_a_signal_runs_where_the_signal_is_used(self):
        """Connecting hands the handler over (registered); emitting fires it (spec 002, D16)."""
        self.assertIn("Shop._on_sold", reached(self.r, "Shop._init", (HANDED,)))
        self.assertIn("Shop._on_sold", reached(self.r, "Shop.sell", (FIRES,)))

    def test_a_function_kept_in_a_dictionary(self):
        """`table["run"].call(n)`: the entry's function, run by `call` (spec 002, D15/D16): fired."""
        self.assertIn("Shop.polish", reached(self.r, "Shop.count", (FIRES,)))

    def test_a_receiver_nothing_is_known_of_is_found_by_name(self):
        self.assertEqual({"Crate.lift"}, reached(self.r, "Shop.unknown", (CALL,), BY_NAME))
        self.assertEqual({"Crate.weigh"}, reached(self.r, "Crate.lift", (CALL,), EXACT))


class TypeScript(unittest.TestCase):
    def setUp(self):
        self.r = traced("ts")

    def test_imports_by_alias_namespace_and_default(self):
        found = reached(self.r, "main", (CALL, NEW))
        self.assertLessEqual({"Shop.constructor", "Crate", "Shop.open", "run", "sharpen"}, found)

    def test_a_callback_runs_where_the_function_it_was_given_to_calls_it(self):
        self.assertEqual({"later", "byFirst"}, reached(self.r, "run", (CALL,)))
        self.assertEqual({"sharpen"}, reached(self.r, "later", (CALL,)))

    def test_an_object_literal_s_method_and_property(self):
        self.assertIn("main.hop", reached(self.r, "main", (CALL,)))
        self.assertEqual({"later"}, reached(self.r, "main.hop", (CALL,)))

    def test_a_parameter_property_s_type_and_an_override(self):
        self.assertEqual({"Crate.lift"}, reached(self.r, "Shop.open", (CALL,)))
        self.assertEqual({"Counter.greet", "Shop.greet"}, reached(self.r, "Counter.visit", (CALL,)))

    def test_callbacks_handed_to_code_the_fixture_does_not_define(self):
        handed = reached(self.r, "main", (HANDED,))
        self.assertIn("onSold", handed)
        self.assertTrue(any(label.startswith("main.<lambda:") for label in handed), handed)

    def test_a_receiver_nothing_is_known_of_is_found_by_name(self):
        self.assertEqual({"Crate.lift", "Winch.lift"}, reached(self.r, "loose", (CALL,), BY_NAME))
        self.assertEqual(set(), reached(self.r, "onSold", (CALL,), BY_NAME))


class Cpp(unittest.TestCase):
    def setUp(self):
        self.r = traced("cpp")

    def test_a_method_written_out_of_its_class_reads_its_members(self):
        self.assertEqual({"fx.Crate.lift"}, reached(self.r, "fx.Shop.Shop", (CALL,)))
        self.assertEqual({"fx.helper"}, reached(self.r, "fx.Shop.greet", (CALL,)))

    def test_a_virtual_method_and_its_override(self):
        self.assertEqual({"fx.Counter.greet", "fx.Shop.greet"}, reached(self.r, "fx.Counter.visit", (CALL,)))

    def test_a_lambda_kept_in_a_local(self):
        found = reached(self.r, "fx.Shop.open", (CALL,))
        self.assertTrue(any(label.startswith("fx.Shop.open.twice") for label in found), found)
        self.assertEqual({"fx.Crate.lift", "fx.helper"}, reached(self.r, "fx.Shop.open.twice", (CALL,)))

    def test_objects_made_and_a_function_handed_by_its_address(self):
        self.assertIn("fx.Shop.Shop", reached(self.r, "fx.Shop.make", (NEW,)))
        self.assertLessEqual({"fx.Shop.open", "fx.Counter.visit"}, reached(self.r, "fx.Shop.make", (CALL,)))
        self.assertEqual({"fx.helper"}, reached(self.r, "fx.Shop.make", (HANDED,)))


if __name__ == "__main__":
    unittest.main()
