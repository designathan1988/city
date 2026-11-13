"""Shaders made up at random, new on every run, in GLSL: uniforms the code outside sets, scalars and vectors (vec2..vec4
made by their names, their components, operators on them), the built-in functions of numbers and of vectors, functions
of the shader's own, choices and loops; the colour it writes (`out vec4`) is its output. To run one in software it is
compiled as C++ with a header of the shading language's types and functions, each written as GLSL 4.60 defines it
(§5.4, §5.5, §8.3, §8.5), every operation in 32-bit reals; a driver sets the uniforms of a situation, runs `main` and
prints the colour's components, bit for bit.
"""
from __future__ import annotations

import random
from dataclasses import dataclass, field

LETTERS = "xyzw"


def header() -> str:
    """The shading language's types and functions in C++ (32-bit reals, every operation as GLSL writes it)."""
    out = ["#include <cstdint>", "#include <cstdio>", "#include <cstring>"]
    for n in (2, 3, 4):
        v, cs = f"vec{n}", LETTERS[:n]
        out.append(f"struct {v} {{ float {', '.join(cs)};")
        out.append(f"  {v}() : " + ", ".join(f"{c}(0.0f)" for c in cs) + " {}")
        out.append(f"  explicit {v}(float s) : " + ", ".join(f"{c}(s)" for c in cs) + " {}")
        out.append(f"  {v}(" + ", ".join(f"float {c}_" for c in cs) + ") : " + ", ".join(f"{c}({c}_)" for c in cs)
                   + " {}")
        if n > 2:
            lower = f"vec{n - 1}"
            out.append(f"  {v}(const struct {lower} &a, float s);")
        out.append("};")
    for n in (3, 4):
        v, lower, cs = f"vec{n}", f"vec{n - 1}", LETTERS[:n]
        out.append(f"inline {v}::{v}(const {lower} &a, float s) : " +
                   ", ".join(f"{c}(a.{c})" for c in cs[:-1]) + f", {cs[-1]}(s) {{}}")
    for n in (2, 3, 4):
        v, cs = f"vec{n}", LETTERS[:n]
        for op in "+-*/":
            out.append(f"inline {v} operator{op}({v} a, {v} b) {{ {v} r; " +
                       " ".join(f"r.{c} = a.{c} {op} b.{c};" for c in cs) + " return r; }")
            out.append(f"inline {v} operator{op}({v} a, float b) {{ {v} r; " +
                       " ".join(f"r.{c} = a.{c} {op} b;" for c in cs) + " return r; }")
            out.append(f"inline {v} operator{op}(float a, {v} b) {{ {v} r; " +
                       " ".join(f"r.{c} = a {op} b.{c};" for c in cs) + " return r; }")
        out.append(f"inline {v} operator-({v} a) {{ {v} r; " + " ".join(f"r.{c} = -a.{c};" for c in cs) +
                   " return r; }")
        terms = [f"a.{c} * b.{c}" for c in cs]
        out.append(f"inline float dot({v} a, {v} b) {{ return {' + '.join(terms)}; }}")
        out.append(f"inline float length({v} a) {{ return __builtin_sqrtf(dot(a, a)); }}")
        out.append(f"inline float distance({v} a, {v} b) {{ return length(a - b); }}")
        out.append(f"inline {v} normalize({v} a) {{ return a / length(a); }}")
    out += [
        "inline float mix(float x, float y, float a) { return x * (1.0f - a) + y * a; }",
        "inline float clamp(float x, float lo, float hi) { return __builtin_fminf(__builtin_fmaxf(x, lo), hi); }",
        "inline float step(float e, float x) { return x < e ? 0.0f : 1.0f; }",
        "inline float smoothstep(float e0, float e1, float x) {",
        "  float t = clamp((x - e0) / (e1 - e0), 0.0f, 1.0f); return t * t * (3.0f - 2.0f * t); }",
        "inline float floor(float x) { return __builtin_floorf(x); }",
        "inline float fract(float x) { return x - __builtin_floorf(x); }",
        "inline float abs(float x) { return __builtin_fabsf(x); }",
        "inline float min(float a, float b) { return __builtin_fminf(a, b); }",
        "inline float max(float a, float b) { return __builtin_fmaxf(a, b); }",
        "inline float sqrt(float x) { return __builtin_sqrtf(x); }",
        "inline vec3 cross(vec3 a, vec3 b) {",
        "  return vec3(a.y * b.z - b.y * a.z, a.z * b.x - b.z * a.x, a.x * b.y - b.x * a.y); }",
    ]
    for n in (2, 3, 4):
        v, cs = f"vec{n}", LETTERS[:n]
        out.append(f"inline {v} mix({v} x, {v} y, float a) {{ return {v}(" +
                   ", ".join(f"mix(x.{c}, y.{c}, a)" for c in cs) + "); }")
        for name in ("abs", "fract", "floor"):
            out.append(f"inline {v} {name}({v} x) {{ return {v}(" + ", ".join(f"{name}(x.{c})" for c in cs) + "); }")
        for name in ("min", "max"):
            out.append(f"inline {v} {name}({v} x, {v} y) {{ return {v}(" +
                       ", ".join(f"{name}(x.{c}, y.{c})" for c in cs) + "); }")
        out.append(f"inline {v} clamp({v} x, float lo, float hi) {{ return {v}(" +
                   ", ".join(f"clamp(x.{c}, lo, hi)" for c in cs) + "); }")
    return "\n".join(out) + "\n"


