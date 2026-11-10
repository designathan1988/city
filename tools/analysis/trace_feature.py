"""Trace one feature end to end through any code base written in GDScript and C++ (Godot and GDExtension).

From a starting point it resolves every name used to its definition by the languages' own rules (log/trace-rules.md:
scopes, classes and bases, namespaces, includes, preloads, the engine's names: bindings, signals, scenes, autoloads),
follows every value that flows into a name, opens each definition and does the same inside, follows every branch and
comes back through each return; then goes up from the starting point to the entry points the engine fires. The tool
knows no project: it reads the folders it is given, or the current folder.

    python trace_feature.py <start> [--root <folder or file> ...] [--out report.txt] [--json trace.json]

<start> is `file:line`, `Class.method`, `ns::Class::method` or a bare name.
Exit code: 0 traced, 2 the starting point names no definition, 1 a root does not exist.
"""
from __future__ import annotations

import argparse
import os
import sys
import tempfile
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from featuretrace import program, report, seeds, walk  # noqa: E402

CACHE = Path(tempfile.gettempdir()) / "feature-trace-cache"


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("start", help="file:line, Class.method, ns::Class::method or a bare name")
    parser.add_argument("--root", action="append", type=Path, help="a folder or file to read (repeatable; default: "
                        "the current folder)")
    parser.add_argument("--depth", type=int, default=None, help="stop the forward walk this deep")
    parser.add_argument("--back-depth", type=int, default=None, help="stop the walk back this deep")
    parser.add_argument("--out", type=Path, help="write the text report here as well")
    parser.add_argument("--json", type=Path, help="write the trace as JSON here")
    parser.add_argument("--cache", type=Path, default=CACHE, help="where facts read are kept by file content")
    parser.add_argument("--no-cache", action="store_true", help="read every file again")
    args = parser.parse_args(argv)

    began = time.perf_counter()
    roots = [p.resolve() for p in (args.root or [Path.cwd()])]
    missing = [str(p) for p in roots if not p.exists()]
    if missing:
        print(f"no such root: {', '.join(missing)}", file=sys.stderr)
        return 1
    base = Path(os.path.commonpath([r if r.is_dir() else r.parent for r in roots]))

    def progress(message: str) -> None:
        print(f"[{time.perf_counter() - began:5.1f}s] {message}", file=sys.stderr, flush=True)

    index, rs, count, broken = program.load(roots, base, None if args.no_cache else args.cache, progress)
    progress(f"{count} files, {len(index.units)} definitions, {len(rs.edges)} links"
             + (f"; {len(broken)} files with syntax the grammar did not fully parse" if broken else ""))
    start = seeds.find(index, args.start)
    if not start:
        print(f"'{args.start}' names no definition in the code read", file=sys.stderr)
        return 2
    up = walk.back(rs, start, args.back_depth)
    down = walk.forward(rs, start, args.depth)
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
