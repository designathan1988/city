"""Material tables against their shaders: each material's params are set on its shader by name (render/city_world.gd
materials(), set_shader_parameter), and Godot drops a name the shader does not declare without a word. So every param
must be a uniform its shader declares (the shader and the .gdshaderinc files it includes, read here: the shader is the
one owner of its uniforms, never a list in a schema), and its value must be what city_world.gd shader_value turns into
that uniform's type: a res:// path for a sampler, a '#' colour for a vec3 or vec4, colours for a vec3 or vec4 array,
numbers for a float array, a number for a float or an int, true or false for a bool."""
import json
import os
import re

INCLUDE = re.compile(r'^[ \t]*#include\s+"([^"]+)"', re.M)
UNIFORM = re.compile(r"\b(?:global\s+|instance\s+)?uniform\s+(?:(?:lowp|mediump|highp)\s+)?(\w+)\s+(\w+)\s*(\[[^\]]*\])?")
COMMENTS = re.compile(r"//[^\n]*|/\*.*?\*/", re.S)
COLOUR = re.compile(r"^#([0-9a-fA-F]{6}|[0-9a-fA-F]{8})$")
VECTORS = {"vec3", "vec4"}
SCALARS = {"float", "int", "uint"}


def _res(path, base=None):
    """A shader path as written (res:// or relative to the including file) to a path in the repository."""
    if path.startswith("res://"):
        return "project/" + path[len("res://"):]
    return os.path.normpath(os.path.join(os.path.dirname(base or ""), path)).replace("\\", "/")


def uniforms_of(ctx, shader, seen=None):
    """{name: (type, is_array)} declared by a shader file and every file it includes; None when the file is missing."""
    seen = set() if seen is None else seen
    if shader in seen:
        return {}
    seen.add(shader)
    full = os.path.join(ctx.root, shader)
    if not os.path.isfile(full):
        return None
    with open(full, encoding="utf-8") as fh:
        raw = fh.read()
    out = {}
    for inc in INCLUDE.findall(raw):  # on the raw text: a res:// path holds '//', and a commented include does not start its line
        out.update(uniforms_of(ctx, _res(inc, shader), seen) or {})
    for kind, name, array in UNIFORM.findall(COMMENTS.sub(" ", raw)):
        out[name] = (kind, bool(array))
    return out


def _fits(value, kind, array):
    """Whether a table value becomes this uniform's type (city_world.gd shader_value)."""
    if isinstance(value, bool):
        return kind == "bool" and not array
    if isinstance(value, (int, float)):
        return kind in SCALARS and not array
    if isinstance(value, str):
        if value.startswith("res://"):
            return "sampler" in kind and not array
        return bool(COLOUR.match(value)) and kind in VECTORS and not array
    if isinstance(value, list) and value:
        if all(isinstance(v, str) and COLOUR.match(v) for v in value):
            return kind in VECTORS and array
        if all(isinstance(v, (int, float)) and not isinstance(v, bool) for v in value):
            return kind == "float" and array
    return False


def inputs(ctx, path):
    tables = set(ctx.rules.get("material_tables", [])) | {u["table"] for u in ctx.rules.get("uniform_tables", [])}
    return path in tables or path.startswith("project/render/shaders/")


def _check_params(ctx, err, table, where, params, shaders):
    """Each key of `params` (keys starting with '_' are notes) a uniform of one of `shaders`, with a value of its type."""
    uniforms = {}
    for shader in shaders:
        found = uniforms_of(ctx, _res(shader))
        if found is None:
            err("SHADER_PARAM", table, f"{where}: shader {shader} is missing")
            return
        uniforms.update(found)
    for param, value in params.items():
        if param.startswith("_"):
            continue
        if param not in uniforms:
            err("SHADER_PARAM", table, f"{where}.{param}: no uniform of {', '.join(shaders)} (or their includes) has this name")
        elif not _fits(value, *uniforms[param]):
            kind, array = uniforms[param]
            err("SHADER_PARAM", table, f"{where}.{param}: {json.dumps(value)[:60]} does not become a {kind}{'[]' if array else ''}")


def check_materials(ctx, err):
    for spec in ctx.rules.get("uniform_tables", []):  # a block of a table set key by key on the listed shaders
        full = os.path.join(ctx.root, spec["table"])
        if not os.path.isfile(full):
            err("SHADER_PARAM", spec["table"], "table missing")
            continue
        with open(full, encoding="utf-8") as fh:
            block = json.load(fh)
        for step in spec["path"].split("."):
            block = block.get(step) if isinstance(block, dict) else None
        if not isinstance(block, dict):  # a block the rule names and the table lacks is a finding, never nothing to check
            err("SHADER_PARAM", spec["table"], f"{spec['path']}: no such block of uniforms in the table")
            continue
        _check_params(ctx, err, spec["table"], spec["path"], block, spec["shaders"])
    for table in ctx.rules.get("material_tables", []):
        full = os.path.join(ctx.root, table)
        if not os.path.isfile(full):
            err("SHADER_PARAM", table, "material table missing")
            continue
        with open(full, encoding="utf-8") as fh:
            materials = json.load(fh)
        for name, row in materials.items():
            if name.startswith("_") or not isinstance(row, dict) or "shader" not in row:
                continue
            _check_params(ctx, err, table, f"{name}.params", row.get("params", {}), [row["shader"]])
