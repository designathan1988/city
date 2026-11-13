"""The questions about a program, on programs made up on every run (random names, random constants): the cost of a
root (a loop in a loop, something made at each turn, code not read at each turn, planted; loops of a fixed size,
clean), two programs compared over a range (a threshold moved, planted; the same thing written otherwise, clean),
links that lead nowhere (state never read, never written, a key of data missing, one never read; planted and clean),
shaders (in a file and in a text, a uniform set by the code by its name, an output under its condition), and the
evaluator's sequences, fixed points, why and why not.
"""
from __future__ import annotations

import json
import os
import random
import shutil
import tempfile
import unittest
from pathlib import Path

from support import HERE, analyse
from generate import Names
from tracer import compare, cost, feature, links
from tracer.conditions import state_of
from tracer.decide import render
from tracer.evaluate import evaluate, why_lines, why_not
from tracer.vocab import OUTCALL, UNIFORM, WRITE
from tracer.walk import find

SEED = int(os.environ.get("TRACER_SEED") or random.SystemRandom().randrange(1 << 30))


class Folder:
    def __enter__(self):
        self.path = Path(tempfile.mkdtemp(prefix=".questions-", dir=HERE))
        return self.path

    def __exit__(self, *_):
        shutil.rmtree(self.path, ignore_errors=True)


def lines_of(text: str, mark: str) -> list:
    return [i + 1 for i, line in enumerate(text.splitlines()) if mark in line]


class Cost(unittest.TestCase):
    def test_planted_costs_are_found_and_clean_ones_are_not(self):
        rng = random.Random(SEED)
        for _ in range(4):
            n = Names(rng)
            k, p, tick, total, items = n(True), n(True), n(), n(), n()
            planted = rng.sample(["nested", "made", "unread"], rng.randint(0, 3))
            body = [f"  {tick}(size: number): void {{", f"    let {total} = 0;"]
            if "nested" in planted:
                body += [f"    for (let i = 0; i < size; i++) {{",
                         f"      for (let j = 0; j < size; j++) {{ {total} = {total} + i * j; }} // nested", "    }"]
            if "made" in planted:
                body += [f"    for (let i = 0; i < size; i++) {{",
                         f"      const q = new {p}(i); // made", f"      {total} = {total} + q.v;", "    }"]
            if "unread" in planted:
                body += [f"    for (let i = 0; i < size; i++) {{",
                         f"      {total} = {total} + this.{items}.indexOf(i); // unread", "    }"]
            body += [f"    for (let i = 0; i < {rng.randint(2, 6)}; i++) {{ {total} = {total} + i; }} // fixed",
                     "  }"]
            text = "\n".join([f"class {p} {{ v: number; constructor(v: number) {{ this.v = v; }} }}",
                              f"export class {k} {{", f"  {items}: number[] = [1, 2, 3];"] + body + ["}"]) + "\n"
            with Folder() as folder:
                (folder / "cost.ts").write_text(text, encoding="utf-8")
                r = analyse([folder], folder)
                found = cost.report(r, find(r, f"{k}.{tick}"))
                with self.subTest(seed=SEED, planted=planted, text=text):
                    at = {(int(i.where.rsplit(":", 1)[1]), i.what, i.order) for i in found}
                    if "nested" in planted:
                        line = lines_of(text, "// nested")[0]
                        self.assertIn((line, "a loop", "quadratic"), at)
                    if "made" in planted:
                        line = lines_of(text, "// made")[0]
                        self.assertTrue(any(w == line and what == "made at each turn" for w, what, _ in at), at)
                    if "unread" in planted:
                        line = lines_of(text, "// unread")[0]
                        self.assertTrue(any(w == line and what == "code not read at each turn" for w, what, _ in at), at)
                    fixed = lines_of(text, "// fixed")[0]
                    self.assertIn((fixed, "a loop", "constant"), at)
                    planted_lines = {lines_of(text, f"// {m}")[0] for m in planted}
                    planted_lines |= {line - 1 for line in planted_lines}       # the loop around each
                    false = [x for x in at if x[0] not in planted_lines | {fixed} and x[2] != "constant"]
                    self.assertEqual([], false)


