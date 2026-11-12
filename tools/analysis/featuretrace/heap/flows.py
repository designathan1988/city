"""Whether a function's runs need a context of their own (log/trace-engine-plan.md, Task E7; Zipper, OOPSLA 2018):
only where objects that differ between its callers can reach its outputs — its returns and yields, a store into an
object reached from one of its inputs, or a call handing an input's objects to code that may keep them in another
input. Its inputs are its parameters and `this`; a parameter whose declared type admits only the language's primitive
values carries no object a context could tell apart. A run whose outputs carry neither an input's objects nor objects it
makes itself runs once for every caller, in its closure's environment. Read from the function's own trees only.
"""
from __future__ import annotations

from ..ir_js import CLASS_VALUE as CAST_MADE
from ..solver import BIGINT, FALSE, NULL, NUMBER, STRING, SYMBOL, TEXT, TRUE, UNDEFINED
from ..vocab import (CALL, CALLABLE, CAST, COLLECTION, CONSTRUCT, INVOKE, JSX, LITERAL, NAME, NEW, OP, SELF,
                     STORE, STRNAME, T_UNION, T_VALUES, UNARY, WRITE)

PRIMITIVE_VALUES = (NUMBER, STRING, BIGINT, SYMBOL, TRUE, FALSE, NULL, UNDEFINED)
FRESH = (OP, UNARY, LITERAL, STRNAME)                  # trees whose value is a new primitive, never an operand's
MADE = (CONSTRUCT, COLLECTION, CALLABLE, JSX)          # trees whose value is an object the run makes
STORES, CALLS = (WRITE, STORE), (CALL, INVOKE, NEW)


def contextual(an, gid: int) -> bool:
    memo = an.memo.setdefault(contextual, {})
    if gid not in memo:
        memo[gid] = Flow(an.ix.units[gid]).needs()
    return memo[gid]


def primitive(tree) -> bool:
    """Whether a declared type admits only the language's primitive values."""
    if not tree:
        return False
    if tree[0] == T_UNION:
        return all(primitive(part) for part in tree[1])
    return tree[0] == T_VALUES and all(v in PRIMITIVE_VALUES or v.startswith(TEXT) for v in tree[1])


class Flow:
    def __init__(self, unit):
        self.unit = unit
        types = list(unit.param_types)
        self.inputs = {name for i, name in enumerate(unit.params) if not primitive(types[i] if i < len(types) else ())}
        self.memo, self.open = {}, set()

    def needs(self) -> bool:
        unit = self.unit
        if any(self.carries(tree) for tree in [r[2] for r in unit.returns] + list(unit.yields)):
            return True
        for ref in unit.refs:
            if ref.kind in STORES and ref.base is not None and ref.args:
                if self.carries(ref.base, True) and self.carries(ref.args[0]):
                    return True                    # an input's object keeps what differs between callers
            elif ref.kind in CALLS:
                parts = list(ref.args) + ([ref.base] if ref.base is not None else [])
                if any(self.carries(p, True) for p in parts) and sum(self.carries(p) for p in parts) > 1:
                    return True                    # the callee may keep one of them in another
        return False

    def carries(self, tree, inputs_only: bool = False) -> bool:
        """Whether a tree's value can hold an input's objects (or, unless `inputs_only`, objects the run makes)."""
        if not isinstance(tree, tuple) or not tree:
            return False
        key = (tree, inputs_only)
        if key not in self.memo:
            self.memo[key] = self.value(tree, inputs_only)
        return self.memo[key]

    def value(self, tree, inputs_only: bool) -> bool:
        kind, unit = tree[0], self.unit
        if kind == NAME:
            name = tree[1]
            if name in self.inputs:
                return True
            if name in unit.lambdas:
                return not inputs_only             # a function declared here: a closure of this run
            if name in unit.types and name not in self.open:
                self.open.add(name)
                found = any(self.carries(t, inputs_only) for t in unit.types[name])
                self.open.discard(name)
                return found
            return False                           # a primitive parameter, an enclosing run's or a global name
        if kind == SELF:
            return True
        if kind in FRESH:
            return False
        if kind == CAST:
            return False if inputs_only else isinstance(tree[1], str) and tree[1].startswith(CAST_MADE)
        parts = [p for p in tree[1:] if isinstance(p, tuple)]
        if kind in MADE and not inputs_only:
            return True
        return any(self.carries(p, inputs_only) for p in self.flatten(parts))

    def flatten(self, parts):
        """The trees inside a tree's parts (a call's arguments are a tuple of trees)."""
        for part in parts:
            if part and isinstance(part[0], tuple):
                yield from part
            else:
                yield part

