"""The program read whole: every file under the roots turned into facts (cached per content), indexed and linked."""
from __future__ import annotations

from pathlib import Path

from . import engine_read, link
from .cpp_read import read_cpp
from .gd_read import read_gd
from .index import Index
from .source import PROJECT_FILE, cached_facts, discover, reader_version


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
    files = discover(roots)
    for root in roots:
        project = godot_root(root / "x") if root.is_dir() else godot_root(root)
        if project is not None and (project / PROJECT_FILE).resolve() not in {p for p, _ in files}:
            files.append(((project / PROJECT_FILE).resolve(), "project"))
    version = reader_version()
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
    resolver = link.build(index, progress)
    return index, resolver, len(files), broken
