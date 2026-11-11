"""What a JavaScript function is (log/trace-rules.md, J2''', N1', N1'', J7'; ECMAScript OrdinaryCallBindThis,
MakeConstructor; MDN `this`, Strict mode, Hoisting): what its `this` is decided by (an arrow's is its enclosing
function's, a method's its receiver's, any other function's what its calls give), whether its code is strict (a module,
a class body, a "use strict" directive, an enclosing function's), whether it is a constructor with a prototype object of
its own (an ordinary function: no arrow, method, async function or generator), and what a hoisted closure writes to a
variable its enclosing function declares later in the text.
"""
from __future__ import annotations

from .model import ClassFact
from .source import field_of, named_children, string_value

USE_STRICT = "use strict"
# What `this` is in a function written as each node (MDN `this`, Arrow function expressions, Method definitions).
ARROW, METHOD, FUNCTION = "arrow", "method", "function"      # what `this` is decided by (`Unit.this_of`)
THIS_OF = {"arrow_function": ARROW, "method_definition": METHOD}


def made(r, node, unit, outer, module) -> None:
    """A function just made: what its `this` is decided by and whether its code is strict."""
    unit.generator = any(c.type == "*" for c in node.children)     # `function*`, `*name()`, `async *name()` (J5)
    unit.asynchronous = any(c.type == "async" for c in node.children)     # an async function (MDN async function)
    unit.this_of = THIS_OF.get(node.type, FUNCTION)
    unit.strict = (outer.strict if outer is not None else module.strict) or in_class(unit.owner) \
        or says_strict(r, field_of(node, "body"))


def says_strict(r, body) -> bool:
    """Whether a script's or a function's body begins with the "use strict" directive (MDN Strict mode)."""
    for st in named_children(body) if body is not None else []:
        if st.type == "comment":
            continue
        inner = (named_children(st) or [None])[0] if st.type == "expression_statement" else None
        if inner is None or inner.type != "string":
            return False
        if string_value(r.src, inner) == USE_STRICT:
            return True
    return False


def in_class(owner: str) -> bool:
    """Whether a function is written in a class body (strict code): its owner is a class, not an object literal, an
    object type or a function's prototype (keys written `<...>` after `#`)."""
    _, mark, rest = owner.partition("#")
    return bool(mark) and not rest.startswith("<")


def module_code(r) -> None:
    """A module's code is strict code (ECMAScript): every function of a file with module syntax."""
    if not r.facts.script:
        for unit in r.facts.units:
            unit.strict = True


def constructor_function(r, node, unit) -> None:
    """An ordinary function (no arrow, method, async function or generator) is a constructor (ECMAScript
    MakeConstructor; MDN new operator): its prototype object is a class of its own, whose objects `new` makes and whose
    properties are what it writes on `this` (N1', N1'')."""
    if unit.this_of != FUNCTION or unit.generator or any(c.type == "async" for c in node.children):
        return
    unit.constructs = f"{r.key}#<new {unit.name}@{unit.line}>"
    r.facts.classes.append(ClassFact(unit.constructs, "js", "", r.key, unit.line))


def late_writes(r) -> None:
    """What a hoisted closure wrote to a variable its enclosing function declares later in the text (J7'): the
    variable is the enclosing function's, and the closure's writes reach it as a closure's do."""
    units = r.facts.units
    for unit in units:
        for name, values in unit.late_writes.items():
            outer = unit
            while outer.outer >= 0 and name not in outer.types:
                outer = units[outer.outer]
            if outer is unit or name not in outer.types:
                continue
            unit.types[name] = unit.types.get(name, []) + list(values)
            unit.captured.add(name)
            outer.written_by.setdefault(name, []).append(unit.local_id)
