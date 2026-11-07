"""Single-owner gates (fitness functions) on tokens: the owner registry, magic numbers (clang-tidy
readability-magic-numbers / C++ Core Guidelines ES.45 style, each literal judged) and data values copied into code."""
import json
import os
import re

import cpp_syntax
import gd_syntax
import lex_c
import lex_gd

OWNER_EXTENSIONS = (".cpp", ".h", ".gd", ".gdshader")


def _in(path, prefixes):
    return any(path.startswith(p) for p in prefixes)


# ---- owner registry ---------------------------------------------------------------------------------------------

def check_owner_symbols(ctx, err):
    for c in ctx.owners["concepts"]:
        owner = c["owner"]
        if owner not in ctx.listed_set:
            err("DONO_AUSENTE", owner, f"owner of concept '{c['id']}' does not exist")
            continue
        sym = c["symbol"].split("::")[-1]
        if not any(t.kind == "ident" and t.text == sym for t in ctx.source(owner).code()):
            err("DONO_AUSENTE", owner, f"symbol '{c['symbol']}' (owner of '{c['id']}') is not in its code")


def owner_inputs(ctx, path):
    return path in {c["owner"] for c in ctx.owners["concepts"]}


def applies_owners(ctx, path):
    return path.endswith(OWNER_EXTENSIONS) and any(_in(path, c["scope"]) for c in ctx.owners["concepts"])


def check_single_owner(ctx, src, err):
    """A concept's re-implementation pattern found in code (comments blanked, strings kept) outside its owner."""
    lines = None
    for c in ctx.owners["concepts"]:
        allow = set(c.get("allow", [])) | {c["owner"]}
        if src.path in allow or any(src.path.startswith(a) for a in allow if a.endswith("/")) or not _in(src.path, c["scope"]):
            continue
        rx = [re.compile(p) for p in c["forbidden_elsewhere"]]
        lines = lines if lines is not None else src.blanked()
        for n, line in enumerate(lines, 1):
            if any(r.search(line) for r in rx):
                err("DONO_UNICO", f"{src.path}:{n}", f"re-implements '{c['id']}': use {c['symbol']} ({c['owner']})")


# ---- named constants and number literals ------------------------------------------------------------------------

def constant_tokens(src):
    """(token ids inside named-constant declarations, {name: token ids of its declaration}) for C++, GLSL and GDScript."""
    code = src.code()
    inside, by_name = set(), {}
    if src.lang in ("cpp", "glsl"):
        for a, b in cpp_syntax.named_constant_ranges(code):
            span = code[a:b + 1]
            ids = {id(t) for t in span}
            inside |= ids
            for k, t in enumerate(span[:-1]):
                if t.kind == "ident" and cpp_syntax.UPPER.match(t.text) and span[k + 1].text in ("=", "{", "["):
                    by_name[t.text] = ids
    elif src.lang == "gd":
        for ln in gd_syntax.logical_lines(src):
            anns = gd_syntax.annotations(ln.toks)
            for _name, a, b in anns:
                inside |= {id(t) for t in ln.toks[a:b]}
            rest = ln.toks[anns[-1][2]:] if anns else ln.toks
            if len(rest) > 1 and rest[0].text == "const" and cpp_syntax.UPPER.match(rest[1].text):
                ids = {id(t) for t in rest}
                inside |= ids
                by_name[rest[1].text] = ids
            elif rest and rest[0].text == "enum":
                inside |= {id(t) for t in rest}
    return inside, by_name


def _value(src, tok):
    return (lex_gd.number_value if src.lang == "gd" else lex_c.number_value)(tok.text)


def applies_magic(ctx, path):
    cfg = ctx.rules["magic_numbers"]
    return _in(path, cfg["paths"]) and not _in(path, cfg.get("exempt", [])) and path.endswith(tuple(cfg["extensions"]))


def check_magic_numbers(ctx, src, err):
    allowed = {float(v) for v in ctx.rules["magic_numbers"]["allowed_values"]}
    inside, _names = constant_tokens(src)
    for t in src.code():
        if t.kind == "number" and id(t) not in inside:
            v = _value(src, t)
            if v is None or v not in allowed:
                err("NUMERO_MAGICO", f"{src.path}:{t.line}", f"literal {t.text}: name it (a constant in its owner) or move it to data/")


# ---- data values copied into code -------------------------------------------------------------------------------

def data_values(ctx):
    """value -> data paths, leaf key -> data paths, over every data table with a schema."""
    def build():
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

        ignore = set(ctx.rules["magic_numbers"].get("data_copy_ignore_files", []))
        for data_path in ctx.rules["schemas"]:
            full = os.path.join(ctx.root, data_path)
            if data_path not in ignore and os.path.exists(full):
                with open(full, encoding="utf-8") as fh:
                    walk(json.load(fh), data_path, "")
        return vals, keys
    return ctx.lazy("data_values", build)


def _distinctive(v):
    """Values unlikely to coincide by chance: 3+ decimals, or large integers that are not round."""
    if v != int(v):
        return abs(round(v, 2) - v) > 1e-9
    n = abs(int(v))
    round_numbers = {10 ** k for k in range(7)} | {2 ** k for k in range(21)}
    return n >= 100 and n not in round_numbers and n % 100 != 0


GENERIC = {"min", "max", "size", "count", "value", "share", "rate", "time", "step", "scale", "range", "base"}


def _name_words(name):
    return {t for t in re.split(r"[_\W]+", name.lower()) if len(t) >= 3 and t not in GENERIC}


def applies_data_copies(ctx, path):
    cfg = ctx.rules["magic_numbers"]
    return _in(path, cfg["data_copy_paths"]) and not _in(path, cfg.get("exempt", [])) and path.endswith(OWNER_EXTENSIONS)


def check_data_copies(ctx, src, err):
    """A definition owned by a data table gets no second owner in code: neither a named constant whose name matches a
    data key nor the same distinctive value typed again (it would drift silently)."""
    allow = set(ctx.rules["magic_numbers"].get("data_copy_allow", []))
    vals, keys = data_values(ctx)
    distinct = {v: w for v, w in vals.items() if len(w) == 1 and _distinctive(v)}
    key_words = {k: _name_words(k) for k in keys}
    _inside, by_name = constant_tokens(src)
    skip = set()
    for name, ids in by_name.items():
        if f"{src.path}:{name}" in allow:
            skip |= ids
            continue
        words = _name_words(name)
        hits = [k for k, kw in key_words.items() if len(kw & words) >= 2 or (words and words == kw)]
        if hits:
            line = min(t.line for t in src.code() if id(t) in ids)
            err("SEGUNDO_DONO", f"{src.path}:{line}", f"{name} repeats the definition of '{hits[0]}' ({keys[hits[0]][0]}): read the data")
            skip |= ids
    for t in src.code():
        if t.kind == "number" and id(t) not in skip:
            v = _value(src, t)
            if v in distinct:
                err("VALOR_DUPLICADO", f"{src.path}:{t.line}", f"{t.text} is already defined in {distinct[v][0]}: read the data")
