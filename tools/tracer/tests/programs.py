"""Programs made up at random, new on every run, the same program written in TypeScript, C++ and GDScript, to be run
for real and evaluated by the tracer (never run by it): loops (a fixed bound, a bound from the input, over a list),
lists written by position, an object's fields kept across a sequence of starts, helper functions, branches, whole
numbers and reals, and the languages' functions of numbers (square root, min, max, abs, floor). Every output goes to
`out(tag, value)`, a function the tracer does not read (the driver gives it): it prints the value's bits exactly.
"""
from __future__ import annotations

import random
from dataclasses import dataclass, field

from generate import Names

WHOLE, REAL = "whole", "real"


@dataclass
class Expr:
    kind: str                    # WHOLE or REAL
    form: tuple                  # ("lit", v) ("var", name) ("field", name) ("bin", op, a, b) ("fn", name, args)
    #                              ("elem", list, index) ("call", helper, args)


@dataclass
class Made:
    files: dict = field(default_factory=dict)        # language -> {file: text}
    steps: list = field(default_factory=list)        # (method, [args]) run in turn


class Maker:
    def __init__(self, rng: random.Random):
        self.rng, self.n = rng, Names(rng)
        self.tags = 0
        self.locals = {WHOLE: [], REAL: []}
        self.lists = []                              # (name, size)
        self.helpers = []                            # (name, param kind, body expr)

    def tag(self) -> str:
        self.tags += 1
        return f"o{self.tags}"

    # ---- expressions --------------------------------------------------------------------------------------------
    def whole(self, depth: int) -> Expr:
        r = self.rng
        pool = self.locals[WHOLE] + ["a", "c"]
        choice = r.random()
        if depth <= 0 or choice < 0.3:
            return Expr(WHOLE, ("lit", r.randint(0, 9))) if r.random() < 0.4 else Expr(WHOLE, ("var", r.choice(pool)))
        if choice < 0.55:
            return Expr(WHOLE, ("bin", r.choice("+-*"), self.whole(depth - 1), self.whole(depth - 1)))
        if choice < 0.7:
            return Expr(WHOLE, ("bin", r.choice("/%"), self.whole(depth - 1), Expr(WHOLE, ("lit", r.randint(1, 5)))))
        if choice < 0.85:
            return Expr(WHOLE, ("fn", r.choice(["min", "max"]), [self.whole(depth - 1), self.whole(depth - 1)]))
        if self.lists and choice < 0.93:
            name, size = r.choice(self.lists)
            return Expr(WHOLE, ("elem", name, r.randrange(size)))
        return Expr(WHOLE, ("fn", "abs", [self.whole(depth - 1)]))

    def real(self, depth: int) -> Expr:
        r = self.rng
        pool = self.locals[REAL] + ["b", "w"]
        choice = r.random()
        if depth <= 0 or choice < 0.3:
            return Expr(REAL, ("lit", r.choice([0.5, 1.25, 2.0, 0.1, 3.75, -1.5]))) if r.random() < 0.4 else \
                Expr(REAL, ("var", r.choice(pool)))
        if choice < 0.6:
            return Expr(REAL, ("bin", r.choice("+-*"), self.real(depth - 1),
                               self.real(depth - 1) if r.random() < 0.7 else self.whole(depth - 1)))
        if choice < 0.7:
            return Expr(REAL, ("bin", "/", self.real(depth - 1), Expr(REAL, ("lit", r.choice([2.0, 4.0, 0.5, 3.0])))))
        if choice < 0.8:
            return Expr(REAL, ("fn", "sqrt", [Expr(REAL, ("bin", "+", Expr(REAL, ("fn", "abs", [self.real(depth - 1)])),
                                                          Expr(REAL, ("lit", 1.0))))]))
        if choice < 0.9:
            return Expr(REAL, ("fn", "floor", [self.real(depth - 1)]))
        if self.helpers:
            name, kind, _ = r.choice(self.helpers)
            arg = self.whole(depth - 1) if kind == WHOLE else self.real(depth - 1)
            return Expr(REAL, ("call", name, [arg]))
        return Expr(REAL, ("fn", r.choice(["min", "max"]), [self.real(depth - 1), self.real(depth - 1)]))

    def condition(self) -> tuple:
        r = self.rng
        if r.random() < 0.5:
            return ("cmp", r.choice(["<", ">", "<=", ">="]), self.whole(1), self.whole(1))
        return ("cmp", r.choice(["<", ">"]), self.real(1), self.real(1))

    # ---- statements ---------------------------------------------------------------------------------------------
    def block(self, depth: int, size: int) -> list:
        out = []
        for _ in range(size):
            out.append(self.statement(depth))
        return out

    def statement(self, depth: int) -> tuple:
        r = self.rng
        choice = r.random()
        if choice < 0.28:
            e = self.whole(2) if r.random() < 0.5 else self.real(2)
            return ("out", self.tag(), e)
        if choice < 0.4:
            name = self.n()
            e = self.whole(2)
            self.locals[WHOLE].append(name)
            return ("let", WHOLE, name, e)
        if choice < 0.5:
            name = self.n()
            e = self.real(2)
            self.locals[REAL].append(name)
            return ("let", REAL, name, e)
        if choice < 0.58:
            return ("set", "c", self.whole(2)) if r.random() < 0.5 else ("set", "w", self.real(2))
        if choice < 0.66 and self.lists:
            name, size = r.choice(self.lists)
            return ("put", name, r.randrange(size), self.whole(2))
        if depth > 0 and choice < 0.78:
            saved = {k: list(v) for k, v in self.locals.items()}
            then = self.block(depth - 1, r.randint(1, 2))
            self.locals = {k: list(v) for k, v in saved.items()}
            other = self.block(depth - 1, r.randint(0, 2))
            self.locals = saved
            return ("if", self.condition(), then, other)
        if depth > 0 and choice < 0.88:
            counter = self.n()
            bound = Expr(WHOLE, ("lit", r.randint(1, 4))) if r.random() < 0.5 else \
                Expr(WHOLE, ("fn", "min", [Expr(WHOLE, ("var", "a")), Expr(WHOLE, ("lit", 5))]))
            saved = {k: list(v) for k, v in self.locals.items()}
            self.locals[WHOLE].append(counter)
            body = self.block(depth - 1, r.randint(1, 2))
            self.locals = saved
            return ("while", counter, bound, body)
        if depth > 0:
            item = self.n()
            values = [r.randint(0, 6) for _ in range(r.randint(1, 3))]
            saved = {k: list(v) for k, v in self.locals.items()}
            self.locals[WHOLE].append(item)
            body = self.block(depth - 1, r.randint(1, 2))
            self.locals = saved
            return ("for", item, values, body)
        return ("out", self.tag(), self.whole(1))

    def make(self) -> tuple:
        r = self.rng
        for _ in range(r.randint(0, 2)):
            name = self.n()
            kind = r.choice([WHOLE, REAL])
            saved = self.locals
            self.locals = {WHOLE: ["x"] if kind == WHOLE else [], REAL: ["x"] if kind == REAL else []}
            body = Expr(REAL, ("bin", "+", Expr(REAL, ("lit", 0.5)), Expr(kind, ("var", "x")))) if kind == WHOLE \
                else Expr(REAL, ("bin", "*", Expr(REAL, ("var", "x")), Expr(REAL, ("lit", 2.0))))
            self.locals = saved
            self.helpers.append((name, kind, body))
        lists = []
        for _ in range(r.randint(0, 2)):
            name, size = self.n(), r.randint(2, 4)
            lists.append((name, [r.randint(0, 5) for _ in range(size)]))
            self.lists.append((name, size))
        start = self.block(2, r.randint(3, 6))
        self.locals = {WHOLE: [], REAL: []}
        step = self.block(1, r.randint(1, 3))
        return lists, start, step


