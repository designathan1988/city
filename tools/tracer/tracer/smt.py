"""Formulas (`vocab`) decided by z3, an SMT solver: truths, reals, texts; choices; the operations; the functions of
numbers the languages define, each by its definition (a square root `r` with r * r = x, r >= 0; a whole power a
product; min, max, abs, clamp, floor, ceil, trunc, round, mix, step, smoothstep, fract written with choices and whole
parts); sine, cosine, tangent, arc tangent, exponent and logarithm as functions z3 knows only the bounds of (sine and
cosine within [-1, 1], the exponent above zero). A case z3 gives is checked by the caller on the formula itself.

    decide(formula, seconds) -> {input key: value} | None (no case) | UNKNOWN (z3 cannot tell, or the formula holds
    what z3 does not read)
"""
from __future__ import annotations

from fractions import Fraction

import z3

from .vocab import (ABS, ADD, ALL, ANY, AT, ATAN, ATAN2, CEIL, CLAMP, COS, DATA, DIV, EQ, EXP, FLOOR, FRACT, GE, GT,
                    ITE, LE, LOG, LT, MAX, MIN, MIX, MOD, MUL, NE, NEG, NOT, POW, REAL32, ROUND, SIGN, SIN, SMOOTH,
                    SQRT, STEP, SUB, SYMBOL, TAN, TEXT, TRUNC)

UNKNOWN = object()
BOUNDED = {SIN: (-1, 1), COS: (-1, 1), EXP: (0, None), ATAN: (-2, 2)}     # what each function's values keep to


class Unread(Exception):
    """A formula with what z3 is not given (a text where a number is, a value of data)."""


