"""VALUE, LOOK and FLOW on the fixture project (log/trace-rules.md): a receiver holds every value that flows into it,
by the language's rules; nothing falls back to a guess by name."""
from __future__ import annotations

import unittest

from .support import FixtureCase


class Flows(FixtureCase):
    def test_untyped_parameter_holds_every_argument_passed(self):
        # FLOW: `x` has no type; its callers pass a RoadTool and a ToolBase
        self.assertEqual(set(self.out("Controller.mixed")), {"RoadTool.click", "ToolBase.click"})

    def test_untyped_parameter_with_one_caller(self):
        self.assertEqual(self.out("Controller.untyped"), {"RoadTool.click": "exact"})

    def test_container_elements_are_what_is_put_into_it(self):
        self.assertIn("RoadTool._lay", self.out("Controller.flows"))

    def test_call_result_is_what_the_function_returns(self):
        # `made()` declares no return type: its value is what its `return` carries (FLOW)
        self.assertEqual(self.out("Controller.flows").get("RoadTool._lay"), "exact")

    def test_engine_member_is_the_engines(self):
        # `name` is a member of the engine base (Node): its value and members are the engine's (LOOK)
        self.assertIn("length", self.rs.external[self.gid("Controller.flows")])

    def test_no_value_reaches_a_parameter_nobody_passes(self):
        self.assertFalse(self.out("Controller.never_called"))
        self.assertIn("click", self.rs.unreached[self.gid("Controller.never_called")])

    def test_cpp_deduced_return(self):
        self.assertEqual(self.out("fx::callers").get("fx::Node2::touch"), "exact")

    def test_cpp_argument_dependent_lookup(self):
        self.assertEqual(self.out("fx::callers").get("fx2::adl_target"), "exact")

    def test_a_name_held_in_a_string_names_the_method(self):
        # G7: Callable(obj, m) and call(m) take the name as a value; the string flows like any value
        self.assertIn("Controller.go_by_name", self.out("Controller.by_names"))
        self.assertIn("Controller.go_by_name", self.out("Controller.by_names", ("name",)))

    def test_a_callable_built_from_a_parameter_and_kept_in_a_container(self):
        self.assertIn("Controller.go_routed", self.out("Controller.by_names"))

    def test_nothing_is_a_candidate(self):
        self.assertFalse([e for e in self.rs.edges if "candidate" in e.how])


if __name__ == "__main__":
    unittest.main()
