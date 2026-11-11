"""The platform from its declarations in the new core (log/trace-core-plan.md, Task 4): a declared method's arguments
reach its receiver's type-parameter places, its callbacks are called with them, its result is them or an object of its
declared type; iteration and `await` follow the language's protocols; the declaration files a program has are those
its tsconfig and its TypeScript choose (fixtures `heap/platform`, `heap/libs`)."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))

from featuretrace import js_libs, program, seeds  # noqa: E402
from featuretrace.heap import analysis  # noqa: E402

FIXTURE = HERE / "fixture" / "heap"
LIBS = FIXTURE / "libs"
_DONE = {}


def analysed():
    if "platform" not in _DONE:
        ix, _, _ = program.read([FIXTURE / "platform"], FIXTURE, None)
        _DONE["platform"] = (ix, analysis.analyse(ix))
    return _DONE["platform"]


class HeapPlatform(unittest.TestCase):
    def setUp(self):
        self.ix, self.result = analysed()

    def gid(self, spec: str) -> int:
        found = seeds.find(self.ix, spec)
        self.assertEqual(1, len(found), found)
        return found[0]

    def classes(self, spec: str) -> set:
        return {v.split(":")[1].rsplit("#", 1)[-1] for v in self.result.returned(self.gid(spec)) if v.startswith("new:")}

    def test_a_push_is_read_back_through_an_alias(self):
        self.assertEqual({"Apple"}, self.classes("pushed"))

    def test_a_callback_gets_the_receiver_elements(self):
        self.assertEqual({"Apple"}, self.classes("mappedValue"))

    def test_a_callback_result_is_an_element_of_the_result(self):
        self.assertEqual({"Pear"}, self.classes("mappedResult"))

    def test_a_map_gives_back_what_was_set(self):
        self.assertEqual({"Apple"}, self.classes("mapGet"))

    def test_then_calls_back_with_the_resolved_value(self):
        self.assertEqual({"Apple"}, self.classes("thenValue"))

    def test_await_gives_the_resolved_value(self):
        self.assertEqual({"Apple"}, self.classes("awaited"))

    def test_an_executor_resolves_its_promise(self):
        self.assertEqual({"Apple"}, self.classes("executed"))

    def test_iterating_a_set_gives_its_elements(self):
        self.assertEqual({"Apple"}, self.classes("iterated"))

    def test_a_host_method_gives_an_object_of_its_declared_type(self):
        found = self.result.returned(self.gid("hosted"))
        self.assertTrue(any(v.startswith("decl:") and "#Element" in v for v in found), found)

    def test_an_object_given_for_an_interface_has_its_method_called_back(self):
        found = self.result.returned(self.gid("listened"))
        self.assertTrue(any(v.startswith("decl:") and "#Element" in v for v in found), found)

    def test_a_function_given_for_a_function_type_is_called_back(self):
        found = self.result.returned(self.gid("called"))
        self.assertTrue(any(v.startswith("decl:") and "#Element" in v for v in found), found)

    def test_an_entry_parameter_is_an_object_of_its_declared_type(self):
        calls = {seeds.label(self.ix, e.dst) for e in self.result.out[self.gid("onFocus")] if e.kind == "call"}
        self.assertTrue(any(c.endswith("Element.focus") for c in calls), calls)

    def test_a_global_augmentation_merges_into_the_global_type(self):
        calls = {seeds.label(self.ix, e.dst) for e in self.result.out[self.gid("titled")] if e.kind == "call"}
        self.assertTrue(any(c.endswith("Page.title") for c in calls), calls)
        found = self.result.returned(self.gid("titled"))
        self.assertTrue(any(v.startswith("decl:") and "#Element" in v for v in found), found)

    def test_a_host_method_called_is_linked(self):
        calls = {seeds.label(self.ix, e.dst) for e in self.result.out[self.gid("focused")] if e.kind == "call"}
        self.assertTrue(any(c.endswith("Element.focus") for c in calls), calls)


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


if __name__ == "__main__":
    unittest.main()
