"""JSX elements (log/trace-rules.md J10; the JSX specification, facebook/jsx; TypeScript handbook, JSX): a capitalized
tag, or a member, is a component handed to the JSX runtime with its props — an object of its attributes (a string
attribute its text, an expression its value, a name alone `true`, a spread its object's members) whose children are kept
under the tracer's own marker (`vocab.JSX_CHILDREN`), for the analysis to name as the runtime's declarations name them;
the attributes' and the children's expressions are read where the element is written.
"""
from __future__ import annotations

from . import ir_js, js_class
from .model import ClassFact
from .source import field_of, named_children, string_value
from .vocab import JSX, JSX_CHILDREN, LITERAL, STRNAME

PLAIN = ("jsx_closing_element", "jsx_text")       # parts of an element that are no expression


def element(r, node, unit, cond, expression) -> None:
    """A JSX element read in `unit` (`expression` reads an expression where it is written): a reference to the
    factory call it is, at its line."""
    opening = field_of(node, "open_tag") if node.type == "jsx_element" else node
    tag = field_of(opening, "name") if opening is not None else None
    if tag is not None:
        r.add(unit, "jsx", r.src.text(tag), node, cond, base=tag_tree(r, tag, unit, None),
              args=(r.jsx_props(node, unit),))
    for part in named_children(node):
        if part is opening:
            for attribute in named_children(opening):
                for value in named_children(attribute):
                    expression(r, value, unit, cond)
        elif part.type not in PLAIN:
            expression(r, part, unit, cond)


def tree(r, node, unit, build) -> tuple:
    """A JSX element as a tree: its tag (a component's value, an intrinsic element's name) and its props."""
    opening = field_of(node, "open_tag") if node.type == "jsx_element" else node
    tag = field_of(opening, "name") if opening is not None else None
    return (JSX, tag_tree(r, tag, unit, build), r.jsx_props(node, unit)) if tag is not None else (LITERAL, "")


def tag_tree(r, tag, unit, build) -> tuple:
    """A capitalized tag or a member is a component (the value it names); any other is an intrinsic element, its name
    a string (TypeScript handbook, JSX: intrinsic and value-based elements). `build` builds a part of a tree; None: a
    tree of its own (`r.ir`)."""
    text = r.src.text(tag)
    if tag.type in ("member_expression", "nested_identifier") or text[:1].isupper():
        return r.ir(tag, unit) if build is None else build(r, tag, unit)
    return (STRNAME, text)


def props(r, node, unit) -> tuple:
    """The props object an element gives its component: a class of its own, made where the element is written."""
    opening = field_of(node, "open_tag") if node.type == "jsx_element" else node
    key = f"{r.key}#<props@{node.start_byte}>"
    cls = ClassFact(key, "js", "", r.key, r.src.line(node), made=js_class.MADE_LITERAL)
    r.facts.classes.append(cls)
    for attribute in named_children(opening):
        kids = named_children(attribute)
        if attribute.type == "jsx_attribute" and kids:
            js_class.field_unit(r, key, r.src.text(kids[0]), attribute, unit).values.append(
                value(r, kids[1] if len(kids) > 1 else None, unit))
        elif attribute.type == "jsx_expression" and kids:         # `{...spread}`: its object's members
            inner = named_children(kids[0])[0] if kids[0].type == "spread_element" and named_children(kids[0]) \
                else kids[0]
            cls.bases.append(" ".join(r.src.text(inner).split()))
    children = [c for c in named_children(node) if c is not opening and c.type not in PLAIN]
    if children:
        field = js_class.field_unit(r, key, JSX_CHILDREN, node, unit)
        field.values += [value(r, c, unit) for c in children]
    return ("cast", ir_js.CLASS_VALUE + key)


def value(r, node, unit) -> tuple:
    """An attribute's or a child's value: a string's text, an expression container's expression, `true` alone."""
    if node is None:
        return (LITERAL, "true")
    if node.type == "string":
        text = string_value(r.src, node)
        return (STRNAME, text) if text is not None else (LITERAL, "string")
    if node.type == "jsx_expression":
        inner = (named_children(node) or [None])[0]
        return r.ir(inner, unit) if inner is not None else (LITERAL, "undefined")
    return r.ir(node, unit)
