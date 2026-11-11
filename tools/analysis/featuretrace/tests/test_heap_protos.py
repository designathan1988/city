"""Prototypes and classes as objects in the new core (log/trace-core-plan.md, Task 2): methods through the chain and
`super`, static members on the class, accessors run on read and write, a function's prototype object, `constructor`
(fixture `heap/protos.js`)."""
from __future__ import annotations

import unittest

from .support import ENGINE, HeapCase


class HeapProtos(HeapCase):
    ROOT = "protos.js"

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


class HeapProtosEngine(HeapProtos):
    CORE = ENGINE


if __name__ == "__main__":
    unittest.main()
