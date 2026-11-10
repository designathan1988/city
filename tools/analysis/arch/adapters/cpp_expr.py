"""C and C++ expressions as summary events: member reads and writes, calls by mechanism, container operations, allocations and small value trees."""
import re

from clang import cindex as ci

K = ci.CursorKind
READ, WRITE, MUTATE, PART = 0, 1, 2, 3  # PART: the object is written in one of its parts (`v.cur.x = 1` for `cur`)
WRAPPERS = {K.UNEXPOSED_EXPR, K.PAREN_EXPR, K.CSTYLE_CAST_EXPR, K.CXX_FUNCTIONAL_CAST_EXPR, K.CXX_STATIC_CAST_EXPR, K.CXX_REINTERPRET_CAST_EXPR,
            K.CXX_CONST_CAST_EXPR}
CALLABLE_DECLS = {K.FUNCTION_DECL, K.CXX_METHOD, K.FUNCTION_TEMPLATE, K.CONSTRUCTOR, K.CONVERSION_FUNCTION}
# What the language's own library calls its container operations and algorithms: words of the language, as a keyword is.
GROW = {"push_back", "emplace_back", "push_front", "emplace_front", "push", "emplace", "insert", "emplace_hint", "try_emplace", "insert_or_assign"}
SHRINK = {"erase", "pop_back", "pop_front", "pop", "extract"}
EMPTY = {"clear"}
LOOKUP = {"find", "count", "contains", "at", "lower_bound", "upper_bound", "equal_range"}
SIZE = {"resize", "reserve", "assign"}
EXCHANGE = {"swap"}
LINEAR = {"find", "find_if", "find_if_not", "remove", "remove_if", "count", "count_if", "any_of", "all_of", "none_of", "for_each", "accumulate",
          "search", "unique", "replace", "replace_if", "min_element", "max_element", "adjacent_find", "mismatch", "equal"}
SORT = {"sort", "stable_sort", "partial_sort", "nth_element"}
PASS_THROUGH = {"move", "forward", "ref", "cref", "as_const"}
THREADS = {"thread", "jthread", "async"}
LOCKS = {"lock_guard", "unique_lock", "shared_lock", "scoped_lock"}
IDENT = re.compile(rb"[A-Za-z_]\w*")
MIXING = {"Xor", "Mul", "Shl", "Or", "Add"}


def strip(c):
    """The expression under implicit conversions, parentheses and casts."""
    while c is not None and c.kind in WRAPPERS:
        kids = list(c.get_children())
        if len(kids) != 1:
            return c if not kids else kids[-1]
        c = kids[0]
    return c


def canon(ref):
    """A specialisation stands for the template it came from: one summary per written body."""
    seen = 0
    while ref is not None and seen < 8:
        origin = ref.specialized_template
        if origin is None or origin == ref:
            break
        ref, seen = origin, seen + 1
    return ref


