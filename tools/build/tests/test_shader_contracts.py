# shader_contracts: each kind of entry written the same on both sides (an int, a float as its expression, a vec3, named sets,
# an enum's values in order from its start with the shaders' prefix and type, in its C++ namespace), a group's file with only its
# own values and all.h with every group, a name given twice refused, a value that is neither a number nor an expression refused,
# and --check failing on an output that is missing, edited, or not the table's at all.
# Run: python -m unittest discover tools/build/tests
import json
import shutil
import sys
import tempfile
import unittest
from contextlib import redirect_stdout
from io import StringIO
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
import shader_contracts  # noqa: E402

GROUP = {
    "doc": "a test group",
    "entries": [
        {"name": "STEPS", "type": "int", "value": 31, "doc": "an int"},
        {"name": "THIRD", "type": "float", "value": "1.0 / 3.0", "doc": "a float as an expression"},
        {"name": "WEIGHTS", "type": "vec3", "value": ["0.3", "0.6", "0.1"], "doc": "a vec3"},
        {"name": "Bits", "type": "ints", "values": {"LOW_BITS": 3, "TOP_BIT": 128}, "doc": "a set of ints"},
        {"name": "Marks", "type": "floats", "values": {"MARK_A": "1.0", "MARK_B": "2.0"}, "doc": "a set of floats"},
        {"name": "Kind", "type": "enum", "cpp": "enum Kind : uint8_t", "values": ["K_A", "K_B"], "count": "K_COUNT", "doc": "plain"},
        {"name": "Part", "type": "enum", "cpp": "enum class Part : int", "values": ["BODY", "CARD"], "shader_prefix": "PART_",
         "shader_type": "float", "doc": "scoped, read as floats"},
        {"name": "Layer", "type": "enum", "cpp": "enum Layer : int8_t", "namespace": "b40", "start": -1, "values": ["L_NONE", "L_ONE"],
         "count": "L_COUNT", "doc": "from -1, in a namespace"},
    ],
}
OTHER = {"doc": "another group", "entries": [{"name": "SPAN_M", "type": "float", "value": "4.0", "doc": "its own"}]}
TABLE = {"groups": {"first": GROUP, "second": OTHER}}


