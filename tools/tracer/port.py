"""The port driver: the reference's features against this game's by the tracer, in one process (each set of roots
analysed once, every question asked of it in memory), so a port is read off both programs and proved by them.

    python tools/tracer/port.py sheet <area> [--feature N] [--json]
        Each row of the area's sheet (doc/specs/003-roadcraft-features/sheets/<area>.md, its table of features): the files
        its evidence names (`file:line` in its cells), found in both games; the tracer's sheet of each side from those
        points (outputs, conditions, state read and written, constants, data); what one side holds that the other does not;
        the cost of each side's functions (loops and their order: the reference's faults never to bring). The report:
        dist/port/<area>.json; the summary on the console.
    python tools/tracer/port.py check [DIR]
        Every comparison pair under DIR (default project/tests/port): the reference's map and this game's (`trace.py
        --compare` maps) run in the same situations, swept; a pair passes only where no band differs but in the ways it
        declares (`expected`), and never on a comparison of nothing or of a run that stayed partial. Exit 1 on a
        failure.

A pair (JSON): {"name": "...", "reference": {map}, "city": {map}, "shared": {"v": 10}, "sweep": "v:0:20:9",
"expected": ["other values: return"]}: each map as compare.py reads it, its root relative to its game.
"""
from __future__ import annotations

import argparse
import json
import re
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[1]
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(REPO / "tools"))

from tracer import analyse, compare, cost, feature  # noqa: E402

SHEETS = REPO / "doc" / "specs" / "003-roadcraft-features" / "sheets"
REPORTS = REPO / "dist" / "port"
PAIRS = REPO / "project" / "tests" / "port"
CITY_TREES = ("native/src", "project/game", "project/ui", "project/render", "project/data")
REFERENCE_CODE = (".ts", ".tsx", ".mts", ".js")
CODE = (".ts", ".tsx", ".mts", ".js", ".cpp", ".h", ".gd", ".gdshader", ".gdshaderinc")
SKIPPED = ("node_modules", ".godot", "addons", "__pycache__")
REF_TOKEN = re.compile(r"([\w./-]+\.(?:ts|tsx|mts|js|cpp|h|gd|gdshader|gdshaderinc|json|csv))(?::(\d+)(?:-\d+)?)?")
ROW = re.compile(r"^\|\s*(\d+)\s*\|(.+)\|\s*$")
PROBLEM = re.compile(r"\bP[a-z]+-\d+\b")     # a problem's id in the register
SHOWN = 12                       # how many of each list the console summary shows


def reference_root() -> Path:
    from tasks.locations import locate
    return Path(locate("roadcraft_lab")) / "src"


def files_by_name(roots: list) -> dict:
    """Every code and data file under the roots by its file name (a name may be several files)."""
    out = {}
    for root in roots:
        for path in Path(root).rglob("*"):
            if path.is_file() and not any(part in SKIPPED for part in path.parts):
                out.setdefault(path.name, []).append(path)
    return out


def resolve(written: str, index: dict) -> Path | None:
    """The file a sheet's `file` names: the one of that name whose path ends as written; one alone, else none."""
    found = [p for p in index.get(Path(written).name, []) if feature.same_file(p.as_posix(), written)]
    return found[0] if len(found) == 1 else None


def rows_of(area: str) -> list:
    """The features of the area's sheet: (number, the row's cells), from its table's numbered rows."""
    text = (SHEETS / f"{area}.md").read_text(encoding="utf-8")
    return [(int(m.group(1)), [c.strip() for c in m.group(2).split("|")]) for m in map(ROW.match, text.splitlines()) if m]


def register() -> dict:
    """The problem register by id (tools/atlas/problems.py, its one owner)."""
    from atlas import problems
    return {entry["id"]: entry for entry in problems.read(str(REPO / problems.REGISTER))}


def cited(cell: str, problems: dict) -> list:
    """The text a cell gives as evidence: the cell, and for each problem it names, the files the problem's owner,
    symptom, cause and criterion write."""
    texts = [cell]
    for name in PROBLEM.findall(cell):
        entry = problems.get(name)
        if entry:
            texts += [entry.get("owner") or ""] + [entry.get(k) or "" for k in ("symptom", "cause", "criterion")]
            texts += [str(e) for e in entry.get("evidence") or []]
    return texts


