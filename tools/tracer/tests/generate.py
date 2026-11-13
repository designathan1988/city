"""Fictitious programs, different on every run, for the tracer to trace whole: a chain of functions f0 -> f1 -> ... ->
fn in which every link is made by a mechanism of the language drawn at random (a call, a method, a callback, a bound
function, an event, a signal, a table read by a key, a worker's message, a queue of functions, ...), every name made
of random syllables, the functions spread over files at random, and decoys beside them: functions of the same names
in classes nothing uses, and entries of a table no caller selects. The trace must go from f0 to fn and back, by
exact links and links by text alone, and reach no decoy.
"""
from __future__ import annotations

import random
from dataclasses import dataclass, field
from pathlib import Path

SYLLABLES = ("ka", "to", "mi", "ru", "ve", "lo", "sa", "pe", "di", "no", "bu", "ze", "fa", "ri", "go", "la", "te",
             "xo", "ni", "ma", "qu", "jo", "wi", "hy", "cel", "dor", "pak", "tun", "vim", "zor")
RESERVED = {"do", "if", "in", "for", "new", "try", "var", "let", "case", "else", "enum", "void", "with", "this",
            "true", "null", "func", "pass", "self", "super", "class", "const", "break", "while", "yield", "await",
            "async", "static", "signal", "return", "extends", "default", "delete", "export", "import", "typeof",
            "switch", "public", "private", "struct", "union", "auto", "goto", "long", "char", "int", "float", "bool",
            "match", "not", "and", "or", "tool", "is", "as", "of", "get", "set", "go", "to", "no"}


class Names:
    def __init__(self, rng: random.Random):
        self.rng, self.used = rng, set()

    def __call__(self, upper: bool = False) -> str:
        while True:
            word = "".join(self.rng.choice(SYLLABLES) for _ in range(self.rng.randint(2, 4)))
            if word not in self.used and word not in RESERVED:
                self.used.add(word)
                return word.capitalize() if upper else word


@dataclass
class Program:
    language: str
    files: dict = field(default_factory=dict)            # path -> text
    chain: list = field(default_factory=list)            # the functions' names, start first
    hops: list = field(default_factory=list)             # the mechanism of each link
    decoys: list = field(default_factory=list)           # names of functions no trace from the start may reach
    state: tuple | None = None                           # (the function writing it, the state, its owner, its reader)

    def write(self, folder: Path) -> None:
        for path, text in self.files.items():
            target = folder / path
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_text(text, encoding="utf-8")

    def describe(self) -> str:
        return f"{self.language}: " + " ".join(f"{a} -[{m}]->" for a, m in zip(self.chain, self.hops)) + \
            f" {self.chain[-1]}; decoys {self.decoys}; state {self.state}"


def inside(text: str, head: str, line: str) -> str:
    """The text with a line put first in the body of the function whose header starts as written."""
    at = text.index(head)
    body = text.index("{", text.index(")", at)) + 1
    return text[:body] + " " + line + text[body:]


def state_writer(rng: random.Random, chain: list, home: dict) -> str | None:
    """A function of the chain beside its end (same file or class), not the end: it writes the state the end reads."""
    beside = [f for f in chain[:-1] if home[f] == home[chain[-1]]]
    return rng.choice(beside) if beside else None


