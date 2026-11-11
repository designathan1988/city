"""Names the languages' specifications define, which the analysis reads as the languages' own data (log/trace-rules.md,
"The feature tracer is generic"; tests/test_generic.py `LANGUAGE`): never a program's, a library's or a platform's.
"""
from __future__ import annotations

from .js_class import ACCESSORS
from .rules import CONSTRUCTOR

# ECMA-262 MakeConstructor: an ordinary function's own property naming the object its `new` gives as [[Prototype]];
# ClassDefinitionEvaluation: a class's, likewise. OrdinaryCreateFromConstructor reads it.
PROTOTYPE = "prototype"
# ECMA-262 MakeConstructor and ClassDefinitionEvaluation: the prototype object's property naming its constructor.
PROTOTYPE_CONSTRUCTOR = CONSTRUCTOR
# ECMA-262 accessor property descriptors: [[Get]] and [[Set]], written `get` and `set` in a class or a literal.
GETTER, SETTER = ACCESSORS
