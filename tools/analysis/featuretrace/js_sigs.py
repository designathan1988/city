"""Declared types kept whole (log/trace-core-plan.md, Task 4): where `js_types` erases a TypeScript type to the values
it admits (T1), this keeps it as a type tree (`vocab` T_*) whose type parameters stay what they are — the declaring
type's, the function's own, an alias's — so the analysis can read a platform's signatures as what they say: a
parameter of type `T` puts its argument in the receiver's place of `T`, a result of type `T` reads it. The tree is
TypeScript's type grammar as written (TypeScript handbook: Generics, Everyday Types, Object Types, Type Operators,
Indexed Access, Conditional, Mapped and Template Literal Types); a name is kept with the file and scope it is written
in, for the analysis to resolve as the language does. `T[]` is `Array<T>` (handbook, Everyday Types: Arrays).
"""
from __future__ import annotations

from . import js_types
from .solver import FALSE, TRUE, UNDEFINED
from .source import field_of, named_children
from .vocab import (LEVEL_ALIAS, LEVEL_OWN, LEVEL_TYPE, T_BOTH, T_FN, T_INDEX, T_NAMED, T_OBJECT, T_OPEN, T_PARAM,
                    T_QUERY, T_THIS, T_TUPLE, T_UNION, T_VALUES)

ARRAY = "Array"                       # the type `T[]` abbreviates (TypeScript handbook, Everyday Types: Arrays)
CONSTRUCT_MEMBER = "[[Construct]]"    # a construct signature (`new (...): T`): ECMA-262's [[Construct]] internal method
CALL_MEMBER = "[[Call]]"              # a call signature (`(...): T`): ECMA-262's [[Call]] internal method
OPEN = (T_OPEN,)
NOT_WRITTEN = ()                      # no type written: the values come from the flows
PREDICATES = ("type_predicate_annotation", "asserts_annotation")
LEVELS = (LEVEL_TYPE, LEVEL_OWN, LEVEL_ALIAS)


def open_params(r, node, level: int, owner: str) -> tuple:
    """The type parameters `node` declares, visible to what is written inside it (in both views: `js_types` erases
    them, this keeps them at `level`). Returns each one's default type (OPEN where none is written)."""
    params = field_of(node, "type_parameters") if node is not None else None
    items = [p for p in named_children(params) if p.type == "type_parameter"] if params is not None else []
    names = [r.src.text(field_of(p, "name")) for p in items]
    r.type_params.append(set(names))
    r.sig_scope.append({n: (T_PARAM, level, i) for i, n in enumerate(names)} if level in LEVELS else
                       dict.fromkeys(names, OPEN))
    return tuple(of(r, field_of(p, "value"), owner) or OPEN for p in items)


def open_names(r, names, kept) -> None:
    """Names a type introduces for what is written inside it alone (a mapped type's key, an `infer`): `kept` each."""
    r.type_params.append(set(names))
    r.sig_scope.append(dict.fromkeys(names, kept))


def close_params(r) -> None:
    r.type_params.pop()
    r.sig_scope.pop()


def of(r, annotation, owner: str) -> tuple:
    """A type annotation as a type tree; NOT_WRITTEN when there is none."""
    if annotation is None:
        return NOT_WRITTEN
    if annotation.type in PREDICATES:                    # `x is T`, `asserts x is T`: a boolean result
        return (T_VALUES, (TRUE, FALSE))
    return tree(r, js_types.bare(annotation), owner)


