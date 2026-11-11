"""Names the languages' specifications define, which the analysis reads as the languages' own data (log/trace-rules.md,
"The feature tracer is generic"; tests/test_generic.py `LANGUAGE`): never a program's, a library's or a platform's.
"""
from __future__ import annotations

from .js_class import ACCESSORS
from .js_sigs import ARRAY, CALL_MEMBER, CONSTRUCT_MEMBER
from .rules import CONSTRUCTOR, INDEX_MEMBER
from .vocab import SITE_ARRAY, SITE_FUNCTION, SITE_LITERAL, SITE_NEW, SITE_TEXT

# ECMA-262 MakeConstructor: an ordinary function's own property naming the object its `new` gives as [[Prototype]];
# ClassDefinitionEvaluation: a class's, likewise. OrdinaryCreateFromConstructor reads it.
PROTOTYPE = "prototype"
# ECMA-262 MakeConstructor and ClassDefinitionEvaluation: the prototype object's property naming its constructor.
PROTOTYPE_CONSTRUCTOR = CONSTRUCTOR
# ECMA-262 accessor property descriptors: [[Get]] and [[Set]], written `get` and `set` in a class or a literal.
GETTER, SETTER = ACCESSORS
# TypeScript: `T[]` is `Array<T>`; a construct and a call signature are ECMA-262's [[Construct]] and [[Call]]; an
# index signature reads any property name (`js_sigs`, `rules`).
ARRAY_TYPE = ARRAY
CONSTRUCT_SIGNATURE, CALL_SIGNATURE, INDEX_SIGNATURE = CONSTRUCT_MEMBER, CALL_MEMBER, INDEX_MEMBER
# ECMA-262 GetIterator, IteratorNext, IteratorValue: an iterable's @@iterator method (the reader names it as written,
# `[Symbol.iterator]`), the iterator's `next`, the iterator result's `value`.
ITERATOR = "[Symbol.iterator]"
NEXT = "next"
VALUE = "value"
# ECMA-262 Promise Resolve Functions and NewPromiseResolveThenableJob: a thenable is an object with a callable `then`,
# called with the resolving functions; `await` (Await, PromiseResolve) resolves through it.
THEN = "then"
# The intrinsic prototype each kind of value is made with (ECMA-262 ArrayCreate %Array.prototype%,
# OrdinaryFunctionCreate %Function.prototype%, OrdinaryObjectCreate %Object.prototype% for literals and, at the end of
# their chain, class instances, ToObject's wrappers for primitives), by the name TypeScript's lib declares its members.
INTRINSICS = {SITE_ARRAY: "Array", SITE_FUNCTION: "Function", SITE_LITERAL: "Object", SITE_NEW: "Object",
              SITE_TEXT: "String", "string": "String", "number": "Number", "bigint": "BigInt", "symbol": "Symbol",
              "true": "Boolean", "false": "Boolean"}