# ---- TypeScript -------------------------------------------------------------------------------------------------
def ts_hop(kind: str, a: str, b: str, n: Names, prog: Program, file_of: dict, files: dict) -> str:
    if kind == "direct":
        return f"export function {a}(): void {{ {b}(); }}"
    if kind == "method":
        k, m = n(True), n()
        decoy = n()
        prog.decoys.append(decoy)
        return (f"class {k} {{ {m}(): void {{ {b}(); }} }}\n"
                f"class {n(True)} {{ {m}(): void {{ {decoy}(); }} }}\nfunction {decoy}(): void {{}}\n"
                f"export function {a}(): void {{ new {k}().{m}(); }}")
    if kind == "callback":
        return f"export function {a}(): void {{ [1, 2].forEach(() => {b}()); }}"
    if kind == "bind":
        v = n()
        return f"export function {a}(): void {{ const {v} = {b}.bind(null); {v}(); }}"
    if kind == "factory":
        mk, k = n(), n()
        return f"function {mk}() {{ return {{ {k}: {b} }}; }}\nexport function {a}(): void {{ {mk}().{k}(); }}"
    if kind == "event":
        t, e = n(), n()
        return (f"export function {a}(): void {{ const {t} = new EventTarget(); {t}.addEventListener(\"{e}\", {b}); "
                f"{t}.dispatchEvent(new Event(\"{e}\")); }}")
    if kind == "table":
        o, tbl, run, k1, k2 = n(), n(True), n(), n(), n()
        prog.decoys.append(o)
        return (f"function {o}(): void {{}}\nconst {tbl} = {{ {k1}: {b}, {k2}: {o} }};\n"
                f"function {run}(key: string): void {{ {tbl}[key](); }}\n"
                f"export function {a}(): void {{ {run}(\"{k1}\"); }}")
    if kind == "then":
        return f"export function {a}(): void {{ Promise.resolve(1).then(() => {b}()); }}"
    if kind == "interface":
        i, impl, go, use = n(True), n(True), n(), n()
        return (f"interface {i} {{ {go}(): void; }}\nclass {impl} implements {i} {{ {go}(): void {{ {b}(); }} }}\n"
                f"function {use}(x: {i}): void {{ x.{go}(); }}\nexport function {a}(): void {{ {use}(new {impl}()); }}")
    if kind == "getter":
        g, v, o = n(True), n(), n()
        return (f"class {g} {{ get {v}(): number {{ {b}(); return 1; }} }}\n"
                f"export function {a}(): void {{ const {o} = new {g}(); void {o}.{v}; }}")
    if kind == "field":
        h, cb, fire, o = n(True), n(), n(), n()
        return (f"class {h} {{ {cb}: () => void = () => {{}}; {fire}(): void {{ this.{cb}(); }} }}\n"
                f"export function {a}(): void {{ const {o} = new {h}(); {o}.{cb} = {b}; {o}.{fire}(); }}")
    if kind == "worker":
        wk, sc, w = n(), n(), n()
        files[f"{wk}.ts"] = (f"import {{ {b} }} from \"./{file_of[b]}\";\n"
                             f"const {sc} = self as unknown as {{ onmessage: ((e: MessageEvent) => void) | null }};\n"
                             f"{sc}.onmessage = () => {{ {b}(); }};\n")
        return (f"export function {a}(): void {{ const {w} = new Worker(new URL(\"./{wk}.ts\", import.meta.url)); "
                f"{w}.postMessage(1); }}")
    if kind == "await":
        h = n()
        return f"async function {h}(): Promise<void> {{ {b}(); }}\nexport async function {a}(): Promise<void> {{ await {h}(); }}"
    if kind == "arrow":
        h = n()
        return f"const {h} = (): void => {{ {b}(); }};\nexport function {a}(): void {{ {h}(); }}"
    if kind == "static":
        s, m = n(True), n()
        return f"class {s} {{ static {m}(): void {{ {b}(); }} }}\nexport function {a}(): void {{ {s}.{m}(); }}"
    if kind == "override":
        base, d, m, use = n(True), n(True), n(), n()
        return (f"class {base} {{ {m}(): void {{}} }}\nclass {d} extends {base} {{ {m}(): void {{ {b}(); }} }}\n"
                f"function {use}(x: {base}): void {{ x.{m}(); }}\nexport function {a}(): void {{ {use}(new {d}()); }}")
    if kind == "param":
        h, cb = n(), n()
        return f"function {h}({cb}: () => void): void {{ {cb}(); }}\nexport function {a}(): void {{ {h}({b}); }}"
    if kind == "timer":
        return f"export function {a}(): void {{ setTimeout(() => {b}(), 0); }}"
    if kind == "this":
        k, m, g = n(True), n(), n()
        return (f"class {k} {{ {m}(): void {{ this.{g}(); }} {g}(): void {{ {b}(); }} }}\n"
                f"export function {a}(): void {{ new {k}().{m}(); }}")
    if kind == "destructure":
        k = n()
        return f"export function {a}(): void {{ const {{ {k} }} = {{ {k}: {b} }}; {k}(); }}"
    if kind == "optional":
        k, m, o = n(True), n(), n()
        return (f"class {k} {{ {m}(): void {{ {b}(); }} }}\n"
                f"export function {a}(): void {{ const {o}: {k} | null = new {k}(); {o}?.{m}(); }}")
    if kind == "map":
        mp, k = n(), n()
        return (f"export function {a}(): void {{ const {mp} = new Map<string, () => void>(); "
                f"{mp}.set(\"{k}\", {b}); {mp}.get(\"{k}\")?.(); }}")
    if kind == "closure":
        mk, g = n(), n()
        return (f"function {mk}(): () => void {{ return () => {b}(); }}\n"
                f"export function {a}(): void {{ const {g} = {mk}(); {g}(); }}")
    raise ValueError(kind)


