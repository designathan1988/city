"""What a node declares: the name it gives itself through `name`/`declarator`, whether it is callable (a parameter
list on it or along its declarator chain), its parameters and their types, every declarator of one declaration,
the type it names, and the statement shapes that bind names for a body (a loop's `left`, a clause's parameters)."""
from .nodes import chain_leaves, leaf_starts, text_of
from .roles import ASSIGNMENT_OPERATORS
from .words import IDENT

PARAMETER_FIELDS = ('parameters', 'parameter')
NOT_A_PARAMETER_NAME = frozenset(['type', 'value', 'default_value'])
# Deepest declarator chain followed (C pointer-to-function-returning-reference and friends stay far below this).
MAX_CHAIN = 16
# The block opener: a child of a statement that opens with it holds statements (declarations among them).
BLOCK_OPENERS = frozenset(['{'])


def definition_of(node, src):
    """(name, qualifier tuple, start bytes of the name's leaves) when the node names itself, else None."""
    qualifier, chain, current = [], [], node
    for _ in range(MAX_CHAIN):
        if current.child_by_field_name('name') is not None:
            holder = current
            while True:
                scope_node = holder.child_by_field_name('scope')
                if scope_node is not None:
                    qualifier.append(text_of(src, scope_node))
                    chain.extend(leaf_starts(scope_node))
                inner = holder.child_by_field_name('name')
                if inner is None:
                    break
                holder = inner
            return _named_leaf(holder, src, qualifier, chain)
        following = current.child_by_field_name('declarator')
        if following is None:
            if current is node:
                return None
            if current.named_child_count == 0:
                return _named_leaf(current, src, qualifier, chain)
            if current.named_child_count != 1:
                return None
            following = current.named_children[0]
        current = following
    return None


def _named_leaf(node, src, qualifier, chain):
    if not node.is_named or node.named_child_count != 0:
        return None
    name = ' '.join(text_of(src, node).split())
    if not any(c.isalnum() or c == '_' for c in name):
        return None
    return name, tuple(qualifier), frozenset(chain + [node.start_byte])


def wraps_definitions(node, src):
    """True when an unfielded named child is itself a definition: the node only groups definitions."""
    cursor = node.walk()
    if not cursor.goto_first_child():
        return False
    while True:
        child = cursor.node
        if (cursor.field_name is None and child.is_named and not child.is_extra and child.child_count
                and definition_of(child, src) is not None):
            return True
        if not cursor.goto_next_sibling():
            return False


def callable_part(node, depth):
    """The node holding the parameter list, found on the node or along its declarator/value chain."""
    if depth > MAX_CHAIN:
        return None
    for field in PARAMETER_FIELDS:
        if node.child_by_field_name(field) is not None:
            return node
    for field in ('declarator', 'value'):
        child = node.child_by_field_name(field)
        if child is not None:
            hit = callable_part(child, depth + 1)
            if hit is not None:
                return hit
    # the same chain definition_of follows: below the definition itself, a node wrapping a single named child
    # (`& f(...)`, a reference declarator) leads to it
    if depth > 0 and node.named_child_count == 1:
        return callable_part(node.named_children[0], depth + 1)
    return None


def parameter_names(holder, src):
    names, types = set(), {}
    params = holder.child_by_field_name('parameters')
    if params is not None:
        items = params.named_children
    else:
        single = holder.child_by_field_name('parameter')
        items = [single] if single is not None else []
    for item in items:
        item_type = type_of(src, item)
        before = set(names)
        stack = [item]
        while stack:
            current = stack.pop()
            if current.is_extra:
                continue
            if current.child_count == 0:
                text = text_of(src, current)
                if current.is_named and IDENT.fullmatch(text):
                    names.add(text)
                continue
            cursor = current.walk()
            cursor.goto_first_child()
            while True:
                if cursor.field_name not in NOT_A_PARAMETER_NAME:
                    stack.append(cursor.node)
                if not cursor.goto_next_sibling():
                    break
        if item_type:
            types.update({n: item_type for n in names - before})
    return names, types


def declarator_names(node, src):
    """The name each `declarator` of one declaration declares (int a = 1, b = 2 declares a and b): down each
    declarator through its `name`/`declarator` field or its single named child to the name."""
    names = []
    for current in node.children_by_field_name('declarator'):
        for _ in range(MAX_CHAIN):
            if current.child_count == 0:
                text = text_of(src, current)
                if current.is_named and IDENT.fullmatch(text):
                    names.append(text)
                break
            following = current.child_by_field_name('name') or current.child_by_field_name('declarator')
            if following is None and current.named_child_count == 1:
                following = current.named_children[0]
            if following is None:
                break
            current = following
    return names


def member_name(node, src):
    """A member of a container's body declared by its first child alone (an enum value with no `name` field)."""
    first = node.named_children[0] if node.named_child_count else None
    if first is None or first.child_count or first.is_extra:
        return None
    text = text_of(src, first)
    return (text, first.start_byte) if IDENT.fullmatch(text) else None


def type_of(src, node):
    """The type a declaration gives its name: the last name of its `type` (or `return_type`) field, template arguments
    left out (std::vector<Note> is a vector)."""
    if node is None:
        return None
    holder = node.child_by_field_name('type') or node.child_by_field_name('return_type')
    if holder is None:
        return None
    if holder.child_count == 0:
        text = text_of(src, holder)
        return text if IDENT.fullmatch(text) else None
    leaves = chain_leaves(src, holder)
    return text_of(src, leaves[-1]) if leaves else None


def bind_statement_names(node, src, unit):
    """A statement that runs a body with names bound for it declares them: the `left` part of a loop (for x in xs)
    and the parameters of a clause (catch (const E &e))."""
    if node.child_by_field_name('body') is None:
        return
    if any(node.child_by_field_name(f) is not None for f in PARAMETER_FIELDS):
        names, types = parameter_names(node, src)
        unit['locals'].update(names)
        unit['local_types'].update(types)
    bound = node.child_by_field_name('left')
    if bound is None or {text_of(src, c) for c in node.children if not c.is_named} & ASSIGNMENT_OPERATORS:
        return
    unit['locals'].update(text_of(src, leaf) for leaf in chain_leaves(src, bound))


def opens_block(node, src):
    """A block: its first token is the block opener of the language."""
    return node.child_count > 0 and not node.children[0].is_named and text_of(src, node.children[0]) in BLOCK_OPENERS


def keyword_led(node, src):
    """A clause led by a reserved word (else, elif, catch, finally): its first child is an anonymous word."""
    return node.child_count > 0 and not node.children[0].is_named and text_of(src, node.children[0]).isidentifier()
