"""Code-level gates: C++ layer dependencies from #include directives, and orphan scripts and shaders (no scene, script,
shader, project.godot, data table or core source names them in code)."""
import cpp_syntax

REFERRERS = (".gd", ".tscn", ".tres", ".godot", ".gdshader", ".gdshaderinc")
JUDGED = (".gd", ".gdshader", ".gdshaderinc")
# What else names a resource by its path: the data tables (materials.json's shaders) and the core (a shader it loads by name).
OTHER_REFERRERS = (("project/data/", (".json",)), ("native/src/", (".cpp", ".h")))


def layer_of(path, layers):
    best = None
    for prefix in layers:
        if path.startswith(prefix + "/") and (best is None or len(prefix) > len(best)):
            best = prefix
    return best


def applies_layers(ctx, path):
    return path.startswith("native/src/") and path.endswith((".cpp", ".h"))


def check_layers(ctx, src, err):
    layers = ctx.rules["cpp_layers"]
    layer = layer_of(src.path, layers)
    if layer is None:
        err("CAMADA_DESCONHECIDA", src.path, "folder without a rule in rules.json/cpp_layers")
        return
    for inc, angle, line in cpp_syntax.includes(src):
        if angle and "/" not in inc:
            continue  # standard library
        if not any(inc.startswith(p) for p in layers[layer]):
            err("CAMADA", f"{src.path}:{line}", f"'{inc}' is not allowed in {layer} (rules.json/cpp_layers)")


def _referrer(path):
    return (path.startswith("project/") and path.endswith(REFERRERS)) or any(
        path.startswith(root) and path.endswith(ext) for root, ext in OTHER_REFERRERS)


def orphan_inputs(ctx, path):
    return _referrer(path) or (path.startswith("project/") and path.endswith(".uid"))


def check_orphans(ctx, err):
    """Every GDScript and shader is named by another file's code: a res:// path, a uid:// id or its class_name (in the
    project's own code: a C++ identifier is not a GDScript class). The names are indexed once (word -> files), so the check
    is linear in the project's size."""
    files = [f for f in ctx.files if _referrer(f)]
    uid_to_path = {}
    for f in ctx.files:
        if f.startswith("project/") and f.endswith(".uid"):
            uid = ctx.facts(f).get("uid")
            if uid:
                uid_to_path[uid] = f[: -len(".uid")]
    referenced_by, word_files = {}, {}
    for f in files:
        facts = ctx.facts(f)
        if facts.get("lex_error"):
            err("ORFAO", f, f"cannot be read as code: {facts['lex_error']}")
            continue
        for ref in facts.get("refs", []):
            target = uid_to_path.get(ref) or ("project/" + ref[len("res://"):] if ref.startswith("res://") else None)
            if target:
                referenced_by.setdefault(target, set()).add(f)
        for w in facts.get("idents", []) if f.startswith("project/") else ():
            word_files.setdefault(w, set()).add(f)
    for f in files:
        if not f.endswith(JUDGED) or not f.startswith("project/") or f.startswith("project/tests/"):
            continue
        users = set(referenced_by.get(f, ())) - {f}
        cls = ctx.facts(f).get("class_name")
        if cls:
            users |= set(word_files.get(cls, ())) - {f}
        if not users:
            err("ORFAO", f, "no scene, script, shader, project.godot, data table or core source names this file: delete it or "
                "connect it")
