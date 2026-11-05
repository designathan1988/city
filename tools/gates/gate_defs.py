"""Single-owner gates (fitness functions): owner registry, magic numbers (clang-tidy
readability-magic-numbers / C++ Core Guidelines ES.45 style), data values copied into code, and UI state
outside the store (Redux principle: keep state minimal and derive the rest)."""
import json
import os
import re

NUM = re.compile(r"(?<![\w.#])(\d+\.\d*|\.\d+|\d+)(?:[eE][-+]?\d+)?f?(?![\w.])")
STRING = re.compile(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'')


def _code_lines(path):
    """Lines with strings and comments blanked out (keeps line numbers)."""
    with open(path, encoding="utf-8", errors="replace") as fh:
        text = fh.read()
    text = re.sub(r"/\*.*?\*/", lambda m: "\n" * m.group(0).count("\n"), text, flags=re.S)
    out = []
    for line in text.splitlines():
        line = STRING.sub('""', line)
        line = re.sub(r"//.*", "", line)
        if path.endswith((".gd", ".py")):
            line = re.sub(r"#.*", "", line)
        out.append(line)
    return out


def _in_scope(f, prefixes):
    return any(f.startswith(p) for p in prefixes)


def check_owners(root, files, err):
    with open(os.path.join(root, "tools", "gates", "owners.json"), encoding="utf-8") as fh:
        reg = json.load(fh)
    for c in reg["concepts"]:
        owner = c["owner"]
        if owner not in files:
            err("DONO_AUSENTE", owner, f"dono do conceito '{c['id']}' não existe")
            continue
        sym = c["symbol"].split("::")[-1]
        with open(os.path.join(root, owner), encoding="utf-8") as fh:
            if not re.search(r"\b%s\b" % re.escape(sym), fh.read()):
                err("DONO_AUSENTE", owner, f"símbolo '{c['symbol']}' (dono de '{c['id']}') não encontrado")
        rx = [re.compile(p) for p in c["forbidden_elsewhere"]]
        allow = set(c.get("allow", [])) | {owner}
        for f in files:
            if f in allow or any(f.startswith(a) for a in allow if a.endswith("/")):
                continue
            if not _in_scope(f, c["scope"]) or not f.endswith((".cpp", ".h", ".gd", ".gdshader")):
                continue
            for n, line in enumerate(_code_lines(os.path.join(root, f)), 1):
                if any(r.search(line) for r in rx):
                    err("DONO_UNICO", f"{f}:{n}", f"reimplementa '{c['id']}': use {c['symbol']} ({owner})")


def _allowed_number(tok, cfg):
    v = float(tok.rstrip("f"))
    return v in cfg["allowed_values"]


def check_magic_numbers(root, rules, files, err):
    cfg = rules["magic_numbers"]
    named = re.compile(cfg["named_constant_line"])
    for f in files:
        if not _in_scope(f, cfg["paths"]) or _in_scope(f, cfg.get("exempt", [])):
            continue
        if not f.endswith(tuple(cfg["extensions"])):
            continue
        for n, line in enumerate(_code_lines(os.path.join(root, f)), 1):
            if named.search(line) or line.lstrip().startswith(("#include", "@export_range", "#define")):
                continue
            for m in NUM.finditer(line):
                if not _allowed_number(m.group(0), cfg):
                    err("NUMERO_MAGICO", f"{f}:{n}", f"literal {m.group(0)}: dê um nome (constante no dono) ou mova para data/")
                    break


def _data_values(root, rules):
    """value -> list of data paths, and leaf key -> list of data paths."""
    vals, keys = {}, {}

    def walk(node, where, leaf):
        if isinstance(node, dict):
            for k, v in node.items():
                walk(v, f"{where}.{k}", k)
        elif isinstance(node, list):
            for i, v in enumerate(node):
                walk(v, f"{where}[{i}]", leaf)
        elif isinstance(node, (int, float)) and not isinstance(node, bool):
            vals.setdefault(float(node), []).append(where)
            if leaf and not leaf.startswith("_"):
                keys.setdefault(leaf, []).append(where)

    for data_path in rules["schemas"]:
        if data_path in rules["magic_numbers"].get("data_copy_ignore_files", []):
            continue
        full = os.path.join(root, data_path)
        if os.path.exists(full):
            with open(full, encoding="utf-8") as fh:
                walk(json.load(fh), data_path, "")
    return vals, keys


def _distinctive(v):
    """Values unlikely to coincide by chance: 3+ decimals, or large integers that are not round."""
    if v != int(v):
        return abs(round(v, 2) - v) > 1e-9
    n = abs(int(v))
    round_numbers = {10 ** k for k in range(7)} | {2 ** k for k in range(21)}
    return n >= 100 and n not in round_numbers and n % 100 != 0


CONST_NAME = re.compile(r"\b(?:const|constexpr)\s+[\w:<>]+\s+([A-Z][A-Z0-9_]+)|^\s*const\s+([A-Z][A-Z0-9_]+)")
GENERIC = {"min", "max", "size", "count", "value", "share", "rate", "time", "step", "scale", "range", "base"}


def _tokens(name):
    return {t for t in re.split(r"[_\W]+", name.lower()) if len(t) >= 3 and t not in GENERIC}


def check_data_copies(root, rules, files, err):
    """A definition owned by a data table must not get a second owner in code: neither the same distinctive
    value typed again, nor a named constant whose name matches a data key (it would drift silently)."""
    cfg = rules["magic_numbers"]
    allow = set(cfg.get("data_copy_allow", []))
    vals, keys = _data_values(root, rules)
    distinct = {v: w for v, w in vals.items() if len(w) == 1 and _distinctive(v)}
    key_tokens = {k: _tokens(k) for k in keys}
    for f in files:
        if not _in_scope(f, cfg["data_copy_paths"]) or _in_scope(f, cfg.get("exempt", [])):
            continue
        if not f.endswith((".cpp", ".h", ".gd", ".gdshader")):
            continue
        for n, line in enumerate(_code_lines(os.path.join(root, f)), 1):
            m = CONST_NAME.search(line)
            name = (m.group(1) or m.group(2)) if m else None
            if name and f"{f}:{name}" in allow:
                continue
            if name:
                toks = _tokens(name)
                hits = [k for k, kt in key_tokens.items() if len(kt & toks) >= 2 or (toks and toks == kt)]
                if hits:
                    err("SEGUNDO_DONO", f"{f}:{n}", f"{name} repete a definição de '{hits[0]}' ({keys[hits[0]][0]}): leia do dado")
                    continue
            for mm in NUM.finditer(line):
                v = float(mm.group(0).rstrip("f"))
                if v in distinct:
                    err("VALOR_DUPLICADO", f"{f}:{n}", f"{mm.group(0)} já é definido em {distinct[v][0]}: leia do dado")
                    break


def check_ui_state(root, files, err):
    """UI scripts hold no game state: only node refs (@onready) and exported config."""
    for f in files:
        if not f.startswith("project/ui/") or not f.endswith(".gd"):
            continue
        for n, line in enumerate(_code_lines(os.path.join(root, f)), 1):
            if re.match(r"var\s", line) and "@onready" not in line and "@export" not in line:
                err("ESTADO_NA_UI", f"{f}:{n}", "UI não guarda estado: leia do CityStore/ToolController/GameFlow")
