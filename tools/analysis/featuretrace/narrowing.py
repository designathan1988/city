"""Where a condition proves something about a name (log/trace-rules.md, "Narrowing at branch conditions": R1, R2, N1),
while a function is read: a stack of regions, each with the facts that hold in it. A fact is `(name_tree, guard,
positive)`: the guard (`js_guards`, GDScript `is`) is true (positive) or false of the name's value there. Every tree
built while facts hold reads each narrowed name as `("narrow", name_tree, guard, positive)` (`apply`, called by
`scopes.scoped_tree`), innermost fact outermost; the typer filters the name's values by it (`narrow_values`).

A fact on a name the region assigns (an assignment, an update, a declaration, a loop or destructuring binding), or that
any function written inside the enclosing function assigns, does not hold there (R2): the name is read whole.
"""
from __future__ import annotations

from . import scopes
from .source import field_of, named_children

# The grammar fields that name what a construct assigns, per language (an identifier, or a pattern of them).
ASSIGNING = {
    "js": {"assignment_expression": "left", "augmented_assignment_expression": "left", "update_expression": "argument",
           "for_in_statement": "left", "variable_declarator": "name"},
    "gd": {"assignment": "left", "augmented_assignment": "left", "variable_statement": "name"},
}
NAMES = ("identifier", "shorthand_property_identifier_pattern", "name")
PATTERNS = ("object_pattern", "array_pattern", "pair_pattern", "assignment_pattern", "rest_pattern",
            "object_assignment_pattern", "parenthesized_expression")
FUNCTIONS = {"js": ("function_expression", "arrow_function", "generator_function", "function_declaration",
                    "generator_function_declaration", "method_definition"), "gd": ()}
LEAVING = {"js": ("return_statement", "throw_statement", "break_statement", "continue_statement"),
           "gd": ("return_statement", "break_statement", "continue_statement")}
BLOCKS = {"js": ("statement_block", "else_clause"), "gd": ("body", "else_clause")}


def frames(r) -> list:
    if getattr(r, "narrowings", None) is None:
        r.narrowings = []
    return r.narrowings


def _targets(r, node, out: set) -> None:
    if node is None:
        return
    if node.type in NAMES:
        out.add(r.src.text(node))
    elif node.type in PATTERNS:
        for child in named_children(node):
            _targets(r, field_of(child, "value") or field_of(child, "left") or child, out)


def assigned_names(r, nodes: list, lang: str) -> set[str]:
    """The names the nodes assign, anywhere inside them, as written."""
    fields, out, todo = ASSIGNING.get(lang, {}), set(), [n for n in nodes if n is not None]
    while todo:
        node = todo.pop()
        if node.type in fields:
            _targets(r, field_of(node, fields[node.type]), out)
        todo += named_children(node)
    return out


def enter_function(r, body, lang: str) -> None:
    """A function starts: the names the functions written inside it assign may change between a test and a use."""
    inner, todo = [], [body] if body is not None else []
    while todo:
        node = todo.pop()
        if node.type in FUNCTIONS.get(lang, ()):
            inner.append(node)
        else:
            todo += named_children(node)
    stack = getattr(r, "closure_written", None)
    if stack is None:
        r.closure_written = stack = []
    stack.append(assigned_names(r, inner, lang) | (stack[-1] if stack else set()))


def leave_function(r) -> None:
    r.closure_written.pop()


def region_facts(r, facts: list, region: list, lang: str) -> list:
    """The facts that hold over a region: not those on a name it assigns (its key where the reader is), nor on a name
    a closure of the function assigns (by name, whatever block declares it) (R2)."""
    if not facts:
        return []
    killed = {scopes.visible(r, name) for name in assigned_names(r, region, lang)}
    shared = r.closure_written[-1] if getattr(r, "closure_written", None) else set()
    return [f for f in facts if f[0][0] == "name" and f[0][1] not in killed and f[0][1].split("#")[0] not in shared]


def open(r, facts: list, region: list, lang: str) -> None:
    """A region starts with the facts that hold over it."""
    frames(r).append(region_facts(r, facts, region, lang))


def close(r) -> None:
    frames(r).pop()


def _chain(tree) -> tuple[tuple, list]:
    """A read through facts as (the name read, the facts already on it)."""
    applied = []
    while tree[0] == "narrow":
        applied.append((tree[2], tree[3]))
        tree = tree[1]
    return tree, applied


def wrap(tree, facts: list):
    """`tree` with each name a fact is about read through that fact, once (a name already read through some facts
    gets the others outside them)."""
    if not facts or not isinstance(tree, tuple) or not tree:
        return tree
    if tree[0] in ("name", "narrow"):
        name, applied = _chain(tree)
        if name[0] == "name":
            for name_tree, guard, positive in facts:
                if name_tree == name and (guard, positive) not in applied:
                    tree = ("narrow", tree, guard, positive)
                    applied.append((guard, positive))
            return tree
    return tuple(wrap(part, facts) if isinstance(part, tuple) else part for part in tree)


def apply(r, tree):
    """A tree built where the reader is: every fact of every open region applied, outer regions first."""
    held = [fact for frame in frames(r) for fact in frame]
    return wrap(tree, held) if held else tree


def after_if(st, when_true: list, when_false: list, lang: str) -> list:
    """The facts that hold after an `if` (R1): the false facts when only its consequence always leaves, the true ones
    when only its alternative does."""
    consequence, alt = field_of(st, "consequence") or field_of(st, "body"), field_of(st, "alternative")
    out, back = leaves(consequence, lang), alt is not None and leaves(alt, lang)
    if out and not back:
        return when_false
    if back and not out:
        return when_true
    return []


def leaves(node, lang: str) -> bool:
    """Whether a statement always ends its path: a jump, a block with one, an `if` whose every branch does."""
    if node is None:
        return False
    if node.type in LEAVING[lang]:
        return True
    if node.type in BLOCKS[lang]:
        return any(leaves(child, lang) for child in named_children(node))
    if node.type == "if_statement" and lang == "js":
        alt = field_of(node, "alternative")
        return leaves(field_of(node, "consequence"), lang) and alt is not None and leaves(alt, lang)
    return False
