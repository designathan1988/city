"""The reference game's features for the inventory (tools/atlas/inventory.py), read anew each run from its code folders: where
the tree is comes from tools/gates/rules.json `lab_reference` and this machine's tools/tasks/locations.py, as LAB_COPY finds
it. The rules, each a declaration's form and never a list of features:
- texts: a file named by a language's code (en.ts, pt-BR.ts) holding `'key': 'text'` entries; one text a key, in each language;
- actions: the members of an exported union of string literals whose type name ends in tool, mode or action; and the first
  literal argument of a call to a function whose name holds `command` (a registry of the player's commands);
- data: each exported upper-case constant whose value is a table (a list of strings, a list of objects with an id, key or name,
  a map of named objects), its entries; and the entries of its JSON files (inventory_json.py);
- systems: every code file, its first comment its description (a worker's name ends in .worker).
Read for behaviour only: nothing of it is copied into this repository; the reports it feeds live under dist/."""
import json
import os
import re
import sys

TOOLS_DIR = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
if TOOLS_DIR not in sys.path:
    sys.path.insert(0, TOOLS_DIR)
from atlas import inventory_json  # noqa: E402
from atlas import inventory_rules as rules  # noqa: E402
from atlas import inventory_ts as ts  # noqa: E402
from tasks import locations  # noqa: E402  the one owner of this machine's paths

GAME = "reference"
RULES = "tools/gates/rules.json"
CODE = (".ts", ".tsx", ".js", ".mjs")
DECLARATIONS = ".d.ts"  # type declarations only: no feature
COMMAND_CALL = re.compile(r"""\b(\w*[Cc]ommand)\s*\(\s*(['"])([\w.\-]+)\2""")
LITERAL = re.compile(r"'([\w.\-]+)'|\"([\w.\-]+)\"")


def locate(repo_root):
    """(the reference tree, its code folders, the folders that are not its code): rules.json lab_reference, `@name` a
    location of this machine (tools/tasks/locations.py raises Missing naming what to set)."""
    with open(os.path.join(repo_root, RULES), encoding="utf-8") as fh:
        place = json.load(fh)["lab_reference"]
    where = place["place"]
    root = locations.locate(where[1:]) if where.startswith("@") else os.path.join(repo_root, where)
    if not os.path.isdir(root):
        raise locations.Missing(f"the reference tree {root} is not there: set it (tools/tasks/locations.py)")
    return root, list(place.get("dirs") or ["."]), set(place.get("skip_dirs", []))


def files(root, dirs, skip):
    """The code and JSON files under the code folders, relative to `root`, hidden folders and `skip` left out."""
    out = []
    for top in dirs:
        for dirpath, subdirs, names in os.walk(os.path.join(root, top)):
            subdirs[:] = sorted(d for d in subdirs if d not in skip and not d.startswith("."))
            for n in sorted(names):
                if (n.endswith(CODE) and not n.endswith(DECLARATIONS)) or n.endswith(".json"):
                    out.append(os.path.relpath(os.path.join(dirpath, n), root).replace(os.sep, "/"))
    return out


def _at(rel, lines, offset):
    return f"{rel}:{lines.of(offset)}"


def _within(rel, dirs):
    """A path as the game's own folders name it (the code folder's name left out), the place its area is read from."""
    for top in dirs:
        prefix = top.strip("./") + "/"
        if prefix != "/" and rel.startswith(prefix):
            return rel[len(prefix):]
    return rel


def _code(rel, place, text, out, gaps):
    lines = ts.Lines(text)
    name = os.path.splitext(place)[0]
    out.append(rules.feature(GAME, rules.module_area(place), "system", name, f"{rel}:1",
                             description=rules.first_comment(text)))
    found, other = ts.tables(text)
    for const, entries, _off in found:
        for entry, off, how in entries:
            area = rules.area_of(const, entry) or rules.module_area(place)
            out.append(rules.feature(GAME, area, "data", f"{const}/{entry}", _at(rel, lines, off),
                                     description=f"{how} of {const}", id=rules.norm_id(entry), table=const))
    gaps["upper-case constants whose value is no table"] += other
    found, other = ts.unions(text)
    for type_name, members, _off in found:
        if rules.names_actions(type_name):
            for member, off in members:
                area = rules.area_of(member, type_name) or rules.module_area(place)
                out.append(rules.feature(GAME, area, "action", f"{type_name}.{member}", _at(rel, lines, off),
                                         description=f"a member of {type_name}, an exported union of the player's choices",
                                         id=rules.norm_id(member), keys=rules.label_keys(type_name, member)))
        else:
            gaps["exported unions of strings not named as actions (tool, mode, action)"] += 1
    gaps["exported types that are no union of strings"] += other
    for m in COMMAND_CALL.finditer(text):
        command = m.group(3)
        area = rules.area_of(*command.split(".")) or rules.module_area(place)
        out.append(rules.feature(GAME, area, "action", command, _at(rel, lines, m.start()),
                                 description=f"a command registered by {m.group(1)}()", id=rules.norm_id(command),
                                 keys=rules.label_keys("", command)))


def _texts(dictionaries):
    """One text feature a key: its text in each language, its source the English file's line when it has one."""
    out = []
    for key, by_lang in dictionaries.items():
        first = by_lang.get("en") or next(iter(by_lang.values()))
        area = rules.area_of(*key.split(".")) or rules.module_area(first[2])
        out.append(rules.feature(GAME, area, "text", key, first[1], label=first[0],
                                 texts={lang: v[0] for lang, v in by_lang.items()},
                                 sources={lang: v[1] for lang, v in by_lang.items()}))
    return out


def _uses(texts_by_rel, keys):
    """Each text key's first use in the code: a string literal equal to it, outside the dictionaries."""
    used = {}
    for rel, text in texts_by_rel:
        lines = None
        for m in LITERAL.finditer(text):
            key = m.group(1) or m.group(2)
            if key in keys and key not in used:
                lines = lines or ts.Lines(text)
                used[key] = _at(rel, lines, m.start())
    return used


def read(root, dirs, skip):
    """Every feature the rules find: {"features", "files", "languages", "gaps"}."""
    rels = files(root, dirs, skip)
    out, dictionaries, languages, code = [], {}, {}, []
    gaps = {"dictionary entries whose text is no string literal": 0, "upper-case constants whose value is no table": 0,
            "exported unions of strings not named as actions (tool, mode, action)": 0,
            "exported types that are no union of strings": 0, "JSON files that cannot be read": 0}
    for rel in rels:
        with open(os.path.join(root, rel), encoding="utf-8", errors="replace") as fh:
            text = fh.read()
        place = _within(rel, dirs)
        stem = os.path.basename(rel).split(".")[0]
        if rel.endswith(".json"):
            found, why = inventory_json.entries(GAME, rel, text)
            out += found
            gaps["JSON files that cannot be read"] += 1 if why else 0
            continue
        if rules.language_file(stem):
            entries, skipped = ts.dictionary(text)
            gaps["dictionary entries whose text is no string literal"] += skipped
            if entries:
                lang, lines = rules.language(stem), ts.Lines(text)
                languages[lang] = rel
                for key, value, off in entries:
                    dictionaries.setdefault(key, {})[lang] = (value, _at(rel, lines, off), place)
                continue
        code.append((rel, text))
        _code(rel, place, text, out, gaps)
    texts = _texts(dictionaries)
    rules.mark_uses(texts, _uses(code, set(dictionaries)))
    return {"features": texts + out, "files": len(rels), "languages": languages,
            "gaps": [{"game": GAME, "what": k, "count": v} for k, v in gaps.items() if v]}
