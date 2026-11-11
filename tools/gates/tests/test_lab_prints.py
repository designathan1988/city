"""Tests of the roadcraft's fingerprint store (tools/gates/lab_prints.py), its reading (tools/gates/lab_tokens.py) and its
units (tools/gates/lab_units.py): the language-neutral tokens three languages give and what `flatten` takes out, a list's
repeated pattern, the winnowed fingerprints, function bodies and the names a source defines for others, a data table's keys,
a program held in a string, and the store built from a reference tree.
Run: python -m unittest discover -s tools/gates/tests -p "test_lab_*.py" -v"""
import os
import sys
import tempfile
import unittest

GATES = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
if GATES not in sys.path:
    sys.path.insert(0, GATES)

import lab_prints  # noqa: E402
import lab_tokens  # noqa: E402
import lab_units  # noqa: E402
from source import Source  # noqa: E402

REF = os.path.join(os.path.dirname(os.path.abspath(__file__)), "lab", "ref")
CFG = {"k": 12, "w": 8, "min_name_len": 8, "trivial_numbers": [0, 1, 2, -1, 0.5, 100, 1000]}


def source(name, text):
    root = tempfile.mkdtemp()
    with open(os.path.join(root, name), "w", encoding="utf-8", newline="\n") as fh:
        fh.write(text)
    return Source(root, name)


def neutral(name, text):
    return [t for t, _line in lab_tokens.neutral_tokens(source(name, text))]


def flat(name, text):
    """The flattened body of the only function of `text`."""
    return [t for t, _line in lab_units.spans(source(name, text), set())[0].tokens]


class NeutralTokens(unittest.TestCase):
    def test_neutral_tokens_drop_types(self):
        want = ["D", "I", "=", "N:3.5", ";"]
        self.assertEqual(neutral("a.ts", "const a: number = 3.5;"), want)
        self.assertEqual(neutral("a.cpp", "const double a = 3.5;"), want)
        self.assertEqual(neutral("a.cpp", "std::vector<Loaf> a = 3.5;"), want)  # a C++ type is a declaration word
        self.assertEqual(neutral("a.ts", "const a = b as Loaf;"), neutral("a.ts", "const a = b;"))
        self.assertEqual(neutral("a.ts", "f(b!);"), neutral("a.ts", "f(b);"))

    def test_keywords_map_across_languages(self):
        self.assertEqual(neutral("a.ts", "if (a === b) { return x; }"), neutral("a.cpp", "if (a == b) { return x; }"))
        self.assertEqual(neutral("a.gd", "elif a == b:\n\tpass\n")[:2], ["else", "if"])
        heads = [neutral("a.ts", "function f(x) {}")[:3], neutral("a.gd", "func f(x):\n\tpass\n")[:3],
                 neutral("a.cpp", "double f(double x) {}")[:3]]
        self.assertEqual(heads, [["F", "I", "("]] * 3)

    def test_range_loop_kept_in_cpp(self):
        self.assertIn("xs", "".join(t.text for t in source("a.cpp", "for (auto x : xs) {}").code()))
        self.assertEqual(neutral("a.cpp", "for (auto x : xs) {}")[:8], ["for", "(", "D", "I", ":", "I", ")", "{"])

    def test_flatten_reads_three_languages_alike(self):
        ts = flat("a.ts", "function f(xs) { for (const x of xs) { if (this.ok) { this.n += x.size; } } }")
        cpp = flat("a.cpp", "void f(const std::vector<Item>& xs) { for (const Item& x : xs) { if (ok) { n += x.size(); } } }")
        gd = flat("a.gd", "func f(xs):\n\tfor x in xs:\n\t\tif self.ok:\n\t\t\tself.n += x.size()\n")
        self.assertEqual(ts, ["for", "I", "in", "I", "if", "I", "I", "+=", "I", ".", "I"])
        self.assertEqual(cpp, ts)
        self.assertEqual(gd, ts)


