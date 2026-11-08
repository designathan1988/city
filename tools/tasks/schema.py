# The one JSON Schema checker of the tools (stdlib only): the data tables' schemas (tools/gates/gate_data.py) and the suites'
# (suites.py) both ask here. The keywords: type, enum, const, required, properties, additionalProperties, items, minItems,
# maxItems, minLength, pattern, minimum, maximum, exclusiveMinimum, oneOf, allOf, $ref to #/$defs; and the data tables' own:
# strict (no key but the properties), additional (the schema of every other key), pattern_prefix (a string's start) and
# unique_key (a list of objects with that key once each). An unknown keyword is an error of the schema.
from __future__ import annotations

import re

TYPES = {"object": dict, "array": list, "string": str, "boolean": bool, "null": type(None)}
KNOWN = {"$schema", "$id", "title", "description", "$comment", "$defs", "_doc", "type", "enum", "const", "required", "properties",
         "additionalProperties", "items", "minItems", "maxItems", "minLength", "pattern", "minimum", "maximum", "exclusiveMinimum", "oneOf",
         "allOf", "$ref", "strict", "additional", "pattern_prefix", "unique_key"}


def _is(value, kind: str) -> bool:
    if kind == "integer":
        return isinstance(value, int) and not isinstance(value, bool)
    if kind == "number":
        return isinstance(value, (int, float)) and not isinstance(value, bool)
    return isinstance(value, TYPES[kind])


def _extra(schema: dict):
    """What a key outside `properties` must be: False (none allowed), a schema, or True (anything)."""
    if schema.get("strict"):
        return False
    if "additional" in schema:
        return schema["additional"]
    return schema.get("additionalProperties", True)


def errors(value, schema: dict, root: dict | None = None, where: str = "$") -> list[str]:
    """Every place where `value` breaks `schema`, as 'path: reason'."""
    root = root if root is not None else schema
    unknown = set(schema) - KNOWN
    if unknown:
        return [f"{where}: schema keyword not supported: {', '.join(sorted(unknown))}"]
    if "$ref" in schema:
        ref = schema["$ref"]
        if not ref.startswith("#/$defs/"):
            return [f"{where}: only #/$defs references are supported ({ref})"]
        return errors(value, root["$defs"][ref[len("#/$defs/"):]], root, where)
    out = []
    for part in schema.get("allOf", []):
        out += errors(value, part, root, where)
    if "type" in schema:
        kinds = schema["type"] if isinstance(schema["type"], list) else [schema["type"]]
        if not any(_is(value, k) for k in kinds):
            return [f"{where}: expected {' or '.join(kinds)}, got {type(value).__name__}"]
    if "enum" in schema and value not in schema["enum"]:
        out.append(f"{where}: {value!r} not one of {schema['enum']}")
    if "const" in schema and value != schema["const"]:
        out.append(f"{where}: must be {schema['const']!r}")
    if "oneOf" in schema:
        passing = [s for s in schema["oneOf"] if not errors(value, s, root, where)]
        if len(passing) != 1:
            out.append(f"{where}: matches {len(passing)} of the oneOf alternatives, not exactly one")
    if isinstance(value, dict):
        for k in schema.get("required", []):
            if k not in value:
                out.append(f"{where}: missing '{k}'")
        props = schema.get("properties", {})
        extra = _extra(schema)
        for k, v in value.items():
            if k in props:
                out += errors(v, props[k], root, f"{where}.{k}")
            elif extra is False:
                out.append(f"{where}: unexpected '{k}'")
            elif isinstance(extra, dict):
                out += errors(v, extra, root, f"{where}.{k}")
    if isinstance(value, list):
        if len(value) < schema.get("minItems", 0):
            out.append(f"{where}: needs at least {schema['minItems']} items")
        if "maxItems" in schema and len(value) > schema["maxItems"]:
            out.append(f"{where}: holds at most {schema['maxItems']} items")
        if "items" in schema:
            for i, v in enumerate(value):
                out += errors(v, schema["items"], root, f"{where}[{i}]")
        if schema.get("unique_key"):
            keys = [v.get(schema["unique_key"]) for v in value if isinstance(v, dict)]
            if len(keys) != len(set(keys)):
                out.append(f"{where}: '{schema['unique_key']}' repeated")
    if isinstance(value, str):
        if len(value) < schema.get("minLength", 0):
            out.append(f"{where}: shorter than {schema['minLength']}")
        if "pattern" in schema and not re.search(schema["pattern"], value):
            out.append(f"{where}: does not match {schema['pattern']!r}")
        if "pattern_prefix" in schema and not value.startswith(schema["pattern_prefix"]):
            out.append(f"{where}: must start with {schema['pattern_prefix']!r}")
    if _is(value, "number"):
        if "minimum" in schema and value < schema["minimum"]:
            out.append(f"{where}: below {schema['minimum']}")
        if "maximum" in schema and value > schema["maximum"]:
            out.append(f"{where}: above {schema['maximum']}")
        if "exclusiveMinimum" in schema and value <= schema["exclusiveMinimum"]:
            out.append(f"{where}: must be above {schema['exclusiveMinimum']}")
    return out
