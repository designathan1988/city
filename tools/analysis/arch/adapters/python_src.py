"""Python by its own parser: functions, classes, imports, calls, files it opens to write and the processes it starts."""
import ast

from ..model import GRAMMAR, Facts
from ..shape import HASH_STYLE, shape


class Reader(ast.NodeVisitor):
    def __init__(self, file, text, facts, modules):
        self.file, self.text, self.f, self.modules = file, text, facts, modules
        self.lines = text.split("\n")
        self.stack = ["py:" + file]
        self.depth = 0
        self.imports = {}

    def unit_of(self, node, name, kind):
        uid = f"{self.stack[-1]}.{name}"
        body = "\n".join(self.lines[node.lineno - 1:node.end_lineno])
        self.f.unit(uid, kind, name, self.file, node.lineno, node.end_lineno, self.stack[-1], shape=shape(body, HASH_STYLE), statements=node.end_lineno - node.lineno)
        self.f.define("method", name, uid)
        return uid

    def visit_FunctionDef(self, node):
        uid = self.unit_of(node, node.name, "function")
        self.stack.append(uid)
        was, self.depth = self.depth, 0
        self.generic_visit(node)
        self.depth = was
        self.stack.pop()

    visit_AsyncFunctionDef = visit_FunctionDef

    def visit_ClassDef(self, node):
        uid = self.unit_of(node, node.name, "class")
        self.stack.append(uid)
        self.generic_visit(node)
        self.stack.pop()

    def visit_Import(self, node):
        for a in node.names:
            self.imports[a.asname or a.name.split(".")[0]] = a.name
            self.f.edge(self.stack[-1], "import", self.module(a.name), self.file, node.lineno, GRAMMAR)
            self.f.site(self.file, node.lineno, 1, "import")

    def visit_ImportFrom(self, node):
        base = self.resolve_from(node)
        for a in node.names:
            self.imports[a.asname or a.name] = f"{base}.{a.name}"
        self.f.edge(self.stack[-1], "import", self.module(base), self.file, node.lineno, GRAMMAR)
        self.f.site(self.file, node.lineno, 1, "import")

    def resolve_from(self, node):
        if not node.level:
            return node.module or ""
        parts = self.file.split("/")[:-node.level]
        return ".".join([*parts, node.module] if node.module else parts)

    def module(self, name):
        found = self.modules.get(name) or next((p for m, p in self.modules.items() if m.endswith("." + name)), None)
        return "py:" + found if found else "ext:" + name.split(".")[0]

    def visit_For(self, node):
        self.loop(node)

    visit_While = visit_For

    def loop(self, node):
        self.f.cost(self.stack[-1], "loop", self.file, node.lineno, self.depth + 1, over="", early=False, forever=False)
        self.depth += 1
        self.generic_visit(node)
        self.depth -= 1

    def visit_Call(self, node):
        name = self.label_of(node.func)
        line = node.lineno
        self.f.site(self.file, line, node.col_offset + 1, "call")
        if name:
            head, _, last = name.rpartition(".")
            origin = self.imports.get(head.split(".")[0], "") if head else self.imports.get(last, "")
            short = last
            if origin and self.module(origin.rsplit(".", 1)[0] if not head else origin).startswith("ext:"):
                self.f.edge(self.stack[-1], "call", f"ext:{origin.split('.')[0]}.{short}", self.file, line, GRAMMAR, depth=self.depth, locks=())
                if origin.split(".")[0] == "subprocess":
                    self.process(node)
            else:
                self.f.edge(self.stack[-1], "call", "name:method:" + short, self.file, line, GRAMMAR, depth=self.depth, locks=())
            if short == "open" and len(node.args) > 1 and isinstance(node.args[1], ast.Constant) and "w" in str(node.args[1].value):
                self.f.cost(self.stack[-1], "writes_file", self.file, line, self.depth, path=self.constant(node.args[0]))
        self.generic_visit(node)

    def constant(self, node):
        if isinstance(node, ast.Constant):
            return str(node.value)
        if isinstance(node, ast.Name):
            return "$" + node.id
        return ""

    def process(self, node):
        first = node.args[0] if node.args else None
        if isinstance(first, (ast.List, ast.Tuple)) and first.elts:
            first = first.elts[0]
        word = self.constant(first) if first is not None else ""
        self.f.edge(self.stack[-1], "process", "ext:process." + word.strip("$"), self.file, node.lineno, GRAMMAR)

    def label_of(self, node):
        if isinstance(node, ast.Name):
            return node.id
        if isinstance(node, ast.Attribute):
            head = self.label_of(node.value)
            return f"{head}.{node.attr}" if head else node.attr
        return ""

    def visit_Constant(self, node):
        if isinstance(node.value, str) and node.value and "\n" not in node.value and " " not in node.value:
            self.f.ref(None, node.value, self.stack[-1], self.file, node.lineno, "python")
            self.f.site(self.file, node.lineno, node.col_offset + 1, "string")


def scan(repo, tracked, scope):
    files = sorted(p for p in scope if p.endswith(".py"))
    modules = {p[:-3].replace("/", "."): p for p in files}
    facts, broken = Facts(), []
    for file in files:
        text = repo.text(tracked[file])
        if text is None:
            continue
        root = "py:" + file
        facts.unit(root, "file", file.rsplit("/", 1)[-1], file, 1, text.count("\n") + 1, shape=shape(text, HASH_STYLE))
        facts.define("file", file, root)
        try:
            tree = ast.parse(text)
        except (SyntaxError, ValueError):
            broken.append(file)
            continue
        Reader(file, text, facts, modules).visit(tree)
    return facts, {"broken": broken}
