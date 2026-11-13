"""Programs made up at random, new on every run, in TypeScript and GDScript, whose functions link to each other in every
way the languages have: a call, a call on an object of a class known only as it runs (a derived class held as its
base), a function passed as a value through one to three calls, kept in a field, kept in a collection, run by an
event or a signal, run by its name in a text; and the ways a name alone does not say which function runs: an overload
chosen by its arguments' types, a method hidden below its class (not virtual: the declared type's runs), methods of
one name in classes apart, a lambda handed to a function (a decoy hands it another), a template's function calling
a method of its type's argument. Every function says when it is entered (`enter(id)`, code the tracer
does not read): run for real, the program prints every link that happened (caller -> callee). Decoys: a function
that hands another function to the same chain and is never run, so that function must not be reached from the start.
"""
from __future__ import annotations

import random
from dataclasses import dataclass, field

from generate import Names

KINDS = ("direct", "derived", "value", "field", "collection", "event", "text", "overload", "hidden", "samename",
         "lambda", "template")


@dataclass
class Linked:
    files: dict = field(default_factory=dict)        # language -> {file: text}
    start: str = "start"
    plan: list = field(default_factory=list)         # (from, kind, to)


def plan(rng: random.Random, n: Names) -> tuple:
    """A tree of functions from the start, each link a kind drawn at random; decoys beside it."""
    names = [n() for _ in range(rng.randint(4, 8))]
    links, frontier = [], ["start"]
    for name in names:
        source = rng.choice(frontier)
        links.append((source, rng.choice(KINDS), name))
        frontier.append(name)
    return names, links


TS_SINK = """const stack: string[] = [];
(globalThis as any).enter = (id: string) => { console.log(`edge ${stack.length ? stack[stack.length - 1] : "-"} ${id}`); stack.push(id); };
(globalThis as any).leave = () => { stack.pop(); };
const program = await import("./program.mts");
program.start();
"""

GD_SINK = """extends RefCounted

var stack = []

func enter(id):
\tprint("edge ", stack.back() if stack.size() > 0 else "-", " ", id)
\tstack.push_back(id)

func leave():
\tstack.pop_back()
"""

GD_DRIVER = """extends SceneTree

func _init():
\tvar p = load("res://program.gd").new()
\tp.sink = load("res://sink.gd").new()
\tp.start()
\tquit()
"""


def write_ts(rng, n, names, links) -> str:
    out = ["declare function enter(id: string): void;", "declare function leave(): void;", ""]
    bodies = {name: [] for name in ["start"] + names}
    extra = []
    for source, kind, target in links:
        body = bodies[source]
        if kind == "direct":
            body.append(f"{target}();")
        elif kind == "derived":
            base, kid, m = n(True), n(True), n()
            extra += [f"class {base} {{ {m}(): void {{ enter(\"{base}.{m}\"); leave(); }} }}",
                      f"class {kid} extends {base} {{ {m}(): void {{ enter(\"{kid}.{m}\"); {target}(); leave(); }} }}"]
            body.append(f"{{ const o: {base} = new {kid}(); o.{m}(); }}")
        elif kind == "value":
            depth = rng.randint(1, 3)
            chain = [n() for _ in range(depth)]
            decoy, other = n(), n()
            for i, c in enumerate(chain):
                call = f"g();" if i == depth - 1 else f"{chain[i + 1]}(g);"
                extra.append(f"function {c}(g: () => void): void {{ enter(\"{c}\"); {call} leave(); }}")
            extra.append(f"function {other}(): void {{ enter(\"{other}\"); leave(); }}")
            extra.append(f"export function {decoy}(): void {{ enter(\"{decoy}\"); {chain[0]}({other}); leave(); }}")
            body.append(f"{chain[0]}({target});")
        elif kind == "field":
            hub, held, fire = n(True), n(), n()
            extra.append(f"class {hub} {{ {held}!: () => void; "
                         f"{fire}(): void {{ enter(\"{hub}.{fire}\"); this.{held}(); leave(); }} }}")
            body.append(f"{{ const h = new {hub}(); h.{held} = {target}; h.{fire}(); }}")
        elif kind == "collection":
            box, items, run = n(True), n(), n()
            extra.append(f"class {box} {{ {items}: Array<() => void> = []; "
                         f"{run}(): void {{ enter(\"{box}.{run}\"); this.{items}[0](); leave(); }} }}")
            body.append(f"{{ const b = new {box}(); b.{items} = [{target}]; b.{run}(); }}")
        elif kind == "overload":
            f = n()
            extra += [f"function {f}(x: number): void;", f"function {f}(x: string): void;",
                      f"function {f}(x: any): void {{ enter(\"{f}\"); {target}(); leave(); }}"]
            body.append(f"{f}(1);")
        elif kind in ("samename", "template", "hidden"):
            a, b, m, other, run = n(True), n(True), n(), n(), n()
            extra += [f"class {a} {{ {m}(): void {{ enter(\"{a}.{m}\"); {target}(); leave(); }} }}",
                      f"class {b} {{ {m}(): void {{ enter(\"{b}.{m}\"); {other}(); leave(); }} }}",
                      f"function {other}(): void {{ enter(\"{other}\"); leave(); }}"]
            if kind == "template":
                extra.append(f"function {run}<T extends {{ {m}(): void }}>(t: T): void "
                             f"{{ enter(\"{run}\"); t.{m}(); leave(); }}")
                body.append(f"{run}(new {a}());")
            else:
                body.append(f"{{ const x = new {a}(); x.{m}(); }}")
        elif kind == "lambda":
            run, decoy, other = n(), n(), n()
            extra += [f"function {run}(g: () => void): void {{ enter(\"{run}\"); g(); leave(); }}",
                      f"function {other}(): void {{ enter(\"{other}\"); leave(); }}",
                      f"export function {decoy}(): void {{ enter(\"{decoy}\"); {run}(() => {{ {other}(); }}); leave(); }}"]
            body.append(f"{run}(() => {{ {target}(); }});")
        elif kind == "event":
            name = n()
            body.append(f"{{ const t = new EventTarget(); t.addEventListener(\"{name}\", {target}); "
                        f"t.dispatchEvent(new Event(\"{name}\")); }}")
        else:
            key = n()
            body.append(f"{{ const table: Record<string, () => void> = {{ {key}: {target} }}; table[\"{key}\"](); }}")
    for name, body in bodies.items():
        out.append(f"export function {name}(): void {{\n  enter(\"{name}\");\n" +
                   "".join(f"  {s}\n" for s in body) + "  leave();\n}\n")
    return "\n".join(out + extra) + "\n"


