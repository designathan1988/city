"""Tuning gate: native/src/sim/catalog/tuning_fields.h (names) and project/data/catalog/tuning.json (values)
must declare exactly the same keys, and every key must be read somewhere (C++ or GDScript)."""
import json
import os
import re

HEADER = "native/src/sim/catalog/tuning_fields.h"
DATA = "project/data/catalog/tuning.json"
FIELD = re.compile(r"X\((\w+),\s*(\w+)\)")


def check_tuning(root, files, err):
    with open(os.path.join(root, HEADER), encoding="utf-8") as fh:
        declared = {f"{g}_{n}" for g, n in FIELD.findall(fh.read())}
    with open(os.path.join(root, DATA), encoding="utf-8") as fh:
        data = json.load(fh)
    valued = set()
    for group, values in data.items():
        if not isinstance(values, dict):
            err("TUNING", DATA, f"grupo '{group}' deve ser um objeto")
            continue
        for name, v in values.items():
            valued.add(f"{group}_{name}")
            if not isinstance(v, (int, float)) or isinstance(v, bool):
                err("TUNING", DATA, f"{group}.{name} deve ser número")
    for key in sorted(declared - valued):
        err("TUNING", DATA, f"'{key}' declarado em {HEADER} sem valor no JSON")
    for key in sorted(valued - declared):
        err("TUNING", HEADER, f"'{key}' existe no JSON mas não em CITY_TUNING_FIELDS")
    code = []
    for f in files:
        if f == HEADER or not f.endswith((".cpp", ".h", ".gd")):
            continue
        if f.startswith("native/src/") or f.startswith("project/"):
            with open(os.path.join(root, f), encoding="utf-8", errors="replace") as fh:
                code.append(fh.read())
    blob = "\n".join(code)
    for key in sorted(declared):
        if not re.search(r"\b%s\b" % re.escape(key), blob):
            err("TUNING_SEM_USO", HEADER, f"'{key}' nunca é lido: remova o campo ou use-o")


CONST = re.compile(r"constexpr\s+(?:int|float)\s+([A-Z][A-Z0-9_]*)\s*=\s*([^;/]+);")


def _eval_constant(name, defs, depth=0):
    expr = defs[name]
    for other in sorted(defs, key=len, reverse=True):
        if other != name and re.search(r"\b%s\b" % other, expr) and depth < 8:
            expr = re.sub(r"\b%s\b" % other, str(_eval_constant(other, defs, depth + 1)), expr)
    return eval(re.sub(r"(\d)\.?f\b", r"\1", expr), {"__builtins__": {}})  # arithmetic on literals only


def check_mirrors(root, rules, err):
    """Values that must be written twice (e.g. a shader array size) are checked against their C++ owner."""
    for m in rules.get("mirrors", []):
        defs = {}
        for h in m["headers"]:
            with open(os.path.join(root, h), encoding="utf-8") as fh:
                for name, expr in CONST.findall(fh.read()):
                    defs[name] = expr.strip()
        want = _eval_constant(m["constant"], defs)
        with open(os.path.join(root, m["file"]), encoding="utf-8") as fh:
            found = re.search(m["pattern"], fh.read())
        if not found or int(found.group(1)) != int(want):
            err("ESPELHO", m["file"], f"deve usar {m['constant']} = {want} (dono: {m['headers'][-1]})")
