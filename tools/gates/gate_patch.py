"""NUMBER_MOVED: a number moved to make a check pass is a patch (CLAUDE.md, no-patch). The watched numbers (rules.json
patch.watched: the tests' named constants, the tasks' deadlines, the gates' own thresholds) are compared by name with the
file as HEAD holds it; a value that changed passes only when its cause is recorded first, in a line of the logs or in a
fixed problem of a trace's ledger, naming the file, the name moved and the value it moved to. A number that is new, or
gone, is no move."""
import glob
import json
import os
import re

import gd_syntax
import scope
import source as source_mod
from context import digest

ID = "NUMBER_MOVED"
_NUMBER = re.compile(r"-?(?:0|[1-9]\d*)(?:\.\d+)?(?:[eE][-+]?\d+)?")
_UNARY_BEFORE = {"(", "[", "{", ",", ":", "=", ":=", "-", "+", "*", "/"}


def _watch(ctx, path):
    for w in ctx.rules["patch"]["watched"]:
        if any(path == p or (p.endswith("/") and path.startswith(p)) for p in w["paths"]):
            return w
    return None


def applies(ctx, path):
    w = _watch(ctx, path)
    ext = ".gd" if w and w["kind"] == "gd_const" else ".json"
    return w is not None and path.endswith(ext) and ctx.changed.get(path) is not None


def _records(ctx):
    """Every line a cause may be written in: the logs' lines and the fixed problems' accounts of every trace's ledger."""
    def build():
        cfg = ctx.rules["patch"]
        lines = []
        for folder in cfg["logs"]:
            for p in sorted(glob.glob(os.path.join(ctx.root, folder, "**", "*.md"), recursive=True)):
                with open(p, encoding="utf-8", errors="replace") as fh:
                    lines += fh.read().splitlines()
        for p in sorted(glob.glob(os.path.join(ctx.root, cfg["ledgers"], "*", "ledger.json"))):
            try:
                with open(p, encoding="utf-8") as fh:
                    problems = json.load(fh).get("problems", [])
            except (OSError, ValueError):
                continue
            items = problems.values() if isinstance(problems, dict) else problems
            lines += [x.get("how", "") for x in items if isinstance(x, dict) and x.get("status") == "fixed"]
        return lines
    return ctx.lazy("patch_records", build)


def context(ctx):
    """What a verdict reads besides the file: the records, and which HEAD the file is held against."""
    head = "" if ctx.before_root is not None else scope.head_commit(ctx.root)
    return digest(_records(ctx), head)


class _Text(source_mod.Source):
    """A source made from a text (the file before the change), lexed like the file itself."""

    def __init__(self, path, text):  # noqa: D107  the base reads a file; this one is given its text
        self.path, self.text, self.binary, self.lang = path, text, False, source_mod.language_of(path)
        self._tokens, self._error = None, None


def _gd_numbers(src):
    """{name or name[k]: (value, line)} of the numbers in the class-level constants."""
    out = {}
    for d in gd_syntax.declarations(gd_syntax.logical_lines(src)):
        if d.keyword != "const" or not d.member:
            continue
        found = []
        for i, t in enumerate(d.init_toks):
            if t.kind != "number":
                continue
            unary = i >= 1 and d.init_toks[i - 1].text == "-" and (i == 1 or d.init_toks[i - 2].text in _UNARY_BEFORE)
            text = t.text.replace("_", "")
            value = float(int(text, 0)) if text[:2].lower() in ("0x", "0b") else float(text)
            found.append((-value if unary else value, t.line))
        for k, v in enumerate(found):
            out[d.name if len(found) == 1 else f"{d.name}[{k}]"] = v
    return out


class _JsonNumber(float):
    line = 0


def _json_value(text, i):
    """(value, next index) of the JSON value at text[i]; numbers carry their line."""
    while text[i] in " \t\r\n":
        i += 1
    c = text[i]
    if c == "{":
        out, i = {}, i + 1
        while True:
            while text[i] in " \t\r\n,":
                i += 1
            if text[i] == "}":
                return out, i + 1
            key, i = _json_value(text, i)
            i = text.index(":", i) + 1
            out[key], i = _json_value(text, i)
    if c == "[":
        out, i = [], i + 1
        while True:
            while text[i] in " \t\r\n,":
                i += 1
            if text[i] == "]":
                return out, i + 1
            v, i = _json_value(text, i)
            out.append(v)
    if c == '"':
        end = i + 1
        while text[end] != '"':
            end += 2 if text[end] == "\\" else 1
        return json.loads(text[i:end + 1]), end + 1
    m = _NUMBER.match(text, i)
    if m:
        n = _JsonNumber(m.group(0))
        n.line = text.count("\n", 0, i) + 1
        return n, m.end()
    for word, v in (("true", True), ("false", False), ("null", None)):
        if text.startswith(word, i):
            return v, i + len(word)
    raise ValueError(f"no JSON value at line {text.count(chr(10), 0, i) + 1}")


def _json_numbers(text, keys):
    """{key path: (value, line)} of the numbers under the watched keys ('*' every key); a list's object is named by its
    `name` or `id` member, so a list reordered moves nothing."""
    out = {}

    def walk(node, path, last):
        if isinstance(node, dict):
            for k, v in node.items():
                walk(v, f"{path}.{k}" if path else k, k)
        elif isinstance(node, list):
            for i, v in enumerate(node):
                tag = (v.get("name") or v.get("id")) if isinstance(v, dict) else None
                walk(v, f"{path}[{tag if isinstance(tag, str) else i}]", last)
        elif isinstance(node, _JsonNumber) and ("*" in keys or last in keys):
            out[path] = (float(node), node.line, last)

    walk(_json_value(text, 0)[0], "", "")
    return {p: (v, line) for p, (v, line, _last) in out.items()}, {p: last for p, (_v, _l, last) in out.items()}


def _numbers(src, text, watch):
    """({name: (value, line)}, {name: the word a record names it by})."""
    if watch["kind"] == "gd_const":
        found = _gd_numbers(src if text is None else _Text(src.path, text))
        return found, {n: n.split("[")[0] for n in found}
    return _json_numbers(src.text if text is None else text, set(watch["keys"]))


def _recorded(ctx, path, word, value):
    """A record of this move: a line naming the file, the name and the value it moved to (an older line about the same
    constant names another value, so it cannot pass a later move)."""
    base = os.path.basename(path)
    name = re.compile(rf"\b{re.escape(word)}\b")
    moved_to = re.compile(rf"(?<![\d.]){re.escape(f'{value:g}')}(?![\d])")
    return any((path in line or base in line) and name.search(line) and moved_to.search(line) for line in _records(ctx))


def check_patch(ctx, src, err):
    watch = _watch(ctx, src.path)
    before = ctx.before(src.path)
    if watch is None or before is None:
        return
    old, _ = _numbers(src, before, watch)
    new, words = _numbers(src, None, watch)
    for name, (value, line) in sorted(new.items(), key=lambda kv: kv[1][1]):
        was = old.get(name)
        if was is None or was[0] == value or _recorded(ctx, src.path, words[name], value):
            continue
        err(ID, f"{src.path}:{line}", f"{name} moved from {was[0]:g} to {value:g}: a number moved to make a check pass is a "
            f"patch; record the cause first (a log line, or a fixed trace problem, naming {src.path}, {words[name]} and "
            f"{value:g})")