def write_gd(rng, n, names, links) -> str:
    out = ["extends RefCounted", "", "var sink = null"]
    bodies = {name: [] for name in ["start"] + names}
    extra, fields, signals = [], [], []
    for source, kind, target in links:
        body = bodies[source]
        if kind in ("direct", "derived", "overload", "hidden", "samename", "lambda", "template"):
            body.append(f"{target}()")
        elif kind == "value":
            depth = rng.randint(1, 3)
            chain = [n() for _ in range(depth)]
            decoy, other = n(), n()
            for i, c in enumerate(chain):
                call = "g.call()" if i == depth - 1 else f"{chain[i + 1]}(g)"
                extra.append(f"func {c}(g):\n\tsink.enter(\"{c}\")\n\t{call}\n\tsink.leave()\n")
            extra.append(f"func {other}():\n\tsink.enter(\"{other}\")\n\tsink.leave()\n")
            extra.append(f"func {decoy}():\n\tsink.enter(\"{decoy}\")\n\t{chain[0]}({other})\n\tsink.leave()\n")
            body.append(f"{chain[0]}({target})")
        elif kind == "field":
            held = n()
            fields.append(f"var {held} = null")
            body += [f"{held} = {target}", f"{held}.call()"]
        elif kind == "collection":
            items = n()
            fields.append(f"var {items} = []")
            body += [f"{items} = [{target}]", f"{items}[0].call()"]
        elif kind == "event":
            name = n()
            signals.append(f"signal {name}")
            body += [f"{name}.connect({target})", f"{name}.emit()"]
        else:
            body.append(f"call(\"{target}\")")
    out += signals + fields + [""]
    for name, body in bodies.items():
        out.append(f"func {name}():\n\tsink.enter(\"{name}\")\n" + "".join(f"\t{s}\n" for s in body) +
                   "\tsink.leave()\n")
    return "\n".join(out + extra) + "\n"




CPP_SINK = """#include <cstdio>
#include <string>
#include <vector>
static std::vector<std::string> stack;
void enter(const char *id) {
  std::printf("edge %s %s\\n", stack.empty() ? "-" : stack.back().c_str(), id);
  stack.push_back(id);
}
void leave() { stack.pop_back(); }
"""

CPP_DRIVER = """#include "program.cpp"
int main() { start(); return 0; }
"""


