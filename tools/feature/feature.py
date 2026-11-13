"""A functionality read off a program's exact graph (cpp_graph.py, ts_graph.js, gd_graph.py), from the places where it
produces something: what sets each place off (every path from where the program is entered; a function handed to
another is reached only through the one that handed it), the conditions on each path, what the place does (its
calls and writes, with their conditions), what it runs, the state read on the way and who writes it, the named
constants and numbers and the data the texts it uses name, what leaves the program, and what each run costs (loops,
loops in loops, calls in loops; marked when a path from a frame root reaches them). With two programs, both side by
side, and the values one has that the other has not.

  python feature.py GRAPH[+GRAPH] POINT[,POINT] [--data DIR] [--vs GRAPH[+GRAPH] POINT[,POINT] [--vs-data DIR]]
                    [--frame NAME[,NAME]] [--depth N] [--out FILE.html]

A point is `path:line` (the innermost function written there; the path's end is enough) or a function's name.
A frame root is a function the platform runs every frame, or a platform function a handler is handed to (by name).
"""
from __future__ import annotations

import html
import json
import os
import re
import sys
import time
from collections import defaultdict, deque
from fractions import Fraction
from pathlib import Path

ROWS = ("calls", "hands", "invokes", "forwards", "stores", "field_calls", "fields", "loops", "texts", "numbers",
        "consts", "registers", "overrides")


def norm(p: str) -> str:
    return os.path.normcase(os.path.abspath(p)).replace("\\", "/")


