"""The starting point: where in the code the feature begins.

Given as `file:line` (the definition that holds the line), `Class.method` / `Class::method` / `ns::Class::method`
(GDScript class_name, autoload or file name, C++ qualified name), or a bare name (every definition with it).
"""
from __future__ import annotations

import re

from .model import FUNCTION_KINDS

AT_LINE = re.compile(r"^(?P<file>.+):(?P<line>\d+)$")


def class_label(ix, key: str) -> str:
    cls = ix.classes.get(key)
    if cls is None:
        return key
    if cls.lang == "cpp":
        return key
    for name, k in ix.gd_globals.items():
        if k == key:
            return name
    if cls.outer:
        return f"{class_label(ix, cls.outer)}.{cls.name}"
    return key.rsplit("/", 1)[-1]


def label(ix, gid: int) -> str:
    unit = ix.units[gid]
    if unit.kind == "engine_signal":
        return f"engine signal '{unit.name}'"
    if unit.lang == "cpp":
        return unit.qual
    return f"{class_label(ix, unit.owner)}.{unit.name}"


def at_line(ix, file: str, line: int) -> list[int]:
    wanted = file.replace("\\", "/")
    inside = [u for u in ix.units if u.file.endswith(wanted) and u.line <= line <= u.end_line and u.kind != "class"]
    if not inside:
        return []
    functions = [u for u in inside if u.kind in FUNCTION_KINDS] or inside
    best = min(functions, key=lambda u: u.end_line - u.line)
    return [best.gid]


def find(ix, spec: str) -> list[int]:
    spec = spec.strip()
    match = AT_LINE.match(spec)
    if match:
        found = at_line(ix, match.group("file").replace("res://", ""), int(match.group("line")))
        if found:
            return found
    parts = re.split(r"::|\.", spec)
    # a definition's own name may hold a dot (a property's `x.get`, a lambda): the longest name that matches wins
    for cut in range(1, len(parts)):
        found = in_scope(ix, spec, ".".join(parts[cut:]), parts[:cut])
        if found:
            return found
    candidates = ix.by_name.get(parts[-1], [])
    functions = [g for g in candidates if ix.units[g].kind in FUNCTION_KINDS]
    return ix.definitions(functions) if functions else list(candidates)


def in_scope(ix, spec: str, name: str, scope: list[str]) -> list[int]:
    candidates = ix.by_name.get(name, [])
    want = ".".join(scope)
    found = []
    for gid in candidates:
        unit = ix.units[gid]
        owner = class_label(ix, unit.owner) if unit.lang == "gd" else unit.owner.replace("::", ".")
        stem = unit.owner.rsplit("/", 1)[-1].removesuffix(".gd") if unit.lang == "gd" else ""
        if owner == want or owner.endswith("." + want) or stem == want or unit.qual.endswith(spec):
            found.append(gid)
    functions = [g for g in found if ix.units[g].kind in FUNCTION_KINDS]
    return ix.definitions(functions) if functions else found