TS_HOPS = ("direct", "method", "callback", "bind", "factory", "event", "table", "then", "interface", "getter",
           "field", "worker", "await", "arrow", "static", "override", "param", "timer", "this", "destructure",
           "optional", "map", "closure")


def typescript(rng: random.Random) -> Program:
    n = Names(rng)
    prog = Program("typescript")
    chain = [n() for _ in range(rng.randint(4, 8))]
    paths = [n() for _ in range(rng.randint(1, 3))]
    file_of = {f: rng.choice(paths) for f in chain}
    blocks, extra = {p: [] for p in paths}, {}
    for a, b in zip(chain, chain[1:]):
        kind = rng.choice(TS_HOPS)
        prog.hops.append(kind)
        blocks[file_of[a]].append(ts_hop(kind, a, b, n, prog, file_of, extra))
    w, st, end = state_writer(rng, chain, file_of), n(), chain[-1]
    read = f" const {n()} = {st}; " if w else ""
    blocks[file_of[end]].append(f"export function {end}(): void {{{read}}}")
    for p in paths:
        imports = sorted({(file_of[f], f) for f in chain if file_of[f] != p and f in "\n".join(blocks[p])})
        head = "".join(f"import {{ {f} }} from \"./{q}\";\n" for q, f in imports)
        body = "\n\n".join(blocks[p]) + "\n"
        if w and p == file_of[w]:
            head += f"let {st} = 0;\n"
            body = inside(body, f"function {w}(", f"{st} = {rng.randint(1, 9)};")
            prog.state = (w, st, p, end)
        prog.files[f"{p}.ts"] = head + body
    prog.files.update({k: v for k, v in extra.items()})
    prog.chain = chain
    return prog


# ---- GDScript ---------------------------------------------------------------------------------------------------
def gd_hop(kind: str, a: str, b: str, n: Names, here: str, there: str, signals: list) -> str:
    same = here == there
    call = f"{b}()" if same else f"{there}.new().{b}()"
    fn = b if same else f"{there}.new().{b}"
    if kind == "direct":
        body = f"\t{call}"
    elif kind == "signal":
        s = n()
        signals.append(s)
        body = f"\t{s}.connect({fn})\n\t{s}.emit()"
    elif kind == "bind":
        v = n()
        body = f"\tvar {v} = {fn}.bind(1)\n\t{v}.call()"
    elif kind == "call_text":
        body = f"\tcall(\"{b}\")" if same else f"\t{there}.new().call(\"{b}\")"
    elif kind == "dict":
        d, k = n(), n()
        body = f"\tvar {d} = {{\"{k}\": {fn}}}\n\t{d}[\"{k}\"].call()"
    elif kind == "deferred":
        body = f"\t{fn}.call_deferred()"
    elif kind == "lambda":
        v = n()
        body = f"\tvar {v} = func(): {call}\n\t{v}.call()"
    elif kind == "emit_text":
        s = n()
        signals.append(s)
        body = f"\tconnect(\"{s}\", {fn})\n\temit_signal(\"{s}\")"
    elif kind == "callv":
        body = f"\tcallv(\"{b}\", [])" if same else f"\t{there}.new().callv(\"{b}\", [])"
    elif kind == "param":
        h, cb = n(), n()
        return f"func {h}({cb}: Callable):\n\t{cb}.call()\n\nfunc {a}(_x = 0):\n\t{h}({fn})\n"
    elif kind == "callable_text":
        target = "self" if same else f"{there}.new()"
        body = f"\tCallable({target}, \"{b}\").call()"
    elif kind == "array":
        v = n()
        body = f"\tvar {v} = [{fn}]\n\t{v}[0].call()"
    elif kind == "self":
        body = f"\tself.{b}()" if same else f"\t{call}"
    elif kind == "variable":
        v = n()
        body = f"\tvar {v} = {there}.new()\n\t{v}.{b}()" if not same else f"\tvar {v} = self\n\t{v}.{b}()"
    else:
        raise ValueError(kind)
    return f"func {a}(_x = 0):\n{body}\n"


