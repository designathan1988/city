"""The platform from its declarations in the new core (log/trace-core-plan.md, Task 4): a declared method's arguments
reach its receiver's type-parameter places, its callbacks are called with them, its result is them or an object of its
declared type; iteration and `await` follow the language's protocols; the declaration files a program has are those
its tsconfig and its TypeScript choose (fixtures `heap/platform`, `heap/libs`)."""
from __future__ import annotations

import unittest

from .. import js_libs, program
from .support import ENGINE, HEAP, HeapCase

LIBS = HEAP / "libs"


class HeapPlatform(HeapCase):
    ROOT = "platform"

    def test_a_push_is_read_back_through_an_alias(self):
        self.assertEqual({"Apple"}, self.classes("pushed"))

    def test_a_callback_gets_the_receiver_elements(self):
        self.assertEqual({"Apple"}, self.classes("mappedValue"))

    def test_a_callback_result_is_an_element_of_the_result(self):
        self.assertEqual({"Pear"}, self.classes("mappedResult"))

    def test_a_map_gives_back_what_was_set(self):
        self.assertEqual({"Apple"}, self.classes("mapGet"))

    def test_a_primitive_keeps_no_property(self):
        self.assertEqual(set(), self.classes("primitiveKeepsNothing"))   # ECMA-262 PutValue: a wrapper, gone
        self.assertEqual(set(), self.classes("undefinedKeepsNothing"))   # undefined has no property at all

    def test_what_is_written_into_unknown_code_is_called_back(self):
        handed = self.names("escaped", ("handed",))
        self.assertTrue(any("<lambda" in n for n in handed), handed)

    def test_a_structural_argument_reaches_only_its_own_call(self):
        self.assertEqual({"Apple"}, self.classes("unboxApple"))
        self.assertEqual({"Pear"}, self.classes("unboxPear"))

    def test_a_method_returning_this_gives_its_receiver(self):
        self.assertEqual({"Apple"}, self.classes("chained"))              # `set(...): this` then `get`

    def test_then_calls_back_with_the_resolved_value(self):
        self.assertEqual({"Apple"}, self.classes("thenValue"))

    def test_await_gives_the_resolved_value(self):
        self.assertEqual({"Apple"}, self.classes("awaited"))

    def test_an_executor_resolves_its_promise(self):
        self.assertEqual({"Apple"}, self.classes("executed"))

    def test_iterating_a_set_gives_its_elements(self):
        self.assertEqual({"Apple"}, self.classes("iterated"))

    def test_a_host_method_gives_an_object_of_its_declared_type(self):
        self.assertTrue(self.declared("hosted", "Element"), self.returned("hosted"))

    def test_an_object_given_for_an_interface_has_its_method_called_back(self):
        self.assertTrue(self.declared("listened", "Element"), self.returned("listened"))

    def test_a_function_given_for_a_function_type_is_called_back(self):
        self.assertTrue(self.declared("called", "Element"), self.returned("called"))

    def test_an_entry_parameter_is_an_object_of_its_declared_type(self):
        self.assertIn(("call", "Element.focus"), {(k, d.rsplit(".", 2)[-2] + "." + d.rsplit(".", 1)[-1])
                                                for k, d in self.links("onFocus")})

    def test_a_global_augmentation_merges_into_the_global_type(self):
        self.assertTrue(any(k == "call" and d.endswith("Page.title") for k, d in self.links("titled")))
        self.assertTrue(self.declared("titled", "Element"), self.returned("titled"))

    def test_a_host_method_called_is_linked(self):
        self.assertTrue(any(k == "call" and d.endswith("Element.focus") for k, d in self.links("focused")))


class DeclarationFiles(unittest.TestCase):
    def names(self, folder: str) -> set:
        return {p.relative_to(LIBS / "node_modules").as_posix() for p in js_libs.declaration_files(LIBS / folder)}

    def test_the_tsconfig_chooses_the_libs_and_the_types(self):
        self.assertEqual({"typescript/lib/lib.es2015.d.ts", "typescript/lib/lib.es5.d.ts",
                          "typescript/lib/lib.es2015.core.d.ts", "typescript/lib/lib.dom.d.ts",
                          "@types/thing/index.d.ts"}, self.names("chosen"))

    def test_the_program_reads_them_beside_its_own_files(self):
        ix, _, _ = program.read([LIBS / "chosen"], LIBS, None, declarations=True)
        files = {u.file.rsplit("/", 1)[-1] for u in ix.units}
        self.assertTrue({"main.ts", "lib.es5.d.ts", "lib.es2015.core.d.ts", "lib.dom.d.ts", "index.d.ts"} <= files, files)

    def test_the_default_lib_and_every_visible_types_package(self):
        self.assertEqual({"typescript/lib/lib.d.ts", "typescript/lib/lib.es5.d.ts", "typescript/lib/lib.dom.d.ts",
                          "@types/thing/index.d.ts", "@types/other/main.d.ts", "@types/other/more.d.ts"},
                         self.names("plain"))


class HeapPlatformEngine(HeapPlatform):
    CORE = ENGINE


if __name__ == "__main__":
    unittest.main()
