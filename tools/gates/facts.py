"""Per-file facts the whole-repo gates index once (JSON values, cached by file content): identifiers and strings in code,
resource references, GDScript class names, JobPool declarations and the copy-detector's token windows."""
import hashlib
import posixpath
import re

import cpp_syntax
from lex_other import LexError
from tokens import string_body

_RES = re.compile(r"(?:res|uid)://[^\s\"'*]*")
_MAX_STRING = 300
BASE, MOD = 1000003, (1 << 61) - 1
_token_ids = {}
_CLONE_SKIP_FIRST = {"py": {"import", "from"}, "gd": {"extends", "class_name"}}


def token_id(text):
    """Stable 64-bit id of a token's text (blake2b, not Python's per-process salted hash())."""
    v = _token_ids.get(text)
    if v is None:
        v = _token_ids[text] = int.from_bytes(hashlib.blake2b(text.encode("utf-8"), digest_size=8).digest(), "big")
    return v


def file_facts(source, rules):
    out = {"lang": source.lang, "binary": source.binary}
    if source.binary or source.lang is None:
        return out
    try:
        code = source.code()
    except LexError as e:
        out["lex_error"] = str(e)
        return out
    out["idents"] = sorted({t.text for t in code if t.kind == "ident"})
    strings = sorted({string_body(t) for t in code if t.kind == "string" and len(t.text) <= _MAX_STRING})
    out["strings"] = strings
    out["refs"] = sorted(_references(source, strings))
    if source.lang == "gd":
        out.update(_gd_class(code))
    if source.lang == "cpp":
        out["jobpool_names"] = sorted(cpp_syntax.names_of_type(code, "JobPool"))
        out["defines_jobkey"] = any(t.text == "struct" and i + 2 < len(code) and code[i + 1].text == "JobKey"
                                    and code[i + 2].text == "{" for i, t in enumerate(code))
    if source.path.endswith(".uid"):
        out["uid"] = source.text.strip()
    cfg = rules["clones"]
    if any(source.path.startswith(p) for p in cfg["paths"]) and source.path.endswith(tuple(cfg["extensions"])):
        out["windows"] = clone_windows(source, code, cfg["min_tokens"])
    return out


def _references(source, strings):
    """res:// and uid:// paths named in strings; a relative shader #include is resolved against the file's folder."""
    refs = set()
    for s in strings:
        refs.update(m.group(0) for m in _RES.finditer(s))
    if source.lang == "glsl" and source.path.startswith("project/"):
        for inc, _angle, _line in cpp_syntax.includes(source):
            if "://" not in inc:
                here = posixpath.dirname(source.path[len("project/"):])
                refs.add("res://" + posixpath.normpath(posixpath.join(here, inc)))
    return refs


def _gd_class(code):
    info = {"class_name": None, "extends": None}
    for i, t in enumerate(code[:-1]):
        nxt = code[i + 1]
        if t.text == "class_name" and nxt.kind == "ident" and info["class_name"] is None:
            info["class_name"] = nxt.text
        elif t.text == "extends" and info["extends"] is None and (i == 0 or code[i - 1].line != t.line):
            info["extends"] = string_body(nxt) if nxt.kind == "string" else nxt.text
    return info


def clone_windows(source, code, width):
    """[[hash, line], ...] of every run of `width` code tokens (Karp-Rabin over stable token ids); include, import,
    extends and class_name lines are left out, as they repeat by nature."""
    skip_first = _CLONE_SKIP_FIRST.get(source.lang, set())
    toks, line_first = [], {}
    for t in code:
        line_first.setdefault(t.line, t.text)
        if t.pp or line_first[t.line] in skip_first:
            continue
        toks.append(t)
    if len(toks) < width:
        return []
    ids = [token_id(t.text) for t in toks]
    power = pow(BASE, width - 1, MOD)
    h, out = 0, []
    for i, v in enumerate(ids):
        if i >= width:
            h = (h - ids[i - width] * power) % MOD
        h = (h * BASE + v) % MOD
        if i >= width - 1:
            out.append([h, toks[i - width + 1].line])
    return out
