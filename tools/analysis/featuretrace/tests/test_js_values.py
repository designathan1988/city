"""JavaScript and TypeScript values by the languages' own rules (log/trace-rules.md, "JavaScript and TypeScript values
by the languages' own rules": L1, L2, T1-T3, O1, N1), checked on the links they make and the values the typer holds."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))
sys.path.insert(0, str(HERE))

from support import FixtureCase  # noqa: E402
from featuretrace import js_library  # noqa: E402
from featuretrace.solver import CLASS, ELEM, ENGINE, FN, TEXT  # noqa: E402

BOX = CLASS + "types.ts#Box"


class JsValues(FixtureCase):
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"

    def values(self, spec: str, tree: tuple) -> set:
        unit = self.ix.units[self.gid(spec)]
        return {text for text, _ in self.rs.typer.evaluate(tree, unit)}

    def test_inner_function_is_not_global(self):
        found = self.names("use")
        self.assertIn("script.js.shown", found)
        self.assertNotIn("script.js.x", found)                 # `outer`'s local, not a global (L2)

    def test_inner_function_is_not_a_member(self):
        self.assertNotIn("K.helper", self.names("viaMember"))   # `m`'s local, not a member of K (L1)

    def test_union_with_null_is_the_class(self):
        self.assertEqual({CLASS + "types.ts#Building"}, self.values("levelOf", ("name", "b")))

    def test_type_parameters_are_erased(self):
        self.assertEqual(set(), self.values("levelOf", ("name", "t")))

    def test_declared_return_gives_elements(self):
        self.assertIn("Box.open", self.names("walk"))

    def test_map_get_and_entries(self):
        self.assertIn("Box.open", self.names("lookup"))        # get: the value
        self.assertIn("Box.open", self.names("entries"))       # iteration: [key, value]
        self.assertIn("Box.open", self.names("types.ts:26"))   # forEach: the value (in the callback)

    def test_promise_awaited(self):
        self.assertIn("Box.open", self.names("later"))

    def test_js_arithmetic_is_a_primitive(self):
        self.assertEqual({ENGINE}, self.values("scaled", ("name", "n")))

    def test_values_do_not_depend_on_the_file(self):
        typer = self.rs.typer
        returned = [text for text, _ in typer.current(typer.ret_node(self.ix.units[self.gid("allBoxes")]))]
        self.assertEqual(1, returned.count(f"Array<{BOX}>"))   # one value from both files
        self.assertEqual({f"Array<{BOX}>", ENGINE}, set(returned))   # and the array literals `[]`

    def test_no_annotation_text_is_a_value(self):
        for node, held in self.rs.typer.value.items():
            for text, _ in held:
                core = text
                while core.startswith(ELEM):
                    core = core[len(ELEM):]
                allowed = core == ENGINE or core.startswith((CLASS, FN, TEXT)) or js_library.is_container(core)
                self.assertTrue(allowed, f"{text!r} in {str(node)[:80]}")

    def test_new_calls_a_function(self):
        found = self.names("mk")
        self.assertIn("news.js.Maker", found)                  # called as a constructor
        self.assertIn("Box.open", found)                       # the object it returns

    def test_new_through_a_variable(self):
        self.assertIn("Box.open", self.names("viaVariable"))

    def test_generator_iteration_gives_its_yields(self):
        self.assertIn("Box.open", self.names("fromGenerator"))

    def test_for_in_names_the_properties(self):
        self.assertIn("Box.open", self.names("eachKey"))                   # k: "a", "b"; table[k]: the Boxes

    def test_object_keys_values_entries(self):
        self.assertIn("Box.open", self.names("keysOf"))
        self.assertIn("Box.open", self.names("valuesOf"))
        self.assertIn("Box.open", self.names("entriesOf"))

    def test_host_type_is_the_host_value(self):
        self.assertEqual({ENGINE}, self.values("draw", ("name", "c")))   # HTMLCanvasElement: the host's (H1)
        self.assertEqual(set(), self.values("draw", ("name", "p")))      # an alias of the program: erased (T1)

    def test_closure_write_is_read_where_written(self):
        # `shown` is the closure's parameter (a Box), not script.js's global function of that name
        held = self.values("collects", ("name", "all"))
        self.assertIn(ELEM + BOX, held)
        self.assertFalse(any(text.startswith((FN, ELEM + FN)) for text in held), held)

    def test_closure_sees_a_later_declaration(self):
        self.assertIn("Box.open", self.names("closures.ts:13"))         # run(): helper is declared after it

    def test_computed_key_names_its_members(self):
        self.assertIn("Box.open", self.names("pick"))                      # t["a"]: the field a
        self.assertEqual(set(), self.values("anyKey", ("name", "v")))      # an unknown key reads no property


if __name__ == "__main__":
    unittest.main()
