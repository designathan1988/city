"""C++ bodies: each statement read by its rule (log/trace-rules.md, C++ rows), with the branch conditions that guard it.

Locals record the trees of the values they hold (FLOW): a declared type as a value of that type; `auto` from its
initializer; `T x(a, b);` declares an object (direct-initialization) when a or b is a value, else a function
(`cpp_ambig`); `auto [a, b] = v` binds v's parts; a range-for variable holds the range's elements; each assignment adds
its value. Every path of `if`, `switch`, loops and `try` is followed (BRANCH); a `return` keeps its value's tree (JUMP).
"""
from __future__ import annotations

from . import c_family, cpp_ambig, cpp_expr, cpp_implicit, scopes
from .cpp_names import type_of, unwrap
from .gd_body import squash
from .model import Unit, add_ref, declare, declare_function, enclose
from .source import field_of, named_children


def add(r, unit: Unit, kind: str, name: str, node, cond, **extra) -> None:
    add_ref(r, unit, kind, name, r.src.line(node), cond, **extra)


def block(r, node, unit, cond) -> None:
    scopes.open_scope(r)
    for st in named_children(node):
        statement(r, st, unit, cond)
    scopes.close_scope(r)


def bind_parts(r, node, unit, source: tuple | None) -> None:
    """`auto [a, b] = v`: a and b are v's first and second parts (a pair's or a map entry's)."""
    names = [r.src.text(c) for c in named_children(node) if c.type == "identifier"]
    for i, name in enumerate(names):
        part = ("first", "second")[i] if len(names) == 2 else ""
        declare(r, unit, name, [("member", source, part, False)] if source is not None and part else [])


def local(r, node, unit, cond) -> None:
    """A local declaration: each declarator's name and the value it holds."""
    from .cpp_read import deduced
    typ = field_of(node, "type")
    if typ is not None and typ.type in ("class_specifier", "struct_specifier", "union_specifier"):
        r.local_class(typ, unit)                # a class declared in the function ([class.local])
    type_text = type_of(r.src.text(typ)) if typ is not None else ""
    for i, child in enumerate(node.children):
        if node.field_name_for_child(i) != "declarator":
            continue
        name = unwrap(child)
        value = field_of(child, "value") if child.type == "init_declarator" else None
        if name is not None and name.type == "function_declarator":
            object_or_function(r, node, child, name, type_text, unit, cond)
            continue
        if name is not None and name.type == "structured_binding_declarator":
            bind_parts(r, name, unit, r.ir(value, unit) if value is not None else None)
            expr(r, value, unit, cond)
            continue
        if name is None:
            continue
        text = r.src.text(name)
        if value is not None and value.type == "lambda_expression":
            declare_function(r, unit, text, r.lambda_id(value, unit))
            continue
        if deduced(type_text):
            declare(r, unit, text, [r.ir(value, unit)] if value is not None else [])
        else:
            declare(r, unit, text, [("cast", type_text)])
            cpp_implicit.declared_object(r, unit, child, type_text, node, cond, add)
        expr(r, value, unit, cond)


def object_or_function(r, node, declarator, function, type_text: str, unit, cond) -> None:
    """`T x(a, b);` in a block: a local object direct-initialized with a and b when the declaration can be one, else
    the declaration of a function (`cpp_ambig`); the object's local and uses are recorded, its uses marked."""
    from .cpp_read import deduced
    names = cpp_ambig.deciding_names(r, function)
    name = field_of(function, "declarator")
    if not names or name is None:
        return
    auto = deduced(type_text)
    args = object_reading(r, unit, declarator, function, "" if auto else type_text, node, cond, names)
    declare(r, unit, r.src.text(name), list(args) if auto and len(args) == 1 else [] if auto else [("cast", type_text)])


def object_reading(r, unit, declarator, function, type_text: str, node, cond, names: list) -> tuple:
    """`T x(a, b);` read as an object (`cpp_ambig`): its parentheses read as initializers (what they read and call)
    and T's construction with them, each use marked with the names that decide. Returns the initializers' trees."""
    start = len(unit.refs)
    reading = cpp_ambig.as_arguments(r, function)
    args = ()
    if reading is not None:
        again, nodes = reading
        for arg in nodes:
            expr(again, arg, unit, cond)
        args = tuple(again.ir(arg, unit) for arg in nodes)
    cpp_implicit.declared_object(r, unit, declarator, type_text, node, cond, add, args)
    cpp_ambig.mark(unit, start, names)
    return args


