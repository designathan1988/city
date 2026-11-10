"""GDScript bodies: each statement and expression read by its rule, with the branch conditions that guard it.

Branches (`if`/`elif`/`else`, `match`, `for`, `while`) are all followed; the condition of each is carried on every
reference written under it. A member chain `a.b.c()` keeps its receiver names in order, so the resolver can find the
class of `a`, then of `b`, then the definition of `c`.
"""
from __future__ import annotations

from . import gd_calls
from .rules import SIGNAL_OPS
from .source import field_of, is_identifier, named_children, string_value

COND_WIDTH = 90
UNKNOWN = "<?>"
TYPED = "<type>"   # a receiver whose type the expression itself names (a cast)
CALLED = "<call>"


def squash(text: str) -> str:
    text = " ".join(text.split())
    return text if len(text) <= COND_WIDTH else text[:COND_WIDTH - 3] + "..."


def statement(r, st, unit, cls, cond) -> None:
    src = r.src
    kind = st.type
    if kind in ("variable_statement", "const_statement"):
        name = src.text(field_of(st, "name"))
        value = field_of(st, "value")
        if value is not None and value.type == "lambda":
            unit.lambdas[name] = r.lambda_unit(value, unit, cls).local_id
            unit.types[name] = ""
            return
        typ = field_of(st, "type")
        unit.types[name] = src.text(typ) if typ is not None and typ.type == "type" else (
            r.infer(value) if value is not None else "")
        if value is not None:
            expr(r, value, unit, cls, cond)
    elif kind == "if_statement":
        test = field_of(st, "condition")
        expr(r, test, unit, cls, cond)
        saved = narrow(r, test)
        r.block(field_of(st, "body"), unit, cls, cond + ("if " + squash(src.text(test)),))
        r.narrowed = saved
        for i, child in enumerate(st.children):
            if st.field_name_for_child(i) != "alternative":
                continue
            if child.type == "elif_clause":
                other = field_of(child, "condition")
                expr(r, other, unit, cls, cond)
                r.block(field_of(child, "body"), unit, cls, cond + ("elif " + squash(src.text(other)),))
            else:
                r.block(field_of(child, "body"), unit, cls, cond + ("else of if " + squash(src.text(test)),))
    elif kind == "for_statement":
        left, right = field_of(st, "left"), field_of(st, "right")
        unit.types[src.text(left)] = ""
        expr(r, right, unit, cls, cond)
        label = f"for {src.text(left)} in {squash(src.text(right))}"
        r.block(field_of(st, "body"), unit, cls, cond + (label,))
    elif kind == "while_statement":
        test = field_of(st, "condition")
        expr(r, test, unit, cls, cond)
        r.block(field_of(st, "body"), unit, cls, cond + ("while " + squash(src.text(test)),))
    elif kind == "match_statement":
        value = field_of(st, "value")
        expr(r, value, unit, cls, cond)
        body = field_of(st, "body")
        for section in named_children(body) if body is not None else []:
            block = field_of(section, "body")
            pattern = ", ".join(src.text(c) for c in named_children(section) if c is not block)
            label = f"match {squash(src.text(value))}: {squash(pattern)}"
            for part in named_children(section):
                if part.type == "pattern_binding":
                    for name in named_children(part):
                        unit.types[src.text(name)] = ""
                elif part.type == "pattern_guard":
                    expr(r, part, unit, cls, cond + (label,))
            if block is not None:
                r.block(block, unit, cls, cond + (label,))
    elif kind == "return_statement":
        values = named_children(st)
        unit.returns.append((src.line(st), squash(src.text(values[0])) if values else ""))
        for v in values:
            expr(r, v, unit, cls, cond)
    else:
        expr(r, st, unit, cls, cond)


def type_test(r, node) -> tuple[str, str] | None:
    """`x is T` / `x as T`: (the name tested, the type), else None."""
    if node is None or node.type != "binary_operator":
        return None
    op = field_of(node, "op")
    left, right = field_of(node, "left"), field_of(node, "right")
    if op is None or left is None or right is None or r.src.text(op) not in ("is", "as"):
        return None
    return r.src.text(left), r.src.text(right)