# ---- writing it in each language ----------------------------------------------------------------------------------
def num(v, lang: str) -> str:
    if isinstance(v, float):
        return repr(v)
    return str(v)


class Writer:
    def __init__(self, lang: str):
        self.lang = lang

    def expr(self, e: Expr) -> str:
        f = e.form
        lang = self.lang
        if f[0] == "lit":
            return num(f[1], lang) if f[1] >= 0 else f"({num(f[1], lang)})"
        if f[0] == "var":
            return f"this.{f[1]}" if lang == "ts" and f[1] in ("c", "w") else f[1]
        if f[0] == "bin":
            return f"({self.expr(f[2])} {f[1]} {self.expr(f[3])})"
        if f[0] == "elem":
            return f"this.{f[1]}[{f[2]}]" if lang == "ts" else f"{f[1]}[{f[2]}]"
        if f[0] == "call":
            return f"this.{f[1]}({self.expr(f[2][0])})" if lang == "ts" else f"{f[1]}({self.expr(f[2][0])})"
        name, args = f[1], [self.expr(a) for a in f[2]]
        if lang == "ts":
            return f"Math.{name}({', '.join(args)})"
        if lang == "cpp":
            if name in ("min", "max") and e.kind == REAL:
                return f"std::{name}((double)({args[0]}), (double)({args[1]}))"
            return f"std::{name}({', '.join(args)})"
        return f"{name}({', '.join(args)})"

    def cond(self, c: tuple) -> str:
        return f"({self.expr(c[2])} {c[1]} {self.expr(c[3])})"


