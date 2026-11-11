"""The members of the language's values in JavaScript (log/trace-rules.md, T5; MDN Object coercion, String, Number,
Boolean, BigInt, Symbol, Object.prototype).

A property read on a primitive is read on its wrapper object: the wrapper prototype's member, else Object.prototype's,
else `undefined`. A method read without a call is a function; called, it gives its result by kind; calling a name no
prototype declares throws (no value). A member of null or undefined throws. A string's index is a one-character string
or, past its end, `undefined` (bracket notation); its iteration gives one-character strings; a number, a boolean, a
BigInt or a symbol has no index (`undefined`) and no iteration (it throws). Any other value of the language (an object
or a function it makes, the host's value) answers by the engine.
"""
from __future__ import annotations

from .library import ITEM
from .solver import BIGINT, ENGINE, FALSE, FUNCTION, NULL, NUMBER, STRING, SYMBOL, TEXT, TRUE, UNDEFINED

BOOLEAN = (TRUE, FALSE)
STRINGS = f"Array<{STRING}>"
MATCHES = f"Array<{STRINGS}>"                 # `matchAll`: an iterator of match arrays (an iterator is an Array, T2)
CONSTRUCTOR = "constructor"
OBJECT_PROTOTYPE = {**dict.fromkeys(("hasOwnProperty", "isPrototypeOf", "propertyIsEnumerable"), BOOLEAN),
                    "toLocaleString": (STRING,), "toString": (STRING,)}
OBJECT_PROPERTIES = {"__proto__": (ENGINE,)}
STRING_METHODS = {
    **dict.fromkeys(("charAt", "concat", "normalize", "padEnd", "padStart", "repeat", "replace", "replaceAll",
                     "slice", "substring", "substr", "toLowerCase", "toUpperCase", "toLocaleLowerCase",
                     "toLocaleUpperCase", "trim", "trimStart", "trimEnd", "trimLeft", "trimRight", "toString",
                     "valueOf", "toWellFormed", "anchor", "big", "blink", "bold", "fixed", "fontcolor", "fontsize",
                     "italics", "link", "small", "strike", "sub", "sup"), (STRING,)),
    **dict.fromkeys(("charCodeAt", "indexOf", "lastIndexOf", "localeCompare", "search"), (NUMBER,)),
    **dict.fromkeys(("includes", "startsWith", "endsWith", "isWellFormed"), BOOLEAN),
    "at": (STRING, UNDEFINED), "codePointAt": (NUMBER, UNDEFINED), "split": (STRINGS,), "match": (STRINGS, NULL),
    "matchAll": (MATCHES,), "[Symbol.iterator]": (STRINGS,),
}
KINDS = {          # kind: (its value, its wrapper prototype's methods by result, its own properties)
    "string": (STRING, STRING_METHODS, {"length": (NUMBER,)}),
    "number": (NUMBER, {**dict.fromkeys(("toFixed", "toPrecision", "toExponential", "toString", "toLocaleString"),
                                        (STRING,)), "valueOf": (NUMBER,)}, {}),
    "boolean": (None, {"toString": (STRING,), "valueOf": BOOLEAN}, {}),
    "bigint": (BIGINT, {"toString": (STRING,), "toLocaleString": (STRING,), "valueOf": (BIGINT,)}, {}),
    "symbol": (SYMBOL, {"toString": (STRING,), "valueOf": (SYMBOL,), "[Symbol.toPrimitive]": (SYMBOL,)},
               {"description": (STRING, UNDEFINED)}),
}
NULLISH = "nullish"


def kind(text: str) -> str | None:
    """A primitive's kind (`nullish` for null and undefined); None for any other value."""
    if text.startswith(TEXT) or text == STRING:
        return "string"
    if text in BOOLEAN:
        return "boolean"
    if text in (NULL, UNDEFINED):
        return NULLISH
    return {NUMBER: "number", BIGINT: "bigint", SYMBOL: "symbol"}.get(text)


def member(text: str, name: str, called: bool) -> list[str]:
    """What reading member `name` of a value of the language gives, or calling it (`called`)."""
    k = kind(text)
    if k is None:
        return [ENGINE]
    if k == NULLISH:
        return []                                       # a TypeError
    own, methods, properties = KINDS[k]
    if k == "string" and name.isdigit():                # `s["0"]`: an index (MDN String: character access)
        return [] if called else [STRING, UNDEFINED]
    properties = {**OBJECT_PROPERTIES, **properties}
    if name in properties:
        return [] if called else list(properties[name])
    if name == CONSTRUCTOR:                             # the wrapper's function: called, a conversion to its kind
        return list(BOOLEAN if own is None else (own,)) if called else [FUNCTION]
    found = methods.get(name) or OBJECT_PROTOTYPE.get(name)
    if found is None:
        return [] if called else [UNDEFINED]            # no such property: undefined; called, a TypeError
    return list(found) if called else [FUNCTION]


def apparent(text: str, name: str) -> bool:
    """Whether a primitive's wrapper has member `name` (its apparent type; TypeScript handbook, Type Compatibility)."""
    k = kind(text)
    if k is None or k == NULLISH:
        return False
    _, methods, properties = KINDS[k]
    return name in methods or name in properties or name in OBJECT_PROTOTYPE or name in OBJECT_PROPERTIES         or name == CONSTRUCTOR or k == "string" and name.isdigit()


def step(text: str, kind_of_step: str) -> list[str]:
    """An index (`ITEM`) or the iteration of a value of the language."""
    k = kind(text)
    if k is None:
        return [ENGINE]
    if k == "string":
        return [STRING, UNDEFINED] if kind_of_step == ITEM else [STRING]
    return [UNDEFINED] if kind_of_step == ITEM and k != NULLISH else []


def calls(text: str) -> list[str]:
    """Calling a value of the language: a primitive throws; an object or a function of the language is the engine's."""
    return [] if kind(text) is not None else [ENGINE]
