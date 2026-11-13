# What a Godot run loads, read from the project's files by every link Godot follows: a res:// path written in any text the
# run reads (a script, a scene, a shader, a data table, the project settings, the core's C++), a path built from a format or
# a prefix (every file it can name), a directory listed (every file in it), a script named by its class_name or autoload,
# a shader include, a uid://, a glTF's own files, the extensions every run loads, an asset's import settings. The closure is
# a superset of what the run opens: a task keyed by it reruns whenever anything the run may read changed, and on nothing else.
from __future__ import annotations

import json
import re
from pathlib import Path

from . import inputs

RES = re.compile(r"res://[^\s\"'<>|,;()\[\]{}]*")
JOINED = re.compile(r"[\"']\s*[+%]")  # what follows a bare res:// that a path is built on (a concatenation, a format)
FORMAT = re.compile(r"%[-+ 0#]*\d*(?:\.\d+)?[sdifxXcv]|\{[^}]*\}")  # GDScript's % formats and String.format's {name}
CLASS_NAME = re.compile(r"^\s*class_name\s+([A-Za-z_]\w*)", re.M)
IDENT = re.compile(r"\b[A-Za-z_]\w*\b")
# GDScript's strings (triple-quoted first, then single-line, raw or with escapes, & and ^ names included) and comments: a
# global name inside them is text, not a reference the compiler resolves
GD_TEXT = re.compile(r'"""[\s\S]*?"""|\'\'\'[\s\S]*?\'\'\'|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'|#[^\n]*')
UID = re.compile(r"uid://[a-z0-9]+")
UID_DECL = re.compile(r"\buid=\"(uid://[a-z0-9]+)\"")
INCLUDE = re.compile(r"#include\s+\"([^\"]+)\"")
SECTION = re.compile(r"^\[([^\]]+)\]\s*$")
KEY = re.compile(r"^([A-Za-z0-9_/.]+)\s*=\s*(.*)$")
MAIN_SCENE = "run/main_scene"  # the scene a run opens when its command line names none
SCENE_SUFFIXES = (".tscn", ".scn")
TEXT_SUFFIXES = frozenset({".gd", ".tscn", ".tres", ".gdshader", ".gdshaderinc", ".json", ".cfg", ".godot", ".gdextension",
                           ".gltf", ".import", ".uid", ".csv", ".txt"})
NATIVE_SUFFIXES = frozenset({".cpp", ".h", ".hpp", ".cc", ".inc"})
SIDECARS = (".import", ".uid")
GENERATED = ".godot/"  # the editor's own cache: made from the sources, never a source
SETTINGS = "project.godot"
EXTENSION = ".gdextension"


class Project:
    """One Godot project inside a repository (`project` is its folder, repo-relative) and the C++ folders whose res:// paths
    the loaded libraries open. Files are git's view of the worktree (tracked and untracked, ignored left out)."""

    def __init__(self, root: Path | str, project: str, native: tuple[str, ...] = ()):
        self.root = Path(root).resolve()
        self.base = project.strip("/") + "/"
        listed = inputs.snapshot(self.root).files()
        self.files = sorted(f[len(self.base):] for f in listed if f.startswith(self.base)
                            and not f[len(self.base):].startswith(GENERATED))
        self.file_set = set(self.files)
        self.native = [f for f in listed for n in native if f.startswith(n.strip("/") + "/")
                       and Path(f).suffix in NATIVE_SUFFIXES]
        self._text: dict[str, str] = {}
        self._links: dict[str, frozenset[str]] = {}  # links() per file: every task's closure walks the same files
        self._native_refs: set[str] | None = None  # native_refs(), read once: every task's closure starts from it
        self.classes = self._classes()
        self.uids = self._uids()
        self.main_scene, self.settings_refs = self._settings()

    # reading --------------------------------------------------------------------------------------------------------------
    def text(self, rel: str) -> str:
        """A project file's text ('' for a binary or missing one)."""
        if rel not in self._text:
            path = self.root / self.base / rel
            ok = Path(rel).suffix in TEXT_SUFFIXES and path.is_file()
            self._text[rel] = path.read_text(encoding="utf-8", errors="replace") if ok else ""
        return self._text[rel]

    def _classes(self) -> dict[str, str]:
        """Global names a script reaches without a path: every class_name, and the autoloads of the project settings."""
        out = {}
        for f in self.files:
            if f.endswith(".gd"):
                for m in CLASS_NAME.finditer(self.text(f)):
                    out[m.group(1)] = f
        for section, key, value in self._settings_lines():
            if section == "autoload":
                for lit in self.literals(value):
                    out[key] = lit[len("res://"):]
        return out

    def _uids(self) -> dict[str, str]:
        """uid:// -> file: a script's `.uid` sidecar, a scene's or resource's header, an asset's import settings."""
        out = {}
        for f in self.files:
            if f.endswith(".uid"):
                m = UID.search(self.text(f))
                if m:
                    out[m.group(0)] = f[:-len(".uid")]
            elif f.endswith((".tscn", ".tres", ".import")):
                m = UID_DECL.search(self.text(f).split("\n", 1)[0] if not f.endswith(".import") else self.text(f))
                if m:
                    out[m.group(1)] = f[:-len(".import")] if f.endswith(".import") else f
        return out

    def _settings_lines(self):
        section = ""
        for line in self.text(SETTINGS).splitlines():
            if line.lstrip().startswith(";"):
                continue  # a ConfigFile comment
            s = SECTION.match(line)
            if s:
                section = s.group(1)
                continue
            k = KEY.match(line)
            if k:
                yield section, k.group(1), k.group(2)
            else:
                yield section, "", line

    def _settings(self) -> tuple[str, set[str]]:
        """The main scene, and what every run reads from the project settings (autoloads, globals, icons: every res:// but
        the main scene's)."""
        main, refs = "", set()
        for section, key, value in self._settings_lines():
            found = self.literals(value)
            if section == "application" and key == MAIN_SCENE:
                main = found[0][len("res://"):] if found else ""
                continue
            for lit in found:
                refs |= self.resolve(lit)
        return main, refs

    # links ----------------------------------------------------------------------------------------------------------------
    def resolve(self, lit: str, base: str = "") -> set[str]:
        """The project files a res:// literal can name: the file; every file under a directory; with a format in it, or
        naming nothing whole, every file its fixed parts allow (a path built from it)."""
        rest = lit[len("res://"):] if lit.startswith("res://") else _join(base, lit)
        if rest.startswith(GENERATED):
            return set()
        if FORMAT.search(rest):
            return self._glob(FORMAT.sub("**", rest))
        rest = rest.rstrip("/") if rest.endswith("/") else rest
        if rest in self.file_set:
            return {rest}
        under = self._glob(rest + "/**") if rest else set(self.files)
        return under or self._glob(rest + "**")

    def _glob(self, pattern: str) -> set[str]:
        rx = inputs.glob_regex(pattern)
        return {f for f in self.files if rx.match(f)}

    def literals(self, text: str) -> list[str]:
        """The res:// paths a text names. The scheme alone names a path only where one is built on it ("res://" + name,
        "res://%s"); elsewhere (prose, a scheme test such as begins_with("res://")) it names nothing."""
        return [m.group(0) for m in RES.finditer(text)
                if m.group(0) != "res://" or JOINED.match(text, m.end())]

    def links(self, rel: str) -> frozenset[str]:
        """The files one project file makes the run load, worked out once per file."""
        if rel not in self._links:
            self._links[rel] = frozenset(self._read_links(rel))
        return self._links[rel]

    def _read_links(self, rel: str) -> set[str]:
        if rel == SETTINGS:
            return set(self.settings_refs)  # the main scene only when the command line names no scene (closure)
        text = self.text(rel)
        out: set[str] = set()
        for lit in self.literals(text):
            out |= self.resolve(lit)
        for uid in UID.findall(text):
            if uid in self.uids:
                out.add(self.uids[uid])
        suffix = Path(rel).suffix
        if suffix in (".gdshader", ".gdshaderinc"):
            for inc in INCLUDE.findall(text):
                if not inc.startswith("res://"):
                    out |= self.resolve(inc, rel)
        elif suffix == ".gltf":
            out |= self._gltf_files(rel, text)
        elif suffix == ".gd":
            names = set(IDENT.findall(GD_TEXT.sub(" ", text))) & set(self.classes)
            out |= {self.classes[n] for n in names}
        out.discard(rel)
        return out

    def _gltf_files(self, rel: str, text: str) -> set[str]:
        try:
            doc = json.loads(text)
        except ValueError:
            return set(self._glob(_join(rel, "**")))  # unreadable: everything beside it
        uris = [x.get("uri", "") for k in ("images", "buffers") for x in doc.get(k, []) if isinstance(x, dict)]
        return set().union(*(self.resolve(u, rel) for u in uris if u and not u.startswith("data:"))) if uris else set()

    def native_refs(self) -> set[str]:
        """The res:// paths the loaded libraries' sources name, read once per Project (the same set each call: not to be
        changed)."""
        if self._native_refs is None:
            out: set[str] = set()
            for f in self.native:
                text = (self.root / f).read_text(encoding="utf-8", errors="replace")
                for lit in self.literals(text):
                    out |= self.resolve(lit)
            self._native_refs = out
        return self._native_refs

    # closure --------------------------------------------------------------------------------------------------------------
    def closure(self, roots: list[str]) -> list[str]:
        """Repo-relative files a run started on `roots` (res:// paths from its command line) may load, sorted."""
        start = set().union(*(self.resolve(r) for r in roots)) if roots else set()
        if not any(r.endswith(SCENE_SUFFIXES) for r in roots) and self.main_scene:
            start.add(self.main_scene)
        start |= {SETTINGS} if SETTINGS in self.file_set else set()
        start |= {f for f in self.files if f.endswith(EXTENSION)} | self.settings_refs | self.native_refs()
        seen, todo = set(), list(start)
        while todo:
            f = todo.pop()
            if f in seen or f not in self.file_set:
                continue
            seen.add(f)
            todo.extend(self.links(f) - seen)
            todo.extend(f + s for s in SIDECARS if f + s in self.file_set)
        return sorted(self.base + f for f in seen)


def _join(base: str, rel: str) -> str:
    """A path written relative to the file `base` (a shader include, a glTF's image), as a project path."""
    parts = Path(base).parent.parts + Path(rel).parts
    out: list[str] = []
    for p in parts:
        if p == "..":
            if out:
                out.pop()
        elif p not in (".", ""):
            out.append(p)
    return "/".join(out)


LOADED = re.compile(r"^Loading resource: (res://\S+)", re.M)


def loaded(log_text: str, project: str) -> list[str]:
    """The repo-relative files a --verbose Godot log says ResourceLoader loaded (the editor's cache left out)."""
    base = project.strip("/") + "/"
    out = {base + m[len("res://"):] for m in LOADED.findall(log_text) if not m[len("res://"):].startswith(GENERATED)}
    return sorted(out)
