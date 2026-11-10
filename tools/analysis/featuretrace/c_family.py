"""Statements C++ and JavaScript/TypeScript write with the same grammar shape (log/trace-rules.md, BRANCH): one owner for
reading them, each language's own `statement` and `expr` handed in."""
from __future__ import annotations

from .gd_body import squash
from .source import field_of, named_children


def if_statement(r, st, unit, cond, statement, expr) -> None:
    """`if (test) a else b`: the test is read, then both paths, each under its condition."""
    test = field_of(st, "condition")
    expr(r, test, unit, cond)
    label = "if " + squash(r.src.text(test))
    statement(r, field_of(st, "consequence"), unit, cond + (label,))
    alt = field_of(st, "alternative")
    for sub in named_children(alt) if alt is not None else []:
        statement(r, sub, unit, cond + ("else of " + label,))
