"""The heap analysis's core (log/trace-core-plan.md, Task 1): abstract objects by allocation site, fields per object,
calls resolved by the callee's objects, aliases through the heap (fixture `heap/core.ts`)."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))

from featuretrace import program, seeds  # noqa: E402
from featuretrace.heap import analysis  # noqa: E402

FIXTURE = HERE / "fixture" / "heap"
_DONE = {}


def analysed():
    if "core" not in _DONE:
        ix, files, broken = program.read([FIXTURE], FIXTURE, None)
        _DONE["core"] = (ix, analysis.analyse(ix))
    return _DONE["core"]


class HeapCore(unittest.TestCase):
    def setUp(self):
        self.ix, self.result = analysed()

    def gid(self, spec: str) -> int:
        found = seeds.find(self.ix, spec)
        self.assertEqual(1, len(found), found)
        return found[0]

    def calls(self, spec: str) -> set:
        return {seeds.label(self.ix, e.dst) for e in self.result.out[self.gid(spec)] if e.kind in ("call", "new")}

    def returned(self, spec: str) -> set:
        return self.result.returned(self.gid(spec))

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

    def test_a_computed_key_written_reaches_a_named_read(self):
        found = self.returned("computedAlias")
        self.assertTrue(any(v.startswith("new:") and "Tile" in v for v in found), found)

    def test_an_object_literal_property(self):
        found = self.returned("literalRead")
        self.assertTrue(any(v.startswith("new:") and "Box" in v for v in found), found)


if __name__ == "__main__":
    unittest.main()
