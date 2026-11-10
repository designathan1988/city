# A small JSON Schema checker (the keywords the suites schema uses: type, enum, const, required, properties,
# additionalProperties, items, minItems, minLength, minimum, exclusiveMinimum, oneOf, allOf, $ref to #/$defs). Stdlib only.
from __future__ import annotations

TYPES = {"object": dict, "array": list, "string": str, "boolean": bool, "null": type(None)}
KNOWN = {"$schema", "$id", "title", "description", "$comment", "$defs", "type", "enum", "const", "required", "properties",
         "additionalProperties", "items", "minItems", "minLength", "minimum", "exclusiveMinimum", "oneOf", "allOf", "$ref"}


def _is(value, kind: str) -> bool:
    if kind == "integer":
        return isinstance(value, int) and not isinstance(value, bool)
    if kind == "number":
        return isinstance(value, (int, float)) and not isinstance(value, bool)
    return isinstance(value, TYPES[kind])


def errors(value, schema: dict, root: dict | None = None, where: str = "$") -> list[str]:
    """Every place where `value` breaks `schema`, as 'path: reason'. An unknown keyword is an error of the schema."""
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
        extra = schema.get("additionalProperties", True)
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
        if "items" in schema:
            for i, v in enumerate(value):
                out += errors(v, schema["items"], root, f"{where}[{i}]")
    if isinstance(value, str) and len(value) < schema.get("minLength", 0):
        out.append(f"{where}: shorter than {schema['minLength']}")
    if _is(value, "number"):
        if "minimum" in schema and value < schema["minimum"]:
            out.append(f"{where}: below {schema['minimum']}")
        if "exclusiveMinimum" in schema and value <= schema["exclusiveMinimum"]:
            out.append(f"{where}: must be above {schema['exclusiveMinimum']}")
    return out
