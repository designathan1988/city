"""Oracle of callers across languages for a Godot project: the pairs (method of the native core exposed to scripts,
script function that calls it), read from the scripts' own text. Not part of the tracer: the bench compares the
tracer's backward traces with these.

    python godot_callers.py <project folder> <out.json> --table FILE.gd [--via NAME]... [--direct NAME]...

A call is a pair when it writes, as the first argument of one of the `--via` functions, a key a table maps to a
method's name (`dispatch(&"key", ...)`, the table `const T := {&"key": "method"}` in `--table`), or calls a method on
one of the `--direct` names (`_core.method(...)`). The caller is named as the tracer labels it: the script's class
name, else its file's stem, then the function (a property's getter or setter written in place: the property). A table's
method written under an area of the core (`"area/method"`) is that method.
"""
from __future__ import annotations

import argparse
import json
import re
from pathlib import Path

CLASS = re.compile(r"^class_name\s+(\w+)", re.M)
FUNC = re.compile(r"^(?:static\s+)?func\s+(\w+)")
PROPERTY = re.compile(r"^var\s+(\w+)\b[^=]*:\s*(?:#.*)?$")      # `var x: T:` opens its getter and setter below
PAIR = re.compile(r'&"(\w+)"\s*:\s*"(?:\w+/)?(\w+)"')


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("project")
    parser.add_argument("out")
    parser.add_argument("--table", action="append", required=True)
    parser.add_argument("--via", action="append", default=[])
    parser.add_argument("--direct", action="append", default=[])
    parser.add_argument("--skip", action="append", default=["addons"])
    args = parser.parse_args(argv)
    root = Path(args.project)
    table = {}
    for one in args.table:
        table.update(dict(PAIR.findall((root / one).read_text(encoding="utf-8"))))
    via = re.compile(r"\b(?:%s)\(\s*&\"(\w+)\"" % "|".join(map(re.escape, args.via))) if args.via else None
    direct = re.compile(r"\b(?:%s)\.(\w+)\(" % "|".join(map(re.escape, args.direct))) if args.direct else None
    pairs = set()
    for path in sorted(root.rglob("*.gd")):
        if any(part in args.skip for part in path.relative_to(root).parts):
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        named = CLASS.search(text)
        owner = named.group(1) if named else path.stem
        current = None
        for line in text.splitlines():
            m = FUNC.match(line) or PROPERTY.match(line)
            if m:
                current = m.group(1)
            elif line[:1].isalpha():             # any other statement of the file's own level: no function's
                current = None
            if current is None:
                continue
            caller = f"{owner}.{current}"
            for key in via.findall(line) if via else ():
                if key in table:
                    pairs.add((table[key], caller))
            for name in direct.findall(line) if direct else ():
                pairs.add((name, caller))
    rows = [{"method": m, "caller": c} for m, c in sorted(pairs)]
    Path(args.out).write_text(json.dumps(rows, indent=0) + "\n", encoding="utf-8")
    print(f"{len(rows)} pairs, {len({m for m, _ in pairs})} methods -> {args.out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
