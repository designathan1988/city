"""C++ expressions (log/trace-rules.md: CALL, FLOW, BIND): calls, member access, assignments, addresses of functions,
lambdas, `new`, and Godot's registrations. Receivers, arguments and written values are kept as expression trees; a
member reached with `->` has as receiver what the pointer points at.
"""
from __future__ import annotations

from . import cpp_body, cpp_implicit
from .cpp_names import strip_templates
from .ir import put
from .model import STD_MUTATORS
from .rules import GDEXT_BIND as BIND_CALLS, GDEXT_PROPERTY, GDEXT_SIGNAL, NAME_CALLS
from .source import field_of, is_identifier, named_children, string_value

THREAD_NAMES = ("thread", "jthread", "async")
LITERALS = ("string_literal", "raw_string_literal", "number_literal", "char_literal", "this", "true", "false", "null",
            "nullptr", "comment", "type_descriptor", "primitive_type", "sizeof_expression", "concatenated_string")
STATEMENTS = ("compound_statement", "if_statement", "for_statement", "for_range_loop", "while_statement",
              "do_statement", "switch_statement", "return_statement", "declaration", "try_statement")


def add(r, unit, kind, name, node, cond, **extra) -> None:
    cpp_body.add(r, unit, kind, name, node, cond, **extra)


def scoped(text: str) -> tuple[str, str]:
    """`ns::A::f` as ("ns::A", "f")."""
    scope, _, name = strip_templates(text).rpartition("::")
    return scope, name


def receiver(r, node, unit) -> tuple:
    """The receiver of a member access: the object written, or what it points at for `->`."""
    base = r.ir(field_of(node, "argument"), unit)
    op = field_of(node, "operator")
    return ("deref", base) if op is not None and r.src.text(op) == "->" else base


def owner_of(base: tuple | None) -> tuple[tuple | None, str]:
    """A receiver tree as (its own receiver, member name): `a.b` -> (a, "b"), `x` -> (None, "x")."""
    if base is None:
        return None, ""
    if base[0] == "member":
        return (("deref", base[1]) if base[3] else base[1]), base[2]
    if base[0] == "name":
        return None, base[1]
    return base, ""


def expression(r, node, unit, cond) -> None:
    if node is None:
        return
    kind = node.type
    if kind in STATEMENTS:
        cpp_body.statement(r, node, unit, cond)
    elif kind == "call_expression":
        call(r, node, unit, cond)
    elif kind == "field_expression":
        expression(r, field_of(node, "argument"), unit, cond)
        add(r, unit, "read", strip_templates(r.src.text(field_of(node, "field"))), node, cond,
            base=receiver(r, node, unit))
    elif kind == "identifier":
        add(r, unit, "read", r.src.text(node), node, cond)
    elif kind == "qualified_identifier":
        scope, name = scoped(r.src.text(node))
        add(r, unit, "read", name, node, cond, qualified=True, scope=scope)
    elif kind == "assignment_expression":
        cpp_implicit.operator(r, unit, node, cond, add)
        value = r.ir(field_of(node, "right"), unit)
        op = field_of(node, "operator")
        target(r, field_of(node, "left"), unit, cond, value if op is None or r.src.text(op) == "=" else None)
        expression(r, field_of(node, "right"), unit, cond)
    elif kind == "update_expression":
        cpp_implicit.operator(r, unit, node, cond, add)
        target(r, field_of(node, "argument"), unit, cond, None)
    elif kind == "compound_literal_expression":
        # `T{args}` constructs a T (C7)
        scope, name = scoped(r.src.text(field_of(node, "type")))
        values = field_of(node, "value") or next((c for c in named_children(node)
                                                   if c.type == "initializer_list"), None)
        add(r, unit, "new", name, node, cond, qualified=bool(scope), scope=scope, text=cpp_implicit.IMPLICIT,
            args=tuple(r.ir(v, unit) for v in named_children(values)) if values is not None else ())
        expression(r, values, unit, cond)
    elif kind == "pointer_expression" and r.src.text(node).lstrip().startswith("&"):
        inner = field_of(node, "argument")
        if inner is not None and inner.type in ("identifier", "qualified_identifier"):
            scope, name = scoped(r.src.text(inner))
            add(r, unit, "value", name, inner, cond, qualified=bool(scope), scope=scope)
        else:
            expression(r, inner, unit, cond)
    elif kind in ("binary_expression", "unary_expression", "subscript_expression"):
        cpp_implicit.operator(r, unit, node, cond, add)
        for child in named_children(node):
            expression(r, child, unit, cond)
    elif kind == "delete_expression":
        cpp_implicit.destroyed(r, unit, node, cond, add)
        for child in named_children(node):
            expression(r, child, unit, cond)
    elif kind == "field_initializer":
        member = next((c for c in named_children(node) if c.type in ("field_identifier", "identifier",
                                                                      "qualified_identifier", "template_type")), None)
        values = next((c for c in named_children(node) if c.type in ("argument_list", "initializer_list")), None)
        trees = tuple(r.ir(v, unit) for v in named_children(values)) if values is not None else ()
        if member is not None:
            # `: m(v)` writes member m; `: Base(args)` runs the base's constructor (C7)
            add(r, unit, "write", r.src.text(member), node, cond, args=trees[:1])
            scope, name = scoped(r.src.text(member))
            add(r, unit, "new", name, node, cond, qualified=bool(scope), scope=scope, args=trees,
                text=cpp_implicit.IMPLICIT)
        expression(r, values, unit, cond)
    elif kind == "lambda_expression":
        add(r, unit, "lambda", "", node, cond, target=r.lambda_id(node, unit))
    elif kind == "new_expression":
        scope, name = scoped(r.src.text(field_of(node, "type")))
        args = next((c for c in named_children(node) if c.type in ("argument_list", "initializer_list")), None)
        add(r, unit, "new", name, node, cond, qualified=bool(scope), scope=scope,
            args=tuple(r.ir(a, unit) for a in named_children(args)) if args is not None else ())
        expression(r, args, unit, cond)
    elif kind not in LITERALS:
        for child in named_children(node):
            expression(r, child, unit, cond)


