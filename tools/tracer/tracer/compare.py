"""Two programs that do the same thing (a reference and one being built), or two versions of one, compared from their
code alone (neither is run): each evaluated in the same situation, given in variables of its own the two share; each
program's short map says how its inputs come from those variables, where it starts and which of its outputs are the
functionality. Over a range of one variable, the bands where the outputs differ (one happens and not the other, other
values, another order), with why on each side; of two versions, the lines changed that the differing outputs went
through.

A map (JSON): {"root": "<folder>", "start": "<label>", "inputs": {"<input>": "<expression of the shared variables>"},
"given": {"<input>": <value>}, "outputs": ["<what is called, or file:line>", ...]}
"""
from __future__ import annotations

import ast
import builtins
import difflib
import json
import math
from dataclasses import dataclass, field
from pathlib import Path

from .evaluate import evaluate, plain, why_lines, why_not

CLOSE = 1e-9                     # values this near are the same (said where it matters)


def value_of(expression: str, names: dict):
    """An expression of the shared variables: numbers, the four operations, powers, brackets, and the functions of
    Python's own math (sqrt, sin, radians, ...); nothing else is read."""
    def ev(node):
        if isinstance(node, ast.Expression):
            return ev(node.body)
        if isinstance(node, ast.Constant) and isinstance(node.value, (int, float, bool, str)):
            return node.value
        if isinstance(node, ast.Name):
            return names[node.id]
        if isinstance(node, ast.UnaryOp) and isinstance(node.op, (ast.USub, ast.UAdd, ast.Not)):
            v = ev(node.operand)
            return -v if isinstance(node.op, ast.USub) else (not v if isinstance(node.op, ast.Not) else v)
        if isinstance(node, ast.BinOp):
            a, b = ev(node.left), ev(node.right)
            ops = {ast.Add: lambda: a + b, ast.Sub: lambda: a - b, ast.Mult: lambda: a * b, ast.Div: lambda: a / b,
                   ast.Pow: lambda: a ** b, ast.Mod: lambda: a % b, ast.FloorDiv: lambda: a // b}
            return ops[type(node.op)]()
        if isinstance(node, ast.Call) and isinstance(node.func, ast.Name) and not node.keywords:
            fn = getattr(math, node.func.id, None)
            if fn is None:
                fn = getattr(builtins, node.func.id, None)
                if fn not in (min, max, abs, round):
                    raise ValueError(node.func.id)
            return fn(*[ev(a) for a in node.args])
        raise ValueError(ast.dump(node)[:60])
    return ev(ast.parse(expression, mode="eval"))


@dataclass
class Side:
    """One program as a map says it."""
    root: str
    start: str
    inputs: dict
    given: dict
    outputs: list
    result: object = None
    starts: list = field(default_factory=list)


def side(path: str) -> Side:
    held = json.loads(Path(path).read_text(encoding="utf-8"))
    base = Path(path).parent
    root = str((base / held["root"]).resolve()) if not Path(held["root"]).is_absolute() else held["root"]
    return Side(root, held["start"], held.get("inputs", {}), held.get("given", {}), held.get("outputs", []))


def ready(s: Side) -> Side:
    from . import analyse
    from .walk import find
    if s.result is None:
        s.result = analyse([s.root], s.root, keep=True)
        s.starts = find(s.result, s.start)
    return s


def outputs_of(s: Side, run) -> list:
    """The outputs of the functionality in a run, in order: (what is called, its values, the output)."""
    out = []
    for o in run.outputs:
        if not s.outputs or o.callee in s.outputs or any(o.where.endswith(w) or w.endswith(o.where)
                                                         for w in s.outputs):
            out.append((o.callee, [plain(a) for a in o.args], o))
    return out


def run_side(s: Side, shared: dict):
    situation = dict(s.given)
    for name, expression in s.inputs.items():
        situation[name] = value_of(str(expression), shared)
    return evaluate(s.result, [(s.starts[0], [])], situation)


def same(a, b) -> bool:
    if isinstance(a, (int, float)) and isinstance(b, (int, float)) and not isinstance(a, bool):
        return abs(a - b) <= CLOSE * max(1.0, abs(a), abs(b))
    if isinstance(a, list) and isinstance(b, list):
        return len(a) == len(b) and all(same(x, y) for x, y in zip(a, b))
    return a == b


def differences(one: list, two: list) -> list:
    """How two lists of outputs differ: (kind, the output on one side or None, on the other or None)."""
    out = []
    names_one = [c for c, _, _ in one]
    names_two = [c for c, _, _ in two]
    for k, (callee, values, o) in enumerate(one):
        match = next((j for j, (c, v, _) in enumerate(two) if c == callee), None)
        if match is None:
            out.append(("only in the first", o, None))
        elif not same(values, two[match][1]):
            out.append(("other values", o, two[match][2]))
    for callee, values, o in two:
        if callee not in names_one:
            out.append(("only in the second", None, o))
    common = [c for c in names_one if c in names_two]
    if common != [c for c in names_two if c in names_one]:
        out.append(("another order", None, None))
    return out


def why_side(run, o) -> list:
    if o is None:
        return []
    lines = [f"{o.where}  {o.callee}({', '.join(repr(plain(a)) for a in o.args)})"]
    lines += [f"    because {w} was {'true' if t else 'false'}" for w, _, t in o.path[-4:]]
    for a in o.args[:3]:
        lines += ["    " + row for row in why_lines(a.why, limit=10)]
    return lines


def compare(path_one: str, path_two: str, shared: dict, sweep: tuple | None = None) -> dict:
    """Both programs in the same situations: one, or `sweep` = (variable, low, high, samples)."""
    one, two = ready(side(path_one)), ready(side(path_two))
    samples = [dict(shared)]
    if sweep is not None:
        name, low, high, count = sweep
        count = max(2, int(count))
        samples = [{**shared, name: low + (high - low) * k / (count - 1)} for k in range(count)]
    rows = []
    for situation in samples:
        run_one, run_two = run_side(one, situation), run_side(two, situation)
        found = differences(outputs_of(one, run_one), outputs_of(two, run_two))
        rows.append((situation, found, run_one, run_two))
    bands = []
    for situation, found, run_one, run_two in rows:
        key = tuple(sorted((kind, a.callee if a else "", b.callee if b else "") for kind, a, b in found))
        at = situation.get(sweep[0]) if sweep else None
        if bands and bands[-1].key == key:
            bands[-1].high = at
        else:
            bands.append(Band(key, at, at, found, (run_one, run_two)))
    return Done((one, two), rows, bands, sweep)


@dataclass
class Band:
    """A range of the swept variable where the two differ the same way (or do not differ)."""
    key: tuple
    low: object
    high: object
    found: list
    runs: tuple


@dataclass
class Done:
    sides: tuple
    rows: list
    bands: list
    sweep: tuple | None


def changed_lines(root_one: str, root_two: str) -> dict:
    """The lines changed between two versions: relative path -> the line numbers in the second."""
    out = {}
    a, b = Path(root_one), Path(root_two)
    for path in b.rglob("*"):
        if not path.is_file() or path.suffix.lower() not in source_suffixes():
            continue
        rel = path.relative_to(b).as_posix()
        old = (a / rel).read_text(encoding="utf-8", errors="replace").splitlines() if (a / rel).exists() else []
        new = path.read_text(encoding="utf-8", errors="replace").splitlines()
        lines = set()
        for tag, i1, i2, j1, j2 in difflib.SequenceMatcher(None, old, new).get_opcodes():
            if tag != "equal":
                lines |= set(range(j1 + 1, max(j2, j1 + 1) + 1))
        if lines:
            out[rel] = sorted(lines)
    return out


def source_suffixes() -> set:
    from .languages import BY_EXTENSION, DATA_FORMATS
    return set(BY_EXTENSION) | set(DATA_FORMATS)


def blamed(changed: dict, o) -> list:
    """The changed lines a differing output went through: its own line, the conditions it was taken under, where its
    values were written."""
    if o is None:
        return []
    places = {o.where} | {w for w, _, _ in o.path}
    for a in o.args:
        stack = [a.why]
        while stack:
            w = stack.pop()
            if w.where:
                places.add(w.where)
            stack.extend(w.parts)
    out = []
    for place in places:
        path, _, line = place.rpartition(":")
        for rel, lines in changed.items():
            if (path.endswith(rel) or rel.endswith(path)) and line.isdigit() and int(line) in lines:
                out.append(f"{rel}:{line}")
    return sorted(set(out))


def text_report(done, changed: dict | None = None) -> str:
    one, two = done.sides
    lines = [f"Comparing {one.start} ({one.root}) with {two.start} ({two.root}):"]
    sweep = done.sweep
    for band in done.bands:
        span = f"{sweep[0]} from {band.low:g} to {band.high:g}" if sweep else "in the situation given"
        if not band.found:
            lines.append(f"  {span}: the same outputs")
            continue
        lines.append(f"  {span}: they differ")
        run_one, run_two = band.runs
        for kind, a, b in band.found:
            lines.append(f"    {kind}: {a.callee if a else ''} {b.callee if b else ''}")
            for row in why_side(run_one, a):
                lines.append(f"      first:  {row}")
            for row in why_side(run_two, b):
                lines.append(f"      second: {row}")
            if a is None and b is not None:
                lines += [f"      first, why not: {row}" for row in why_not(run_one, b.where)[:4]]
            if changed is not None:
                lines += [f"      changed line: {where}" for where in blamed(changed, b or a)]
    return "\n".join(lines)


def data(done, changed: dict | None = None) -> dict:
    one, two = done.sides
    sweep = done.sweep
    out = {"the first": one.start, "the second": two.start, "the bands": []}
    for band in done.bands:
        run_one, run_two = band.runs
        out["the bands"].append({
            "from the value": band.low, "to the value": band.high,
            "the differences": [{"how they differ": kind, "the first": why_side(run_one, a),
                                 "the second": why_side(run_two, b),
                                 "changed lines": blamed(changed, b or a) if changed is not None else []}
                                for kind, a, b in band.found]})
    if sweep:
        out["swept over"] = sweep[0]
    return out
