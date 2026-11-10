"""Calls the languages make without a call written (G8, C7) and values that come back to themselves (FLOW fixpoint),
each on the small fixture."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))

from featuretrace import program, seeds  # noqa: E402

FIXTURE = HERE / "fixture"


class Implicit(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.ix, cls.rs, _, _ = program.load([FIXTURE / "project", FIXTURE / "native"], FIXTURE, None)

    def calls(self, spec):
        found = seeds.find(self.ix, spec)
        self.assertEqual(len(found), 1, spec)
        return {seeds.label(self.ix, e.dst) for e in self.rs.out[found[0]] if e.kind in ("call", "new")}

    def test_property_write_and_read_run_set_and_get(self):
        out = self.calls("Controller.uses_props")
        self.assertTrue({"Controller.watched.set", "Controller.watched.get", "Controller._set_named"} <= out, out)

    def test_inside_its_own_setter_the_name_is_the_member(self):
        self.assertNotIn("Controller.watched.set", self.calls("Controller.watched.set"))

    def test_value_through_a_cycle_of_calls(self):
        self.assertIn("RoadTool.click", self.calls("Controller.cyc_b"))

    def test_object_declaration_constructor_and_destructor(self):
        self.assertTrue({"fx::Vec::Vec", "fx::Vec::~Vec"} <= self.calls("fx::implicit_calls"))

    def test_operator_and_range_for(self):
        self.assertTrue({"fx::Vec::operator+", "fx::Vec::begin", "fx::Vec::end"} <= self.calls("fx::implicit_calls"))

    def test_macro_use(self):
        self.assertIn("TWICE", self.calls("fx::implicit_calls"))

    def test_constructor_runs_member_constructors(self):
        self.assertIn("fx::Vec::Vec", self.calls("fx::Holder::Holder"))


if __name__ == "__main__":
    unittest.main()
