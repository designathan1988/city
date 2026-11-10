"""GDScript bodies: each statement and expression read by its rule (log/trace-rules.md, GDScript rows).

Branches (`if`/`elif`/`else`, `match` with `when` guards, `for`, `while`) are all followed, the condition carried on
every reference under them (BRANCH). Each name that receives a value records the value's tree (FLOW): `var`
initializers and assignments, `for` (the elements walked), `match` bindings (the value matched), `return`. An `is` test
narrows the tested name inside its branch. Member chains are walked step by step, each step's receiver kept as a tree.
"""
from __future__ import annotations

from . import gd_calls, ir_gd
from .ir import put
from .rules import SIGNAL_OPS
from .source import field_of, is_identifier, named_children

COND_WIDTH = 90
STATEMENTS = ("if_statement", "for_statement", "while_statement", "match_statement", "return_statement",
              "variable_statement", "const_statement")


def squash(text: str) -> str:
    text = " ".join(text.split())
    return text if len(text) <= COND_WIDTH else text[:COND_WIDTH - 3] + "..."


def statement(r, st, unit, cls, cond) -> None:
    src = r.src
    kind = st.type
    if kind in ("variable_statement", "const_statement"):
        name = src.text(field_of(st, "name"))
        value, typ = field_of(st, "value"), field_of(st, "type")
        if value is not None and value.type == "lambda":
            unit.lambdas[name] = r.lambda_id(value, unit, cls)
            unit.types[name] = [("callable", unit.lambdas[name])]
            return
        if typ is not None and typ.type == "type":
            unit.types[name] = [("cast", src.text(typ))]
        else:
            unit.types[name] = [r.ir(value, unit, cls)] if value is not None else []
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
        unit.types[src.text(left)] = [("elements", r.ir(right, unit, cls))]
        expr(r, right, unit, cls, cond)
        r.block(field_of(st, "body"), unit, cls, cond + (f"for {src.text(left)} in {squash(src.text(right))}",))
    elif kind == "while_statement":
        test = field_of(st, "condition")
        expr(r, test, unit, cls, cond)
        r.block(field_of(st, "body"), unit, cls, cond + ("while " + squash(src.text(test)),))
    elif kind == "match_statement":
        match(r, st, unit, cls, cond)
    elif kind == "return_statement":
        values = named_children(st)
        unit.returns.append((src.line(st), squash(src.text(values[0])) if values else "",
                             r.ir(values[0], unit, cls) if values else ("literal", "void")))
        for v in values:
            expr(r, v, unit, cls, cond)
    else:
        expr(r, st, unit, cls, cond)


def match(r, st, unit, cls, cond) -> None:
    """`match v:` each pattern a branch; a `var x` binding holds v; a `when` guard is read under its branch."""
    src = r.src
    value = field_of(st, "value")
    expr(r, value, unit, cls, cond)
    matched = r.ir(value, unit, cls)
    body = field_of(st, "body")
    for section in named_children(body) if body is not None else []:
        block = field_of(section, "body")
        pattern = ", ".join(src.text(c) for c in named_children(section) if c is not block)
        label = f"match {squash(src.text(value))}: {squash(pattern)}"
        for part in named_children(section):
            if part.type == "pattern_binding":
                for name in named_children(part):
                    unit.types[src.text(name)] = [matched]
            elif part is not block:
                expr(r, part, unit, cls, cond + (label,))     # a pattern reads the names it compares with
        if block is not None:
            r.block(block, unit, cls, cond + (label,))


def narrow(r, test) -> dict:
    """Inside the branch, a name an `is` test proves (alone or joined by `and`) is of that type. Returns the
    narrowing to put back after the branch."""
    saved, todo = dict(r.narrowed), [test]
    r.narrowed = dict(r.narrowed)
    while todo:
        node = todo.pop()
        if node is None or node.type != "binary_operator":
            continue
        op = r.src.text(field_of(node, "op")).strip()
        left, right = field_of(node, "left"), field_of(node, "right")
        if op in ("and", "&&"):
            todo += [left, right]
        elif op == "is" and left is not None and is_identifier(r.src.text(left)):
            r.narrowed[r.src.text(left)] = r.src.text(right)
    return saved


def expr(r, node, unit, cls, cond) -> None:
    if node is None:
        return
    kind = node.type
    if kind in STATEMENTS:
        statement(r, node, unit, cls, cond)
    elif kind == "call":
        gd_calls.call(r, node, unit, cls, cond)
    elif kind == "attribute":
        attribute(r, node, unit, cls, cond, value=None)
    elif kind in ("assignment", "augmented_assignment"):
        left, right = field_of(node, "left"), field_of(node, "right")
        value = r.ir(right, unit, cls) if kind == "assignment" else r.ir(left, unit, cls)
        target(r, left, unit, cls, cond, value)
        expr(r, right, unit, cls, cond)
    elif kind == "identifier":
        if r.src.text(node) not in ("self", "super"):
            r.add(unit, "read", r.src.text(node), node, cond)
    elif kind == "lambda":
        r.add(unit, "lambda", "", node, cond, target=r.lambda_id(node, unit, cls))
    elif kind not in ("string", "string_name", "get_node", "comment", "node_path"):
        for child in named_children(node):
            expr(r, child, unit, cls, cond)


def target(r, node, unit, cls, cond, value: tuple) -> None:
    """The left side of an assignment: the name written and the value that flows into it."""
    if node is None:
        return
    if node.type == "identifier":
        name = r.src.text(node)
        if name in unit.types:
            unit.types[name].append(value)
        r.add(unit, "write", name, node, cond, args=(value,))
    elif node.type == "attribute":
        attribute(r, node, unit, cls, cond, value=value)
    elif node.type == "subscript":
        kids = named_children(node)
        target(r, kids[0], unit, cls, cond, put(value))
        for k in kids[1:]:
            expr(r, k, unit, cls, cond)
    else:
        expr(r, node, unit, cls, cond)


def attribute(r, node, unit, cls, cond, value: tuple | None) -> None:
    """A member chain, step by step; with `value`, the chain is the left side of an assignment."""
    src = r.src
    kids = named_children(node)
    head, segs = kids[0], kids[1:]
    expr(r, head, unit, cls, cond)
    base = r.ir(head, unit, cls)
    for i, seg in enumerate(segs):
        last = i == len(segs) - 1
        nxt = segs[i + 1] if not last else None
        name = src.text(seg) if seg.type == "identifier" else src.text(named_children(seg)[0])
        if seg.type == "identifier":
            ahead = src.text(named_children(nxt)[0]) if nxt is not None and nxt.type == "attribute_call" else ""
            if ahead not in SIGNAL_OPS:
                written = value is not None and last
                r.add(unit, "write" if written else "read", name, seg, cond, base=base,
                      args=(value,) if written else ())
        elif seg.type == "attribute_call":
            gd_calls.member_call(r, seg, name, base, unit, cls, cond)
        elif seg.type == "attribute_subscript":
            r.add(unit, "read", name, seg, cond, base=base)
            for part in named_children(seg)[1:]:
                expr(r, part, unit, cls, cond)
        base = ir_gd.step(src, base, seg, lambda n: r.lambda_id(n, unit, cls))
