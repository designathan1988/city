"""What a GDScript condition proves about names (log/trace-rules.md, "Narrowing at branch conditions": G2; GDScript
reference, `is`): `x is T` on a local or a parameter of the function, `not` / `!` (swaps), `and` / `&&` (both facts when
true), `or` / `||` (both negations when false), parentheses. A fact is `(name_tree, ("is", T), positive)`
(`narrowing`).
"""
from __future__ import annotations

from . import scopes
from .source import field_of, named_children

NOT, AND, OR = ("not", "!"), ("and", "&&"), ("or", "||")


def _inner(node):
    while node is not None and node.type == "parenthesized_expression":
        node = (named_children(node) or [None])[0]
    return node


def name_of(r, node, unit):
    """The renamed tree of a local or parameter of the function (a lambda's capture is a copy: its own)."""
    node = _inner(node)
    if node is None or node.type != "identifier":
        return None
    tree = scopes.renamed(r, ("name", r.src.text(node)))
    return tree if tree[1] in unit.types else None


def facts(r, node, unit) -> tuple[list, list]:
    """The facts a condition proves: (when it is true, when it is false)."""
    node = _inner(node)
    if node is None:
        return [], []
    if node.type == "unary_operator" and node.children and r.src.text(node.children[0]) in NOT:
        when_true, when_false = facts(r, (named_children(node) or [None])[-1], unit)
        return when_false, when_true
    if node.type != "binary_operator":
        return [], []
    op = r.src.text(field_of(node, "op")).strip()
    left, right = field_of(node, "left"), field_of(node, "right")
    if op in AND:
        return facts(r, left, unit)[0] + facts(r, right, unit)[0], []
    if op in OR:
        return [], facts(r, left, unit)[1] + facts(r, right, unit)[1]
    name = name_of(r, left, unit) if op == "is" else None
    if name is None or right is None:
        return [], []
    fact = (name, ("is", r.src.text(right)), True)
    return [fact], [(name, fact[1], False)]
