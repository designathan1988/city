"""One C or C++ call read by the compiler: what it runs, by which mechanism, what its arguments mutate and what it hands over."""
from clang import cindex as ci

from ..core import words_of
from .cpp_body import GROW_WORDS, MUTATE, READ, SEARCH_WORDS

K = ci.CursorKind
ByRef = ci.TypeKind.LVALUEREFERENCE
ByPointer = ci.TypeKind.POINTER


def _mutable_param(param):
    t = param.type
    if t.kind == ByRef or t.kind == ByPointer:
        return not t.get_pointee().is_const_qualified()
    return False


def _kind_of(ref):
    rk = ref.kind
    name = ref.spelling
    if rk == K.CONSTRUCTOR:
        return "construct"
    if rk == K.DESTRUCTOR:
        return "destroy"
    if rk == K.CONVERSION_FUNCTION:
        return "convert"
    if name.startswith("operator") and not name[8:9].isalnum() and name[8:9] != "_":
        return "operator"
    if ref.is_virtual_method():
        return "virtual"
    return "call"


def _template_of(ref):
    origin = ref.specialized_template
    return origin if origin is not None and origin.kind == K.FUNCTION_TEMPLATE else None


def _given_name(args):
    """The string in one argument and the function in another: a call that binds one to the other."""
    name, fn, holder = None, None, None
    for i, a in enumerate(args):
        for k in a.walk_preorder():
            if k.kind == K.STRING_LITERAL and name is None:
                text = k.spelling
                name, holder = text[text.find('"') + 1:-1], i
    if name is None:
        return None, None
    for i, a in enumerate(args):
        if i == holder:
            continue
        for k in a.walk_preorder():
            if k.kind in (K.DECL_REF_EXPR, K.MEMBER_REF_EXPR) and k.referenced is not None and k.referenced.kind in (K.CXX_METHOD, K.FUNCTION_DECL):
                fn = fn or k.referenced
    return name, fn


def call(body, c, st):
    ref = c.referenced
    line = c.location.line
    o = body.o
    body.f.site(body.file, line, c.location.column, "call")
    body.f.site(body.file, c.extent.start.line, c.extent.start.column, "call")
    args = list(c.get_arguments())
    arg_set = {a.hash for a in args}
    callee_name = ref.spelling if ref is not None else ""
    callee = o.target(ref) if ref is not None else ""
    if ref is None or ref.kind in (K.FIELD_DECL, K.VAR_DECL, K.PARM_DECL):
        origin = body.first_field(c) if ref is None else (o.target(ref) if ref.kind != K.PARM_DECL else "")
        body.edge(st, "indirect", origin or "ext:indirect", line, depth=st.depth, locks=st.locks)
        body.f.cost(st.unit, "indirect", body.file, line, st.depth, st.locks)
    else:
        kind = _kind_of(ref)
        if callee_name == "operator()" and callee.startswith("ext:"):
            kind = "indirect"
            callee = body.first_field(c) or callee
            body.f.cost(st.unit, "indirect", body.file, line, st.depth, st.locks)
        extra = {}
        template = _template_of(ref)
        if template is not None:
            kind = "template"
            callee = o.target(template)
        if ref.kind == K.CONSTRUCTOR and c.extent.start.offset == (args[0].extent.start.offset if args else -1):
            kind = "convert"
        body.edge(st, kind, callee, line, depth=st.depth, locks=st.locks, **extra)
        if callee.startswith("ext:"):
            body.f.cost(st.unit, "boundary", body.file, line, st.depth, st.locks, api=callee[4:])
        hints(body, st, line, callee, callee_name, c)
    params = list(ref.get_arguments()) if ref is not None and ref.kind in (K.CXX_METHOD, K.FUNCTION_DECL, K.CONSTRUCTOR, K.FUNCTION_TEMPLATE) else []
    for k in c.get_children():
        if k.hash not in arg_set:
            receiver(body, k, st, ref)
    if ref is not None and ref.kind in (K.CXX_METHOD, K.FUNCTION_DECL, K.CONSTRUCTOR) and len(args) >= 2:
        name, fn = _given_name(args)
        if name is not None and fn is not None:
            body.f.define("method", name, o.target(fn))
            body.f.edge(st.unit, "bind", o.target(fn), body.file, line, name=name)
    if ref is not None and ref.kind == K.CXX_METHOD and len(args) == len(params) + 1 and args:
        # An operator written as `a = b`: the first argument is the object.
        body.expr(args[0], st, READ if ref.is_const_method() else MUTATE)
        args = args[1:]
    for i, a in enumerate(args):
        mode = MUTATE if i < len(params) and _mutable_param(params[i]) else READ
        argument(body, a, st, mode, callee, callee_name, i, ref)


