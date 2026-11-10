"""C++ bodies: each statement and expression read by its rule, with the branch conditions that guard it.

Locals and parameters are recorded with their declared type, so the resolver can find the class behind `x.f()`.
A member chain keeps its receivers in order (`state.log.road_chunks.push_back` -> receiver state, log, road_chunks).
Godot's registrations are read where they are written: `bind_method(D_METHOD("name"), &C::m)` binds a name the engine
calls, `ADD_SIGNAL(MethodInfo("name"))` declares a signal, `ADD_PROPERTY(PropertyInfo(.., "p"), "set", "get")` binds a
property to its accessors, `emit_signal("name")` fires one.
"""
from __future__ import annotations

from . import cpp_expr
from .cpp_names import strip_templates, template_call_type, type_of, unwrap
from .gd_body import CALLED, squash
from .model import Ref, Unit
from .source import field_of, named_children

OF = "<of>"


def strip(text: str) -> str:
    return strip_templates(text)


def add(r, unit: Unit, kind: str, name: str, node, cond, **extra) -> None:
    unit.refs.append(Ref(kind, name, line=r.src.line(node), cond=cond, **extra))


def block(r, node, unit, cond) -> None:
    for st in named_children(node):
        statement(r, st, unit, cond)


def local(r, node, unit, cond) -> None:
    """A local declaration: each declarator's name and type; a lambda kept by its name."""
    typ = field_of(node, "type")
    type_text = type_of(r.src.text(typ)) if typ is not None else ""
    for i, child in enumerate(node.children):
        if node.field_name_for_child(i) != "declarator":
            continue
        name = unwrap(child)
        value = field_of(child, "value") if child.type == "init_declarator" else None
        if name is not None and name.type == "structured_binding_declarator":
            bind_names(r, name, unit, OF + one_line(r, value) if value is not None else "")
            if value is not None:
                expr(r, value, unit, cond)
            continue
        if name is None:
            continue
        text = r.src.text(name)
        if value is not None and value.type == "lambda_expression":
            unit.lambdas[text] = lambda_unit(r, value, unit).local_id
            unit.types[text] = ""
            continue
        deduced = type_text.replace("const", "").replace("&", "").replace("*", "").strip() in ("auto", "")
        unit.types[text] = infer(r, value) if deduced and value is not None else type_text
        if value is not None:
            expr(r, value, unit, cond)
        elif child.type not in ("identifier",):
            expr(r, child, unit, cond)


def one_line(r, node) -> str:
    return " ".join(r.src.text(node).split())


def bind_names(r, node, unit, source: str) -> None:
    """`auto [a, b] = x`: a and b are x's first and second (a pair's or a map entry's parts)."""
    names = [r.src.text(c) for c in named_children(node) if c.type == "identifier"]
    for i, name in enumerate(names):
        unit.types[name] = f"{source}.{('first', 'second')[i]}" if source and len(names) == 2 else ""


def infer(r, value) -> str:
    """The type an `auto` takes from its initialiser, when the expression names it."""
    if value is None:
        return ""
    if value.type == "call_expression":
        fn = field_of(value, "function")
        named = template_call_type(r.src.text(fn))
        return named if named else CALLED + strip(r.src.text(fn))
    if value.type == "new_expression":
        return type_of(r.src.text(field_of(value, "type")))
    if value.type == "compound_literal_expression":
        return type_of(r.src.text(field_of(value, "type")))
    if value.type in ("identifier", "field_expression", "subscript_expression", "pointer_expression"):
        return OF + " ".join(r.src.text(value).split())
    return ""


def statement(r, st, unit, cond) -> None:
    src = r.src
    kind = st.type
    if kind == "compound_statement":
        block(r, st, unit, cond)
    elif kind == "declaration":
        local(r, st, unit, cond)
    elif kind == "if_statement":
        test = field_of(st, "condition")
        expr(r, test, unit, cond)
        label = "if " + squash(src.text(test))
        statement(r, field_of(st, "consequence"), unit, cond + (label,))
        alt = field_of(st, "alternative")
        if alt is not None:
            for sub in named_children(alt):
                statement(r, sub, unit, cond + ("else of " + label,))
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
    elif kind == "return_statement":
        values = named_children(st)
        unit.returns.append((src.line(st), squash(src.text(values[0])) if values else ""))
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
    src = r.src
    body = field_of(st, "body")
    if st.type == "for_range_loop":
        right = field_of(st, "right")
        walked = OF + one_line(r, right) + "." + cpp_expr.ITER
        name = unwrap(field_of(st, "declarator"))
        typ = field_of(st, "type")
        declared = type_of(src.text(typ)) if typ is not None else ""
        deduced = declared.replace("const", "").replace("&", "").replace("*", "").strip() in ("auto", "")
        if name is not None and name.type == "structured_binding_declarator":
            bind_names(r, name, unit, walked)
        elif name is not None:
            unit.types[src.text(name)] = walked if deduced else declared
        expr(r, right, unit, cond)
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


def lambda_unit(r, node, owner: Unit) -> Unit:
    line = r.src.line(node)
    lam = Unit("lambda", "cpp", f"{owner.name}.<lambda:{line}>", f"{owner.qual}.<lambda:{line}>", owner.owner,
               r.src.display, line, r.src.end_line(node), local_id=len(r.facts.units))
    r.facts.units.append(lam)
    lam.types = dict(owner.types)
    lam.lambdas = dict(owner.lambdas)
    decl = field_of(node, "declarator")
    if decl is not None:
        r.params(decl, lam)
    body = field_of(node, "body")
    if body is not None:
        block(r, body, lam, ())
    return lam


def expr(r, node, unit, cond) -> None:
    cpp_expr.expression(r, node, unit, cond)
