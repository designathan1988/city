"""One C or C++ call read by the compiler: a call of a parameter, of a stored callable, of a local lambda, of the language's own library (container
operations, algorithms, threads) or of a function, and the name a call binds to a function."""
from clang import cindex as ci

from .cpp_expr import CALLABLE_DECLS, EMPTY, EXCHANGE, GROW, LINEAR, LOOKUP, MUTATE, PASS_THROUGH, READ, SHRINK, SIZE, SORT, THREADS, K, canon, strip

MUTATING = {"insert", "erase", "clear", "resize", "reserve", "swap"}


def _op(name):
    if name in GROW:
        return "insert"
    if name in SHRINK:
        return "erase"
    if name in EMPTY:
        return "clear"
    if name in LOOKUP:
        return "find"
    if name in SIZE:
        return name if name != "assign" else "resize"
    if name in EXCHANGE:
        return "swap"
    if name == "operator[]":
        return "index"
    return None


def _object(c, kids, args, name):
    """The object a member call runs on, and the arguments that remain."""
    head = kids[0] if kids else None
    if head is not None and head.kind == K.MEMBER_REF_EXPR and head.spelling == name:
        inner = list(head.get_children())
        return (inner[0] if inner else None), args
    if name.startswith("operator") and args:
        return args[0], args[1:]
    return None, args


def call(w, c, stmt, mode=READ):
    ref = c.referenced
    kids = list(c.get_children())
    args = list(c.get_arguments())
    if ref is None:
        head = strip(kids[0]) if kids else None
        if head is not None and head.kind == K.DECL_REF_EXPR and head.referenced is not None and head.referenced.kind == K.PARM_DECL:
            _param_call(w, c, head.referenced, args)
        else:
            obj = next(iter(kids[0].get_children()), None) if kids and kids[0].kind == K.MEMBER_REF_EXPR else None
            name = w.dependent_name(c)
            if name in SIZE and args:
                w.ev("alloc", c, w.sources(args[0]))
            descs = [w.arg(a) for a in args]
            found = [d for d in _candidates(head, len(args)) if not w.u.of_the_language(d)]
            for d in found:
                w.ev("call", c, w.u.target(canon(d)), descs, w.arg(obj) if obj is not None else None, stmt, False)
            if not found:
                # A name a template leaves open whose every listed declaration is the language's own (a library that brings `memcpy` in by
                # several `using` declarations) is a call of the language, whichever of them the instantiation takes.
                listed = _overloads(head)
                mine = bool(listed) and all(w.u.of_the_language(d) for d in listed)
                w.ev("call", c, ("lang:" if mine else "dep:") + name, descs, w.arg(obj) if obj is not None else None, stmt, False)
        for k in kids:
            w.expr(k, READ)
        return
    ctor = ref.kind == K.CONSTRUCTOR
    ref = canon(ref)
    ctor = ctor or ref.kind == K.CONSTRUCTOR
    rk = ref.kind
    if rk == K.PARM_DECL:
        _param_call(w, c, ref, args)
        return _walk(w, kids, READ)
    if rk in (K.FIELD_DECL, K.VAR_DECL):
        return _held_call(w, c, ref, args, kids)
    name = ref.spelling
    if name == "operator()" and kids:
        obj = strip(kids[0])
        target = obj.referenced if obj is not None else None
        if target is not None and target.kind == K.PARM_DECL:
            _param_call(w, c, target, args[1:] if args else args)
            return _walk(w, kids, READ)
        if target is not None and target.kind in (K.FIELD_DECL, K.VAR_DECL):
            return _held_call(w, c, target, args[1:] if args else args, kids)
    if w.u.of_the_language(ref):
        return _language(w, c, ref, kids, args, stmt, mode, ctor)
    obj, rest = _object(c, kids, args, name)
    w.ev("call", c, w.u.target(ref), [w.arg(a) for a in rest], w.arg(obj) if obj is not None else None, stmt, bool(ref.is_virtual_method()))
    _binds(w, rest)
    if obj is not None:
        w.expr(obj, READ if ref.kind != K.CXX_METHOD or ref.is_const_method() else MUTATE)
    params = list(ref.get_arguments()) if rk in CALLABLE_DECLS and rk != K.FUNCTION_TEMPLATE else []
    for i, a in enumerate(rest):
        mutable = i < len(params) and params[i].type.kind.name in ("LVALUEREFERENCE", "POINTER") and not params[i].type.get_pointee().is_const_qualified()
        w.expr(a, MUTATE if mutable else READ)


def _candidates(head, count):
    """The functions a call a template leaves open may run, as the compiler lists them: every overload by that name that takes that many
    arguments."""
    if head is None:
        return []
    if head.kind == K.DECL_REF_EXPR and head.referenced is not None and head.referenced.kind in CALLABLE_DECLS:
        return [head.referenced]
    if head.kind != K.OVERLOADED_DECL_REF:
        return []
    lib = ci.conf.lib
    out = []
    for i in range(lib.clang_getNumOverloadedDecls(head)):
        d = lib.clang_getOverloadedDecl(head, i)
        d._tu = head._tu
        if d.kind not in CALLABLE_DECLS:
            continue
        params = [k for k in d.get_children() if k.kind == K.PARM_DECL]
        if d.kind == K.FUNCTION_TEMPLATE or len(params) >= count:
            out.append(d)
    return out


