"""JavaScript / TypeScript expressions and calls (log/trace-rules.md, JavaScript and TypeScript rows; J2, J3, J5, J6, J10).

Every name read, member read or written, call and `new` becomes a reference with its receiver and arguments kept as
trees. The language's own calls are references too, marked implicit (silent when nothing of the program answers):
`[Symbol.iterator]` for spread and `yield*`, `then` for `await`, `valueOf`/`toString`/`[Symbol.toPrimitive]` for
arithmetic, comparison and template substitutions (J5). Events by type: subscribing (`addEventListener`, `on`, ...)
and firing (`dispatchEvent`, `emit`, `postMessage`) (J6). A capitalized JSX tag is a component handed to the JSX
runtime (J10). `f.call(t, ...)` / `f.apply(t, a)` call f with the arguments after t (J2).
"""
from __future__ import annotations

from . import js_body
from .cpp_implicit import IMPLICIT
from .ir_js import args_of, member_name
from .rules import (CALL_FORMS, COERCION_MEMBERS, CONSTRUCTOR, EVENT_FIRE, EVENT_OBJECTS, EVENT_SUBSCRIBE,
                    ITERATOR_MEMBER, MESSAGE_FIRE, THEN_MEMBER)
from .source import field_of, named_children, string_value

COERCING = ("+", "-", "*", "/", "%", "**", "<", ">", "<=", ">=", "==", "!=", "+=", "-=", "*=", "/=")
PLAIN = ("number", "string", "template_string", "true", "false", "null", "undefined", "regex")
FUNCTION_VALUES = ("arrow_function", "function_expression", "generator_function")
SKIP = ("comment", "type_identifier", "predefined_type", "type_annotation", "type_arguments", "type_parameters",
        "property_identifier", "private_property_identifier", "statement_identifier", "regex", "number", "string",
        "template_string", "true", "false", "null", "undefined", "this", "super", "import", "meta_property",
        "jsx_text", "html_character_reference", "hash_bang_line")


def expression(r, node, unit, cond) -> None:
    if node is None or node.type in SKIP and node.type != "template_string":
        return
    kind = node.type
    if kind in ("identifier", "shorthand_property_identifier"):
        if r.src.text(node) != "undefined":
            r.add(unit, "read", r.src.text(node), node, cond)
    elif kind == "member_expression":
        obj = field_of(node, "object")
        if obj is not None and obj.type == "import":
            return
        expression(r, obj, unit, cond)
        r.add(unit, "read", member_name(r.src, field_of(node, "property")), node, cond, base=r.ir(obj, unit))
    elif kind == "subscript_expression":
        obj, index = field_of(node, "object"), field_of(node, "index")
        expression(r, obj, unit, cond)
        key = string_value(r.src, index)
        if key is not None:
            r.add(unit, "read", key, node, cond, base=r.ir(obj, unit))
        else:
            expression(r, index, unit, cond)
    elif kind == "call_expression":
        call(r, node, unit, cond)
    elif kind == "new_expression":
        ctor, args = field_of(node, "constructor"), field_of(node, "arguments")
        if ctor is not None and ctor.type not in ("identifier", "member_expression"):
            expression(r, ctor, unit, cond)
        r.add(unit, "new", "", node, cond, base=r.ir(ctor, unit), args=args_of(r, args, unit))
        arguments(r, args, unit, cond)
    elif kind == "assignment_expression":
        left, right = field_of(node, "left"), field_of(node, "right")
        js_body.target(r, left, unit, cond, r.ir(right, unit))
        expression(r, right, unit, cond)
    elif kind == "augmented_assignment_expression":
        left, right = field_of(node, "left"), field_of(node, "right")
        expression(r, left, unit, cond)
        js_body.target(r, left, unit, cond, r.ir(node, unit))
        expression(r, right, unit, cond)
        coerce(r, node, (left,), unit, cond)
    elif kind == "update_expression":
        argument = field_of(node, "argument")
        expression(r, argument, unit, cond)
        js_body.target(r, argument, unit, cond, None)
        coerce(r, node, (argument,), unit, cond)
    elif kind == "binary_expression":
        left, right = field_of(node, "left"), field_of(node, "right")
        expression(r, left, unit, cond)
        expression(r, right, unit, cond)
        if r.src.text(field_of(node, "operator")).strip() in COERCING:
            coerce(r, node, (left, right), unit, cond)
    elif kind == "unary_expression":
        argument = field_of(node, "argument")
        expression(r, argument, unit, cond)
        if r.src.text(field_of(node, "operator")).strip() in ("+", "-", "~"):
            coerce(r, node, (argument,), unit, cond)
    elif kind == "template_string":
        for part in named_children(node):
            if part.type == "template_substitution":
                inner = (named_children(part) or [None])[0]
                expression(r, inner, unit, cond)
                coerce(r, part, (inner,), unit, cond)
    elif kind == "await_expression":
        inner = (named_children(node) or [None])[0]
        expression(r, inner, unit, cond)
        implicit(r, node, THEN_MEMBER, inner, unit, cond)          # a thenable's `then` (J6)
    elif kind in ("spread_element", "yield_expression"):
        inner = (named_children(node) or [None])[0]
        expression(r, inner, unit, cond)
        if kind == "spread_element" or any(c.type == "*" for c in node.children):
            implicit(r, node, ITERATOR_MEMBER, inner, unit, cond)
    elif kind in FUNCTION_VALUES:
        r.add(unit, "lambda", "", node, cond, target=r.lambda_id(node, unit))
    elif kind == "class":
        r.class_value(node, unit)
    elif kind == "object":
        r.object_class(node, unit)
    elif kind in ("jsx_element", "jsx_self_closing_element"):
        jsx(r, node, unit, cond)
    else:
        for child in named_children(node):
            expression(r, child, unit, cond)


