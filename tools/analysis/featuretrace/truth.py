"""What a value can be when the language tests it (log/trace-rules.md, "Narrowing at branch conditions": V1, N1): its
truthiness (MDN "Falsy": `false`, `0`, `-0`, `0n`, `""`, `null`, `undefined`, `NaN` and every other value truthy), the
strings `typeof` can give for it (MDN `typeof`, an exhaustive table), and whether it can equal a constant. A value the
tracer cannot decide (the engine's, a number) can be anything; an object of the program, a function, a container
is an object; a class value is either a class (a function) or one of its objects.
"""
from __future__ import annotations

from . import js_library
from .solver import CLASS, ELEM, ENGINE, FALSE, FN, NULL, TEXT, TRUE, UNDEFINED

JS_CONSTANTS = {"true": TRUE, "false": FALSE, "null": NULL, "undefined": UNDEFINED}   # the literal -> its value (V1)
TYPEOF = frozenset({"undefined", "object", "boolean", "number", "bigint", "string", "symbol", "function"})
FALSY = (FALSE, NULL, UNDEFINED, TEXT)        # TEXT alone: the empty string


def _object(text: str) -> bool:
    return text.startswith((CLASS, FN, ELEM)) or js_library.is_container(text)


def can_be_truthy(text: str) -> bool:
    return text not in FALSY


def can_be_falsy(text: str) -> bool:
    return text in FALSY or not (_object(text) or text == TRUE or text.startswith(TEXT))


def typeof_results(text: str) -> frozenset:
    """The strings `typeof` can give for a value."""
    if text.startswith(CLASS):
        return frozenset({"object", "function"})
    if text.startswith(FN):
        return frozenset({"function"})
    if text.startswith(TEXT):
        return frozenset({"string"})
    if text.startswith(ELEM) or js_library.is_container(text):
        return frozenset({"object"})
    if text in (TRUE, FALSE):
        return frozenset({"boolean"})
    if text == NULL:
        return frozenset({"object"})
    if text == UNDEFINED:
        return frozenset({"undefined"})
    return TYPEOF


def decided(text: str) -> bool:
    """A value whose identity is its value: a constant of the language, a string the program writes."""
    return text in (TRUE, FALSE, NULL, UNDEFINED) or text.startswith(TEXT)


def can_equal(text: str, constant: str) -> bool:
    """Whether a value can be strictly equal (`===`) to a constant of the language or a string."""
    if decided(text):
        return text == constant
    return not _object(text) or text == ENGINE


def can_differ(text: str, constant: str) -> bool:
    return not (decided(text) and text == constant)


def is_nullish(text: str) -> bool | None:
    """`x == null`: True for `null` and `undefined`, False for what cannot be either, None when it cannot tell."""
    if text in (NULL, UNDEFINED):
        return True
    if decided(text) or _object(text):
        return False
    return None
