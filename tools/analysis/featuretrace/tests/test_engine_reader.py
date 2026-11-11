"""The engine's reader (log/trace-engine-plan.md, Task E2): every fixture's input file read by the engine is, value for
value, what Python reads from it (the canonical text both write); a pickle outside the engine's opcodes is refused;
a process past its deadline is stopped."""
from __future__ import annotations

import dataclasses
import pickle
import tempfile
import unittest
from pathlib import Path

from .support import FIXTURE
from .. import engine, engine_input, program

DEADLINE = 60                    # one fixture's dump; it takes well under a second
SETS = ((("heap/core.ts",), False), (("heap/contexts.js",), False), (("heap/protos.js",), False),
        (("heap/platform",), False), (("heap/summaries",), False),
        (("heap/jsxauto", "heap/jsxauto/node_modules/rt"), False), (("web", "web/node_modules/three"), True),
        (("jsvalues",), True))


def canonical(value) -> str:
    """The text tools/analysis/engine/src/dump.h describes, made from the Python value."""
    if value is None:
        return "N"
    if value is True or value is False:
        return "T" if value else "F"
    if isinstance(value, int):
        return f"i{value}"
    if isinstance(value, str):
        return "s" + value.encode("utf-8", "surrogatepass").hex()
    if isinstance(value, tuple):
        return "(" + ",".join(canonical(v) for v in value) + ")"
    if isinstance(value, list):
        return "[" + ",".join(canonical(v) for v in value) + "]"
    if isinstance(value, dict):
        return "{" + ",".join(f"{canonical(k)}:{canonical(v)}" for k, v in value.items()) + "}"
    if isinstance(value, frozenset):
        return "f<" + ",".join(sorted(canonical(v) for v in value)) + ">"
    if isinstance(value, set):
        return "<" + ",".join(sorted(canonical(v) for v in value)) + ">"
    if dataclasses.is_dataclass(value):
        name = f"{type(value).__module__}.{type(value).__qualname__}".encode().hex()
        return "@" + name + "{" + ",".join(f"{canonical(k)}:{canonical(v)}" for k, v in vars(value).items()) + "}"
    raise TypeError(type(value))


class EngineReader(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        engine.build(progress=lambda line: None)

    def dumped(self, data: bytes) -> str:
        with tempfile.TemporaryDirectory() as folder:
            source, out = Path(folder) / "input.pickle", Path(folder) / "dump.txt"
            source.write_bytes(data)
            engine.run(["dump", str(source), str(out)], DEADLINE, progress=lambda line: None)
            return out.read_bytes().decode("ascii")

    def test_every_fixture_input_reads_as_python_reads_it(self):
        for roots, declarations in SETS:
            with self.subTest(roots=roots):
                ix, _, _ = program.read([FIXTURE / r for r in roots], FIXTURE / "heap", None, declarations=declarations)
                with tempfile.TemporaryDirectory() as folder:
                    path = Path(folder) / "input.pickle"
                    engine_input.write(ix, path)
                    data = path.read_bytes()
                self.assertEqual(canonical(pickle.loads(data)), self.dumped(data))

    def test_the_values_at_the_edges_of_the_subset(self):
        value = (None, True, False, 0, -1, 255, 256, 65535, 65536, -(2 ** 31), 2 ** 40, -(2 ** 59), "", "\u00e9\U0001f600",
                 [], {}, set(), frozenset({1, "a"}), {"k": [(), {"n": {2}}]}, ("x",) * 300)
        data = pickle.dumps(value, protocol=engine_input.PROTOCOL)
        self.assertEqual(canonical(value), self.dumped(data))

    def test_an_opcode_outside_the_subset_is_refused(self):
        with self.assertRaises(engine.EngineError) as caught:
            self.dumped(pickle.dumps(1.5, protocol=engine_input.PROTOCOL))
        self.assertIn("outside the engine's subset", str(caught.exception))

    def test_a_process_past_its_deadline_is_stopped(self):
        with self.assertRaises(engine.EngineError) as caught:
            engine.watched(["cmake", "-E", "sleep", "30"], 0.5, progress=lambda line: None)
        self.assertIn("deadline", str(caught.exception))


if __name__ == "__main__":
    unittest.main()
