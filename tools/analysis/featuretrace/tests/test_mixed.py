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
from featuretrace.solver import CLASS, ENGINE, NUMBER, TEXT, UNDEFINED  # noqa: E402


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

    def test_descriptor_members_by_their_values(self):
        self.assertEqual({self.SPOT}, self.returned("heldShort"))         # `{ value }`
        self.assertEqual({self.SPOT}, self.returned("heldByRef"))         # `{ get: spotGetter }`
        self.assertIn(self.SPOT, self.returned("heldViaVar"))             # a descriptor held in a variable
        self.assertIn(self.SPOT, self.returned("heldComputed"))           # a key no literal names: `o[k] = v`
        self.assertIn(self.SPOT, self.returned("heldMap"))                # a map of descriptors held in a variable
        handed = self.names("Holder2.constructor", ("handed",))          # the getter handed to the language
        self.assertTrue(any("spotGetter" in n for n in handed), handed)


class LateWrites(Solved, FixtureCase):
    """J7': a hoisted closure writes a variable declared before it in the text (fixture `jsvalues/latewrite.js`)."""
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"

    def test_hoisted_closure_writes_the_enclosing_variable(self):
        self.assertIn(CLASS + "latewrite.js#LateBox", self.returned("lateWrite"))

    def test_a_block_local_is_not_a_later_function_of_its_name(self):
        found = self.returned("scopedLocal")                         # the block's `chosen`, not the function's arrow
        self.assertNotIn(UNDEFINED, found)
        self.assertTrue(any(v.startswith("Array<") for v in found), found)


class FunctionConstructors(Solved, FixtureCase):
    """N1': `new F()` on a function makes the object F writes on `this` (fixture `jsvalues/newfn.js`)."""
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"

    def test_new_object_of_a_function(self):
        made = self.returned("madeObject")
        self.assertTrue(made and all("Counter2" in v for v in made), made)
        self.assertEqual({NUMBER}, self.returned("madeCount"))
        self.assertEqual({NUMBER}, self.returned("madeBump"))
        self.assertEqual(set(), self.names("Counter2", ("handed",)))     # written on `this` before it held a value

    def test_prototype_of_a_function(self):
        self.assertEqual({NUMBER}, self.returned("protoArea"))           # `F.prototype.m = ...` (N1'')
        made = self.returned("protoSelf")
        self.assertTrue(made and all("Shape2" in v for v in made), made)
        self.assertEqual({TEXT + "legacy"}, self.returned("protoReplaced"))     # `F.prototype = {...}`
        self.assertEqual({NUMBER}, self.returned("protoInherited"))      # `Object.create(Base.prototype)`
        self.assertEqual({TEXT + "x"}, self.returned("fnExpando"))       # a property written on a function
        self.assertEqual({UNDEFINED}, self.returned("arrowProto"))       # an arrow has no prototype

    def test_this_of_a_function_by_its_calls(self):
        self.assertIn(UNDEFINED, self.returned("thisPlain"))             # a plain call in strict code
        self.assertFalse(any("newfn.js" == v.lstrip("#") for v in self.returned("thisPlain")))
        made = self.returned("thisCall")                                 # `f.call(o)`
        self.assertTrue(any("Shape2" in v for v in made), made)
        self.assertEqual({ENGINE}, self.returned("thisHanded"))          # the engine decides it
        self.assertEqual({ENGINE}, self.returned("sloppyRead"))          # sloppy code: the global object
        self.assertEqual({UNDEFINED}, self.returned("strictRead"))       # "use strict"
        self.assertEqual({NUMBER}, self.returned("heldRun"))             # the object whose `run` holds it
        self.assertNotIn(NUMBER, self.returned("heldWrite"))             # `this.mark = v` writes heldC's alone


class StaticReceiver(FixtureCase):
    """C++: the receiver's static type decides (fixture `pending/handed.cpp`)."""
    ROOTS = ("pending",)
    BASE = "pending"

    def test_lambda_passed_to_a_method_of_the_program_is_not_handed(self):
        self.assertIn("Pool::each", self.names("Watcher::_notification"))
        self.assertEqual(set(), self.names("Watcher::_notification", ("handed",)))


if __name__ == "__main__":
    unittest.main()