class Expressions:
    """Mixed into the body walker: `u` is the translation unit reader, `s` the summary being filled, `ctx()` the loops and guards in force."""

    def text(self, c):
        return self.src[c.extent.start.offset:c.extent.end.offset]

    def op_of(self, c):
        kind = None
        try:
            kind = c.binary_operator.name
        except (AttributeError, ValueError):
            kind = None
        if kind and kind != "Invalid":
            return kind
        kids = list(c.get_children())
        if len(kids) == 2:
            return self.src[kids[0].extent.end.offset:kids[1].extent.start.offset].strip().decode("latin1") or "?"
        return "?"

    def unary(self, c):
        t = self.text(c)
        for op in (b"++", b"--"):
            if t.startswith(op) or t.endswith(op):
                return op.decode()
        return t[:1].decode("latin1")

    def field_of(self, c):
        """The member an expression names: `a.b[i].c` is `c`; nothing for a local."""
        c = strip(c)
        while c is not None:
            if c.kind == K.MEMBER_REF_EXPR or c.kind == K.DECL_REF_EXPR:
                ref = c.referenced
                if ref is not None and self.is_state(ref):
                    return self.u.target(ref)
                return None
            if c.kind == K.ARRAY_SUBSCRIPT_EXPR or (c.kind == K.CALL_EXPR and c.spelling in ("operator[]", "operator*", "operator->")):
                kids = list(c.get_children())
                c = strip(kids[0]) if kids else None
                continue
            return None
        return None

    def is_state(self, ref):
        if ref.kind == K.FIELD_DECL:
            return not self.u.of_the_language(ref)
        if ref.kind != K.VAR_DECL:
            return False
        parent = ref.semantic_parent
        return ref.storage_class == ci.StorageClass.STATIC or (parent is not None and parent.kind in (K.NAMESPACE, K.TRANSLATION_UNIT, K.STRUCT_DECL, K.CLASS_DECL))

    def arg(self, a):
        """What an argument is: a parameter, a lambda, a member, a local, a text, a function, a number, or what it reads."""
        c = strip(a)
        if c is None:
            return ["x", [], [], []]
        if c.kind == K.CALL_EXPR and c.spelling in PASS_THROUGH and self.u.of_the_language(c.referenced):
            kids = list(c.get_arguments())
            if len(kids) == 1:
                return self.arg(kids[0])
        if c.kind == K.CALL_EXPR and c.referenced is not None and c.referenced.kind == K.CONSTRUCTOR and self.u.of_the_language(c.referenced):
            kids = list(c.get_arguments())
            if len(kids) == 1:
                return self.arg(kids[0])  # a conversion the language makes: the value is what it converts
        if c.kind == K.CALL_EXPR and c.spelling == "operator*":
            kids = [k for k in c.get_children() if strip(k) is not None and strip(k).kind == K.DECL_REF_EXPR and strip(k).spelling != "operator*"]
            if kids:
                return self.arg(kids[0])
        if c.kind == K.LAMBDA_EXPR:
            return ["l", self.lambda_(c)]
        if c.kind == K.STRING_LITERAL:
            t = c.spelling
            return ["s", t[t.find('"') + 1:-1]]
        if c.kind == K.INTEGER_LITERAL:
            return ["i", self.text(c).decode("latin1")]
        if c.kind == K.UNARY_OPERATOR:
            inner = strip(next(iter(c.get_children()), None))
            if inner is not None and inner.referenced is not None and inner.referenced.kind in CALLABLE_DECLS:
                return ["m", self.u.target(canon(inner.referenced))]
        if c.kind in (K.DECL_REF_EXPR, K.MEMBER_REF_EXPR) and c.referenced is not None:
            ref = c.referenced
            if ref.kind == K.PARM_DECL:
                return self.param_ref(ref) or ["x", [], [], []]
            if ref.kind in CALLABLE_DECLS:
                return ["m", self.u.target(canon(ref))]
            if self.is_state(ref):
                return ["f", self.u.target(ref)]
            if ref.kind == K.VAR_DECL:
                lam = self.lambdas.get(ref.get_usr())
                return ["l", lam] if lam else ["v", ref.spelling]
        fields, names, params = self.reads(c)
        return ["x", fields, names, params]

    def param_ref(self, ref):
        found = self.params.get(ref.get_usr())
        if found is None:
            return None
        owner, i = found
        return ["p", i] if owner == self.s["id"] else ["o", owner, i]

    def reads(self, c):
        """Members, locals and parameters an expression reads, and the calls it makes, as a value's sources."""
        fields, names, params = set(), set(), set()
        for k in c.walk_preorder():
            if k.kind in (K.MEMBER_REF_EXPR, K.DECL_REF_EXPR) and k.referenced is not None:
                ref = k.referenced
                if self.is_state(ref):
                    fields.add(self.u.target(ref))
                elif ref.kind == K.PARM_DECL and ref.get_usr() in self.params:
                    params.add(self.params[ref.get_usr()][1])
                elif ref.kind == K.VAR_DECL:
                    names.add(ref.spelling)
            elif k.kind == K.CALL_EXPR and k.referenced is not None and k.referenced.kind in CALLABLE_DECLS:
                names.add("()" + self.u.target(canon(k.referenced)))
        return sorted(fields), sorted(names), sorted(params)

    def sources(self, c):
        if c is None:
            return []
        fields, names, params = self.reads(c)
        return [*(["f", f] for f in fields), *(["call", n[2:]] if n.startswith("()") else ["v", n] for n in names), *(["p", p] for p in params)]

    def tree(self, c):
        """A small value tree: enough to evaluate a condition or an update at chosen values."""
        c = strip(c)
        if c is None:
            return ["?"]
        if c.kind == K.INTEGER_LITERAL:
            t = self.text(c).decode("latin1").rstrip("uUlL")
            try:
                return ["lit", int(t, 0)]
            except ValueError:
                return ["?"]
        if c.kind == K.CXX_BOOL_LITERAL_EXPR:
            return ["lit", 1 if self.text(c) == b"true" else 0]
        if c.kind in (K.MEMBER_REF_EXPR, K.DECL_REF_EXPR) and c.referenced is not None:
            ref = c.referenced
            if ref.kind == K.FIELD_DECL:
                return ["f", self.u.target(ref)]
            if ref.kind == K.PARM_DECL and ref.get_usr() in self.params:
                return ["p", self.params[ref.get_usr()][1]]
            return ["v", ref.spelling]
        kids = list(c.get_children())
        if c.kind == K.BINARY_OPERATOR and len(kids) == 2:
            return ["b", self.op_of(c), self.tree(kids[0]), self.tree(kids[1])]
        if c.kind == K.UNARY_OPERATOR and kids:
            return ["u", self.unary(c), self.tree(kids[0])]
        if c.kind == K.CONDITIONAL_OPERATOR and len(kids) == 3:
            return ["?:", self.tree(kids[0]), self.tree(kids[1]), self.tree(kids[2])]
        return ["?"]

    def expr(self, c, mode=READ, stmt=None):
        kind = c.kind
        if kind == K.CALL_EXPR:
            self.call(c, stmt, mode)
        elif kind == K.MEMBER_REF_EXPR or kind == K.DECL_REF_EXPR:
            ref = c.referenced
            if ref is not None:
                if self.is_state(ref):
                    self.ev("wp" if mode == PART else "w" if mode != READ else "r", c, self.u.target(ref))
                elif ref.kind == K.PARM_DECL:
                    found = self.params.get(ref.get_usr())
                    if found is not None and found[0] != self.s["id"]:
                        self.ev("opr", c, found[0], found[1])
            for k in c.get_children():
                # A part written is its whole written in part: `v.cur.x = 1` writes `x` and, in part, `cur` (its own event, so what is known field
                # by field stays so). Through a pointer the part is another object: the pointer is read.
                through_pointer = k.type.get_canonical().kind == ci.TypeKind.POINTER
                self.expr(k, PART if kind == K.MEMBER_REF_EXPR and mode != READ and not through_pointer else READ)
        elif kind == K.ARRAY_SUBSCRIPT_EXPR:
            kids = list(c.get_children())
            if kids:
                self.expr(kids[0], mode)
            for k in kids[1:]:
                self.expr(k, READ)
        elif kind == K.BINARY_OPERATOR or kind == K.COMPOUND_ASSIGNMENT_OPERATOR:
            self.binary(c, kind)
        elif kind == K.UNARY_OPERATOR:
            kids = list(c.get_children())
            op = self.unary(c)
            if op in ("++", "--") and kids:
                field = self.field_of(kids[0])
                if field and self.keep_trees:
                    self.s["assigns"].append([field, op, ["lit", 1]])
            for k in kids:
                self.expr(k, MUTATE if op in ("++", "--") else mode)
        elif kind == K.LAMBDA_EXPR:
            self.lambda_(c)
        elif kind == K.CXX_NEW_EXPR:
            sizes = [s for k in c.get_children() if k.kind not in (K.TYPE_REF, K.TEMPLATE_REF, K.NAMESPACE_REF) for s in self.sources(k)]
            if "[" in self.text(c).decode("latin1"):
                self.ev("alloc", c, sizes)
            for k in c.get_children():
                self.expr(k, READ)
        elif kind in (K.COMPOUND_STMT, K.IF_STMT, K.FOR_STMT, K.WHILE_STMT, K.DO_STMT, K.CXX_FOR_RANGE_STMT, K.SWITCH_STMT, K.DECL_STMT, K.RETURN_STMT):
            self.stmt(c)
        else:
            for k in c.get_children():
                self.expr(k, mode)

    def binary(self, c, kind):
        kids = list(c.get_children())
        op = self.op_of(c)
        if len(kids) == 2 and (kind == K.COMPOUND_ASSIGNMENT_OPERATOR or op == "Assign" or op == "="):
            field = self.field_of(kids[0])
            self.expr(kids[0], WRITE if op in ("Assign", "=") else MUTATE)
            self.expr(kids[1], READ)
            if field:
                self.stores(c, kids[1])
                if self.keep_trees and op not in ("Assign", "="):
                    self.s["assigns"].append([field, op, self.tree(kids[1])])
            else:
                target = strip(kids[0])
                if target is not None and target.kind == K.DECL_REF_EXPR and target.referenced is not None and target.referenced.kind == K.VAR_DECL:
                    for held in self.aliases.get(target.spelling, ()):
                        self.ev("w", c, held)
                    self.ev("set", c, target.spelling, self.sources(kids[1]), self.atom(strip(kids[1])))
            if op not in ("Assign", "="):
                self.s.setdefault("ops", []).append(op)
            return
        if op in MIXING:
            self.s.setdefault("ops", []).append(op)
        for k in kids:
            self.expr(k, READ)

    def stores(self, at, value):
        """A parameter written into a member is kept beyond the call."""
        _, _, params = self.reads(value)
        for p in params:
            self.ev("store", at, p)
        for k in value.walk_preorder():
            if k.kind == K.DECL_REF_EXPR and k.referenced is not None and k.referenced.kind == K.PARM_DECL:
                found = self.params.get(k.referenced.get_usr())
                if found is not None and found[0] != self.s["id"]:
                    self.ev("ostore", at, found[0], found[1])

    def call(self, c, stmt, mode=READ):
        from .cpp_calls import call
        call(self, c, stmt, mode)

    def dependent_name(self, c):
        """A call a template leaves open: the method's name is the last word written before its arguments."""
        head = next(iter(c.get_children()), None)
        if head is None:
            return ""
        words = IDENT.findall(self.text(head))
        return words[-1].decode() if words else ""
