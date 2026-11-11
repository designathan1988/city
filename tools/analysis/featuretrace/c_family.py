"""Statements C++ and JavaScript/TypeScript write with the same grammar shape (log/trace-rules.md, BRANCH): one owner for
reading them, each language's own `statement`, `expr` and condition reader (`js_guards.facts`; none for C++) handed
in."""
from __future__ import annotations

from . import narrowing
from .gd_body import squash
from .source import field_of, named_children


def if_statement(r, st, unit, cond, statement, expr, guards=None, lang: str = "js") -> tuple[list, list]:
    """`if (test) a else b`: the test is read, then both paths, each under its condition and the facts it proves
    (R1). Returns the facts (when true, when false) for what follows the `if`."""
    test = field_of(st, "condition")
    expr(r, test, unit, cond)
    when_true, when_false = guards(r, test, unit) if guards is not None else ([], [])
    label = "if " + squash(r.src.text(test))
    consequence, alt = field_of(st, "consequence"), field_of(st, "alternative")
    narrowing.open(r, when_true, [consequence], lang)
    statement(r, consequence, unit, cond + (label,))
    narrowing.close(r)
    narrowing.open(r, when_false, [alt], lang)
    for sub in named_children(alt) if alt is not None else []:
        statement(r, sub, unit, cond + ("else of " + label,))
    narrowing.close(r)
    return when_true, when_false
