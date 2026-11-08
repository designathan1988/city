"""Translation gate: the text tables of project/data/i18n (key, then one column per language, the same header in every file)
name each key once across all the files, give every row as many cells as the header, and hold no text broken by a wrong
encoding (UTF-8 read as Latin-1, or the replacement character), which the player would read as it stands."""
import csv
import io
import os
import re

I18N = "project/data/i18n/"
BROKEN = re.compile("[\u00c3\u00c2][\u0080-\u00bf]|\u00e2\u20ac|\ufffd")


def inputs(ctx, path):
    return path.startswith(I18N) and path.endswith(".csv")


def check_i18n(ctx, err):
    header, owner = None, {}
    files = sorted(f for f in ctx.files if inputs(ctx, f))
    if not files:
        err("I18N", I18N, "no translation table: the game's text has nowhere to come from")
    for f in files:
        with open(os.path.join(ctx.root, f), encoding="utf-8", newline="") as fh:
            rows = list(csv.reader(io.StringIO(fh.read())))
        if not rows:
            err("I18N", f, "an empty table")
            continue
        if header is None:
            header = rows[0]
        elif rows[0] != header:
            err("I18N", f, f"header {rows[0]} differs from {header}: every table has the same columns")
        for line, row in enumerate(rows[1:], 2):
            if not row:
                continue
            key = row[0]
            if len(row) != len(rows[0]):
                err("I18N", f"{f}:{line}", f"'{key}' has {len(row)} cells for {len(rows[0])} columns")
            if key in owner:
                err("I18N", f"{f}:{line}", f"'{key}' is already given at {owner[key]}: one key, one text")
            else:
                owner[key] = f"{f}:{line}"
            for cell in row[1:]:
                if BROKEN.search(cell):
                    err("I18N", f"{f}:{line}", f"'{key}' holds text broken by an encoding: {cell[:60]!r}")