def driver(uniforms: list) -> str:
    sets = "".join(f"  std::sscanf(argv[{i + 1}], \"%f\", &{u});\n" for i, u in enumerate(uniforms))
    return ("#include \"shading.h\"\n#define uniform\n#define out\n#define main shader_main\n"
            "#include \"shader.frag\"\n#undef main\n"
            "static void show(float v) { std::uint32_t b; std::memcpy(&b, &v, 4); std::printf(\"%08x\\n\", "
            "(unsigned) b); }\n"
            f"int main(int argc, char **argv) {{\n  if (argc < {len(uniforms) + 1}) return 2;\n{sets}"
            "  shader_main();\n  show(frag.x); show(frag.y); show(frag.z); show(frag.w);\n  return 0;\n}\n")


@dataclass
class Shader:
    text: str
    uniforms: list
    situations: list = field(default_factory=list)       # {uniform: value}


class Writer:
    def __init__(self, rng: random.Random):
        self.rng = rng
        self.scalars, self.vectors, self.helpers = [], {}, []
        self.count = 0

    def name(self, head: str) -> str:
        self.count += 1
        return f"{head}{self.count}"

    def literal(self) -> str:
        return f"{self.rng.randint(-8, 8) / 4}f"

    def scalar(self, depth: int) -> str:
        rng = self.rng
        if depth <= 0 or rng.random() < 0.25:
            pick = rng.random()
            if pick < 0.5 and self.scalars:
                return rng.choice(self.scalars)
            if pick < 0.75 and self.vectors:
                v = rng.choice(list(self.vectors))
                return f"{v}.{rng.choice(LETTERS[:self.vectors[v]])}"
            return self.literal()
        a, b = (lambda: self.scalar(depth - 1)), (lambda: self.scalar(depth - 1))
        kind = rng.randrange(17)
        if kind < 4:
            return f"({a()} {rng.choice('+-*')} {b()})"
        if kind == 4:
            return f"({a()} / (abs({b()}) + 1.0f))"
        if kind == 5:
            return f"mix({a()}, {b()}, {a()})"
        if kind == 6:
            lo = rng.randint(-8, 0) / 4
            return f"clamp({a()}, {lo}f, {lo + rng.randint(1, 8) / 4}f)"
        if kind == 7:
            return f"step({a()}, {b()})"
        if kind == 8:
            lo = rng.randint(-8, 0) / 4
            return f"smoothstep({lo}f, {lo + rng.randint(1, 8) / 4}f, {a()})"
        if kind == 9:
            return f"{rng.choice(['fract', 'floor', 'abs'])}({a()})"
        if kind == 10:
            return f"{rng.choice(['min', 'max'])}({a()}, {b()})"
        if kind == 11:
            return f"sqrt(abs({a()}))"
        if kind in (12, 13):
            n = rng.choice([2, 3, 4])
            pick = rng.randrange(3)
            if pick == 0:
                return f"dot({self.vector(n, depth - 1)}, {self.vector(n, depth - 1)})"
            if pick == 1:
                return f"length({self.vector(n, depth - 1)})"
            return f"distance({self.vector(n, depth - 1)}, {self.vector(n, depth - 1)})"
        if kind == 14 and self.helpers:
            return f"{rng.choice(self.helpers)}({a()}, {b()})"
        if kind == 15:
            return f"(-({a()}))"
        return a()

    def vector(self, n: int, depth: int) -> str:
        rng = self.rng
        own = [v for v, size in self.vectors.items() if size == n]
        if depth <= 0 or rng.random() < 0.3:
            if own and rng.random() < 0.6:
                return rng.choice(own)
            if rng.random() < 0.3:
                return f"vec{n}({self.scalar(0)})"
            return f"vec{n}(" + ", ".join(self.scalar(0) for _ in range(n)) + ")"
        kind = rng.randrange(8)
        a = lambda: self.vector(n, depth - 1)                       # noqa: E731
        if kind < 2:
            return f"({a()} {rng.choice('+-*')} {a()})"
        if kind == 2:
            return f"({a()} * {self.scalar(depth - 1)})"
        if kind == 3:
            return f"({self.scalar(depth - 1)} * {a()})"
        if kind == 4:
            return f"mix({a()}, {a()}, {self.scalar(depth - 1)})"
        if kind == 5 and n > 2:
            return f"vec{n}({self.vector(n - 1, depth - 1)}, {self.scalar(depth - 1)})"
        if kind == 6:
            head = "abs(" + self.scalar(depth - 1) + ") + 1.0f"
            return f"normalize(vec{n}(" + ", ".join([head] + [self.scalar(0) for _ in range(n - 1)]) + "))"
        if kind == 7 and n == 3:
            return f"cross({a()}, {a()})"
        return f"(-({a()}))"


