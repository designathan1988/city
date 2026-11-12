"""Whether a function's runs need a context of their own (log/trace-engine-plan.md, Task E7; Zipper, OOPSLA 2018):
only where objects that differ between its callers can reach its outputs — its returns and yields, a store into an
object reached from one of its inputs, or a call handing an input's objects to code that may keep them in another
input. Its inputs are its parameters and `this`; a parameter whose declared type admits only the language's primitive
values carries no object a context could tell apart. A run whose outputs carry neither an input's objects nor objects it
makes itself runs once for every caller, in its closure's environment. Read from the function's own trees only.
The parameter a plain call is told apart by (Wei & Ryder, "Adaptive Context-sensitive Analysis for JavaScript",
ECOOP 2015: 1st-parameter sensitivity, chosen per function; Agesen's Cartesian Product Algorithm, ECOOP 1995): the
first one whose objects can reach the function's returns or yields — a wrapper calling it from one place then still
gives each of its callers what that caller handed it.
"""
from __future__ import annotations

from ..ir_js import CLASS_VALUE as CAST_MADE
from ..js_class import MADE_LITERAL as LITERAL_MADE
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


def carried(an, gid: int) -> int | None:
    """The index of the first parameter whose objects can reach the function's returns or yields; None where none's
    can."""
    memo = an.memo.setdefault(carried, {})
    if gid not in memo:
        unit = an.ix.units[gid]
        outputs = [r[2] for r in unit.returns] + list(unit.yields)
        memo[gid] = next((i for i, name in enumerate(unit.params) if name in Flow(unit).inputs and
                          any(Flow(unit, name, an.ix).carries(tree, True) for tree in outputs)), None)
    return memo[gid]


def primitive(tree) -> bool:
    """Whether a declared type admits only the language's primitive values."""
    if not tree:
        return False
    if tree[0] == T_UNION:
        return all(primitive(part) for part in tree[1])
    return tree[0] == T_VALUES and all(v in PRIMITIVE_VALUES or v.startswith(TEXT) for v in tree[1])


class Flow:
    def __init__(self, unit, only: str | None = None, ix=None):
        """The flows of a function's trees from its inputs: every parameter of no primitive type and `this`, or one
        parameter alone (`only`); with the index, an object literal made here carries what its spreads and its
        properties' values read (CopyDataProperties, PropertyDefinitionEvaluation)."""
        self.unit, self.only, self.ix = unit, only, ix
        types = list(unit.param_types)
        self.inputs = {only} if only is not None else {
            name for i, name in enumerate(unit.params) if not primitive(types[i] if i < len(types) else ())}
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
            return self.only is None
        if kind in FRESH:
            return False
        if kind == CAST:
            made = isinstance(tree[1], str) and tree[1].startswith(CAST_MADE)
            if made and inputs_only and self.ix is not None:
                return any(self.carries(t, True) for t in self.literal_parts(tree[1][len(CAST_MADE):]))
            return False if inputs_only else made
        parts = [p for p in tree[1:] if isinstance(p, tuple)]
        if kind in MADE and not inputs_only:
            return True
        return any(self.carries(p, inputs_only) for p in self.flatten(parts))

    def literal_parts(self, key: str) -> list:
        """An object literal's spreads and its properties' values, as trees read where it is written."""
        cls = self.ix.classes.get(key)
        if cls is None or cls.made != LITERAL_MADE:
            return []
        return list(cls.spreads) + [tree for gids in self.ix.members[key].values() for gid in gids
                                    for tree in self.ix.units[gid].values]

    def flatten(self, parts):
        """The trees inside a tree's parts (a call's arguments are a tuple of trees)."""
        for part in parts:
            if part and isinstance(part[0], tuple):
                yield from part
            else:
                yield part