GD_HOPS = ("direct", "signal", "bind", "call_text", "dict", "deferred", "lambda", "emit_text", "callv", "param",
           "callable_text", "array", "self", "variable")


def gdscript(rng: random.Random) -> Program:
    n = Names(rng)
    prog = Program("gdscript")
    chain = [n() for _ in range(rng.randint(4, 8))]
    classes = [n(True) for _ in range(rng.randint(1, 3))]
    class_of = {f: rng.choice(classes) for f in chain}
    funcs, signals = {c: [] for c in classes}, {c: [] for c in classes}
    for a, b in zip(chain, chain[1:]):
        kind = rng.choice(GD_HOPS)
        prog.hops.append(kind)
        funcs[class_of[a]].append(gd_hop(kind, a, b, n, class_of[a], class_of[b], signals[class_of[a]]))
    w, st, end = state_writer(rng, chain, class_of), n(), chain[-1]
    funcs[class_of[end]].append(f"func {end}(_x = 0):\n\t" + (f"var {n()} = {st}\n" if w else "pass\n"))
    decoy_class = n(True)                    # a class nothing uses, with functions of the chain's names
    for c in classes:
        head = f"class_name {c}\nextends Node\n\n" + "".join(f"signal {s}\n" for s in signals[c])
        body = "\n".join(funcs[c])
        if w and c == class_of[w]:
            head += f"var {st} = 0\n"
            at = body.index(f"func {w}(_x = 0):\n") + len(f"func {w}(_x = 0):\n")
            body = body[:at] + f"\t{st} = {rng.randint(1, 9)}\n" + body[at:]
            prog.state = (f"{c}.{w}", st, c, f"{c}.{end}")
        prog.files[f"{c.lower()}.gd"] = head + "\n" + body
    prog.files[f"{decoy_class.lower()}.gd"] = f"class_name {decoy_class}\nextends Node\n\n" + "\n".join(
        f"func {f}(_x = 0):\n\tpass\n" for f in chain[1:])
    prog.files["project.godot"] = "config_version=5\n"
    prog.decoys += [f"{decoy_class}.{f}" for f in chain[1:]]
    prog.chain = [f"{class_of[f]}.{f}" for f in chain]
    return prog


