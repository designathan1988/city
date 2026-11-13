"""Every import in the tools' Python resolves: to the standard library, an installed package, or a module file on the
sys.path its program has. A file's sys.path is its own folder when it runs as a script (a __main__ block, or nothing
imports it), the folders it inserts into sys.path, the folders the modules it imports insert (sys.path is one list for the
process), and the sys.path of every program that imports it."""
import ast
import os
import site
import sys
from importlib.machinery import PathFinder

from scope import FIXTURES

TOOLS = "tools/"


def applies(ctx, path):
    """Every Python file under tools/, the folders rules.json excludes from the other gates included."""
    return path.startswith(TOOLS) and path.endswith(".py") and not path.startswith(FIXTURES)


def _site_dirs():
    dirs = list(site.getsitepackages()) + [site.getusersitepackages()]
    return [d for d in dirs if os.path.isdir(d)]


def _installed(top, site_dirs):
    return top in sys.builtin_module_names or top in sys.stdlib_module_names or \
        PathFinder.find_spec(top, site_dirs) is not None


def _catches_import(handler):
    t = handler.type
    names = {e.id for e in (t.elts if isinstance(t, ast.Tuple) else [t]) if isinstance(e, ast.Name)}
    return bool(names & {"ImportError", "ModuleNotFoundError"})


def _path_value(node, here, env):
    """Static value of a sys.path entry built from __file__, literals and module-level names with os.path or pathlib
    (`env`: name -> value), else None."""
    if isinstance(node, ast.Constant) and isinstance(node.value, str):
        return os.path.normpath(os.path.join(here, node.value))
    if isinstance(node, ast.Name):
        return os.path.join(here, "__file__") if node.id == "__file__" else env.get(node.id)
    if isinstance(node, ast.Attribute) and node.attr == "parent":
        inner = _path_value(node.value, here, env)
        return os.path.dirname(inner) if inner else None
    if isinstance(node, ast.Subscript) and isinstance(node.value, ast.Attribute) and node.value.attr == "parents":
        inner = _path_value(node.value.value, here, env)
        if inner is None or not isinstance(node.slice, ast.Constant) or not isinstance(node.slice.value, int):
            return None
        for _ in range(node.slice.value + 1):
            inner = os.path.dirname(inner)
        return inner
    if isinstance(node, ast.BinOp) and isinstance(node.op, ast.Div):
        left = _path_value(node.left, here, env)
        right = node.right.value if isinstance(node.right, ast.Constant) else None
        return os.path.normpath(os.path.join(left, right)) if left and isinstance(right, str) else None
    if not isinstance(node, ast.Call):
        return None
    fn = node.func
    name = fn.attr if isinstance(fn, ast.Attribute) else getattr(fn, "id", "")
    if name == "resolve" and isinstance(fn, ast.Attribute):
        return _path_value(fn.value, here, env)
    if not node.args:
        return None
    first = _path_value(node.args[0], here, env)
    if first is None:
        return None
    if name in ("abspath", "realpath", "normpath", "Path", "str", "fspath"):
        return first
    if name == "dirname":
        return os.path.dirname(first)
    if name == "join":
        rest = [a.value for a in node.args[1:] if isinstance(a, ast.Constant) and isinstance(a.value, str)]
        return os.path.normpath(os.path.join(first, *rest)) if len(rest) == len(node.args) - 1 else None
    return None


def _is_path_call(node):
    if not (isinstance(node, ast.Call) and isinstance(node.func, ast.Attribute) and node.func.attr in ("insert", "append")):
        return False
    target = node.func.value
    return isinstance(target, ast.Attribute) and target.attr == "path" and bool(node.args)


