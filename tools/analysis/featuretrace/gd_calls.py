"""GDScript calls: a call by name or on a receiver, and the engine's calls by name (`emit`, `connect`, `emit_signal`,
`call*`, `Callable(obj, "name")`, `preload`/`load`, `X.new()`), with every `&"name"` handed to a call kept as a name.
"""
from __future__ import annotations

from . import gd_body
from .rules import BIND_CALLS, NAME_CALLS
from .source import field_of, is_identifier, named_children, string_value


def arguments(r, args, unit, cls, cond, skip=()) -> None:
    for i, a in enumerate(named_children(args) if args is not None else []):
        if i in skip:
            continue
        value = string_value(r.src, a)
        if a.type == "string_name" and is_identifier(value):
            r.add(unit, "name", value, a, cond, text=value)
        else:
            gd_body.expr(r, a, unit, cls, cond)


def handler(r, node, unit, cls, cond) -> tuple:
    """What a connect hands over: a method by name, an object's method, a lambda, or Callable(obj, "m")."""
    if node is None:
        return ()
    if node.type == "identifier":
        return ("bare", r.src.text(node))
    if node.type == "lambda":
        return ("unit", r.lambda_unit(node, unit, cls).local_id)
    if node.type == "call" and r.src.text(named_children(node)[0]) == "Callable":
        args = named_children(field_of(node, "arguments"))
        if len(args) >= 2 and string_value(r.src, args[1]) is not None:
            return ("member", chain_of(r, args[0]), string_value(r.src, args[1]))
        return handler(r, args[0], unit, cls, cond) if args else ()
    if node.type == "attribute":
        kids = named_children(node)
        while kids and kids[-1].type == "attribute_call" and r.src.text(named_children(kids[-1])[0]) in BIND_CALLS:
            arguments(r, field_of(kids[-1], "arguments"), unit, cls, cond)
            kids = kids[:-1]
        names = [r.src.text(k) for k in kids]
        if len(kids) == 1:
            return handler(r, kids[0], unit, cls, cond)
        if kids and all(k.type == "identifier" for k in kids):
            return ("member", tuple(names[:-1]), names[-1])
    gd_body.expr(r, node, unit, cls, cond)
    return ()


def chain_of(r, node) -> tuple:
    if node.type == "identifier":
        return (r.src.text(node),)
    if node.type == "attribute" and all(k.type == "identifier" for k in named_children(node)):
        return tuple(r.src.text(k) for k in named_children(node))
    return (gd_body.UNKNOWN,)


def call(r, node, unit, cls, cond) -> None:
    src = r.src
    callee = named_children(node)[0]
    args = field_of(node, "arguments")
    argv = named_children(args) if args is not None else []
    name = src.text(callee)
    first = string_value(src, argv[0]) if argv else None
    if name in ("preload", "load") and first is not None:
        r.add(unit, "load", "", node, cond, text=first)
        return
    if name == "super":
        r.add(unit, "call", unit.name.split(".")[0], node, cond, recv=("super",))
    elif name in NAME_CALLS and is_identifier(first):
        r.add(unit, "name", first, node, cond, recv=("self",), text=first)
        arguments(r, args, unit, cls, cond, skip=(0,))
        return
    elif name == "emit_signal" and first is not None:
        r.add(unit, "emit", first, node, cond, recv=("self",))
        arguments(r, args, unit, cls, cond, skip=(0,))
        return
    elif name == "connect" and first is not None:
        spec = handler(r, argv[1] if len(argv) > 1 else None, unit, cls, cond)
        r.add(unit, "connect", first, node, cond, recv=("self",), handler=spec)
        return
    elif name == "Callable" and len(argv) >= 2 and is_identifier(string_value(src, argv[1])):
        r.add(unit, "value", string_value(src, argv[1]), node, cond, recv=chain_of(r, argv[0]))
        gd_body.expr(r, argv[0], unit, cls, cond)
        return
    elif callee.type == "identifier":
        r.add(unit, "call", name, node, cond, argc=len(argv))
    else:
        gd_body.expr(r, callee, unit, cls, cond)
    arguments(r, args, unit, cls, cond)


def member_call(r, seg, name, chain, unit, cls, cond) -> None:
    src = r.src
    args = field_of(seg, "arguments")
    argv = named_children(args) if args is not None else []
    first = string_value(src, argv[0]) if argv else None
    if name == "emit" and chain and is_identifier(chain[-1]):
        r.add(unit, "emit", chain[-1], seg, cond, recv=chain[:-1])
    elif name == "connect" and chain and is_identifier(chain[-1]) and first is None:
        spec = handler(r, argv[0] if argv else None, unit, cls, cond)
        r.add(unit, "connect", chain[-1], seg, cond, recv=chain[:-1], handler=spec)
        return
    elif name == "connect" and first is not None:
        spec = handler(r, argv[1] if len(argv) > 1 else None, unit, cls, cond)
        r.add(unit, "connect", first, seg, cond, recv=chain, handler=spec)
        return
    elif name in ("disconnect", "is_connected"):
        return
    elif name == "emit_signal" and first is not None:
        r.add(unit, "emit", first, seg, cond, recv=chain)
        arguments(r, args, unit, cls, cond, skip=(0,))
        return
    elif name in NAME_CALLS and is_identifier(first):
        r.add(unit, "name", first, seg, cond, recv=chain, text=first)
        arguments(r, args, unit, cls, cond, skip=(0,))
        return
    elif name == "new":
        r.add(unit, "new", chain[-1] if chain else "", seg, cond, recv=chain[:-1])
    else:
        r.add(unit, "call", name, seg, cond, recv=chain, argc=len(argv))
    arguments(r, args, unit, cls, cond)
