"""The walk of a C or C++ expression: field reads and writes, references, operators, strings, lambdas and calls, with the context they sit in."""
from clang import cindex as ci

from ..model import AST

K = ci.CursorKind
READ, WRITE, MUTATE = 0, 1, 2
LOOPS = {K.FOR_STMT, K.WHILE_STMT, K.DO_STMT, K.CXX_FOR_RANGE_STMT}
ASSIGN = {"Assign"}
COMPOUND_ASSIGN = {"MulAssign", "DivAssign", "RemAssign", "AddAssign", "SubAssign", "ShlAssign", "ShrAssign", "AndAssign", "XorAssign", "OrAssign"}


class State:
    """Where the walk is: the unit edges belong to, how many loops deep, which locks are held."""

    __slots__ = ("unit", "depth", "locks", "early", "lambdas", "branches")

    def __init__(self, unit, depth=0, locks=()):
        self.unit, self.depth, self.locks = unit, depth, locks
        self.early = [False]
        self.lambdas = [0]
        self.branches = [0]

    def inside(self, unit=None, depth=None, locks=None):
        s = State(self.unit if unit is None else unit, self.depth if depth is None else depth, self.locks if locks is None else locks)
        s.early, s.lambdas, s.branches = self.early, self.lambdas, self.branches
        return s