def target(r, node, unit, cond, value: tuple | None) -> None:
    """The left side of an assignment or an increment: the name written and the value that flows into it
    (None for a compound assignment or an increment: the value keeps its type)."""
    if node is None:
        return
    args = (value,) if value is not None else ()
    if node.type == "identifier":
        name = r.src.text(node)
        if name in unit.types and value is not None:
            unit.types[name].append(value)
        add(r, unit, "write", name, node, cond, args=args)
    elif node.type == "qualified_identifier":
        scope, name = scoped(r.src.text(node))
        add(r, unit, "write", name, node, cond, qualified=True, scope=scope, args=args)
    elif node.type == "field_expression":
        expression(r, field_of(node, "argument"), unit, cond)
        add(r, unit, "write", r.src.text(field_of(node, "field")), node, cond, base=receiver(r, node, unit), args=args)
    elif node.type == "subscript_expression":
        target(r, field_of(node, "argument"), unit, cond, put(value) if value is not None else None)
        for child in named_children(node)[1:]:
            expression(r, child, unit, cond)
    elif node.type in ("pointer_expression", "parenthesized_expression"):
        target(r, field_of(node, "argument") or (named_children(node) or [None])[0], unit, cond, value)
    else:
        expression(r, node, unit, cond)


def call(r, node, unit, cond) -> None:
    src = r.src
    fn = field_of(node, "function")
    args = field_of(node, "arguments")
    argv = named_children(args) if args is not None else []
    base, scope = None, ""
    if fn.type == "field_expression":
        expression(r, field_of(fn, "argument"), unit, cond)
        base = receiver(r, fn, unit)
        name = strip_templates(src.text(field_of(fn, "field")))
    elif fn.type in ("identifier", "qualified_identifier", "template_function"):
        scope, name = scoped(src.text(fn))
        if fn.type == "template_function" or "<" in src.text(fn):
            targs = field_of(fn, "arguments") or next((c for c in named_children(fn)
                                                        if c.type == "template_argument_list"), None)
            for t in named_children(targs) if targs is not None else []:
                tscope, tname = scoped(src.text(t))
                if is_identifier(tname):
                    add(r, unit, "new", tname, t, cond, qualified=bool(tscope), scope=tscope,
                        text=cpp_implicit.IMPLICIT)
    else:
        # calling the value of an expression (`handlers[i](x)`, `make()(x)`): what that value holds is called
        expression(r, fn, unit, cond)
        add(r, unit, "invoke", "", node, cond, base=r.ir(fn, unit), args=tuple(r.ir(a, unit) for a in argv))
        for a in argv:
            expression(r, a, unit, cond)
        return
    if registration(r, name, argv, unit, node, cond):
        return
    first = string_value(src, argv[0]) if argv else None
    trees = tuple(r.ir(a, unit) for a in argv)
    if name == "emit_signal" and first is not None:
        add(r, unit, "emit", first, node, cond, base=base, args=trees[1:])
    elif name in NAME_CALLS and is_identifier(first):
        add(r, unit, "name", first, node, cond, base=base, text=first, args=trees[1:])
    else:
        add(r, unit, "call", name, node, cond, base=base, qualified=bool(scope), scope=scope, argc=len(argv),
            args=trees)
        owner, member = owner_of(base)
        if name in STD_MUTATORS and member:
            value = put(trees[-1]) if trees else None
            if owner is None and member in unit.types and value is not None:
                unit.types[member].append(value)
            add(r, unit, "write", member, node, cond, base=owner, args=(value,) if value is not None else ())
    thread = name in THREAD_NAMES
    for a in argv:
        before = len(unit.refs)
        expression(r, a, unit, cond)
        if thread:
            for ref in unit.refs[before:]:
                if ref.kind == "value":
                    ref.kind = "thread"


def registration(r, name, argv, unit, node, cond) -> bool:
    """Godot's bindings: a name the engine calls a method by, a signal, a property's accessors (BIND)."""
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