class Translate:
    def __init__(self):
        self.reals, self.truths, self.texts = {}, {}, {}
        self.functions, self.extra, self.count = {}, [], 0

    def fresh(self, kind):
        self.count += 1
        return kind(f"${self.count}")

    def real_input(self, key):
        if key not in self.reals:
            self.reals[key] = self.fresh(z3.Real)
        return self.reals[key]

    def truth_input(self, key):
        if key not in self.truths:
            self.truths[key] = self.fresh(z3.Bool)
        return self.truths[key]

    def text_input(self, key):
        if key not in self.texts:
            self.texts[key] = self.fresh(z3.String)
        return self.texts[key]

    def function(self, head: str, arity: int):
        if head not in self.functions:
            self.count += 1
            self.functions[head] = z3.Function(f"${self.count}", *([z3.RealSort()] * (arity + 1)))
        return self.functions[head]

    # ---- numbers -------------------------------------------------------------------------------------------------
    def real(self, x):
        if isinstance(x, bool):
            return z3.RealVal(1 if x else 0)
        if isinstance(x, (Fraction, int)):
            x = Fraction(x)
            return z3.RealVal(x.numerator) / z3.RealVal(x.denominator) if x.denominator != 1 else z3.RealVal(
                x.numerator)
        if not isinstance(x, tuple) or not x:
            raise Unread()
        head = x[0]
        if head == SYMBOL:
            return self.real_input(x[1])
        if head == AT:
            return self.real(x[2])
        if head in (TEXT, DATA):
            raise Unread()
        if head == ITE:
            return z3.If(self.truth(x[1]), self.real(x[2]), self.real(x[3]))
        if head in (ALL, ANY, NOT, LT, LE, GT, GE, EQ, NE):
            return z3.If(self.truth(x), z3.RealVal(1), z3.RealVal(0))
        parts = [self.real(y) for y in x[1:]]
        if head == ADD:
            return parts[0] + parts[1]
        if head == SUB:
            return parts[0] - parts[1]
        if head == MUL:
            return parts[0] * parts[1]
        if head == DIV:
            return parts[0] / parts[1]
        if head == NEG:
            return -parts[0]
        if head == MOD:
            return parts[0] - parts[1] * self.trunc(parts[0] / parts[1])
        return self.of_function(head, parts, x)

    @staticmethod
    def trunc(a):
        return z3.If(a >= 0, z3.ToReal(z3.ToInt(a)), -z3.ToReal(z3.ToInt(-a)))

    def of_function(self, head: str, parts: list, x):
        a = parts[0]
        if head == SQRT:
            r = self.fresh(z3.Real)
            self.extra += [r * r == a, r >= 0]
            return r
        if head == POW:
            power = x[2]
            if isinstance(power, Fraction) and power.denominator == 1 and 0 <= power <= 8:
                out = z3.RealVal(1)
                for _ in range(int(power)):
                    out = out * a
                return out
            return self.function(POW, 2)(a, parts[1])
        if head == ABS:
            return z3.If(a >= 0, a, -a)
        if head in (MIN, MAX):
            out = a
            for b in parts[1:]:
                out = z3.If(b < out, b, out) if head == MIN else z3.If(b > out, b, out)
            return out
        if head == FLOOR:
            return z3.ToReal(z3.ToInt(a))
        if head == CEIL:
            return -z3.ToReal(z3.ToInt(-a))
        if head == TRUNC:
            return self.trunc(a)
        if head == ROUND:
            return z3.ToReal(z3.ToInt(a + z3.RealVal(1) / 2))
        if head == CLAMP:
            low, high = parts[1], parts[2]
            kept = z3.If(a < low, low, a)
            return z3.If(kept > high, high, kept)
        if head == SIGN:
            return z3.If(a > 0, z3.RealVal(1), z3.If(a < 0, z3.RealVal(-1), z3.RealVal(0)))
        if head == MIX:
            return a + (parts[1] - a) * parts[2]
        if head == STEP:
            return z3.If(parts[1] < a, z3.RealVal(0), z3.RealVal(1))
        if head == FRACT:
            return a - z3.ToReal(z3.ToInt(a))
        if head == SMOOTH:
            t = (parts[2] - a) / (parts[1] - a)
            t = z3.If(t < 0, z3.RealVal(0), z3.If(t > 1, z3.RealVal(1), t))
            return t * t * (3 - 2 * t)
        if head == REAL32:                       # the nearest 32-bit real: taken as the value (the case is checked)
            return a
        if head in (SIN, COS, TAN, ATAN, EXP, LOG, ATAN2):
            value = self.function(head, len(parts))(*parts)
            low, high = BOUNDED.get(head, (None, None))
            if low is not None:
                self.extra.append(value > low if head == EXP else value >= low)
            if high is not None:
                self.extra.append(value <= high)
            return value
        raise Unread()

    # ---- truths --------------------------------------------------------------------------------------------------
    def text(self, x):
        if isinstance(x, tuple) and x and x[0] == TEXT and isinstance(x[1], str):
            return z3.StringVal(x[1])
        if isinstance(x, tuple) and x and x[0] == SYMBOL:
            return self.text_input(x[1])
        if isinstance(x, tuple) and x and x[0] == AT:
            return self.text(x[2])
        raise Unread()

    def truth(self, x):
        if isinstance(x, bool):
            return z3.BoolVal(x)
        if isinstance(x, (Fraction, int)):
            return z3.BoolVal(Fraction(x) != 0)
        if not isinstance(x, tuple) or not x:
            raise Unread()
        head = x[0]
        if head == AT:
            return self.truth(x[2])
        if head == SYMBOL:
            return self.truth_input(x[1])
        if head == ALL:
            return z3.And(*[self.truth(y) for y in x[1:]])
        if head == ANY:
            return z3.Or(*[self.truth(y) for y in x[1:]])
        if head == NOT:
            return z3.Not(self.truth(x[1]))
        if head == ITE:
            return z3.If(self.truth(x[1]), self.truth(x[2]), self.truth(x[3]))
        if head in (LT, LE, GT, GE, EQ, NE):
            a, b = x[1], x[2]
            texts = [y for y in (a, b) if isinstance(y, tuple) and y and y[0] == TEXT]
            if texts and head in (EQ, NE):
                same = self.text(a) == self.text(b)
                return same if head == EQ else z3.Not(same)
            if head in (EQ, NE) and (truth_like(a) or truth_like(b)):
                same = self.truth(a) == self.truth(b)
                return same if head == EQ else z3.Not(same)
            ra, rb = self.real(a), self.real(b)
            return {LT: ra < rb, LE: ra <= rb, GT: ra > rb, GE: ra >= rb, EQ: ra == rb, NE: ra != rb}[head]
        if head in (TEXT, DATA):
            raise Unread()
        return self.real(x) != 0


def truth_like(x) -> bool:
    from .decide import is_truth
    return is_truth(x)


def value_of(v):
    """A z3 value as the formulas' values: a fraction (exact, or near for an algebraic number), a truth, a text."""
    if z3.is_true(v):
        return True
    if z3.is_false(v):
        return False
    if z3.is_rational_value(v):
        return Fraction(v.numerator_as_long(), v.denominator_as_long())
    if z3.is_algebraic_value(v):
        return Fraction(v.approx(30).as_decimal(30).rstrip("?"))
    if z3.is_string_value(v):
        return v.as_string()
    return None


def decide(formula, seconds: float):
    """Values of the formula's inputs that make it hold, None where none do, UNKNOWN where z3 cannot tell."""
    t = Translate()
    try:
        goal = t.truth(formula)
    except (Unread, z3.Z3Exception):
        return UNKNOWN
    s = z3.Solver()
    s.set(timeout=max(1, int(seconds * 1000)))
    s.add(goal, *t.extra)
    try:
        verdict = s.check()
    except z3.Z3Exception:
        return UNKNOWN
    if verdict == z3.unsat:
        return None
    if verdict != z3.sat:
        return UNKNOWN
    model = s.model()
    values = {}
    for table in (t.reals, t.truths, t.texts):
        for key, var in table.items():
            got = value_of(model.eval(var, model_completion=True))
            if got is not None:
                values[key] = got
    return values