def write_ts(lists, start, step, helpers) -> str:
    w = Writer("ts")
    lines = ["declare function out(tag: string, value: number): void;", "", "export class Prog {",
             "  c: number = 0;", "  w: number = 0.5;"]
    lines += [f"  {name}: number[] = [{', '.join(map(str, values))}];" for name, values in lists]
    for name, kind, body in helpers:
        lines.append(f"  {name}(x: number): number {{ return {w.expr(body)}; }}")

    def block(stmts, pad):
        out = []
        for s in stmts:
            out += stmt(s, pad)
        return out

    def stmt(s, pad):
        p = "  " * pad
        if s[0] == "out":
            return [f"{p}out(\"{s[1]}\", {w.expr(s[2])});"]
        if s[0] == "let":
            return [f"{p}let {s[2]}: number = {w.expr(s[3])};"]
        if s[0] == "set":
            return [f"{p}this.{s[1]} = {w.expr(s[2])};"]
        if s[0] == "put":
            return [f"{p}this.{s[1]}[{s[2]}] = {w.expr(s[3])};"]
        if s[0] == "if":
            out = [f"{p}if {w.cond(s[1])} {{"] + block(s[2], pad + 1)
            if s[3]:
                out += [f"{p}}} else {{"] + block(s[3], pad + 1)
            return out + [f"{p}}}"]
        if s[0] == "while":
            return [f"{p}let {s[1]}: number = 0;", f"{p}while ({s[1]} < {w.expr(s[2])}) {{"] + block(s[3], pad + 1) + \
                [f"{p}  {s[1]} = {s[1]} + 1;", f"{p}}}"]
        return [f"{p}for (const {s[1]} of [{', '.join(map(str, s[2]))}]) {{"] + block(s[3], pad + 1) + [f"{p}}}"]
    lines.append("  start(a: number, b: number): void {")
    lines += block(start, 2)
    lines += ["  }", "  step(): void {", "    const a = 1;", "    const b = 0.25;"] + block(step, 2) + ["  }", "}"]
    return "\n".join(lines) + "\n"


def write_cpp(lists, start, step, helpers) -> str:
    w = Writer("cpp")
    lines = ["#include <algorithm>", "#include <cmath>", "#include <initializer_list>",
             "void out(const char *tag, int value);", "void out(const char *tag, double value);", "",
             "struct Prog {", "  int c = 0;", "  double w = 0.5;"]
    lines += [f"  int {name}[{len(values)}] = {{{', '.join(map(str, values))}}};" for name, values in lists]
    for name, kind, body in helpers:
        lines.append(f"  double {name}({'int' if kind == WHOLE else 'double'} x) {{ return {w.expr(body)}; }}")

    def block(stmts, pad):
        out = []
        for s in stmts:
            out += stmt(s, pad)
        return out

    def stmt(s, pad):
        p = "  " * pad
        if s[0] == "out":
            return [f"{p}out(\"{s[1]}\", {w.expr(s[2])});"]
        if s[0] == "let":
            return [f"{p}{'int' if s[1] == WHOLE else 'double'} {s[2]} = {w.expr(s[3])};"]
        if s[0] == "set":
            return [f"{p}{s[1]} = {w.expr(s[2])};"]
        if s[0] == "put":
            return [f"{p}{s[1]}[{s[2]}] = {w.expr(s[3])};"]
        if s[0] == "if":
            out = [f"{p}if {w.cond(s[1])} {{"] + block(s[2], pad + 1)
            if s[3]:
                out += [f"{p}}} else {{"] + block(s[3], pad + 1)
            return out + [f"{p}}}"]
        if s[0] == "while":
            return [f"{p}int {s[1]} = 0;", f"{p}while ({s[1]} < {w.expr(s[2])}) {{"] + block(s[3], pad + 1) + \
                [f"{p}  {s[1]} = {s[1]} + 1;", f"{p}}}"]
        return [f"{p}for (int {s[1]} : {{{', '.join(map(str, s[2]))}}}) {{"] + block(s[3], pad + 1) + [f"{p}}}"]
    lines.append("  void start(int a, double b) {")
    lines += block(start, 2)
    lines += ["  }", "  void step() {", "    int a = 1;", "    double b = 0.25;"] + block(step, 2) + ["  }", "};"]
    return "\n".join(lines) + "\n"