class Compare(unittest.TestCase):
    def test_a_moved_threshold_is_found_in_its_band_and_the_same_thing_written_otherwise_is_not(self):
        rng = random.Random(SEED + 1)
        for _ in range(3):
            n = Names(rng)
            show, lamp = n(), n()
            low = rng.randint(3, 20)
            moved = rng.random() < 0.6
            other = low + rng.randint(2, 6) if moved else low
            one = (f"declare function {show}(what: string, on: number): void;\n"
                   f"export function {lamp}(d: number): void {{\n"
                   f"  if (d < {low}) {{ {show}(\"lit\", 1); }} else {{ {show}(\"lit\", 0); }}\n}}\n")
            two = (f"declare function {show}(what: string, on: number): void;\n"
                   f"export function {lamp}(x: number): void {{\n"
                   f"  {show}(\"lit\", x < {2 * other} ? 1 : 0);\n}}\n")
            with Folder() as folder:
                for name, text, start, inputs in (("one", one, lamp, {"d": "dist"}),
                                                   ("two", two, lamp, {"x": "dist * 2"})):
                    (folder / name).mkdir()
                    (folder / name / "lamp.ts").write_text(text, encoding="utf-8")
                    (folder / f"{name}.json").write_text(json.dumps(
                        {"root": name, "start": start, "inputs": inputs, "outputs": [show]}), encoding="utf-8")
                done = compare.compare(str(folder / "one.json"), str(folder / "two.json"), {},
                                       ("dist", 0.0, 40.0, 81))
                differing = [b for b in done.bands if b.found]
                with self.subTest(seed=SEED, low=low, other=other):
                    if not moved:
                        self.assertEqual([], differing)
                    else:
                        self.assertTrue(differing)
                        self.assertLessEqual(differing[0].low, low + 0.5)
                        self.assertGreaterEqual(differing[-1].high, other - 0.5)


class Links(unittest.TestCase):
    def test_planted_broken_links_are_found_and_clean_ones_are_not(self):
        rng = random.Random(SEED + 2)
        for _ in range(4):
            n = Names(rng)
            k, kept, lost, ghost, key, extra, load = n(True), n(), n(), n(), n(), n(), n()
            planted = rng.sample(["written", "read", "missing", "unread"], rng.randint(0, 4))
            data = {key: rng.randint(1, 9)}
            if "unread" in planted:
                data[extra] = 1
            reads = [f"this.{kept}"] + ([f"this.{ghost}"] if "read" in planted else [])
            text = "\n".join([
                f"declare function {load}(path: string): string;",
                f"const tune = JSON.parse({load}(\"data://tune.json\"));",
                f"export class {k} {{",
                f"  {kept} = 0;" + (f" {lost} = 0;" if "written" in planted else "") +
                (f" {ghost}: number;" if "read" in planted else ""),
                f"  set(v: number): void {{ this.{kept} = v + tune.{key};" +
                (f" this.{lost} = v;" if "written" in planted else "") + " }",
                f"  get(): number {{ return {' + '.join(reads)}" +
                (f" + tune.{n()}" if "missing" in planted else "") + "; }",
                "}"]) + "\n"
            with Folder() as folder:
                (folder / "links.ts").write_text(text, encoding="utf-8")
                (folder / "tune.json").write_text(json.dumps(data), encoding="utf-8")
                r = analyse([folder], folder)
                kinds = {b.kind for b in links.broken(r)}
                with self.subTest(seed=SEED, planted=planted, text=text):
                    expected = {"written": links.WRITTEN_NOT_READ, "read": links.READ_NOT_WRITTEN,
                                "missing": links.KEY_MISSING, "unread": links.KEY_NOT_READ}
                    for m in planted:
                        self.assertIn(expected[m], kinds)
                    for m in set(expected) - set(planted):
                        self.assertNotIn(expected[m], kinds)


class Shaders(unittest.TestCase):
    def test_a_shader_in_a_file_and_in_a_text_with_a_uniform_set_by_the_code(self):
        rng = random.Random(SEED + 3)
        n = Names(rng)
        fade, glow, level, edge = n(), n(), n(), rng.choice([0.25, 0.5, 0.75])
        frag = (f"uniform float {fade};\nvoid main() {{\n  if ({fade} > {edge}) {{\n    gl_FragColor = vec4(1.0);\n"
                f"  }} else {{\n    discard;\n  }}\n}}\n")
        host = (f"declare const gl: any;\ndeclare const prog: any;\nexport function {level}(v: number): void {{\n"
                f"  gl.uniform1f(gl.getUniformLocation(prog, \"{fade}\"), v);\n"
                f"  const src = `uniform float {glow};\n  void main() {{ gl_FragColor = vec4({glow}); }}`;\n"
                f"  gl.shaderSource(prog, src);\n}}\n")
        with Folder() as folder:
            (folder / "lamp.frag").write_text(frag, encoding="utf-8")
            (folder / "host.ts").write_text(host, encoding="utf-8")
            r = analyse([folder], folder)
            p = r.program
            shown = {p.files[e.file].display for e in r.cases.effects}
            self.assertTrue(any("#" in s for s in shown) or any(f.display.startswith("host.ts#") for f in p.files),
                            [f.display for f in p.files])
            out = [e for e in r.cases.effects if p.files[e.file].display == "lamp.frag" and e.kind == WRITE
                   and e.name == "gl_FragColor"]
            self.assertTrue(out, [(e.kind, e.name) for e in r.cases.effects])
            self.assertIn(fade, render(out[0].guard))
            self.assertTrue(any(e.kind == WRITE and isinstance(e.target, tuple) and e.target[:2] == (UNIFORM, fade)
                                for e in r.cases.effects))
            self.assertTrue(any(e.kind == OUTCALL and e.name == "discard" for e in r.cases.effects))
            kinds = {(b.kind, b.what) for b in links.broken(r)}
            self.assertIn((links.UNIFORM_NOT_WRITTEN, glow), kinds)


