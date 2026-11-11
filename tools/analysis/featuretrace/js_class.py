"""JavaScript / TypeScript classes and object literals (log/trace-rules.md; J4, J5, J9).

A class has its bases (`extends`, and `implements` for the members its interfaces declare), its constructor (the one
written, else the default one, which calls the base's), its methods, getters and setters (accessor units of a field:
a read calls the getter, a write the setter), fields (instance initializers run in the constructor, static ones and
static blocks and decorators in `<static>`). A class written inside a function is local to it. An object literal is a
class of its own whose values are read where it is written (J9); a spread adds the spread object's class as a base. An
object type is an interface without a name (T1'); an optional property (`p?: T`) holds `undefined` besides T's values.
"""
from __future__ import annotations

from . import ir_js, js_body, js_bounds, js_decl, js_types, jsdoc
from .model import ClassFact, declare, enclose
from .rules import CONSTRUCTOR, INDEX_MEMBER
from .solver import UNDEFINED
from .source import field_of, named_children

ACCESSORS = ("get", "set")
SHAPE = "<type@"                   # an object type's class key: `<file>#<type@<start byte>>` (T1')


def class_declaration(r, node, name: str, owner: str, holder) -> str:
    """A class: its bases (`extends`, `implements`), its constructor (written or default), members, `<static>`."""
    local = holder is not None and holder.name != "<module>" and holder.kind in ("func", "lambda")
    key = f"{r.key}#{holder.name}.{name}@{r.src.line(node)}" if local else js_decl.scoped(owner, name)
    if local:
        r.facts.classes.append(ClassFact(key, "js", name, r.key, r.src.line(node)))
        r.unit("class", name, holder.owner, node, qual=key)
        declare(r, holder, name, [("cast", ir_js.CLASS_VALUE + key)])
    else:
        js_decl.new_class(r, node, name, owner)
    cls = next(c for c in reversed(r.facts.classes) if c.key == key)
    for heritage in (c for c in named_children(node) if c.type == "class_heritage"):
        for clause in named_children(heritage):
            if clause.type == "extends_clause":
                cls.bases.append(" ".join(r.src.text(field_of(clause, "value")).split()))
            elif clause.type == "implements_clause":
                cls.bases += [r.src.text(t) for t in named_children(clause)]
    outer = holder if local else None
    body = field_of(node, "body")
    explicit = next((m for m in named_children(body) if m.type == "method_definition"
                     and ir_js.member_name(r.src, field_of(m, "name")) == CONSTRUCTOR), None) if body else None
    r.type_params.append(js_types.parameters(r, node))      # the class's type parameters are erased (T1)
    try:
        if explicit is not None:
            r.function(explicit, CONSTRUCTOR, key, outer)
        r.constructor_of(key, node)
        for decorator in (c for c in named_children(node) if c.type == "decorator"):
            js_body.decorator(r, decorator, r.static_of(key, node))
        members(r, body, key, outer, node)
    finally:
        r.type_params.pop()
    return key