class Graph:
    def __init__(self, spec: str):
        self.fn, self.roots_dirs = {}, []
        g = {k: [] for k in ROWS}
        for path in spec.split("+"):
            part = json.loads(Path(path).read_text(encoding="utf-8"))
            self.roots_dirs.append(part["root"])
            self.fn.update(part["functions"])
            for k in ROWS:
                g[k] += part.get(k, [])
        self.g = g
        self.callers, self.callees = defaultdict(list), defaultdict(list)
        for c in g["calls"]:
            self.callees[c["from"]].append(c)
            if not c["out"]:                        # also a declaration with no body (an interface's, a pure virtual)
                self.callers[c["to"]].append(c)
        self.bases, self.overriders = defaultdict(list), defaultdict(list)
        for o in g["overrides"]:                    # a method runs where what it overrides is called
            self.bases[o["fn"]].append(o["base"])
            self.overriders[o["base"]].append(o["fn"])
        self.handed_by, self.hands_from = defaultdict(list), defaultdict(list)
        for h in g["hands"]:
            self.handed_by[h["fn"]].append(h)
            self.hands_from[h["from"]].append(h)
        self.stored_fn = defaultdict(list)          # a function value -> the fields it is kept in
        self.stored_param = defaultdict(list)       # (function, parameter) -> the fields that parameter is kept in
        for s in g["stores"]:
            if "fn" in s:
                self.stored_fn[s["fn"]].append(s)
            else:
                self.stored_param[(s["in"], s["param"])].append(s)
        self.forwarded = defaultdict(list)          # (function, parameter) -> (callee, its parameter) it is passed on to
        for f in g["forwards"]:
            self.forwarded[(f["in"], f["param"])].append((f["to"], f["index"]))
        self.field_runs = defaultdict(list)
        for fc in g["field_calls"]:
            self.field_runs[fc["field"]].append(fc)
        self.uses, self.writers = defaultdict(list), defaultdict(list)
        for f in g["fields"]:
            self.uses[f["in"]].append(f)
            if f["write"]:
                self.writers[f["field"]].append(f)
        self.per = {k: defaultdict(list) for k in ("loops", "texts", "numbers", "consts")}
        for k in self.per:
            for row in g[k]:
                self.per[k][row["in"]].append(row)
        self.by_file = defaultdict(list)
        for i, f in self.fn.items():
            self.by_file[f["file"]].append(i)

    def short(self, file: str) -> str:
        for r in self.roots_dirs:
            if file.startswith(r):
                return file[len(r):].lstrip("/")
        return file

    def at(self, i: str) -> str:
        f = self.fn.get(i)
        return f"{self.short(f['file'])}:{f['line']}" if f else ""

    def name(self, i: str) -> str:
        f = self.fn.get(i)
        if f:
            return f["name"]
        if i.startswith("c:"):                     # a clang USR of something not defined here
            parts = re.findall(r"@(?:N|S|C|F|FT|E|U)@([A-Za-z_~]\w*)", i)
            return "::".join(dict.fromkeys(parts)) or i
        return i.replace("out:", "")

    def find(self, point: str) -> list:
        path, _, line = point.rpartition(":")
        if path and line.isdigit():
            want, line = path.replace("\\", "/").lower(), int(line)
            found = []
            for file, ids in self.by_file.items():
                if file == norm(path) or file.endswith("/" + want):
                    inside = [i for i in ids if self.fn[i]["line"] <= line <= self.fn[i]["end"]]
                    if inside:
                        found.append(min(inside, key=lambda i: self.fn[i]["end"] - self.fn[i]["line"]))
            return found
        return [i for i, f in self.fn.items() if f["name"] == point or re.search(r"(::|\.)" + re.escape(point) + "$", f["name"])]

    def before(self, i: str) -> list:
        """What runs a function: its callers; for a function handed as a value, the one that handed it (it runs inside
        what it was handed to, on that call); for one kept to be run later (a field, a signal), who runs what is kept."""
        out = [(c["from"], "calls" + (f" [{c['how']}]" if c.get("how", "exact") != "exact" else ""), c["line"], c["guards"])
               for c in self.callers.get(i, ())]
        seen_base, todo = set(), list(self.bases.get(i, ()))
        while todo:                                 # the calls of what it overrides, up the chain
            b = todo.pop()
            if b in seen_base:
                continue
            seen_base.add(b)
            out += [(c["from"], f"calls {self.name(b)} (overridden here)", c["line"], c["guards"]) for c in self.callers.get(b, ())]
            todo += self.bases.get(b, [])
        for h in self.handed_by.get(i, ()):
            out.append((h["from"], f"hands it to {self.name(h['to'])}", h["line"], h["guards"]))
            # what it is handed to may keep it in a field (directly or through the functions it passes it on to):
            # then it runs where that field is run
            seen, todo = set(), [(h["to"], h["index"])]
            while todo and len(seen) < 32:
                key = todo.pop()
                if key in seen:
                    continue
                seen.add(key)
                for s in self.stored_param.get(key, ()):
                    for fc in self.field_runs.get(s["field"], ()):
                        out.append((fc["in"], f"runs {s['field']} (handed by {self.name(h['from'])})", fc["line"], fc["guards"]))
                todo += self.forwarded.get(key, [])
        for s in self.stored_fn.get(i, ()):
            for fc in self.field_runs.get(s["field"], ()):
                out.append((fc["in"], f"runs {s['field']} (kept by {self.name(s['in'])})", fc["line"], fc["guards"]))
        return out

    def roots(self, starts: list, limit: int = 60) -> list:
        paths, queue, seen = [], deque([(s, [(s, None, None, [])]) for s in starts]), set()
        while queue and len(paths) < limit:
            at, path = queue.popleft()
            pre = self.before(at)
            if not pre:
                paths.append(list(reversed(path)))
                continue
            for src, how, line, guards in pre:
                if src in {p[0] for p in path}:
                    continue
                if (src, at) in seen:
                    paths.append(list(reversed(path + [(src, how + " (joins a way above)", line, guards)])))
                    continue
                seen.add((src, at))
                queue.append((src, path + [(src, how, line, guards)]))
        return paths

    def ahead(self, starts: list, depth: int) -> list:
        seen, order, queue = set(starts), [], deque((s, 0) for s in starts)
        while queue:
            at, d = queue.popleft()
            order.append((at, d))
            if d >= depth:
                continue
            nxt = [c["to"] for c in self.callees.get(at, ()) if not c["out"]] + [h["fn"] for h in self.hands_from.get(at, ())]
            nxt += [o for t in list(nxt) for o in self.overriders.get(t, ())]     # a call of a method runs its overrides
            for to in nxt:
                if to in self.fn and to not in seen:
                    seen.add(to)
                    queue.append((to, d + 1))
        return order


