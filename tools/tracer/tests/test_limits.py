"""The limits a port met, each on programs made up on every run (random names, random numbers): `**` worked out in
every language that writes it; what a start gives back reported and compared (`return`), and a comparison of nothing
said so; the unknown a choice waits on named; a property's getter and setter written in place read as functions (no
link said to lead nowhere through them); a class's name read as a global not taken for state; a box of no variable
with a term of no value said undecided, not impossible; a point's file matched at a folder's boundary, a file alone
taken as a point; a feature's cost kept to its own functions.
"""
from __future__ import annotations

import json
import os
import random
import shutil
import tempfile
import unittest
from fractions import Fraction
from pathlib import Path

from support import HERE, analyse
from generate import Names
from tracer import compare, cost, feature, links
from tracer.decide import BOUND, EQ, Literal, nonlinear
from tracer.evaluate import evaluate, plain, run_data, run_text
from tracer.vocab import TEXT, UNDECIDED
from tracer.walk import find

SEED = int(os.environ.get("TRACER_SEED") or random.SystemRandom().randrange(1 << 30))


class Folder:
    def __enter__(self):
        self.path = Path(tempfile.mkdtemp(prefix=".limits-", dir=HERE))
        return self.path

    def __exit__(self, *_):
        shutil.rmtree(self.path, ignore_errors=True)


def ran(folder: Path, start: str, args: list, situation: dict | None = None):
    r = analyse([folder], folder, keep=True)
    found = find(r, start)
    assert found, f"nothing is named {start}"
    return r, evaluate(r, [(found[0], args)], situation or {})


class Power(unittest.TestCase):
    def test_the_power_operator_is_worked_out_and_given_back(self):
        rng = random.Random(SEED)
        for language, write in (("ts", lambda f, k: f"export function {f}(x: number): number {{\n  return x ** {k};\n}}\n"),
                                ("gd", lambda f, k: f"extends Node\n\n\nfunc {f}(x):\n\treturn x ** {k}\n")):
            n = Names(rng)
            f, k, x = n(), rng.randint(2, 5), rng.randint(2, 9)
            with Folder() as folder:
                (folder / f"power.{language}").write_text(write(f, k), encoding="utf-8")
                r, run = ran(folder, f, [x])
                with self.subTest(seed=SEED, language=language, x=x, k=k):
                    self.assertEqual(x ** k, plain(run.returned), [why for _, why in run.partial])
                    self.assertIn(f"Returned: {x ** k!r}", run_text(r, run))
                    self.assertEqual(x ** k, run_data(r, run)["what it gave back"])


class Returned(unittest.TestCase):
    def maps(self, folder: Path, one: str, two: str, outputs: list) -> tuple:
        for name, text in (("one", one), ("two", two)):
            (folder / name).mkdir()
            (folder / name / "rule.ts").write_text(text, encoding="utf-8")
            (folder / f"{name}.json").write_text(json.dumps({"root": name, "start": self.start, "inputs": {"v": "v"},
                                                             "outputs": outputs}), encoding="utf-8")
        return str(folder / "one.json"), str(folder / "two.json")

    def test_what_two_pure_functions_give_back_is_compared(self):
        rng = random.Random(SEED + 1)
        for moved in (False, True):
            n = Names(rng)
            self.start, k = n(), rng.randint(2, 9)
            other = k + rng.randint(1, 3) if moved else k
            one = f"export function {self.start}(v: number): number {{\n  return v * {k} + 1;\n}}\n"
            two = f"export function {self.start}(v: number): number {{\n  const w = {other} * v;\n  return 1 + w;\n}}\n"
            with Folder() as folder:
                done = compare.compare(*self.maps(folder, one, two, [compare.RETURN_OUTPUT]), {}, ("v", 0.0, 10.0, 11))
                kinds = {kind for band in done.bands for kind, _, _ in band.found}
                with self.subTest(seed=SEED, moved=moved):
                    self.assertNotIn(compare.NOTHING, kinds)
                    self.assertEqual({"other values"} if moved else set(), kinds)

    def test_a_comparison_of_nothing_is_never_the_same(self):
        rng = random.Random(SEED + 2)
        n = Names(rng)
        self.start, absent = n(), n()
        body = f"export function {self.start}(v: number): number {{\n  return v;\n}}\n"
        with Folder() as folder:
            done = compare.compare(*self.maps(folder, body, body, [absent]), {"v": 1})
            self.assertEqual([[(compare.NOTHING, None, None)]], [band.found for band in done.bands])
            self.assertIn(compare.NOTHING, compare.text_report(done))

    def test_a_comparison_of_a_partial_run_is_never_the_same(self):
        rng = random.Random(SEED + 8)
        n = Names(rng)
        self.start, limit = n(), n().upper()
        known = f"export function {self.start}(v: number): number {{\n  return v * 2;\n}}\n"
        waits = f"declare const {limit}: number;\nexport function {self.start}(v: number): number {{\n  return v * {limit};\n}}\n"
        with Folder() as folder:
            done = compare.compare(*self.maps(folder, known, waits, [compare.RETURN_OUTPUT]), {"v": 3})
            kinds = {kind for band in done.bands for kind, _, _ in band.found}
            with self.subTest(seed=SEED):
                self.assertIn(compare.PARTIAL, kinds)
                self.assertIn(limit, compare.text_report(done))


