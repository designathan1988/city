"""Prototypes and classes as objects in the new core (log/trace-core-plan.md, Task 2): methods through the chain and
`super`, static members on the class, accessors run on read and write, a function's prototype object, `constructor`
(fixture `heap/protos.js`)."""
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
    if "protos" not in _DONE:
        ix, _, _ = program.read([FIXTURE / "protos.js"], FIXTURE, None)
        _DONE["protos"] = (ix, analysis.analyse(ix))
    return _DONE["protos"]


class HeapProtos(unittest.TestCase):
    def setUp(self):
        self.ix, self.result = analysed()

    def returned(self, spec: str) -> set:
        found = seeds.find(self.ix, spec)
        self.assertEqual(1, len(found), found)
        return self.result.returned(found[0])

    def made(self, spec: str, cls: str) -> bool:
        return any(v.startswith("new:") and v.split(":")[1].endswith(cls) for v in self.returned(spec))

    def test_a_method_through_the_chain_and_super(self):
        self.assertTrue(self.made("inherited", "#Leaf"), self.returned("inherited"))

    def test_a_static_member_on_the_class(self):
        self.assertTrue(self.made("staticMade", "#Base"), self.returned("staticMade"))

    def test_a_base_constructor_through_super(self):
        self.assertTrue(self.made("superField", "#Leaf"), self.returned("superField"))

    def test_accessors_run_on_read_and_write(self):
        self.assertTrue(self.made("getterRead", "#Leaf"), self.returned("getterRead"))
        self.assertTrue(self.made("setterWrite", "#Leaf"), self.returned("setterWrite"))

    def test_a_function_s_prototype(self):
        self.assertTrue(self.made("legacyGreet", "#Leaf"), self.returned("legacyGreet"))
        self.assertTrue(self.made("replacedKind", "#Leaf"), self.returned("replacedKind"))

    def test_constructor_is_the_class(self):
        self.assertTrue(self.made("constructorOf", "#Derived"), self.returned("constructorOf"))


if __name__ == "__main__":
    unittest.main()