class Fingerprints(unittest.TestCase):
    def test_fingerprints_stable(self):
        stream = [(t, i // 4 + 1) for i, t in enumerate(["D", "I", "=", "N:3.5", ";"] * 10)]
        first = lab_tokens.fingerprints(stream, CFG["k"], CFG["w"])
        self.assertTrue(first)
        self.assertEqual(first, lab_tokens.fingerprints(stream, CFG["k"], CFG["w"]))
        other = [(t.replace("3.5", "4.5"), line) for t, line in stream]
        self.assertNotEqual({h for h, _ in first}, {h for h, _ in lab_tokens.fingerprints(other, CFG["k"], CFG["w"])})

    def test_list_pattern_is_no_information(self):
        stream = [(t, 1) for t in ["I", ".", "I", "=", "I"] * 6]
        self.assertTrue(lab_tokens.kgrams(stream, CFG["k"]))
        self.assertEqual(lab_tokens.kgrams(stream, CFG["k"], informative=True), [])


class Names(unittest.TestCase):
    def test_normal_name(self):
        for name in ("openStretch", "open_stretch", "OpenStretch"):
            self.assertEqual(lab_tokens.normal_name(name, CFG["min_name_len"]), "openstretch")
        self.assertIsNone(lab_tokens.normal_name("lane", CFG["min_name_len"]))
        self.assertIsNone(lab_tokens.normal_name("stretching", CFG["min_name_len"]))  # one part

    def test_api_names_ts(self):
        names = {n for n, _line in lab_units.api_names(Source(REF, "stations.ts"))}
        self.assertTrue({"SPAN_LIMIT", "evenStations", "StationRow", "poleSpacing", "stationAt"} <= names, names)

    def test_locals_and_parameters_are_not_api(self):
        names = {n for n, _line in lab_units.api_names(Source(REF, "oven.ts"))}
        self.assertTrue({"batchMinutes", "nextBatch", "rackFree", "weightGrams", "RACK_SLOTS"} <= names, names)
        self.assertFalse({"baseMinutes", "freedSlots", "capacity", "heaviest"} & names, names)
        cpp = {n for n, _l in lab_units.api_names(source("a.cpp", "struct S { float lane_width; };\n"
                                                                   "float f(float half_width) { float local_width = 1; return local_width; }\n"
                                                                   "enum class Kind { RoadSide, KerbLine };\n"))}
        self.assertEqual(cpp, {"S", "lane_width", "f", "Kind", "RoadSide", "KerbLine"})

    def test_data_keys(self):
        self.assertEqual(lab_units.data_keys('{\n  "rack_slots": 14,\n  "nested": {"crust_factor": 1}\n}\n'),
                         [("rack_slots", 2), ("nested", 3), ("crust_factor", 3)])


class Units(unittest.TestCase):
    def test_ts_units(self):
        found = lab_units.units(Source(REF, "oven.ts"))
        self.assertEqual([n for n, _f, _l in found], ["batchMinutes", "enqueueLoaf", "nextBatch", "coolingSlots"])
        arrows = lab_units.units(source("a.ts", "const f = (x: number): number => {\n  return x;\n};\nif (a) {\n  b();\n}\n"))
        self.assertEqual(arrows, [("f", 1, 3)])

    def test_program_in_a_string(self):
        spans = lab_units.spans(Source(REF, "oven.ts"), set())
        shader = [s for s in spans if s.line == 49]
        self.assertEqual(len(shader), 1)
        self.assertIn(2.6, shader[0].values)
        self.assertIn("glazeDepth", {n for n, _l in lab_units.program_names(Source(REF, "oven.ts"))})


class Store(unittest.TestCase):
    def test_build_store_indexes_names_and_prints(self):
        store = lab_prints.build_store(REF, {"dirs": [], "skip_dirs": []}, CFG)
        self.assertIn("evenstations", store.names)
        self.assertIn("stations.ts", store.names["evenstations"][0])
        self.assertTrue(store.prints)
        self.assertTrue(any(31.25 in values for values in store.numbers.values()), store.numbers)
        self.assertEqual(store.unit_tokens("oven.ts:11")[:3], ["D", "I", "="])
        again = lab_prints.Store.from_json(store.to_json())
        self.assertEqual((again.names, again.numbers, again.tokens), (store.names, store.numbers, store.tokens))


if __name__ == "__main__":
    unittest.main()
