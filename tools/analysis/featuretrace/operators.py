"""EXPR (log/trace-rules.md): what `[]`, a range-for, `->`, `*` and the other operators yield on a value. On a class
of the program they go through its operator (the template's parameters replaced); on a library type the library's
rules answer; on what a container holds they give an element. Each function takes the typer that evaluates values."""
from __future__ import annotations

from .cpp_names import template_args
from .library import DEREF, ITEM, ITER, clean, library_step
from .solver import ELEM, ENGINE, FN

OPERATORS = {ITEM: "operator[]", DEREF: "operator->", ITER: "begin"}
COMPARISONS = ("==", "!=", "<", ">", "<=", ">=", "<=>", "&&", "||", "!", "and", "or", "not", "in", "is")


def step(typer, step: str, text: str, ctx, depth: int) -> list[tuple]:
    """`[]`, a range-for, `->`: an element of a container, what a pointer points at."""
    if text.startswith(ELEM):
        return [(text[len(ELEM):], ctx)]        # an element: by index, by a walk, through an iterator
    if text == ENGINE or text.startswith(FN):
        return [(ENGINE, ctx)]
    key = typer.type_class(text, ctx)
    if key is None:
        return [(library_step(step, text, False), ctx)]
    found = typer.ix.lookup(key, OPERATORS[step])
    if not found:
        return [(text, ctx)]           # a raw pointer's `->`, `*` on a pointer: the object itself
    out = []
    for gid in typer.ix.definitions(found):
        for value, where in typer.returns(typer.ix.units[gid], depth):
            value, replaced = typer.substitute(key, text, value)
            if step == ITER:
                value = library_step(DEREF, value, False) if template_args(clean(value)) else value
            out.append((value, ctx if replaced else where))
    return out


def operator(typer, op: str, text: str, ctx, depth: int) -> list[tuple]:
    """An operator on a value: a comparison is the language's; `*` dereferences; on a class of the program its
    operator's result."""
    if op in COMPARISONS:
        return [(ENGINE, ctx)]
    if op == "*":
        return step(typer, DEREF, text, ctx, depth)
    key = typer.type_class(text, ctx)
    if key and ctx.lang == "cpp":
        found = typer.ix.lookup(key, "operator" + op)
        if found:
            return typer.returns(typer.ix.units[typer.ix.definitions(found)[0]], depth)
    return [(text, ctx)]
