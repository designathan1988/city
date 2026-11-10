"""What each name of a construct does, from the language's own constructs: the called name of a call, the target of an
assignment or an increment, every name of an import, the bases a container's header names; member-access chains
(a.b.c) recorded once at their top, and functions handed to a receiver in a call (registrations)."""
from .nodes import chain_leaves, line_of, text_of
from .words import IDENT

# Roles of a name, from the construct of the language holding it.
CALL, WRITE, READ, DEPEND, MEMBER = 'call', 'write', 'read', 'dep', 'member'
NAME_ROLES = (CALL, WRITE, READ, DEPEND)
# The language's member-access operators: what follows one is a member of what precedes it, found through its type.
MEMBER_OPERATORS = frozenset(['.', '->', '::', '?.', '?->'])
# The words for the object a method runs on: its members are those of the enclosing class.
SELF_WORDS = frozenset(['this', 'self'])
# The call operator: a construct's `arguments` are call arguments when they open with it; type arguments (<...>) and
# subscripts ([...]) share the field name but call nothing.
CALL_OPENERS = frozenset(['('])
# The language's own words for a mutation: the assignment operators and the increments.
ASSIGNMENT_OPERATORS = frozenset(['=', ':=', '+=', '-=', '*=', '/=', '%=', '|=', '&=', '^=', '<<=', '>>=', '**=', '//='])
INCREMENT_OPERATORS = frozenset(['++', '--'])
# The words that make one file or unit depend on another: imports and includes (keywords), loads (built-in calls).
DEPENDENCY_WORDS = frozenset(['import', 'include', '#include', 'extends', 'from', 'require', 'using', 'use', 'preload',
                              'load', 'import_module'])


def classify(node, src, roles):
    """Gives the names under a construct their role: the called name of a call, the target of an assignment or an
    increment, every name of an import/include/load. A name with no role from its constructs is read."""
    anonymous = [c for c in node.children if not c.is_named]
    words = {text_of(src, c) for c in anonymous}
    if words & ASSIGNMENT_OPERATORS or words & INCREMENT_OPERATORS:
        target = (node.child_by_field_name('left') or node.child_by_field_name('declarator')
                  or node.child_by_field_name('name') or (node.named_children[0] if node.named_child_count else None))
        last = last_identifier(src, target)
        if last is not None:
            roles[last] = WRITE
    arguments = call_arguments(node, src)
    if arguments is not None:
        callee = node.child_by_field_name('function') or node.child_by_field_name('constructor')
        last = last_identifier(src, callee if callee is not None else node, arguments)
        if last is not None:
            roles.setdefault(last, CALL)
    if words & DEPENDENCY_WORDS:  # anonymous children are the grammar's own reserved words
        mark_all(src, node, roles, DEPEND)


def call_arguments(node, src):
    """The call arguments of a construct, when it is a call: its `arguments` field opened by the call operator."""
    arguments = node.child_by_field_name('arguments')
    if arguments is None or arguments.child_count == 0 or text_of(src, arguments.children[0]) not in CALL_OPENERS:
        return None
    return arguments


def last_identifier(src, root, excluded=None):
    """Start byte of the last name (in source order) of a callee or target expression: f in obj.f, place in A::place,
    make_shared in std::make_shared<T>, v in v[i]; arguments and subscripts are not part of the name."""
    if root is None:
        return None
    if excluded is not None and root.start_byte == excluded.start_byte and root.end_byte == excluded.end_byte:
        return None
    leaves = [leaf for leaf in chain_leaves(src, root)
              if excluded is None or not excluded.start_byte <= leaf.start_byte < excluded.end_byte]
    return leaves[-1].start_byte if leaves else None


def mark_all(src, root, roles, role):
    stack = [root]
    while stack:
        current = stack.pop()
        if current.child_count == 0:
            if current.is_named and IDENT.fullmatch(text_of(src, current)):
                roles.setdefault(current.start_byte, role)
            continue
        stack.extend(current.children)


def mark_header(node, src, chain, roles):
    """The names a container's header writes besides its own name (class A : public B, class A extends B) are what it
    depends on: its bases. Its body holds members, not dependencies."""
    cursor = node.walk()
    if not cursor.goto_first_child():
        return
    while True:
        child = cursor.node
        if cursor.field_name != 'body' and child.is_named and not child.is_extra \
                and not any(child.start_byte <= start < child.end_byte for start in chain):
            mark_all(src, child, roles, DEPEND)
        if not cursor.goto_next_sibling():
            return


