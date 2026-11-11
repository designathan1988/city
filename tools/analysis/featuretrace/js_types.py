"""TypeScript annotations as the values they allow (log/trace-rules.md, "JavaScript and TypeScript values by the
languages' own rules", T1-T3; "What narrowing showed on the lab app", T1').

Types are erased: an annotation gives the values the type admits. A name gives the class it names (the typer resolves
it, `TypeNames.canon`), or the values a type alias admits (`ALIAS`, read where the alias is declared); each member of a
union or an intersection gives its own; `typeof x` gives the value of x; the language's containers give a generic text
of the ECMAScript built-ins (`js_library`) whose index, iteration and element calls give the element types. A primitive
gives the language's value of its kind, `boolean` both constants, a literal type its value, an object type the
interface it writes (a class of its own), a function type a function the program does not name, `T['k']` the member
k of T, a conditional type either branch. A type parameter in scope and `never` give nothing: the values come from the
flows.
"""
from __future__ import annotations

from .js_library import ALIAS_LOCAL, CANONICAL, UNWRAP
from .solver import BIGINT, ENGINE, FALSE, FUNCTION, NULL, NUMBER, OBJECT, STRING, SYMBOL, TEXT, TRUE, UNDEFINED
from .source import field_of, named_children, string_value

WRAPPERS = ("type_annotation", "opting_type_annotation", "omitting_type_annotation", "adding_type_annotation",
            "parenthesized_type", "readonly_type", "optional_type")
EITHER = ("union_type", "intersection_type")
NAMES = ("type_identifier", "nested_type_identifier", "identifier")
UNKNOWN = "?"                     # a type argument that names nothing with a runtime value
# The language's values each predefined type admits (TypeScript handbook, Everyday Types; T1')
PREDEFINED = {"number": (NUMBER,), "string": (STRING,), "bigint": (BIGINT,), "symbol": (SYMBOL,),
              "boolean": (TRUE, FALSE), "object": (OBJECT,), "any": (ENGINE,), "unknown": (ENGINE,),
              "void": (UNDEFINED,), "undefined": (UNDEFINED,), "null": (NULL,), "never": ()}
CONSTANTS = {"true": TRUE, "false": FALSE, "null": NULL, "undefined": UNDEFINED}
CALLABLE_TYPES = ("function_type", "constructor_type")
SEPARATORS = frozenset("<>()[]{},|")       # what a container's text uses to write its arguments
KEYS = (STRING, NUMBER, SYMBOL)            # `keyof T`: a property key of T (strings, numbers, symbols)


def values(r, annotation, unit) -> list[tuple]:
    """The value trees an annotation allows, read in `unit` (`typeof x` is x's value there)."""
    out = []
    for part in members(annotation):
        out += admitted(r, part, unit)
    return out


def admitted(r, node, unit) -> list[tuple]:
    """The value trees one type (no union, no wrapper) admits."""
    kind = node.type
    if kind == "type_query":
        expr = (named_children(node) or [None])[0]
        return [r.ir(expr, unit)] if expr is not None else []
    if kind == "lookup_type":                 # `T['k']`: T's member k; `T[number]`: T's elements
        kids = named_children(node)
        if len(kids) < 2:
            return []
        key = literal_string(r, kids[1])
        index = ("cast", NUMBER) if key is None else None
        return [("member", t, key, False) if key is not None else ("index", t, index)
                for t in values(r, kids[0], unit)]
    if kind == "conditional_type":            # `A extends B ? X : Y`: X or Y
        return values(r, field_of(node, "consequence"), unit) + values(r, field_of(node, "alternative"), unit)
    if kind == "this_type":
        return [("self",)]
    language = language_values(r, node)
    if language is not None:
        return [("cast", v) for v in language]
    text = written(r, node, unit)
    return [("cast", text)] if text and text != UNKNOWN else []


def language_values(r, node) -> tuple | None:
    """The language's values a type admits by itself (T1'): a primitive, a literal, `keyof`, a function type; None for
    a type that names something (a class, an alias, a container, a type parameter) or writes an interface."""
    kind = node.type
    if kind == "predefined_type" or kind in NAMES and r.src.text(node) == "bigint" and "bigint" not in r.type_scope():
        return PREDEFINED.get(r.src.text(node).strip(), (ENGINE,))
    if kind == "literal_type":
        inner = (named_children(node) or [None])[0]
        if inner is None:
            return ()
        if inner.type in CONSTANTS:
            return (CONSTANTS[inner.type],)
        text = literal_string(r, node)
        if text is not None:
            return (TEXT + text,)
        return (BIGINT,) if r.src.text(inner).endswith("n") else (NUMBER,)   # a number or BigInt literal type
    if kind == "template_literal_type":
        return (STRING,)
    if kind == "index_type_query":
        return KEYS
    if kind in CALLABLE_TYPES:
        return (FUNCTION,)
    if kind in ("infer_type", "existential_type"):
        return ()
    return None