def write_gd(lists, start, step, helpers) -> str:
    w = Writer("gd")
    lines = ["extends RefCounted", "", "var sink = null", "var c: int = 0", "var w: float = 0.5"]
    lines += [f"var {name} = [{', '.join(map(str, values))}]" for name, values in lists]
    for name, kind, body in helpers:
        lines += ["", f"func {name}(x: {'int' if kind == WHOLE else 'float'}) -> float:", f"\treturn {w.expr(body)}"]

    def block(stmts, pad):
        out = []
        for s in stmts:
            out += stmt(s, pad)
        return out or ["\t" * pad + "pass"]

    def stmt(s, pad):
        p = "\t" * pad
        if s[0] == "out":
            return [f"{p}sink.out(\"{s[1]}\", {w.expr(s[2])})"]
        if s[0] == "let":
            return [f"{p}var {s[2]}: {'int' if s[1] == WHOLE else 'float'} = {w.expr(s[3])}"]
        if s[0] == "set":
            return [f"{p}{s[1]} = {w.expr(s[2])}"]
        if s[0] == "put":
            return [f"{p}{s[1]}[{s[2]}] = {w.expr(s[3])}"]
        if s[0] == "if":
            out = [f"{p}if {w.cond(s[1])}:"] + block(s[2], pad + 1)
            if s[3]:
                out += [f"{p}else:"] + block(s[3], pad + 1)
            return out
        if s[0] == "while":
            return [f"{p}var {s[1]}: int = 0", f"{p}while {s[1]} < {w.expr(s[2])}:"] + block(s[3], pad + 1) + \
                [f"{p}\t{s[1]} = {s[1]} + 1"]
        return [f"{p}for {s[1]} in [{', '.join(map(str, s[2]))}]:"] + block(s[3], pad + 1)
    lines += ["", "func start(a: int, b: float) -> void:"] + block(start, 1)
    lines += ["", "func step() -> void:", "\tvar a: int = 1", "\tvar b: float = 0.25"] + block(step, 1)
    return "\n".join(lines) + "\n"


TS_DRIVER = """const bits = (v: number) => Buffer.from(new Float64Array([v]).buffer).toString("hex");
(globalThis as any).out = (tag: string, value: number) => console.log(`${tag} f ${bits(value)}`);
const { Prog } = await import("./program.mts");
const p = new Prog();
%CALLS%
"""
CPP_SINK = """#include <cstdio>
#include <cstring>
void out(const char *tag, int value) { std::printf("%s i %d\\n", tag, value); }
void out(const char *tag, double value) {
  unsigned char b[8]; std::memcpy(b, &value, 8);
  std::printf("%s f ", tag);
  for (int i = 0; i < 8; i++) std::printf("%02x", b[i]);
  std::printf("\\n");
}
"""
CPP_DRIVER = """#include "program.cpp"
int main() {
  Prog p;
%CALLS%
  return 0;
}
"""
GD_SINK = """extends RefCounted

func out(tag, value):
\tif typeof(value) == TYPE_FLOAT:
\t\tprint(tag, " f ", PackedFloat64Array([value]).to_byte_array().hex_encode())
\telse:
\t\tprint(tag, " i ", value)
"""
GD_DRIVER = """extends SceneTree

func _init():
\tvar p = load("res://program.gd").new()
\tp.sink = load("res://sink.gd").new()
%CALLS%
\tquit()
"""


def made(rng: random.Random) -> Made:
    m = Maker(rng)
    lists, start, step = m.make()
    a, b = rng.randint(0, 6), rng.choice([0.5, 1.25, 2.0, -1.5, 3.75])
    repeats = rng.randint(0, 2)
    steps = [("start", [a, b])] + [("step", [])] * repeats
    calls_ts = "\n".join(f"p.{name}({', '.join(repr(x) for x in args)});" for name, args in steps)
    calls_cpp = "\n".join(f"  p.{name}({', '.join(repr(x) for x in args)});" for name, args in steps)
    calls_gd = "\n".join(f"\tp.{name}({', '.join(repr(x) for x in args)})" for name, args in steps)
    out = Made(steps=steps)
    out.files["ts"] = {"program.mts": write_ts(lists, start, step, m.helpers),
                       "driver.mts": TS_DRIVER.replace("%CALLS%", calls_ts)}
    out.files["cpp"] = {"program.cpp": write_cpp(lists, start, step, m.helpers), "sink.cpp": CPP_SINK,
                        "driver.cpp": CPP_DRIVER.replace("%CALLS%", calls_cpp)}
    out.files["gd"] = {"program.gd": write_gd(lists, start, step, m.helpers), "sink.gd": GD_SINK,
                       "driver.gd": GD_DRIVER.replace("%CALLS%", calls_gd), "project.godot": "config_version=5\n"}
    return out
