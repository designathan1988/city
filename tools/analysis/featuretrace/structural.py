"""Objects typed as an interface (log/trace-rules.md, "What narrowing showed on the lab app": S1; TypeScript handbook,
Type Compatibility: the type system is structural, an object whose type has an interface's members is of the interface).

An interface or an object type (an alias of one too) has no object of its own: its value in the tracer (the class the
type names, T1) stands for every object typed as it. Where a value of a class of the program (an object literal, a
class's instance, another interface's value) reaches a place declared with such a type (a parameter, a local, a field,
a variable, a function's result), it is one of those objects: the value's property of each name the interface declares
holds what is written through the interface and what the interface's type admits, and a read through the interface can
give what that property holds. Each object stays its own: what one object typed as the interface holds is not another's.
A property the value's class does not declare is the one JS code creates on it (`js_transfer.prop_node`, V2).
"""
from __future__ import annotations

from . import js_transfer
from .js_class import SHAPE
from .rules import INDEX_MEMBER

DECLARED, JOINS = "#declared", "#joins"


def declared_trees(trees) -> list:
    """The trees of a place's declaration that can name an interface: a type's name (a class, an interface, an alias)
    or an object type; never a value of the language a type gives (`<lang>number`, a string), an object literal or a
    class expression (values)."""
    return [tree for tree in trees if tree[0] == "cast" and (SHAPE in tree[1] or not tree[1].startswith(("#", "<")))]


def typed(t, place, trees, unit) -> None:
    """A place declared with `trees`: each value that reaches it is joined to each interface its declaration names, as
    both grow."""
    declared = declared_trees(trees) if unit.lang == "js" else []
    shapes = (DECLARED, place)
    if not declared or shapes in t.wired:
        return
    t.wired.add(shapes)
    t.value.setdefault(shapes, [])
    for tree in declared:
        t.follow(t.node_of(tree, unit), shapes)
    joins = (JOINS, place)
    t.value.setdefault(joins, [])

    def by_value(_, values):
        for text, ctx in values:
            for shape, where in t.held(shapes):
                join(t, text, ctx, shape, where)

    def by_shape(_, values):
        for shape, where in values:
            for text, ctx in t.held(place):
                join(t, text, ctx, shape, where)
    t.subscribe(place, joins, ("typed", "value"), by_value)
    t.subscribe(shapes, joins, ("typed", "shape"), by_shape)


def join(t, text: str, ctx, shape: str, where) -> None:
    """One value typed as one interface: their properties of one name are one."""
    ix = t.ix
    s = t.type_class(shape, where)
    k = t.type_class(text, ctx)
    if not s or not k or s == k or not ix.classes[s].shape or (k, s) in t.joined:
        return
    t.joined.add((k, s))
    for name, field in fields(ix, s):
        mine = [g for g in ix.lookup(k, name) if ix.units[g].kind == "field"]
        for node in [t.hold_node(ix.units[g]) for g in mine] or [js_transfer.prop_node(k, name)]:
            t.follow(node, t.hold_node(field))              # a read through the interface can be this object's
            for part in t.own_parts(field):                 # what is written through it, what its type admits
                t.follow(part, node)


def fields(ix, key: str) -> list:
    """The properties an interface and the interfaces it extends declare (its methods are no stored value)."""
    return [(name, ix.units[g]) for k in js_transfer.chain(ix, key) for name, gids in ix.members[k].items()
            if name != INDEX_MEMBER for g in gids if ix.units[g].kind == "field"]
