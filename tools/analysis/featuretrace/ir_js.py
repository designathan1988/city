"""JavaScript and TypeScript expressions to IR, one builder per expression construct (log/trace-rules.md, JavaScript and
TypeScript rows; J1-J10).

`this` is the class's instance (`("self",)`), `super` its base. A function or arrow is a callable of its unit; a class
expression and an object literal are values of their own class (`("cast", "#<key>")`). `a ?? b`, `a || b`, `a && b`
and the ternary hold either side; `as`/`satisfies`/`!`/`<T>` keep the value with the type named; `x.call(...)`,
`x.apply(...)`, `x.bind(...)` are member calls, decided by x's values (J2'': Function.prototype's on a function, the
object's own method on an object); `import("m")` is the module's namespace; `await x` what x resolves to.
"""
from __future__ import annotations

from . import js_guards, js_types, narrowing, scopes
from .ir import LITERAL, name, union
from .source import field_of, named_children, string_value

CLASS_VALUE = "#"                 # the typer's prefix of a class of the program (solver.CLASS)
EITHER = ("??", "||", "&&")       # value-preserving operators (MDN Logical OR, Nullish coalescing)
TESTS = ("==", "!=", "===", "!==", "<", ">", "<=", ">=", "in", "instanceof")
WORD_UNARY = {"typeof": "string", "void": "undefined", "delete": "boolean", "!": "boolean"}   # what each gives (MDN)
LITERALS = {"true": "true", "false": "false", "null": "null", "undefined": "undefined",
            "regex": "RegExp", "template_string": "string", "update_expression": "numeric", "meta_property": ""}
FUNCTIONS = ("function_expression", "arrow_function", "generator_function", "function_declaration",
             "generator_function_declaration")


def build(r, node, unit) -> tuple:
    """The IR of an expression node, read in `unit` (the reader `r` makes lambda units and object-literal classes)."""
    if node is None:
        return LITERAL
    builder = BUILDERS.get(node.type)
    if builder is not None:
        return builder(r, node, unit)
    if node.type in LITERALS:
        return ("literal", LITERALS[node.type])
    return LITERAL


def args_of(r, node, unit) -> tuple:
    """A call's arguments; a spread argument stands for the elements it spreads (J5)."""
    if node is None or node.type != "arguments":
        return (build(r, node, unit),) if node is not None else ()
    return tuple(build(r, a, unit) for a in named_children(node))


def member_name(src, node) -> str:
    """A property as the program names it: `x`, `#x`, a string key's text, `[Symbol.iterator]`."""
    if node.type == "computed_property_name":
        inner = (named_children(node) or [None])[0]
        text = string_value(src, inner) if inner is not None else None
        return text if text is not None else "[" + " ".join(src.text(inner).split()) + "]"
    text = string_value(src, node)
    return text if text is not None else src.text(node)


def _identifier(r, node, unit):
    text = r.src.text(node)
    return ("literal", "undefined") if text == "undefined" else name(text)


def _member(r, node, unit):
    obj = field_of(node, "object")
    if obj is not None and obj.type == "import":
        return LITERAL                                        # import.meta
    return ("member", build(r, obj, unit), member_name(r.src, field_of(node, "property")), False)


def _subscript(r, node, unit):
    index = field_of(node, "index")
    key = string_value(r.src, index)
    base = build(r, field_of(node, "object"), unit)
    return ("member", base, key, False) if key is not None else ("index", base, build(r, index, unit))


def _call(r, node, unit):
    fn, args = field_of(node, "function"), field_of(node, "arguments")
    if fn.type == "import":
        spec = string_value(r.src, (named_children(args) or [None])[0]) if args is not None else None
        return ("module", spec) if spec is not None else LITERAL
    if fn.type == "super":
        return ("call", ("member", ("super",), "constructor", False), args_of(r, args, unit))
    return ("call", build(r, fn, unit), args_of(r, args, unit))


def _new(r, node, unit):
    """`new X(...)`: X is an expression; its value decides what is made (N1: a class's instance, what a function
    called as a constructor returns, the host's object)."""
    return ("construct", build(r, field_of(node, "constructor"), unit), node.start_byte,     # its allocation site,
            args_of(r, field_of(node, "arguments"), unit))                                  # its arguments


def _jsx(r, node, unit):
    """A JSX element: a call of the JSX factory with its tag and its props; its value is what that gives (`js_jsx`)."""
    from . import js_jsx
    return js_jsx.tree(r, node, unit, build)


def guarded(r, facts: list, node, unit):
    """An operand's tree read where facts hold (R1): its names as the locals they name, through the facts."""
    return narrowing.wrap(scopes.renamed(r, build(r, node, unit)), narrowing.region_facts(r, facts, [node], "js"))