def number(v: str):
    """A number as the code writes it: decimal (a float's `f`, an integer's `u`/`l` suffix dropped) or hexadecimal,
    octal, binary."""
    v = v.lower().replace("_", "").replace("'", "")
    try:
        if v.startswith(("0x", "0b", "0o")):
            return Fraction(int(v.rstrip("ul"), 0))
        return Fraction(v.rstrip("ful")).limit_denominator(1 << 20)
    except (ValueError, ZeroDivisionError):
        return None


def data_values(data_dirs, texts: set) -> dict:
    """The values the data files hold under the texts the functionality uses as keys."""
    out = {}

    def walk(v, where, path):
        if isinstance(v, dict):
            for k, x in v.items():
                if k in texts and not isinstance(x, (dict, list)) or (k in texts and isinstance(x, list) and len(x) <= 8):
                    out.setdefault(f"{path}{k}", (x, where))
                walk(x, where, f"{path}{k}.")
        elif isinstance(v, list):
            for x in v[:200]:
                walk(x, where, path)

    for d in data_dirs:
        for p in Path(d).rglob("*.json"):
            try:
                walk(json.loads(p.read_text(encoding="utf-8")), p.name, f"{p.stem}:")
            except (ValueError, OSError):
                pass
    return out


def sheet(g: Graph, points: list, depth: int, frames: list, data_dirs: list) -> dict:
    starts = [i for p in points for i in g.find(p)]
    if not starts:
        return {"error": f"no function at {points}"}
    paths = g.roots(starts)
    framed = lambda p: any(f and (any(re.search(r"(::|\.)" + re.escape(f) + r"$", g.name(x[0])) or g.name(x[0]) == f for x in p)
                                  or any(f in (x[1] or "") for x in p)) for f in frames)
    every_frame = any(framed(p) for p in paths)
    on_way = {x[0] for p in paths for x in p}
    framed_fns = {x[0] for p in paths if framed(p) for x in p}     # on a way the platform runs every frame
    ahead = g.ahead(starts, depth)
    run = {i for i, _ in ahead}
    outputs = []
    for i in starts:
        for c in g.callees.get(i, ()):
            outputs.append({"at": f"{g.short(g.fn[i]['file'])}:{c['line']}", "in": g.name(i), "leaves": c["out"],
                            "calls": (c.get("name") or c["to"]).replace("out:", "") if c["out"] else g.name(c["to"]),
                            "when": c["guards"]})
        for u in g.uses.get(i, ()):
            if u["write"]:
                outputs.append({"at": f"{g.short(g.fn[i]['file'])}:{u['line']}", "in": g.name(i), "writes": u["field"],
                                "when": u["guards"]})
    near = run | {x[0] for p in paths for x in p[-3:]}       # the starts, what they run, and their nearest callers
    state = defaultdict(set)
    for i in near:
        for u in g.uses.get(i, ()):
            if not u["write"]:
                state[u["field"]].add(g.name(i))
    written_by = {f: sorted({f"{g.name(w['in'])} @{g.at(w['in'])}" for w in g.writers.get(f, ())})[:6] for f in state}
    # the texts of the functions near it and of those that write the state it reads (where data is loaded into it)
    loaders = {w["in"] for f in state for w in g.writers.get(f, ())}
    texts = {t["text"] for i in near | loaders for t in g.per["texts"].get(i, ()) if 0 < len(t["text"]) < 60}
    consts = {}
    for i in near:
        for k in g.per["consts"].get(i, ()):
            consts.setdefault(k["name"], (k["value"], g.name(i)))
    numbers = defaultdict(set)
    for i in near:
        for n in g.per["numbers"].get(i, ()):
            numbers[n["value"]].add(g.name(i))
    data = data_values(data_dirs, texts) if data_dirs else {}
    leaves = sorted({(c.get("name") or c["to"]).replace("out:", "") for i in run for c in g.callees.get(i, ()) if c["out"]})
    cost = []
    for i in sorted(run | on_way):
        for lp in g.per["loops"].get(i, ()):
            inside = sorted({g.name(c["to"]) if not c["out"] else (c.get("name") or c["to"]).replace("out:", "")
                             for c in g.callees.get(i, ()) if lp["line"] <= c["line"] <= lp["end"]})
            cost.append({"fn": g.name(i), "at": g.at(i), "line": lp["line"], "depth": lp["depth"], "cond": lp["cond"],
                         "calls": inside[:12],
                         # on a framed way, or run by a start that is on one
                         "every frame": i in framed_fns or (i in run and bool(framed_fns & set(starts)))})
    values = set()
    for v, _ in consts.values():
        values |= {number(x) for x in re.findall(r"-?\d+(?:\.\d+)?(?:e-?\d+)?", v)}
    values |= {number(v) for v in numbers}
    for v, _ in data.values():
        for x in (v if isinstance(v, list) else [v]):
            if isinstance(x, (int, float)) and not isinstance(x, bool):
                values.add(Fraction(x).limit_denominator(1 << 20))
    values.discard(None)
    return {"starts": [f"{g.name(i)}  {g.at(i)}" for i in starts], "every frame": every_frame,
            "paths": [[{"fn": g.name(f), "at": g.at(f), "how": how, "line": line, "when": guards} for f, how, line, guards in p]
                      for p in paths],
            "outputs": outputs, "ahead": [(g.name(i), g.at(i), d) for i, d in ahead],
            "state": {f: {"read by": sorted(r)[:6], "written by": written_by[f]} for f, r in sorted(state.items())},
            "consts": {k: v[0] for k, v in sorted(consts.items())}, "numbers": sorted(numbers),
            "data": {k: v[0] for k, v in sorted(data.items())}, "texts": sorted(texts), "leaves": leaves, "cost": cost,
            "values": sorted(values)}


