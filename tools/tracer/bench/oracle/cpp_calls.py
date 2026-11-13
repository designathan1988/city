"""Oracle for C++: the compiler's own answer to "what does this call run?" (libclang, the compiler's library).
For every call to a function or a method written in the project's own files (operators and implicit calls left
out), the declaration and the definition the compiler resolves it to. Not part of the tracer: the bench compares
the tracer's links with these.

    python cpp_calls.py <compile_commands.json> <project folder> <out.jsonl> [--toolchain DIR] [--libclang FILE] [--jobs N]

`--libclang`: the C API library of a clang as new as the toolchain's (its headers use what an older clang reads wrong). A
unit clang cannot read without errors makes no answer: the run says the first errors and exits 1 (a ruler with holes).

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
SHOWN = 8                        # the compiler errors a failed run shows


def arguments(entry: dict, toolchain: str | None) -> list:
    """The unit's own compile flags, and what the build's compiler adds to them that the library reads from nowhere: the
    target and options of the toolchain's configuration files for the compiler's name, its sysroot and its own builtin
    headers (the library would otherwise look for another target's headers in its own install)."""
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
        compiler = Path(words[0]).name.removesuffix(".exe")
        triple, _, driver = compiler.rpartition("-")
        out = config_flags(tc, triple, driver) + out + [f"--sysroot={tc}"]
        builtin = sorted((tc / "lib" / "clang").iterdir()) if (tc / "lib" / "clang").is_dir() else []
        if builtin:
            out += ["-resource-dir", str(builtin[-1])]
    return out


def config_flags(tc: Path, triple: str, driver: str) -> list:
    """The flags the toolchain's clang reads from its default configuration files (Clang User's Manual, Configuration
    files): `<triple>-<driver>.cfg`, else `<driver>.cfg` and `<triple>.cfg`, in the folder of the executable; the triple as
    written and as clang normalizes a MinGW one (`-mingw32` is `-windows-gnu`)."""
    folder = tc / "bin"
    triples = [triple] + ([triple.removesuffix("mingw32") + "windows-gnu"] if triple.endswith("-mingw32") else [])
    for t in triples:
        if (folder / f"{t}-{driver}.cfg").is_file():
            return read_config(folder / f"{t}-{driver}.cfg")
    out = read_config(folder / f"{driver}.cfg") if (folder / f"{driver}.cfg").is_file() else []
    for t in triples:
        if (folder / f"{t}.cfg").is_file():
            return out + read_config(folder / f"{t}.cfg")
    return out


def read_config(path: Path) -> list:
    """A configuration file's flags, its `@file` lines read from the including file's folder; `#` lines are comments."""
    out = []
    for line in path.read_text(encoding="utf-8").splitlines():
        if line.strip().startswith("#"):
            continue
        for word in shlex.split(line, posix=True):
            out += read_config(path.parent / word[1:]) if word.startswith("@") else [word]
    return out


def norm(path) -> str:
    return os.path.normcase(os.path.abspath(str(path))).replace("\\", "/")


def one(job):
    """The calls of one translation unit, as rows; its errors, the first of them said."""
    import clang.cindex as ci
    path, args, project, library = job
    if library and not ci.Config.loaded:
        ci.Config.set_library_file(library)
    index = ci.Index.create()
    try:
        tu = index.parse(path, args=args)
    except ci.TranslationUnitLoadError:
        return [], [f"{path}: clang could not read it"]
    errors = [f"{d.location.file}:{d.location.line}: {d.spelling}" for d in tu.diagnostics
              if d.severity >= ci.Diagnostic.Error]
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
    parser.add_argument("--libclang", help="the libclang library to load (as new as the toolchain's clang)")
    parser.add_argument("--jobs", type=int, default=os.cpu_count())
    args = parser.parse_args(argv)
    project = norm(args.project)
    entries = [e for e in json.loads(Path(args.database).read_text(encoding="utf-8"))
               if norm(e["file"]).startswith(project + "/")]
    jobs = [(e["file"], arguments(e, args.toolchain), args.project, args.libclang) for e in entries]
    seen, rows, errors = set(), [], []
    with ProcessPoolExecutor(args.jobs) as pool:
        for found, wrong in pool.map(one, jobs):
            errors += wrong
            for row in found:
                key = (row["file"], row["end"][0], row["end"][1])
                if key not in seen:
                    seen.add(key)
                    rows.append(row)
    if errors:
        print(f"{len(entries)} units read with {len(errors)} compiler errors: no answer written; the first:", file=sys.stderr)
        for line in errors[:SHOWN]:
            print(f"  {line}", file=sys.stderr)
        return 1
    Path(args.out).write_text("".join(json.dumps(r) + "\n" for r in rows), encoding="utf-8")
    print(f"{len(entries)} units, {len(rows)} calls, no compiler error -> {args.out}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