def statement(r, st, unit, cond) -> None:
    src = r.src
    kind = st.type
    if kind == "compound_statement":
        block(r, st, unit, cond)
    elif kind == "declaration":
        local(r, st, unit, cond)
    elif kind in ("class_specifier", "struct_specifier", "union_specifier"):
        r.local_class(st, unit)                 # `struct X { ... };` in a block: a local class ([class.local])
    elif kind == "if_statement":
        c_family.if_statement(r, st, unit, cond, statement, expr)
    elif kind in ("for_statement", "for_range_loop", "while_statement", "do_statement"):
        loop(r, st, unit, cond)
    elif kind == "switch_statement":
        test = field_of(st, "condition")
        expr(r, test, unit, cond)
        body = field_of(st, "body")
        for case in named_children(body) if body is not None else []:
            value = field_of(case, "value")
            label = f"switch {squash(src.text(test))}: " + ("case " + squash(src.text(value)) if value else "default")
            for sub in named_children(case):
                if sub is not value:
                    statement(r, sub, unit, cond + (label,))
    elif kind in ("return_statement", "co_return_statement"):
        values = named_children(st)
        unit.returns.append((src.line(st), squash(src.text(values[0])) if values else "",
                             r.ir(values[0], unit) if values else ("literal", "void")))
        for v in values:
            expr(r, v, unit, cond)
    elif kind == "try_statement":
        for sub in named_children(st):
            if sub.type == "catch_clause":
                params = field_of(sub, "parameters")
                label = "catch " + squash(src.text(params)) if params is not None else "catch"
                statement(r, field_of(sub, "body"), unit, cond + (label,))
            else:
                statement(r, sub, unit, cond)
    else:
        expr(r, st, unit, cond)


def loop(r, st, unit, cond) -> None:
    """A loop: its init declarations and loop variable are local to it (a scope of their own)."""
    scopes.open_scope(r)
    try:
        loop_body(r, st, unit, cond)
    finally:
        scopes.close_scope(r)


def loop_body(r, st, unit, cond) -> None:
    from .cpp_read import deduced
    src = r.src
    body = field_of(st, "body")
    if st.type == "for_range_loop":
        right = field_of(st, "right")
        walked = ("elements", r.ir(right, unit))
        name = unwrap(field_of(st, "declarator"))
        typ = field_of(st, "type")
        declared = type_of(src.text(typ)) if typ is not None else ""
        if name is not None and name.type == "structured_binding_declarator":
            bind_parts(r, name, unit, walked)
        elif name is not None:
            declare(r, unit, src.text(name), [walked] if deduced(declared) else [("cast", declared)])
        expr(r, right, unit, cond)
        cpp_implicit.range_calls(r, unit, right, st, cond, add)
        label = f"for ({squash(src.text(field_of(st, 'declarator')))} : {squash(src.text(right))})"
    else:
        parts = []
        for name in ("initializer", "condition", "update"):
            part = field_of(st, name)
            if part is None:
                continue
            parts.append(src.text(part).rstrip(";").strip())
            if part.type == "declaration":
                local(r, part, unit, cond)
            else:
                expr(r, part, unit, cond)
        word = {"for_statement": "for", "while_statement": "while", "do_statement": "do-while"}[st.type]
        label = f"{word} " + squash("; ".join(parts))
    if body is not None:
        statement(r, body, unit, cond + (label,))


def tail_calls(r, body, unit: Unit) -> None:
    """The calls after which the function only returns: the value of a `return`, and the last statement on each path
    to the end of the body (`return;` after it included). A compiler may make such a call a jump (a sibling call), and
    the function's frame is then gone from the stack while the callee runs."""
    def called(node) -> None:
        while node is not None and node.type == "parenthesized_expression":
            node = (named_children(node) or [None])[0]
        if node is not None and node.type == "call_expression":
            unit.tails.add(r.src.line(node))

    def last(st) -> None:
        if st is None:
            return
        if st.type == "compound_statement":
            items = [c for c in named_children(st) if c.type != "comment"]
            while items and items[-1].type == "return_statement" and not named_children(items[-1]):
                items.pop()
            if items:
                last(items[-1])
        elif st.type == "if_statement":
            last(field_of(st, "consequence"))
            alt = field_of(st, "alternative")
            for sub in named_children(alt) if alt is not None else []:
                last(sub)
        elif st.type == "expression_statement":
            for child in named_children(st):
                called(child)

    def returns(node) -> None:
        for child in named_children(node):
            if child.type == "lambda_expression":
                continue                                    # its returns are its own
            if child.type == "return_statement" and named_children(child):
                called(named_children(child)[0])
            returns(child)

    last(body)
    returns(body)


def lambda_unit(r, node, owner: Unit) -> Unit:
    line = r.src.line(node)
    lam = Unit("lambda", "cpp", f"{owner.name}.<lambda:{line}>", f"{owner.qual}.<lambda:{line}>", owner.owner,
               r.src.display, line, r.src.end_line(node), local_id=len(r.facts.units))
    r.facts.units.append(lam)
    enclose(lam, owner)
    scopes.open_scope(r)                        # its parameters: a scope of its own inside the enclosing blocks
    decl = field_of(node, "declarator")
    if decl is not None:
        r.params(decl, lam)
    body = field_of(node, "body")
    if body is not None:
        block(r, body, lam, ())
        tail_calls(r, body, lam)
    scopes.close_scope(r)
    return lam


def expr(r, node, unit, cond) -> None:
    cpp_expr.expression(r, node, unit, cond)
