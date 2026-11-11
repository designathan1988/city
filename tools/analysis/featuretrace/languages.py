"""Names the languages' specifications define, which the analysis reads as the languages' own data (log/trace-rules.md,
"The feature tracer is generic"; tests/test_generic.py `LANGUAGE`): never a program's, a library's or a platform's.
"""
from __future__ import annotations

from .js_class import ACCESSORS
from .js_config import JSX_OPTIONS
from .js_sigs import ARRAY, CALL_MEMBER, CONSTRUCT_MEMBER
from .rules import CONSTRUCTOR, INDEX_MEMBER
from .vocab import (ELEMENTS_FIELD, RESULT_FIELD, SITE_ARRAY, SITE_FUNCTION, SITE_GENERATOR, SITE_LITERAL, SITE_NEW,
                    SITE_PROMISE, SITE_TEXT)

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
# their chain, class instances, ToObject's wrappers for primitives), by the names TypeScript's lib declares its members
# under, in the order the checker reads them (a function's apparent type: CallableFunction, then Function).
INTRINSICS = {SITE_ARRAY: ("Array",), SITE_FUNCTION: ("CallableFunction", "Function"), SITE_LITERAL: ("Object",),
              SITE_NEW: ("Object",), SITE_TEXT: ("String",), "string": ("String",), "number": ("Number",),
              "bigint": ("BigInt",), "symbol": ("Symbol",), "true": ("Boolean",), "false": ("Boolean",),
              SITE_GENERATOR: ("Generator",), SITE_PROMISE: ("Promise",)}
# The internal slot of a generator object (what it yields, its elements) and of a promise (ECMA-262 [[PromiseResult]]),
# and the type parameter of the intrinsic's declared type that holds it (TypeScript's lib: Generator<T, ...>'s T is
# what it yields, Promise<T>'s T what it resolves to).
SLOTS = {SITE_GENERATOR: (ELEMENTS_FIELD, 0), SITE_PROMISE: (RESULT_FIELD, 0)}
# TypeScript's JSX transforms (handbook, JSX; tsconfig `jsx`, `jsxFactory`, `jsxImportSource`): the classic one calls
# the factory named by `jsxFactory` (TypeScript's default React.createElement); the automatic ones call the function
# of the runtime module `<jsxImportSource><suffix>` (default source react); `preserve` and `react-native` leave the
# element to another tool. The children's prop is the property `JSX.ElementChildrenAttribute` declares.
JSX_MODE_OPTION, JSX_FACTORY_OPTION, JSX_SOURCE_OPTION = JSX_OPTIONS
JSX_CLASSIC = ("react",)
JSX_AUTOMATIC = {"react-jsx": ("/jsx-runtime", "jsx"), "react-jsxdev": ("/jsx-dev-runtime", "jsxDEV")}
JSX_FACTORY_DEFAULT = "React.createElement"
JSX_SOURCE_DEFAULT = "react"
JSX_NAMESPACE = "JSX"
JSX_CHILDREN_ATTRIBUTE = "ElementChildrenAttribute"
# A position typed by what every object or every function is constrains nothing of where a value goes.
OPEN_INTRINSICS = (SITE_LITERAL, SITE_FUNCTION)
