"""Godot's shading language: functions, uniforms and varyings as units, includes followed, reads and writes between stages, and costs."""
import re

from ..model import GRAMMAR, Facts
from ..shape import shape

EXT = (".gdshader", ".gdshaderinc")
DECL = re.compile(r"^\s*(global\s+|instance\s+)?uniform\s+(?:\w+\s+)?([\w\[\]]+)\s+(\w+)[^;]*;", re.M)
CONSTANT = re.compile(r"^const\s+(?:\w+\s+)?[\w\[\]]+\s+(\w+)\s*=", re.M)
VARYING = re.compile(r"^\s*varying\s+(?:flat\s+|smooth\s+)?(?:\w+\s+)?([\w\[\]]+)\s+(\w+)", re.M)
FUNCTION = re.compile(r"^[ \t]*(?:[\w\[\]]+[ \t]+)+(\w+)[ \t]*\([^;{}]*\)\s*\{", re.M)
INCLUDE = re.compile(r'^\s*#include\s+"([^"]+)"', re.M)
CALL = re.compile(r"\b([A-Za-z_]\w*)\s*\(")
WORD = re.compile(r"\b[A-Za-z_]\w*\b")
LOOP = re.compile(r"\b(for|while)\s*\(")
BRANCHES = re.compile(r"^\s*#(if|ifdef|ifndef|elif)\b(.*)$", re.M)
COMMENT = re.compile(r"(?<!:)//[^\n]*|/\*.*?\*/", re.S)
SCREEN = re.compile(r"\bhint_(\w*screen\w*|\w*depth\w*|\w*normal_roughness\w*)\b")
SAMPLE = re.compile(r"\btexture\w*\s*\(")


def _blank(text):
    """Comments out, lines kept."""
    return COMMENT.sub(lambda m: re.sub(r"[^\n]", " ", m.group(0)), text)


def _body_end(text, open_brace):
    depth, i = 0, open_brace
    while i < len(text):
        c = text[i]
        depth += (c == "{") - (c == "}")
        if depth == 0:
            return i
        i += 1
    return len(text) - 1


def _line(text, i):
    return text.count("\n", 0, i) + 1


class Reader:
    def __init__(self, file, text, facts):
        self.file, self.raw, self.text, self.f = file, text, _blank(text), facts
        self.root = "sh:" + file

    def run(self):
        f, file, text = self.f, self.file, self.text
        f.unit(self.root, "file", file.rsplit("/", 1)[-1], file, 1, text.count("\n") + 1, shape=shape(text))
        f.define("file", file, self.root)
        for m in INCLUDE.finditer(self.raw):
            n = _line(self.raw, m.start(1))
            path = m.group(1).replace("res://", "")
            f.ref("file", path, self.root, file, n, "include")
            f.site(file, n, 1, "include")
        for m in BRANCHES.finditer(text):
            if not re.search(rf"#define\s+{re.escape(m.group(2).strip())}\s*$", text, re.M):  # a guard against a second include chooses nothing
                f.cost(self.root, "variant", file, _line(text, m.start()), 0, condition=m.group(2).strip())
        for m in CONSTANT.finditer(text):
            f.unit(f"{self.root}.{m.group(1)}", "global", m.group(1), file, _line(text, m.start()), _line(text, m.start()), self.root, constant=True)
        uniforms = {}
        for m in DECL.finditer(text):
            name = m.group(3)
            uid = f"{self.root}.{name}"
            n = _line(text, m.start())
            scope = (m.group(1) or "").strip()
            f.unit(uid, "uniform", name, file, n, n, self.root, type=m.group(2), scope=scope)
            f.define("uniform", name, uid)
            uniforms[name] = uid
            if SCREEN.search(m.group(0)):
                f.cost(uid, "screen_read", file, n, 0, hint=SCREEN.search(m.group(0)).group(1))
            if scope == "instance":
                f.cost(uid, "instance_uniform", file, n, 0)
        varyings = {}
        for m in VARYING.finditer(text):
            uid = f"{self.root}~{m.group(2)}"
            n = _line(text, m.start())
            f.unit(uid, "varying", m.group(2), file, n, n, self.root, type=m.group(1))
            varyings[m.group(2)] = uid
        spans = []
        for m in FUNCTION.finditer(text):
            name = m.group(1)
            if name in ("if", "for", "while", "switch"):
                continue
            end = _body_end(text, m.end() - 1)
            spans.append((name, m.start(), end))
        for name, start, end in spans:
            self.function(name, start, end, uniforms, varyings)

    def function(self, name, start, end, uniforms, varyings):
        f, file, text = self.f, self.file, self.text
        uid = f"{self.root}#{name}"
        line, last = _line(text, start), _line(text, end)
        body = text[start:end]
        f.unit(uid, "function", name, file, line, last, self.root, shape=shape(body), statements=body.count(";"))
        f.define("shaderfn", name, uid)
        loops = len(LOOP.findall(body))
        samples = len(SAMPLE.findall(body))
        if samples:
            f.cost(uid, "samples", file, line, 0, count=samples, in_loop=loops > 0)
        if "discard" in body:
            f.cost(uid, "discard", file, line, 0)
        for m in LOOP.finditer(body):
            f.cost(uid, "loop", file, line + body.count("\n", 0, m.start()), 1, over="", early="break" in body, forever=False)
        for m in CALL.finditer(body):
            n = line + body.count("\n", 0, m.start())
            if m.group(1) != name:
                f.edge(uid, "call", "name:shaderfn:" + m.group(1), file, n, GRAMMAR, depth=0, locks=())
        words = set(WORD.findall(body))
        for w in sorted(words & set(uniforms)):
            f.edge(uid, "read", uniforms[w], file, line, GRAMMAR, depth=0, locks=())
        for w in sorted(words & set(varyings)):
            kind = "write" if re.search(rf"\b{w}\s*(\.\w+\s*)?=[^=]", body) else "read"
            f.edge(uid, kind, varyings[w], file, line, GRAMMAR, depth=0, locks=())


def scan(repo, tracked, scope):
    facts = Facts()
    for file in sorted(scope):
        if file.endswith(EXT):
            text = repo.text(tracked[file])
            if text is not None:
                Reader(file, text, facts).run()
    return facts, {}
