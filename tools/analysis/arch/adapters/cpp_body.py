"""The walk of a C or C++ function body: statements, loops, branches, locks and allocations, with their context. Expressions are `cpp_expr`."""
from clang import cindex as ci

from ..core import words_of
from ..model import AST, NAME
from .cpp_expr import LOOPS, MUTATE, READ, WRITE, Expressions, K, State

CONTAINER_WORDS = {"vector", "map", "set", "list", "deque", "string", "array", "dictionary", "queue"}
GROW_WORDS = {"push", "emplace", "insert", "append", "resize", "reserve", "assign"}
SEARCH_WORDS = {"find", "count", "contains", "search", "has", "index"}


def macro_names(facts, uid, file, line, tokens):
    """A macro that applies a parameter to its arguments (an X-macro) or stringifies: the names it hands out become names of data."""
    words = [t.spelling for t in tokens]
    params = set()
    if len(words) > 2 and words[1] == "(":
        for w in words[2:]:
            if w == ")":
                break
            if w != ",":
                params.add(w)
    for i, w in enumerate(words):
        if w == "#" and i + 1 < len(words):
            facts.define("macro", words[i + 1], uid)
        if w in params and i + 1 < len(words) and words[i + 1] == "(":
            depth, arg = 0, []
            for x in words[i + 1:]:
                depth += (x == "(") - (x == ")")
                if x not in "(),":
                    arg.append(x)
                if depth == 0:
                    break
            for name in arg:
                facts.ref(None, name, uid, file, line, "xmacro")
            if len(arg) > 1:
                facts.ref(None, "_".join(arg), uid, file, line, "xmacro")


class Body(Expressions):
    def stmt(self, c, st):
        """Statements carry control; anything else is an expression."""
        kind = c.kind
        if kind == K.COMPOUND_STMT:
            inner = st.inside()
            for k in c.get_children():
                self.stmt(k, inner)
        elif kind in LOOPS:
            self.loop(c, st)
        elif kind == K.IF_STMT:
            self.branch_if(c, st)
        elif kind == K.SWITCH_STMT:
            self.switch(c, st)
        elif kind == K.DECL_STMT:
            for k in c.get_children():
                if k.kind == K.VAR_DECL:
                    self.local(k, st)
                else:
                    self.stmt(k, st)
        elif kind == K.RETURN_STMT or kind == K.BREAK_STMT:
            st.early[0] = True
            if kind == K.RETURN_STMT:
                self.f.cost(st.unit, "return", self.file, c.location.line, st.depth, ())
            for k in c.get_children():
                self.expr(k, st, READ)
        elif kind == K.CASE_STMT or kind == K.DEFAULT_STMT or kind == K.LABEL_STMT:
            for k in c.get_children():
                self.stmt(k, st)
        else:
            self.expr(c, st, READ)

    def loop(self, c, st):
        kids = list(c.get_children())
        line = c.location.line
        over = self.first_field(kids[0]) if c.kind == K.CXX_FOR_RANGE_STMT and kids else None
        if over is None:
            for k in kids[:-1]:
                over = over or self.first_field(k)
        was, st.early[0] = st.early[0], False
        inner = st.inside(depth=st.depth + 1)
        for k in kids[:-1]:
            self.expr(k, inner, READ)
        if kids:
            self.stmt(kids[-1], inner)
        self.f.cost(st.unit, "loop", self.file, line, st.depth + 1, st.locks, over=over or "", early=st.early[0], forever=len(kids) <= 2 and c.kind != K.CXX_FOR_RANGE_STMT)
        st.early[0] = was

    def sub_unit(self, st, line, end, prefix, kind):
        st.branches[0] += 1
        uid = f"{self.unit}#{prefix}{st.branches[0]}"
        self.f.unit(uid, kind, f"{prefix}{st.branches[0]}", self.file, line, end, st.unit)
        self.edge(st, "branch", uid, line)
        return uid

    def branch_if(self, c, st):
        kids = list(c.get_children())
        if not kids:
            return
        self.expr(kids[0], st, READ)
        fields = [self.o.target(k.referenced) for k in kids[0].walk_preorder() if k.kind == K.MEMBER_REF_EXPR and k.referenced is not None and k.referenced.kind == K.FIELD_DECL]
        for k in kids[1:]:
            uid = self.sub_unit(st, k.location.line, k.extent.end.line, "b", "branch")
            self.f.cost(uid, "guard", self.file, c.location.line, st.depth, (), on=sorted(set(fields)))
            self.stmt(k, st.inside(unit=uid))

    def switch(self, c, st):
        kids = list(c.get_children())
        if len(kids) < 2:
            return
        self.expr(kids[0], st, READ)
        self.f.cost(st.unit, "decision", self.file, c.location.line, st.depth, (), on=[self.first_field(kids[0]) or ""])
        current = st
        for k in kids[-1].get_children():
            if k.kind == K.CASE_STMT or k.kind == K.DEFAULT_STMT:
                uid = self.sub_unit(st, k.location.line, k.extent.end.line, "c", "branch")
                current = st.inside(unit=uid)
                kk = list(k.get_children())
                for label in kk[:-1]:
                    for r in label.walk_preorder():
                        if r.kind == K.DECL_REF_EXPR and r.referenced is not None and r.referenced.kind == K.ENUM_CONSTANT_DECL:
                            self.f.edge(uid, "case", self.o.target(r.referenced), self.file, k.location.line, AST)
                if kk:
                    self.stmt(kk[-1], current)
            else:
                self.stmt(k, current)

    def local(self, c, st):
        line = c.location.line
        type_ = c.type
        decl = type_.get_declaration()
        words = set(words_of(type_.spelling.split("<")[0].split("::")[-1]))
        kids = [k for k in c.get_children() if k.kind not in (K.TYPE_REF, K.TEMPLATE_REF, K.NAMESPACE_REF)]
        name = c.spelling
        if type_.kind == ci.TypeKind.LVALUEREFERENCE and kids:
            field = self.first_field(kids[-1])
            if field:
                self.alias[name] = field
                self.f.site(self.file, line, c.location.column, "alias")
        if "lock" in words or "guard" in words:
            mutex = next((self.first_field(k) for k in kids if self.first_field(k)), "")
            self.f.cost(st.unit, "lock", self.file, line, st.depth, st.locks, mutex=mutex or "", shared="shared" in words)
            self.edge(st, "lock", mutex or "ext:lock", line)
            st.locks = (*st.locks, mutex or "?")
        elif words & CONTAINER_WORDS and st.depth > 0 and type_.kind != ci.TypeKind.LVALUEREFERENCE:
            self.f.cost(st.unit, "alloc", self.file, line, st.depth, st.locks, what=type_.spelling.split("<")[0], name=name)
        if decl is not None and decl.kind in (K.CLASS_DECL, K.STRUCT_DECL) and decl.location.file is not None:
            mine = self.o.rel(decl.location.file.name)
            if mine is not None:
                self.edge(st, "raii", self.o.target(decl), line)
            if words & CONTAINER_WORDS and kids and type_.kind != ci.TypeKind.LVALUEREFERENCE and self.first_field(kids[-1]):
                self.f.cost(st.unit, "copy", self.file, line, st.depth, st.locks, what=self.first_field(kids[-1]))
        for k in kids:
            self.expr(k, st, READ)


__all__ = ["Body", "State", "READ", "WRITE", "MUTATE", "macro_names", "NAME", "GROW_WORDS", "SEARCH_WORDS"]
