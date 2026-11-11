"""The TypeScript tree as the language groups it (log/trace-rules.md, J13).

tree-sitter-typescript (0.23; its issue #282) reads a call written with type arguments after a unary operator, `await`,
a prefix update or a binary expression that ends in one as a call of that whole expression: `void f<T>(a).catch(b)`
comes out as `call(member(call(unary(void f), <T>, (a)), catch), (b))`. In ECMAScript a callee is a member, call or
primary expression (an operator expression is one only inside parentheses, a node of its own), so the operator applies
to the whole chain: `void (f<T>(a).catch(b))`. `grouped` gives the readers a view of the parser's tree in which each
such chain is regrouped: the operator's rightmost operand is the callee, the chain (calls, members, indexes, `!`) is
rebuilt on it, and the operators wrap the chain, each node spanning its own text. Every other node is the parser's,
seen through the view (its type, children, fields and span).
"""
from __future__ import annotations

# An operator expression and the field of its rightmost operand (None: its one named child).
OPERANDS = {"unary_expression": "argument", "update_expression": "argument", "binary_expression": "right",
            "await_expression": None}
# A node a chain continues through and the field it continues down (None: its one named child).
CHAIN = {"call_expression": "function", "member_expression": "object", "subscript_expression": "object",
         "non_null_expression": None}
TYPE_ARGUMENTS = "type_arguments"


class View:
    """A node of the tree as the language groups it: the parser's node, or one the regrouping rebuilt (its children
    given, its span from its operand's start)."""
    __slots__ = ("raw", "type", "is_named", "start_byte", "end_byte", "_chained", "_children", "_fields")

    def __init__(self, raw, chained: bool = False, children: list | None = None, fields: dict | None = None,
                 start: int | None = None, end: int | None = None):
        self.raw, self.type, self.is_named = raw, raw.type, raw.is_named
        self.start_byte = raw.start_byte if start is None else start
        self.end_byte = raw.end_byte if end is None else end
        self._chained, self._children, self._fields = chained, children, fields

    @property
    def has_error(self) -> bool:
        return self.raw.has_error

    @property
    def children(self) -> list:
        if self._children is None:
            self._children, self._fields = _wrapped(self.raw)
        return self._children

    def child_by_field_name(self, name: str):
        if self._children is None:
            self._children, self._fields = _wrapped(self.raw)
        return self._fields.get(name)


def grouped(root) -> View:
    """The view of a parsed TypeScript or TSX tree."""
    return view(root)


def view(raw, chained: bool = False) -> View:
    """A node of the parser's tree as the language groups it: a chain's top (a chain node no chain continues through)
    whose bottom call the parser gave an operator expression is regrouped."""
    if not chained and raw.type in CHAIN:
        path = misgrouped(raw)
        if path:
            return lift(down_of(path[-1]), raw, path)
    return View(raw, chained)


def down_of(raw):
    """The node a chain node continues down to: a call's callee, a member's or an index's object, a `!`'s operand."""
    field = CHAIN[raw.type]
    if field is not None:
        return raw.child_by_field_name(field)
    return next((c for c in raw.children if c.is_named), None)


def operand_of(raw):
    """An operator expression's rightmost operand."""
    field = OPERANDS[raw.type]
    if field is not None:
        return raw.child_by_field_name(field)
    return next((c for c in raw.children if c.is_named), None)


def misgrouped(top) -> list | None:
    """The chain from `top` down to a call written with type arguments whose callee the parser made an operator
    expression, or None."""
    path, node = [], top
    while node is not None and node.type in CHAIN:
        path.append(node)
        node = down_of(node)
    bottom = path[-1]
    if node is not None and node.type in OPERANDS and bottom.type == "call_expression" \
            and bottom.child_by_field_name(TYPE_ARGUMENTS) is not None:
        return path
    return None


def lift(raw, top, path: list) -> View:
    """`raw`, the expression the parser made the chain's callee: an operator wraps what its rightmost operand gives;
    anything else is the callee, the chain rebuilt on it."""
    if raw.type in OPERANDS:
        operand = operand_of(raw)
        return rebuilt(raw, operand, lift(operand, top, path), raw.start_byte, top.end_byte)
    node = View(raw, chained=True)
    for link in reversed(path):
        node = rebuilt(link, down_of(link), node, raw.start_byte, link.end_byte)
    return node


def rebuilt(raw, replaced, inner: View, start: int, end: int) -> View:
    """`raw` with its child `replaced` given as `inner`, spanning start..end; its other children as the parser's."""
    children, fields = [], {}
    for i, child in enumerate(raw.children):
        node = inner if child == replaced else view(child)
        children.append(node)
        name = raw.field_name_for_child(i)
        if name:
            fields.setdefault(name, node)
    return View(raw, children=children, fields=fields, start=start, end=end)


def _wrapped(raw) -> tuple[list, dict]:
    """The views of a parser node's children, and of its fields (a field is the child itself)."""
    children, fields = [], {}
    down = CHAIN.get(raw.type, False)
    for i, child in enumerate(raw.children):
        name = raw.field_name_for_child(i)
        chained = down is not False and (name == down if down is not None else child.is_named)
        node = view(child, chained)
        children.append(node)
        if name:
            fields.setdefault(name, node)
    return children, fields
