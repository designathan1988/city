"""Tests of the roadcraft's fingerprint store (tools/gates/lab_prints.py) and its reading (tools/gates/lab_tokens.py): the
language-neutral tokens three languages give, the winnowed fingerprints, the names a file defines and their normal form, the
store built from a reference tree. Run: python -m unittest discover -s tools/gates/tests -p "test_lab_*.py" -v"""
import os
import sys
import tempfile
import unittest

GATES = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
if GATES not in sys.path:
    sys.path.insert(0, GATES)

import lab_prints  # noqa: E402
import lab_tokens  # noqa: E402
from source import Source  # noqa: E402

REF = os.path.join(os.path.dirname(os.path.abspath(__file__)), "lab", "ref")
CFG = {"k": 12, "w": 8, "min_name_len": 8, "number_run": 3, "trivial_numbers": [0, 1, 2, -1, 0.5, 100, 1000]}


def neutral(name, text):
    """The neutral token texts of `text` read as the file `name`."""
    with tempfile.TemporaryDirectory() as root:
        with open(os.path.join(root, name), "w", encoding="utf-8", newline="\n") as fh:
            fh.write(text)
        return [t for t, _line in lab_tokens.neutral_tokens(Source(root, name))]


class NeutralTokens(unittest.TestCase):
    def test_neutral_tokens_drop_types(self):
        want = ["D", "I", "=", "N:3.5", ";"]
        self.assertEqual(neutral("a.ts", "const a: number = 3.5;"), want)
        self.assertEqual(neutral("a.cpp", "const double a = 3.5;"), want)

    def test_keywords_map_across_languages(self):
        self.assertEqual(neutral("a.ts", "if (a === b) { return x; }"), neutral("a.cpp", "if (a == b) { return x; }"))
        self.assertEqual(neutral("a.gd", "elif a == b:\n\tpass\n")[:2], ["else", "if"])
        heads = [neutral("a.ts", "function f(x) {}")[:3], neutral("a.gd", "func f(x):\n\tpass\n")[:3],
                 neutral("a.cpp", "double f(double x) {}")[:3]]
        self.assertEqual(heads, [["F", "I", "("]] * 3)


class Fingerprints(unittest.TestCase):
    def test_fingerprints_stable(self):
        stream = [(t, i // 4 + 1) for i, t in enumerate(["D", "I", "=", "N:3.5", ";"] * 10)]
        first = lab_tokens.fingerprints(stream, CFG["k"], CFG["w"])
        self.assertTrue(first)
        self.assertEqual(first, lab_tokens.fingerprints(stream, CFG["k"], CFG["w"]))
        other = [(t.replace("3.5", "4.5"), line) for t, line in stream]
        self.assertNotEqual({h for h, _ in first}, {h for h, _ in lab_tokens.fingerprints(other, CFG["k"], CFG["w"])})


class Names(unittest.TestCase):
    def test_normal_name(self):
        for name in ("openStretch", "open_stretch", "OpenStretch"):
            self.assertEqual(lab_tokens.normal_name(name, CFG["min_name_len"]), "openstretch")
        self.assertIsNone(lab_tokens.normal_name("lane", CFG["min_name_len"]))
        self.assertIsNone(lab_tokens.normal_name("stretching", CFG["min_name_len"]))  # one part

    def test_defined_names_ts(self):
        names = lab_tokens.defined_names(Source(REF, "stations.ts"))
        self.assertTrue({"SPAN_LIMIT", "evenStations", "StationRow", "poleSpacing", "stationAt"} <= names, names)


class Store(unittest.TestCase):
    def test_build_store_indexes_names_and_prints(self):
        store = lab_prints.build_store(REF, CFG)
        self.assertIn("evenstations", store.names)
        self.assertIn("stations.ts", store.names["evenstations"][0])
        self.assertTrue(store.prints)
        self.assertTrue(any(31.25 in key for key in store.numbers), store.numbers)


if __name__ == "__main__":
    unittest.main()
