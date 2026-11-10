"""C++ expressions: calls, member chains, assignments, addresses of functions, lambdas, `new`, and Godot's registrations.

Each construct is read by its grammar rule. A member chain records its receivers in order and returns them, so a
call on a call's result (`get().f()`) keeps the first call as the receiver's origin.
"""
from __future__ import annotations

from . import cpp_body
from .cpp_names import TYPED, strip_templates, template_call_type
from .gd_body import CALLED, UNKNOWN
from .model import STD_MUTATORS
from .rules import GDEXT_BIND as BIND_CALLS, GDEXT_PROPERTY, GDEXT_SIGNAL, NAME_CALLS
from .source import field_of, is_identifier, named_children, string_value

ITEM = "<item>"      # a subscript's result: a container's element
ITER = "<iter>"      # what a range-for walks: a container's element (a map's key and value pair)
DEREF = "<deref>"    # `->`: the object a pointer, smart pointer, optional or iterator stands for
THREAD_NAMES = ("thread", "jthread", "async")


def add(r, unit, kind, name, node, cond, **extra) -> None:
    cpp_body.add(r, unit, kind, name, node, cond, **extra)


def qualified(text: str) -> tuple[tuple, str]:
    parts = strip_templates(text).split("::")
    return tuple(parts[:-1]), parts[-1]


def expression(r, node, unit, cond) -> None:
    if node is None:
        return
    kind = node.type
    if kind in ("compound_statement", "if_statement", "for_statement", "for_range_loop", "while_statement",
                "do_statement", "switch_statement", "return_statement", "declaration", "try_statement"):
        cpp_body.statement(r, node, unit, cond)
    elif kind == "call_expression":
        call(r, node, unit, cond)
    elif kind in ("field_expression", "subscript_expression"):
        chain(r, node, unit, cond)
    elif kind == "identifier":
        add(r, unit, "read", r.src.text(node), node, cond)
    elif kind == "qualified_identifier":
        recv, name = qualified(r.src.text(node))
        add(r, unit, "read", name, node, cond, recv=recv, qualified=True)
    elif kind in ("assignment_expression",):
        target(r, field_of(node, "left"), unit, cond)
        expression(r, field_of(node, "right"), unit, cond)
    elif kind == "update_expression":
        target(r, field_of(node, "argument"), unit, cond)
    elif kind == "pointer_expression" and r.src.text(node).lstrip().startswith("&"):
        inner = field_of(node, "argument")
        if inner is not None and inner.type in ("identifier", "qualified_identifier"):
            recv, name = qualified(r.src.text(inner))
            add(r, unit, "value", name, inner, cond, recv=recv, qualified=bool(recv))
        else:
            expression(r, inner, unit, cond)
    elif kind == "lambda_expression":
        lam = cpp_body.lambda_unit(r, node, unit)
        add(r, unit, "lambda", lam.name, node, cond, target=lam.local_id)
    elif kind == "new_expression":
        recv, name = qualified(r.src.text(field_of(node, "type")))
        add(r, unit, "new", name, node, cond, recv=recv, qualified=bool(recv))
        for child in named_children(node):
            if child.type in ("argument_list", "initializer_list"):
                expression(r, child, unit, cond)
    elif kind not in ("string_literal", "raw_string_literal", "number_literal", "char_literal", "this", "true",
                      "false", "null", "nullptr", "comment", "type_descriptor", "primitive_type", "sizeof_expression"):
        for child in named_children(node):
            expression(r, child, unit, cond)


def target(r, node, unit, cond) -> None:
    """The left side of an assignment or an increment: the name written."""
    if node is None:
        return
    if node.type == "identifier":
        add(r, unit, "write", r.src.text(node), node, cond)
    elif node.type == "qualified_identifier":
        recv, name = qualified(r.src.text(node))
        add(r, unit, "write", name, node, cond, recv=recv, qualified=True)
    elif node.type == "field_expression":
        base = through(r, node, chain(r, field_of(node, "argument"), unit, cond))
        add(r, unit, "write", r.src.text(field_of(node, "field")), node, cond, recv=base)
    elif node.type == "subscript_expression":
        target(r, field_of(node, "argument"), unit, cond)
        for child in named_children(node)[1:]:
            expression(r, child, unit, cond)
    elif node.type in ("pointer_expression", "parenthesized_expression"):
        inner = field_of(node, "argument") or (named_children(node) or [None])[0]
        target(r, inner, unit, cond)
    else:
        expression(r, node, unit, cond)


def through(r, node, base: tuple) -> tuple:
    """A member reached with `->` is reached through what the receiver points at."""
    op = field_of(node, "operator")
    return base + (DEREF,) if op is not None and r.src.text(op) == "->" else base


