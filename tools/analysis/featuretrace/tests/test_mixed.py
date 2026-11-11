"""A callable handed to the engine through a receiver that can also be an object of the program (log/trace-rules.md,
J6, J11): written into its member or passed to its method, the engine's object runs it (fixture `jsvalues/mixed.ts`)."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))
sys.path.insert(0, str(HERE))

from support import FixtureCase, Solved  # noqa: E402
from featuretrace.solver import NUMBER, TEXT  # noqa: E402


class Mixed(FixtureCase):
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"

    def handed(self, spec: str) -> set:
        return self.names(spec, ("handed",))

    def test_written_into_a_member_of_a_mixed_receiver(self):
        self.assertTrue(any("<lambda:" in n for n in self.handed("mixedWrite")), self.handed("mixedWrite"))
        self.assertIn("OwnPort.onmessage", self.names("mixedWrite", ("write",)))     # and the program's object holds it

    def test_passed_to_a_method_of_a_mixed_receiver(self):
        self.assertTrue(any("<lambda:" in n for n in self.handed("mixedCall")), self.handed("mixedCall"))
        self.assertIn("OwnThen.then", self.names("mixedCall"))                        # and the program's method gets it


class FunctionForms(Solved, FixtureCase):
    """J2'': `call`, `apply` and `bind` by the receiver's value (fixture `jsvalues/forms.js`)."""
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"

    def test_bind_does_not_call(self):
        self.assertEqual({NUMBER}, self.held("FormSizer.setSize", "width"))     # `resize(3, 4)`: never the object

    def test_bound_leading_arguments(self):
        self.assertEqual({TEXT + "second"}, self.returned("boundLeading"))      # `g("second")` is f("first", "second")

    def test_methods_named_like_the_forms(self):
        self.assertEqual({NUMBER}, self.returned("ownBind"))
        self.assertEqual({TEXT + "p"}, self.returned("ownCall"))
        self.assertIn("FormBinder.bind", self.names("ownBind"))

    def test_call_and_apply(self):
        self.assertEqual({TEXT + "c"}, self.returned("callForm"))
        self.assertEqual({TEXT + "x", TEXT + "y"}, self.returned("applyForm"))


class StaticReceiver(FixtureCase):
    """C++: the receiver's static type decides (fixture `pending/handed.cpp`)."""
    ROOTS = ("pending",)
    BASE = "pending"

    def test_lambda_passed_to_a_method_of_the_program_is_not_handed(self):
        self.assertIn("Pool::each", self.names("Watcher::_notification"))
        self.assertEqual(set(), self.names("Watcher::_notification", ("handed",)))


if __name__ == "__main__":
    unittest.main()
