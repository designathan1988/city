"""The inventory's reading of JSON tables (tools/atlas/inventory.py), with the line of each value: Python's json gives no
positions, so every `"key": value` pair of the text is queued by (key, value) in the order it is written and taken back in the
same order as the parsed table is walked (json keeps a document's order), which gives each pair its own line. Two rules read
features from a table: its entries (an object with an id or name, and the keys of a map of named objects) and its controls
(an interface object carrying a text key, a tool, an action or an id)."""
import json
import os
import re
import sys
from collections import deque

TOOLS_DIR = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
if TOOLS_DIR not in sys.path:
    sys.path.insert(0, TOOLS_DIR)
from atlas import inventory_rules as rules  # noqa: E402

# Every string is matched whole, a key's value after it when a colon follows; a string's body is the unrolled loop
# [^"\\]*(?:\\.[^"\\]*)*, whose class runs as one repeat (an alternation per character took 1.4 s on a 7 MB string).
STRING = r'"([^"\\\n]*(?:\\.[^"\\\n]*)*)"'
PAIR = re.compile(STRING + r'(?:\s*:\s*("[^"\\\n]*(?:\\.[^"\\\n]*)*"|-?\d+(?:\.\d+)?(?:[eE][+-]?\d+)?|true|false|null|\{|\[))?')
ID_FIELDS = ("id", "name")  # the field an entry of a table is named by
CONTROL_NAMES = ("tool", "action", "id", "do")  # what an interface object is named by, in this order
TEXT_KEY = "_key"  # a field ending so holds a text key (label_key, title_key, note_key)
NAMED_MAP = 2  # a map of named objects has at least this many


def _value_key(v):
    if isinstance(v, dict):
        return ("@", "{")
    if isinstance(v, list):
        return ("@", "[")
    return (type(v).__name__, v)


def _strings(text):
    """PAIR matched at each string's opening quote, found by str.find: the regex engine's own search tries every
    character (1.4 s on a 7 MB table of numbers holding 2360 quotes), find skips to the next quote."""
    at = text.find('"')
    while at != -1:
        m = PAIR.match(text, at)
        if m is None:  # an unterminated string: no JSON, the parse refuses it
            at = text.find('"', at + 1)
            continue
        yield m
        at = text.find('"', m.end())


def positions(text):
    """(key, value) -> the lines it is written on, in order (a container value is keyed by its opening bracket)."""
    out, line, last = {}, 1, 0
    for m in _strings(text):
        if m.group(2) is None:
            continue
        line += text.count("\n", last, m.start())
        last = m.start()
        try:
            key = json.loads('"' + m.group(1) + '"')
            raw = m.group(2)
            value = ("@", raw) if raw in ("{", "[") else _value_key(json.loads(raw))
        except ValueError:
            continue
        out.setdefault((key, value), deque()).append(line)
    return out


def walk(node, pos, visit, path=()):
    """Every object of a parsed table, its pairs taken in document order: visit(obj, path, lines), lines its key -> line."""
    if isinstance(node, dict):
        lines = {}
        for k, v in node.items():
            queue = pos.get((k, _value_key(v)))
            lines[k] = queue.popleft() if queue else None
            if isinstance(v, (dict, list)):
                walk(v, pos, visit, path + (k,))
        visit(node, path, lines)
    elif isinstance(node, list):
        for v in node:
            walk(v, pos, visit, path)


def load(text):
    """The parsed table and its positions, or (None, the reason it cannot be read)."""
    try:
        return json.loads(text), positions(text)
    except ValueError as e:
        return None, str(e)


def _at(rel, line):
    return f"{rel}:{line or 1}"


def entries(game, rel, text):
    """The entries of a data table: each object named by an id field, and each key of a map whose values are all objects
    (two or more): (features, the reason nothing could be read or None)."""
    data, pos = load(text)
    if data is None:
        return [], pos
    stem = os.path.splitext(os.path.basename(rel))[0]
    out = []

    def add(name, path, line, how):
        table = "/".join((stem,) + path)
        area = rules.area_of(stem, *path, name) or rules.module_area(rel)
        out.append(rules.feature(game, area, "data", f"{table}/{name}", _at(rel, line), description=f"{how} of {table}",
                                 id=rules.norm_id(name), table=table))

    def visit(obj, path, lines):
        for field in ID_FIELDS:
            if isinstance(obj.get(field), str):
                add(obj[field], path, lines[field], f"an entry named by its {field}")
                break
        named = [k for k in obj if not k.startswith("_")]
        if len(named) >= NAMED_MAP and all(isinstance(obj[k], dict) for k in named):
            for k in named:
                add(k, path, lines[k], "a named entry")
    walk(data, pos, visit)
    return sorted(out, key=lambda f: int(f["source"].rsplit(":", 1)[1])), None


def controls(game, rel, text, texts):
    """The interface objects of a table: each carrying a text key field (`*_key`), a tool, an action or an id, named by
    the first of tool, action, id, do; its label the text of its label or title key. (features, keys used, reason or None)."""
    data, pos = load(text)
    if data is None:
        return [], [], pos
    stem = os.path.splitext(os.path.basename(rel))[0]
    out, used = [], []

    def visit(obj, path, lines):
        keys = [(k, v) for k, v in obj.items() if k.endswith(TEXT_KEY) and isinstance(v, str)]
        used.extend((v, _at(rel, lines[k])) for k, v in keys)
        named = next((f for f in CONTROL_NAMES if isinstance(obj.get(f), str)), None)
        if not keys and named is None:
            return
        name = obj[named] if named else keys[0][1]
        label_key = obj.get("label_key") or obj.get("title_key") or (keys[0][1] if keys else "")
        table = "/".join((stem,) + path)
        rest = ", ".join(f"{k}={v}" for k, v in obj.items() if k != named and not isinstance(v, (dict, list))
                         and not k.startswith("_"))
        line = min((ln for ln in lines.values() if ln), default=1)
        area = rules.area_of(name, label_key, *path[::-1]) or rules.area_of(stem) or rules.module_area(rel)
        out.append(rules.feature(game, area, "action", name, _at(rel, line), description=f"control of {table}: {rest}",
                                 id=rules.norm_id(name), key=label_key if label_key in texts else ""))
    walk(data, pos, visit)
    return sorted(out, key=lambda f: int(f["source"].rsplit(":", 1)[1])), used, None
