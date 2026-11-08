"""Tuning gate: native/src/sim/catalog/tuning_fields.h (names) and project/data/catalog/tuning.json (values) declare
exactly the same keys and every key is read in code (an identifier or a string token; a comment is not a read); a group's
`_doc` is its text, not a key (the schema requires it). Mirror gate: a value written twice (a shader array's size) equals
its C++ owner; a table with no mirror registered checks nothing and fails."""
import os
import re

import jsonfile

HEADER = "native/src/sim/catalog/tuning_fields.h"
DATA = "project/data/catalog/tuning.json"
RULES = "tools/gates/rules.json"
FIELD = re.compile(r"\bX[ID]?\((\w+),\s*(\w+)\)")  # X a float, XI a count, XD a double
DOC = "_doc"


def reader(path):
    return path != HEADER and path.endswith((".cpp", ".h", ".gd")) and path.startswith(("native/src/", "project/"))


def inputs(ctx, path):
    return path in (HEADER, DATA) or reader(path)


def check_tuning(ctx, err):
    declared = {f"{g}_{n}" for g, n in FIELD.findall("\n".join(ctx.source(HEADER).blanked()))}
    data = jsonfile.load(os.path.join(ctx.root, DATA))
    valued = set()
    for group, values in data.items():
        if group == DOC:
            continue
        if not isinstance(values, dict):
            err("TUNING", DATA, f"group '{group}' must be an object")
            continue
        for name, v in values.items():
            if name == DOC:
                continue
            valued.add(f"{group}_{name}")
            if not isinstance(v, (int, float)) or isinstance(v, bool):
                err("TUNING", DATA, f"{group}.{name} must be a number")
    for key in sorted(declared - valued):
        err("TUNING", DATA, f"'{key}' is declared in {HEADER} without a value in the JSON")
    for key in sorted(valued - declared):
        err("TUNING", HEADER, f"'{key}' is in the JSON but not in CITY_TUNING_FIELDS")
    read = set()
    for f in ctx.files:
        if reader(f):
            facts = ctx.facts(f)
            read.update(facts.get("idents", []))
            read.update(facts.get("strings", []))
    for key in sorted(declared - read):
        err("TUNING_SEM_USO", HEADER, f"'{key}' is never read in code: remove the field or use it")


CONST = re.compile(r"constexpr\s+(?:int|float)\s+([A-Z][A-Z0-9_]*)\s*=\s*([^;]+);")


def _eval_constant(name, defs, depth=0):
    expr = defs[name]
    for other in sorted(defs, key=len, reverse=True):
        if other != name and re.search(r"\b%s\b" % other, expr) and depth < 8:
            expr = re.sub(r"\b%s\b" % other, str(_eval_constant(other, defs, depth + 1)), expr)
    return eval(re.sub(r"(\d)\.?f\b", r"\1", expr), {"__builtins__": {}})  # arithmetic on literals only


def mirror_inputs(ctx, path):
    return path == RULES or any(path == m["file"] or path in m["headers"] for m in ctx.rules.get("mirrors", []))


def check_mirrors(ctx, err):
    """Values that must be written twice (e.g. a shader array size) are checked against their C++ owner. An empty table
    proves nothing: it fails (CLAUDE.md: an empty set fails)."""
    if not ctx.rules.get("mirrors"):
        err("ESPELHO", RULES, "no mirror is registered: every value written twice must be listed here, or generated from its owner")
    for m in ctx.rules.get("mirrors", []):
        defs = {}
        for h in m["headers"]:
            for name, expr in CONST.findall("\n".join(ctx.source(h).blanked())):
                defs[name] = expr.strip()
        want = _eval_constant(m["constant"], defs)
        found = re.search(m["pattern"], "\n".join(ctx.source(m["file"]).blanked()))
        if not found or int(found.group(1)) != int(want):
            err("ESPELHO", m["file"], f"must use {m['constant']} = {want} (owner: {m['headers'][-1]})")