class Expressions:
    def __init__(self, owner, unit, file):
        self.o, self.f, self.unit, self.file = owner, owner.facts, unit, file
        self.alias = {}

    def edge(self, st, kind, dst, line, **attrs):
        self.f.edge(st.unit, kind, dst, self.file, line, AST, **attrs)

    def first_field(self, c):
        for k in c.walk_preorder():
            if k.kind == K.MEMBER_REF_EXPR or k.kind == K.DECL_REF_EXPR:
                ref = k.referenced
                if ref is not None and ref.kind in (K.FIELD_DECL, K.VAR_DECL) and ref.semantic_parent is not None and ref.semantic_parent.kind != K.FUNCTION_DECL:
                    if ref.kind == K.FIELD_DECL or ref.storage_class == ci.StorageClass.STATIC:
                        return self.o.target(ref)
        return None

    def access(self, c, st, mode, ref):
        target = self.o.target(ref)
        line = c.location.line
        self.f.site(self.file, line, c.location.column, "access")
        if mode == READ:
            self.edge(st, "read", target, line, depth=st.depth, locks=st.locks)
        else:
            self.edge(st, "write", target, line, depth=st.depth, locks=st.locks, via="mutate" if mode == MUTATE else "assign")

    def expr(self, c, st, mode):
        kind = c.kind
        if kind == K.CALL_EXPR:
            self.call(c, st)
        elif kind == K.MEMBER_REF_EXPR or kind == K.DECL_REF_EXPR:
            ref = c.referenced
            if ref is not None:
                self.reference(c, st, mode, ref)
            for k in c.get_children():
                self.expr(k, st, MUTATE if mode != READ else READ)
        elif kind == K.BINARY_OPERATOR or kind == K.COMPOUND_ASSIGNMENT_OPERATOR:
            self.binary(c, st)
        elif kind == K.UNARY_OPERATOR:
            self.unary(c, st, mode)
        elif kind == K.LAMBDA_EXPR:
            self.lambda_(c, st, None)
        elif kind == K.CXX_FUNCTIONAL_CAST_EXPR or kind == K.CSTYLE_CAST_EXPR or kind == K.CXX_STATIC_CAST_EXPR:
            self.f.site(self.file, c.extent.start.line, c.extent.start.column, "cast")
            for k in c.get_children():
                self.expr(k, st, mode)
        elif kind == K.CXX_NEW_EXPR:
            self.f.cost(st.unit, "alloc", self.file, c.location.line, st.depth, st.locks, what="new")
            for k in c.get_children():
                self.expr(k, st, READ)
        elif kind == K.STRING_LITERAL:
            self.string(c, st, "")
        elif kind == K.CXX_CONST_CAST_EXPR:
            self.f.cost(st.unit, "const_cast", self.file, c.location.line, st.depth, st.locks)
            self.edge(st, "const_cast", self.first_field(c) or "ext:const_cast", c.location.line)
            for k in c.get_children():
                self.expr(k, st, MUTATE)
        elif kind == K.COMPOUND_STMT or kind in LOOPS or kind == K.IF_STMT or kind == K.SWITCH_STMT or kind == K.DECL_STMT or kind == K.RETURN_STMT:
            self.stmt(c, st)
        else:
            for k in c.get_children():
                self.expr(k, st, mode)

    def reference(self, c, st, mode, ref):
        rk = ref.kind
        if rk == K.FIELD_DECL or (rk == K.VAR_DECL and ref.storage_class == ci.StorageClass.STATIC) or (rk == K.VAR_DECL and ref.semantic_parent is not None and ref.semantic_parent.kind in (K.NAMESPACE, K.TRANSLATION_UNIT)):
            self.access(c, st, mode, ref)
        elif rk == K.VAR_DECL and ref.spelling in self.alias and mode != READ:
            self.edge(st, "write", self.alias[ref.spelling], c.location.line, depth=st.depth, locks=st.locks, via="alias")
        elif rk in (K.FUNCTION_DECL, K.CXX_METHOD, K.FUNCTION_TEMPLATE):
            self.edge(st, "fnref", self.o.target(ref), c.location.line)
            self.f.site(self.file, c.location.line, c.location.column, "fnref")
        elif rk == K.ENUM_CONSTANT_DECL:
            self.edge(st, "enumref", self.o.target(ref), c.location.line)

    def binary(self, c, st):
        kids = list(c.get_children())
        name = c.binary_operator.name if c.kind == K.BINARY_OPERATOR else "AddAssign"
        if len(kids) == 2 and (name in ASSIGN or name in COMPOUND_ASSIGN):
            self.expr(kids[0], st, WRITE)
            self.expr(kids[1], st, READ)
            if kids[1].kind == K.LAMBDA_EXPR or kids[1].kind == K.UNEXPOSED_EXPR:
                field = self.first_field(kids[0])
                fn = next((k for k in kids[1].walk_preorder() if k.kind == K.LAMBDA_EXPR), None)
                if field and fn is not None:
                    self.edge(st, "stores_fn", field, c.location.line)
            return
        for k in kids:
            self.expr(k, st, READ)

    def unary(self, c, st, mode):
        start = c.extent.start.offset
        source = self.o.texts.get(self.file, b"")
        head = source[start:start + 2]
        tail = source[c.extent.end.offset - 2:c.extent.end.offset]
        kids = list(c.get_children())
        if head in (b"++", b"--") or tail in (b"++", b"--"):
            for k in kids:
                self.expr(k, st, WRITE)
            return
        if head[:1] == b"&" and kids:
            ref = kids[0].referenced
            if ref is not None and ref.kind in (K.FIELD_DECL, K.CXX_METHOD):
                self.edge(st, "memberptr", self.o.target(ref), c.location.line)
                self.f.site(self.file, c.location.line, c.location.column, "memberptr")
        for k in kids:
            self.expr(k, st, mode)

    def string(self, c, st, role):
        text = c.spelling
        if len(text) >= 2 and text[0] in "\"'" or text[:2] in ("L\"", "u\"", "U\""):
            value = text[text.find('"') + 1:-1]
            self.f.ref(None, value, st.unit, self.file, c.location.line, role)
            self.f.site(self.file, c.location.line, c.location.column, "string")

    def lambda_(self, c, st, callee):
        st.lambdas[0] += 1
        uid = f"{self.unit}#l{st.lambdas[0]}"
        line = c.location.line
        source = self.o.texts.get(self.file, b"")
        head = source[c.extent.start.offset:c.extent.start.offset + 200].split(b"]")[0].decode("utf8", "replace")
        self.f.unit(uid, "lambda", f"l{st.lambdas[0]}", self.file, line, c.extent.end.line, st.unit, capture=head[1:])
        self.edge(st, "lambda", uid, line, passed_to=callee or "", capture=head[1:], depth=st.depth)
        inner = State(uid, 0, ())
        inner.lambdas, inner.branches = st.lambdas, st.branches
        for k in c.get_children():
            if k.kind == K.COMPOUND_STMT:
                self.stmt(k, inner)
        return uid

    def call(self, c, st):
        from .cpp_calls import call
        call(self, c, st)