def made(rng: random.Random) -> Shader:
    w = Writer(rng)
    uniforms = [f"u{i}" for i in range(rng.randint(1, 3))]
    lines = [f"uniform float {u};" for u in uniforms] + ["out vec4 frag;", ""]
    w.scalars = list(uniforms)
    for _ in range(rng.randint(0, 2)):
        h = w.name("h")
        saved = w.scalars, w.vectors
        w.scalars, w.vectors = ["a", "b"], {}
        c = w.scalar(2)
        test = f"{w.scalar(1)} > {w.scalar(1)}"
        other = w.scalar(2)
        lines.append(f"float {h}(float a, float b) {{\n  float c = {c};\n  if ({test}) {{\n    c = {other};\n  }}\n"
                     f"  return c;\n}}\n")
        w.scalars, w.vectors = saved
        w.helpers.append(h)
    body = []
    for _ in range(rng.randint(2, 6)):
        if rng.random() < 0.6 or not w.scalars:
            s = w.name("s")
            body.append(f"float {s} = {w.scalar(3)};")
            w.scalars.append(s)
        else:
            n = rng.choice([2, 3, 4])
            v = w.name("v")
            body.append(f"vec{n} {v} = {w.vector(n, 2)};")
            w.vectors[v] = n
        if rng.random() < 0.35 and len(w.scalars) > 1:
            s = rng.choice([x for x in w.scalars if x not in uniforms] or [None])
            if s is not None:
                body.append(f"if ({w.scalar(1)} > {w.scalar(1)}) {{\n    {s} = {w.scalar(2)};\n  }} else {{\n"
                            f"    {s} = {w.scalar(2)};\n  }}")
        if rng.random() < 0.2:
            s = rng.choice([x for x in w.scalars if x not in uniforms] or [None])
            if s is not None:
                i = w.name("i")
                body.append(f"for (int {i} = 0; {i} < {rng.randint(1, 4)}; {i}++) {{\n    {s} = {s} * 0.5f + "
                            f"{w.scalar(1)};\n  }}")
    body.append(f"frag = vec4({w.vector(3, 2)}, {w.scalar(2)});" if rng.random() < 0.5 else
                f"frag = vec4({w.scalar(2)}, {w.scalar(2)}, {w.scalar(2)}, 1.0f);")
    lines.append("void main() {\n" + "".join(f"  {s}\n" for s in body) + "}\n")
    shader = Shader("\n".join(lines), uniforms)
    for _ in range(3):
        shader.situations.append({u: rng.randint(-16, 16) / 4 for u in uniforms})
    return shader