def points_of(cells: list, city: dict, reference: dict, problems: dict) -> dict:
    """The evidence a row writes, each `file[:line]` resolved in the game it belongs to: the reference's code by its own
    extensions, any other file in this game first, else in the reference; a problem's id by the files it names."""
    out = {"city": [], "reference": [], "unresolved": []}
    for cell in cells:
        for text in cited(cell, problems):
            for name, line in REF_TOKEN.findall(text):
                side = "reference" if name.endswith(REFERENCE_CODE) else "city"
                path = resolve(name, reference if side == "reference" else city)
                if path is None and side == "city":
                    path, side = resolve(name, reference), "reference"
                if path is None:
                    out["unresolved"].append(f"{name}:{line}" if line else name)
                elif (path, int(line) if line else None) not in out[side]:
                    out[side].append((path, int(line) if line else None))
    out["unresolved"] = sorted(set(out["unresolved"]))
    return out


def code_points(points: list) -> list:
    """The points in code the tracer reads (a line, or a file whole: every output and write in it); a data file is
    evidence for the sheet, not a point to trace from."""
    return [(p, line) for p, line in points if p.suffix in CODE]


def roots_of(rows: list) -> tuple:
    """One side's roots for the whole area: every folder its evidence is in (one analysis per side, every feature of
    the area asked of it, a call from one of its folders to another linked)."""
    return tuple(sorted({p.parent.as_posix() for points in rows for p, _ in code_points(points)}))


def analysed(roots: tuple):
    began = time.monotonic()
    result = analyse(list(roots), keep=True)
    print(f"  analysed {len(roots)} folder(s), {len(result.program.files)} files, in {time.monotonic() - began:.1f} s"
          + (f" (stopped: {result.cut})" if result.cut else ""), file=sys.stderr, flush=True)
    return result


def side_sheet(result, points: list) -> dict:
    """The tracer's sheet of one side's points, with the cost of what sets them off kept to the feature's own
    functions (the loops it runs, around its outputs or in the functions on the way)."""
    code = code_points(points)
    if not code or result is None:
        return {"points": [], "note": "no point in code: nothing to trace on this side"}
    wanted = [f"{p.as_posix()}:{line}" if line else p.as_posix() for p, line in code]
    made = feature.sheet(result, wanted)
    out = feature.data_of(result, wanted, made)
    slice_units = feature.units_of(result, wanted)
    starts = [root for root, _, _ in made.roots] or sorted({result.cases.effects[i].unit for i in made.outputs})
    out["the cost"] = cost.data(result, starts, slice_units) if starts else []
    out["points"] = wanted
    out["points with no output"] = [w for w in wanted if not feature.points(result, [w])]
    return out


def differences(one: dict, two: dict) -> dict:
    """What one side's sheet holds that the other's does not: constants, state, data; and the counts of each."""
    def held(s: dict, key: str) -> set:
        return set(s.get(key, []))
    out = {}
    for key in ("the constants", "state read", "state written", "the data"):
        out[f"{key}: the reference's alone"] = sorted(held(one, key) - held(two, key))
        out[f"{key}: this game's alone"] = sorted(held(two, key) - held(one, key))
    for key in ("the outputs", "conditions at", "the functions", "the cost"):
        out[f"{key}: counts (reference, this game)"] = [len(one.get(key, [])), len(two.get(key, []))]
    out["the reference's loops past linear"] = [c for c in one.get("the cost", [])
                                               if c["the order"] not in ("constant", "linear")]
    out["this game's loops past linear"] = [c for c in two.get("the cost", [])
                                           if c["the order"] not in ("constant", "linear")]
    return out


