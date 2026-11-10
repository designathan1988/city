"""Values: every expression written as a marker the typer can work out, and every place a value flows into a name.

The rule (GDScript reference, "Variant" and static typing: an untyped variable has the type of the value assigned to
it; cppreference, `auto`: deduced from the initializer, a generic lambda's parameters from the call's arguments, a
deduced return type from the `return` statements): the type of a name is its declared type, or else the union of the
types of all values that flow into it -- its initializer and every assignment, the argument at every call site for a
parameter, the `return` values for a call's result, the values put into a container for its elements, the elements
walked for a loop variable. A value the program does not create (a literal, what the engine or the library returns)
is the engine's: its members are the engine's.

Markers: a type as written; OF + member chain (`<of>a.b[i]`); CALLED + callee (`<call>a.f`); TYPED + type (a cast);
ENGINE; ANY + markers joined by SEP (a union: a ternary); OP + operator + SEP + left operand marker; LAMBDA + local id.
"""
from __future__ import annotations

from . import gd_body
from .rules import BIND_CALLS
from .source import field_of, named_children, string_value

OF = "<of>"
ENGINE = "<engine>"
ANY = "<any>"
OP = "<op>"
LAMBDA = "<lambda>"
PUT = "<put>"         # a value put into a container: what its elements hold
SEP = "\x1e"
GD_LITERALS = ("integer", "float", "string", "string_name", "true", "false", "null", "array", "dictionary",
               "node_path", "get_node", "comment")
CPP_LITERALS = ("number_literal", "string_literal", "raw_string_literal", "char_literal", "true", "false", "null",
                "nullptr", "concatenated_string", "initializer_list", "sizeof_expression", "user_defined_literal")


def one_line(src, node) -> str:
    return " ".join(src.text(node).split())


def union(markers) -> str:
    kept = [m for m in dict.fromkeys(markers) if m]
    return kept[0] if len(kept) == 1 else (ANY + SEP.join(kept) if kept else "")


def split(marker: str) -> list[str]:
    return marker[len(ANY):].split(SEP) if marker.startswith(ANY) else [marker]


def gd_mark(r, node) -> str:
    """The marker of a GDScript expression."""
    if node is None:
        return ""
    kind = node.type
    if kind in GD_LITERALS:
        return ENGINE
    if kind in ("identifier", "attribute", "subscript") and not any(
            c.type in ("attribute_call", "call") for c in named_children(node)):
        return OF + one_line(r.src, node)
    if kind == "parenthesized_expression":
        return gd_mark(r, named_children(node)[0]) if named_children(node) else ""
    if kind == "binary_operator":
        op = field_of(node, "op")
        if op is not None and r.src.text(op) in ("is", "as"):
            return gd_body.TYPED + r.src.text(field_of(node, "right")) if r.src.text(op) == "as" else ENGINE
        return OP + (r.src.text(op) if op is not None else "") + SEP + gd_mark(r, field_of(node, "left"))
    if kind in ("conditional_expression", "ternary_expression"):
        parts = named_children(node)
        return union([gd_mark(r, parts[0]), gd_mark(r, parts[-1])]) if parts else ""
    if kind in ("unary_operator", "await_expression"):
        return gd_mark(r, named_children(node)[-1]) if named_children(node) else ""
    if kind == "call" and r.src.text(named_children(node)[0]) == "Callable":
        args = named_children(field_of(node, "arguments")) if field_of(node, "arguments") is not None else []
        name = string_value(r.src, args[1]) if len(args) >= 2 else None
        if name:
            return OF + one_line(r.src, args[0]) + "." + name     # Callable(obj, "m"): obj's method m
        return gd_mark(r, args[0]) if args else ENGINE
    if kind == "attribute":
        kids = named_children(node)
        if kids[-1].type == "attribute_call" and r.src.text(named_children(kids[-1])[0]) in BIND_CALLS:
            if len(kids) == 2:
                return gd_mark(r, kids[0])                            # f.bind(x) holds f
            return OF + ".".join(one_line(r.src, k) for k in kids[:-1])
    if kind in ("call", "attribute"):
        typed = r.infer(node)
        if typed:
            return typed
        return gd_body.CALLED + squash_calls(r.src, node)
    if kind == "lambda":
        return ""
    return ENGINE


def squash_calls(src, node) -> str:
    """`a.b(x).c(y)` -> `a.b.c`: a call chain as the steps the typer walks (the last one called)."""
    if node.type == "call":
        return one_line(src, named_children(node)[0])
    parts = []
    for child in named_children(node):
        if child.type == "attribute_call":
            parts.append(one_line(src, named_children(child)[0]))
        elif child.type == "call":
            parts.append(one_line(src, named_children(child)[0]) + "()")
        else:
            parts.append(one_line(src, child))
    return ".".join(parts)


def cpp_mark(r, node) -> str:
    """The marker of a C++ expression."""
    from .cpp_names import template_call_type, type_of
    if node is None:
        return ""
    kind = node.type
    if kind in CPP_LITERALS:
        return ENGINE
    if kind in ("identifier", "field_expression", "subscript_expression", "qualified_identifier") and not \
            any(c.type == "call_expression" for c in named_children(node)):
        return OF + one_line(r.src, node)
    if kind == "this":
        return OF + "this"
    if kind in ("parenthesized_expression", "pointer_expression", "reference_expression"):
        inner = field_of(node, "argument") or (named_children(node) or [None])[-1]
        marker = cpp_mark(r, inner)
        if kind == "pointer_expression" and r.src.text(node).lstrip().startswith("*"):
            return OP + "*" + SEP + marker
        return marker
    if kind == "call_expression":
        fn = field_of(node, "function")
        named = template_call_type(r.src.text(fn))
        return named or gd_body.CALLED + one_line(r.src, fn).replace("->", ".")
    if kind == "new_expression":
        return type_of(r.src.text(field_of(node, "type")))
    if kind in ("cast_expression", "compound_literal_expression"):
        return type_of(r.src.text(field_of(node, "type")))
    if kind == "conditional_expression":
        return union([cpp_mark(r, field_of(node, "consequence")), cpp_mark(r, field_of(node, "alternative"))])
    if kind in ("binary_expression", "assignment_expression"):
        op = field_of(node, "operator")
        return OP + (r.src.text(op) if op is not None else "") + SEP + cpp_mark(r, field_of(node, "left"))
    if kind in ("unary_expression", "update_expression"):
        return cpp_mark(r, field_of(node, "argument"))
    if kind == "lambda_expression":
        return ""
    return ENGINE


def element(marker: str) -> str:
    return PUT + marker if marker else ""


def walked(marker: str) -> str:
    """What a loop over a value walks: its elements."""
    return OP + "<iter>" + SEP + marker if marker else ""