def as_text(title: str, s: dict) -> str:
    if "error" in s:
        return f"== {title}: {s['error']}"
    L = [f"== {title}", "Starts:"] + [f"  {x}" for x in s["starts"]]
    L.append(f"What sets it off ({len(s['paths'])} ways; every frame: {'yes' if s['every frame'] else 'no'}):")
    for p in s["paths"]:
        L.append("  " + "  ->  ".join(f"{x['fn']} [{x['at']}]" + (f" ({x['how']})" if x["how"] and x["how"] != "calls" else "") for x in p))
        for x in p:
            if x["when"]:
                L.append(f"      at {x['fn']} line {x['line']}: when " + " && ".join(x["when"]))
    L.append("What it does at the starts (calls and writes, with their conditions):")
    for o in s["outputs"]:
        what = f"writes {o['writes']}" if "writes" in o else ("LEAVES -> " if o["leaves"] else "calls ") + o["calls"]
        L.append(f"  {o['at']}  {what}" + (f"   when {' && '.join(o['when'])}" if o["when"] else ""))
    L.append("What it runs:")
    L += [f"  {'  ' * d}{n} [{a}]" for n, a, d in s["ahead"][:80]]
    L.append("Named constants (as written):")
    L += [f"  {k} = {v}" for k, v in s["consts"].items()]
    L.append("Numbers written in place: " + ", ".join(s["numbers"][:150]))
    if s["data"]:
        L.append("Data under the texts it uses:")
        L += [f"  {k} = {json.dumps(v)}" for k, v in s["data"].items()]
    L.append("State read near it, and who writes it:")
    for f, v in s["state"].items():
        L.append(f"  {f}: read by {', '.join(v['read by'])}; written by {'; '.join(v['written by']) or 'nobody in the program'}")
    L.append("Leaves the program (engine, libraries): " + ", ".join(s["leaves"][:150]))
    L.append("Cost (loops of the functions on the way and run):")
    for r in s["cost"]:
        L.append(f"  {'EVERY FRAME ' if r['every frame'] else ''}{r['fn']} [{r['at']}] line {r['line']} depth {r['depth']}: "
                 f"{r['cond'][:100]}  calls {', '.join(r['calls'])}")
    return "\n".join(L)


