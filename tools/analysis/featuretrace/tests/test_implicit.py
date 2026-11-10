"""Calls the languages make without a call written (G8, C7) and values that come back to themselves (FLOW fixpoint),
each on the small fixture."""
from __future__ import annotations

import unittest

from .support import FixtureCase


class Implicit(FixtureCase):
    def test_property_write_and_read_run_set_and_get(self):
        out = self.names("Controller.uses_props")
        self.assertTrue({"Controller.watched.set", "Controller.watched.get", "Controller._set_named"} <= out, out)

    def test_inside_its_own_setter_the_name_is_the_member(self):
        self.assertNotIn("Controller.watched.set", self.names("Controller.watched.set"))

    def test_value_through_a_cycle_of_calls(self):
        self.assertIn("RoadTool.click", self.names("Controller.cyc_b"))

    def test_object_declaration_constructor_and_destructor(self):
        self.assertTrue({"fx::Vec::Vec", "fx::Vec::~Vec"} <= self.names("fx::implicit_calls"))

    def test_operator_and_range_for(self):
        self.assertTrue({"fx::Vec::operator+", "fx::Vec::begin", "fx::Vec::end"} <= self.names("fx::implicit_calls"))

    def test_macro_use(self):
        self.assertIn("TWICE", self.names("fx::implicit_calls"))

    def test_constructor_runs_member_constructors(self):
        self.assertIn("fx::Vec::Vec", self.names("fx::Holder::Holder"))


if __name__ == "__main__":
    unittest.main()
