"""JavaScript and TypeScript values by the languages' own rules (log/trace-rules.md, "JavaScript and TypeScript values
by the languages' own rules": L1, L2, T1-T3, O1, N1), checked on the links they make and the values the typer holds."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))
sys.path.insert(0, str(HERE))

from js_expectations import JsValuesExpectations  # noqa: E402
from support import FixtureCase  # noqa: E402
from featuretrace import js_library  # noqa: E402
from featuretrace.solver import BIGINT, CLASS, ELEM, ENGINE, FN, LANG, NULL, NUMBER, TEXT  # noqa: E402

BOX = CLASS + "types.ts#Box"


class JsValues(FixtureCase, JsValuesExpectations):
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"

    def values(self, spec: str, tree: tuple) -> set:
        unit = self.ix.units[self.gid(spec)]
        return {text for text, _ in self.rs.typer.evaluate(tree, unit)}

    def test_union_with_null_is_the_class(self):
        self.assertEqual({CLASS + "types.ts#Building", NULL}, self.values("levelOf", ("name", "b")))   # and null (T1')

    def test_type_parameters_are_erased(self):
        self.assertEqual(set(), self.values("levelOf", ("name", "t")))

    def test_declared_return_gives_elements(self):
        self.assertIn("Box.open", self.names("walk"))

    def test_js_arithmetic_is_a_primitive(self):
        self.assertEqual({NUMBER, BIGINT}, self.values("scaled", ("name", "n")))        # a number or a BigInt (T1'')

    def test_values_do_not_depend_on_the_file(self):
        typer = self.rs.typer
        returned = [text for text, _ in typer.current(typer.ret_node(self.ix.units[self.gid("allBoxes")]))]
        self.assertEqual(1, returned.count(f"Array<{BOX}>"))   # one value from both files
        self.assertEqual({f"Array<{BOX}>", js_library.EMPTY_ARRAY}, set(returned))   # and the array literals `[]`
                                                                                      # (T2'')

    def test_no_annotation_text_is_a_value(self):
        for node, held in self.rs.typer.value.items():
            for text, _ in held:
                core = text
                while core.startswith(ELEM):
                    core = core[len(ELEM):]
                allowed = core == ENGINE or core.startswith((CLASS, FN, TEXT, LANG)) or js_library.is_container(core)
                self.assertTrue(allowed, f"{text!r} in {str(node)[:80]}")

    def test_host_type_is_the_host_value(self):
        self.assertEqual({ENGINE}, self.values("draw", ("name", "c")))   # HTMLCanvasElement: the host's (H1)
        point = self.values("draw", ("name", "p"))                       # an alias of the program: what its type admits,
        self.assertEqual(1, len(point), point)                           # here an object type (T1')
        self.assertTrue(next(iter(point)).startswith(CLASS + "props.ts#<type@"), point)

    def test_closure_write_is_read_where_written(self):
        # `shown` is the closure's parameter (a Box), not script.js's global function of that name
        held = self.values("collects", ("name", "all"))
        self.assertIn(ELEM + BOX, held)
        self.assertFalse(any(text.startswith((FN, ELEM + FN)) for text in held), held)

    def test_an_unknown_key_reads_no_property(self):
        self.assertEqual(set(), self.values("anyKey", ("name", "v")))      # an unknown key reads no property


if __name__ == "__main__":
    unittest.main()