def imports_only(node, src):
    """An import or include (a dependency word among its own tokens) names what it brings in, it defines nothing;
    unless it has a `name` field of its own (an alias: using V = std::vector<int>)."""
    words = {text_of(src, c) for c in node.children if not c.is_named}
    return bool(words & DEPENDENCY_WORDS) and node.child_by_field_name('name') is None


def member_access(node, src):
    """A member access construct: an operand first, then only member operators between its parts (a.b, p->f, A::b).
    The same token elsewhere (the return-type arrow of `func f() -> int:`, beside `func` and `:`) is not one."""
    operators = {text_of(src, c) for c in node.children if not c.is_named}
    return bool(operators) and operators <= MEMBER_OPERATORS and node.children[0].is_named


def record_chain(node, src, roles, skip, unit, starts):
    """A member-access chain (a.b.c, A::b, self.x.f()), recorded once at its top: its names in order and the role of
    the last one; every name after the first is a member, resolved through the types, never by name alone."""
    if not member_access(node, src):
        return
    parent = node.parent
    if parent is not None and parent.named_child_count and parent.named_children[0].start_byte == node.start_byte             and parent.named_children[0].end_byte == node.end_byte and member_access(parent, src):
        return
    leaves = [leaf for leaf in chain_leaves(src, node) if leaf.start_byte not in skip]
    if len(leaves) < 2:
        return
    last = leaves[-1]
    role = roles.get(last.start_byte)
    if role is None and last.parent is not None and call_arguments(last.parent, src) is not None:
        role = CALL
    for leaf in leaves[1:]:
        roles[leaf.start_byte] = MEMBER
    # A call whose value is thrown away (the whole statement) is made for its effect; one whose value is used is a query.
    call = parent if parent is not None and call_arguments(parent, src) is not None else node
    holder = call.parent
    discarded = holder is not None and call.start_byte == holder.start_byte and holder.end_byte - call.end_byte <= 1
    unit['chains'].append((tuple(text_of(src, leaf) for leaf in leaves), role or READ, line_of(starts, node.start_byte),
                           discarded))


def record_registration(node, src, unit, starts):
    """A call handing names to a receiver: (names of the receiver side, names passed as arguments, line).

    The receiver side is the callee (`function` field) or, where the grammar keeps the callee beside the call (a method
    call under its object's chain), the call's parent without the arguments. A function passed as an argument is then
    registered on the receiver: firing the receiver reaches it."""
    arguments = call_arguments(node, src)
    side = node.child_by_field_name('function') or node
    parent = node.parent
    if side is node and parent is not None and member_access(parent, src) and parent.named_children \
            and parent.named_children[-1].start_byte == node.start_byte:
        side = parent  # the call is the last part of a member access (obj.method(...)): the object belongs to it
    passed = [name for name in (reference_of(src, a) for a in arguments.named_children) if name]
    if not passed:
        return
    callee = ordered_identifiers(src, side, arguments)
    if callee:
        unit['regs'].append((tuple(callee), tuple(passed), line_of(starts, node.start_byte)))


def reference_of(src, node):
    """The name an argument refers to when the argument is only a name: a plain name, a member chain (a.b, A::b) or a
    name under one prefix operator (&f); None for an expression that computes or defines something (a call, a closure,
    a + b), whose names are the caller's own reads and calls."""
    if node.child_count == 0:
        text = text_of(src, node)
        return text if node.is_named and IDENT.fullmatch(text) else None
    named = node.named_children
    operators = {text_of(src, c) for c in node.children if not c.is_named}
    if len(named) == 1 and (not operators or len(operators) == 1 and node.children[0].start_byte < named[0].start_byte):
        return reference_of(src, named[0])
    if len(named) >= 2 and operators and operators <= MEMBER_OPERATORS:
        names = [reference_of(src, c) for c in named]
        return names[-1] if all(names) else None
    return None


def ordered_identifiers(src, root, excluded):
    found, stack = [], [root]
    while stack:
        current = stack.pop()
        if excluded is not None and current.start_byte == excluded.start_byte and current.end_byte == excluded.end_byte:
            continue
        if current.child_count == 0:
            if current.is_named and IDENT.fullmatch(text_of(src, current)):
                found.append((current.start_byte, text_of(src, current)))
            continue
        stack.extend(current.children)
    return [t for _, t in sorted(found)]