def receiver(body, k, st, ref):
    """The object a member call runs on: a non-const method mutates it."""
    mode = READ
    if ref is not None and ref.kind == K.CXX_METHOD and not ref.is_const_method():
        mode = MUTATE
    if k.kind == K.MEMBER_REF_EXPR:
        for inner in k.get_children():
            body.expr(inner, st, mode)
    elif k.kind == K.UNEXPOSED_EXPR or k.kind == K.DECL_REF_EXPR:
        ref2 = k.referenced
        if ref2 is not None and ref2.kind in (K.CXX_METHOD, K.FUNCTION_DECL, K.FUNCTION_TEMPLATE, K.CONVERSION_FUNCTION):
            return
        for inner in k.get_children():
            receiver(body, inner, st, ref)
    else:
        body.expr(k, st, mode)


def argument(body, a, st, mode, callee, callee_name, index, ref):
    line = a.location.line
    found = next((k for k in a.walk_preorder() if k.kind == K.LAMBDA_EXPR), None)
    if found is not None:
        uid = body.lambda_(found, st, callee)
        param = ""
        if ref is not None and ref.kind in (K.CXX_METHOD, K.FUNCTION_DECL, K.CONSTRUCTOR):
            params = list(ref.get_arguments())
            if index < len(params):
                param = params[index].type.spelling
        words = set(words_of(callee_name)) | set(words_of(ref.semantic_parent.spelling if ref is not None and ref.semantic_parent is not None else ""))
        kind = "thread" if "thread" in words else "stored" if "function" in param and "&" not in param else "callback"
        body.f.edge(uid, kind, callee or "ext:unknown", body.file, line, name=callee_name)
        return
    if a.kind == K.STRING_LITERAL or (a.kind == K.UNEXPOSED_EXPR and any(k.kind == K.STRING_LITERAL for k in a.get_children())):
        for k in a.walk_preorder():
            if k.kind == K.STRING_LITERAL:
                body.string(k, st, f"{callee_name}#{index}")
        return
    body.expr(a, st, mode)


def hints(body, st, line, callee, name, c):
    """What a library call does to a cost, told by the words of its own name."""
    if not callee.startswith("ext:"):
        return
    words = set(words_of(name))
    indexed = callee.endswith("operator[]") and "map" in words_of(callee[4:].split(".")[0])
    if words & GROW_WORDS or indexed:
        body.f.cost(st.unit, "grow", body.file, line, st.depth, st.locks, api=callee[4:], field=body.first_field(c) or "")
    if words & {"erase", "clear", "pop", "remove", "shrink"}:
        body.f.cost(st.unit, "shrink", body.file, line, st.depth, st.locks, api=callee[4:], field=body.first_field(c) or "")
    if words & SEARCH_WORDS:
        body.f.cost(st.unit, "search", body.file, line, st.depth, st.locks, api=callee[4:])
    if words & {"sleep", "wait", "notify", "yield"}:
        body.f.cost(st.unit, "wait", body.file, line, st.depth, st.locks, api=callee[4:])
        body.edge(st, "wait" if "notify" not in words else "notify", callee, line)
    if words & {"make"} and words & {"unique", "shared"}:
        body.f.cost(st.unit, "alloc", body.file, line, st.depth, st.locks, what=name)