class Unknowns(unittest.TestCase):
    def test_a_branch_on_a_value_defined_elsewhere_names_it(self):
        rng = random.Random(SEED + 3)
        for file, write in (
                ("gate.ts", lambda f, k: f"declare const {k}: number;\nexport function {f}(x: number): number {{\n"
                                         f"  if (x > {k}) {{\n    return 1;\n  }}\n  return 0;\n}}\n"),
                ("gate.cpp", lambda f, k: f"extern const float {k};\nfloat {f}(float x) {{\n"
                                          f"  if (x > {k}) {{\n    return 1.f;\n  }}\n  return 0.f;\n}}\n")):
            n = Names(rng)
            f, limit, x = n(), n().upper(), rng.randint(1, 9)
            with Folder() as folder:
                (folder / file).write_text(write(f, limit), encoding="utf-8")
                r, run = ran(folder, f, [x])
                said = " ".join(why for _, why in run.partial)
                given = evaluate(r, [(find(r, f)[0], [x])], {limit: x - 1})
                with self.subTest(seed=SEED, file=file, said=said):
                    self.assertIn("unknown value", said)
                    self.assertIn(limit, said)
                    self.assertEqual(1, plain(given.returned))
                    self.assertEqual([], [b for b in links.broken(r) if b.what.endswith(limit)])


class Properties(unittest.TestCase):
    def test_a_getter_written_in_place_reads_and_writes_what_it_names(self):
        rng = random.Random(SEED + 4)
        n = Names(rng)
        holder, shown, value, stale, compute, mark = n(True), n(), n(), n(), n(), n()
        text = (f"class_name {holder}\nextends Node\n\nvar {shown}: int:\n\tget:\n\t\tif _{stale}:\n"
                f"\t\t\t_{stale} = false\n\t\t\t_{value} = {compute}()\n\t\treturn _{value}\n"
                f"var _{value} := 0\nvar _{stale} := false\n\n\nfunc {compute}() -> int:\n\treturn 3\n\n\n"
                f"func {mark}() -> void:\n\t_{stale} = true\n")
        user = f"extends Node\n\n\nfunc use(h: {holder}) -> int:\n\th.{mark}()\n\treturn h.{shown}\n"
        with Folder() as folder:
            (folder / "holder.gd").write_text(text, encoding="utf-8")
            (folder / "user.gd").write_text(user, encoding="utf-8")
            r = analyse([folder], folder, keep=True)
            named = [(b.kind, b.what) for b in links.broken(r)
                     if any(b.what.endswith(x) for x in (shown, f"_{value}", f"_{stale}", holder))]
            with self.subTest(seed=SEED):
                self.assertEqual([], named)


class Decide(unittest.TestCase):
    def test_a_box_of_no_variable_with_a_term_of_no_value_is_undecided(self):
        term = (TEXT, "a value the code read does not give")
        rows = (Literal(BOUND, (("t", Fraction(1)),), const=Fraction(0), rel=EQ),)
        self.assertEqual(UNDECIDED, nonlinear(rows, {"t": term}, {}, None))


