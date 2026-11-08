"""Data gates: every data table parses, names each key of an object once, and matches its schema (tools/tasks/schema.py,
the tools' one checker)."""
import os
import sys

import jsonfile

TOOLS_DIR = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))


def _schema_errors(value, schema):
    """The one JSON Schema checker of the tools (tools/tasks/schema.py), its data-table keywords included."""
    if TOOLS_DIR not in sys.path:
        sys.path.insert(0, TOOLS_DIR)
    from tasks.schema import errors
    return errors(value, schema)


def inputs(ctx, path):
    return (path.startswith("project/data/") and path.endswith(".json")) or path.startswith("tools/gates/schemas/")


def check_data(ctx, err):
    rules = ctx.rules
    for f in ctx.files:
        if f.startswith("project/data/") and f.endswith(".json") and f not in rules["schemas"]:
            err("DADOS_SEM_SCHEMA", f, "every data table needs a schema in rules.json/schemas")
    for data_path, schema_path in rules["schemas"].items():
        full = os.path.join(ctx.root, data_path)
        if not os.path.exists(full):
            err("DADOS", data_path, "data table missing")
            continue
        try:
            data = jsonfile.load(full)
            schema = jsonfile.load(os.path.join(ctx.root, schema_path))
        except (OSError, ValueError) as e:  # malformed, or a key written twice (jsonfile)
            err("DADOS", data_path, f"invalid JSON: {e}")
            continue
        for e in _schema_errors(data, schema):
            err("DADOS", data_path, e)
