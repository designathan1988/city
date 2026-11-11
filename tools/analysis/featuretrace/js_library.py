"""The ECMAScript built-ins' containers (log/trace-rules.md, "JavaScript and TypeScript values by the languages' own
rules", T2; MDN Array, Set, Map, Promise; TypeScript utility types).

A container named by an annotation is a canonical generic text, its type arguments resolved when the value is made:
`Array<T>` (arrays, read-only arrays, iterables, iterators, generators, array-likes, tuples as `Array<A | B>`),
`Set<T>`, `Map<K, V>` (and `WeakMap`), `Record<K, V>`, `Promise<T>`. Index, iteration and the element calls give the
element types (a Map's iteration gives its `[K, V]` entries, its `get` and `forEach` the values); every other member
of a built-in is the language's value. Containers the program builds itself are the solver's `ELEM` values; the calls
that give one of their elements are `ELEMENT_CALLS`, those that give the same container `SAME_CALLS`.
"""
from __future__ import annotations

from .library import DEREF, ENGINE, ITEM, ITER

EACH = "<each>"                       # what a callback handed to a container's own iteration receives (forEach, map...)
UNKNOWN = "?"
PRIMITIVES = frozenset({"number", "string", "boolean", "bigint", "symbol", "object", "unknown", "any", "never", "void",
                        "null", "undefined"})
CANONICAL = {**dict.fromkeys(("Array", "ReadonlyArray", "Iterable", "IterableIterator", "Iterator", "Generator",
                              "AsyncIterable", "AsyncIterableIterator", "AsyncIterator", "AsyncGenerator", "ArrayLike",
                              "IteratorObject"), "Array"),
             **dict.fromkeys(("Set", "ReadonlySet", "WeakSet"), "Set"),
             **dict.fromkeys(("Map", "ReadonlyMap", "WeakMap"), "Map"),
             "Record": "Record", **dict.fromkeys(("Promise", "PromiseLike"), "Promise")}
UNWRAP = ("Partial", "Readonly", "Required", "NonNullable", "Awaited", "Pick", "Omit")
ELEMENT_CALLS = ("at", "find", "findLast", "pop", "shift")
SAME_CALLS = ("filter", "slice", "concat", "sort", "reverse", "toSorted", "toReversed", "toSpliced", "splice",
              "copyWithin", "fill", "flat", "values", "next")   # `next()`: an iterator result, whose `value` is one
RESULT_VALUE = "value"            # an iterator result's element (MDN Iteration protocols)
HEADS = frozenset(CANONICAL.values())


def is_container(text: str) -> bool:
    return head_args(text)[0] in HEADS


def head_args(text: str) -> tuple[str, list[str]]:
    """`Map<K, V | W>` as ("Map", ["K", "V | W"]); a text with no type arguments as (text, [])."""
    start = text.find("<")
    if start < 0 or not text.endswith(">"):
        return text.strip(), []
    return text[:start].strip(), split_top(text[start + 1:-1], ",")


def split_top(text: str, sep: str) -> list[str]:
    """`text` cut at `sep` outside every bracket."""
    parts, depth, start = [], 0, 0
    for i, c in enumerate(text):
        if c in "<([{":
            depth += 1
        elif c in ">)]}":
            depth -= 1
        elif c == sep and depth == 0:
            parts.append(text[start:i])
            start = i + 1
    parts.append(text[start:])
    return [p.strip() for p in parts if p.strip()]


def canonical(text: str, resolve) -> str:
    """A JS text in its one form: a class of the program as `#key`, a container with its arguments resolved, or "" for
    what has no runtime value (`resolve(name)` gives a name's class key or None)."""
    text = text.strip()
    if text.startswith("#"):
        return text
    head, args = head_args(text)
    if head in HEADS:
        resolved = []
        for arg in args:
            parts = [p for p in (canonical(p, resolve) for p in split_top(arg, "|")) if p]
            resolved.append(" | ".join(parts) if parts else UNKNOWN)
        return f"{head}<{', '.join(resolved)}>"
    if head in PRIMITIVES or head == UNKNOWN or not head:
        return ""
    key = resolve(head)
    return "#" + key if key else ""


def parts_of(arg: str) -> list[str]:
    return [p for p in split_top(arg, "|") if p != UNKNOWN]


def step(kind: str, text: str) -> list[str]:
    """`[]`, iteration or a callback's element on a built-in container (the language's value for the rest)."""
    head, args = head_args(text)
    if head == "Array" and args and kind in (ITEM, ITER, EACH):
        return parts_of(args[0])
    if head == "Set" and args and kind in (ITER, EACH):
        return parts_of(args[0])
    if head == "Map" and len(args) == 2:
        if kind == ITER:
            return [f"Array<{args[0]} | {args[1]}>"]
        if kind == EACH:
            return parts_of(args[1])
    if head == "Record" and len(args) == 2 and kind == ITEM:
        return parts_of(args[1])
    return [] if kind == DEREF else [ENGINE]


def member(name: str, text: str, called: bool) -> list[str]:
    """A member of a built-in container: element calls, the calls that give the container or its parts again."""
    head, args = head_args(text)
    if head == "Record" and len(args) == 2:
        return [] if called else parts_of(args[1])
    if head == "Array" and args and not called and name == RESULT_VALUE:
        return parts_of(args[0])                  # an iterator's `next()` result (`next` keeps the container)
    if not called or not args:
        return [ENGINE]
    if head == "Array":
        if name in ELEMENT_CALLS:
            return parts_of(args[0])
        if name in SAME_CALLS:
            return [text]
        if name == "entries":
            return [f"Array<Array<{args[0]}>>"]
    if head == "Set" and name in ("values", "keys", "add"):
        return [text] if name == "add" else [f"Array<{args[0]}>"]
    if head == "Map" and len(args) == 2:
        if name == "get":
            return parts_of(args[1])
        if name in ("values", "keys"):
            return [f"Array<{args[1] if name == 'values' else args[0]}>"]
        if name == "entries":
            return [f"Array<Array<{args[0]} | {args[1]}>>"]
        if name == "set":
            return [text]
    return [ENGINE]


def awaited(text: str) -> list[str]:
    """What `await` (and `then`'s callback) gives for a value: a promise's type argument, any other value itself."""
    head, args = head_args(text)
    if head == "Promise":
        return parts_of(args[0]) if args else []
    return [text]
