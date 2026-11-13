"""Programs made up with defects planted by construction, and clean ones: random names, random constants, random ways
of writing each check, random order of the functions, each scenario in a file of its own, in TypeScript, GDScript
and C++. Each scenario says what a reader of the code finds wrong and on which lines (nothing, where it is clean):

- BELIEF: the writers of a field check a flag before writing it; one does not
- GAP, OVERLAP: two sibling objects' flag written from one input, by two thresholds: a range where neither is true,
  a range where both are (clean: they meet)
- HYSTERESIS: one flag kept by hysteresis (its last value an input), its sibling the opposite of the input alone:
  both true while the first remembers (clean: the sibling is the first's opposite)
- BOUND: the writers of a counter keep it within a range (checks, clamps); one does not
- ORDER: the writers of a collection keep its elements in order (each checks its neighbours); one does not
- TWO: one object put in two containers at once (clean: two objects)
- RELATION: the writers of two fields keep one below the other (each checks the other); one does not
"""
from __future__ import annotations

import random
from dataclasses import dataclass, field
from pathlib import Path

from generate import Names

BELIEF, GAP, OVERLAP, HYSTERESIS, BOUND, ORDER, TWO = "belief", "gap", "overlap", "hysteresis", "bound", "order", "two"
RELATION = "relation"
KINDS = (BELIEF, GAP, OVERLAP, HYSTERESIS, BOUND, ORDER, TWO, RELATION)


@dataclass
class Block:
    """Lines of code, with the indices of the lines a defect is planted on."""
    lines: list
    marks: list = field(default_factory=list)


@dataclass
class Scenario:
    kind: str
    planted: bool
    path: str = ""
    lines: list = field(default_factory=list)    # the planted lines (1-based), none where clean


@dataclass
class Made:
    language: str
    files: dict = field(default_factory=dict)
    scenarios: list = field(default_factory=list)

    def write(self, folder: Path) -> None:
        for path, text in self.files.items():
            (folder / path).parent.mkdir(parents=True, exist_ok=True)
            (folder / path).write_text(text, encoding="utf-8")

    def describe(self) -> str:
        return f"{self.language}: " + ", ".join(f"{s.kind}{'*' if s.planted else ''}@{s.path}:{s.lines}"
                                               for s in self.scenarios)


def assemble(head: list, blocks: list, rng: random.Random, tail: list = ()) -> tuple:
    """A file of a head, the blocks in random order and a tail: its text and the planted lines."""
    rng.shuffle(blocks)
    lines, planted = list(head), []
    for b in blocks:
        planted += [len(lines) + 1 + m for m in b.marks]
        lines += b.lines + [""]
    lines += list(tail)
    return "\n".join(lines) + "\n", planted


def b(text: str, *marks: int) -> Block:
    return Block(text.split("\n"), list(marks))


