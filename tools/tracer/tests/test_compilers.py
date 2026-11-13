"""What each call runs, as the language's compiler says (`tracer/compilers.py`), on made-up code where the text alone
does not say it: an overload chosen by its argument's type, a method hidden below its class called through the base
(not virtual: the base's runs), methods of one name in classes apart called on an object a declared function gives.
Where a compiler is not installed, its tests say so and are skipped.
"""
from __future__ import annotations

import unittest

from support import CALL, FIXTURE, analyse, reached
from tracer import compilers


def lines_of(result, label: str) -> set:
    """The lines of the definitions the code of that label calls."""
    p = result.program
    return {p.defs[link.dst].line for d in p.defs if p.label(d.id) == label for link in result.out.get(d.id, ())
            if link.kind == CALL}


class Compiled(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.notes = []
        cls.result = analyse([FIXTURE / "compiled"], FIXTURE / "compiled", say=cls.notes.append)

    def answered(self, name: str) -> None:
        said = [n for n in self.notes if n.startswith(name + ":")]
        if not said or " 0 calls answered" in said[0] or "(" in said[0]:
            self.skipTest(f"{name} did not answer: {said}")

    def test_cpp_the_overload_chosen(self):
        self.answered(compilers.CLANG)
        self.assertEqual({3}, lines_of(self.result, "bramble"))

    def test_cpp_the_hidden_method_of_the_base(self):
        self.answered(compilers.CLANG)
        self.assertEqual({"Vend.tock"}, reached(self.result, "frindle", {CALL}))

    def test_cpp_the_method_of_the_declared_type(self):
        self.answered(compilers.CLANG)
        got = reached(self.result, "frob", {CALL})
        self.assertIn("Vend.tock", got)
        self.assertNotIn("Wisk.tock", got)

    def test_typescript_the_method_of_the_declared_type(self):
        self.answered(compilers.TSC)
        got = reached(self.result, "brew", {CALL})
        self.assertIn("Kettle.pour", got)
        self.assertNotIn("Ladle.pour", got)


if __name__ == "__main__":
    unittest.main()
