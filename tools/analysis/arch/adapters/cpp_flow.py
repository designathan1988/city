"""The walk of a C or C++ body into its summary: statements, the loops each event sits in with what bounds them, the guards that dominate it, locals
that hold lambdas, locks and threads. Each lambda is a summary of its own, linked to where it was written."""
from clang import cindex as ci

from .cpp_cond import RELATIONS, Conditions
from .cpp_expr import LOCKS, READ, THREADS, Expressions, K, strip

LOOPS = {K.FOR_STMT, K.WHILE_STMT, K.DO_STMT, K.CXX_FOR_RANGE_STMT}
EXITS = {K.RETURN_STMT, K.BREAK_STMT, K.CONTINUE_STMT, K.GOTO_STMT}
SKIP = (K.TYPE_REF, K.TEMPLATE_REF, K.NAMESPACE_REF)


def new_summary(kind, uid, name, file, line, end, cls, parent):
    return {"kind": kind, "id": uid, "name": name, "file": file, "line": line, "end": end, "cls": cls, "parent": parent, "params": [], "ret": "",
            "virtual": False, "over": [], "foreign_over": [], "mangled": "", "ev": [], "ctxs": [], "loops": [], "guards": [], "rejects": [],
            "assigns": [], "inits": [], "returns": [], "binds": []}