# ---- C++ --------------------------------------------------------------------------------------------------------
def cpp_hop(kind: str, a: str, b: str, n: Names, prog: Program) -> str:
    if kind == "direct":
        return f"void {a}() {{ {b}(); }}"
    if kind == "virtual":
        base, d, m, x, p = n(True), n(True), n(), n(), n()
        return (f"struct {base} {{ virtual void {m}() {{}} }};\nstruct {d} : {base} {{ void {m}() override {{ {b}(); }} }};\n"
                f"void {a}() {{ {d} {x}; {base} *{p} = &{x}; {p}->{m}(); }}")
    if kind == "lambda":
        run = n()
        return f"template <class F> void {run}(F f) {{ f(); }}\nvoid {a}() {{ {run}([] {{ {b}(); }}); }}"
    if kind == "function_member":
        h, cb, fire, x = n(True), n(), n(), n()
        return (f"struct {h} {{ std::function<void()> {cb}; void {fire}() {{ {cb}(); }} }};\n"
                f"void {a}() {{ {h} {x}; {x}.{cb} = [] {{ {b}(); }}; {x}.{fire}(); }}")
    if kind == "pointer":
        fp = n()
        return f"void {a}() {{ void (*{fp})() = &{b}; {fp}(); }}"
    if kind == "operator":
        v, x, y = n(True), n(), n()
        return (f"struct {v} {{ {v} operator+(const {v} &o) const {{ {b}(); return o; }} }};\n"
                f"void {a}() {{ {v} {x}, {y}; {x} + {y}; }}")
    if kind == "queue":
        j, pool, sub, go, x = n(True), n(True), n(), n(), n()
        return (f"struct {j} {{ int r; std::function<void()> w; }};\n"
                f"struct {pool} {{ std::vector<{j}> q; void {sub}(std::function<void()> w) {{ q.push_back({j}{{ 1, std::move(w) }}); }}"
                f" void {go}() {{ {j} it = q.front(); auto w = std::move(it.w); w(); }} }};\n"
                f"void {a}() {{ {pool} {x}; {x}.{sub}([] {{ {b}(); }}); {x}.{go}(); }}")
    if kind == "method":
        k, m, x = n(True), n(), n()
        decoy = n()
        prog.decoys.append(decoy)
        return (f"struct {k} {{ void {m}() {{ {b}(); }} }};\nvoid {decoy}() {{}}\n"
                f"struct {n(True)} {{ void {m}() {{ {decoy}(); }} }};\nvoid {a}() {{ {k} {x}; {x}.{m}(); }}")
    if kind == "static":
        s, m = n(True), n()
        return f"struct {s} {{ static void {m}() {{ {b}(); }} }};\nvoid {a}() {{ {s}::{m}(); }}"
    if kind == "namespace":
        ns, g = n(), n()
        return f"namespace {ns} {{ void {g}() {{ {b}(); }} }}\nvoid {a}() {{ {ns}::{g}(); }}"
    if kind == "param":
        h = n()
        return f"void {h}(void (*cb)()) {{ cb(); }}\nvoid {a}() {{ {h}({b}); }}"
    if kind == "std_param":
        h = n()
        return f"void {h}(const std::function<void()> &cb) {{ cb(); }}\nvoid {a}() {{ {h}([] {{ {b}(); }}); }}"
    if kind == "functor":
        k, x = n(True), n()
        return f"struct {k} {{ void operator()() const {{ {b}(); }} }};\nvoid {a}() {{ {k} {x}; {x}(); }}"
    if kind == "vector":
        v = n()
        return (f"void {a}() {{ std::vector<std::function<void()>> {v}; {v}.push_back({b}); "
                f"for (auto &f : {v}) f(); }}")
    if kind == "heap":
        k, m, x = n(True), n(), n()
        return f"struct {k} {{ void {m}() {{ {b}(); }} }};\nvoid {a}() {{ {k} *{x} = new {k}(); {x}->{m}(); }}"
    raise ValueError(kind)


CPP_HOPS = ("direct", "virtual", "lambda", "function_member", "pointer", "operator", "queue", "method", "static",
            "namespace", "param", "std_param", "functor", "vector", "heap")


def cpp(rng: random.Random) -> Program:
    n = Names(rng)
    prog = Program("cpp")
    chain = [n() for _ in range(rng.randint(4, 8))]
    blocks = []
    for a, b in zip(chain, chain[1:]):
        kind = rng.choice(CPP_HOPS)
        prog.hops.append(kind)
        blocks.append(cpp_hop(kind, a, b, n, prog))
    w, st, end = rng.choice(chain[:-1]), n(), chain[-1]
    blocks.append(f"void {end}() {{ int {n()} = {st}; }}")
    rng.shuffle(blocks)
    head = "#include <functional>\n#include <vector>\n\n" + "".join(f"void {f}();\n" for f in chain) + f"int {st} = 0;\n"
    body = inside("\n\n".join(blocks) + "\n", f"void {w}() {{", f"{st} = {rng.randint(1, 9)};")
    path = n()
    prog.files[f"{path}.cpp"] = head + "\n" + body
    prog.state = (w, st, path, end)
    prog.chain = chain
    return prog


LANGUAGES = {"typescript": typescript, "gdscript": gdscript, "cpp": cpp}
