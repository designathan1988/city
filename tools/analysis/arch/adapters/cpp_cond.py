"""Conditions of C and C++ guards as atoms: the calls, membership tests, comparisons, members, locals and parameters a branch asks about, split
at the top-level `||` or `&&`, each with whether it is negated."""
from .cpp_expr import CALLABLE_DECLS, K, canon, strip

RELATIONS = {"GT", "LT", "GE", "LE", "EQ", "NE", ">", "<", ">=", "<=", "==", "!="}


class Conditions:
    """Mixed into the body walker, beside the expressions."""

    def atoms(self, cond):
        c = strip(cond)
        op = "one"
        if c is not None and c.kind == K.BINARY_OPERATOR and self.op_of(c) in ("LOr", "LAnd", "||", "&&"):
            op = "or" if self.op_of(c) in ("LOr", "||") else "and"
        parts, todo = [], [c]
        while todo:
            x = strip(todo.pop(0))
            if op != "one" and x is not None and x.kind == K.BINARY_OPERATOR and self.op_of(x) in (("LOr", "||") if op == "or" else ("LAnd", "&&")):
                todo[:0] = list(x.get_children())
            else:
                parts.append(x)
        out = []
        for p in parts:
            neg = False
            while p is not None and p.kind == K.UNARY_OPERATOR and self.unary(p) == "!":
                neg = not neg
                p = strip(next(iter(p.get_children()), None))
            out.append([*self.atom(p), neg])
        return op, out

    def atom(self, c):
        """What one condition asks: a call, a member's membership test, a comparison, a member, a local or a parameter."""
        if c is None:
            return ["?"]
        if c.kind == K.CALL_EXPR:
            ref = canon(c.referenced) if c.referenced is not None else None
            kids, args = list(c.get_children()), list(c.get_arguments())
            if ref is not None and ref.spelling == "operator()" and kids:
                obj = strip(kids[0])
                target = obj.referenced if obj is not None else None
                if target is not None and target.get_usr() in self.lambdas:
                    return ["lcall", self.lambdas[target.get_usr()], [self.arg(a) for a in args[1:]]]
            if ref is not None and ref.kind in CALLABLE_DECLS:
                obj = next(iter(kids[0].get_children()), None) if kids and kids[0].kind == K.MEMBER_REF_EXPR else None
                field = self.field_of(obj) if obj is not None else None
                if field and self.u.of_the_language(ref):
                    return ["c", ref.spelling, field, self.arg(args[0]) if args else None]
                return ["call", self.u.target(ref) if not self.u.of_the_language(ref) else "lang:" + ref.spelling, [self.arg(a) for a in args],
                        self.arg(obj) if obj is not None else None]
            return ["?"]
        if c.kind == K.BINARY_OPERATOR and self.op_of(c) in RELATIONS:
            l, r = list(c.get_children())
            return ["cmp", self.op_of(c), self.atom(strip(l)), self.atom(strip(r)), self.tree(l), self.tree(r)]
        if c.kind in (K.DECL_REF_EXPR, K.MEMBER_REF_EXPR) and c.referenced is not None:
            ref = c.referenced
            if self.is_state(ref):
                return ["f", self.u.target(ref)]
            if ref.kind == K.PARM_DECL and ref.get_usr() in self.params:
                return ["p", self.params[ref.get_usr()][1]]
            return ["v", ref.spelling]
        if c.kind == K.INTEGER_LITERAL or c.kind == K.CXX_BOOL_LITERAL_EXPR:
            return ["k", self.tree(c)]
        return ["?"]
