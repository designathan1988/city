"""TypeScript annotations as the values they allow (log/trace-rules.md, "JavaScript and TypeScript values by the
languages' own rules", T1-T3).

Types are erased: an annotation gives only what it names that has a runtime value. A name gives the class it names (the
typer resolves it, `TypeNames.canon`; a name that names no class gives nothing); each member of a union or an
intersection gives its own; `typeof x` gives the value of x; the language's containers give a generic text of the
ECMAScript built-ins (`js_library`) whose index, iteration and element calls give the element types. A type parameter in
scope, a primitive, a literal, a function, object, conditional, mapped or indexed type and `keyof` give nothing: the
values come from the flows.
"""
from __future__ import annotations

from .js_library import CANONICAL, UNWRAP
from .source import field_of, named_children

WRAPPERS = ("type_annotation", "opting_type_annotation", "omitting_type_annotation", "adding_type_annotation",
            "parenthesized_type", "readonly_type", "optional_type")
EITHER = ("union_type", "intersection_type")
NAMES = ("type_identifier", "nested_type_identifier", "identifier")
UNKNOWN = "?"                     # a type argument that names nothing with a runtime value


def values(r, annotation, unit) -> list[tuple]:
    """The value trees an annotation allows, read in `unit` (`typeof x` is x's value there)."""
    out = []
    for part in members(annotation):
        if part.type == "type_query":
            expr = (named_children(part) or [None])[0]
            if expr is not None:
                out.append(r.ir(expr, unit))
            continue
        text = written(r, part)
        if text and text != UNKNOWN:
            out.append(("cast", text))
    return out


def members(node) -> list:
    """The members of a union or intersection (nested ones flattened), each without its wrappers."""
    node = bare(node)
    if node is None:
        return []
    if node.type in EITHER:
        return [m for child in named_children(node) for m in members(child)]
    return [node]


def bare(node):
    while node is not None and node.type in WRAPPERS:
        node = (named_children(node) or [None])[0]
    return node


def written(r, node) -> str:
    """One type as the text the typer resolves: a name, or a built-in container of the element types written;
    UNKNOWN for what gives no value."""
    node = bare(node)
    if node is None:
        return UNKNOWN
    kind = node.type
    if kind in EITHER:
        parts = [t for t in (written(r, m) for m in members(node)) if t != UNKNOWN]
        return " | ".join(parts) if parts else UNKNOWN
    if kind in NAMES:
        text = " ".join(r.src.text(node).split())
        return UNKNOWN if text in r.type_scope() else text
    if kind == "array_type":
        return f"Array<{written(r, named_children(node)[0])}>"
    if kind == "tuple_type":
        parts = [written(r, rest_element(e)) for e in named_children(node)]
        parts = [t for t in parts if t != UNKNOWN]
        return f"Array<{' | '.join(parts)}>" if parts else UNKNOWN
    if kind == "generic_type":
        return generic(r, node)
    return UNKNOWN                # primitives, literals, function/object/conditional/mapped/indexed types, keyof


def rest_element(node):
    """A tuple's `...T[]` element: the elements of T[]."""
    if node.type == "rest_type":
        inner = bare((named_children(node) or [None])[0])
        if inner is not None and inner.type == "array_type":
            return named_children(inner)[0]
        return inner
    return node


def generic(r, node) -> str:
    head = " ".join(r.src.text(field_of(node, "name")).split())
    args_node = field_of(node, "type_arguments")
    args = [written(r, a) for a in named_children(args_node)] if args_node is not None else []
    if head in UNWRAP:
        return args[0] if args else UNKNOWN
    if head in CANONICAL:
        return f"{CANONICAL[head]}<{', '.join(args)}>"
    return UNKNOWN if head in r.type_scope() else head      # a class of the program's: its type arguments are erased


def parameters(r, node) -> set:
    """The type parameters a generic declaration introduces (`<T, U extends X>`)."""
    params = field_of(node, "type_parameters") if node is not None else None
    if params is None:
        return set()
    return {r.src.text(field_of(p, "name")) for p in named_children(params) if p.type == "type_parameter"}
