"""The GDScript program's graph, in the same rows as cpp_graph.py and ts_graph.js, joined to the C++ graph through the
names the C++ code registers functions under (a call handing a function together with a name). GDScript binds a
call by the object it runs on as it runs (the GDScript reference: duck typing), so a call is bound where the program
says which: a function of the same class or the classes it extends; a class named by `class_name`; an autoload named
in project.godot; a name the C++ code registers. Where more than one function could be meant, every one is kept and
the call is marked `by name` with how many.

  python gd_graph.py PROJECT_DIR OUT.json [--cpp CPP_GRAPH.json]
"""
from __future__ import annotations

import json
import os
import re
import sys
import time
from collections import defaultdict
from pathlib import Path

import tree_sitter_gdscript
from tree_sitter import Language, Parser

PARSER = Parser(Language(tree_sitter_gdscript.language()))
SKIP = {".godot", "addons", "tests", "test"}


def norm(p) -> str:
    return os.path.normcase(os.path.abspath(str(p))).replace("\\", "/")


def txt(src: bytes, n) -> str:
    return " ".join(src[n.start_byte:n.end_byte].decode("utf-8", "replace").split())[:300]


def autoloads(project: Path) -> dict:
    out, inside = {}, False
    for line in (project / "project.godot").read_text(encoding="utf-8", errors="replace").splitlines():
        if line.startswith("["):
            inside = line.strip() == "[autoload]"
            continue
        m = re.match(r'(\w+)="\*?res://(.+?)"', line) if inside else None
        if m:
            out[m.group(1)] = norm(project / m.group(2))
    return out


