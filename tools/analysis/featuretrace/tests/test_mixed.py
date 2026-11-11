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
from featuretrace.solver import CLASS, ENGINE, NUMBER, TEXT  # noqa: E402


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


class ConstructorProperty(Solved, FixtureCase):
    """J4': `obj.constructor` is the object's class (fixture `jsvalues/ctor.js`)."""
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"

    def test_constructor_is_the_class(self):
        self.assertEqual({CLASS + "ctor.js#Cloner"}, self.returned("ctorOf"))
        self.assertEqual({CLASS + "ctor.js#SubCloner"}, self.returned("cloned"))   # the receiver's own class


class HostBase(Solved, FixtureCase):
    """V2': a base the tracer did not read can hold a member, whatever code writes (fixture `jsvalues/hostbase.ts`)."""
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"

    def test_member_of_an_unread_base(self):
        self.assertIn(ENGINE, self.returned("hostGet"))         # `Map.prototype.get`, the host's


class DefinedProperties(Solved, FixtureCase):
    """J14: `Object.defineProperty`, `Object.defineProperties` (fixture `jsvalues/defineprops.js`)."""
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"
    SPOT = CLASS + "defineprops.js#Spot"

    def test_value_and_getter(self):
        self.assertEqual({self.SPOT}, self.returned("heldSpot"))
        self.assertEqual({NUMBER}, self.returned("heldId"))
        self.assertEqual({self.SPOT}, self.returned("heldTwice"))


class LateWrites(Solved, FixtureCase):
    """J7': a hoisted closure writes a variable declared before it in the text (fixture `jsvalues/latewrite.js`)."""
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"

    def test_hoisted_closure_writes_the_enclosing_variable(self):
        self.assertIn(CLASS + "latewrite.js#LateBox", self.returned("lateWrite"))


class FunctionConstructors(Solved, FixtureCase):
    """N1': `new F()` on a function makes the object F writes on `this` (fixture `jsvalues/newfn.js`)."""
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"

    def test_new_object_of_a_function(self):
        made = self.returned("madeObject")
        self.assertTrue(made and all("Counter2" in v for v in made), made)
        self.assertEqual({NUMBER}, self.returned("madeCount"))
        self.assertEqual({NUMBER}, self.returned("madeBump"))


class StaticReceiver(FixtureCase):
    """C++: the receiver's static type decides (fixture `pending/handed.cpp`)."""
    ROOTS = ("pending",)
    BASE = "pending"

    def test_lambda_passed_to_a_method_of_the_program_is_not_handed(self):
        self.assertIn("Pool::each", self.names("Watcher::_notification"))
        self.assertEqual(set(), self.names("Watcher::_notification", ("handed",)))


if __name__ == "__main__":
    unittest.main()
