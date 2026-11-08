"""Single-owner gates (fitness functions) on tokens: the owner registry, magic numbers (clang-tidy
readability-magic-numbers / C++ Core Guidelines ES.45 style, each literal judged) and data values copied into code."""
import os
import re

import cpp_syntax
import gd_syntax
import jsonfile
import lex_c
import lex_gd

OWNER_EXTENSIONS = (".cpp", ".h", ".gd", ".gdshader", ".gdshaderinc")


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


def constant_declarations(src):
    """The tokens of each declaration that names a constant with a value of its own (C++ and GLSL `const`/`constexpr`
    and `#define`, GDScript `const`); enumerator lists name codes, not values, and are left out."""
    code = src.code()
    out = []
    if src.lang in ("cpp", "glsl"):
        for a, b in cpp_syntax.named_constant_ranges(code):
            if code[a].text != "{":  # an enumerator list's range starts at its brace
                out.append(code[a:b + 1])
    elif src.lang == "gd":
        for ln in gd_syntax.logical_lines(src):
            anns = gd_syntax.annotations(ln.toks)
            rest = ln.toks[anns[-1][2]:] if anns else ln.toks
            if len(rest) > 1 and rest[0].text == "const" and cpp_syntax.UPPER.match(rest[1].text):
                out.append(rest)
    return out


def applies_magic(ctx, path):
    cfg = ctx.rules["magic_numbers"]
    return _in(path, cfg["paths"]) and not _in(path, cfg.get("exempt", [])) and path.endswith(tuple(cfg["extensions"]))


def _sourced(decl, comment_lines, constant_lines, code_lines):
    """A declaration states its source when a comment shares one of its lines, or closes right above the run of constant
    declarations it belongs to (a comment over a block of constants speaks for the block)."""
    first, last = decl[0].line, decl[-1].last_line
    if any(n in comment_lines for n in range(first, last + 1)):
        return True
    k = first - 1
    while k in constant_lines:
        k -= 1
    return k in comment_lines and k not in code_lines


def check_magic_numbers(ctx, src, err):
    """A literal in code is named; a named constant holding a value says where the value comes from (CLAUDE.md: values one
    can tune live in data with their reason, so a constant in code is a definition, a norm or a derivation, and says so)."""
    allowed = {float(v) for v in ctx.rules["magic_numbers"]["allowed_values"]}
    inside, _names = constant_tokens(src)
    for t in src.code():
        if t.kind == "number" and id(t) not in inside:
            v = _value(src, t)
            if v is None or v not in allowed:
                err("NUMERO_MAGICO", f"{src.path}:{t.line}", f"literal {t.text}: name it (a constant in its owner) or move it to data/")
    decls = constant_declarations(src)
    if not decls:
        return
    comment_lines = {n for t in src.comments() for n in range(t.line, t.last_line + 1)}
    code_lines = {n for t in src.code() for n in range(t.line, t.last_line + 1)}
    constant_lines = {n for d in decls for n in range(d[0].line, d[-1].last_line + 1)}
    for d in decls:
        held = [t for t in d if t.kind == "number" and _value(src, t) not in allowed]
        if held and not _sourced(d, comment_lines, constant_lines, code_lines):
            name = next((t.text for t in d if t.kind == "ident" and cpp_syntax.UPPER.match(t.text)), d[0].text)
            err("CONSTANT_WITHOUT_SOURCE", f"{src.path}:{d[0].line}",
                f"{name} holds {held[0].text} and says nowhere what it is: a definition, a norm or a derivation in a comment, or data/")


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
                try:
                    walk(jsonfile.load(full), data_path, "")
                except ValueError:  # malformed or with a key written twice: the data gate reports it
                    continue
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
STYLES = "styles"  # a table's block per regional style: the style's id names no domain


def _name_words(name):
    return {t for t in re.split(r"[_\W]+", name.lower()) if len(t) >= 3 and t not in GENERIC}


def _domain_words(where, places):
    """The words of a code path, or of a data key's path without its leaf, that name what it is about: not a place of the
    repository (rules.json magic_numbers.data_copy_places), not a style's id, not a number."""
    words, skip = set(), False
    for part in re.split(r"[./\[\]]+", where.lower()):
        if skip:
            skip = False
            continue
        skip = part == STYLES
        words |= {t for t in re.split(r"[^a-z0-9]+", part) if len(t) >= 4 and t not in places and not t.isdigit()}
    return words


def _literal_of(src, decl, name):
    """The value `name` is given when its initializer is one literal (signed or not), else None."""
    for i, t in enumerate(decl):
        if t.text == name and i + 2 < len(decl) and decl[i + 1].text in ("=", ":=", "{"):
            j = i + 2
            sign = -1.0 if decl[j].text == "-" else 1.0
            j += 1 if decl[j].text == "-" else 0
            ends = j + 1 == len(decl) or (j + 1 < len(decl) and decl[j + 1].text in (";", ",", "}"))
            if j < len(decl) and decl[j].kind == "number" and ends:
                v = _value(src, decl[j])
                return None if v is None else sign * v
    return None


def applies_data_copies(ctx, path):
    cfg = ctx.rules["magic_numbers"]
    return _in(path, cfg["data_copy_paths"]) and not _in(path, cfg.get("exempt", [])) and path.endswith(OWNER_EXTENSIONS)


def check_data_copies(ctx, src, err):
    """A definition owned by a data table gets no second owner in code. A named constant is one when it is about the same
    thing (a word of its file's path is a word of the key's table or path) and either bears the key's whole name (two words
    or more) or shares two of its words and holds one of its values (not a trivial one); a distinctive value of a key about
    the same thing, typed again, is one too. Name, place and value are each weak alone: together they name a copy one can
    act on."""
    cfg = ctx.rules["magic_numbers"]
    allowed = {float(v) for v in cfg["allowed_values"]}
    places = set(cfg["data_copy_places"])
    vals, keys = data_values(ctx)
    at = {w: v for v, ws in vals.items() for w in ws}
    distinct = {v: w[0] for v, w in vals.items() if len(w) == 1 and _distinctive(v)}
    key_words = {k: _name_words(k) for k in keys}
    here = _domain_words(src.path, places)
    skip = set()
    for decl in constant_declarations(src):
        names = [t.text for k, t in enumerate(decl[:-1])
                 if t.kind == "ident" and cpp_syntax.UPPER.match(t.text) and decl[k + 1].text in ("=", ":=", "{", "[")]
        for name in names:
            words, v = _name_words(name), _literal_of(src, decl, name)
            for key, kw in key_words.items():
                named = name.lower() == key and "_" in key
                valued = len(kw & words) >= 2 and v is not None and v not in allowed and any(at[w] == v for w in keys[key])
                near = [w for w in keys[key] if here & _domain_words(w.rsplit(".", 1)[0], places)] if named or valued else []
                if near:
                    err("SEGUNDO_DONO", f"{src.path}:{decl[0].line}", f"{name} is a second owner of '{key}' ({near[0]}): read the data")
                    skip |= {id(t) for t in decl}
                    break
    for t in src.code():
        if t.kind == "number" and id(t) not in skip:
            v = _value(src, t)
            if v in distinct and here & _domain_words(distinct[v].rsplit(".", 1)[0], places):
                err("VALOR_DUPLICADO", f"{src.path}:{t.line}", f"{t.text} is already defined in {distinct[v]}: read the data")
