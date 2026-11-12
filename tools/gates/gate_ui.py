"""UI scripts keep no game state (Redux principle: keep state minimal and derive the rest): a member variable of a UI
script may only hold a Node reference or an engine object; @onready changes when a value is set, not what it is;
exported configuration is allowed while the script never writes it."""
import json
import os
import re

import gd_syntax

UI_ROOT = "project/ui/"
NODE_CALLS = {"get_node", "get_node_or_null", "find_child", "get_parent", "get_child", "get_first_node_in_group",
              "instantiate", "get_viewport", "get_window", "get_owner"}
OBJECT_CALLS = {"create_tween", "get_tree", "preload", "load", "get_theme_font", "get_theme_stylebox", "get_theme_icon",
                "duplicate"}
ASSIGN_OPS = {"=", "+=", "-=", "*=", "/=", "%=", "**=", "&=", "|=", "^=", "<<=", ">>="}
_FEATURES = re.compile(r'config/features=PackedStringArray\("(\d+)\.(\d+)"')


def api_path(root):
    """The engine's class list for the Godot version project.godot declares (godot-cpp's extension_api)."""
    with open(os.path.join(root, "project", "project.godot"), encoding="utf-8") as fh:
        m = _FEATURES.search(fh.read())
    if not m:
        raise ValueError("project/project.godot declares no config/features version")
    return f"native/godot-cpp/gdextension/extension_api-{m.group(1)}-{m.group(2)}.json"


def _autoloads(root):
    with open(os.path.join(root, "project", "project.godot"), encoding="utf-8") as fh:
        text = fh.read()
    section = text.split("[autoload]", 1)[1].split("\n[", 1)[0] if "[autoload]" in text else ""
    return {line.split("=", 1)[0].strip() for line in section.splitlines() if "=" in line}


def project_classes(ctx):
    """class_name -> extends of every project script, from the cached per-file facts."""
    def build():
        out = {}
        for f in ctx.files:
            if f.startswith("project/") and f.endswith(".gd"):
                facts = ctx.facts(f)
                if facts.get("class_name"):
                    out[facts["class_name"]] = facts.get("extends") or "RefCounted"
        return out
    return ctx.lazy("project_classes", build)


def context_inputs(ctx):
    return [api_path(ctx.root), sorted(project_classes(ctx).items()), sorted(_autoloads(ctx.root))]


class Types:
    def __init__(self, ctx):
        with open(os.path.join(ctx.root, api_path(ctx.root)), encoding="utf-8") as fh:
            api = json.load(fh)
        self.inherits = {c["name"]: c.get("inherits") for c in api["classes"]}
        self.project = project_classes(ctx)
        self.autoloads = _autoloads(ctx.root)

    def is_node(self, name, depth=0):
        if name == "Node":
            return True
        parent = self.project.get(name) if name in self.project else self.inherits.get(name)
        return bool(parent) and depth < 64 and self.is_node(parent, depth + 1)

    def is_reference(self, name):
        """A Node (engine or project) or an engine object: neither holds game numbers. A project class that is not a
        Node is a data holder."""
        return self.is_node(name) or (name in self.inherits and name not in self.project)


def _type_ok(types, toks):
    names = [t.text for t in toks if t.kind == "ident"]
    if not names:
        return False
    if names[0] in ("Array", "Dictionary") and len(names) > 1:
        return all(types.is_reference(n) for n in names[1:])
    return len(names) == 1 and types.is_reference(names[0])


def _init_ok(types, toks):
    if not toks:
        return False
    if toks[0].text in ("$", "%"):
        return True
    if len(toks) == 1 and toks[0].kind == "ident" and toks[0].text in types.autoloads:
        return True
    if len(toks) >= 3 and toks[0].kind == "ident" and toks[1].text == "." and toks[2].text == "new":
        return types.is_reference(toks[0].text)
    calls = [toks[k].text for k in range(len(toks) - 1) if toks[k].kind == "ident" and toks[k + 1].text == "("]
    depth, last = 0, None
    for k, t in enumerate(toks):
        if t.text in ("(", "[", "{"):
            depth += 1
        elif t.text in (")", "]", "}"):
            depth -= 1
        elif depth == 0 and t.kind == "ident" and k + 1 < len(toks) and toks[k + 1].text == "(":
            last = t.text
    return bool(calls) and (last in NODE_CALLS or last in OBJECT_CALLS)


def _written(lines, decl):
    for ln in lines:
        if ln.first == decl.line:
            continue
        toks = ln.toks
        for i, t in enumerate(toks[:-1]):
            if t.text == decl.name and toks[i + 1].text in ASSIGN_OPS:
                prev = toks[i - 1].text if i else ""
                if prev != "." or (i >= 2 and toks[i - 2].text == "self"):
                    return True
    return False


def applies(ctx, path):
    return path.startswith(UI_ROOT) and path.endswith(".gd")


def check_ui_state(ctx, src, err):
    types = ctx.lazy("ui_types", lambda: Types(ctx))
    lines = gd_syntax.logical_lines(src)
    for d in gd_syntax.declarations(lines):
        if d.keyword != "var" or not d.member:
            continue
        exported = any(a.startswith("export") for a in d.annotations)
        if exported and not _written(lines, d):
            continue
        ok = _type_ok(types, d.type_toks) if d.type_toks else _init_ok(types, d.init_toks)
        if not ok:
            what = "".join(t.text for t in d.type_toks) or "a value"
            why = "an exported value the script writes" if exported else what
            err("STATE_IN_UI", f"{src.path}:{d.line}",
                f"member '{d.name}' holds {why}: UI keeps no game state, read CityStore.snapshot/ToolController/GameFlow")
