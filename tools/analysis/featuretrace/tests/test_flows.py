"""VALUE, LOOK and FLOW on the fixture project (log/trace-rules.md): a receiver holds every value that flows into it,
by the language's rules; nothing falls back to a guess by name."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))

from featuretrace import program, seeds  # noqa: E402

FIXTURE = HERE / "fixture"


class Flows(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.ix, cls.rs, _, _ = program.load([FIXTURE / "project", FIXTURE / "native"], FIXTURE, None)

    def gid(self, spec):
        found = seeds.find(self.ix, spec)
        self.assertEqual(len(found), 1, spec)
        return found[0]

    def calls(self, spec, kind="call"):
        return {seeds.label(self.ix, e.dst): e.how for e in self.rs.out[self.gid(spec)] if e.kind == kind}

    def test_untyped_parameter_holds_every_argument_passed(self):
        # FLOW: `x` has no type; its callers pass a RoadTool and a ToolBase
        self.assertEqual(set(self.calls("Controller.mixed")), {"RoadTool.click", "ToolBase.click"})

    def test_untyped_parameter_with_one_caller(self):
        self.assertEqual(self.calls("Controller.untyped"), {"RoadTool.click": "exact"})

    def test_container_elements_are_what_is_put_into_it(self):
        self.assertIn("RoadTool._lay", self.calls("Controller.flows"))

    def test_call_result_is_what_the_function_returns(self):
        # `made()` declares no return type: its value is what its `return` carries (FLOW)
        self.assertEqual(self.calls("Controller.flows").get("RoadTool._lay"), "exact")

    def test_engine_member_is_the_engines(self):
        # `name` is a member of the engine base (Node): its value and members are the engine's (LOOK)
        self.assertIn("length", self.rs.external[self.gid("Controller.flows")])

    def test_no_value_reaches_a_parameter_nobody_passes(self):
        self.assertFalse(self.calls("Controller.never_called"))
        self.assertIn("click", self.rs.unreached[self.gid("Controller.never_called")])

    def test_cpp_deduced_return(self):
        self.assertEqual(self.calls("fx::callers").get("fx::Node2::touch"), "exact")

    def test_cpp_argument_dependent_lookup(self):
        self.assertEqual(self.calls("fx::callers").get("fx2::adl_target"), "exact")

    def test_nothing_is_a_candidate(self):
        self.assertFalse([e for e in self.rs.edges if "candidate" in e.how])


if __name__ == "__main__":
    unittest.main()