# ---- TypeScript ---------------------------------------------------------------------------------------------------
class TS:
    ext = ".ts"

    def belief(self, n, rng, planted):
        k, g, t = n(True), n(), n()
        forms = [lambda w, p: b(f"export function {w}(o: {k}, {p}: number): void {{\n  if (o.{g}) return;\n"
                                f"  o.{t} = {p};\n}}"),
                 lambda w, p: b(f"export function {w}(o: {k}, {p}: number): void {{\n  if (!o.{g}) {{\n"
                                f"    o.{t} = {p} + 1;\n  }}\n}}"),
                 lambda w, p: b(f"export function {w}(o: {k}, {p}: number): void {{\n"
                                f"  if (!o.{g} && {p} > {rng.randint(1, 9)}) {{ o.{t} = {p}; }}\n}}")]
        blocks = [rng.choice(forms)(n(), n()) for _ in range(rng.randint(3, 5))]
        if planted:
            w, p = n(), n()
            blocks.append(b(f"export function {w}(o: {k}, {p}: number): void {{\n  o.{t} = {p};\n}}", 1))
        return [f"export class {k} {{ {g} = false; {t} = 0; }}", ""], blocks

    def alternatives(self, n, rng, kind, planted):
        ca, cb, s, x, ia, ib, t1, t2 = n(True), n(True), n(), n(), n(), n(), n().upper(), n().upper()
        low = rng.randint(2, 40)
        high = low + rng.randint(2, 9) if kind == GAP else low - rng.randint(2, 9) if kind == OVERLAP else low
        if not planted:
            high = low
        head = [f"const {t1} = {low};", f"const {t2} = {high};", f"let {x} = 0;",
                f"class {ca} {{ {s} = false; }}", f"class {cb} {{ {s} = false; }}",
                f"const {ia} = new {ca}();", f"const {ib} = new {cb}();", ""]
        one = rng.choice([f"{ia}.{s} = {x} < {t1};", f"{ia}.{s} = !({x} >= {t1});", f"{ia}.{s} = {t1} > {x};"])
        two = rng.choice([f"{ib}.{s} = {x} >= {t2};", f"{ib}.{s} = !({x} < {t2});", f"{ib}.{s} = {t2} <= {x};"])
        blocks = [b(f"export function {n()}(v: number): void {{ {x} = v; }}"),
                  b(f"export function {n()}(): void {{\n  {one}\n}}", 1),
                  b(f"export function {n()}(): void {{\n  {two}\n}}", 1)]
        return head, blocks

    def hysteresis(self, n, rng, planted):
        ca, cb, s, x, ia, ib, hi, lo = n(True), n(True), n(), n(), n(), n(), n().upper(), n().upper()
        low = rng.randint(2, 30)
        head = [f"const {lo} = {low};", f"const {hi} = {low + rng.randint(3, 9)};", f"let {x} = 0;",
                f"class {ca} {{ {s} = false; }}", f"class {cb} {{ {s} = false; }}",
                f"const {ia} = new {ca}();", f"const {ib} = new {cb}();", ""]
        other = f"{ib}.{s} = !({x} > {hi});" if planted else f"{ib}.{s} = !{ia}.{s};"
        blocks = [b(f"export function {n()}(v: number): void {{ {x} = v; }}"),
                  b(f"export function {n()}(): void {{\n  {ia}.{s} = {x} > {hi} || ({ia}.{s} && {x} > {lo});\n"
                    f"  {other}\n}}", 1, 2)]
        return head, blocks

    def bound(self, n, rng, planted):
        k, v, cap = n(True), n(), n().upper()
        top = rng.randint(20, 90)
        head = [f"const {cap} = {top};", f"class {k} {{ {v} = 0; }}", ""]
        blocks = [b(f"export function {n()}(t: {k}, m: number): void {{\n  if (m < 0) return;\n"
                    f"  if (t.{v} + m > {cap}) {{\n    t.{v} = {cap};\n  }} else {{\n    t.{v} = t.{v} + m;\n  }}\n}}"),
                  b(f"export function {n()}(t: {k}, m: number): void {{\n  if (m < 0 || m > t.{v}) return;\n"
                    f"  t.{v} = t.{v} - m;\n}}"),
                  b(f"export function {n()}(t: {k}): void {{ t.{v} = 0; }}")]
        if planted:
            wrong = rng.choice([f"t.{v} = t.{v} - m;", f"t.{v} = t.{v} + m;", f"t.{v} = m;"])
            blocks.append(b(f"export function {n()}(t: {k}, m: number): void {{\n  {wrong}\n}}", 1))
        return head, blocks

    def order(self, n, rng, planted):
        k, c = n(True), n()
        head = [f"class {k} {{ {c}: number[] = []; }}", ""]
        blocks = [b(f"export function {n()}(r: {k}, i: number, gap: number): void {{\n"
                    f"  if (gap < 0 || r.{c}[i - 1] + gap > r.{c}[i + 1]) return;\n"
                    f"  r.{c}[i] = r.{c}[i - 1] + gap;\n}}"),
                  b(f"export function {n()}(r: {k}, i: number): void {{\n"
                    f"  if (r.{c}[i] + 1 > r.{c}[i + 1]) return;\n  r.{c}[i] = r.{c}[i] + 1;\n}}")]
        if planted:
            blocks.append(b(f"export function {n()}(r: {k}, i: number, v: number): void {{\n  r.{c}[i] = v;\n}}", 1))
        return head, blocks

    def relation(self, n, rng, planted):
        k, lo, hi = n(True), n(), n()
        head = [f"class {k} {{ {lo} = 0; {hi} = {rng.randint(5, 50)}; }}", ""]
        blocks = [b(f"export function {n()}(s: {k}, d: number): void {{\n  if (d < 0) return;\n"
                    f"  s.{hi} = s.{hi} + d;\n}}"),
                  b(f"export function {n()}(s: {k}, d: number): void {{\n  if (d < 0 || s.{lo} + d > s.{hi}) return;\n"
                    f"  s.{lo} = s.{lo} + d;\n}}"),
                  b(f"export function {n()}(s: {k}, d: number): void {{\n  if (d < 0 || s.{hi} - d < s.{lo}) return;\n"
                    f"  s.{hi} = s.{hi} - d;\n}}")]
        if planted:
            blocks.append(b(f"export function {n()}(s: {k}, d: number): void {{\n"
                            f"  {rng.choice([f's.{lo} = s.{lo} + d;', f's.{hi} = s.{hi} - d;'])}\n}}", 1))
        return head, blocks

    def two(self, n, rng, planted):
        k, it, other = n(True), n(), n()
        last = it if planted else other
        return [f"class {k} {{}}", ""], [b(f"export function {n()}(a: Set<{k}>, z: Set<{k}>, {it}: {k}, "
                                           f"{other}: {k}): void {{\n  a.add({it});\n  z.add({last});\n}}",
                                           *((1, 2) if planted else ()))]


