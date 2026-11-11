"""The native engine's input file (log/trace-engine-plan.md, Task E1; log/trace-research.md F18): written from a
fixture's index and read back, it is the units, the classes and the tables computed from that index; it uses only the
pickle opcodes the engine reads; and the tables hold what the readers resolve (a name's definition, an import's module,
a file's JSX runtime, a declared type's class or alias, the global intrinsic types, each constant from its owner)."""
from __future__ import annotations

import pickle
import pickletools
import tempfile
import unittest
from pathlib import Path

from .support import FIXTURE
from .. import engine_input, languages, program, seeds, vocab
from ..heap import analysis, values
from ..heap.typesys import ALIAS, CLASS
from ..model import Unit

# The opcodes of pickle protocol 5 the engine reads (Lib/pickletools.py names them; log/trace-research.md F18).
ENGINE_OPCODES = {
    "PROTO", "FRAME", "MEMOIZE", "BINGET", "LONG_BINGET", "MARK", "STOP", "NONE", "NEWTRUE", "NEWFALSE", "BININT",
    "BININT1", "BININT2", "LONG1", "SHORT_BINUNICODE", "BINUNICODE", "EMPTY_TUPLE", "TUPLE1", "TUPLE2", "TUPLE3", "TUPLE",
    "EMPTY_LIST", "APPEND", "APPENDS", "EMPTY_DICT", "SETITEM", "SETITEMS", "EMPTY_SET", "ADDITEMS", "FROZENSET",
    "STACK_GLOBAL", "NEWOBJ", "BUILD"}


class InputCase(unittest.TestCase):
    ROOTS = ()
    DECLARATIONS = False

    @classmethod
    def setUpClass(cls):
        cls.ix, _, _ = program.read([FIXTURE / r for r in cls.ROOTS], FIXTURE / "heap", None,
                                    declarations=cls.DECLARATIONS)
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder) / "input.pickle"
            engine_input.write(cls.ix, path)
            cls.data = path.read_bytes()
        cls.read_back = pickle.loads(cls.data)

    def part(self, name: str):
        return self.read_back[name]

    def test_the_file_is_the_tables_of_the_index(self):
        self.assertEqual(dict(zip(engine_input.ROOT_PARTS, engine_input.tables(self.ix))), self.read_back)

    def test_the_file_uses_only_what_the_engine_reads(self):
        used = {op.name for op, _, _ in pickletools.genops(self.data)}
        self.assertEqual(set(), used - ENGINE_OPCODES)

    def test_every_unit_is_in_order(self):
        units = self.part("units")
        self.assertEqual(list(range(len(self.ix.units))), [u.gid for u in units])

    def test_the_vocabulary_is_each_constant_from_its_owner(self):
        found = dict(self.part("vocabulary"))
        self.assertEqual(vocab.T_NAMED, found["vocab.T_NAMED"])
        self.assertEqual(languages.INTRINSICS, found["languages.INTRINSICS"])
        self.assertEqual("gid", found["model.Unit"][found["model.Unit"].index("gid")])
        self.assertIn("refs", found["model.Unit"])
        self.assertEqual(analysis.TREE, found["heap.analysis.TREE"])            # the new core's own tags
        self.assertEqual(values.NO_ENV, found["heap.values.NO_ENV"])
        self.assertNotIn("heap.values.READS", found)                           # a memo is no constant


class PlatformInput(InputCase):
    ROOTS = ("heap/platform",)

    def test_a_name_read_names_its_definition(self):
        rows = {(r[0], r[1]): r for r in self.part("names")}
        (pushed,), (apple,) = seeds.find(self.ix, "pushed"), seeds.find(self.ix, "Apple")
        self.assertIn(apple, rows[(pushed, "Apple")][2])     # `new Apple()`: the class declared in the module


class JsxInput(InputCase):
    ROOTS = ("heap/jsxauto", "heap/jsxauto/node_modules/rt")

    def test_the_jsx_runtime_is_resolved(self):
        rows = self.part("jsx")
        self.assertEqual(1, len(rows))
        self.assertTrue(rows[0][2], rows[0])                       # the module the automatic transform imports


class WebInput(InputCase):
    ROOTS = ("web", "web/node_modules/three")
    DECLARATIONS = True

    def test_a_declared_type_names_its_class_or_alias(self):
        resolved = [r[3] for r in self.part("types") if r[3]]
        self.assertTrue(resolved)
        for kind, target in resolved:
            self.assertIn(kind, (ALIAS, CLASS))
            self.assertTrue(target in self.ix.classes if kind == CLASS else 0 <= target < len(self.ix.units))

    def test_an_import_leads_to_its_module(self):
        imports = [i for m in self.part("modules") for i in m[2]]
        self.assertTrue(imports)
        self.assertTrue(all(target is not None for _, target in imports), imports)

    def test_the_intrinsic_types_are_global_classes(self):
        found = [key for _, keys in self.part("intrinsics") for key in keys if key is not None]
        self.assertTrue(found)
        self.assertTrue(all(key in self.ix.classes for key in found))

    def test_the_units_are_the_index_units(self):
        self.assertTrue(all(isinstance(u, Unit) for u in self.part("units")))


class ValuesInput(InputCase):
    ROOTS = ("jsvalues",)
    DECLARATIONS = True

    def test_each_operator_and_literal_gives_the_language_values(self):
        operators = {(op, unary): kinds for op, unary, kinds in self.part("operators")}
        self.assertIn("string", operators[("+", False)])                      # `+` concatenates too
        literals = dict(self.part("literals"))
        self.assertEqual(["undefined"], literals["undefined"])


class JsxClassicInput(InputCase):
    ROOTS = ("heap/jsxclassic",)

    def test_the_classic_factory_is_a_name_read_where_the_element_is(self):
        heads = {name for _, name, *_ in self.part("names")}
        self.assertTrue(heads & {"React", "h"}, heads)


if __name__ == "__main__":
    unittest.main()
