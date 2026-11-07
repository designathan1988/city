"""Copies with code moved around and nothing else: a file split, a function renamed, a function moved, a shim added. Done on the commit's text, in
the language the code is in, with what the grammar says about where functions start and end."""
import collections
import re

from ..core import IDENT
from ..spans import _gd


def _free_function(text, members, known, name, span):
    """A function that can leave its file and mean the same: only its file knows its name, it touches nothing else of the file, and the file calls it."""
    lines = text.split("\n")
    body = "\n".join(lines[span[0] - 1:span[1]])
    used = set(IDENT.findall(body)) & set(members)
    return lines[span[0] - 1].startswith("func ") and known[name] == 1 and used == {name} and text.count(name) > 1 and "self" not in body


def _script_with_functions(run):
    """The largest script with such a function, chosen by shape."""
    known = collections.Counter(w for words in run.roles.tokens.values() for w in words)
    best = None
    for path in sorted(run.scope):
        if path.endswith(".gd"):
            text = run.texts.get(path)
            if text is None or "class_name" in text:
                continue
            members = _gd(text.encode("utf8"))
            parts = {k: v for k, v in members.items() if text.split("\n")[v[0] - 1].startswith("func ")}
            free = sorted(k for k, v in parts.items() if _free_function(text, members, known, k, v))
            if len(parts) >= 3 and free and (best is None or len(parts) > best[1]):
                best = (path, len(parts), parts, text, free[0])
    return best


def views(run):
    path, _, parts, text, last = _script_with_functions(run)
    lines = text.split("\n")
    names = sorted(parts, key=lambda k: parts[k][0])
    start, end = parts[last][0] - 1, parts[last][1]
    block = lines[start:end]
    rest = lines[:start] + lines[end:]
    other = path.rsplit(".", 1)[0] + "_piece.gd"
    moved = "\n".join(["# The same responsibility, in a second file.", "extends RefCounted", "", *block, ""])
    out = {}
    out["split"] = {"edit": {path: "\n".join(rest).encode("utf8")}, "add": {other: moved.encode("utf8")}, "tag": "split"}
    fresh = last + "_again"
    renamed = re.sub(rf"\b{re.escape(last)}\b", fresh, text)
    out["rename"] = {"edit": {path: renamed.encode("utf8")}, "tag": "rename"}
    shim = f"\nfunc {last}_shim():\n	return {last}()\n"
    out["forward"] = {"edit": {path: (text + shim).encode("utf8")}, "tag": "forward"}
    first = names[0]
    a, b = parts[first][0] - 1, parts[first][1]
    swapped = lines[b:] + [""] + lines[a:b] if a == 0 else lines[:a] + lines[b:] + [""] + lines[a:b]
    out["move"] = {"edit": {path: "\n".join(swapped).encode("utf8")}, "tag": "move"}
    return out
