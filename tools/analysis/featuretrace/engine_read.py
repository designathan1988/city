"""The engine's own files, where references live outside the code: scenes, the project file and data.

A scene (`.tscn`) attaches scripts to nodes and connects a node's signal to a method of another node's script
(`[connection signal= from= to= method=]`). The project file names the autoloads, scripts the engine creates by name
and makes visible everywhere. A data file (`.json`) names things by string values (a tool, a command, an id); each value
spelled like an identifier is kept, so the report can say where data names a definition on the trace.
"""
from __future__ import annotations

import re

from .model import FileFacts
from .source import Source, is_identifier

ATTR = re.compile(r'(\w+)\s*=\s*"((?:[^"\\]|\\.)*)"')
EXT_REF = re.compile(r'ExtResource\(\s*"?([^")\s]+)"?\s*\)')
JSON_STRING = re.compile(rb'"((?:[^"\\]|\\.)*)"(\s*:)?')


def section(line: str) -> tuple[str, dict]:
    head = line.strip()[1:-1]
    kind = head.split(" ", 1)[0]
    return kind, dict(ATTR.findall(head))


def read_scene(src: Source) -> FileFacts:
    facts = FileFacts(src.display, "scene")
    text = src.data.decode("utf-8", "replace")
    scripts, node_script, pending = {}, {}, []
    current = None
    for number, line in enumerate(text.splitlines(), start=1):
        stripped = line.strip()
        if stripped.startswith("[") and stripped.endswith("]"):
            kind, attrs = section(stripped)
            current = None
            if kind == "ext_resource" and attrs.get("type") == "Script":
                scripts[attrs.get("id", "")] = attrs.get("path", "")
            elif kind == "node":
                parent = attrs.get("parent")
                name = attrs.get("name", "")
                current = "." if parent is None else (name if parent == "." else f"{parent}/{name}")
            elif kind == "connection":
                pending.append((attrs.get("signal", ""), attrs.get("from", ""), attrs.get("to", ""),
                                attrs.get("method", ""), number))
        elif current is not None and stripped.startswith("script"):
            match = EXT_REF.search(stripped)
            if match:
                node_script[current] = scripts.get(match.group(1), "")
    for signal, source, target, method, number in pending:
        facts.connections.append((signal, node_script.get(source, ""), node_script.get(target, ""), method, number))
    facts.mentions = [(path, 0) for path in node_script.values() if path]
    return facts


def read_project(src: Source) -> FileFacts:
    facts = FileFacts(src.display, "project")
    group = ""
    for line in src.data.decode("utf-8", "replace").splitlines():
        stripped = line.strip()
        if stripped.startswith("[") and stripped.endswith("]"):
            group = stripped[1:-1]
        elif group == "autoload" and "=" in stripped:
            name, value = stripped.split("=", 1)
            facts.autoloads[name.strip()] = value.strip().strip('"').lstrip("*")
        elif stripped.startswith("run/main_scene"):
            facts.mentions.append((stripped.split("=", 1)[1].strip().strip('"'), 0))
    return facts


def read_data(src: Source) -> FileFacts:
    facts = FileFacts(src.display, "data")
    for match in JSON_STRING.finditer(src.data):
        if match.group(2):
            continue
        value = match.group(1).decode("utf-8", "replace")
        if is_identifier(value):
            facts.mentions.append((value, src.line(match.start())))
    return facts