def differences(a: dict, b: dict, an: str, bn: str) -> str:
    if "error" in a or "error" in b:
        return ""
    fa, fb = set(a["values"]), set(b["values"])
    show = lambda xs: ", ".join(f"{float(x):g}" for x in sorted(xs)[:200])
    L = [f"== Values {an} has that {bn} has not (constants, numbers, data): {show(fa - fb)}",
         f"== Values {bn} has that {an} has not: {show(fb - fa)}",
         f"== Values in both: {show(fa & fb)}",
         f"== Every frame: {an} {'yes' if a['every frame'] else 'no'}, {bn} {'yes' if b['every frame'] else 'no'}; "
         f"loops on the way: {an} {len(a['cost'])} ({sum(r['depth'] > 1 for r in a['cost'])} inside another), "
         f"{bn} {len(b['cost'])} ({sum(r['depth'] > 1 for r in b['cost'])} inside another)"]
    return "\n".join(L)


def main() -> int:
    a = sys.argv[1:]
    opt = lambda k, d=None: a[a.index(k) + 1] if k in a else d
    depth, out = int(opt("--depth", 3)), opt("--out")
    frames = [f for f in (opt("--frame", "") or "").split(",") if f]
    t0 = time.monotonic()
    sides = [(a[0], a[1].split(","), [d for d in (opt("--data", "") or "").split(",") if d])]
    if "--vs" in a:
        k = a.index("--vs")
        sides.append((a[k + 1], a[k + 2].split(","), [d for d in (opt("--vs-data", "") or "").split(",") if d]))
    texts, sheets = [], []
    for spec, points, data in sides:
        s = sheet(Graph(spec), points, depth, frames, data)
        title = Path(spec.split("+")[0]).stem
        sheets.append((title, s))
        texts.append(as_text(title, s))
    diff = differences(sheets[0][1], sheets[1][1], sheets[0][0], sheets[1][0]) if len(sheets) == 2 else ""
    report = (diff + "\n\n" if diff else "") + "\n\n".join(texts) + f"\n\n({time.monotonic() - t0:.1f} s)"
    if out:
        Path(out).with_suffix(".txt").write_text(report, encoding="utf-8")
        cols = "".join(f"<td><pre>{html.escape(t)}</pre></td>" for t in texts)
        Path(out).write_text(
            "<!doctype html><meta charset=utf-8><title>Feature</title><style>body{font:12px ui-monospace,monospace;margin:12px;"
            "background:#fff;color:#111}@media(prefers-color-scheme:dark){body{background:#151517;color:#eee}}"
            "td{vertical-align:top;border:1px solid #888;padding:6px;width:50%}pre{white-space:pre-wrap}</style>"
            f"<pre>{html.escape(diff)}</pre><table><tr>{cols}</tr></table>", encoding="utf-8")
        Path(out).with_suffix(".json").write_text(json.dumps(dict(sheets), indent=1, default=str), encoding="utf-8")
    print(report)
    return 0


if __name__ == "__main__":
    sys.exit(main())