# ---- GDScript -----------------------------------------------------------------------------------------------------
class GD:
    ext = ".gd"

    def head(self, n, extra=()) -> list:
        return [f"class_name {n(True)}", "extends Node", ""] + list(extra)

    def belief(self, n, rng, planted):
        g, t = n(), n()
        forms = [lambda w, p: b(f"func {w}({p}):\n\tif {g}:\n\t\treturn\n\t{t} = {p}"),
                 lambda w, p: b(f"func {w}({p}):\n\tif not {g}:\n\t\t{t} = {p} + 1"),
                 lambda w, p: b(f"func {w}({p}):\n\tif not {g} and {p} > {rng.randint(1, 9)}:\n\t\t{t} = {p}")]
        blocks = [rng.choice(forms)(n(), n()) for _ in range(rng.randint(3, 5))]
        if planted:
            p = n()
            blocks.append(b(f"func {n()}({p}):\n\t{t} = {p}", 1))
        return self.head(n, [f"var {g} = false", f"var {t} = 0", ""]), blocks

    def siblings(self, n, s) -> tuple:
        ca, cb = n(True), n(True)
        files = {f"{ca.lower()}.gd": f"class_name {ca}\nextends Node\n\nvar {s} = false\n",
                 f"{cb.lower()}.gd": f"class_name {cb}\nextends Node\n\nvar {s} = false\n"}
        return ca, cb, files

    def alternatives(self, n, rng, kind, planted):
        s, x, ia, ib, t1, t2 = n(), n(), n(), n(), n().upper(), n().upper()
        ca, cb, files = self.siblings(n, s)
        low = rng.randint(2, 40)
        high = low + rng.randint(2, 9) if kind == GAP else low - rng.randint(2, 9) if kind == OVERLAP else low
        if not planted:
            high = low
        one = rng.choice([f"{ia}.{s} = {x} < {t1}", f"{ia}.{s} = not ({x} >= {t1})", f"{ia}.{s} = {t1} > {x}"])
        two = rng.choice([f"{ib}.{s} = {x} >= {t2}", f"{ib}.{s} = not ({x} < {t2})", f"{ib}.{s} = {t2} <= {x}"])
        head = self.head(n, [f"const {t1} = {low}", f"const {t2} = {high}", f"var {x} = 0",
                             f"var {ia} = {ca}.new()", f"var {ib} = {cb}.new()", ""])
        blocks = [b(f"func {n()}(v):\n\t{x} = v"), b(f"func {n()}():\n\t{one}", 1), b(f"func {n()}():\n\t{two}", 1)]
        return head, blocks, files

    def hysteresis(self, n, rng, planted):
        s, x, ia, ib, hi, lo = n(), n(), n(), n(), n().upper(), n().upper()
        ca, cb, files = self.siblings(n, s)
        low = rng.randint(2, 30)
        head = self.head(n, [f"const {lo} = {low}", f"const {hi} = {low + rng.randint(3, 9)}", f"var {x} = 0",
                             f"var {ia} = {ca}.new()", f"var {ib} = {cb}.new()", ""])
        other = f"{ib}.{s} = not ({x} > {hi})" if planted else f"{ib}.{s} = not {ia}.{s}"
        blocks = [b(f"func {n()}(v):\n\t{x} = v"),
                  b(f"func {n()}():\n\t{ia}.{s} = {x} > {hi} or ({ia}.{s} and {x} > {lo})\n\t{other}", 1, 2)]
        return head, blocks, files

    def bound(self, n, rng, planted):
        v, cap = n(), n().upper()
        head = self.head(n, [f"const {cap} = {rng.randint(20, 90)}", f"var {v} = 0", ""])
        blocks = [b(f"func {n()}(m):\n\tif m < 0:\n\t\treturn\n\tif {v} + m > {cap}:\n\t\t{v} = {cap}\n"
                    f"\telse:\n\t\t{v} = {v} + m"),
                  b(f"func {n()}(m):\n\tif m < 0 or m > {v}:\n\t\treturn\n\t{v} = {v} - m"),
                  b(f"func {n()}():\n\t{v} = 0")]
        if planted:
            wrong = rng.choice([f"{v} = {v} - m", f"{v} = {v} + m", f"{v} = m"])
            blocks.append(b(f"func {n()}(m):\n\t{wrong}", 1))
        return head, blocks

    def order(self, n, rng, planted):
        c = n()
        head = self.head(n, [f"var {c} = []", ""])
        blocks = [b(f"func {n()}(i, gap):\n\tif gap < 0 or {c}[i - 1] + gap > {c}[i + 1]:\n\t\treturn\n"
                    f"\t{c}[i] = {c}[i - 1] + gap"),
                  b(f"func {n()}(i):\n\tif {c}[i] + 1 > {c}[i + 1]:\n\t\treturn\n\t{c}[i] = {c}[i] + 1")]
        if planted:
            blocks.append(b(f"func {n()}(i, v):\n\t{c}[i] = v", 1))
        return head, blocks

    def relation(self, n, rng, planted):
        lo, hi = n(), n()
        head = self.head(n, [f"var {lo} = 0", f"var {hi} = {rng.randint(5, 50)}", ""])
        blocks = [b(f"func {n()}(d):\n\tif d < 0:\n\t\treturn\n\t{hi} = {hi} + d"),
                  b(f"func {n()}(d):\n\tif d < 0 or {lo} + d > {hi}:\n\t\treturn\n\t{lo} = {lo} + d"),
                  b(f"func {n()}(d):\n\tif d < 0 or {hi} - d < {lo}:\n\t\treturn\n\t{hi} = {hi} - d")]
        if planted:
            blocks.append(b(f"func {n()}(d):\n\t{rng.choice([f'{lo} = {lo} + d', f'{hi} = {hi} - d'])}", 1))
        return head, blocks

    def two(self, n, rng, planted):
        it, other = n(), n()
        last = it if planted else other
        return self.head(n), [b(f"func {n()}(a: Array, z: Array, {it}, {other}):\n\ta.append({it})\n"
                                f"\tz.append({last})", *((1, 2) if planted else ()))]


