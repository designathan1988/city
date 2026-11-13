"""This game's features for the inventory (tools/atlas/inventory.py), read anew each run from the repository. The rules, each
a place and a form, never a list of features:
- texts: every row of project/data/i18n/*.csv (the key, then a column per language);
- actions: StoreApi's COMMANDS (what the interface may send the core) and, as queries, its QUERIES; every member of an
  enum, GDScript or C++, whose name ends in tool, mode or action (project/game, project/ui, project/render, native/src);
  every interface object of
  project/data/ui/*.json carrying a text key, a tool, an action or an id (inventory_json.controls);
- data: the entries of project/data/catalog/*.json and project/data/render/*.json (inventory_json.entries);
- systems: every module of native/src (a .h and its .cpp one module) and every script and shader of project/game, project/ui
  and project/render, each with its file's first comment;
- tests: every script of project/tests, with its first comment (what it proves)."""
import csv
import os
import re
import sys

TOOLS_DIR = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
if TOOLS_DIR not in sys.path:
    sys.path.insert(0, TOOLS_DIR)
from atlas import inventory_json  # noqa: E402
from atlas import inventory_rules as rules  # noqa: E402
from atlas import inventory_ts as ts  # noqa: E402  its Lines: offsets to line numbers

GAME = "city"
I18N = "project/data/i18n"
UI = "project/data/ui"
DATA = ("project/data/catalog", "project/data/render")
STORE_API = "project/game/store/store_api.gd"
NATIVE = "native/src"
SCRIPTS = ("project/game", "project/ui", "project/render")
TESTS = "project/tests"
NATIVE_CODE = (".h", ".cpp")
SCRIPT_CODE = (".gd", ".gdshader", ".gdshaderinc")
STORE_DICT = re.compile(r"^const (COMMANDS|QUERIES)\s*:?=\s*\{(.*?)^\}", re.M | re.S)
STORE_ENTRY = re.compile(r'&"(\w+)"\s*:\s*"([^"]+)"')
ENUM = re.compile(r"^[ \t]*enum\s+(?:class\s+|struct\s+)?(\w+)\s*(?::\s*[\w:]+\s*)?\{([^}]*)\}", re.M)  # GDScript and C++
COMMENT = re.compile(r"#[^\n]*|//[^\n]*|/\*.*?\*/", re.S)
IDENTIFIER = re.compile(r"[A-Za-z_]\w*")
LITERAL = re.compile(r"\"([\w.\-]+)\"")


def _walk(repo, top, exts):
    out = []
    for dirpath, dirs, names in os.walk(os.path.join(repo, top)):
        dirs[:] = sorted(d for d in dirs if not d.startswith("."))
        out += [os.path.relpath(os.path.join(dirpath, n), repo).replace(os.sep, "/") for n in sorted(names) if n.endswith(exts)]
    return out


def _read(repo, rel):
    with open(os.path.join(repo, rel), encoding="utf-8", errors="replace") as fh:
        return fh.read()


def texts(repo, gaps):
    """The rows of every translation table: key -> text feature; and the languages: code -> the first table naming it."""
    out, languages = {}, {}
    for rel in _walk(repo, I18N, (".csv",)):
        with open(os.path.join(repo, rel), encoding="utf-8", newline="") as fh:
            reader, last = csv.reader(fh), 0
            header = next(reader, [])
            langs = [rules.language(c) for c in header[1:]]
            for c in langs:
                languages.setdefault(c, rel)
            last = reader.line_num
            for row in reader:
                line, last = last + 1, reader.line_num
                if not row or not row[0].strip():
                    continue
                if row[0] in out:
                    gaps["text keys written twice (the first kept)"] += 1
                    continue
                by_lang = {lang: v for lang, v in zip(langs, row[1:]) if v}
                label = by_lang.get("en") or next(iter(by_lang.values()), "")
                area = rules.area_of(*row[0].split(".")) or rules.module_area(rel)
                out[row[0]] = rules.feature(GAME, area, "text", row[0], f"{rel}:{line}", label=label, texts=by_lang)
    return out, languages


def store_api(repo):
    """StoreApi's commands (actions) and queries, each with the core method that runs it."""
    text, out = _read(repo, STORE_API), []
    lines = ts.Lines(text)
    for block in STORE_DICT.finditer(text):
        kind, what = ("action", "command") if block.group(1) == "COMMANDS" else ("query", "query")
        for m in STORE_ENTRY.finditer(text, block.start(2), block.end(2)):
            name, method = m.groups()
            owner = method.split("/")[0] if "/" in method else ""
            area = rules.area_of(name, owner) or rules.module_area(STORE_API)
            out.append(rules.feature(GAME, area, kind, name, f"{STORE_API}:{lines.of(m.start())}",
                                     description=f"StoreApi {what}, run by the core's {method}",
                                     id=rules.norm_id(name), keys=rules.label_keys("", name)))
    return out


