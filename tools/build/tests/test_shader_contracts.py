# shader_contracts: each kind of entry written the same on both sides (an int, a float as its expression, an enum's values
# in order with the shaders' prefix and type), a value that is neither a number nor an expression refused, and --check
# failing on an output that is missing or not what the table makes. Run: python -m unittest discover tools/build/tests
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

TABLE = {
    "entries": [
        {"name": "STEPS", "type": "int", "value": 31, "doc": "an int"},
        {"name": "THIRD", "type": "float", "value": "1.0 / 3.0", "doc": "a float as an expression"},
        {"name": "Kind", "type": "enum", "cpp": "enum Kind : uint8_t", "values": ["K_A", "K_B"], "count": "K_COUNT", "doc": "plain"},
        {"name": "Part", "type": "enum", "cpp": "enum class Part : int", "values": ["BODY", "CARD"], "shader_prefix": "PART_",
         "shader_type": "float", "doc": "scoped, read as floats"},
    ]
}


class ShaderContractsTest(unittest.TestCase):
    def test_header_writes_each_kind(self):
        text = shader_contracts.header(TABLE)
        self.assertIn("constexpr int STEPS = 31;", text)
        self.assertIn("constexpr float THIRD = float(1.0 / 3.0);", text)
        self.assertIn("enum Kind : uint8_t { K_A, K_B, K_COUNT };", text)
        self.assertIn("enum class Part : int { BODY, CARD };", text)
        self.assertIn('{ "STEPS", double(STEPS) },', text)
        self.assertIn('{ "THIRD", double(1.0 / 3.0) },', text)  # the scripts get the decimal, not its float
        self.assertIn('{ "K_COUNT", double(K_COUNT) },', text)
        self.assertIn('{ "PART_CARD", double(Part::CARD) },', text)

    def test_include_writes_each_kind_guarded(self):
        text = shader_contracts.include(TABLE)
        self.assertIn("#ifndef SHADER_CONTRACTS_INCLUDED\n#define SHADER_CONTRACTS_INCLUDED", text)
        self.assertIn("const int STEPS = 31;", text)
        self.assertIn("const float THIRD = 1.0 / 3.0;", text)
        self.assertIn("const int K_A = 0;\n", text)
        self.assertIn("const int K_COUNT = 2;\n", text)
        self.assertIn("const float PART_BODY = 0.0;\nconst float PART_CARD = 1.0;\n", text.split("// scoped, read as floats\n")[1])
        self.assertTrue(text.rstrip().endswith("#endif"))

    def test_refuses_what_is_not_a_value(self):
        for value in (True, None, [1]):
            with self.subTest(value=value), self.assertRaises(ValueError):
                shader_contracts.header({"entries": [{"name": "X", "type": "int", "value": value, "doc": ""}]})
        with self.assertRaises(ValueError):
            shader_contracts.header({"entries": [{"name": "X", "type": "vec3", "value": 1, "doc": ""}]})


class CheckTest(unittest.TestCase):
    def setUp(self):
        base = Path(tempfile.mkdtemp(prefix="shader-contracts-test-"))
        self.addCleanup(shutil.rmtree, base, True)
        (base / "table.json").write_text(json.dumps(TABLE), encoding="utf-8")
        for name, path in (("ROOT", base), ("TABLE", base / "table.json"), ("HEADER", base / "h.h"), ("INCLUDE", base / "i.inc")):
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
        self.assertIn("stale: h.h", out)
        self.assertIn("stale: i.inc", out)

    def test_written_outputs_pass_and_an_edit_fails(self):
        self.assertEqual(self.run_main()[0], 0)
        self.assertEqual(self.run_main("--check"), (0, ""))
        header = self.base / "h.h"
        header.write_text(header.read_text(encoding="utf-8").replace("= 31;", "= 32;"), encoding="utf-8")
        code, out = self.run_main("--check")
        self.assertEqual(code, 1)
        self.assertEqual(out.strip(), "stale: h.h (python tools/build/shader_contracts.py)")


if __name__ == "__main__":
    unittest.main()