def implicit(r, node, member: str, operand, unit, cond) -> None:
    """A call the language makes on a value (J5): silent when no class of the program answers it."""
    if operand is not None and operand.type not in PLAIN:
        r.add(unit, "call", member, node, cond, base=r.ir(operand, unit), text=IMPLICIT, argc=0)


def coerce(r, node, operands, unit, cond) -> None:
    """A value turned into a primitive: its `[Symbol.toPrimitive]`, `valueOf` or `toString` (J5)."""
    for operand in operands:
        for member in COERCION_MEMBERS:
            implicit(r, node, member, operand, unit, cond)


def arguments(r, args, unit, cond) -> None:
    if args is None:
        return
    for a in (named_children(args) if args.type == "arguments" else [args]):
        expression(r, a, unit, cond)


def call(r, node, unit, cond) -> None:
    fn, args = field_of(node, "function"), field_of(node, "arguments")
    argv = (named_children(args) if args.type == "arguments" else [args]) if args is not None else []
    trees = args_of(r, args, unit)
    if fn.type == "import":                                     # import("m"): the module runs (J3)
        spec = string_value(r.src, argv[0]) if argv else None
        if spec is not None:
            r.add(unit, "import", "", node, cond, text=spec)
    elif fn.type == "super":
        r.add(unit, "call", CONSTRUCTOR, node, cond, base=("super",), args=trees, argc=len(argv))
    elif fn.type == "identifier":
        name = r.src.text(fn)
        spec = string_value(r.src, argv[0]) if name == "require" and argv else None
        if spec is not None:
            r.add(unit, "import", "", node, cond, text=spec)    # CommonJS require("m")
        else:
            r.add(unit, "call", name, node, cond, args=trees, argc=len(argv))
    elif fn.type == "member_expression":
        obj = field_of(fn, "object")
        method = member_name(r.src, field_of(fn, "property"))
        expression(r, obj, unit, cond)
        base = r.ir(obj, unit)
        if method in CALL_FORMS:
            r.add(unit, "invoke", "", node, cond, base=base, args=r.ir(node, unit)[2])
        else:
            r.add(unit, "call", method, node, cond, base=base, args=trees, argc=len(argv))
            events(r, node, method, base, argv, unit, cond)
    else:
        expression(r, fn, unit, cond)
        r.add(unit, "invoke", "", node, cond, base=r.ir(fn, unit), args=trees)
    arguments(r, args, unit, cond)


def events(r, node, method: str, base, argv, unit, cond) -> None:
    """Events by type (J6): a subscription links the handler to the event; firing links to the event."""
    if method in EVENT_SUBSCRIBE:
        at_type, at_handler = EVENT_SUBSCRIBE[method]
        if len(argv) > at_handler:
            kind = string_value(r.src, argv[at_type])
            r.add(unit, "listen", kind or "", node, cond, base=base, handler=r.ir(argv[at_handler], unit),
                  args=(r.ir(argv[at_type], unit),))
    elif method in EVENT_FIRE and len(argv) > EVENT_FIRE[method]:
        event = argv[EVENT_FIRE[method]]
        kind = event_type(r, event)
        payload = (r.ir(event, unit),) if method == "dispatchEvent" else tuple(r.ir(a, unit) for a in argv[1:])
        r.add(unit, "fire", kind or "", node, cond, base=base, handler=None if kind else r.ir(event, unit),
              args=payload)
    elif method in MESSAGE_FIRE:
        r.add(unit, "fire", MESSAGE_FIRE[method], node, cond, base=base, args=(("literal", ""),))


def event_type(r, event) -> str | None:
    """The type of an event written at the call: a string, `{type: "t"}`, `new Event("t")`."""
    text = string_value(r.src, event)
    if text is not None:
        return text
    if event.type == "object":
        for pair in (c for c in named_children(event) if c.type == "pair"):
            if member_name(r.src, field_of(pair, "key")) == "type":
                return string_value(r.src, field_of(pair, "value"))
    if event.type == "new_expression":
        ctor = r.src.text(field_of(event, "constructor")).split(".")[-1]
        args = field_of(event, "arguments")
        first = (named_children(args) or [None])[0] if args is not None else None
        if ctor in EVENT_OBJECTS and first is not None:
            return string_value(r.src, first)
    return None


def jsx(r, node, unit, cond) -> None:
    """A JSX element: a capitalized tag (or a member) is a component handed to the JSX runtime with its props (J10);
    the attributes' and children's expressions are read."""
    opening = field_of(node, "open_tag") if node.type == "jsx_element" else node
    tag = field_of(opening, "name") if opening is not None else None
    if tag is not None:
        text = r.src.text(tag)
        if tag.type in ("member_expression", "nested_identifier") or text[:1].isupper():
            r.add(unit, "jsx", text, node, cond, base=r.ir(tag, unit), args=(("literal", ""),))
    for part in named_children(node):
        if part is opening or part.type in ("jsx_closing_element", "jsx_text"):
            if part is opening:
                for attribute in named_children(opening):
                    for value in named_children(attribute):
                        expression(r, value, unit, cond)
            continue
        expression(r, part, unit, cond)
