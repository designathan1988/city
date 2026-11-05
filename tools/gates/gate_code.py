"""Code-level gates: C++ layer dependencies, forbidden patterns, orphan GDScript files."""
import os
import re

INCLUDE = re.compile(r'#include\s*[<"]([^>"]+)[>"]')


def _layer_for(path, layers):
    best = None
    for prefix in layers:
        if path.startswith(prefix + "/") and (best is None or len(prefix) > len(best)):
            best = prefix
    return best


def check_layers(root, rules, files, err):
    layers = rules["cpp_layers"]
    for f in files:
        if not f.startswith("native/src/") or not f.endswith((".cpp", ".h")):
            continue
        layer = _layer_for(f, layers)
        if layer is None:
            err("CAMADA_DESCONHECIDA", f, "pasta sem regra em rules.json/cpp_layers")
            continue
        with open(os.path.join(root, f), encoding="utf-8") as fh:
            for n, line in enumerate(fh, 1):
                m = INCLUDE.search(line)
                if not m:
                    continue
                inc = m.group(1)
                if "/" not in inc and line.strip().startswith("#include <"):
                    continue  # standard library
                if not any(inc.startswith(p) for p in layers[layer]):
                    err("CAMADA", f"{f}:{n}", f"'{inc}' não é permitido em {layer} (rules.json/cpp_layers)")


def check_forbidden(root, rules, files, err):
    compiled = [(r, re.compile(r["pattern"])) for r in rules["forbidden"]]
    for f in files:
        if f.endswith((".json", ".csv")) and not f.startswith("project/"):
            continue
        for rule, rx in compiled:
            excepted = any(f == e or (e.endswith("/") and f.startswith(e)) for e in rule.get("except", []))
            if not any(f.startswith(p) for p in rule["paths"]) or excepted:
                continue
            with open(os.path.join(root, f), encoding="utf-8", errors="replace") as fh:
                for n, line in enumerate(fh, 1):
                    if rx.search(line):
                        err(rule["id"], f"{f}:{n}", rule["message"])


def check_orphans(root, files, err):
    """Every GDScript/shader must be referenced by a scene, script, project.godot or class_name use."""
    texts = {}
    for f in files:
        if f.startswith("project/") and f.endswith((".gd", ".tscn", ".tres", ".godot", ".gdshader")):
            with open(os.path.join(root, f), encoding="utf-8", errors="replace") as fh:
                texts[f] = fh.read()
    for f, text in texts.items():
        if not f.endswith((".gd", ".gdshader")) or f.startswith("project/tests/"):
            continue
        res = "res://" + f[len("project/"):]
        m = re.search(r"^class_name\s+(\w+)", text, re.M)
        cls = re.compile(r"\b%s\b" % m.group(1)) if m else None
        used = any(res in t or (cls and cls.search(t)) for o, t in texts.items() if o != f)
        if not used:
            err("ORFAO", f, "arquivo não referenciado por cena/script/project.godot: apague ou ligue ao projeto")
