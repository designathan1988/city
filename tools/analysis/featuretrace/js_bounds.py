"""What a declared type admits, read from its syntax (log/trace-rules.md, S2; TypeScript handbook, Type Compatibility,
Everyday Types, Utility Types).

A bound is a tuple of admit tokens kept on the unit where the type is written, resolved by `bounds` there: `("any",)`
(any value: `any`, `unknown`, a type parameter, a type the tracer cannot read), `("number",)`, `("string",)`,
`("text", s)` (a string literal type), `("boolean",)`, `("bigint",)`, `("symbol",)`, `("null",)`, `("undefined",)`,
`("nonnullish",)` (`{}`, `Object`: any value but null and undefined), `("object",)` (`object`, `Record`, `Partial`,
`Pick`, `Omit`: any object), `("function",)`, `("constructor",)`, `("array",)` (arrays and tuples),
`("container", head)` (`Map`, `Set`, `Promise`), `("members", names)` (an object type: what has its required members),
`("named", text)` (a class, an interface or an alias of the program, or a host type: resolved where it is written).
A union or an intersection admits what any member admits; an empty tuple is no bound. A type of the language's
library is what it admits, never the tracer's form of its values (S2''): `Function` admits a class, `Iterable` what
has `[Symbol.iterator]`, `ArrayLike` what has `length`.
"""
from __future__ import annotations

from .ir_js import member_name
from .js_library import CANONICAL
from .js_types import NAMES, bare, literal_string, members
from .rules import ITERATOR_MEMBER
from .source import field_of, named_children

ANY = ("any",)
PREDEFINED = {"number": ("number",), "string": ("string",), "boolean": ("boolean",), "bigint": ("bigint",),
              "symbol": ("symbol",), "object": ("object",), "null": ("null",), "undefined": ("undefined",),
              "void": ("undefined",), "any": ANY, "unknown": ANY}
CONSTANTS = {"null": ("null",), "undefined": ("undefined",), "true": ("boolean",), "false": ("boolean",)}
SAME_TYPE = ("Readonly", "Required", "NonNullable", "Awaited")      # the type argument's bound
ANY_OBJECT = ("Partial", "Pick", "Omit", "Record")                  # some object of the type argument's members
NONNULLISH_NAMES = ("Object",)
PROMISE = "Promise"
ARRAY = ("array",)
# The library's types by what they admit (TypeScript lib.es5/es2015: a class is a `Function`; an iterable has
# `[Symbol.iterator]`, an async one `[Symbol.asyncIterator]`, an iterator `next` (the tracer's iterators and generator
# objects are arrays, T2), an array-like `length`, a thenable `then`).
LIBRARY = {"Function": [("constructor",)], "NewableFunction": [("constructor",)], "CallableFunction": [("function",)],
           "Array": [ARRAY], "ReadonlyArray": [ARRAY],
           **dict.fromkeys(("Iterable",), [ARRAY, ("members", (ITERATOR_MEMBER,))]),
           **dict.fromkeys(("AsyncIterable",), [ARRAY, ("members", ("[Symbol.asyncIterator]",))]),
           **dict.fromkeys(("Iterator", "IterableIterator", "IteratorObject", "Generator", "AsyncIterator",
                            "AsyncIterableIterator", "AsyncGenerator"), [ARRAY, ("members", ("next",))]),
           "ArrayLike": [ARRAY, ("members", ("length",))],
           "PromiseLike": [("container", PROMISE), ("members", ("then",))]}


def of(r, annotation, optional: bool = False) -> tuple:
    """The bound of a type annotation (`optional`: `undefined` admitted too, `x?: T`)."""
    if annotation is None:
        return ()
    tokens = []
    for node in members(r, annotation):
        tokens += one(r, node)
    if not tokens:
        return ()                                   # `never` alone: nothing to bound by
    if ANY in tokens:
        return (ANY,)
    return tuple(dict.fromkeys(tokens + ([("undefined",)] if optional else [])))


