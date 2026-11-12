"""The program read whole: every file under the roots turned into facts (cached per content), indexed and linked;
each phase that builds what the run keeps runs as a building phase (`building`)."""
from __future__ import annotations

import gc
import os
import sys
import threading
import time
from contextlib import contextmanager
from pathlib import Path

from . import engine_read, js_libs, link
from .cpp_read import read_cpp
from .gd_read import read_gd
from .js_config import read_package, read_tsconfig
from .js_read import read_js
from .index import Index
from .source import PROJECT_FILE, cached_facts, discover, reader_version

READERS = (read_gd, read_cpp, read_js, read_tsconfig, read_package, engine_read.read_scene, engine_read.read_project,
           engine_read.read_data)            # what turns a file into facts: the facts cache's version covers them
RECURSION = 200_000
STACK_BYTES = 255 << 20         # under 256 MiB, the most CPython gives a thread on Windows: room for deep chains


def rooted(paths: list[Path]) -> tuple[list[Path], Path, list[str]]:
    """The roots a tool was given, resolved; the folder the displayed paths start at; the roots that do not exist."""
    roots = [p.resolve() for p in paths]
    missing = [str(p) for p in roots if not p.exists()]
    base = Path(os.path.commonpath([r if r.is_dir() else r.parent for r in roots])) if not missing else Path()
    return roots, base, missing


def teller(began: float):
    """What prints a line of progress to stderr with the seconds since `began`."""
    def progress(message) -> None:
        print(f"[{time.perf_counter() - began:6.1f}s] {str(message).strip()}", file=sys.stderr, flush=True)
    return progress


def godot_root(path: Path) -> Path | None:
    """The folder holding project.godot above a file: where `res://` points."""
    for folder in [path.parent, *path.parents]:
        if (folder / PROJECT_FILE).is_file():
            return folder
    return None


def res_path(path: Path) -> str:
    root = godot_root(path)
    return "res://" + path.relative_to(root).as_posix() if root else path.as_posix()


def reader_for(path: Path):
    def read(src, lang):
        if lang == "gd":
            return read_gd(src, res_path(path))
        if lang == "cpp":
            return read_cpp(src)
        if lang == "js":
            return read_js(src)
        if lang == "tsconfig":
            return read_tsconfig(src)
        if lang == "package":
            return read_package(src)
        if lang == "scene":
            facts = engine_read.read_scene(src)
            root = godot_root(path)
            facts.mentions = [(p, 0) for p, _ in facts.mentions] if root else []
            return facts
        if lang == "project":
            return engine_read.read_project(src)
        return engine_read.read_data(src)
    return read


def load(roots: list[Path], base: Path, cache_dir: Path | None, progress=None):
    """Facts of every file, then the index and the links. Returns (index, resolver, files read, files with errors)."""
    index, count, broken = read(roots, base, cache_dir, progress)
    resolver = deep(lambda: link.build(index, progress))
    return index, resolver, count, broken


def read(roots: list[Path], base: Path, cache_dir: Path | None, progress=None, declarations: bool = False):
    """Facts of every file under the roots, indexed. Returns (index, files read, files with errors). With
    `declarations`, the declaration files the program's TypeScript chooses are read too (`js_libs`)."""
    with building():
        return _read(roots, base, cache_dir, progress, declarations)


def _read(roots: list[Path], base: Path, cache_dir: Path | None, progress, declarations: bool):
    files = discover(roots)
    if declarations:
        known = {p for p, _ in files}
        for root in roots:
            files += [(f, "js") for f in js_libs.declaration_files(root) if f not in known and not known.add(f)]
    for root in roots:
        project = godot_root(root / "x") if root.is_dir() else godot_root(root)
        if project is not None and (project / PROJECT_FILE).resolve() not in {p for p, _ in files}:
            files.append(((project / PROJECT_FILE).resolve(), "project"))
    version = reader_version(READERS)
    all_facts, broken = [], []
    for number, (path, lang) in enumerate(files, start=1):
        try:
            display = path.relative_to(base).as_posix()
        except ValueError:
            display = path.as_posix()
        context = res_path(path) if lang == "gd" else ""
        facts = cached_facts(path, display, lang, reader_for(path), cache_dir, version, context)
        if facts.errors:
            broken.append(display)
        all_facts.append(facts)
        if progress and number % 200 == 0:
            progress(f"read {number}/{len(files)} files")
    index = Index(all_facts)
    if progress:
        progress(f"indexed {len(index.units)} definitions; linking")
    return index, len(files), broken


def deep(work):
    """Runs `work` with a stack as deep as the code's chains of values need (flows nest as deep as the program
    calls): a thread with a large stack and a recursion limit to match, instead of a cut at a fixed depth."""
    result, failure = [], []

    def run():
        try:
            result.append(work())
        except BaseException as error:      # handed back to the caller's thread unchanged
            failure.append(error)

    old_limit, old_size = sys.getrecursionlimit(), threading.stack_size()
    sys.setrecursionlimit(RECURSION)
    threading.stack_size(STACK_BYTES)
    try:
        with building():
            worker = threading.Thread(target=run)
            worker.start()
            worker.join()
    finally:
        threading.stack_size(old_size)
        sys.setrecursionlimit(old_limit)
    if failure:
        raise failure[0]
    return result[0]


@contextmanager
def building():
    """A phase that builds the data the run keeps (the facts, the index, the links, an analysis): every object it makes
    stays reachable for the run, so the cyclic collector, which would traverse them all again and again and find
    nothing (log/trace-research.md F17), does not run while it builds, and what it built is moved out of every later
    collection (`gc.freeze`). The collector's state is the caller's again when the phase ends, failed or not."""
    was = gc.isenabled()
    gc.disable()
    try:
        yield
    finally:
        gc.freeze()
        if was:
            gc.enable()