def narrow(r, test) -> dict:
    """Inside the branch, a name an `is` test proves (alone or joined by `and`) is of that type; the references read
    there carry it. Returns the narrowing to put back after the branch."""
    saved, todo = dict(r.narrowed), [test]
    r.narrowed = dict(r.narrowed)
    while todo:
        node = todo.pop()
        if node is None:
            continue
        op = field_of(node, "op") if node.type == "binary_operator" else None
        if op is not None and r.src.text(op) in ("and", "&&"):
            todo += [field_of(node, "left"), field_of(node, "right")]
            continue
        found = type_test(r, node)
        if found and r.src.text(field_of(node, "op")) == "is" and is_identifier(found[0]):
            r.narrowed[found[0]] = found[1]
    return saved


def expr(r, node, unit, cls, cond) -> None:
    if node is None:
        return
    kind = node.type
    if kind in ("if_statement", "for_statement", "while_statement", "match_statement", "return_statement",
                "variable_statement", "const_statement"):
        statement(r, node, unit, cls, cond)
    elif kind == "call":
        gd_calls.call(r, node, unit, cls, cond)
    elif kind == "attribute":
        attribute(r, node, unit, cls, cond, write=False)
    elif kind in ("assignment", "augmented_assignment"):
        left, right = field_of(node, "left"), field_of(node, "right")
        target(r, left, unit, cls, cond)
        expr(r, right, unit, cls, cond)
        hint = r.infer(right) if kind == "assignment" and right is not None else ""
        name = r.src.text(left).removeprefix("self.") if left is not None else ""
        if hint and is_identifier(name) and name not in unit.types:
            r.assigned.setdefault((cls.key, name), hint)
    elif kind == "identifier":
        r.add(unit, "read", r.src.text(node), node, cond)
    elif kind == "lambda":
        lam = r.lambda_unit(node, unit, cls)
        r.add(unit, "lambda", lam.name, node, cond, target=lam.local_id)
    elif kind not in ("string", "string_name", "get_node", "comment"):
        for child in named_children(node):
            expr(r, child, unit, cls, cond)


def target(r, node, unit, cls, cond) -> None:
    """The left side of an assignment: the name written."""
    if node is None:
        return
    if node.type == "identifier":
        r.add(unit, "write", r.src.text(node), node, cond)
    elif node.type == "attribute":
        attribute(r, node, unit, cls, cond, write=True)
    elif node.type == "subscript":
        kids = named_children(node)
        target(r, kids[0], unit, cls, cond)
        for k in kids[1:]:
            expr(r, k, unit, cls, cond)
    else:
        expr(r, node, unit, cls, cond)


def attribute(r, node, unit, cls, cond, write: bool) -> None:
    src = r.src
    kids = named_children(node)
    head, segs = kids[0], kids[1:]
    if head.type == "identifier":
        chain = [src.text(head)]
        if chain[0] not in ("self", "super"):
            r.add(unit, "read", chain[0], head, cond)
        if chain[0] in r.narrowed:
            chain = [TYPED + r.narrowed[chain[0]]]
    elif head.type == "parenthesized_expression" and type_test(r, named_children(head)[0]):
        expr(r, head, unit, cls, cond)
        chain = [TYPED + type_test(r, named_children(head)[0])[1]]
    elif head.type == "call" and named_children(head)[0].type == "identifier":
        gd_calls.call(r, head, unit, cls, cond)
        chain = [CALLED + src.text(named_children(head)[0])]
    else:
        expr(r, head, unit, cls, cond)
        chain = [UNKNOWN]
    for i, seg in enumerate(segs):
        last = i == len(segs) - 1
        nxt = segs[i + 1] if not last else None
        if seg.type == "identifier":
            name = src.text(seg)
            ahead = src.text(named_children(nxt)[0]) if nxt is not None and nxt.type == "attribute_call" else ""
            if ahead not in SIGNAL_OPS:
                r.add(unit, "write" if write and last else "read", name, seg, cond, recv=tuple(chain))
            chain.append(name)
        elif seg.type == "attribute_call":
            name = src.text(named_children(seg)[0])
            gd_calls.member_call(r, seg, name, tuple(chain), unit, cls, cond)
            chain.append(CALLED + name)
        else:
            expr(r, seg, unit, cls, cond)
            chain.append(UNKNOWN)