class Reader:
    def __init__(self, project: Path, registry: dict):
        self.project = project
        self.registry = registry                      # name -> [C++ function ids]
        self.files = [p for p in project.rglob("*.gd") if not (SKIP & {x.lower() for x in p.relative_to(project).parts})]
        self.classes, self.extends, self.funcs, self.vars = {}, {}, defaultdict(dict), defaultdict(set)
        self.var_types = defaultdict(dict)            # file -> variable -> the class its declaration names
        self.by_node = {}                             # (file, start byte) -> function id, inner classes' too
        self.local_types = {}                         # the typed parameters of the function being read
        self.by_name = defaultdict(list)
        self.auto = autoloads(project)
        self.g = {"language": "gdscript", "root": norm(project), "functions": {}, "calls": [], "hands": [], "invokes": [],
                  "forwards": [], "stores": [], "field_calls": [], "fields": [], "loops": [], "texts": [], "numbers": [],
                  "consts": [], "registers": [], "errors": {}}
        self.trees = {}

    def first_pass(self):
        """Every class, what it extends, its functions and its variables."""
        for path in self.files:
            src = path.read_bytes()
            tree = PARSER.parse(src)
            f = norm(path)
            self.trees[f] = (src, tree)
            if tree.root_node.has_error:
                self.g["errors"][f] = ["the grammar could not read part of the file"]
            cls = path.stem
            for n in tree.root_node.children:
                if n.type == "class_name_statement":
                    name = next((c for c in n.children if c.type in ("name", "identifier")), None)
                    if name is not None:
                        cls = txt(src, name)
                if n.type == "extends_statement":
                    self.extends[f] = txt(src, n).replace("extends", "").strip().strip('"')
            self.classes[f] = cls
            self.members(f, src, tree.root_node, cls, top=True)
        self.class_file = {c: f for f, c in self.classes.items()}

    def members(self, f, src, body, cls: str, top: bool):
        """The functions and variables of a class body; an inner class's own, under its name (GDScript reference:
        inner classes)."""
        for n in body.children:
            if n.type == "function_definition":
                name = txt(src, n.child_by_field_name("name"))
                i = f"{f}:{n.start_point[0] + 1}:{n.start_point[1] + 1}"
                self.by_node[(f, n.start_byte)] = i
                if top:
                    self.funcs[f][name] = i
                self.by_name[name].append(i)
                self.g["functions"][i] = {"name": f"{cls}.{name}", "file": f, "line": n.start_point[0] + 1,
                                          "end": n.end_point[0] + 1, "kind": "function", "owner": None}
            elif n.type in ("variable_statement", "const_statement", "signal_statement"):
                name = n.child_by_field_name("name")
                if name is not None and top:
                    self.vars[f].add(txt(src, name))
                    kind = n.child_by_field_name("type")
                    value = n.child_by_field_name("value")
                    declared = txt(src, kind) if kind is not None and kind.type == "type" else ""
                    if not declared and value is not None and txt(src, value).endswith(".new()"):
                        declared = txt(src, value)[:-len(".new()")]
                    if declared:
                        self.var_types[f][txt(src, name)] = declared
            elif n.type == "class_definition":
                name = n.child_by_field_name("name")
                inner = n.child_by_field_name("body")
                if inner is not None:
                    self.members(f, src, inner, f"{cls}.{txt(src, name) if name is not None else '?'}", top=False)

    def own(self, f: str, name: str):
        """A function of a class or of the classes it extends."""
        seen = 0
        while f and seen < 10:
            if name in self.funcs.get(f, {}):
                return self.funcs[f][name]
            base = self.extends.get(f, "")
            f = self.class_file.get(base) or (norm(self.project / base.replace("res://", "")) if base.endswith(".gd") else None)
            seen += 1
        return None

    def targets(self, f: str, receiver: str, name: str) -> tuple:
        """The functions a call of `name` on `receiver` may run, and how they were found."""
        if not receiver or receiver in ("self",):
            t = self.own(f, name)
            if t:
                return [t], "exact"
            return [], "out"
        if receiver == "super":                       # the base class's function of that name
            base = self.extends.get(f, "")
            bf = self.class_file.get(base) or (norm(self.project / base.replace("res://", "")) if base.endswith(".gd") else None)
            t = self.own(bf, name) if bf else None
            return ([t], "exact") if t else ([], "out")
        head = receiver.split(".")[0]
        declared = self.local_types.get(head) or self.var_types.get(f, {}).get(head)
        if declared and len(receiver.split(".")) == 1 and declared in self.class_file:
            t = self.own(self.class_file[declared], name)         # a variable whose declaration names its class
            if t:
                return [t], "exact"
        if head in self.auto and len(receiver.split(".")) == 1:
            t = self.own(self.auto[head], name)
            if t:
                return [t], "exact"
        if head in self.class_file and len(receiver.split(".")) == 1:
            t = self.own(self.class_file[head], name)
            if t:
                return [t], "exact"
        cands = list(self.by_name.get(name, ())) + list(self.registry.get(name, ()))
        if 0 < len(cands) <= 6:
            return cands, f"by name ({len(cands)})"
        return [], "out"

    def walk(self, f, src, n, fn, guards, depth):
        t = n.type
        line = n.start_point[0] + 1
        if t == "lambda":
            i = f"{f}:{line}:{n.start_point[1] + 1}"
            owner = self.g["functions"].get(fn, {}).get("name", "?")
            self.g["functions"][i] = {"name": f"{owner}.<lambda:{line}>", "file": f, "line": line,
                                      "end": n.end_point[0] + 1, "kind": "lambda", "owner": fn}
            for c in n.children:
                self.walk(f, src, c, i, [], 0)
            return
        if t in ("if_statement", "elif_clause"):
            cond = n.child_by_field_name("condition")
            text = txt(src, cond) if cond is not None else "?"
            body = n.child_by_field_name("body")
            for c in n.children:
                if c is body:
                    self.walk(f, src, c, fn, guards + [f"{text} @{line}"], depth)
                elif c.type in ("elif_clause", "else_clause"):
                    self.walk(f, src, c, fn, guards + [f"!({text}) @{line}"], depth)
                else:
                    self.walk(f, src, c, fn, guards, depth)
            return
        if t == "conditional_expression":
            cond = n.child_by_field_name("condition")
            text = txt(src, cond) if cond is not None else "?"
            for fld, g in (("left", f"{text} @{line}"), ("right", f"!({text}) @{line}")):
                c = n.child_by_field_name(fld)
                if c is not None:
                    self.walk(f, src, c, fn, guards + [g], depth)
            if cond is not None:
                self.walk(f, src, cond, fn, guards, depth)
            return
        if t in ("for_statement", "while_statement"):
            head = txt(src, n).split(":")[0][:160]
            self.g["loops"].append({"in": fn, "line": line, "end": n.end_point[0] + 1, "depth": depth + 1, "cond": head})
            for c in n.children:
                self.walk(f, src, c, fn, guards + [f"loop {head} @{line}"], depth + 1)
            return
        if t == "pattern_section":
            label = txt(src, n).split(":")[0][:80]
            for c in n.children:
                self.walk(f, src, c, fn, guards + [f"case {label} @{line}"], depth)
            return
        if t in ("call", "attribute_call") and fn is not None:
            name_node = next((c for c in n.children if c.type == "identifier"), None)
            name = txt(src, name_node) if name_node is not None else ""
            receiver = ""
            if t == "attribute_call" and n.parent is not None and n.parent.type == "attribute":
                parts = [txt(src, c) for c in n.parent.children if c.type in ("identifier", "attribute_call") and c.start_byte < n.start_byte]
                receiver = ".".join(p.split("(")[0] for p in parts)
            args = n.child_by_field_name("arguments") or next((c for c in n.children if c.type == "arguments"), None)
            arg_nodes = [c for c in (args.named_children if args is not None else [])]
            if name in ("connect",) and receiver:
                signal = receiver.split(".")[-1]
                for a in arg_nodes:
                    for h in self.handed(f, src, a):
                        self.g["stores"].append({"in": fn, "fn": h, "field": f"signal:{signal}", "line": line})
            elif name == "emit" and receiver:
                self.g["field_calls"].append({"in": fn, "field": f"signal:{receiver.split('.')[-1]}", "line": line, "guards": guards})
            elif name == "emit_signal" and arg_nodes and arg_nodes[0].type in ("string", "string_name"):
                sig = txt(src, arg_nodes[0]).lstrip("&").strip('"')
                self.g["field_calls"].append({"in": fn, "field": f"signal:{sig}", "line": line, "guards": guards})
            else:
                tos, how = self.targets(f, receiver, name)
                if tos:
                    for to in tos:
                        self.g["calls"].append({"from": fn, "to": to, "name": None, "line": line, "guards": guards,
                                                "out": False, "how": how})
                else:
                    self.g["calls"].append({"from": fn, "to": f"out:{(receiver + '.' if receiver else '') + name}",
                                            "name": (receiver + "." if receiver else "") + name, "line": line,
                                            "guards": guards, "out": True})
                for k, a in enumerate(arg_nodes):
                    for h in self.handed(f, src, a):
                        self.g["hands"].append({"from": fn, "fn": h, "to": tos[0] if tos else f"out:{name}", "index": k,
                                                "line": line, "guards": guards})
                    # a text that names a function: one of this class (call_deferred, Callable(self, "x")), or one the
                    # C++ code registers — that function, by its name
                    if a.type in ("string", "string_name"):
                        s = txt(src, a).lstrip("&").strip('"')
                        own = self.own(f, s)
                        for to in ([own] if own else []) + list(self.registry.get(s, ())):
                            self.g["calls"].append({"from": fn, "to": to, "name": None, "line": line, "guards": guards,
                                                    "out": False, "how": "by text"})
        if t == "assignment" and fn is not None:
            left = n.child_by_field_name("left")
            if left is not None:
                name = txt(src, left).split(".")[-1].split("[")[0]
                if name in self.vars.get(f, ()) or txt(src, left).startswith("self."):
                    self.g["fields"].append({"in": fn, "field": f"{self.classes[f]}.{name}", "write": True, "line": line, "guards": guards})
        if t == "identifier" and fn is not None and n.parent is not None and n.parent.type not in ("call", "attribute_call", "function_definition"):
            name = txt(src, n)
            if name in self.vars.get(f, ()):
                self.g["fields"].append({"in": fn, "field": f"{self.classes[f]}.{name}", "write": False, "line": line, "guards": guards})
        if t in ("string", "string_name") and fn is not None:
            self.g["texts"].append({"in": fn, "text": txt(src, n).lstrip("&").strip('"'), "line": line})
        if t in ("integer", "float") and fn is not None:
            self.g["numbers"].append({"in": fn, "value": txt(src, n), "line": line})
        if t == "function_definition":
            fn = self.by_node.get((f, n.start_byte), fn)
            guards, depth = [], 0
            self.local_types = {}
            params = n.child_by_field_name("parameters")
            for p in (params.named_children if params is not None else []):
                if p.type == "typed_parameter":
                    ident = next((c for c in p.children if c.type == "identifier"), None)
                    kind = p.child_by_field_name("type")
                    if ident is not None and kind is not None:
                        self.local_types[txt(src, ident)] = txt(src, kind)
        for c in n.children:
            self.walk(f, src, c, fn, guards, depth)

    def handed(self, f, src, a) -> list:
        """The functions an argument hands: a lambda written there, a function of the class named, `x.bind(...)`."""
        if a.type == "lambda":
            return [f"{f}:{a.start_point[0] + 1}:{a.start_point[1] + 1}"]
        if a.type == "identifier":
            t = self.own(f, txt(src, a))
            return [t] if t else []
        if a.type == "attribute":
            head = a.children[0] if a.children else None
            if head is not None and head.type == "identifier":
                t = self.own(f, txt(src, head))
                return [t] if t else []
        return []

    def run(self):
        self.first_pass()
        for f, (src, tree) in self.trees.items():
            self.walk(f, src, tree.root_node, None, [], 0)
        # a function of a class that one of the classes it extends also has runs where that one is called
        self.g["overrides"] = []
        for f, funcs in self.funcs.items():
            base = self.extends.get(f, "")
            bf = self.class_file.get(base) or (norm(self.project / base.replace("res://", "")) if base.endswith(".gd") else None)
            for name, i in funcs.items():
                b = self.own(bf, name) if bf else None
                if b and b != i:
                    self.g["overrides"].append({"fn": i, "base": b})
        return self.g


def main() -> int:
    project, out = Path(sys.argv[1]), sys.argv[2]
    t0 = time.monotonic()
    registry = defaultdict(list)
    if "--cpp" in sys.argv:
        cpp = json.loads(Path(sys.argv[sys.argv.index("--cpp") + 1]).read_text(encoding="utf-8"))
        for r in cpp.get("registers", ()):
            registry[r["name"]].append(r["fn"])
    g = Reader(project, registry).run()
    Path(out).write_text(json.dumps(g), encoding="utf-8")
    print(f"{len(g['functions'])} functions, {len(g['calls'])} calls "
          f"({sum(1 for c in g['calls'] if c.get('how', '').startswith('by'))} by name or text), "
          f"{len(g['stores'])} handlers connected, {len(g['errors'])} files the grammar could not read whole; "
          f"{time.monotonic() - t0:.1f} s", file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
