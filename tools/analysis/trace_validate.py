"""The feature tracer checked against a real run of the program: the calls seen on its sampled stacks must be links the
tracer found (`featuretrace/validate.py`). Knows no project: the command, the folders and the modules are given.

    python trace_validate.py record --cmd "<command line>" --out stacks.json [--seconds S] [--interval-ms M] [--hidden]
    python trace_validate.py check stacks.json [--root <folder> ...] [--map <module file name>=<linker map>]
                                   [--symbolizer llvm-symbolizer] [--out report.txt]

A module is named by its linker map (lld's COFF map, made with `-Wl,-Map=<file>`: a stripped module keeps its layout)
or, when it keeps its symbol table or debug information, by llvm-symbolizer.
Exit code: 0 every call seen has a link; 4 some call has none; 1 nothing of the program was seen (no verdict).
"""
from __future__ import annotations

import argparse
import os
import shutil
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from featuretrace import program, runtime, validate  # noqa: E402
from trace_feature import CACHE  # noqa: E402


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = parser.add_subparsers(dest="action", required=True)
    rec = sub.add_parser("record", help="run a command and keep its sampled stacks")
    rec.add_argument("--cmd", required=True)
    rec.add_argument("--out", type=Path, required=True)
    rec.add_argument("--seconds", type=float, default=30)
    rec.add_argument("--interval-ms", type=float, default=5)
    rec.add_argument("--hidden", action="store_true", help="run it on a private desktop")
    chk = sub.add_parser("check", help="check the tracer's links against recorded stacks")
    chk.add_argument("stacks", type=Path)
    chk.add_argument("--root", action="append", type=Path)
    chk.add_argument("--map", action="append", default=[], help="<module file name>=<linker map> (repeatable)")
    chk.add_argument("--symbolizer", default=shutil.which("llvm-symbolizer") or "")
    chk.add_argument("--out", type=Path)
    chk.add_argument("--cache", type=Path, default=CACHE)
    args = parser.parse_args(argv)
    began = time.perf_counter()

    def progress(message: str) -> None:
        print(f"[{time.perf_counter() - began:6.1f}s] {message}", file=sys.stderr, flush=True)

    if args.action == "record":
        runtime.save(runtime.record(args.cmd, args.seconds, args.interval_ms, args.hidden, progress), args.out)
        progress(f"written {args.out}")
        return 0
    maps = {}
    for item in args.map:
        module, _, path = item.partition("=")
        if not path or not Path(path).is_file():
            print(f"--map {item}: give <module file name>=<an existing linker map>", file=sys.stderr)
            return 1
        maps[module] = Path(path)
    data = runtime.load(args.stacks)
    progress(f"{len(data['stacks'])} distinct stacks; naming frames")
    names = validate.frame_names(data, maps, args.symbolizer)
    roots = [p.resolve() for p in (args.root or [Path.cwd()])]
    base = Path(os.path.commonpath([r if r.is_dir() else r.parent for r in roots]))
    index, rs, count, _ = program.load(roots, base, args.cache, progress)
    progress(f"{count} files, {len(index.units)} definitions, {len(rs.edges)} links; checking")
    result = program.deep(lambda: validate.check(rs, data, names))
    text = validate.report(result)
    sys.stdout.reconfigure(encoding="utf-8")
    print(text, end="")
    if args.out:
        args.out.parent.mkdir(parents=True, exist_ok=True)
        args.out.write_text(text, encoding="utf-8")
    if not result.verdict:
        return 1
    return 4 if any(v is None for v in result.verdict.values()) else 0


if __name__ == "__main__":
    sys.exit(main())