def tree(r, node, owner: str) -> tuple:
    if node is None:
        return OPEN
    kind = node.type
    if kind == "default_type":                           # a type parameter's default (`<T = U>`)
        return tree(r, js_types.bare((named_children(node) or [None])[0]), owner)
    if kind == "intersection_type" and not any(js_types.is_primitive(r, m) for m in js_types.members(r, node)):
        return (T_BOTH, tuple(tree(r, js_types.bare(m), owner) for m in named_children(node)))   # one value of each
    if kind in js_types.EITHER:
        return (T_UNION, tuple(tree(r, m, owner) for m in js_types.members(r, node)))
    if kind in js_types.CALLABLE_TYPES:
        return function_type(r, node, owner)
    if kind == "conditional_type":                       # either branch; what `infer` names has no value of its own
        infers = [r.src.text(field_of(n, "name") or named_children(n)[0]) for n in walk(field_of(node, "right"))
                  if n.type == "infer_type"]
        open_names(r, infers, OPEN)
        try:
            return (T_UNION, (of(r, field_of(node, "consequence"), owner), of(r, field_of(node, "alternative"), owner)))
        finally:
            close_params(r)
    if kind == "lookup_type":
        kids = named_children(node)
        return (T_INDEX, tree(r, js_types.bare(kids[0]), owner), tree(r, js_types.bare(kids[1]), owner)) \
            if len(kids) >= 2 else OPEN
    if kind == "this_type":
        return (T_THIS,)
    if kind == "type_query":
        inner = (named_children(node) or [None])[0]
        return (T_QUERY, " ".join(r.src.text(inner).split()), r.key, owner) if inner is not None else OPEN
    if kind in js_types.NAMES:
        text = " ".join(r.src.text(node).split())
        for scope in reversed(r.sig_scope):
            if text in scope:
                return scope[text]
    language = js_types.language_values(r, node)
    if language is not None:
        return (T_VALUES, tuple(language))
    if kind in js_types.NAMES:
        return (T_NAMED, " ".join(r.src.text(node).split()), (), r.key, owner)
    if kind == "generic_type":
        args = field_of(node, "type_arguments")
        return (T_NAMED, " ".join(r.src.text(field_of(node, "name")).split()),
                tuple(tree(r, js_types.bare(a), owner) for a in named_children(args)) if args is not None else (),
                r.key, owner)
    if kind == "array_type":
        return (T_NAMED, ARRAY, (tree(r, js_types.bare(named_children(node)[0]), owner),), r.key, owner)
    if kind == "tuple_type":
        return (T_TUPLE, tuple(tree(r, js_types.bare(element(e)), owner) for e in named_children(node)))
    if kind == "object_type":
        return (T_OBJECT, r.shape_class(node))
    return OPEN


def function_type(r, node, owner: str) -> tuple:
    """`(a: A, ...r: R[]) => T`, `new (...) => T`: each parameter's type, the result, the rest parameter's index, the
    declared `this`; its own type parameters have no value of their own."""
    open_params(r, node, -1, owner)
    try:
        params, rest, this = parameters(r, field_of(node, "parameters"), owner)
        result = of(r, field_of(node, "return_type") or field_of(node, "type"), owner)
        return (T_FN, params, result or OPEN, rest, this)
    finally:
        close_params(r)


def parameters(r, node, owner: str) -> tuple[tuple, int, tuple]:
    """A formal parameter list's types: (types, the rest parameter's index or -1, the declared `this` type)."""
    types, rest, this = [], -1, OPEN
    for p in named_children(node) if node is not None else []:
        pattern = field_of(p, "pattern") if p.type in ("required_parameter", "optional_parameter") else p
        if pattern is None or p.type == "comment":
            continue
        written = of(r, field_of(p, "type"), owner)
        if pattern.type == "this":
            this = written or OPEN
            continue
        if pattern.type == "rest_pattern":
            rest = len(types)
        types.append(written or OPEN)
    return tuple(types), rest, this


def element(node):
    """A tuple's element as a type: a rest element `...T[]` is T; an optional `T?` is T."""
    if node.type == "rest_type":
        inner = js_types.bare((named_children(node) or [None])[0])
        return named_children(inner)[0] if inner is not None and inner.type == "array_type" else inner
    return node


def result(r, node, owner: str) -> tuple:
    """A function's declared result: its return type (a construct signature's `type`)."""
    return of(r, field_of(node, "return_type") or (field_of(node, "type") if node.type == "construct_signature"
                                                   else None), owner)


def optional(written: tuple) -> tuple:
    """`p?: T`: T or undefined."""
    return (T_UNION, (written, (T_VALUES, (UNDEFINED,)))) if written else written


def walk(node):
    """`node` and everything under it."""
    todo = [node] if node is not None else []
    while todo:
        n = todo.pop()
        yield n
        todo += named_children(n)