class Points(unittest.TestCase):
    def test_a_file_is_matched_at_a_folders_boundary(self):
        self.assertTrue(feature.same_file("roads/edge.cpp", "C:/game/sim/roads/edge.cpp"))
        self.assertTrue(feature.same_file("sim/roads/edge.cpp", "roads/edge.cpp"))
        self.assertFalse(feature.same_file("roads/edge.cpp", "C:/game/sim/myroads/edge.cpp"))
        self.assertFalse(feature.same_file("edge.cpp", "C:/game/ledge.cpp"))

    def test_a_file_alone_is_every_output_in_it(self):
        rng = random.Random(SEED + 5)
        n = Names(rng)
        show, a, b = n(), n(), n()
        with Folder() as folder:
            for name in (a, b):
                (folder / f"{name}.ts").write_text(
                    f"declare function {show}(what: string): void;\nexport function {name}(): void {{\n"
                    f"  {show}(\"{name}\");\n}}\n", encoding="utf-8")
            r = analyse([folder], folder, keep=True)
            found = feature.points(r, [(folder / f"{a}.ts").as_posix()])
            files = {r.program.files[r.cases.effects[i].file].display for i in found}
            with self.subTest(seed=SEED):
                self.assertEqual({f"{a}.ts"}, files)


class FeatureCost(unittest.TestCase):
    def test_the_cost_of_a_feature_keeps_its_own_loops(self):
        rng = random.Random(SEED + 6)
        n = Names(rng)
        root, feat, other, show, items = n(), n(), n(), n(), n()
        text = (f"declare function {show}(x: number): void;\n"
                f"export function {feat}({items}: number[]): void {{\n"
                f"  for (const a of {items}) {{\n    for (const b of {items}) {{\n      {show}(a * b);\n    }}\n  }}\n}}\n"
                f"export function {other}({items}: number[]): number {{\n"
                f"  let s = 0;\n  for (const a of {items}) {{\n    s += a;\n  }}\n  return s;\n}}\n"
                f"export function {root}({items}: number[]): void {{\n  {other}({items});\n  {feat}({items});\n}}\n")
        with Folder() as folder:
            (folder / "work.ts").write_text(text, encoding="utf-8")
            r = analyse([folder], folder, keep=True)
            line = next(i + 1 for i, row in enumerate(text.splitlines()) if f"{show}(a * b)" in row)
            wanted = [f"work.ts:{line}"]
            starts = find(r, root)
            kept = cost.data(r, starts, feature.units_of(r, wanted))
            every = cost.data(r, starts)
            where = lambda rows: {c["where it is"] for c in rows if c["what is found"] == "a loop"}  # noqa: E731
            with self.subTest(seed=SEED):
                self.assertEqual(3, len(where(every)))
                self.assertEqual(2, len(where(kept)))
                self.assertIn("quadratic", {c["the order"] for c in kept})


class PortDriver(unittest.TestCase):
    def test_a_rows_evidence_and_its_problems_files_are_found_in_their_game(self):
        import port
        rng = random.Random(SEED + 7)
        n = Names(rng)
        rule, owner, area, line, ref_line = n(), n(), n(), rng.randint(2, 99), rng.randint(2, 99)
        problem = f"P{area}-{rng.randint(1, 999)}"
        with Folder() as folder:
            ref, city = folder / "ref" / "sim", folder / "city" / "native" / "sim"
            for path in (ref / f"{rule}.ts", city / f"{rule}.cpp", city / f"{owner}.h",
                         folder / "city" / "native" / f"my{owner}" / f"{owner}.h"):
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text("", encoding="utf-8")
            problems = {problem: {"id": problem, "owner": f"native/sim/{owner}.h", "symptom": f"sim/{rule}.ts:{ref_line} does it",
                                  "cause": "unknown", "criterion": "", "evidence": []}}
            found = port.points_of([f"the {rule}", f"sim/{rule}.cpp:{line}; {problem}", "nowhere.cpp:4"],
                                   port.files_by_name([folder / "city"]), port.files_by_name([folder / "ref"]), problems)
            with self.subTest(seed=SEED):
                self.assertEqual([(city / f"{rule}.cpp", line), (city / f"{owner}.h", None)], found["city"])
                self.assertEqual([(ref / f"{rule}.ts", ref_line)], found["reference"])
                self.assertEqual(["nowhere.cpp:4"], found["unresolved"])
                self.assertEqual(("3", " x | y "), port.ROW.match("| 3 | x | y |").groups())


if __name__ == "__main__":
    unittest.main()
