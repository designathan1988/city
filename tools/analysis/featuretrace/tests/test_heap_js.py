"""The JavaScript and TypeScript expectations of `test_js.py` and `test_js_values.py` on the new core
(log/trace-core-plan.md, Task 6; the shared ones in `js_expectations`): modules, namespaces and barrels, JSX through
each transform, iteration and generators, async functions, the function intrinsics (`bind`, `call`), the platform's
callbacks and events — each from the languages' rules and the fixtures' declarations (a lib written for the tests), no
name of a program or platform read by the analysis."""
from __future__ import annotations

import unittest

from .js_expectations import DeclaredWebExpectations, JsValuesExpectations, WebExpectations
from .support import ENGINE, HeapCase


class WebHeap(HeapCase, WebExpectations, DeclaredWebExpectations):
    ROOTS = ("web", "web/node_modules/three")
    BASE = "web"
    DECLARATIONS = True


class JsxAutomatic(HeapCase):
    ROOTS = ("heap/jsxauto", "heap/jsxauto/node_modules/rt")
    BASE = "heap/jsxauto"

    def test_the_runtime_function_calls_the_component_back(self):
        found = self.names("app.tsx:6")
        self.assertIn("app.tsx.List", found)
        self.assertTrue(any(n.endswith("jsx-runtime.d.ts.jsx") for n in found), found)

    def test_the_children_are_the_prop_the_runtime_names(self):
        self.assertEqual({"Apple"}, self.classes("shown"))


class JsxClassic(HeapCase):
    ROOTS = ("heap/jsxclassic",)
    BASE = "heap/jsxclassic"

    def test_the_named_factory_is_called_with_the_tag_and_props(self):
        self.assertIn("app.tsx.h", self.names("app.tsx:5"))
        self.assertEqual({"Apple"}, self.classes("viaFactory"))


class JsValuesHeap(HeapCase, JsValuesExpectations):
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"
    DECLARATIONS = True

    def test_a_program_annotation_is_no_value(self):
        self.assertIn("types.ts.boxes", self.names("walk"))     # `boxes(): Box[]` returns `[]`: nothing to open
        self.assertNotIn("Box.open", self.names("walk"))

    def test_properties_defined_by_what_the_definers_declare(self):
        # `Object.defineProperty` / `defineProperties` by their declarations alone: `o: T` (the function knows the
        # object only as itself), a `PropertyKey` naming the member, a `PropertyDescriptor` whose `value` is `any` and
        # whose `get` is called back, a map whose keys an index signature types as property keys
        for spec in ("heldSpot", "heldTwice", "heldShort", "heldByRef", "heldViaVar", "heldComputed", "heldMap"):
            self.assertEqual({"Spot"}, self.classes(spec), spec)

    def test_call_and_apply_by_their_declarations(self):
        self.assertEqual({"<new Shape2@16>"}, self.classes("thisCall"))     # `this` is what `call` is given
        self.assertEqual({"str:c"}, self.returned("callForm"))
        self.assertEqual({"str:x", "str:y"}, self.returned("applyForm"))   # `...args: A`: an element of the array A


class WebHeapEngine(WebHeap):
    CORE = ENGINE


class JsxAutomaticEngine(JsxAutomatic):
    CORE = ENGINE


class JsxClassicEngine(JsxClassic):
    CORE = ENGINE


class JsValuesHeapEngine(JsValuesHeap):
    CORE = ENGINE


if __name__ == "__main__":
    unittest.main()