def literal_string(r, node) -> str | None:
    """A string literal type's text (`'k'`), else None."""
    while node is not None and node.type == "literal_type":
        node = (named_children(node) or [None])[0]
    return string_value(r.src, node) if node is not None and node.type == "string" else None


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


def written(r, node, unit) -> str:
    """One type as the text the typer resolves: a name, a built-in container of the element types written, the
    language's values (a string written with a container's separators is any string), the class an object type
    writes, a marker of the values a type that is none of these admits (`r.type_holder`); UNKNOWN for no value."""
    node = bare(node)
    if node is None:
        return UNKNOWN
    kind = node.type
    if kind in EITHER:
        parts = [t for t in (written(r, m, unit) for m in members(node)) if t != UNKNOWN]
        return " | ".join(parts) if parts else UNKNOWN
    language = language_values(r, node)
    if language is not None:
        parts = [STRING if v.startswith(TEXT) and SEPARATORS & set(v) else v for v in language]
        return " | ".join(dict.fromkeys(parts)) if parts else UNKNOWN
    if kind in NAMES:
        text = " ".join(r.src.text(node).split())
        return UNKNOWN if text in r.type_scope() else text
    if kind == "array_type":
        return f"Array<{written(r, named_children(node)[0], unit)}>"
    if kind == "tuple_type":
        parts = [written(r, rest_element(e), unit) for e in named_children(node)]
        parts = [t for t in parts if t != UNKNOWN]
        return f"Array<{' | '.join(parts)}>" if parts else UNKNOWN
    if kind == "generic_type":
        return generic(r, node, unit)
    if kind == "object_type":
        return "#" + r.shape_class(node)            # an interface without a name (T1')
    if kind in ("lookup_type", "conditional_type", "type_query", "this_type"):
        return ALIAS_LOCAL + str(r.type_holder(node, unit))
    return UNKNOWN                # a mapped type writes no member the tracer can name: its values come from the flows


def rest_element(node):
    """A tuple's `...T[]` element: the elements of T[]."""
    if node.type == "rest_type":
        inner = bare((named_children(node) or [None])[0])
        if inner is not None and inner.type == "array_type":
            return named_children(inner)[0]
        return inner
    return node


def generic(r, node, unit) -> str:
    head = " ".join(r.src.text(field_of(node, "name")).split())
    args_node = field_of(node, "type_arguments")
    args = [written(r, a, unit) for a in named_children(args_node)] if args_node is not None else []
    if head in UNWRAP:
        return args[0] if args else UNKNOWN
    if head in CANONICAL:
        return f"{CANONICAL[head]}<{', '.join(args)}>"
    return UNKNOWN if head in r.type_scope() else head      # a class of the program's: its type arguments are erased


def predicate(r, annotation, unit) -> tuple:
    """A return type `x is T` or `asserts x is T` (TypeScript handbook, Narrowing: type predicates, assertion
    functions): (x's parameter index, T's value trees, whether it asserts); () for any other (`this is T`, `asserts x`
    included)."""
    if annotation is None or annotation.type not in ("type_predicate_annotation", "asserts_annotation"):
        return ()
    asserts = annotation.type == "asserts_annotation"
    holder = next((c for c in named_children(annotation) if c.type == "asserts"), None) if asserts else annotation
    found = next((c for c in named_children(holder) if c.type == "type_predicate"), None) if holder else None
    kids = named_children(found) if found is not None else []
    if len(kids) < 2 or kids[0].type != "identifier" or r.src.text(kids[0]) not in unit.params:
        return ()
    return unit.params.index(r.src.text(kids[0])), tuple(values(r, kids[-1], unit)), asserts


def parameters(r, node) -> set:
    """The type parameters a generic declaration introduces (`<T, U extends X>`)."""
    params = field_of(node, "type_parameters") if node is not None else None
    if params is None:
        return set()
    return {r.src.text(field_of(p, "name")) for p in named_children(params) if p.type == "type_parameter"}
