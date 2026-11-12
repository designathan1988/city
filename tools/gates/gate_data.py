"""Data gates: every data table parses with no key written twice in one object and matches its schema, checked by
tools/tasks/schema.py, the tools' one JSON Schema checker (a keyword it does not support is an error of the schema,
never a check skipped), in its complete mode: every object of a table is described and closed and every list says what it
holds, unless the schema marks the node free with the reason."""
import json
import os
import re
import sys

TOOLS_DIR = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
if TOOLS_DIR not in sys.path:
    sys.path.insert(0, TOOLS_DIR)
from atlas import problems as register  # noqa: E402  the register's one owner, whose rule this gate holds
from tasks import schema as json_schema  # noqa: E402


class DuplicateKey(ValueError):
    """A JSON object names one key twice: Python's and Godot's readers keep the last value without a word."""

    def __init__(self, key):
        super().__init__(key)
        self.key = key


def _unique_pairs(pairs):
    out = {}
    for key, value in pairs:
        if key in out:
            raise DuplicateKey(key)
        out[key] = value
    return out


def load_strict(path):
    """A JSON file whose objects each name a key once; raises DuplicateKey or json.JSONDecodeError."""
    with open(path, encoding="utf-8") as fh:
        return json.load(fh, object_pairs_hook=_unique_pairs)


def _key_lines(path, key):
    """The lines where a key is written, to point at both places of a duplicate."""
    rx = re.compile(r'"' + re.escape(key) + r'"\s*:')
    with open(path, encoding="utf-8") as fh:
        return [n for n, line in enumerate(fh, 1) if rx.search(line)]


def inputs(ctx, path):
    return (path.startswith("project/data/") and path.endswith(".json")) or path.startswith("tools/gates/schemas/") or path in ctx.rules["schemas"]


def _load(ctx, rel, err, owner):
    full = os.path.join(ctx.root, rel)
    try:
        return load_strict(full)
    except DuplicateKey as e:
        err("DATA", owner, f"{rel}: key '{e.key}' written twice in one object (lines {', '.join(map(str, _key_lines(full, e.key)))})")
    except (OSError, json.JSONDecodeError) as e:
        err("DATA", owner, f"{rel}: invalid JSON: {e}")
    return None


def _laid(under, over):
    """`over` laid on a copy of `under` block by block, as DataTables._lay does: a dictionary into a dictionary, anything else
    replaces."""
    out = dict(under)
    for key, value in over.items():
        out[key] = _laid(out[key], value) if isinstance(value, dict) and isinstance(out.get(key), dict) else value
    return out


def _styled_errors(data, schema):
    """A styled table (rules.json styled_tables, DataTables.STYLED) as each of its styles makes it: the style laid over the
    table, the styles taken away, judged by the same schema. So one schema holds the table and every style's overlay."""
    base = {k: v for k, v in data.items() if k != "styles"}
    whole = dict(schema, required=[k for k in schema.get("required", []) if k != "styles"])
    out = []
    for style, over in data.get("styles", {}).items():
        if style.startswith("_") or not isinstance(over, dict):
            continue
        out += [f"styles.{style} laid over the table: {e}" for e in json_schema.errors(_laid(base, over), whole, schema, complete=True)]
    return out


def register_inputs(ctx, path):
    entry = ctx.rules["register"]
    return path in (entry["file"], entry["schema"])


def register_extra(ctx):
    """What the register's verdict reads besides its inputs: whether each piece of evidence it names is there."""
    entry = ctx.rules["register"]
    try:
        problems = register.read(os.path.join(ctx.root, entry["file"]))
    except register.RegisterError:
        return []
    named = sorted({e for p in problems for e in (p.get("evidence") or []) if isinstance(e, str)})
    return [[e, e.startswith(register.GIT_REF) or os.path.exists(os.path.join(ctx.root, e))] for e in named]


def check_register(ctx, err):
    """PROBLEM_REGISTER: the one register of known problems holds by its schema and by the rules no keyword states
    (tools/atlas/problems.py faults); a register missing or unreadable is a finding, never an empty pass."""
    entry = ctx.rules["register"]
    path = os.path.join(ctx.root, entry["file"])
    if not os.path.isfile(path):
        err("PROBLEM_REGISTER", entry["file"], "the register is missing: every known problem is kept there")
        return
    schema = _load(ctx, entry["schema"], err, entry["file"])
    if schema is None:
        return
    try:
        problems = register.read(path)
    except register.RegisterError as e:
        err("PROBLEM_REGISTER", entry["file"], str(e))
        return
    for where, reason in register.faults(problems, schema, ctx.root, lambda ref: register.git_ref_exists(ctx.root, ref)):
        err("PROBLEM_REGISTER", f"{entry['file']}:{where}", reason)


def check_data(ctx, err):
    rules = ctx.rules
    for f in ctx.files:
        if f.startswith("project/data/") and f.endswith(".json") and f not in rules["schemas"]:
            err("DATA_WITHOUT_SCHEMA", f, "every data table needs a schema in rules.json/schemas")
    for data_path, schema_path in rules["schemas"].items():
        if not os.path.exists(os.path.join(ctx.root, data_path)):
            err("DATA", data_path, "data table missing")
            continue
        data = _load(ctx, data_path, err, data_path)
        schema = _load(ctx, schema_path, err, data_path)
        if data is None or schema is None:
            continue
        for e in json_schema.errors(data, schema, complete=True):
            err("DATA", data_path, e)
        if data_path in rules.get("styled_tables", []):
            for e in _styled_errors(data, schema):
                err("DATA", data_path, e)