def chain(r, node, unit, cond) -> tuple:
    """The receiver names of a member access, in order; every name and call met on the way is recorded."""
    kind = node.type
    if kind == "identifier":
        name = r.src.text(node)
        add(r, unit, "read", name, node, cond)
        return (name,)
    if kind == "this":
        return ("this",)
    if kind == "qualified_identifier":
        recv, name = qualified(r.src.text(node))
        add(r, unit, "read", name, node, cond, recv=recv, qualified=True)
        return ("::".join(recv + (name,)),)
    if kind == "field_expression":
        base = through(r, node, chain(r, field_of(node, "argument"), unit, cond))
        name = r.src.text(field_of(node, "field"))
        add(r, unit, "read", name, node, cond, recv=base)
        return base + (name,)
    if kind == "call_expression":
        return call(r, node, unit, cond)
    if kind == "subscript_expression":
        base = chain(r, field_of(node, "argument"), unit, cond)
        for child in named_children(node)[1:]:
            expression(r, child, unit, cond)
        return base + (ITEM,)
    if kind in ("parenthesized_expression", "pointer_expression"):
        inner = field_of(node, "argument") or (named_children(node) or [None])[0]
        return chain(r, inner, unit, cond) if inner is not None else (UNKNOWN,)
    expression(r, node, unit, cond)
    return (UNKNOWN,)


def call(r, node, unit, cond) -> tuple:
    """A call; returns the receiver chain its result stands for."""
    src = r.src
    fn = field_of(node, "function")
    args = field_of(node, "arguments")
    argv = named_children(args) if args is not None else []
    if fn.type == "field_expression":
        recv = through(r, fn, chain(r, field_of(fn, "argument"), unit, cond))
        name = strip_templates(src.text(field_of(fn, "field")))
        qual = False
    elif fn.type in ("identifier", "qualified_identifier", "template_function"):
        recv, name = qualified(src.text(fn))
        qual = bool(recv)
        if fn.type == "template_function":
            targs = field_of(fn, "arguments")
            for t in named_children(targs) if targs is not None else []:
                tr, tn = qualified(src.text(t))
                if is_identifier(tn):
                    add(r, unit, "new", tn, t, cond, recv=tr, qualified=bool(tr))
    else:
        expression(r, fn, unit, cond)
        for a in argv:
            expression(r, a, unit, cond)
        return (UNKNOWN,)
    if registration(r, name, argv, unit, node, cond):
        return (UNKNOWN,)
    first = string_value(src, argv[0]) if argv else None
    if name == "emit_signal" and first is not None:
        add(r, unit, "emit", first, node, cond, recv=recv)
        argv = argv[1:]
    elif name in NAME_CALLS and is_identifier(first):
        add(r, unit, "name", first, node, cond, recv=recv, text=first)
        argv = argv[1:]
    else:
        add(r, unit, "call", name, node, cond, recv=recv, qualified=qual, argc=len(argv))
        if name in STD_MUTATORS and recv and is_identifier(recv[-1]):
            add(r, unit, "write", recv[-1], node, cond, recv=recv[:-1])
    thread = name in THREAD_NAMES
    for a in argv:
        before = len(unit.refs)
        expression(r, a, unit, cond)
        if thread:
            for ref in unit.refs[before:]:
                if ref.kind in ("value", "lambda"):
                    ref.kind = "thread" if ref.kind == "value" else ref.kind
    named = template_call_type(src.text(fn)) if fn.type != "field_expression" else None
    return (TYPED + named,) if named else recv + (CALLED + name,)


def registration(r, name, argv, unit, node, cond) -> bool:
    """Godot's bindings: a name the engine calls a method by, a signal, a property's accessors."""
    src = r.src
    line = src.line(node)
    if name in BIND_CALLS:
        method = next((a for a in argv if a.type == "call_expression"
                       and src.text(field_of(a, "function")) == "D_METHOD"), None)
        bound = None
        if method is not None:
            inner = named_children(field_of(method, "arguments"))
            bound = string_value(src, inner[0]) if inner else None
        elif argv and string_value(src, argv[0]) is not None:
            bound = string_value(src, argv[0])
        ptr = next((a for a in argv if a.type == "pointer_expression"), None)
        if bound and ptr is not None:
            r.facts.bindings.append((bound, strip_templates(src.text(ptr)).lstrip("&"), unit.owner, line, "method"))
        return True
    if name == GDEXT_SIGNAL and argv:
        info = named_children(field_of(argv[0], "arguments")) if argv[0].type == "call_expression" else []
        signal = string_value(src, info[0]) if info else None
        if signal:
            r.facts.bindings.append((signal, "", unit.owner, line, "signal"))
        return True
    if name == GDEXT_PROPERTY and len(argv) >= 3:
        info = named_children(field_of(argv[0], "arguments")) if argv[0].type == "call_expression" else []
        prop = next((string_value(src, a) for a in info if string_value(src, a)), None)
        setter, getter = string_value(src, argv[1]), string_value(src, argv[2])
        if prop:
            r.facts.bindings.append((prop, f"{setter or ''}|{getter or ''}", unit.owner, line, "property"))
        return True
    return False