class Flow(Conditions, Expressions):
    def __init__(self, u, src, summary, params, lambdas, scalar_return):
        self.u, self.src, self.s = u, src, summary
        self.params, self.lambdas = dict(params), dict(lambdas)
        self.loops, self.doms, self.known, self.aliases = [], [], {}, {}
        self.keep_trees = True
        self.scalar = scalar_return

    def ctx(self):
        key = (tuple(self.loops), tuple(self.doms))
        i = self.known.get(key)
        if i is None:
            i = self.known[key] = len(self.s["ctxs"])
            self.s["ctxs"].append([list(key[0]), list(key[1])])
        return i

    def ev(self, kind, c, *rest):
        self.s["ev"].append([kind, c.location.line, self.ctx(), *rest])

    def body(self, c):
        """A function: its member initialisers in order, then its statements."""
        pending = None
        for k in c.get_children():
            if k.kind == K.MEMBER_REF and k.referenced is not None and k.referenced.kind == K.FIELD_DECL:
                pending = self.u.target(k.referenced)
            elif k.kind == K.COMPOUND_STMT:
                self.stmt(k)
            elif pending is not None and k.kind not in (K.PARM_DECL, *SKIP):
                self.s["inits"].append([pending, self.tree(k)])
                self.ev("w", k, pending)
                self.stores(k, k)
                self.expr(k, READ)
                pending = None

    def lambda_(self, c):
        file = self.s["file"]
        spot = (file, c.extent.start.offset)
        known = self.u.lambda_ids.get(spot)
        if known is not None:
            self.ev("lam", c, known)
            return known
        lid = f"lambda:{file}:{c.location.line}:{c.location.column}"
        self.u.lambda_ids[spot] = lid
        s = new_summary("lambda", lid, "lambda", file, c.location.line, c.extent.end.line, self.s["cls"], self.s["id"])
        params = dict(self.params)
        for i, p in enumerate(k for k in c.get_children() if k.kind == K.PARM_DECL):
            params[p.get_usr()] = (lid, i)
            s["params"].append([p.spelling, p.type.spelling])
        inner = Flow(self.u, self.src, s, params, self.lambdas, True)
        inner.aliases = dict(self.aliases)
        for k in c.get_children():
            if k.kind == K.COMPOUND_STMT:
                inner.stmt(k)
        self.u.summaries.append(s)
        self.ev("lam", c, lid)
        return lid

    def stmt(self, c):
        kind = c.kind
        if kind == K.COMPOUND_STMT:
            self.block(c)
        elif kind in LOOPS:
            self.loop(c)
        elif kind == K.IF_STMT:
            self.branch(c)
        elif kind == K.SWITCH_STMT:
            kids = list(c.get_children())
            for k in kids[:-1]:
                self.expr(k, READ)
            if kids:
                self.stmt(kids[-1])
        elif kind == K.DECL_STMT:
            for k in c.get_children():
                if k.kind == K.VAR_DECL:
                    self.local(k)
        elif kind == K.RETURN_STMT:
            for k in c.get_children():
                if self.scalar:
                    self.s["returns"].append(self.tree(k))
                self.ev("ret", c, self.sources(k), self.atom(strip(k)))
                self.expr(k, READ)
        elif kind in (K.CASE_STMT, K.DEFAULT_STMT, K.LABEL_STMT):
            for k in c.get_children():
                self.stmt(k)
        elif kind in EXITS or kind == K.NULL_STMT:
            return
        else:
            bare = strip(c)
            if bare is not None and bare.kind == K.CALL_EXPR:
                self.call(bare, [c.extent.start.offset, c.extent.end.offset])
            else:
                self.expr(c, READ)

    def block(self, c):
        pushed = 0
        for k in c.get_children():
            gid = self.branch(k) if k.kind == K.IF_STMT else self.stmt(k)
            if gid is not None:
                self.doms.append(-gid)  # what follows a guard that leaves runs only when its condition did not hold
                pushed += 1
        del self.doms[len(self.doms) - pushed:]

    def exits(self, c):
        if c.kind in EXITS or c.kind == K.CXX_THROW_EXPR:
            return True
        return c.kind == K.COMPOUND_STMT and any(k.kind in EXITS or strip(k) is not None and strip(k).kind == K.CXX_THROW_EXPR for k in c.get_children())

    def says_no(self, c):
        """A branch that answers `false` and leaves."""
        rets = [c] if c.kind == K.RETURN_STMT else [k for k in c.get_children() if k.kind == K.RETURN_STMT] if c.kind == K.COMPOUND_STMT else []
        return any(strip(next(iter(r.get_children()), None)) is not None and self.tree(next(iter(r.get_children()))) == ["lit", 0] for r in rets)

    def branch(self, c):
        kids = list(c.get_children())
        if len(kids) < 2:
            return None
        has_else = len(kids) >= 3 and b"else" in self.src[kids[-2].extent.end.offset:kids[-1].extent.start.offset]
        then, other = (kids[-2], kids[-1]) if has_else else (kids[-1], None)
        cond = kids[-3] if has_else else kids[-2]
        for k in kids[:kids.index(cond)]:
            self.stmt(k) if k.kind != K.VAR_DECL else self.local(k)
        gid = len(self.s["guards"]) + 1
        op, atoms = self.atoms(cond)
        self.expr(cond, READ)
        leaves = self.exits(then)
        self.s["guards"].append([gid, c.location.line, op, atoms, leaves])
        if self.says_no(then):
            self.s["rejects"].append(self.tree(cond))
        self.doms.append(gid)
        self.stmt(then)
        self.doms.pop()
        if other is not None:
            self.doms.append(-gid)
            self.stmt(other)
            self.doms.pop()
        return gid if leaves and other is None else None

    def loop(self, c):
        kids = list(c.get_children())
        lid = len(self.s["loops"])
        bound, var = self.bound(c, kids)
        self.s["loops"].append([lid, c.location.line, self.loops[-1] if self.loops else -1, bound, var])
        if bound[0] == "field":
            self.ev("c", c, "iterate", bound[1], None)
        if c.kind == K.CXX_FOR_RANGE_STMT and len(kids) >= 2:
            self.expr(kids[-2], READ)
            self.loops.append(lid)
            for k in kids[:-2]:
                if k.kind == K.VAR_DECL:
                    self.local(k)
        else:
            self.loops.append(lid)
            for k in kids[:-1]:
                self.stmt(k) if k.kind == K.DECL_STMT else self.expr(k, READ)
        if kids:
            self.stmt(kids[-1])
        self.loops.pop()

    def constant(self, c):
        c = strip(c)
        if c is None:
            return False
        if c.kind in (K.INTEGER_LITERAL, K.CXX_BOOL_LITERAL_EXPR):
            return True
        if c.kind == K.BINARY_OPERATOR:
            return all(self.constant(k) for k in c.get_children())
        ref = c.referenced if c.kind == K.DECL_REF_EXPR else None
        return ref is not None and (ref.kind == K.ENUM_CONSTANT_DECL or ref.kind == K.VAR_DECL and ref.type.is_const_qualified() and self.is_state(ref) is False
                                    or ref.kind == K.VAR_DECL and ref.type.is_const_qualified() and ref.semantic_parent is not None and ref.semantic_parent.kind == K.NAMESPACE)

    def side(self, c):
        """What one side of a loop's test measures: a member's size or end, a parameter, a constant, or what it reads."""
        x = strip(c)
        if self.constant(x):
            return ["const"]
        for k in x.walk_preorder() if x is not None else ():
            if k.kind == K.CALL_EXPR and k.spelling in ("size", "end", "cend") and self.u.of_the_language(k.referenced):
                kids = list(k.get_children())
                inner = list(kids[0].get_children()) if kids and kids[0].kind == K.MEMBER_REF_EXPR else []
                field = self.field_of(inner[0]) if inner else None
                if field:
                    return ["field", field]
        a = self.arg(x)
        if a[0] == "p":
            return ["param", a[1]]
        if a[0] == "f":
            return ["field", a[1]]
        if a[0] == "v":
            return ["local", a[1]]
        return ["expr", *a[1:]] if a[0] == "x" else ["expr", [], [], []]

    def bound(self, c, kids):
        if c.kind == K.CXX_FOR_RANGE_STMT and len(kids) >= 2:
            var = next((k.spelling for k in kids if k.kind == K.VAR_DECL), "")
            rng = strip(kids[-2])
            canonical = rng.type.get_canonical() if rng is not None else None
            if canonical is not None and canonical.kind == ci.TypeKind.CONSTANTARRAY:
                return ["const"], var
            return self.side(rng), var
        body_less = kids[:-1]
        var = ""
        for k in body_less:
            if k.kind == K.DECL_STMT:
                var = next((v.spelling for v in k.get_children() if v.kind == K.VAR_DECL), var)
        if c.kind == K.FOR_STMT:
            tests = [strip(k) for k in body_less if strip(k) is not None and (strip(k).kind == K.BINARY_OPERATOR and self.op_of(strip(k)) in RELATIONS
                                                                               or strip(k).kind == K.CALL_EXPR and strip(k).spelling in ("operator!=", "operator<"))]
            if not tests:
                return (["open"] if not body_less else ["expr", [], [], []]), var
            test = tests[0]
            sides = list(test.get_arguments()) if test.kind == K.CALL_EXPR else list(test.get_children())
            found = [self.side(k) for k in sides]
            for kind in ("field", "param", "local", "expr"):
                for f in found:
                    if f[0] == kind and not (kind == "local" and f[1] == var):
                        return f, var
            return ["const"], var
        cond = kids[0] if c.kind == K.WHILE_STMT else kids[-1] if kids else None
        if cond is None or self.constant(cond):
            return ["open"], var
        fields, names, params = self.reads(cond)
        return (["open"] if not names and not params else ["expr", fields, names, params]), var

    def local(self, v):
        ty = v.type
        decl = ty.get_declaration()
        kids = [k for k in v.get_children() if k.kind not in SKIP]
        storage = "thread" if getattr(v, "tls_kind", None) is not None and v.tls_kind.name != "NONE" else "static" if v.storage_class == ci.StorageClass.STATIC else ""
        lang = decl is not None and decl.kind != K.NO_DECL_FOUND and self.u.of_the_language(decl)
        if storage:
            mine = decl is not None and decl.kind != K.NO_DECL_FOUND and not lang and not self.u.foreign(decl)
            self.ev("tl", v, v.spelling, self.u.target(decl) if mine else ty.spelling, storage)
        init = kids[-1] if kids else None
        bare = strip(init) if init is not None else None
        if ty.kind == ci.TypeKind.LVALUEREFERENCE and bare is not None:
            sides = list(bare.get_children())[1:] if bare.kind == K.CONDITIONAL_OPERATOR else [bare]
            held = [f for f in (self.field_of(x) for x in sides) if f]
            if held:
                self.aliases[v.spelling] = held  # a name for a member: writing it writes the member
        if bare is not None and bare.kind == K.LAMBDA_EXPR:
            self.lambdas[v.get_usr()] = self.lambda_(bare)
            return
        if lang and decl.spelling in LOCKS:
            held = next((f for f in (self.field_of(a) for a in (init.walk_preorder() if init is not None else ())) if f), "")
            self.ev("lock", v, held)
        elif lang and decl.spelling in THREADS:
            self.ev("thread", v, [self.arg(a) for a in (bare.get_arguments() if bare is not None and bare.kind == K.CALL_EXPR else ())])
        elif init is not None:
            self.ev("set", v, v.spelling, self.sources(init), self.atom(bare))
        if init is not None:
            self.expr(init, READ)
