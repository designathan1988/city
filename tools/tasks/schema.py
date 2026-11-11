# The one JSON Schema checker of the tools (stdlib only): the data tables' schemas (tools/gates/gate_data.py) and the suites'
# (suites.py) both ask here. The keywords: type, enum, const, required, properties, additionalProperties, items, minItems,
# maxItems, minLength, pattern, minimum, maximum, exclusiveMinimum, oneOf, allOf, $ref to #/$defs, prefixItems (the schema of
# each leading position of a list, as JSON Schema 2020-12; `items` then judges the positions after them, and `items: false`
# allows none); and the data tables' own:
# strict (no key but the properties), additional (the schema of every other key), pattern_prefix (a string's start),
# unique_key (a list of objects with that key once each), uniqueItems (no member twice, compared by value) and free (the
# reason a node is left undescribed). An unknown keyword is an error of the schema.
# complete=True (the data tables' gate) also refuses a schema that leaves data undescribed: every object a table holds is
# described (properties or an additional schema) and closed (strict, additionalProperties false, or an additional schema),
# and every list says what it holds (items), unless its node says `free` with the reason.
from __future__ import annotations

import json
import re

TYPES = {"object": dict, "array": list, "string": str, "boolean": bool, "null": type(None)}
KNOWN = {"$schema", "$id", "title", "description", "$comment", "$defs", "_doc", "type", "enum", "const", "required", "properties",
         "additionalProperties", "items", "prefixItems", "minItems", "maxItems", "minLength", "pattern", "minimum", "maximum",
         "exclusiveMinimum", "oneOf", "allOf", "$ref", "strict", "additional", "pattern_prefix", "unique_key", "uniqueItems", "free"}


def _is(value, kind: str) -> bool:
    if kind == "integer":
        return isinstance(value, int) and not isinstance(value, bool)
    if kind == "number":
        return isinstance(value, (int, float)) and not isinstance(value, bool)
    return isinstance(value, TYPES[kind])


def _canon(value):
    """A value as JSON Schema compares it for uniqueItems: 1 and 1.0 alike, true never 1, an object's keys in any order."""
    if isinstance(value, bool):
        return ("bool", value)
    if isinstance(value, (int, float)):
        return ("number", float(value))
    if isinstance(value, dict):
        return ("object", tuple(sorted((k, _canon(v)) for k, v in value.items())))
    if isinstance(value, list):
        return ("array", tuple(_canon(v) for v in value))
    return (type(value).__name__, value)


def _extra(schema: dict):
    """What a key outside `properties` must be: False (none allowed), a schema, or True (anything)."""
    if schema.get("strict"):
        return False
    if "additional" in schema:
        return schema["additional"]
    return schema.get("additionalProperties", True)


def _undescribed(value, schema: dict, where: str) -> list[str]:
    """complete=True: what this node leaves undescribed about `value` (nothing for a node marked free with its reason)."""
    if "free" in schema:
        return [] if isinstance(schema["free"], str) and schema["free"].strip() else [f"{where}: free needs the reason, as text"]
    if "oneOf" in schema:
        return []  # the alternative that matches is held to it
    if isinstance(value, dict):
        extra = _extra(schema)
        if "properties" not in schema and not isinstance(extra, dict):
            return [f"{where}: object not described (give it properties, an additional schema, or free with the reason)"]
        if extra is True or extra == {}:  # an empty schema for the other keys takes anything: open, not described
            return [f"{where}: object open to any key (strict, additionalProperties false, an additional schema, or free with the reason)"]
    if isinstance(value, list) and "items" not in schema:
        if "prefixItems" not in schema:
            return [f"{where}: list of anything (give it items, or free with the reason)"]
        if len(value) > len(schema["prefixItems"]):
            return [f"{where}: positions past prefixItems undescribed (give it items, or items false)"]
    return []


def errors(value, schema: dict, root: dict | None = None, where: str = "$", complete: bool = False) -> list[str]:
    """Every place where `value` breaks `schema`, as 'path: reason'; with `complete`, also every node it leaves undescribed."""
    root = root if root is not None else schema
    unknown = set(schema) - KNOWN
    if unknown:
        return [f"{where}: schema keyword not supported: {', '.join(sorted(unknown))}"]
    if "$ref" in schema:
        ref = schema["$ref"]
        if not ref.startswith("#/$defs/"):
            return [f"{where}: only #/$defs references are supported ({ref})"]
        return errors(value, root["$defs"][ref[len("#/$defs/"):]], root, where, complete)
    out = []
    for part in schema.get("allOf", []):
        out += errors(value, part, root, where)  # a part adds constraints: the node itself is what must describe the value
    if "type" in schema:
        kinds = schema["type"] if isinstance(schema["type"], list) else [schema["type"]]
        if not any(_is(value, k) for k in kinds):
            return [f"{where}: expected {' or '.join(kinds)}, got {type(value).__name__}"]
    if complete:
        out += _undescribed(value, schema, where)
        complete = "free" not in schema  # below a free node nothing more is asked
    if "enum" in schema and value not in schema["enum"]:
        out.append(f"{where}: {value!r} not one of {schema['enum']}")
    if "const" in schema and value != schema["const"]:
        out.append(f"{where}: must be {schema['const']!r}")
    if "oneOf" in schema:
        # a node that describes the value itself (properties or an additional schema, items) may choose among alternatives that
        # only constrain it (which keys it has): those are not held to describe it again
        alt_complete = complete and not ("properties" in schema or isinstance(_extra(schema), dict) or "items" in schema or "prefixItems" in schema)
        passing = [s for s in schema["oneOf"] if not errors(value, s, root, where, alt_complete)]
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
                out += errors(v, props[k], root, f"{where}.{k}", complete)
            elif extra is False:
                out.append(f"{where}: unexpected '{k}'")
            elif isinstance(extra, dict):
                out += errors(v, extra, root, f"{where}.{k}", complete)
    if isinstance(value, list):
        if len(value) < schema.get("minItems", 0):
            out.append(f"{where}: needs at least {schema['minItems']} items")
        if "maxItems" in schema and len(value) > schema["maxItems"]:
            out.append(f"{where}: holds at most {schema['maxItems']} items")
        lead = schema.get("prefixItems", [])
        for i, (v, s) in enumerate(zip(value, lead)):
            out += errors(v, s, root, f"{where}[{i}]", complete)
        rest = schema.get("items", True)
        if rest is False and len(value) > len(lead):
            out.append(f"{where}: holds {len(value)} items, at most {len(lead)} allowed")
        elif isinstance(rest, dict):
            for i, v in enumerate(value[len(lead):], len(lead)):
                out += errors(v, rest, root, f"{where}[{i}]", complete)
        if schema.get("unique_key"):
            keys = [v.get(schema["unique_key"]) for v in value if isinstance(v, dict)]
            if len(keys) != len(set(keys)):
                out.append(f"{where}: '{schema['unique_key']}' repeated")
        if schema.get("uniqueItems"):
            seen = set()
            for i, v in enumerate(value):
                key = _canon(v)
                if key in seen:
                    out.append(f"{where}[{i}]: {json.dumps(v)[:60]} already in the list")
                seen.add(key)
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