def sheet(area: str, only: int | None, as_json: bool) -> int:
    city = files_by_name([REPO / t for t in CITY_TREES])
    reference = files_by_name([reference_root()])
    problems = register()
    rows = [(number, cells, points_of(cells, city, reference, problems)) for number, cells in rows_of(area)
            if only is None or number == only]
    if not rows:
        print(f"no numbered row in {SHEETS / (area + '.md')}" + (f" is {only}" if only is not None else ""),
              file=sys.stderr)
        return 1
    results = {}
    for side in ("reference", "city"):
        roots = roots_of([found[side] for _, _, found in rows])
        print(f"{side}: {len(roots)} folder(s)", file=sys.stderr, flush=True)
        results[side] = analysed(roots) if roots else None
    report = {"area": area, "roots": {side: list(roots_of([f[side] for _, _, f in rows])) for side in results},
              "features": []}
    for number, cells, found in rows:
        print(f"feature {number}: {cells[0][:90]}", file=sys.stderr, flush=True)
        one = side_sheet(results["reference"], found["reference"])
        two = side_sheet(results["city"], found["city"])
        report["features"].append({"number": number, "feature": cells[0], "reference": one, "this game": two,
                                   "differences": differences(one, two), "unresolved": found["unresolved"]})
    REPORTS.mkdir(parents=True, exist_ok=True)
    out = REPORTS / f"{area}.json"
    out.write_text(json.dumps(report, indent=1, default=str), encoding="utf-8")
    if as_json:
        print(json.dumps(report, indent=1, default=str))
        return 0
    for f in report["features"]:
        print(f"\n{f['number']}. {f['feature'][:100]}")
        for side, name in (("reference", "the reference"), ("this game", "this game")):
            if not f[side].get("points"):
                print(f"   {name}: no point of its code in the sheet's evidence (name its outputs, `file:line`)")
        for key, value in f["differences"].items():
            if value and value != [0, 0]:
                if isinstance(value[0], dict):           # loops: where each is and its order
                    value = [f"{c['where it is']} {c['the order']}" for c in value]
                shown = value[:SHOWN] if isinstance(value[0], str) else value
                print(f"   {key}: {shown}" + (f" (+{len(value) - SHOWN})" if len(value) > SHOWN and shown is not value
                                              else ""))
        if f["unresolved"]:
            print(f"   not found in either game: {f['unresolved'][:SHOWN]}")
    print(f"\nreport: {out}")
    return 0


def map_of(held: dict, root: Path) -> dict:
    """A pair's map with its roots (a path or a list) made absolute in its game."""
    written = held["root"] if isinstance(held["root"], list) else [held["root"]]
    return {**held, "root": [str((root / r).resolve()) if not Path(r).is_absolute() else r for r in written]}


def check(folder: Path) -> int:
    pairs = sorted(folder.glob("*.json"))
    if not pairs:
        print(f"no comparison pair under {folder}: nothing proved", file=sys.stderr)
        return 1
    failed = 0
    scratch = REPORTS / "maps"
    scratch.mkdir(parents=True, exist_ok=True)
    for path in pairs:
        pair = json.loads(path.read_text(encoding="utf-8"))
        maps = []
        for side, root in (("reference", reference_root()), ("city", REPO)):
            file = scratch / f"{path.stem}.{side}.json"
            file.write_text(json.dumps(map_of(pair[side], root)), encoding="utf-8")
            maps.append(str(file))
        sweep = None
        if pair.get("sweep"):
            name, low, high, count = pair["sweep"].split(":")
            sweep = (name, float(low), float(high), int(count))
        done = compare.compare(maps[0], maps[1], pair.get("shared", {}), sweep)
        expected = set(pair.get("expected", []))
        bad = [(band, kind) for band in done.bands for kind, a, b in band.found
               if kind in compare.UNPROVEN or f"{kind}: {(a or b).callee if (a or b) else ''}" not in expected]
        if bad:
            failed += 1
            print(f"FAIL {pair.get('name', path.stem)}")
            print(compare.text_report(done))
        else:
            print(f"ok   {pair.get('name', path.stem)}: {len(done.bands)} band(s), the same but as declared")
    print(f"FAILURES: {failed}")
    return 1 if failed else 0


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    sub = parser.add_subparsers(dest="action", required=True)
    s = sub.add_parser("sheet", help="each feature of an area's sheet, traced in both games and compared")
    s.add_argument("area")
    s.add_argument("--feature", type=int, help="only this row of the sheet")
    s.add_argument("--json", action="store_true")
    c = sub.add_parser("check", help="every comparison pair proved by --compare")
    c.add_argument("folder", nargs="?", default=str(PAIRS))
    args = parser.parse_args(argv)
    if args.action == "sheet":
        return sheet(args.area, args.feature, args.json)
    return check(Path(args.folder))


if __name__ == "__main__":
    sys.exit(main())
