"""Data gates: every data table parses and matches its schema (minimal JSON-Schema subset, stdlib only)."""
import json
import os

TYPES = {"object": dict, "array": list, "string": str, "boolean": bool, "integer": int, "number": (int, float)}


def validate(value, schema, where, errors):
    t = schema.get("type")
    if t:
        ok = isinstance(value, TYPES[t]) and not (t in ("integer", "number") and isinstance(value, bool))
        if not ok:
            errors.append(f"{where}: esperado {t}")
            return
    if "enum" in schema and value not in schema["enum"]:
        errors.append(f"{where}: valor {value!r} fora de {schema['enum']}")
    if "minimum" in schema and isinstance(value, (int, float)) and value < schema["minimum"]:
        errors.append(f"{where}: {value} < mínimo {schema['minimum']}")
    if "maximum" in schema and isinstance(value, (int, float)) and value > schema["maximum"]:
        errors.append(f"{where}: {value} > máximo {schema['maximum']}")
    if "pattern_prefix" in schema and isinstance(value, str) and not value.startswith(schema["pattern_prefix"]):
        errors.append(f"{where}: deve começar com {schema['pattern_prefix']!r}")
    if isinstance(value, dict):
        for key in schema.get("required", []):
            if key not in value:
                errors.append(f"{where}: falta a chave '{key}'")
        props = schema.get("properties", {})
        for key, sub in value.items():
            if key in props:
                validate(sub, props[key], f"{where}.{key}", errors)
            elif "additional" in schema:
                validate(sub, schema["additional"], f"{where}.{key}", errors)
            elif schema.get("strict"):
                errors.append(f"{where}: chave desconhecida '{key}'")
    if isinstance(value, list):
        if "minItems" in schema and len(value) < schema["minItems"]:
            errors.append(f"{where}: mínimo {schema['minItems']} itens")
        if "maxItems" in schema and len(value) > schema["maxItems"]:
            errors.append(f"{where}: máximo {schema['maxItems']} itens")
        for i, item in enumerate(value):
            if "items" in schema:
                validate(item, schema["items"], f"{where}[{i}]", errors)
        if schema.get("unique_key"):
            keys = [v.get(schema["unique_key"]) for v in value if isinstance(v, dict)]
            if len(keys) != len(set(keys)):
                errors.append(f"{where}: '{schema['unique_key']}' repetido")


def check_data(root, rules, files, err):
    for f in files:
        if f.startswith("project/data/") and f.endswith(".json") and f not in rules["schemas"]:
            err("DADOS_SEM_SCHEMA", f, "toda tabela de dados precisa de schema em rules.json/schemas")
    for data_path, schema_path in rules["schemas"].items():
        full = os.path.join(root, data_path)
        if not os.path.exists(full):
            err("DADOS", data_path, "tabela de dados ausente")
            continue
        try:
            with open(full, encoding="utf-8") as fh:
                data = json.load(fh)
            with open(os.path.join(root, schema_path), encoding="utf-8") as fh:
                schema = json.load(fh)
        except (OSError, json.JSONDecodeError) as e:
            err("DADOS", data_path, f"JSON inválido: {e}")
            continue
        errors = []
        validate(data, schema, "$", errors)
        for e in errors[:20]:
            err("DADOS", data_path, e)