def _overloads(head):
    """Every declaration the compiler lists for a name not yet chosen, of any kind: the reference sits on the expression or below it."""
    if head is None:
        return []
    ref = head if head.kind == K.OVERLOADED_DECL_REF else next((k for k in head.walk_preorder() if k.kind == K.OVERLOADED_DECL_REF), None)
    if ref is None:
        return []
    lib = ci.conf.lib
    out = []
    for i in range(lib.clang_getNumOverloadedDecls(ref)):
        d = lib.clang_getOverloadedDecl(ref, i)
        d._tu = ref._tu
        out.append(d)
    return out


def _walk(w, kids, mode):
    for k in kids:
        w.expr(k, mode)


def _param_call(w, c, param, args):
    found = w.params.get(param.get_usr())
    if found is not None:
        w.ev("pcall", c, found[0], found[1], [w.arg(a) for a in args])


def _held_call(w, c, ref, args, kids):
    """A callable kept in a member is run here; a local holding a lambda runs that lambda."""
    if ref.kind == K.VAR_DECL and ref.get_usr() in w.lambdas:
        w.ev("vcall", c, w.lambdas[ref.get_usr()], [w.arg(a) for a in args])
    elif w.is_state(ref):
        w.ev("fcall", c, w.u.target(ref))
    else:
        field = w.field_of(kids[0]) if kids else None
        if field:
            w.ev("fcall", c, field)
    _walk(w, kids, READ)


def _language(w, c, ref, kids, args, stmt, mode, ctor):
    name = ref.spelling
    owner = ref.semantic_parent.spelling if ref.semantic_parent is not None else ""
    obj, rest = _object(c, kids, args, name)
    op = _op(name) if obj is not None else None
    if ctor and owner in THREADS or ref.kind == K.FUNCTION_DECL and name in THREADS or op == "insert" and _holds_threads(obj):
        w.ev("thread", c, [w.arg(a) for a in rest])
        return _walk(w, kids, READ)
    if name in PASS_THROUGH:
        return _walk(w, kids, READ)
    field = w.field_of(obj) if obj is not None else None
    if obj is not None and name in SIZE and rest and (name != "assign" or len(rest) == 2):
        w.ev("alloc", c, w.sources(rest[0]))
    if field and op:
        key = w.arg(rest[0]) if rest else None
        w.ev("c", c, op, field, key)
        if op in MUTATING or op == "index" and mode != READ:  # an element read by index changes nothing
            for a in rest:
                w.stores(c, a)
    elif obj is None and (name in LINEAR or name in SORT) and args:
        ranged = w.field_of(next(iter(args[0].get_children()), args[0])) if args[0].kind != K.MEMBER_REF_EXPR else None
        ranged = ranged or _begin_field(w, args[0])
        if ranged:
            w.ev("c", c, "linear" if name in LINEAR else "sort", ranged, w.arg(args[2]) if len(args) > 2 else None)
    descs = [w.arg(a) for a in rest]
    if not ctor and (obj is None or field is None and any(d[0] == "l" for d in descs)):  # a constructor or a member container keeps a callable
        w.ev("call", c, "lang:" + name, descs, None, stmt, False)
    if obj is not None:
        w.expr(obj, MUTATE if op in MUTATING or op == "index" and mode != READ else READ)
    for a in rest:
        w.expr(a, READ)


def _holds_threads(obj):
    """A container of the language's threads: adding to it starts one."""
    if obj is None:
        return False
    t = obj.type.get_canonical()
    if t.kind.name in ("LVALUEREFERENCE", "RVALUEREFERENCE", "POINTER"):
        t = t.get_pointee()
    if t.get_num_template_arguments() < 1:
        return False
    element = t.get_template_argument_type(0).get_declaration()
    return element is not None and element.spelling in THREADS


def _begin_field(w, a):
    """`field.begin()` handed to an algorithm: the range it walks is that member."""
    c = strip(a)
    if c is not None and c.kind == K.CALL_EXPR:
        kids = list(c.get_children())
        if kids and kids[0].kind == K.MEMBER_REF_EXPR:
            inner = list(kids[0].get_children())
            return w.field_of(inner[0]) if inner else None
    return None


def _binds(w, args):
    """A text in one argument and a function in another: the call files the function under that name, to be called by it."""
    name, holder = None, None
    for i, a in enumerate(args):
        for k in a.walk_preorder():
            if k.kind == K.STRING_LITERAL:
                text = k.spelling
                name, holder = text[text.find('"') + 1:-1], i
                break
        if name is not None:
            break
    if name is None:
        return
    for i, a in enumerate(args):
        if i == holder:
            continue
        for k in a.walk_preorder():
            if k.kind in (K.DECL_REF_EXPR, K.MEMBER_REF_EXPR) and k.referenced is not None and k.referenced.kind in (K.CXX_METHOD, K.FUNCTION_DECL):
                fn = w.u.target(canon(k.referenced))
                if not fn.startswith("cpp:"):
                    continue
                w.s["binds"].append([name, fn])
                w.u.facts.define("method", name, fn)
                return
