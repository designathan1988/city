"""The library's own rules (log/trace-rules.md, EXPR: a library type answers by the library's rules): what the C++
standard library's containers, pointers, iterators and pairs, and the engine's typed containers (`Array[T]`,
`Dictionary[K, V]`), yield for `[]`, `->`, a range-for, `find`/`begin`, `get`/`value`/`front`, `at`, `first`/`second`.
"""
from __future__ import annotations

import re

from .cpp_names import strip_templates, template_args
from .rules import PASS_THROUGH

ENGINE = "<engine>"
ITEM, ITER, DEREF = "[]", "<iter>", "->"
ITERATOR = "std::__iterator"
QUALIFIERS = re.compile(r"\b(const|volatile|static|constexpr|inline|mutable|typename|struct|class|unsigned)\b|[&*]")
ITERATOR_CALLS = ("find", "begin", "end", "cbegin", "cend", "rbegin", "rend", "lower_bound", "upper_bound")
HELD_CALLS = ("get", "value", "front", "back", "top", "lock")
# `.first` / `.second` read of a value of the program that has no such member: the code compiles only if the receiver
# is the (key, value) pair of an associative container holding the value, so `.second` is the value put and `.first`
# the key (not tracked: the language's)
PAIR_PARTS = ("first", "second")


def passes_through(callee: str) -> bool:
    """`std::move(x)`, `std::forward<T>(x)`: a call of the standard library that returns its argument itself."""
    scope, _, name = callee.rpartition("::")
    return scope in ("std", "") and name in PASS_THROUGH


def substitute(params: list, outer: str, member: str) -> tuple[str, bool]:
    """A member's type with the class template's parameters replaced by the arguments `outer` writes."""
    args = template_args(clean(outer)) if params else []
    if not args:
        return member, False
    out = member
    for param, arg in zip(params, args):
        out = re.sub(rf"\b{re.escape(param)}\b", arg, out)
    return out, out != member


def clean(text: str) -> str:
    return " ".join(QUALIFIERS.sub(" ", text).split())


def library_step(step: str, text: str, called: bool) -> str:
    """What a library type yields (the C++ standard library's and the engine's containers' rules)."""
    text = text.replace("[", "<").replace("]", ">")     # GDScript writes type arguments in brackets
    args = [a for a in template_args(clean(text)) if not a.strip().isdigit()]
    if strip_templates(clean(text)) == ITERATOR and args:
        return library_step(ITER, args[0], False) if step in (DEREF, ITEM) else ENGINE
    if not args:
        return ENGINE
    if called and step in ITERATOR_CALLS:
        return f"{ITERATOR}<{text}>"
    if step in (DEREF, ITER) and len(args) == 1 or called and step in HELD_CALLS:
        return args[0]
    if step == ITER:
        return f"std::pair<{args[0]}, {args[1]}>"
    if step == ITEM or called and step == "at":
        return args[-1]
    if not called and step in ("first", "second") and len(args) >= 2:
        return args[0] if step == "first" else args[1]
    return ENGINE
