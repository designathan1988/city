"""What a JavaScript / TypeScript condition proves about names (log/trace-rules.md, "Narrowing at branch conditions":
G1; TypeScript handbook, Narrowing; MDN `typeof`, `instanceof`, `in`, falsy values).

`facts(r, node, unit)` gives the facts that hold when the condition is true and when it is false, each
`(name_tree, guard, positive)` (`narrowing`). A name is a local or a parameter (never a property path). Guards: the
name's truthiness `("truthy",)`, a property's truthiness `("member", p)` (`x.p`, `x?.p`), `("typeof", t)`,
`("instance", class_tree)`, `("array",)` (the language's `Array.isArray`), `("has", p)` (`"p" in x`),
`("equals", value)` (`===`, `!==` with a constant of the language or a string), `("nullish",)` (`== null`,
`!= null`, `== undefined`), `("member-equals", p, value)` (`x.p === v`, a `switch` case), `("predicate", callee_tree,
index)` (a call of a function declared `x is T`) and, after a call statement, `("asserts", callee_tree, index)` (one
declared `asserts x is T`): only TypeScript declares those. `!` swaps; `&&` keeps both when true, `||` both negations
when false.
"""
from __future__ import annotations

from . import scopes
from .solver import FALSE, NULL, TEXT, TRUE, UNDEFINED
from .source import field_of, named_children, string_value

CONSTANTS = {"null": NULL, "undefined": UNDEFINED, "true": TRUE, "false": FALSE}
EQUAL, DIFFERENT = ("===", "=="), ("!==", "!=")
INNER = ("parenthesized_expression", "non_null_expression", "expression_statement")
TYPESCRIPT = (".ts", ".tsx", ".mts", ".cts")      # the files that can declare a type predicate or an assertion


def typescript(r) -> bool:
    return r.src.path.suffix.lower() in TYPESCRIPT


def _inner(node):
    while node is not None and node.type in INNER:
        node = next((c for c in named_children(node) if c.type != "comment"), None)
    return node


def name_of(r, node, unit):
    """The renamed tree of a local or parameter of the function written alone: never a property path, a module's
    variable or a name a closure shares with its enclosing function (any call may write those, R2)."""
    node = _inner(node)
    if node is None or node.type != "identifier" or r.src.text(node) == "undefined":
        return None
    tree = scopes.renamed(r, ("name", r.src.text(node)))
    return tree if tree[1] in unit.types and tree[1] not in unit.captured else None


def constant(r, node) -> str | None:
    """A constant of the language or a string literal, as the value it is (V1, G7)."""
    node = _inner(node)
    if node is None:
        return None
    text = r.src.text(node)
    if node.type in ("null", "undefined", "true", "false") or node.type == "identifier" and text == "undefined":
        return CONSTANTS[text]
    if node.type in ("string", "template_string"):
        value = string_value(r.src, node)
        return TEXT + value if value is not None else None
    return None


def _both(facts: list) -> tuple[list, list]:
    """A fact and its negation: (when true, when false)."""
    return facts, [(name, guard, not positive) for name, guard, positive in facts]


def _member(r, node, unit):
    """`x.p` / `x?.p` on a name: (name tree, p)."""
    node = _inner(node)
    if node is None or node.type != "member_expression":
        return None, None
    name = name_of(r, field_of(node, "object"), unit)
    prop = field_of(node, "property")
    return (name, r.src.text(prop)) if name is not None and prop is not None else (None, None)


def _comparison(r, op: str, left, right, unit) -> list:
    for a, b in ((left, right), (right, left)):
        a_in = _inner(a)
        if a_in is not None and a_in.type == "unary_expression" and r.src.text(field_of(a_in, "operator")) == "typeof":
            name, kind = name_of(r, field_of(a_in, "argument"), unit), constant(r, b)
            if name is not None and kind is not None and kind.startswith(TEXT):
                return [(name, ("typeof", kind[len(TEXT):]), True)]
        value = constant(r, b)
        if value is None:
            continue
        name = name_of(r, a, unit)
        if name is not None and op in ("==", "!=") and value in (NULL, UNDEFINED):
            return [(name, ("nullish",), True)]
        if name is not None and op in ("===", "!=="):
            return [(name, ("equals", value), True)]
        holder, prop = _member(r, a, unit)
        if holder is not None and op in ("===", "!=="):
            return [(holder, ("member-equals", prop, value), True)]
    return []


def facts(r, node, unit) -> tuple[list, list]:
    """The facts a condition proves: (when it is true, when it is false)."""
    node = _inner(node)
    if node is None:
        return [], []
    kind = node.type
    if kind == "unary_expression" and r.src.text(field_of(node, "operator")) == "!":
        when_true, when_false = facts(r, field_of(node, "argument"), unit)
        return when_false, when_true
    if kind == "binary_expression":
        op = r.src.text(field_of(node, "operator")).strip()
        left, right = field_of(node, "left"), field_of(node, "right")
        if op == "&&":
            return facts(r, left, unit)[0] + facts(r, right, unit)[0], []
        if op == "||":
            return [], facts(r, left, unit)[1] + facts(r, right, unit)[1]
        if op in EQUAL + DIFFERENT:
            found = _both(_comparison(r, op, left, right, unit))
            return found if op in EQUAL else (found[1], found[0])
        if op == "instanceof":
            name = name_of(r, left, unit)
            return _both([(name, ("instance", r.ir(right, unit)), True)] if name is not None else [])
        if op == "in":
            name, prop = name_of(r, right, unit), constant(r, left)
            usable = name is not None and prop is not None and prop.startswith(TEXT)
            return _both([(name, ("has", prop[len(TEXT):]), True)] if usable else [])
        return [], []
    if kind == "call_expression":
        return _call(r, node, unit)
    holder, prop = _member(r, node, unit)
    if holder is not None:
        return _both([(holder, ("member", prop), True)])
    name = name_of(r, node, unit)
    return _both([(name, ("truthy",), True)] if name is not None else [])


def _call(r, node, unit) -> tuple[list, list]:
    """`Array.isArray(x)` (the language's `Array`); any other call: a type predicate on each name passed, decided by
    the callee's declaration (`x is T`) when its values are known."""
    fn, args = field_of(node, "function"), field_of(node, "arguments")
    argv = [a for a in named_children(args) if a.type != "comment"] if args is not None else []
    if fn is not None and r.src.text(fn) == "Array.isArray" and "Array" not in unit.types and argv:
        name = name_of(r, argv[0], unit)
        return _both([(name, ("array",), True)] if name is not None else [])
    return _both(_declared(r, fn, argv, unit, "predicate"))


def _declared(r, fn, argv: list, unit, kind: str) -> list:
    """A call that a function declared `x is T` or `asserts x is T` answers: a fact on each name passed, decided by the
    callee's declaration when its values are known (TypeScript only)."""
    if fn is None or fn.type == "import" or not typescript(r):
        return []
    callee = r.ir(fn, unit)
    return [(name, (kind, callee, i), True) for i, name in enumerate(name_of(r, a, unit) for a in argv)
            if name is not None]


def asserted(r, st, unit) -> list:
    """A statement that calls a function: if it is an assertion function, what follows holds its fact."""
    call = _inner(st)
    if call is None or call.type != "call_expression":
        return []
    args = field_of(call, "arguments")
    argv = [a for a in named_children(args) if a.type != "comment"] if args is not None else []
    return _declared(r, field_of(call, "function"), argv, unit, "asserts")


def case_facts(r, value, test, unit) -> list:
    """What a `switch` case proves when only its own test enters it: the discriminant equal to its literal."""
    return _comparison(r, "===", value, test, unit)
