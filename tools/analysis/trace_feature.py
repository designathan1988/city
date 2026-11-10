"""Trace one feature end to end through the code, by the rules of its languages (GDScript and C++ with Godot).

From a starting point it resolves every name used to its definition (scope, class and bases, namespaces, imports and
preloads, the engine's names: bindings, signals, scenes, autoloads), opens each definition and does the same inside,
follows every branch, and comes back through each return; then goes up from the starting point to the entry points
(engine callbacks, engine signals, bound names). Nothing about this project is written in the tool.

    python tools/analysis/trace_feature.py CityCore::build_road_path
    python tools/analysis/trace_feature.py project/game/tools/road_tool.gd:230 --out dist/trace/road.txt
    python tools/analysis/trace_feature.py RoadTool._lay --exact --depth 6 --json dist/trace/road.json

Exit code: 0 traced, 2 the starting point names no definition, 1 the code could not be read.
"""
from __future__ import annotations

import argparse
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from featuretrace import program, report, seeds, walk  # noqa: E402

REPO = Path(__file__).resolve().parents[2]
DEFAULT_ROOTS = (REPO / "project", REPO / "native" / "src")
CACHE = REPO / "dist" / "cache" / "feature_trace"


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("start", help="file:line, Class.method, ns::Class::method or a bare name")
    parser.add_argument("--root", action="append", type=Path, help="folders or files to read (default: project and "
                        "native/src)")
    parser.add_argument("--exact", action="store_true", help="follow only links whose target the code types; "
                        "leave out candidates reached by name alone")
    parser.add_argument("--depth", type=int, default=None, help="stop the forward walk this deep")
    parser.add_argument("--back-depth", type=int, default=None, help="stop the walk back this deep")
    parser.add_argument("--out", type=Path, help="write the text report here as well")
    parser.add_argument("--json", type=Path, help="write the trace as JSON here")
    parser.add_argument("--no-cache", action="store_true", help="read every file again")
    args = parser.parse_args(argv)

    began = time.perf_counter()
    roots = [p.resolve() for p in (args.root or DEFAULT_ROOTS)]
    missing = [str(p) for p in roots if not p.exists()]
    if missing:
        print(f"no such root: {', '.join(missing)}", file=sys.stderr)
        return 1

    def progress(message: str) -> None:
        print(f"[{time.perf_counter() - began:5.1f}s] {message}", file=sys.stderr, flush=True)

    index, rs, count, broken = program.load(roots, REPO, None if args.no_cache else CACHE, progress)
    progress(f"{count} files, {len(index.units)} definitions, {len(rs.edges)} links"
             + (f"; {len(broken)} files with syntax the grammar did not fully parse" if broken else ""))
    start = seeds.find(index, args.start)
    if not start:
        print(f"'{args.start}' names no definition in the code read", file=sys.stderr)
        return 2
    up = walk.back(rs, start, args.exact, args.back_depth)
    down = walk.forward(rs, start, args.exact, args.depth)
    text = report.text(index, rs, args.start, start, up, down)
    sys.stdout.reconfigure(encoding="utf-8")
    print(text, end="")
    for path, body in ((args.out, text), (args.json, report.to_json(index, rs, args.start, start, up, down)
                                           if args.json else None)):
        if path is not None:
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(body, encoding="utf-8")
            progress(f"written {path}")
    progress("done")
    return 0


if __name__ == "__main__":
    sys.exit(main())