# ---- C++ ----------------------------------------------------------------------------------------------------------
class CPP:
    ext = ".cpp"

    def belief(self, n, rng, planted):
        k, g, t = n(True), n(), n()
        forms = [lambda w, p: b(f"void {w}({k} &o, int {p}) {{\n  if (o.{g}) return;\n  o.{t} = {p};\n}}"),
                 lambda w, p: b(f"void {w}({k} &o, int {p}) {{\n  if (!o.{g}) {{\n    o.{t} = {p} + 1;\n  }}\n}}"),
                 lambda w, p: b(f"void {w}({k} &o, int {p}) {{\n"
                                f"  if (!o.{g} && {p} > {rng.randint(1, 9)}) {{ o.{t} = {p}; }}\n}}")]
        blocks = [rng.choice(forms)(n(), n()) for _ in range(rng.randint(3, 5))]
        if planted:
            p = n()
            blocks.append(b(f"void {n()}({k} &o, int {p}) {{\n  o.{t} = {p};\n}}", 1))
        return [f"struct {k} {{ bool {g} = false; int {t} = 0; }};", ""], blocks

    def alternatives(self, n, rng, kind, planted):
        ca, cb, s, x, ia, ib, t1, t2 = n(True), n(True), n(), n(), n(), n(), n().upper(), n().upper()
        low = rng.randint(2, 40)
        high = low + rng.randint(2, 9) if kind == GAP else low - rng.randint(2, 9) if kind == OVERLAP else low
        if not planted:
            high = low
        head = [f"const int {t1} = {low};", f"const int {t2} = {high};", f"int {x} = 0;",
                f"struct {ca} {{ bool {s} = false; }};", f"struct {cb} {{ bool {s} = false; }};",
                f"{ca} {ia};", f"{cb} {ib};", ""]
        one = rng.choice([f"{ia}.{s} = {x} < {t1};", f"{ia}.{s} = !({x} >= {t1});", f"{ia}.{s} = {t1} > {x};"])
        two = rng.choice([f"{ib}.{s} = {x} >= {t2};", f"{ib}.{s} = !({x} < {t2});", f"{ib}.{s} = {t2} <= {x};"])
        blocks = [b(f"void {n()}(int v) {{ {x} = v; }}"), b(f"void {n()}() {{\n  {one}\n}}", 1),
                  b(f"void {n()}() {{\n  {two}\n}}", 1)]
        return head, blocks

    def hysteresis(self, n, rng, planted):
        ca, cb, s, x, ia, ib, hi, lo = n(True), n(True), n(), n(), n(), n(), n().upper(), n().upper()
        low = rng.randint(2, 30)
        head = [f"const int {lo} = {low};", f"const int {hi} = {low + rng.randint(3, 9)};", f"int {x} = 0;",
                f"struct {ca} {{ bool {s} = false; }};", f"struct {cb} {{ bool {s} = false; }};",
                f"{ca} {ia};", f"{cb} {ib};", ""]
        other = f"{ib}.{s} = !({x} > {hi});" if planted else f"{ib}.{s} = !{ia}.{s};"
        blocks = [b(f"void {n()}(int v) {{ {x} = v; }}"),
                  b(f"void {n()}() {{\n  {ia}.{s} = {x} > {hi} || ({ia}.{s} && {x} > {lo});\n  {other}\n}}", 1, 2)]
        return head, blocks

    def bound(self, n, rng, planted):
        k, v, cap = n(True), n(), n().upper()
        head = [f"const int {cap} = {rng.randint(20, 90)};", f"struct {k} {{ int {v} = 0; }};", ""]
        blocks = [b(f"void {n()}({k} &t, int m) {{\n  if (m < 0) return;\n"
                    f"  if (t.{v} + m > {cap}) {{\n    t.{v} = {cap};\n  }} else {{\n    t.{v} = t.{v} + m;\n  }}\n}}"),
                  b(f"void {n()}({k} &t, int m) {{\n  if (m < 0 || m > t.{v}) return;\n  t.{v} = t.{v} - m;\n}}"),
                  b(f"void {n()}({k} &t) {{ t.{v} = 0; }}")]
        if planted:
            wrong = rng.choice([f"t.{v} = t.{v} - m;", f"t.{v} = t.{v} + m;", f"t.{v} = m;"])
            blocks.append(b(f"void {n()}({k} &t, int m) {{\n  {wrong}\n}}", 1))
        return head, blocks

    def order(self, n, rng, planted):
        k, c = n(True), n()
        head = ["#include <vector>", f"struct {k} {{ std::vector<int> {c}; }};", ""]
        blocks = [b(f"void {n()}({k} &r, int i, int gap) {{\n"
                    f"  if (gap < 0 || r.{c}[i - 1] + gap > r.{c}[i + 1]) return;\n"
                    f"  r.{c}[i] = r.{c}[i - 1] + gap;\n}}"),
                  b(f"void {n()}({k} &r, int i) {{\n  if (r.{c}[i] + 1 > r.{c}[i + 1]) return;\n"
                    f"  r.{c}[i] = r.{c}[i] + 1;\n}}")]
        if planted:
            blocks.append(b(f"void {n()}({k} &r, int i, int v) {{\n  r.{c}[i] = v;\n}}", 1))
        return head, blocks

    def relation(self, n, rng, planted):
        k, lo, hi = n(True), n(), n()
        head = [f"struct {k} {{ int {lo} = 0; int {hi} = {rng.randint(5, 50)}; }};", ""]
        blocks = [b(f"void {n()}({k} &s, int d) {{\n  if (d < 0) return;\n  s.{hi} = s.{hi} + d;\n}}"),
                  b(f"void {n()}({k} &s, int d) {{\n  if (d < 0 || s.{lo} + d > s.{hi}) return;\n"
                    f"  s.{lo} = s.{lo} + d;\n}}"),
                  b(f"void {n()}({k} &s, int d) {{\n  if (d < 0 || s.{hi} - d < s.{lo}) return;\n"
                    f"  s.{hi} = s.{hi} - d;\n}}")]
        if planted:
            blocks.append(b(f"void {n()}({k} &s, int d) {{\n"
                            f"  {rng.choice([f's.{lo} = s.{lo} + d;', f's.{hi} = s.{hi} - d;'])}\n}}", 1))
        return head, blocks

    def two(self, n, rng, planted):
        k, it, other = n(True), n(), n()
        last = it if planted else other
        return ["#include <vector>", f"struct {k} {{}};", ""], \
            [b(f"void {n()}(std::vector<{k} *> &a, std::vector<{k} *> &z, {k} *{it}, {k} *{other}) {{\n"
               f"  a.push_back({it});\n  z.push_back({last});\n}}", *((1, 2) if planted else ()))]


WRITERS = {"typescript": TS(), "gdscript": GD(), "cpp": CPP()}


def make(language: str, rng: random.Random, clean: bool = False) -> Made:
    """A program of one to four scenarios, each planted or clean at random (all clean where asked)."""
    n = Names(rng)
    w = WRITERS[language]
    made = Made(language)
    if language == "gdscript":
        made.files["project.godot"] = "config_version=5\n"
    for kind in rng.sample(KINDS, rng.randint(1, 4)):
        planted = not clean and rng.random() < 0.6
        extra = {}
        if kind in (GAP, OVERLAP):
            got = w.alternatives(n, rng, kind, planted)
        elif kind == HYSTERESIS:
            got = w.hysteresis(n, rng, planted)
        else:
            got = getattr(w, kind)(n, rng, planted)
        head, blocks = got[0], got[1]
        if len(got) > 2:
            extra = got[2]
        text, lines = assemble(head, blocks, rng)
        path = n() + w.ext
        made.files[path] = text
        made.files.update(extra)
        made.scenarios.append(Scenario(kind, planted, path, lines if planted else []))
    return made