def enums(rel, text):
    """The members of each enum (GDScript or C++) named as the player's actions (tool, mode, action): each comma's part,
    its first name."""
    out, lines = [], ts.Lines(text)
    for m in ENUM.finditer(text):
        if not rules.names_actions(m.group(1)):
            continue
        body = COMMENT.sub(lambda c: " " * len(c.group()), m.group(2))  # comments blanked, offsets kept
        for part in re.finditer(r"[^,]+", body):
            member = IDENTIFIER.search(part.group())
            if member is None:
                continue
            name = member.group()
            member_at = part.start() + member.start()
            area = rules.area_of(name, m.group(1)) or rules.module_area(rel)
            out.append(rules.feature(GAME, area, "action", f"{m.group(1)}.{name}", f"{rel}:{lines.of(m.start(2) + member_at)}",
                                     description=f"a member of the enum {m.group(1)} ({rel})", id=rules.norm_id(name),
                                     keys=rules.label_keys(m.group(1), rules.camel(name))))
    return out


def _modules(repo, gaps):
    """One system a native module (its .h and .cpp), one a script or shader; one test a test script."""
    out, grouped = [], {}
    for rel in _walk(repo, NATIVE, NATIVE_CODE):
        grouped.setdefault(os.path.splitext(rel)[0], []).append(rel)
    for name, rels in grouped.items():
        read = [(r, _read(repo, r)) for r in sorted(rels, key=lambda r: not r.endswith(".h"))]
        described = [(r, rules.first_comment(text)) for r, text in read]
        rel, description = next(((r, d) for r, d in described if d), described[0])
        gaps["modules with no first comment"] += 0 if description else 1
        out.append(rules.feature(GAME, rules.module_area(rel), "system", name, f"{rel}:1", description=description))
        for r, text in read:
            out += enums(r, text)
    scripts = [(rel, "system") for top in SCRIPTS for rel in _walk(repo, top, SCRIPT_CODE)]
    scripts += [(rel, "test") for rel in _walk(repo, TESTS, (".gd",))]
    for rel, kind in scripts:
        text = _read(repo, rel)
        description = rules.first_comment(text, hash_comments=rel.endswith(".gd"))
        gaps["modules with no first comment"] += 0 if description else 1
        out.append(rules.feature(GAME, rules.module_area(rel), kind, os.path.splitext(rel)[0], f"{rel}:1",
                                 description=description))
        if kind == "system" and rel.endswith(".gd"):
            out += enums(rel, text)
    native = sum(len(rels) for rels in grouped.values())
    code = [rel for rels in grouped.values() for rel in rels] + [rel for rel, kind in scripts if kind == "system"
                                                                 and rel.endswith(".gd")]
    return out, code, native + len(scripts)


def _uses(repo, code, used, keys):
    """Each text key's uses: a string literal equal to it in a script or a native file (the first in each file), then the
    text key fields of the interface tables."""
    found = {}
    for rel in code:
        text, lines, here = _read(repo, rel), None, set()
        for m in LITERAL.finditer(text):
            key = m.group(1)
            if key in keys and key not in here:
                here.add(key)
                lines = lines or ts.Lines(text)
                found.setdefault(key, []).append(f"{rel}:{lines.of(m.start())}")
    for key, at in used:
        found.setdefault(key, []).append(at)
    return found


def read(repo):
    """Every feature the rules find: {"features", "files", "languages", "gaps"}."""
    gaps = {"text keys written twice (the first kept)": 0, "modules with no first comment": 0,
            "tables that cannot be read": 0, "interface text keys with no text": 0}
    by_key, languages = texts(repo, gaps)
    out = store_api(repo)
    modules, code, code_files = _modules(repo, gaps)
    used = []
    ui = _walk(repo, UI, (".json",))
    for rel in ui:
        found, keys, why = inventory_json.controls(GAME, rel, _read(repo, rel), by_key)
        out += found
        used += keys
        gaps["tables that cannot be read"] += 1 if why else 0
        gaps["interface text keys with no text"] += sum(1 for k, _at in keys if k not in by_key)
    data = [rel for top in DATA for rel in _walk(repo, top, (".json",))]
    for rel in data:
        found, why = inventory_json.entries(GAME, rel, _read(repo, rel))
        out += found
        gaps["tables that cannot be read"] += 1 if why else 0
    rules.mark_uses(by_key.values(), _uses(repo, code, used, by_key))
    files = len(ui) + len(data) + code_files + len(_walk(repo, I18N, (".csv",))) + 1  # and StoreApi
    return {"features": list(by_key.values()) + out + modules, "files": files, "languages": languages,
            "gaps": [{"game": GAME, "what": k, "count": v} for k, v in gaps.items() if v]}