class Evaluator(unittest.TestCase):
    def test_a_sequence_its_fixed_point_why_and_why_not(self):
        rng = random.Random(SEED + 4)
        n = Names(rng)
        k, step, say, cap, count = n(True), n(), n(), rng.randint(2, 5), n()
        text = (f"declare function {say}(v: number): void;\n"
                f"export class {k} {{\n  {count} = 0;\n  {step}(): void {{\n"
                f"    if (this.{count} < {cap}) {{\n      this.{count} = this.{count} + 1;\n"
                f"      {say}(this.{count} * 2);\n    }}\n  }}\n}}\n")
        with Folder() as folder:
            (folder / "seq.ts").write_text(text, encoding="utf-8")
            r = analyse([folder], folder, keep=True)
            d = find(r, f"{k}.{step}")[0]
            run = evaluate(r, [(d, [])], repeat=cap + 2)
            values = [o.args[0].value for o in run.outputs]
            self.assertEqual([2.0 * i for i in range(1, cap + 1)], values)
            self.assertEqual(cap, run.fixpoint)
            self.assertTrue(any("seq.ts:6" in line for line in why_lines(run.outputs[-1].args[0].why)))
            self.assertTrue(any("seq.ts:5" in line for line in why_not(run, "seq.ts:6")))


class Feature(unittest.TestCase):
    def test_a_functionality_from_its_output_its_conditions_state_writers_and_root(self):
        rng = random.Random(SEED + 5)
        n = Names(rng)
        k, level, set_it, tick, show, edge = n(True), n(), n(), n(), n(), rng.randint(2, 9)
        text = (f"declare function {show}(v: number): void;\n"
                f"export class {k} {{\n  {level} = 0;\n"
                f"  {set_it}(v: number): void {{ this.{level} = v * 2; }}\n"
                f"  {tick}(): void {{\n    if (this.{level} > {edge}) {{\n      {show}(this.{level});\n    }}\n  }}\n}}\n")
        with Folder() as folder:
            (folder / "feat.ts").write_text(text, encoding="utf-8")
            r = analyse([folder], folder)
            s = feature.sheet(r, ["feat.ts:7"])
            self.assertTrue(s.outputs)
            self.assertIn("feat.ts:6", s.conditions)
            self.assertIn(f"{k}.{level}", s.reads)
            self.assertTrue(any(w[1] == "feat.ts:4" for w in s.writers), s.writers)
            self.assertTrue(any(r.program.label(root) == f"{k}.{tick}" for root, _, _ in s.roots), s.roots)
            self.assertIn(str(edge), s.constants)


class Model(unittest.TestCase):
    def test_a_model_of_code_not_read_written_by_the_user_is_followed(self):
        rng = random.Random(SEED + 6)
        n = Names(rng)
        lib, show, visible, run, edge = n(), n(), n(), n(), rng.randint(2, 9)
        host = (f"import {{ {lib} }} from \"./model\";\n"
                f"export function {run}(x: number): void {{ {lib}.{show}(x); }}\n")
        model = (f"declare function {visible}(v: number): void;\n"
                 f"export const {lib} = {{\n  {show}(x: number): void {{ if (x > {edge}) {{ {visible}(x); }} }}\n}};\n")
        with Folder() as folder:
            (folder / "host.ts").write_text(host, encoding="utf-8")
            (folder / "model.ts").write_text(model, encoding="utf-8")
            r = analyse([folder], folder, keep=True)
            d = find(r, run)[0]
            above = evaluate(r, [(d, [edge + 1])])
            below = evaluate(r, [(d, [edge - 1])])
            self.assertEqual([visible], [o.callee for o in above.outputs])
            self.assertEqual([], [o.callee for o in below.outputs])


if __name__ == "__main__":
    unittest.main()
