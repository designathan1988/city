"""TypeScript and JavaScript: the language's own type checker when it can be had, else the grammar alone, with less confidence."""
import json
import os
import shutil
import subprocess
import tempfile

import tree_sitter
import tree_sitter_typescript

from ..core import program_root, text_of
from ..model import NAME, Facts
from ..shape import shape

EXT = (".ts", ".tsx", ".js", ".jsx", ".mjs", ".cjs", ".mts", ".cts")
HELPER = os.path.join(os.path.dirname(os.path.abspath(__file__)), "ts_facts.cjs")
PARSER = tree_sitter.Parser(tree_sitter.Language(tree_sitter_typescript.language_tsx()))
DECLARED = {"function_declaration": "function", "method_definition": "method", "class_declaration": "class", "interface_declaration": "alias",
            "generator_function_declaration": "function", "abstract_class_declaration": "class"}


def _compiler(repo, export):
    """The repository's own compiler if its folder has one, else one kept beside this program's outputs."""
    for base in (repo.root, export):
        candidate = os.path.join(base, "node_modules", "typescript", "lib", "typescript.js")
        if os.path.exists(candidate):
            return candidate
    own = os.path.join(program_root(), "dist", "arch", "external", "node_modules", "typescript", "lib", "typescript.js")
    if not os.path.exists(own) and shutil.which("npm"):
        folder = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(own))))
        os.makedirs(folder, exist_ok=True)
        with open(os.path.join(folder, "package.json"), "w", encoding="utf8") as f:
            f.write("{}")
        subprocess.run((shutil.which("npm"), "install", "typescript@5"), cwd=folder, capture_output=True, check=False)
    return own if os.path.exists(own) else None


def _with_checker(compiler, export, files, tsconfig):
    with tempfile.TemporaryDirectory() as tmp:
        job = os.path.join(tmp, "job.json")
        result = os.path.join(tmp, "out.json")
        with open(job, "w", encoding="utf8") as f:
            json.dump({"typescript": compiler, "root": export.replace("\\", "/"), "files": files, "tsconfig": tsconfig}, f)
        run = subprocess.run((shutil.which("node"), "--max-old-space-size=12288", HELPER, job, result), capture_output=True, check=False)
        if run.returncode != 0 or not os.path.exists(result):
            return None
        with open(result, encoding="utf8") as f:
            return json.load(f)


def _facts_of(raw):
    facts = Facts()
    for u in raw["units"]:
        facts.units.append((*u[:8], u[8]))
    for e in raw["edges"]:
        facts.edges.append(tuple(e))
    for d in raw["defs"]:
        facts.defs.append(tuple(d))
    for r in raw["refs"]:
        facts.refs.append(tuple(r))
    for s in raw["sites"]:
        facts.sites.append(tuple(s))
    for c in raw["costs"]:
        facts.costs.append((c[0], c[1], c[2], c[3], c[4], tuple(c[5]), c[6]))
    return facts


def _grammar_only(repo, tracked, files):
    facts = Facts()
    for file in files:
        data = repo.blob(tracked[file])
        if text_of(data) is None:
            continue
        root = "ts:" + file
        facts.unit(root, "file", file.rsplit("/", 1)[-1], file, 1, data.count(b"\n") + 1)
        facts.define("file", file, root)
        stack = [(PARSER.parse(data).root_node, root)]
        while stack:
            n, owner = stack.pop()
            kind = DECLARED.get(n.type)
            if kind:
                name_node = n.child_by_field_name("name")
                if name_node is not None:
                    name = data[name_node.start_byte:name_node.end_byte].decode("utf8", "replace")
                    uid = f"{owner}#{name}"
                    facts.unit(uid, kind, name, file, n.start_point[0] + 1, n.end_point[0] + 1, owner, shape=shape(data[n.start_byte:n.end_byte].decode("utf8", "replace")))
                    facts.define("method", name, uid)
                    owner = uid
            elif n.type == "call_expression":
                fn = n.child_by_field_name("function")
                if fn is not None:
                    word = data[fn.start_byte:fn.end_byte].decode("utf8", "replace").rsplit(".", 1)[-1]
                    facts.edge(owner, "call", "name:method:" + word, file, n.start_point[0] + 1, NAME, depth=0, locks=())
                    facts.site(file, n.start_point[0] + 1, n.start_point[1] + 1, "call")
            stack.extend((c, owner) for c in n.children)
    return facts


def scan(repo, tracked, scope, export):
    files = sorted(p for p in scope if p.endswith(EXT) and not p.endswith(".d.ts"))
    if not files:
        return Facts(), {"files": 0}
    compiler = _compiler(repo, export) if shutil.which("node") else None
    config = next((os.path.join(export, p).replace("\\", "/") for p in sorted(tracked, key=lambda x: (x.count("/"), x)) if p.rsplit("/", 1)[-1] == "tsconfig.json"), None)
    raw = _with_checker(compiler, export, files, config) if compiler else None
    if raw is None:
        return _grammar_only(repo, tracked, files), {"files": len(files), "mode": "grammar"}
    return _facts_of(raw), {"files": len(files), "mode": "checker"}