def _binary(r, node, unit):
    op = r.src.text(field_of(node, "operator")).strip()
    if op in EITHER:                               # each operand where it is the value (`js_guards.operand_facts`)
        (guard, positive), other = js_guards.operand_facts(r, op, field_of(node, "left"), unit)
        own = ("narrow", guarded(r, [], field_of(node, "left"), unit), guard, positive)
        return union(own, guarded(r, other, field_of(node, "right"), unit))
    left, right = build(r, field_of(node, "left"), unit), build(r, field_of(node, "right"), unit)
    if op in TESTS:
        return ("literal", "boolean")
    return ("op", op, left, right)


def _unary(r, node, unit):
    op = r.src.text(field_of(node, "operator")).strip()
    if op in WORD_UNARY:
        return ("literal", WORD_UNARY[op])
    return ("unary", op, build(r, field_of(node, "argument"), unit))


def _number(r, node, unit):
    """A numeric literal: a BigInt when written with the `n` suffix (MDN BigInt)."""
    return ("literal", "bigint" if r.src.text(node).endswith("n") else "number")


def _augmented(r, node, unit):
    """`x op= y`: `||=`, `&&=`, `??=` keep either side; the others give their operator's value (O1)."""
    op = r.src.text(field_of(node, "operator")).strip()[:-1]
    left, right = build(r, field_of(node, "left"), unit), build(r, field_of(node, "right"), unit)
    return union(left, right) if op in EITHER else ("op", op, left, right)


def _typed(r, node, unit):
    """`x as T`, `<T>x`: the value itself, and what the type asserted allows (erased, J8, T1-T2); `x satisfies T`: the
    value alone (TypeScript 4.9: it checks the value against T without changing its type)."""
    kids = named_children(node)
    value = build(r, kids[0], unit) if kids else LITERAL
    if node.type == "satisfies_expression":
        return value
    types = [k for k in kids[1:] if k.type not in ("comment",)]
    for tree in js_types.values(r, types[0], unit) if types else []:
        value = union(value, tree)
    return value


def _inner(r, node, unit):
    kids = [k for k in named_children(node) if k.type != "comment"]
    return build(r, kids[-1] if node.type == "sequence_expression" else kids[0], unit) if kids else LITERAL


def _ternary(r, node, unit):
    when_true, when_false = js_guards.facts(r, field_of(node, "condition"), unit)
    return union(guarded(r, when_true, field_of(node, "consequence"), unit),
                 guarded(r, when_false, field_of(node, "alternative"), unit))


def _array(r, node, unit):
    values = []
    for item in named_children(node):
        if item.type == "spread_element":
            values.append(("elements", build(r, (named_children(item) or [None])[0], unit)))
        elif item.type != "comment":
            values.append(build(r, item, unit))
    return ("collection", tuple(values), node.start_byte)                # its allocation site


def _string(r, node, unit):
    text = string_value(r.src, node)
    return ("strname", text) if text is not None else ("literal", "string")


BUILDERS = {
    "identifier": _identifier,
    "shorthand_property_identifier": _identifier,
    "this": lambda r, n, u: ("self",),
    "super": lambda r, n, u: ("super",),
    "member_expression": _member,
    "subscript_expression": _subscript,
    "call_expression": _call,
    "new_expression": _new,
    "binary_expression": _binary,
    "unary_expression": _unary,
    "as_expression": _typed,
    "satisfies_expression": _typed,
    "type_assertion": _typed,
    "non_null_expression": _inner,
    "instantiation_expression": _inner,
    "parenthesized_expression": _inner,
    "sequence_expression": _inner,
    "ternary_expression": _ternary,
    "assignment_expression": lambda r, n, u: build(r, field_of(n, "right"), u),
    "augmented_assignment_expression": _augmented,
    "number": _number,
    "await_expression": lambda r, n, u: ("await", _inner(r, n, u)),
    "yield_expression": lambda r, n, u: LITERAL,
    "spread_element": lambda r, n, u: ("elements", _inner(r, n, u)),
    "array": _array,
    "object": lambda r, n, u: ("cast", CLASS_VALUE + r.object_class(n, u)),
    "class": lambda r, n, u: ("cast", CLASS_VALUE + r.class_value(n, u)),
    "string": _string,
    "template_string": _string,
    "jsx_element": lambda r, n, u: _jsx(r, n, u),
    "jsx_self_closing_element": lambda r, n, u: _jsx(r, n, u),
    **{kind: (lambda r, n, u: ("callable", r.lambda_id(n, u))) for kind in FUNCTIONS},
}