class ShaderContractsTest(unittest.TestCase):
    def test_group_header_writes_each_kind(self):
        text = shader_contracts.group_header("first", GROUP)
        self.assertIn('#include "core/shader_contracts/contract.h"', text)
        self.assertIn("constexpr int STEPS = 31;", text)
        self.assertIn("constexpr float THIRD = float(1.0 / 3.0);", text)
        self.assertIn("constexpr float WEIGHTS[3] = { float(0.3), float(0.6), float(0.1) };", text)
        self.assertIn("constexpr int LOW_BITS = 3;\nconstexpr int TOP_BIT = 128;", text)
        self.assertIn("constexpr float MARK_B = float(2.0);", text)
        self.assertIn("enum Kind : uint8_t { K_A, K_B, K_COUNT };", text)
        self.assertIn("enum class Part : int { BODY, CARD };", text)
        self.assertIn("namespace b40 { enum Layer : int8_t { L_NONE = -1, L_ONE, L_COUNT }; }", text)
        self.assertIn("constexpr ShaderContract SHADER_CONTRACTS_FIRST[] = {", text)
        self.assertIn('{ "STEPS", 1, { double(STEPS) } },', text)
        self.assertIn('{ "THIRD", 1, { double(1.0 / 3.0) } },', text)  # the scripts get the decimal, not its float
        self.assertIn('{ "WEIGHTS", 3, { double(WEIGHTS[0]), double(WEIGHTS[1]), double(WEIGHTS[2]) } },', text)  # a Vector3 holds floats
        self.assertIn('{ "PART_CARD", 1, { double(Part::CARD) } },', text)
        self.assertIn('{ "L_NONE", 1, { double(b40::L_NONE) } },', text)
        self.assertNotIn("SPAN_M", text)

    def test_group_include_writes_each_kind_guarded(self):
        text = shader_contracts.group_include("first", GROUP)
        self.assertIn("#ifndef CONTRACTS_FIRST_INCLUDED\n#define CONTRACTS_FIRST_INCLUDED", text)
        self.assertIn("const int STEPS = 31;", text)
        self.assertIn("const float THIRD = 1.0 / 3.0;", text)
        self.assertIn("const vec3 WEIGHTS = vec3(0.3, 0.6, 0.1);", text)
        self.assertIn("const int TOP_BIT = 128;", text)
        self.assertIn("const float MARK_A = 1.0;", text)
        self.assertIn("const int K_COUNT = 2;\n", text)
        self.assertIn("const float PART_BODY = 0.0;\nconst float PART_CARD = 1.0;\n", text)
        self.assertIn("const int L_NONE = -1;\nconst int L_ONE = 0;\nconst int L_COUNT = 1;\n", text)
        self.assertTrue(text.rstrip().endswith("#endif"))

    def test_all_gathers_every_group(self):
        text = shader_contracts.all_header(TABLE["groups"])
        self.assertIn('#include "core/shader_contracts/first.h"\n#include "core/shader_contracts/second.h"', text)
        self.assertIn("{ SHADER_CONTRACTS_SECOND, std::size(SHADER_CONTRACTS_SECOND) },", text)

    def test_refuses_a_name_twice(self):
        twice = {"doc": "", "entries": [{"name": "K_A", "type": "int", "value": 1, "doc": ""}]}
        with self.assertRaises(ValueError):
            shader_contracts.outputs({"groups": {"first": GROUP, "again": twice}})

    def test_refuses_what_is_not_a_value(self):
        def one(entry):
            return {"groups": {"g": {"doc": "", "entries": [entry]}}}
        for value in (True, None, [1]):
            with self.subTest(value=value), self.assertRaises(ValueError):
                shader_contracts.outputs(one({"name": "X", "type": "int", "value": value, "doc": ""}))
        with self.assertRaises(ValueError):
            shader_contracts.outputs(one({"name": "X", "type": "mat3", "value": 1, "doc": ""}))
        with self.assertRaises(ValueError):
            shader_contracts.outputs(one({"name": "X", "type": "vec3", "value": ["1.0", "2.0"], "doc": ""}))


class CheckTest(unittest.TestCase):
    def setUp(self):
        base = Path(tempfile.mkdtemp(prefix="shader-contracts-test-"))
        self.addCleanup(shutil.rmtree, base, True)
        (base / "table.json").write_text(json.dumps(TABLE), encoding="utf-8")
        for name, path in (("ROOT", base), ("TABLE", base / "table.json"), ("HEADERS", base / "h"), ("INCLUDES", base / "i")):
            self.addCleanup(setattr, shader_contracts, name, getattr(shader_contracts, name))
            setattr(shader_contracts, name, path)
        self.base = base

    def run_main(self, *argv):
        with redirect_stdout(StringIO()) as out:
            code = shader_contracts.main(list(argv))
        return code, out.getvalue()

    def test_missing_outputs_fail(self):
        code, out = self.run_main("--check")
        self.assertEqual(code, 1)
        self.assertIn("stale: h/first.h", out)
        self.assertIn("stale: i/second.gdshaderinc", out)

    def test_written_outputs_pass_and_an_edit_fails(self):
        self.assertEqual(self.run_main()[0], 0)
        self.assertEqual(self.run_main("--check"), (0, ""))
        header = self.base / "h" / "first.h"
        header.write_text(header.read_text(encoding="utf-8").replace("= 31;", "= 32;"), encoding="utf-8")
        code, out = self.run_main("--check")
        self.assertEqual(code, 1)
        self.assertEqual(out.strip(), "stale: h/first.h (python tools/build/shader_contracts.py)")

    def test_a_file_the_table_does_not_make_fails(self):
        self.assertEqual(self.run_main()[0], 0)
        (self.base / "i" / "first.gdshaderinc.uid").write_text("uid://x", encoding="utf-8")  # Godot's own, beside an include
        self.assertEqual(self.run_main("--check"), (0, ""))
        (self.base / "h" / "gone.h").write_text("// a group the table no longer has", encoding="utf-8")
        code, out = self.run_main("--check")
        self.assertEqual(code, 1)
        self.assertEqual(out.strip(), "stale: h/gone.h (python tools/build/shader_contracts.py)")


if __name__ == "__main__":
    unittest.main()
