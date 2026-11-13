"""Oracle for C++: the compiler's own answer to "what does this call run?" (libclang, the compiler's library).
For every call to a function or a method written in the project's own files (operators and implicit calls left
out), the declaration and the definition the compiler resolves it to. Not part of the tracer: the bench compares
the tracer's links with these.

    python cpp_calls.py <compile_commands.json> <project folder> <out.jsonl> [--toolchain DIR] [--jobs N]

Each line: {"file", "line", "end": [line, column], "callee", "targets": [{"file", "line", "name"}], "outside"},
paths from the project folder, the end column counted from 0 as the grammar reader counts it.
"""
from __future__ import annotations

import argparse
import json
import os
import shlex
import sys
from concurrent.futures import ProcessPoolExecutor
from pathlib import Path

DROP_WITH_VALUE = {"-o", "-MF", "-MT", "-MQ"}
DROP = {"-c", "-MD", "-MMD", "-Winvalid-pch"}


def arguments(entry: dict, toolchain: str | None) -> list:
    words = entry.get("arguments") or shlex.split(entry["command"].replace("\\", "/"), posix=True)
    out, skip = [], False
    for word in words[1:]:
        if skip:
            skip = False
            continue
        if word in DROP_WITH_VALUE:
            skip = True
            continue
        if word in DROP or word.endswith((".cpp", ".cc", ".cxx")) or word.startswith("-W"):
            continue
        out.append(word)
    if toolchain:
        tc = Path(toolchain)
        triple = next((p.name for p in tc.iterdir() if p.name.endswith("-w64-mingw32") and (p / "include").is_dir()), None)
        out += ["-nostdinc++", "-isystem", str(tc / "include" / "c++" / "v1")]
        if triple:
            out += [f"--target={triple}", "-isystem", str(tc / triple / "include")]
    return out


def norm(path) -> str:
    return os.path.normcase(os.path.abspath(str(path))).replace("\\", "/")


def one(job):
    """The calls of one translation unit, as rows."""
    import clang.cindex as ci
    path, args, project = job
    index = ci.Index.create()
    try:
        tu = index.parse(path, args=args)
    except ci.TranslationUnitLoadError:
        return [], 1
    errors = sum(1 for d in tu.diagnostics if d.severity >= ci.Diagnostic.Error)
    kinds = {ci.CursorKind.FUNCTION_DECL, ci.CursorKind.CXX_METHOD, ci.CursorKind.FUNCTION_TEMPLATE}
    rows = []
    root = norm(project) + "/"

    def own(f) -> bool:
        return f is not None and norm(f.name).startswith(root)

    def rel(f) -> str:
        return norm(f.name)[len(root):]

    stack = [tu.cursor]
    while stack:
        cur = stack.pop()
        for child in cur.get_children():
            loc = child.location
            if loc.file is not None and not own(loc.file):
                continue
            stack.append(child)
            if child.kind != ci.CursorKind.CALL_EXPR:
                continue
            ref = child.referenced
            if ref is None or ref.kind not in kinds or ref.spelling.startswith("operator"):
                continue
            ext = child.extent
            if ext.start.file is None or ext.end.file is None or ext.start.line == 0:
                continue
            targets, outside = [], True
            for decl in {ref, ref.get_definition()} - {None}:
                if decl.location.file is not None and own(decl.location.file):
                    outside = False
                    targets.append({"file": rel(decl.location.file), "line": decl.location.line, "name": decl.spelling})
            rows.append({"file": rel(ext.start.file), "line": ext.start.line, "end": [ext.end.line, ext.end.column - 1],
                         "callee": ref.spelling, "targets": targets, "outside": outside and not targets})
    return rows, errors


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("database")
    parser.add_argument("project")
    parser.add_argument("out")
    parser.add_argument("--toolchain", help="a clang toolchain folder whose headers the build used")
    parser.add_argument("--jobs", type=int, default=os.cpu_count())
    args = parser.parse_args(argv)
    project = norm(args.project)
    entries = [e for e in json.loads(Path(args.database).read_text(encoding="utf-8"))
               if norm(e["file"]).startswith(project + "/")]
    jobs = [(e["file"], arguments(e, args.toolchain), args.project) for e in entries]
    seen, rows, errors = set(), [], 0
    with ProcessPoolExecutor(args.jobs) as pool:
        for found, wrong in pool.map(one, jobs):
            errors += wrong
            for row in found:
                key = (row["file"], row["end"][0], row["end"][1])
                if key not in seen:
                    seen.add(key)
                    rows.append(row)
    Path(args.out).write_text("".join(json.dumps(r) + "\n" for r in rows), encoding="utf-8")
    print(f"{len(entries)} units, {len(rows)} calls, {errors} compiler errors -> {args.out}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
