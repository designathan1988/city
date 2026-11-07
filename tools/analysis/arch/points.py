"""The independent count: every link point of every file, listed by a plain lexer that shares nothing with the adapters. Each then ends
resolved, with candidates and confidence, or in the residue with file and line."""
import collections
import re

import tree_sitter
import tree_sitter_cpp
import tree_sitter_gdscript
import tree_sitter_typescript

from .adapters.data import NAMELIKE as JSON_NAME

CODE = re.compile(r"\"(?:\\.|[^\"\\\n])*\"|'(?:\\.|[^'\\\n])*'|//[^\n]*|/\*.*?\*/|#include[^\n]*|(?<![\w.])([A-Za-z_]\w*)\s*\(|(?<![\d.])(?:\.|->)\s*([A-Za-z_]\w*)", re.S)
HASH_CODE = re.compile(r"\"(?:\\.|[^\"\\\n])*\"|'(?:\\.|[^'\\\n])*'|#[^\n]*|(?<![\w.])([A-Za-z_]\w*)\s*\(|\.\s*([A-Za-z_]\w*)", re.S)
JSON_VALUE = re.compile(r":\s*\"((?:\\.|[^\"\\\n])*)\"|[\[,]\s*\"((?:\\.|[^\"\\\n])*)\"")
RESOURCE = re.compile(r"res://[\w./-]+|^\[(?:ext_resource|connection|node)\b", re.M)
LANGS = {}


def _keywords():
    """What a language calls a keyword comes from its grammar, not from a list here."""
    if not LANGS:
        for mod, exts in ((tree_sitter_cpp.language(), ("cpp", "cc", "cxx", "c", "h", "hpp", "hh", "inl", "gdshader", "gdshaderinc")),
                          (tree_sitter_gdscript.language(), ("gd",)), (tree_sitter_typescript.language_tsx(), ("ts", "tsx", "js", "jsx", "mjs", "cjs"))):
            lang = tree_sitter.Language(mod)
            for e in exts:
                LANGS[e] = lang
    return LANGS


def _is_keyword(ext, word):
    lang = _keywords().get(ext)
    return lang is not None and lang.id_for_node_kind(word, False) not in (None, 0)


def _lines(text):
    starts = [0]
    for m in re.finditer("\n", text):
        starts.append(m.end())
    return starts


def _locate(starts, i):
    lo, hi = 0, len(starts) - 1
    while lo < hi:
        mid = (lo + hi + 1) // 2
        if starts[mid] <= i:
            lo = mid
        else:
            hi = mid - 1
    return lo + 1, i - starts[lo] + 1


def enumerate_file(path, text):
    """(line, column, kind, text) for every place in a file where something may link to something else."""
    ext = path.rsplit(".", 1)[-1].lower()
    starts = _lines(text)
    out = []
    if ext == "json":
        for m in JSON_VALUE.finditer(text):
            word = m.group(1) or m.group(2)
            if word and JSON_NAME.match(word):
                out.append((*_locate(starts, m.start()), "value", word))
        return out
    if ext in ("tscn", "tres", "godot", "cfg", "gdextension"):
        for m in RESOURCE.finditer(text):
            out.append((*_locate(starts, m.start()), "resource", m.group(0)))
        return out
    pattern = HASH_CODE if ext in ("gd", "py", "sh") else CODE if ext in _keywords() else None
    if pattern is None:
        return out
    for m in pattern.finditer(text):
        token = m.group(0)
        if m.group(1):
            if not _is_keyword(ext, m.group(1)):
                out.append((*_locate(starts, m.start(1)), "call", m.group(1)))
        elif m.group(2):
            out.append((*_locate(starts, m.start(2)), "member", m.group(2)))
        elif token.startswith("#include"):
            out.append((*_locate(starts, m.start()), "include", token))
        elif token[0] in "\"'" and JSON_NAME.match(token[1:-1]):
            out.append((*_locate(starts, m.start()), "string", token[1:-1]))
    return out


def account(run):
    """Every point against what adapters and linker claimed. Returns the counts by kind and the residue."""
    facts, names, report = run.facts, run.names, run.report
    seen_at = {(f, l, c) for f, l, c, _ in facts.sites}
    by_line = collections.defaultdict(set)
    for e in run.graph.edges.values():
        name = run.graph.units.get(e.dst, {}).get("name") or e.dst.rsplit(".", 1)[-1]
        for f, l in e.sites:
            by_line[(f, l)].add(name)
    for u in run.graph.units.values():
        by_line[(u["file"], u["line"])].add(u["name"])
    readable = {u["file"] for u in run.graph.units.values() if u["kind"] != "file"}
    resolved = report["resolved"]
    known_outside = run.api.words | run.api.props
    totals = collections.Counter()
    residue = []
    for path in sorted(run.scope):
        text = run.texts.get(path)
        if text is None or path in run.roles.index or (path.endswith(".json") and path not in readable):
            continue
        for line, col, kind, word in enumerate_file(path, text):
            totals[kind] += 1
            if (path, line, col) in seen_at or (path, line, word) in resolved or word in by_line.get((path, line), ()):
                totals["resolved"] += 1
            elif word in known_outside:
                totals["outside"] += 1
            else:
                candidates = sorted(names.any.get(word, ()))
                residue.append({"file": path, "line": line, "column": col, "kind": kind, "text": word, "candidates": len(candidates),
                                "confidence": round(1.0 / len(candidates), 3) if candidates else 0.0})
    totals["residue"] = len(residue)
    return dict(totals), residue