def write_cpp(rng, n, names, links) -> str:
    top = ["#include <functional>", "#include <vector>", "void enter(const char *id);", "void leave();"]
    top += [f"void {name}();" for name in ["start"] + names]
    bodies = {name: [] for name in ["start"] + names}
    extra = []
    for source, kind, target in links:
        body = bodies[source]
        if kind == "derived":
            base, kid, m = n(True), n(True), n()
            extra += [f"struct {base} {{ virtual void {m}() {{ enter(\"{base}.{m}\"); leave(); }} }};",
                      f"struct {kid} : {base} {{ void {m}() override {{ enter(\"{kid}.{m}\"); {target}(); leave(); }} }};"]
            body.append(f"{{ {kid} k; {base} &o = k; o.{m}(); }}")
        elif kind == "value":
            depth = rng.randint(1, 3)
            chain = [n() for _ in range(depth)]
            decoy, other = n(), n()
            top += [f"void {c}(void (*g)());" for c in chain] + [f"void {other}();", f"void {decoy}();"]
            for i, c in enumerate(chain):
                call = "g();" if i == depth - 1 else f"{chain[i + 1]}(g);"
                extra.append(f"void {c}(void (*g)()) {{ enter(\"{c}\"); {call} leave(); }}")
            extra.append(f"void {other}() {{ enter(\"{other}\"); leave(); }}")
            extra.append(f"void {decoy}() {{ enter(\"{decoy}\"); {chain[0]}({other}); leave(); }}")
            body.append(f"{chain[0]}({target});")
        elif kind == "field":
            hub, held, fire = n(True), n(), n()
            extra.append(f"struct {hub} {{ void (*{held})() = nullptr; "
                         f"void {fire}() {{ enter(\"{hub}.{fire}\"); {held}(); leave(); }} }};")
            body.append(f"{{ {hub} h; h.{held} = {target}; h.{fire}(); }}")
        elif kind == "collection":
            box, items, run = n(True), n(), n()
            extra.append(f"struct {box} {{ std::vector<std::function<void()>> {items}; "
                         f"void {run}() {{ enter(\"{box}.{run}\"); {items}[0](); leave(); }} }};")
            body.append(f"{{ {box} b; b.{items}.push_back({target}); b.{run}(); }}")
        elif kind == "overload":
            f, other = n(), n()
            top.append(f"void {other}();")
            extra += [f"void {f}(int) {{ enter(\"{f}\"); {target}(); leave(); }}",
                      f"void {f}(double) {{ enter(\"{f}\"); {other}(); leave(); }}",
                      f"void {other}() {{ enter(\"{other}\"); leave(); }}"]
            body.append(f"{f}(1);")
        elif kind in ("hidden", "samename", "template"):
            a, b, m, other, run = n(True), n(True), n(), n(), n()
            top.append(f"void {other}();")
            below = f" : {a}" if kind == "hidden" else ""
            extra += [f"struct {a} {{ void {m}() {{ enter(\"{a}.{m}\"); {target}(); leave(); }} }};",
                      f"struct {b}{below} {{ void {m}() {{ enter(\"{b}.{m}\"); {other}(); leave(); }} }};",
                      f"void {other}() {{ enter(\"{other}\"); leave(); }}"]
            if kind == "hidden":
                body.append(f"{{ {b} k; {a} &o = k; o.{m}(); }}")
            elif kind == "template":
                extra.append(f"template <class T> void {run}(T &t) {{ enter(\"{run}\"); t.{m}(); leave(); }}")
                body.append(f"{{ {a} x; {run}(x); }}")
            else:
                body.append(f"{{ {a} x; x.{m}(); }}")
        elif kind == "lambda":
            run, decoy, other = n(), n(), n()
            top += [f"void {run}(const std::function<void()> &g);", f"void {other}();", f"void {decoy}();"]
            extra += [f"void {run}(const std::function<void()> &g) {{ enter(\"{run}\"); g(); leave(); }}",
                      f"void {other}() {{ enter(\"{other}\"); leave(); }}",
                      f"void {decoy}() {{ enter(\"{decoy}\"); {run}([]() {{ {other}(); }}); leave(); }}"]
            body.append(f"{run}([]() {{ {target}(); }});")
        else:                                    # direct (and what the language has no own way for: an event, a text)
            body.append(f"{target}();")
    out = top + [""] + extra + [""]
    for name, body in bodies.items():
        out.append(f"void {name}() {{\n  enter(\"{name}\");\n" + "".join(f"  {s}\n" for s in body) +
                   "  leave();\n}\n")
    return "\n".join(out) + "\n"


def made(rng: random.Random) -> Linked:
    n = Names(rng)
    names, links = plan(rng, n)
    out = Linked(plan=links)
    out.files["ts"] = {"program.mts": write_ts(rng, n, names, links), "driver.mts": TS_SINK}
    out.files["gd"] = {"program.gd": write_gd(rng, n, names, links), "sink.gd": GD_SINK, "driver.gd": GD_DRIVER,
                       "project.godot": "config_version=5\n"}
    out.files["cpp"] = {"program.cpp": write_cpp(rng, n, names, links), "sink.cpp": CPP_SINK,
                        "driver.cpp": CPP_DRIVER}
    return out
