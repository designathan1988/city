"""The walk of GDScript code: calls by type or by name, reads and writes, signals, text given to the engine, awaits, loops and branches."""
import re
from ..model import GRAMMAR, NAME

BRANCH_TAIL = re.compile(r"#b\d+$")

READ, WRITE = 0, 1
TEXT = {"string", "string_name", "node_path"}


def node_text(n, src):
    return src[n.start_byte:n.end_byte].decode("utf8", "replace")


def literal(n, src):
    text = node_text(n, src).lstrip("&^").strip()
    for q in ('"""', "'''", '"', "'"):
        if text.startswith(q) and text.endswith(q) and len(text) >= 2 * len(q):
            return text[len(q):-len(q)]
    return text


class State:
    __slots__ = ("unit", "depth", "types", "count")

    def __init__(self, unit, depth=0, types=None, count=None):
        self.unit, self.depth = unit, depth
        self.types = {} if types is None else types
        self.count = [0] if count is None else count

    def inside(self, unit=None, depth=None):
        return State(self.unit if unit is None else unit, self.depth if depth is None else depth, self.types, self.count)


class Body:
    def __init__(self, table, cls, facts, src):
        self.t, self.cls, self.f, self.src, self.file = table, cls, facts, src, cls.file

    def line(self, n):
        return n.start_point[0] + 1

    def sub(self, st, n, prefix, kind="branch"):
        st.count[0] += 1
        uid = f"{BRANCH_TAIL.sub('', st.unit)}#{prefix}{st.count[0]}"
        self.f.unit(uid, kind, f"{prefix}{st.count[0]}", self.file, self.line(n), n.end_point[0] + 1, st.unit)
        self.f.edge(st.unit, "branch", uid, self.file, self.line(n), GRAMMAR)
        return uid

    def walk_stable(self, n):
        return sorted(self.walk(n), key=lambda v: v.start_byte)

    def block(self, body, st):
        """Statements in order; what follows an await runs later, so it is a unit of its own."""
        current = st
        for n in body.named_children:
            self.statement(n, current)
            if any(k.type == "await_expression" for k in self.walk(n)) and n.type not in ("if_statement", "for_statement", "while_statement", "match_statement"):
                uid = self.sub(current, n, "a")
                self.f.edge(current.unit, "await", uid, self.file, self.line(n), GRAMMAR)
                self.f.cost(current.unit, "await", self.file, self.line(n), current.depth)
                current = current.inside(unit=uid)

    def walk(self, n):
        stack = [n]
        while stack:
            x = stack.pop()
            yield x
            if x.type != "lambda":
                stack.extend(x.named_children)

    def statement(self, n, st):
        k = n.type
        if k in ("for_statement", "while_statement"):
            self.loop(n, st)
        elif k == "if_statement":
            self.branch(n, st)
        elif k == "match_statement":
            self.match(n, st)
        elif k == "variable_statement":
            self.local(n, st)
        elif k == "return_statement":
            self.f.cost(st.unit, "return", self.file, self.line(n), st.depth)
            self.expr(n, st)
        else:
            self.expr(n, st)

    def loop(self, n, st):
        over = n.child_by_field_name("right") or n.child_by_field_name("condition")
        from .gd_calls import fixed, resizes
        target = self.field_in(over) if over is not None else ""
        body = n.child_by_field_name("body")
        written = over is not None and fixed(self, n, over)
        if written or (target and n.type == "while_statement" and body is not None and resizes(self, body, target)):
            target = ""
        inner = st.inside(depth=st.depth + (0 if written else 1))
        if over is not None:
            self.expr(over, st if n.type == "for_statement" else inner)  # what a `for` walks is asked for once; a `while` asks each time
        early = any(x.type in ("break_statement", "return_statement") for x in self.walk(n))
        self.f.cost(st.unit, "loop", self.file, self.line(n), max(inner.depth, 1), over=target, early=early, forever=n.type == "while_statement" and node_text(over, self.src) == "true")
        if body is not None:
            self.block(body, inner)

    def field_in(self, n):
        """The first field a condition reads, through `Class.field` too."""
        last = None
        for x in sorted(self.walk(n), key=lambda v: v.start_byte):
            if x.type == "identifier":
                word = node_text(x, self.src)
                cls = self.t.script_of(last) if last else self.cls
                uid, owner = self.t.lookup(cls or self.cls, word, "vars")
                if uid and word not in owner.constants:
                    return uid
                last = word
        return ""

    def branch(self, n, st):
        cond = n.child_by_field_name("condition")
        if cond is not None:
            self.expr(cond, st)
        guard = [self.field_in(cond)] if cond is not None else []
        for part in n.named_children:
            if part.type == "body":
                uid = self.sub(st, part, "b")
                self.f.cost(uid, "guard", self.file, self.line(n), st.depth, on=guard)
                self.block(part, st.inside(unit=uid))
            elif part.type in ("elif_clause", "else_clause"):
                c = part.child_by_field_name("condition")
                if c is not None:
                    self.expr(c, st)
                body = part.child_by_field_name("body")
                if body is not None:
                    uid = self.sub(st, part, "b")
                    self.f.cost(uid, "guard", self.file, self.line(part), st.depth, on=guard)
                    self.block(body, st.inside(unit=uid))

    def match(self, n, st):
        value = n.child_by_field_name("value")
        if value is not None:
            self.expr(value, st)
            self.f.cost(st.unit, "decision", self.file, self.line(n), st.depth, on=[self.field_in(value)])
        body = n.child_by_field_name("body")
        for arm in body.named_children if body is not None else ():
            if arm.type != "pattern_section":
                continue
            uid = self.sub(st, arm, "c")
            inner = st.inside(unit=uid)
            for p in arm.named_children:
                if p.type in TEXT:
                    self.text(p, inner, "case")
                elif p.type == "body":
                    self.block(p, inner)
                elif p.type == "attribute" or p.type == "identifier":
                    self.expr(p, inner)
                    self.f.edge(uid, "case", "name:enum:" + node_text(p, self.src).rsplit(".", 1)[-1], self.file, self.line(p), GRAMMAR)

    def local(self, n, st):
        name = node_text(n.child_by_field_name("name"), self.src)
        t = n.child_by_field_name("type")
        v = n.child_by_field_name("value")
        if t is not None and t.type != "inferred_type":
            st.types[name] = node_text(t, self.src)
        elif v is not None:
            cls = self.value_type(v, st)
            if cls:
                st.types[name] = cls
        if v is not None:
            self.expr(v, st)

    def value_type(self, n, st):
        text = node_text(n, self.src)
        if text.endswith(".new()"):
            return text[:-6]
        if n.type == "attribute":
            cls, member = self.chain(n, st, READ, emit=False)
            return cls.name if cls is not None else ""
        return ""

    def value_of_field(self, n, st, name):
        """A table is data with names in it: each entry of a constant dictionary is a unit of its own."""
        if n.type == "dictionary":
            for pair in n.named_children:
                if pair.type != "pair":
                    continue
                key = pair.child_by_field_name("left") or pair.named_children[0]
                value = pair.child_by_field_name("value") or pair.named_children[-1]
                word = literal(key, self.src)
                uid = f"{st.unit}[{word}]"
                self.f.unit(uid, "entry", word, self.file, self.line(pair), pair.end_point[0] + 1, st.unit)
                self.f.define("key", word, uid)
                self.expr(value, st.inside(unit=uid))
            return
        self.expr(n, st)

    def setget(self, n, st, uid, name):
        self.f.unit(uid, "method", "set " + name, self.file, self.line(n), n.end_point[0] + 1, st.unit, setter=True)
        self.f.edge(st.unit, "branch", uid, self.file, self.line(n), GRAMMAR)
        for part in n.named_children:
            body = part.child_by_field_name("body")
            if body is not None:
                self.block(body, State(uid if part.type == "set_body" else st.unit))

    def expr(self, n, st, mode=READ):
        k = n.type
        if k == "call":
            self.call(n, st)
        elif k == "attribute":
            self.chain(n, st, mode)
        elif k == "assignment" or k == "augmented_assignment":
            self.expr(n.child_by_field_name("left"), st, WRITE)
            self.expr(n.child_by_field_name("right"), st)
        elif k == "identifier":
            self.identifier(n, st, mode)
        elif k in TEXT:
            self.text(n, st, "")
        elif k == "lambda":
            self.lambda_(n, st)
        elif k == "get_node":
            self.f.ref("node", node_text(n, self.src).lstrip("$%"), st.unit, self.file, self.line(n), "get_node")
            self.f.site(self.file, self.line(n), n.start_point[1] + 1, "node")
        elif k == "array" or k == "dictionary":
            if st.depth > 0:
                self.f.cost(st.unit, "alloc", self.file, self.line(n), st.depth, what=k)
            for c in n.named_children:
                self.expr(c, st)
        elif k in ("for_statement", "while_statement", "if_statement", "match_statement", "variable_statement"):
            self.statement(n, st)
        elif k == "subscript" and n.named_children:
            self.subscript(n, st, mode)
        else:
            for c in n.named_children:
                self.expr(c, st, mode if k in ("subscript", "parenthesized_expression") else READ)

    def subscript(self, n, st, mode):
        """`Table[word]`: an enum indexed by a value is a choice of code made at run time."""
        base = n.named_children[0]
        name = node_text(base, self.src).rsplit(".", 1)[-1]
        rest = n.named_children[1:]
        if rest and rest[0].type not in TEXT:
            self.f.edge(st.unit, "enum_by_text", "name:enum_type:" + name, self.file, self.line(n), GRAMMAR)
        for c in n.named_children:
            self.expr(c, st, mode)

    def identifier(self, n, st, mode):
        name = node_text(n, self.src)
        if name in st.types and mode == READ:
            return
        self.member(self.cls, name, st, mode, self.line(n), n.start_point[1] + 1)

    def member(self, cls, name, st, mode, line, column):
        """A field or signal of a known class, read or written; a setter makes the write a call."""
        uid, owner = self.t.lookup(cls, name, "vars")
        if uid:
            self.f.edge(st.unit, "write" if mode else "read", uid, self.file, line, GRAMMAR, depth=st.depth, locks=())
            self.f.site(self.file, line, column, "access")
            setter = owner.setters.get(name)
            if mode and setter:
                self.f.edge(st.unit, "setter", setter, self.file, line, GRAMMAR, depth=st.depth)
            return owner.types.get(name, "")
        uid, _ = self.t.lookup(cls, name, "signals")
        if uid:
            self.f.edge(st.unit, "read", uid, self.file, line, GRAMMAR, depth=st.depth, locks=())
            self.f.site(self.file, line, column, "access")
        return ""

    def text(self, n, st, role):
        space = "role:" + role.split(":")[-1] if ":" in role else "name" if n.type == "string_name" else None
        self.f.ref(space, literal(n, self.src), st.unit, self.file, self.line(n), role)
        self.f.site(self.file, self.line(n), n.start_point[1] + 1, "string")

    def lambda_(self, n, st):
        st.count[0] += 1
        uid = f"{st.unit.split('#')[0]}#f{st.count[0]}"
        self.f.unit(uid, "lambda", f"f{st.count[0]}", self.file, self.line(n), n.end_point[0] + 1, st.unit)
        self.f.edge(st.unit, "lambda", uid, self.file, self.line(n), GRAMMAR)
        body = n.child_by_field_name("body")
        if body is not None:
            self.block(body, State(uid, 0, dict(st.types), st.count))
        return uid

    def call(self, n, st):
        from .gd_calls import call
        call(self, n, st)

    def chain(self, n, st, mode, emit=True):
        from .gd_calls import chain
        return chain(self, n, st, mode, emit)


__all__ = ["Body", "State", "node_text", "literal", "READ", "WRITE", "NAME"]
