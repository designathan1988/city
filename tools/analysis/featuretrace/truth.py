"""What a value can be when the language tests it (log/trace-rules.md, "Narrowing at branch conditions": V1, N1; "What
narrowing showed on the lab app": T1''): its truthiness (MDN "Falsy": `false`, `0`, `-0`, `0n`, `""`, `null`,
`undefined`, `NaN` and every other value truthy), the strings `typeof` can give for it (MDN `typeof`, an exhaustive
table), and whether it can equal a constant. A value of one kind of the language (any number, any string, ...) is of
that kind; a value the tracer cannot decide (the engine's) can be anything; an object of the program, a function, a
container is an object; a class value is either a class (a function) or one of its objects. The values a JS literal
and a JS operator give are the language's of their kind (MDN arithmetic, bitwise, relational and unary operators).
"""
from __future__ import annotations

from . import js_library
from .solver import (BIGINT, CLASS, ELEM, ENGINE, FALSE, FN, FUNCTION, NULL, NUMBER, OBJECT, STRING, SYMBOL, TEXT,
                     TRUE, UNDEFINED)

JS_CONSTANTS = {"true": TRUE, "false": FALSE, "null": NULL, "undefined": UNDEFINED}   # the literal -> its value (V1)
TYPEOF = frozenset({"undefined", "object", "boolean", "number", "bigint", "string", "symbol", "function"})
FALSY = (FALSE, NULL, UNDEFINED, TEXT)        # TEXT alone: the empty string
EITHER_WAY = (NUMBER, STRING, BIGINT)         # a value of these kinds can be falsy (`0`, `NaN`, `""`, `0n`) or not
KINDS = {NUMBER: "number", STRING: "string", BIGINT: "bigint", SYMBOL: "symbol", FUNCTION: "function"}
BOOLEAN = (TRUE, FALSE)
# What a JS literal kind gives (`ir_js.LITERALS`, the results of tests and of the word operators)
NUMERIC = (NUMBER, BIGINT)    # MDN: the arithmetic and bitwise operators give a number or a BigInt
LITERAL_VALUES = {"number": (NUMBER,), "bigint": (BIGINT,), "numeric": NUMERIC, "string": (STRING,),
                  "boolean": BOOLEAN, "undefined": (UNDEFINED,)}
OPERATOR_VALUES = {"+": (NUMBER, BIGINT, STRING), ">>>": (NUMBER,)}    # `+` concatenates too; `>>>` never a BigInt


def _object(text: str) -> bool:
    return text.startswith((CLASS, FN, ELEM)) or text in (OBJECT, FUNCTION) or js_library.is_container(text)


def can_be_truthy(text: str) -> bool:
    return text not in FALSY


def can_be_falsy(text: str) -> bool:
    if text in FALSY or text in EITHER_WAY:
        return True
    return not (_object(text) or text in (TRUE, SYMBOL) or text.startswith(TEXT))


def typeof_results(text: str) -> frozenset:
    """The strings `typeof` can give for a value."""
    if text in KINDS:
        return frozenset({KINDS[text]})
    if text.startswith(CLASS) or text == OBJECT:
        return frozenset({"object", "function"})
    if text.startswith(FN):
        return frozenset({"function"})
    if text.startswith(TEXT):
        return frozenset({"string"})
    if text.startswith(ELEM) or js_library.is_container(text):
        return frozenset({"object"})
    if text in BOOLEAN:
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
    if text == STRING:
        return constant.startswith(TEXT)      # any string can be the one written
    if text in KINDS:
        return False                          # a number, a BigInt, a symbol, a function is none of them
    return not _object(text) or text == ENGINE


def can_differ(text: str, constant: str) -> bool:
    return not (decided(text) and text == constant)


def is_nullish(text: str) -> bool | None:
    """`x == null`: True for `null` and `undefined`, False for what cannot be either, None when it cannot tell."""
    if text in (NULL, UNDEFINED):
        return True
    if decided(text) or _object(text) or text in KINDS:
        return False
    return None


def operator_values(op: str, unary: bool) -> tuple:
    """What a JS arithmetic, bitwise or unary operator gives (O1): unary `+` a number, `-` and `~` a number or a
    BigInt; binary `+` a number, a BigInt or a string, `>>>` a number, every other a number or a BigInt."""
    if unary:
        return (NUMBER,) if op == "+" else NUMERIC
    return OPERATOR_VALUES.get(op, NUMERIC)
