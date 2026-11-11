"""GDScript calls (log/trace-rules.md: CALL, NAME, SIGNAL): a call by name or on a receiver, and the engine's calls by
name (`emit`, `connect`, `emit_signal`, `call*`, `Callable(obj, "name")`, `preload`/`load`, `X.new()`), with every
`&"name"` handed to a call kept as a name. Receivers, arguments and handlers are kept as expression trees.
"""
from __future__ import annotations

from . import gd_body
from .ir import put
from .rules import NAME_CALLS
from .model import assign
from .source import field_of, is_identifier, named_children, string_value

PUTS = ("append", "push_back", "push_front", "insert", "append_array", "fill")   # Array: values into its elements


def trees(r, argv, unit, cls) -> tuple:
    return tuple(r.ir(a, unit, cls) for a in argv)


def arguments(r, argv, unit, cls, cond) -> None:
    for a in argv:
        value = string_value(r.src, a)
        if a.type == "string_name" and is_identifier(value):
            r.add(unit, "name", value, a, cond, text=value)
        else:
            gd_body.expr(r, a, unit, cls, cond)


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
        r.add(unit, "call", unit.name.split(".")[0], node, cond, base=("super",), args=trees(r, argv, unit, cls),
              argc=len(argv))
    elif name in NAME_CALLS and is_identifier(first):
        r.add(unit, "name", first, node, cond, base=("self",), text=first, args=trees(r, argv[1:], unit, cls))
        argv = argv[1:]
    elif name == "emit_signal" and first is not None:
        r.add(unit, "emit", first, node, cond, base=("self",), args=trees(r, argv[1:], unit, cls))
        argv = argv[1:]
    elif name == "connect" and first is not None:
        handler = r.ir(argv[1], unit, cls) if len(argv) > 1 else None
        r.add(unit, "connect", first, node, cond, base=("self",), handler=handler)
    elif name == "Callable" and len(argv) >= 2 and is_identifier(string_value(src, argv[1])):
        r.add(unit, "value", string_value(src, argv[1]), node, cond, base=r.ir(argv[0], unit, cls))
        argv = argv[:1]
    elif callee.type == "identifier":
        r.add(unit, "call", name, node, cond, argc=len(argv), args=trees(r, argv, unit, cls))
    else:
        gd_body.expr(r, callee, unit, cls, cond)
        r.add(unit, "invoke", "", node, cond, base=r.ir(callee, unit, cls), args=trees(r, argv, unit, cls))
    arguments(r, argv, unit, cls, cond)


def owner_of(base: tuple) -> tuple[tuple | None, str]:
    """`a.b` as (a, "b"); a plain name as (None, name): the member a signal or a container is."""
    if base[0] == "member":
        return base[1], base[2]
    if base[0] == "name":
        return None, base[1]
    return base, ""


def member_call(r, seg, name, base, unit, cls, cond) -> None:
    src = r.src
    args = field_of(seg, "arguments")
    argv = named_children(args) if args is not None else []
    first = string_value(src, argv[0]) if argv else None
    owner, member = owner_of(base)
    if name == "emit" and member:
        r.add(unit, "emit", member, seg, cond, base=owner, args=trees(r, argv, unit, cls))
    elif name == "connect" and member and first is None:
        r.add(unit, "connect", member, seg, cond, base=owner, handler=r.ir(argv[0], unit, cls) if argv else None)
    elif name == "connect" and first is not None:
        r.add(unit, "connect", first, seg, cond, base=base,
              handler=r.ir(argv[1], unit, cls) if len(argv) > 1 else None)
        argv = argv[2:]
    elif name in ("disconnect", "is_connected"):
        return
    elif name == "emit_signal" and first is not None:
        r.add(unit, "emit", first, seg, cond, base=base, args=trees(r, argv[1:], unit, cls))
        argv = argv[1:]
    elif name in NAME_CALLS and is_identifier(first):
        r.add(unit, "name", first, seg, cond, base=base, text=first, args=trees(r, argv[1:], unit, cls))
        argv = argv[1:]
    elif name == "new":
        r.add(unit, "new", "", seg, cond, base=base, args=trees(r, argv, unit, cls))
    else:
        r.add(unit, "call", name, seg, cond, base=base, argc=len(argv), args=trees(r, argv, unit, cls))
        if name in PUTS and argv and member:
            value = put(r.ir(argv[-1], unit, cls))
            if owner is None:
                assign(r, unit, member, value, shared=False)
            r.add(unit, "write", member, seg, cond, base=owner, args=(value,))
    arguments(r, argv, unit, cls, cond)
