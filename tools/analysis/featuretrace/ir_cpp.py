"""C++ expressions to IR, one builder per expression construct of the grammar (log/trace-rules.md, C++ rows).

Sources: cppreference, Expressions (primary, postfix, unary, cast, binary, conditional, assignment, comma, lambda,
new/delete, sizeof/alignof), member access (`.`, `->`), value categories (an assignment is its left operand, a comma
its right), casts, and the library makers (`make_shared<T>` / `make_unique<T>`: a pointer to T).
"""
from __future__ import annotations

from .cpp_names import strip_templates, template_call_type, type_of
from .ir import LITERAL, name, union
from .rules import CPP_RULES
from .source import field_of, named_children


def build(src, node, lam) -> tuple:
    """The IR of an expression node; `lam(node)` gives the local id of a lambda's unit."""
    if node is None:
        return LITERAL
    builder = BUILDERS.get(node.type)
    if builder is not None:
        return builder(src, node, lam)
    # a construct of another role in an expression's place (log/trace-rules.md): a type names a value of that
    # type (a functional cast `T(x)`); anything else carries no value of the program
    roles = CPP_RULES.get(node.type, ())
    return ("cast", " ".join(src.text(node).split())) if "TYPE" in roles else LITERAL


def _inner(src, node, lam):
    kids = named_children(node)
    return build(src, kids[-1] if kids else None, lam)


def _args(src, node, lam) -> tuple:
    return tuple(build(src, a, lam) for a in named_children(node)) if node is not None else ()


def _call(src, node, lam):
    fn = field_of(node, "function")
    named = template_call_type(src.text(fn)) if fn.type != "field_expression" else None
    if named:
        return ("cast", named)
    callee = build(src, fn, lam)
    if callee[0] == "cast":
        return callee                    # calling a type is a functional cast: a value of that type
    return ("call", callee, _args(src, field_of(node, "arguments"), lam))


def _field(src, node, lam):
    op = field_of(node, "operator")
    return ("member", build(src, field_of(node, "argument"), lam), strip_templates(src.text(field_of(node, "field"))),
            op is not None and src.text(op) == "->")


def _subscript(src, node, lam):
    index = field_of(node, "indices") or next((c for c in named_children(node)[1:]), None)
    if index is not None and index.type == "subscript_argument_list":
        index = (named_children(index) or [None])[0]
    return ("index", build(src, field_of(node, "argument"), lam), build(src, index, lam))


def _pointer(src, node, lam):
    operand = build(src, field_of(node, "argument"), lam)
    return ("addr", operand) if src.text(node).lstrip().startswith("&") else ("deref", operand)


def _unary(src, node, lam):
    op = src.text(field_of(node, "operator")) if field_of(node, "operator") is not None else ""
    return ("unary", op, build(src, field_of(node, "argument"), lam))


def _binary(src, node, lam):
    return ("op", src.text(field_of(node, "operator")), build(src, field_of(node, "left"), lam),
            build(src, field_of(node, "right"), lam))


def _type_of(src, node) -> str:
    typ = field_of(node, "type")
    return type_of(src.text(typ)) if typ is not None else ""


BUILDERS = {
    "identifier": lambda s, n, l: name(s.text(n)),
    "field_identifier": lambda s, n, l: name(s.text(n)),
    "qualified_identifier": lambda s, n, l: name(strip_templates(s.text(n))),
    "template_function": lambda s, n, l: name(strip_templates(s.text(n))),
    "template_method": lambda s, n, l: name(strip_templates(s.text(n))),
    "this": lambda s, n, l: ("self",),
    "call_expression": _call,
    "argument_list": lambda s, n, l: ("collection", _args(s, n, l)),
    "field_expression": _field,
    "subscript_expression": _subscript,
    "subscript_argument_list": _inner,
    "pointer_expression": _pointer,
    "unary_expression": _unary,
    "update_expression": lambda s, n, l: build(s, field_of(n, "argument"), l),
    "binary_expression": _binary,
    "assignment_expression": lambda s, n, l: build(s, field_of(n, "left"), l),     # the value is the left object
    "comma_expression": lambda s, n, l: build(s, field_of(n, "right"), l),
    "conditional_expression": lambda s, n, l: union(build(s, field_of(n, "consequence"), l),
                                                    build(s, field_of(n, "alternative"), l)),
    "cast_expression": lambda s, n, l: ("cast", _type_of(s, n)),
    "compound_literal_expression": lambda s, n, l: ("cast", _type_of(s, n)),
    "new_expression": lambda s, n, l: ("new", _type_of(s, n)),
    "delete_expression": lambda s, n, l: ("literal", "void"),
    "initializer_list": lambda s, n, l: ("collection", _args(s, n, l)),
    "initializer_pair": _inner,
    "lambda_expression": lambda s, n, l: ("callable", l(n)),
    "parenthesized_expression": _inner,
    "co_await_expression": lambda s, n, l: ("await", build(s, field_of(n, "argument"), l)),
    "co_yield_statement": lambda s, n, l: LITERAL,
    **{kind: (lambda s, n, l: ("literal", "std::size_t")) for kind in
       ("sizeof_expression", "alignof_expression", "offsetof_expression")},
    **{kind: (lambda s, n, l: LITERAL) for kind in
       ("number_literal", "char_literal", "character", "string_literal", "raw_string_literal", "concatenated_string",
        "string_content", "raw_string_content", "raw_string_delimiter", "escape_sequence", "user_defined_literal",
        "literal_suffix", "true", "false", "null", "field_designator", "subscript_designator",
        "subscript_range_designator", "generic_expression", "extension_expression", "fold_expression",
        "parameter_pack_expansion", "lambda_capture_specifier", "lambda_default_capture",
        "lambda_capture_initializer")},
}