def result(r, annotation, function) -> tuple:
    """A function's declared result: what it returns (an async function: its `Promise`'s type argument, ECMAScript
    async functions); a type predicate a boolean; a generator's or an assertion's is not bounded."""
    if annotation is None or any(c.type == "*" for c in function.children):
        return ()
    if annotation.type == "type_predicate_annotation":
        return (("boolean",),)
    if annotation.type == "asserts_annotation":
        return ()
    if any(c.type == "async" for c in function.children):
        node = bare(annotation)
        if node is None or node.type != "generic_type" or r.src.text(field_of(node, "name")).strip() != PROMISE:
            return ()
        args = field_of(node, "type_arguments")
        inner = of(r, named_children(args)[0]) if args is not None and named_children(args) else ()
        return inner + (("container", PROMISE),) if inner and inner != (ANY,) else ()
    return of(r, annotation)


def one(r, node) -> list:
    """The tokens of one type (no union, no wrapper)."""
    kind = node.type
    if kind == "predefined_type":
        text = r.src.text(node).strip()
        return [] if text == "never" else [PREDEFINED.get(text, ANY)]
    if kind == "literal_type":
        inner = (named_children(node) or [None])[0]
        if inner is None:
            return [ANY]
        if inner.type in CONSTANTS:
            return [CONSTANTS[inner.type]]
        text = literal_string(r, node)
        if text is not None:
            return [("text", text)]
        return [("bigint",)] if r.src.text(inner).endswith("n") else [("number",)]
    if kind == "template_literal_type":
        return [("string",)]
    if kind == "index_type_query":                  # `keyof T`: a property key
        return [("string",), ("number",), ("symbol",)]
    if kind == "function_type":
        return [("function",)]
    if kind == "constructor_type":
        return [("constructor",)]
    if kind in ("array_type", "tuple_type"):
        return [("array",)]
    if kind == "object_type":
        return [object_type(r, node)]
    if kind in NAMES:
        text = " ".join(r.src.text(node).split())
        return [ANY] if text in r.type_scope() else named(text)
    if kind == "generic_type":
        return generic(r, node)
    return [ANY]                    # lookup, conditional, `typeof`, `this`, mapped, `infer`: the tracer cannot read it


def named(text: str) -> list:
    """The tokens of a type written by its name: a predefined type, a type of the language's library by what it
    admits, else a name resolved where it is written."""
    if text in PREDEFINED:
        return [PREDEFINED[text]]
    if text in LIBRARY:
        return list(LIBRARY[text])
    if text in NONNULLISH_NAMES:
        return [("nonnullish",)]
    head = CANONICAL.get(text)
    if head in ANY_OBJECT:
        return [("object",)]
    if head:
        return [("container", head)]
    return [("named", text)]


def generic(r, node) -> list:
    head = " ".join(r.src.text(field_of(node, "name")).split())
    args_node = field_of(node, "type_arguments")
    args = named_children(args_node) if args_node is not None else []
    if head in r.type_scope():
        return [ANY]
    if head in SAME_TYPE:
        return list(of(r, args[0])) or [ANY] if args else [ANY]
    if head in ANY_OBJECT:
        return [("object",)]
    return named(head)              # a class or interface of the program: its type arguments are erased


def object_type(r, node) -> tuple:
    """`{ a: T; f(): U }`: what has its required members; `{}` or an index signature alone: any value but null and
    undefined; call signatures alone: a function."""
    required, callable_ = [], False
    for member in named_children(node):
        if member.type in ("property_signature", "method_signature"):
            if not any(c.type == "?" for c in member.children) and field_of(member, "name") is not None:
                required.append(member_name(r.src, field_of(member, "name")))
        elif member.type in ("call_signature", "construct_signature"):
            callable_ = True
    if required:
        return ("members", tuple(sorted(set(required))))
    return ("function",) if callable_ else ("nonnullish",)
