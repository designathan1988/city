"""Godot's text formats: scenes, resources, the project file, import and uid files, export presets. All are sections of key = value."""
import fnmatch
import re

from ..model import GRAMMAR, Facts

SECTION = re.compile(r"^\[(\w+)(.*)\]\s*$")
FIELD_VALUE = re.compile(r'([\w/.:-]+)=("(?:\\.|[^"\\])*"|\S+)')
FORMATS = (".tscn", ".tres", ".godot", ".cfg", ".import", ".uid", ".gdextension")


def _attrs(text):
    return {k: v.strip('"') for k, v in FIELD_VALUE.findall(text)}


def _resource(path):
    return path[6:] if path.startswith("res://") else path


class Reader:
    def __init__(self, file, text, facts, info):
        self.file, self.text, self.f, self.info = file, text, facts, info
        self.root = "file:" + file
        self.ext = {}
        self.current = None
        self.section = ""
        self.source = ""

    def run(self):
        f = self.f
        f.unit(self.root, "file", self.file.rsplit("/", 1)[-1], self.file, 1, self.text.count("\n") + 1)
        f.define("file", self.file, self.root)
        if self.file.endswith(".import"):
            f.unit("file:" + self.file[:-len(".import")], "file", self.file[:-len(".import")].rsplit("/", 1)[-1], self.file[:-len(".import")], 1, 1)
        if self.file.endswith(".uid"):
            f.define("uid", self.text.strip(), "file:" + self.file[:-4])
            f.edge(self.root, "uid", "name:file:" + self.file[:-4], self.file, 1, GRAMMAR)
            return
        for n, raw in enumerate(self.text.split("\n"), 1):
            line = raw.strip()
            m = SECTION.match(line)
            if m:
                self.head(m.group(1), _attrs(m.group(2)), n)
            elif "=" in line and not line.startswith((";", "#")):
                self.value(line, n)

    def head(self, kind, a, n):
        f, file = self.f, self.file
        self.section = kind
        self.current = self.root
        if kind == "ext_resource":
            path = _resource(a.get("path", ""))
            self.ext[a.get("id", "")] = path
            f.ref("file", path, self.root, file, n, "ext_resource")
            f.site(file, n, 1, "resource")
            if "uid" in a:
                f.ref("uid", a["uid"], self.root, file, n, "ext_resource")
        elif kind == "node":
            parent = a.get("parent", "")
            name = a.get("name", "")
            path = name if parent in ("", ".") else f"{parent}/{name}"
            self.current = f"scene:{file}#{path}"
            f.unit(self.current, "node", name, file, n, n, self.root, type=a.get("type", ""))
            f.define("node", name, self.current)
            f.define("node", path, self.current)
            f.site(file, n, 1, "node")
            if "groups" in a or "groups=" in self.text.split("\n")[n - 1]:
                for g in re.findall(r'"([^"]+)"', self.text.split("\n")[n - 1].split("groups=", 1)[-1]):
                    f.ref("group", g, self.current, file, n, "scene")
        elif kind == "connection":
            target = f"scene:{file}#{a.get('to', '')}"
            source = f"scene:{file}#{a.get('from', '')}"
            f.edge(source, "connection", "name:method:" + a.get("method", ""), file, n, GRAMMAR, signal=a.get("signal", ""), to=target)
            f.ref("signal", a.get("signal", ""), source, file, n, "connection")
            f.site(file, n, 1, "connection")

    def value(self, line, n):
        key, _, value = line.partition("=")
        key, value = key.strip(), value.strip().strip('"')
        f, file = self.f, self.file
        if self.section == "autoload":
            path = _resource(value.lstrip("*"))
            self.info.setdefault("autoloads", {})[key] = path
            f.ref("file", path, self.root, file, n, "autoload")
            f.edge(self.root, "autoload", "name:file:" + path, file, n, GRAMMAR, name=key)
        elif self.section == "shader_globals":
            uid = f"{self.root}.{key}"
            f.unit(uid, "uniform", key, file, n, n, self.root, globally=True)
            f.define("uniform", key, uid)
        elif self.section == "input":
            uid = f"{self.root}.{key}"
            f.unit(uid, "key", key, file, n, n, self.root)
            f.define("key", key, uid)
        elif file.endswith(".import") and key == "importer":
            f.cost(self.root, "importer", file, n, 0, value=value)
        elif file.endswith(".import") and key == "source_file":
            self.source = _resource(value)
        elif file.endswith(".import") and key == "compress/mode" and value == "0":
            f.cost(self.root, "importer", file, n, 0, value=key, uncompressed=True, source=self.source)
        elif key == "script" and self.ext:
            m = re.search(r'ExtResource\(\s*"?([^"\s)]+)', value)
            if m and m.group(1) in self.ext:
                f.edge(self.current, "script", "name:file:" + self.ext[m.group(1)], file, n, GRAMMAR)
        elif key in ("run/main_scene",):
            self.info.setdefault("main", []).append(_resource(value))
            f.edge(self.root, "main", "name:file:" + _resource(value), file, n, GRAMMAR)
        elif key.endswith("_filter"):
            self.info.setdefault("filters", {}).setdefault(key, []).extend(p.strip() for p in value.split(",") if p.strip())
        elif value.startswith("res://"):
            f.ref("file", _resource(value), self.current or self.root, file, n, key)
        else:
            for found in re.findall(r"res://[\w./-]+", value):
                f.ref("file", _resource(found), self.current or self.root, file, n, key)


def scan(repo, tracked, scope):
    facts, info = Facts(), {}
    for file in sorted(scope):
        if file.endswith(FORMATS):
            text = repo.text(tracked[file])
            if text is not None:
                Reader(file, text, facts, info).run()
    return facts, info


def filtered_out(path, base, filters):
    """Whether an export leaves a file out, by the patterns the preset itself writes."""
    rel = path[len(base):] if path.startswith(base) else path
    return any(fnmatch.fnmatch(rel, p) for p in filters.get("exclude_filter", []))