def members(r, body, key: str, outer, class_node=None) -> None:
    """The members of a class, interface or object literal body."""
    for member in named_children(body) if body is not None else []:
        kind = member.type
        name_node = field_of(member, "name")
        name = ir_js.member_name(r.src, name_node) if name_node is not None else ""
        static = any(c.type == "static" for c in member.children)
        if kind in ("method_definition", "method_signature", "abstract_method_signature"):
            which = next((c.type for c in member.children if c.type in ACCESSORS), None)
            if which:
                prop = field_unit(r, key, name, member, outer)
                fn = r.function(member, f"{name}.{which}", key, outer)
                fn.accessor_of = name
                prop.accessors[which] = fn.local_id
            elif name != CONSTRUCTOR:
                method = r.function(member, name, key, outer)
                method.static = static
                method.optional = any(c.type == "?" for c in member.children)   # `m?(): T`: may be undefined (T1')
        elif kind in ("public_field_definition", "property_signature"):
            value = field_of(member, "value")
            if value is not None and value.type in ir_js.FUNCTIONS:
                r.function(value, name, key, outer).static = static
                continue
            field = field_unit(r, key, name, member, outer)
            field.static = static
            field.type_text = r.type_text(field_of(member, "type"))
            field.values += r.type_values(field_of(member, "type"), field)
            optional = any(c.type == "?" for c in member.children)
            if field_of(member, "type") is not None:
                field.bound = js_bounds.of(r, field_of(member, "type"), optional)   # what it can hold (S2)
            elif jsdoc.is_js(r):
                field.bound = jsdoc.declared(r, member)                            # its JSDoc `@type` (S2')
            if optional:
                field.values.append(("cast", UNDEFINED))        # `p?: T`: undefined added to its type (T1')
            if value is not None:
                field.values.append(r.ir(value, field))
                runner = r.static_of(key, member) if static else r.constructor_of(key, class_node or member)
                js_body.expr(r, value, runner, ())
            elif kind == "public_field_definition" and not any(c.type == "declare" for c in member.children):
                field.values.append(("literal", "undefined"))   # a field written with no initializer (MDN, V1)
        elif kind == "index_signature":         # `[k: K]: V`, `[P in X]: V`: any property name reads V (T2, T1')
            mapped = next((c for c in named_children(member) if c.type == "mapped_type_clause"), None)
            r.type_params.append({r.src.text(field_of(mapped, "name"))} if mapped is not None else set())
            try:
                field = field_unit(r, key, INDEX_MEMBER, member, outer)
                field.values += r.type_values(field_of(member, "type"), field)
                field.bound = js_bounds.of(r, field_of(member, "type"))
            finally:
                r.type_params.pop()
        elif kind == "class_static_block":
            js_body.block(r, field_of(member, "body"), r.static_of(key, member), ())
        elif kind == "decorator":
            js_body.decorator(r, member, r.static_of(key, member))


def field_unit(r, key: str, name: str, node, outer):
    for unit in r.facts.units:
        if unit.owner == key and unit.name == name and unit.kind == "field":
            return unit
    unit = r.unit("field", name, key, node)
    if outer is not None:
        enclose(unit, outer)
    return unit


def object_literal(r, node, unit) -> str:
    """An object literal: a class of its own (J9); its values are read where the literal is written."""
    key = f"{r.key}#<object@{node.start_byte}>"
    cls = ClassFact(key, "js", "", r.key, r.src.line(node))
    r.facts.classes.append(cls)
    outer = unit if unit.kind in ("func", "lambda") and unit.name != "<module>" else None
    for item in named_children(node):
        if item.type == "pair":
            name = ir_js.member_name(r.src, field_of(item, "key"))
            value = field_of(item, "value")
            if value is not None and value.type in ir_js.FUNCTIONS:
                r.function(value, name, key, outer or unit)
                continue
            field = field_unit(r, key, name, item, outer or unit)
            field.values.append(r.ir(value, field))
            js_body.expr(r, value, unit, ())
        elif item.type == "shorthand_property_identifier":
            field = field_unit(r, key, r.src.text(item), item, outer or unit)
            field.values.append(("name", r.src.text(item)))
            js_body.expr(r, item, unit, ())
        elif item.type == "spread_element":
            inner = (named_children(item) or [None])[0]
            if inner is not None:
                cls.bases.append(" ".join(r.src.text(inner).split()))
                js_body.expr(r, inner, unit, ())
        elif item.type == "method_definition":
            members(r, _Single(item), key, outer or unit)
    return key


def object_type(r, node) -> str:
    """An object type written in an annotation (`{ a: T; f(): U }`): an interface without a name, a class of its own
    whose members are those it declares (T1')."""
    key = f"{r.key}#{SHAPE}{node.start_byte}>"
    r.facts.classes.append(ClassFact(key, "js", "", r.key, r.src.line(node), shape=True))
    members(r, node, key, None)
    return key


class _Single:
    """One member handed to `members` as a body."""

    def __init__(self, node):
        self.children = [node]
        self.is_named = True
