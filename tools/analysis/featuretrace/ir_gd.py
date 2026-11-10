"""GDScript expressions to IR, one builder per expression construct of the grammar (log/trace-rules.md, GDScript rows).

Sources: GDScript reference, Operators (precedence; `is`, `as`, `in`, ternary), `await`, `$`/`%` node access, `&"name"`,
`Callable(obj, "m")` and `Callable.bind` (the same target), `preload` (a script resource).
"""
from __future__ import annotations

from .ir import LITERAL, name, union
from .rules import BIND_CALLS
from .source import field_of, named_children, string_value

TESTS = ("is", "in", "not", "and", "or", "&&", "||", "==", "!=", "<", ">", "<=", ">=")
LITERAL_TYPES = {"integer": "int", "float": "float", "string": "String", "true": "bool", "false": "bool",
                 "null": "null", "escape_sequence": "String", "node_path": "NodePath"}


def build(src, node, lam) -> tuple:
    """The IR of an expression node; `lam(node)` gives the local id of a lambda's unit."""
    if node is None:
        return LITERAL
    return BUILDERS[node.type](src, node, lam)


def _args(src, node, lam) -> tuple:
    return tuple(build(src, a, lam) for a in named_children(node)) if node is not None else ()


def _identifier(src, node, lam):
    text = src.text(node)
    return ("self",) if text == "self" else ("super",) if text == "super" else name(text)


def _call(src, node, lam):
    callee = named_children(node)[0]
    args = field_of(node, "arguments")
    argv = named_children(args) if args is not None else []
    text = src.text(callee)
    if text == "Callable" and len(argv) >= 2 and string_value(src, argv[1]):
        return ("member", build(src, argv[0], lam), string_value(src, argv[1]), False)
    if text in ("preload", "load") and argv and string_value(src, argv[0]) is not None:
        return ("cast", "path:" + string_value(src, argv[0]))
    if text == "super":
        return ("call", ("super",), _args(src, args, lam))
    return ("call", build(src, callee, lam), _args(src, args, lam))


def _segment(src, base, seg, lam) -> tuple:
    """One step of a member chain: `.b`, `.f(args)`, `.b[i]`."""
    if seg.type == "identifier":
        return ("member", base, src.text(seg), False)
    first = named_children(seg)[0]
    if seg.type == "attribute_call":
        method = src.text(first)
        if method in BIND_CALLS:
            return base                              # f.bind(x) is a callable of the same function
        return ("call", ("member", base, method, False), _args(src, field_of(seg, "arguments"), lam))
    if seg.type == "attribute_subscript":
        index = next((c for c in named_children(seg) if c.type == "subscript_arguments"), None)
        arg = named_children(index)[0] if index is not None and named_children(index) else None
        return ("index", ("member", base, src.text(first), False), build(src, arg, lam))
    return ("member", base, src.text(seg), False)


def _attribute(src, node, lam):
    kids = named_children(node)
    value = build(src, kids[0], lam)
    for seg in kids[1:]:
        value = _segment(src, value, seg, lam)
    return value


def _subscript(src, node, lam):
    kids = named_children(node)
    index = next((c for c in kids[1:] if c.type == "subscript_arguments"), None)
    arg = named_children(index)[0] if index is not None and named_children(index) else None
    return ("index", build(src, kids[0], lam), build(src, arg, lam))


def _binary(src, node, lam):
    op = src.text(field_of(node, "op")).strip()
    if op == "as":
        return ("cast", src.text(field_of(node, "right")))
    if op.split()[0] in TESTS or op.endswith(" in"):
        return ("literal", "bool")
    return ("op", op, build(src, field_of(node, "left"), lam), build(src, field_of(node, "right"), lam))


def _unary(src, node, lam):
    kids = named_children(node)
    op = src.text(node)[: kids[-1].start_byte - node.start_byte].strip() if kids else ""
    if op in ("not", "!"):
        return ("literal", "bool")
    return ("unary", op, build(src, kids[-1] if kids else None, lam))


def _conditional(src, node, lam):
    kids = named_children(node)
    return union(build(src, kids[0], lam), build(src, kids[-1], lam))


def _collection(src, node, lam):
    values = []
    for item in named_children(node):
        values.append(build(src, named_children(item)[-1], lam) if item.type == "pair" else build(src, item, lam))
    return ("collection", tuple(values))


def _base_call(src, node, lam):
    kids = named_children(node)
    method = next((c for c in kids if c.type == "identifier"), None)
    args = next((c for c in kids if c.type == "arguments"), None)
    return ("call", ("member", ("super",), src.text(method) if method else "", False), _args(src, args, lam))


BUILDERS = {
    "identifier": _identifier,
    "call": _call,
    "attribute": _attribute,
    "attribute_call": lambda s, n, l: _segment(s, ("self",), n, l),
    "attribute_subscript": lambda s, n, l: _segment(s, ("self",), n, l),
    "subscript": _subscript,
    "subscript_arguments": lambda s, n, l: build(s, (named_children(n) or [None])[0], l),
    "binary_operator": _binary,
    "unary_operator": _unary,
    "conditional_expression": _conditional,
    "parenthesized_expression": lambda s, n, l: build(s, (named_children(n) or [None])[0], l),
    "array": _collection,
    "dictionary": _collection,
    "pair": lambda s, n, l: build(s, named_children(n)[-1], l),
    "await_expression": lambda s, n, l: ("await", build(s, (named_children(n) or [None])[-1], l)),
    "lambda": lambda s, n, l: ("callable", l(n)),
    "string_name": lambda s, n, l: ("strname", string_value(s, n) or ""),
    "get_node": lambda s, n, l: ("node",),
    "base_call": _base_call,
    **{kind: (lambda t: (lambda s, n, l: ("literal", t)))(t) for kind, t in LITERAL_TYPES.items()},
}


step = _segment   # one member-chain step on a receiver tree (used by the reader walking a chain)
