"""Text-keys gate: every whole text key the core writes as a string literal (native/src C++, a command's, a hint's or a
refusal's key such as "hint.road.join") has a row in the player's translations (project/data/i18n/*.csv, first column), or
the player reads the bare key. A text key is one of the prefixes of rules.json text_keys, a dot, then letters, digits, dots
and underscores. A key built at run time ("cmd." + name, "hint.road." + reason: a literal ending in its separator) is no
whole key and is not judged; a comment is not a literal."""
import csv
import os
import re

from gate_language import TRANSLATIONS
from tokens import string_body

ID = "TEXT_KEY_MISSING"
SOURCES = "native/src/"
CODE_EXTENSIONS = (".h", ".cpp")
TABLE_EXTENSION = ".csv"
KEY_BODY = r"\.[A-Za-z0-9_.]+$"  # after the prefix: a dot, then the rest of the key
SEPARATOR = "."  # a literal ending in it is the head of a key built at run time


def _code(path):
    return path.startswith(SOURCES) and path.endswith(CODE_EXTENSIONS)


def _table(path):
    return path.startswith(TRANSLATIONS) and path.endswith(TABLE_EXTENSION)


def inputs(ctx, path):
    """The core's C++ files and the translation tables."""
    return _code(path) or _table(path)


def _table_keys(ctx):
    """The first column of every row of every translation table."""
    keys = set()
    for rel in ctx.files:
        if _table(rel):
            with open(os.path.join(ctx.root, rel), encoding="utf-8", newline="") as fh:
                keys.update(row[0] for row in csv.reader(fh) if row)
    return keys


def _key_shape(ctx):
    """The shape of a text key: one of the prefixes rules.json text_keys names, then KEY_BODY."""
    heads = "|".join(re.escape(p) for p in ctx.rules["text_keys"]["prefixes"])
    return re.compile(f"^(?:{heads}){KEY_BODY}")


def _whole_key(key, text):
    """A whole key: the shape of one, not the head of a key built at run time."""
    return bool(key.match(text)) and not text.endswith(SEPARATOR)


def check_keys(ctx, err):
    """The cached string facts (facts.py) name a file's missing keys; only such a file is read again for their lines."""
    known = _table_keys(ctx)
    key = _key_shape(ctx)
    for path in sorted(f for f in ctx.files if _code(f)):
        missing = {s for s in ctx.facts(path).get("strings", []) if _whole_key(key, s) and s not in known}
        if not missing:
            continue
        for tok in ctx.source(path).code():
            if tok.kind == "string" and string_body(tok) in missing:
                err(ID, f"{path}:{tok.line}", f"'{string_body(tok)}' is shown by the core with no row in {TRANSLATIONS}*.csv: "
                                              "add its key with the pt_BR and en texts")