class Program:
    """One file's imports [(module, level, line)], sys.path entries, module-level path names and whether it runs as a
    script. Names imported from another tool module are filled in by `bind` once every program is parsed."""

    def __init__(self, root, path, text):
        self.tree = ast.parse(text, filename=path)
        self.path, self.folder = path, os.path.normpath(os.path.join(root, os.path.dirname(path)))
        parents = {c: n for n in ast.walk(self.tree) for c in ast.iter_child_nodes(n)}
        self.imports, self.main, self.borrowed = [], False, {}
        self.path_exprs = [n.args[-1] for n in ast.walk(self.tree) if _is_path_call(n)]
        for node in ast.walk(self.tree):
            if isinstance(node, ast.Import) and not self._optional(node, parents):
                self.imports += [(a.name, 0, node.lineno) for a in node.names]
            elif isinstance(node, ast.ImportFrom) and not self._optional(node, parents):
                if node.level == 0:
                    self.imports.append((node.module, 0, node.lineno))
                    self.borrowed.update({a.asname or a.name: (node.module, a.name) for a in node.names})
                else:
                    for a in node.names:
                        sub = f"{node.module}.{a.name}" if node.module else a.name
                        self.imports.append((node.module or "", node.level, node.lineno, sub))
            elif isinstance(node, ast.Compare) and isinstance(node.left, ast.Name) and node.left.id == "__name__":
                self.main = True
        self.env, self.inserted = {}, set()
        self.bind({})

    def bind(self, imported):
        """Evaluates module-level assignments and the sys.path entries with `imported` (name -> value) known."""
        env = dict(imported)
        for stmt in self.tree.body:
            if isinstance(stmt, ast.Assign) and len(stmt.targets) == 1 and isinstance(stmt.targets[0], ast.Name):
                value = _path_value(stmt.value, self.folder, env)
                if value:
                    env[stmt.targets[0].id] = value
        self.env = env
        self.inserted = {v for v in (_path_value(e, self.folder, env) for e in self.path_exprs) if v}

    @staticmethod
    def _optional(node, parents):
        """Inside `try:` with an ImportError handler: an optional dependency, absent by design."""
        p = parents.get(node)
        while p is not None:
            if isinstance(p, ast.Try) and node in [x for b in p.body for x in ast.walk(b)] and \
                    any(_catches_import(h) for h in p.handlers):
                return True
            p = parents.get(p)
        return False


def _module_file(folder, dotted):
    base = os.path.join(folder, *dotted.split("."))
    for cand in (base + ".py", os.path.join(base, "__init__.py")):
        if os.path.isfile(cand):
            return os.path.normpath(cand)
    return os.path.normpath(base) if os.path.isdir(base) else None


def _relative_base(prog, level):
    base = prog.folder
    for _ in range(level - 1):
        base = os.path.dirname(base)
    return base


def check_imports(ctx, err):
    site_dirs = _site_dirs()
    progs = {}
    for f in ctx.listed:
        if applies(ctx, f):
            try:
                progs[os.path.normpath(os.path.join(ctx.root, f))] = Program(ctx.root, f, ctx.source(f).text)
            except SyntaxError as e:
                err("PYTHON_IMPORT_UNRESOLVED", f"{f}:{e.lineno}", f"does not parse: {e.msg}")
    for _ in range(2):  # names a module takes from another tool module (`from support import TOOLS`)
        for prog in progs.values():
            imported = {}
            for local, (module, name) in prog.borrowed.items():
                source_prog = progs.get(_target(prog, (module, 0, 0), prog.inserted | {prog.folder}))
                if source_prog and name in source_prog.env:
                    imported[local] = source_prog.env[name]
            prog.bind(imported)
    importers = {p: set() for p in progs}
    imported = {p: set() for p in progs}

    def own(prog):
        return prog.inserted | {prog.folder}

    for key, prog in progs.items():  # who imports whom, each importer seen with its own folders
        for imp in prog.imports:
            target = _target(prog, imp, own(prog))
            if target in progs:
                importers[target].add(key)
                imported[key].add(target)
    # sys.path is one list for the whole process: a module's own insertions run when it is imported, so they reach the module
    # that imports it (and that one's importers) as well as everything imported after them
    roots = {k: p.inserted | ({p.folder} if p.main or not importers[k] else set()) for k, p in progs.items()}
    changed = True
    while changed:
        changed = False
        for k in progs:
            merged = roots[k].union(*(roots[g] for g in importers[k]), *(progs[t].inserted for t in imported[k]))
            if merged != roots[k]:
                roots[k], changed = merged, True
    for k, prog in progs.items():
        for imp in prog.imports:
            top = imp[0].split(".")[0]
            if imp[1] == 0 and _installed(top, site_dirs):
                continue
            if _target(prog, imp, roots[k]) is None:
                shown = "." * imp[1] + imp[0]
                err("PYTHON_IMPORT_UNRESOLVED", f"{prog.path}:{imp[2]}",
                    f"'{shown}' is no module on this program's sys.path, the standard library or the installed packages")


def _target(prog, imp, roots):
    """The module file an import names, or None. `from .x import y` accepts y as a module or a name in x."""
    if imp[1]:
        base = _relative_base(prog, imp[1])
        hit = _module_file(base, imp[3]) if len(imp) > 3 else None
        if hit:
            return hit
        return _module_file(base, imp[0]) if imp[0] else (os.path.normpath(base) if os.path.isdir(base) else None)
    for r in roots:
        hit = _module_file(r, imp[0])
        if hit:
            return hit
    return None
