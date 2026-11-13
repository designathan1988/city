"""Deciding a formula of `conditions`: whether some values of its inputs make it hold, and which (a case). The tracer's
own, exact on what it reads: truths combined by `&&`, `||`, `!` and a choice; numbers compared after adding them and
multiplying them by numbers (linear arithmetic over the rationals, by Fourier-Motzkin elimination); texts compared
for being equal. A product of two inputs, a remainder, a division by an input are inputs of their own. Numbers are
rationals: a case may need a fraction where the program keeps whole numbers (said by the case's values).

    satisfy(formula) -> Case | None (no values make it hold) | UNDECIDED (more cases than looked at)
"""
from __future__ import annotations

import math
import time
from dataclasses import dataclass, field
from fractions import Fraction
from pathlib import Path

from .vocab import (ABS, ADD, ALL, ANY, AT, CEIL, CLAMP, COS, DATA, DIV, EQ, EXP, FLOOR, FUNCTIONS, GE, GT, ITE, LE,
                    LOG, LT, MAX, MIN, MOD, MUL, NE, NEG, NOT, POW, ROUND, ROUNDING, SIN, SQRT, SUB, SYMBOL, TEXT, TRUNC,
                    UNDECIDED, BY_OWN, BY_Z3)

try:                                     # the SMT solver, where it is installed; the tracer's own solver else
    from . import smt
except ImportError:
    smt = None

LIMIT = 20000                    # the most conjunctions one question looks at
SECONDS = 0.5                    # the longest one question may take
GROWTH = 4000                    # the most inequalities an elimination keeps
OTHER = "(another text)"
# The kinds of literal a formula comes down to (marks, not names).
TRUTH, BOUND, SAME = "?", "<=>", "=\""


@dataclass
class Case:
    values: dict                 # input key -> its value
    shown: dict                  # input key -> as written
    where: list                  # the places of the conditions the case passes through
    whole: bool = True           # every number of the case is a whole number
    rounding: bool = False       # the formula divides or writes a fraction: floating point may round otherwise
    approximate: bool = False    # found by intervals: an equality holds within `NEAR`, not exactly
    decided_by: str = BY_OWN     # which solver decided it

    def text(self) -> str:
        if not self.values:
            return "in every case" + (ROUNDING if self.rounding else "")
        return ", ".join(f"{self.shown.get(k, k)} = {show(v)}" for k, v in sorted(
            self.values.items(), key=lambda kv: str(self.shown.get(kv[0], kv[0])))) + (ROUNDING if self.rounding else "") \
            + (" (found by intervals: an equality holds within 1e-9)" if self.approximate else "")


def show(v) -> str:
    if isinstance(v, bool):
        return "true" if v else "false"
    if isinstance(v, Fraction):
        return str(v.numerator) if v.denominator == 1 else f"{v.numerator}/{v.denominator} ({float(v):g})"
    return repr(v)


# ---- reading a formula --------------------------------------------------------------------------------------------
def is_truth(x) -> bool:
    """Whether a formula is a truth (not a number, not a text, not an input of either kind)."""
    if isinstance(x, bool):
        return True
    if not isinstance(x, tuple):
        return False
    if x[0] == AT:
        return is_truth(x[2])
    if x[0] in (ALL, ANY, NOT, LT, LE, GT, GE, EQ, NE):
        return True
    return x[0] == ITE and (is_truth(x[2]) or is_truth(x[3]))


def symbols(x, out=None) -> dict:
    """Every input of a formula: key -> as written."""
    out = {} if out is None else out
    if isinstance(x, tuple):
        if x[0] == SYMBOL:
            out[x[1]] = x[2]
        elif x[0] == AT:
            symbols(x[2], out)
        elif x[0] not in (TEXT, DATA):
            for y in x[1:]:
                symbols(y, out)
    return out


def render(x) -> str:
    """A formula as code writes it."""
    if isinstance(x, bool):
        return "true" if x else "false"
    if isinstance(x, Fraction):
        return show(x)
    if not isinstance(x, tuple):
        return str(x)
    head = x[0]
    if head == SYMBOL:
        return x[2]
    if head == TEXT:
        return repr(x[1])
    if head == DATA:
        return Path(x[1]).name + "".join(f"[{k!r}]" for k in x[2])
    if head == AT:
        return render(x[2])
    if head == NOT:
        return f"!({render(x[1])})"
    if head == NEG:
        return f"-({render(x[1])})"
    if head == ITE:
        return f"({render(x[1])} ? {render(x[2])} : {render(x[3])})"
    if head in (ALL, ANY):
        return "(" + f" {head} ".join(render(y) for y in x[1:]) + ")"
    if head in FUNCTIONS:            # a function of numbers: every argument it holds (one to three; min and max more)
        return f"{head}(" + ", ".join(render(y) for y in x[1:]) + ")"
    return f"({render(x[1])} {head} {render(x[2])})"


# ---- linear terms -------------------------------------------------------------------------------------------------
def linear(x, terms: dict):
    """A number formula as (coefficients by input, constant), or None where it is no number (a truth, a text). An
    operation that is not linear is an input of its own, written into `terms`."""
    if isinstance(x, bool):
        return None
    if isinstance(x, Fraction):
        return {}, x
    if isinstance(x, int):
        return {}, Fraction(x)
    if not isinstance(x, tuple):
        return None
    head = x[0]
    if head == SYMBOL:
        terms.setdefault(x[1], x[2])
        return {x[1]: Fraction(1)}, Fraction(0)
    if head == AT:
        return linear(x[2], terms)
    if head == TEXT or is_truth(x):
        return None
    if head == NEG:
        inner = linear(x[1], terms)
        return None if inner is None else scale(inner, Fraction(-1))
    if head in (ADD, SUB):
        a, b = linear(x[1], terms), linear(x[2], terms)
        if a is None or b is None:
            return None
        return plus(a, b if head == ADD else scale(b, Fraction(-1)))
    if head in (MUL, DIV):
        a, b = linear(x[1], terms), linear(x[2], terms)
        if a is not None and b is not None:
            if head == MUL and not a[0]:
                return scale(b, a[1])
            if not b[0] and (head == MUL or b[1] != 0):
                return scale(a, b[1] if head == MUL else 1 / b[1])
    if head in (MUL, DIV, MOD) or head in FUNCTIONS:     # not linear: an input of its own, its formula kept
        key = render(x)
        terms.setdefault(key, x)
        return {key: Fraction(1)}, Fraction(0)
    return None


def scale(a, k: Fraction):
    return {v: c * k for v, c in a[0].items()}, a[1] * k


def plus(a, b):
    out = dict(a[0])
    for v, c in b[0].items():
        out[v] = out.get(v, Fraction(0)) + c
    return {v: c for v, c in out.items() if c != 0}, a[1] + b[1]


def lifted(x):
    """The first choice inside a number formula: (its test, the formula when it holds, when not), or None."""
    if not isinstance(x, tuple) or x[0] in (SYMBOL, TEXT, DATA):
        return None
    if x[0] == ITE:
        return x[1], x[2], x[3]
    if x[0] == AT:
        return lifted(x[2])
    for i, y in enumerate(x[1:], 1):
        found = lifted(y)
        if found is not None:
            test, yes, no = found
            return test, x[:i] + (yes,) + x[i + 1:], x[:i] + (no,) + x[i + 1:]
    return None


# ---- the literals a formula comes down to -------------------------------------------------------------------------
@dataclass(frozen=True)
class Literal:
    kind: str                    # TRUTH, BOUND, SAME
    key: object                  # an input (TRUTH, SAME); for BOUND, the coefficients as sorted pairs
    holds: bool = True           # TRUTH, SAME: holds or not
    const: Fraction = Fraction(0)    # BOUND: sum + const (rel) 0
    rel: str = LE                # BOUND: LT, LE or EQ
    text: object = None          # SAME: the text
    where: frozenset = frozenset()


def bound(a, b, rel: str, where, terms) -> object:
    """`a rel b` for two number formulas, as literals (rel among <, <=, ==, !=, >, >=)."""
    la, lb = linear(a, terms), linear(b, terms)
    if la is None or lb is None:
        return None
    diff = plus(la, scale(lb, Fraction(-1)))
    if rel in (GT, GE):
        diff, rel = scale(diff, Fraction(-1)), LT if rel == GT else LE
    coeffs, const = diff
    if not coeffs:
        holds = {LT: const < 0, LE: const <= 0, EQ: const == 0, NE: const != 0}[rel]
        return holds
    key = tuple(sorted(coeffs.items(), key=lambda kv: str(kv[0])))
    if rel == NE:
        return (ANY, Literal(BOUND, key, True, const, LT, None, where),
                Literal(BOUND, tuple((v, -c) for v, c in key), True, -const, LT, None, where))
    return Literal(BOUND, key, True, const, rel, None, where)


def normal(x, holds: bool, where: frozenset, terms: dict):
    """A formula as `&&` and `||` of literals (negations pushed down), each literal knowing the places of the
    conditions it comes from."""
    if isinstance(x, bool):
        return x == holds
    if isinstance(x, (Fraction, int)):
        return (x != 0) == holds
    head = x[0]
    if head == AT:
        return normal(x[2], holds, where | {x[1]}, terms)
    if head == NOT:
        return normal(x[1], not holds, where, terms)
    if head in (ALL, ANY):
        both = ALL if (head == ALL) == holds else ANY
        return (both,) + tuple(normal(y, holds, where, terms) for y in x[1:])
    if head == ITE:
        test, yes, no = x[1], x[2], x[3]
        picked = (ANY, (ALL, test, yes), (ALL, (NOT, test), no))
        return normal(picked, holds, where, terms)
    if head == SYMBOL:
        return Literal(TRUTH, x[1], holds, where=where)
    if head == TEXT:
        return bool(x[1]) == holds
    if head in (LT, LE, GT, GE, EQ, NE):
        a, b = x[1], x[2]
        for side in (0, 1):
            found = lifted((a, b)[side])
            if found is not None:
                test, yes, no = found
                pair = (yes, b) if side == 0 else (a, yes)
                other = (no, b) if side == 0 else (a, no)
                choice = (ANY, (ALL, test, (head,) + pair), (ALL, (NOT, test), (head,) + other))
                return normal(choice, holds, where, terms)
        rel = head if holds else {LT: GE, LE: GT, GT: LE, GE: LT, EQ: NE, NE: EQ}[head]
        if rel in (EQ, NE):
            texts = [y for y in (a, b) if isinstance(y, tuple) and y[0] == TEXT]
            if texts:
                other = b if texts[0] is a else a
                if isinstance(other, tuple) and other[0] == TEXT:
                    return (other[1] == texts[0][1]) == (rel == EQ)
                key = other[1] if isinstance(other, tuple) and other[0] == SYMBOL else render(other)
                terms.setdefault(key, render(other))
                return Literal(SAME, key, rel == EQ, text=texts[0][1], where=where)
            if is_truth(a) or is_truth(b):                       # truths equal: both hold or neither
                same = (ANY, (ALL, a, b), (ALL, (NOT, a), (NOT, b)))
                return normal(same if rel == EQ else (NOT, same), True, where, terms)
        made = bound(a, b, rel, where, terms)
        if made is not None:
            return made
        key = render(x)
        terms.setdefault(key, key)
        return Literal(TRUTH, key, holds, where=where)
    if head in (ADD, SUB, MUL, DIV, MOD, NEG):                   # a number read as a truth: not zero
        return normal((NE, x, Fraction(0)), holds, where, terms)
    key = render(x)
    terms.setdefault(key, key)
    return Literal(TRUTH, key, holds, where=where)


# ---- the conjunctions ---------------------------------------------------------------------------------------------
class Budget:
    """How many conjunctions, and how long, one question may take: past either, it is not decided (and said)."""
    def __init__(self):
        self.left = LIMIT
        self.until = time.monotonic() + SECONDS

    def spend(self) -> bool:
        self.left -= 1
        if self.left % 64 == 0 and time.monotonic() > self.until:
            self.left = -1
        return self.left >= 0


def edges(n: Literal) -> list:
    """What a bound on one input says of it: [(input, below (True) or above, the number, strict)]."""
    if n.kind != BOUND or len(n.key) != 1:
        return []
    (v, c), = n.key
    at = -n.const / c                            # c*v + const (rel) 0  ->  v (rel') at
    strict = n.rel == LT
    if n.rel == EQ:
        return [(v, True, at, False), (v, False, at, False)]
    return [(v, c > 0, at, strict)]              # c > 0: v below at; c < 0: v above at


def conjunctions(n, budget: Budget, sofar: tuple = ()):
    """Every conjunction of literals that makes a normal formula hold, each with what came before it."""
    if n is True:
        yield sofar
        return
    if n is False:
        return
    if isinstance(n, Literal):
        if budget.spend() and not clashes(sofar, n):
            yield sofar + (n,)
        return
    if n[0] == ANY:
        for y in n[1:]:
            yield from conjunctions(y, budget, sofar)
            if budget.left < 0:
                return
        return
    yield from every(list(n[1:]), budget, sofar)


def every(parts: list, budget: Budget, sofar: tuple):
    if not parts:
        yield sofar
        return
    for got in conjunctions(parts[0], budget, sofar):
        if not budget.spend():
            return
        yield from every(parts[1:], budget, got)


def clashes(sofar: tuple, n: Literal) -> bool:
    """A literal against one already taken of the same truth input or text, or a bound against the bounds already
    taken on the same input: no case has both (a branch cut as soon as it cannot hold)."""
    mine = edges(n)
    for m in sofar:
        for v, below, at, strict in mine:
            for w, other_below, other_at, other_strict in edges(m):
                if v != w or below == other_below:
                    continue
                hi, lo = (at, other_at) if below else (other_at, at)
                hi_strict, lo_strict = (strict, other_strict) if below else (other_strict, strict)
                if lo > hi or (lo == hi and (hi_strict or lo_strict)):
                    return True
        if m.kind == n.kind == TRUTH and m.key == n.key and m.holds != n.holds:
            return True
        if m.kind == n.kind == SAME and m.key == n.key:
            if m.holds and n.holds and m.text != n.text:
                return True
            if m.holds != n.holds and m.text == n.text:
                return True
    return False


# ---- numbers: the simplex method, exact -----------------------------------------------------------------------------
def solve_bounds(rows: list, until: float | None = None):
    """Values for the inputs of rows (coefficients dict, constant, strict) each meaning `sum + const < 0` (strict) or
    `<= 0`, or None where no values do; UNDECIDED past the time given. By the simplex method over the rationals
    (Bland's rule, two phases): each input the difference of two parts not below zero; the strict rows hold with a
    margin `t` the method makes as large as it can (they all hold if and only if it can be made above zero). The values
    found are made whole numbers where that keeps every row."""
    names = sorted({v for row in rows for v in row[0]}, key=str)
    if not names:
        return {} if all(row[1] < 0 or (not row[2] and row[1] == 0) for row in rows) else None
    strict = any(row[2] for row in rows)
    width = 2 * len(names) + (1 if strict else 0)
    at = {v: i for i, v in enumerate(names)}
    matrix, bound = [], []
    for coeffs, const, is_strict in rows:
        line = [Fraction(0)] * width
        for v, c in coeffs.items():
            line[2 * at[v]] += c
            line[2 * at[v] + 1] -= c
        if is_strict:
            line[-1] = Fraction(1)
        matrix.append(line)
        bound.append(-const)
    if strict:                                   # the margin is at most one
        line = [Fraction(0)] * width
        line[-1] = Fraction(1)
        matrix.append(line)
        bound.append(Fraction(1))
    aim = [Fraction(0)] * width
    if strict:
        aim[-1] = Fraction(1)
    found = simplex(matrix, bound, aim, until)
    if found is None or found == UNDECIDED:
        return found
    if strict and found[-1] <= 0:
        return None
    values = {v: found[2 * at[v]] - found[2 * at[v] + 1] for v in names}
    whole = {v: Fraction(round(x)) for v, x in values.items()}
    return whole if all(keeps(row, whole) for row in rows) else values


def keeps(row, values: dict) -> bool:
    coeffs, const, strict = row
    total = const + sum(c * values[v] for v, c in coeffs.items())
    return total < 0 or (not strict and total == 0)


def simplex(matrix: list, bound: list, aim: list, until: float | None):
    """Some z >= 0 with matrix z <= bound making aim z as large as it can be, or None where no z keeps the rows;
    UNDECIDED past the time given."""
    m, n = len(matrix), len(aim)
    width = n + m
    table, basis, artificial = [], [], []
    for i, (line, b) in enumerate(zip(matrix, bound)):
        row = list(line) + [Fraction(1) if j == i else Fraction(0) for j in range(m)]
        if b < 0:
            row, b = [-x for x in row], -b
            artificial.append(i)
        table.append(row + [b])
    k = len(artificial)
    for i, row in enumerate(table):              # the artificial columns, one for each row below zero
        rhs = row.pop()
        row += [Fraction(1) if i == artificial[a] else Fraction(0) for a in range(k)] + [rhs]
        basis.append(width + artificial.index(i) if i in artificial else n + i)
    total = width + k
    if k:
        cost = [Fraction(0)] * width + [Fraction(-1)] * k
        if pivot_all(table, basis, cost, total, until) == UNDECIDED:
            return UNDECIDED
        if sum(table[i][-1] for i, j in enumerate(basis) if j >= width) > 0:
            return None
        for i, j in enumerate(basis):            # an artificial left in the basis at zero: out
            if j >= width:
                swap = next((c for c in range(width) if table[i][c] != 0), None)
                if swap is not None:
                    pivot(table, basis, i, swap)
        for row in table:
            del row[width:width + k]
        keep = [i for i, j in enumerate(basis) if j < width]
        table[:] = [table[i] for i in keep]
        basis[:] = [basis[i] for i in keep]
    cost = list(aim) + [Fraction(0)] * m
    if pivot_all(table, basis, cost, width, until) == UNDECIDED:
        return UNDECIDED
    z = [Fraction(0)] * width
    for i, j in enumerate(basis):
        z[j] = table[i][-1]
    return z[:n]


def pivot_all(table: list, basis: list, cost: list, columns: int, until: float | None):
    """Pivots until no column improves the aim (Bland's rule: the first column that does, the first row that
    limits it)."""
    while True:
        if until is not None and time.monotonic() > until:
            return UNDECIDED
        enter = None
        for j in range(columns):
            if j in basis:
                continue
            reduced = cost[j] - sum(cost[basis[i]] * table[i][j] for i in range(len(table)))
            if reduced > 0:
                enter = j
                break
        if enter is None:
            return True
        leave, best = None, None
        for i, row in enumerate(table):
            if row[enter] > 0:
                ratio = row[-1] / row[enter]
                if best is None or ratio < best or (ratio == best and basis[i] < basis[leave]):
                    leave, best = i, ratio
        if leave is None:                        # unbounded: the aim grows without end (it is bounded here)
            return True
        pivot(table, basis, leave, enter)


def pivot(table: list, basis: list, i: int, j: int) -> None:
    lead = table[i][j]
    table[i] = [x / lead for x in table[i]]
    for r, row in enumerate(table):
        if r != i and row[j] != 0:
            factor = row[j]
            table[r] = [x - factor * y for x, y in zip(row, table[i])]
    basis[i] = j


def pick(lo, lo_strict, hi, hi_strict) -> Fraction:
    """A value between two bounds: zero, else a whole number, else the middle."""
    def inside(x) -> bool:
        return (lo is None or x > lo or (not lo_strict and x == lo)) and \
            (hi is None or x < hi or (not hi_strict and x == hi))
    if inside(Fraction(0)):
        return Fraction(0)
    near = set()
    for edge in (lo, hi):
        if edge is not None:
            near |= {math.floor(edge) - 1, math.floor(edge), math.ceil(edge), math.ceil(edge) + 1}
    for x in sorted(near, key=abs):
        if inside(Fraction(x)):
            return Fraction(x)
    return (lo + hi) / 2 if lo is not None and hi is not None else (lo if lo is not None else hi)


def theory(lits: tuple, terms: dict, until: float | None = None):
    """The values of a conjunction of literals, or None; UNDECIDED where the numbers are too many to decide."""
    values, rows = {}, []
    texts = {}
    for n in lits:
        if n.kind == TRUTH:
            values[n.key] = n.holds
        elif n.kind == SAME:
            pos, neg = texts.setdefault(n.key, (set(), set()))
            (pos if n.holds else neg).add(n.text)
        else:
            coeffs = dict(n.key)
            if n.rel == EQ:
                rows.append((coeffs, n.const, False))
                rows.append(({v: -c for v, c in coeffs.items()}, -n.const, False))
            else:
                rows.append((coeffs, n.const, n.rel == LT))
    for key, (pos, neg) in texts.items():
        if len(pos) > 1 or pos & neg:
            return None
        values[key] = next(iter(pos)) if pos else OTHER
    if rows:
        found = solve_bounds(rows, until)
        if found is None or found == UNDECIDED:
            return found
        values.update(found)
    return values


MEMO = {}                        # a formula's text -> its answer (a pure function of the formula: kept between runs)


def satisfy(formula):
    """Some values of the inputs that make the formula hold (a `Case`), None if none do, UNDECIDED if it could not
    tell within its limits. Answered once for each formula (`MEMO`)."""
    key = repr(canon(formula))
    if key not in MEMO:
        MEMO[key] = solve(formula)
    return MEMO[key]


def canon(x):
    """A formula with each input named by how it is shown and where (not by the number of its definition, which any
    file added or removed changes)."""
    if isinstance(x, tuple) and x:
        if x[0] == SYMBOL:
            return (SYMBOL, x[2], x[3])
        if x[0] in (TEXT, DATA):
            return x
        return (x[0],) + tuple(canon(y) for y in x[1:])
    return x


def inexact(x) -> bool:
    """Whether a formula divides or writes a number that is no whole number (where floating point rounds)."""
    if isinstance(x, Fraction):
        return x.denominator != 1
    if isinstance(x, tuple) and x and x[0] not in (SYMBOL, TEXT, DATA):
        return x[0] in (DIV, MOD) or any(inexact(y) for y in x[1:])
    return False


def solve(formula):
    """z3 first, where it is installed: a case it gives is kept where the formula holds on it, checked exactly; no case
    from it is kept as it is. Where z3 is not there, cannot tell, or gives a case the check refuses, the tracer's own
    solver answers. Each case says which decided it."""
    if smt is not None:
        got = smt.decide(formula, SECONDS)
        if got is None:
            return None
        if got is not smt.UNKNOWN:
            terms = symbols(formula)
            if truth_at(formula, got) is True:
                where = sorted(places_in(formula), key=place_order)
                whole = all(not isinstance(v, Fraction) or v.denominator == 1 for v in got.values())
                values = {k: v for k, v in got.items() if k in terms}
                return Case(values, {k: terms.get(k, k) for k in values}, where, whole, inexact(formula), False,
                            BY_Z3)
    return own(formula)


def places_in(x, out=None) -> set:
    """Where the conditions of a formula are written."""
    out = set() if out is None else out
    if isinstance(x, tuple) and x and x[0] not in (SYMBOL, TEXT, DATA):
        if x[0] == AT:
            out.add(x[1])
            places_in(x[2], out)
        else:
            for y in x[1:]:
                places_in(y, out)
    return out


def truth_at(x, values: dict):
    """Whether a formula holds at values of its inputs, worked out exactly (None where it cannot be)."""
    if isinstance(x, bool):
        return x
    if isinstance(x, (Fraction, int)):
        return Fraction(x) != 0
    if not isinstance(x, tuple) or not x:
        return None
    head = x[0]
    if head == AT:
        return truth_at(x[2], values)
    if head == SYMBOL:
        v = values.get(x[1])
        return v if isinstance(v, bool) else (v != 0 if isinstance(v, Fraction) else None)
    if head in (ALL, ANY):
        parts = [truth_at(y, values) for y in x[1:]]
        if head == ALL:
            return False if False in parts else (None if None in parts else True)
        return True if True in parts else (None if None in parts else False)
    if head == NOT:
        inner = truth_at(x[1], values)
        return None if inner is None else not inner
    if head == ITE:
        test = truth_at(x[1], values)
        return None if test is None else truth_at(x[2] if test else x[3], values)
    if head in (LT, LE, GT, GE, EQ, NE):
        a, b = value_at(x[1], values), value_at(x[2], values)
        if a is None or b is None:
            return None
        if isinstance(a, str) or isinstance(b, str) or isinstance(a, bool) or isinstance(b, bool):
            if head not in (EQ, NE):
                return None
            return (a == b) == (head == EQ)
        return {LT: a < b, LE: a <= b, GT: a > b, GE: a >= b, EQ: a == b, NE: a != b}[head]
    v = value_at(x, values)
    return None if v is None or isinstance(v, str) else bool(v)


def value_at(x, values: dict):
    """A formula's value at values of its inputs: a fraction, a truth or a text, exactly (None where it cannot be)."""
    if isinstance(x, tuple) and x:
        head = x[0]
        if head == TEXT:
            return x[1]
        if head == SYMBOL:
            return values.get(x[1])
        if head == AT:
            return value_at(x[2], values)
        if head == ITE:
            test = truth_at(x[1], values)
            return None if test is None else value_at(x[2] if test else x[3], values)
        if head in (ALL, ANY, NOT, LT, LE, GT, GE, EQ, NE):
            return truth_at(x, values)
    point = {k: v for k, v in values.items() if isinstance(v, Fraction)}
    return at_point(x, point)


def own(formula):
    terms = symbols(formula)
    n = normal(formula, True, frozenset(), terms)
    budget, undecided = Budget(), False
    tried = set()
    for lits in conjunctions(n, budget):
        same = frozenset(lits)
        if same in tried:                        # the same literals by another way: asked already
            continue
        tried.add(same)
        values = theory(lits, terms, budget.until)
        if values == UNDECIDED:
            undecided = True
            continue
        if values is not None and any(isinstance(terms.get(k), tuple) for k in values):
            values = nonlinear(lits, terms, values, budget.until)   # the terms not linear: kept, or by intervals
            if values == UNDECIDED:
                undecided = True
                continue
        if values is not None:
            where = sorted({w for lit in lits for w in lit.where}, key=place_order)
            whole = all(not isinstance(v, Fraction) or v.denominator == 1 for v in values.values())
            near = values.pop(APPROXIMATE, False)
            shown = {k: render(terms[k]) if isinstance(terms.get(k), tuple) else terms.get(k, k) for k in values}
            return Case(values, shown, where, whole, inexact(formula), near, BY_OWN)
    return UNDECIDED if undecided or budget.left < 0 else None


# ---- what is not linear: kept exactly, else by intervals (branch and prune) ----------------------------------------
BIG = 1e6                        # the box an input with no bound is looked for in
STEPS = 4000                     # the most boxes looked at
NEAR = 1e-9                      # how near an equality found by intervals is said to hold (said: approximate)
APPROXIMATE = "~"


def at_point(x, point: dict):
    """A formula's value at values of its inputs (exact where the operations are), or None."""
    from .conditions import number_function
    if isinstance(x, bool):
        return Fraction(int(x))
    if isinstance(x, (Fraction, int)):
        return Fraction(x)
    if not isinstance(x, tuple) or not x:
        return None
    head = x[0]
    if head == SYMBOL:
        return point.get(x[1])
    if head == AT:
        return at_point(x[2], point)
    parts = [at_point(y, point) for y in x[1:]]
    if any(p is None for p in parts):
        return None
    try:
        if head == ADD:
            return parts[0] + parts[1]
        if head == SUB:
            return parts[0] - parts[1]
        if head == MUL:
            return parts[0] * parts[1]
        if head == DIV:
            return parts[0] / parts[1]
        if head == MOD:
            return parts[0] - parts[1] * math.trunc(parts[0] / parts[1])
        if head == NEG:
            return -parts[0]
    except ZeroDivisionError:
        return None
    return number_function(head, parts) if head in FUNCTIONS else None


def inputs_of(x, out: set) -> set:
    if isinstance(x, tuple) and x:
        if x[0] == SYMBOL:
            out.add(x[1])
        elif x[0] not in (TEXT, DATA):
            for y in x[1:]:
                inputs_of(y, out)
    return out


def span(x, box: dict) -> tuple:
    """The interval a formula's values lie in over a box of its inputs (interval arithmetic)."""
    inf = math.inf
    if isinstance(x, bool):
        return float(x), float(x)
    if isinstance(x, (Fraction, int)):
        return float(x), float(x)
    if not isinstance(x, tuple) or not x:
        return -inf, inf
    head = x[0]
    if head == SYMBOL:
        return box.get(x[1], (-BIG, BIG))
    if head == AT:
        return span(x[2], box)
    parts = [span(y, box) for y in x[1:]]
    a = parts[0]
    try:
        if head == ADD:
            return a[0] + parts[1][0], a[1] + parts[1][1]
        if head == SUB:
            return a[0] - parts[1][1], a[1] - parts[1][0]
        if head == NEG:
            return -a[1], -a[0]
        if head == MUL:
            b = parts[1]
            ends = [a[0] * b[0], a[0] * b[1], a[1] * b[0], a[1] * b[1]]
            return min(ends), max(ends)
        if head == DIV:
            b = parts[1]
            if b[0] <= 0 <= b[1]:
                return -inf, inf
            ends = [a[0] / b[0], a[0] / b[1], a[1] / b[0], a[1] / b[1]]
            return min(ends), max(ends)
        if head == SQRT:
            return math.sqrt(max(a[0], 0.0)), math.sqrt(max(a[1], 0.0))
        if head in (EXP,):
            return math.exp(min(a[0], 700)), math.exp(min(a[1], 700))
        if head == LOG:
            return (math.log(a[0]) if a[0] > 0 else -inf), (math.log(a[1]) if a[1] > 0 else -inf)
        if head in (SIN, COS):
            if a[1] - a[0] >= 2 * math.pi:
                return -1.0, 1.0
            f = math.sin if head == SIN else math.cos
            points = [a[0], a[1]] + [k * math.pi / 2 for k in range(math.ceil(a[0] / (math.pi / 2)),
                                                                    math.floor(a[1] / (math.pi / 2)) + 1)]
            values = [f(p) for p in points]
            return min(values), max(values)
        if head == ABS:
            if a[0] >= 0:
                return a
            if a[1] <= 0:
                return -a[1], -a[0]
            return 0.0, max(-a[0], a[1])
        if head in (MIN, MAX):
            pick = min if head == MIN else max
            return pick(p[0] for p in parts), pick(p[1] for p in parts)
        if head in (FLOOR, CEIL, TRUNC, ROUND):
            return math.floor(a[0]) - 1, math.ceil(a[1]) + 1
        if head == CLAMP:
            lo, hi = parts[1], parts[2]
            return max(a[0], lo[0]), min(a[1], hi[1])
        if head == POW:
            b = parts[1]
            if b[0] == b[1] and float(b[0]).is_integer():
                n = int(b[0])
                ends = [a[0] ** n, a[1] ** n] + ([0.0] if a[0] <= 0 <= a[1] else [])
                return min(ends), max(ends)
    except (OverflowError, ValueError, ZeroDivisionError):
        pass
    return -inf, inf


def row_value(row, terms: dict, at) -> object:
    """The left side of a row `sum c * term + const`, each term its own input or its formula, by `at`."""
    coeffs, const, strict = row
    total = at(const)
    for k, c in coeffs.items():
        formula = terms.get(k)
        part = at(formula) if isinstance(formula, tuple) else at((SYMBOL, k, "", ""))
        if part is None:
            return None
        total = total + at(c) * part if not isinstance(total, tuple) else None
    return total


def nonlinear(lits: tuple, terms: dict, values: dict, until: float | None):
    """Values of a conjunction whose rows hold terms that are not linear: the linear values found, where the terms
    computed from them keep every row; else a point found by branch and prune over the inputs of the terms, said
    approximate where an equality holds only within `NEAR`; None where the boxes prove no point does."""
    rows = []
    for n in lits:
        if n.kind != BOUND:
            continue
        coeffs = dict(n.key)
        if n.rel == EQ:
            rows.append((coeffs, n.const, False, True))
        else:
            rows.append((coeffs, n.const, n.rel == LT, False))
    base = set()
    for coeffs, *_ in rows:
        for k in coeffs:
            formula = terms.get(k)
            base |= inputs_of(formula, set()) if isinstance(formula, tuple) else {k}
    point = {k: v for k, v in values.items() if k in base and isinstance(v, Fraction)}

    def keeps_exactly(p: dict) -> bool:
        for coeffs, const, strict, equal in rows:
            total = row_value((coeffs, const, strict), terms, lambda y: at_point(y, p) if isinstance(y, tuple)
                              else Fraction(y))
            if total is None or (total != 0 if equal else not (total < 0 or (not strict and total == 0))):
                return False
        return True
    # Inputs are named by what the program writes (a definition, a place), not only by texts: merged as a literal.
    if all(k in point for k in base) and keeps_exactly(point):
        return {**values, **point}
    boxes = [{k: (-BIG, BIG) for k in base}]
    for k, v in point.items():                   # the linear values first: a box around them
        boxes.append({**{j: (-BIG, BIG) for j in base}, k: (float(v) - 1, float(v) + 1)})
    unknown = False                              # a box no split decides held a term with no value at its point
    for _ in range(STEPS):
        if not boxes:
            return UNDECIDED if unknown else None
        if until is not None and time.monotonic() > until:
            return UNDECIDED
        box = boxes.pop()
        out_of_it = False
        for coeffs, const, strict, equal in rows:
            lo, hi = float(const), float(const)
            for k, c in coeffs.items():
                formula = terms.get(k)
                s = span(formula if isinstance(formula, tuple) else (SYMBOL, k, "", ""), box)
                c = float(c)
                lo, hi = lo + min(c * s[0], c * s[1]), hi + max(c * s[0], c * s[1])
            if lo > 0 or (strict and lo >= 0) or (equal and hi < 0):
                out_of_it = True
                break
        if out_of_it:
            continue
        middle = {k: Fraction((a + b) / 2) for k, (a, b) in box.items()}
        whole = {k: Fraction(round(v)) for k, v in middle.items()}
        for p in (whole, middle):
            if keeps_exactly(p):
                return {**values, **p}
        near = middle
        if all(row_near(r, terms, near) for r in rows) and (not box or max(b - a for a, b in box.values()) < 1e-7):
            return {**values, **near, APPROXIMATE: True}
        if not box:                      # no variable left to split: its rows failed, or a term has no value (not proved)
            unknown = unknown or any(row_value((c, k, s), terms, lambda y: at_point(y, near) if isinstance(y, tuple)
                                               else Fraction(y)) is None for c, k, s, _ in rows)
            continue
        widest = max(box, key=lambda k: box[k][1] - box[k][0])
        a, b = box[widest]
        if b - a < 1e-12:
            continue
        half = (a + b) / 2
        boxes.append({**box, widest: (a, half)})
        boxes.append({**box, widest: (half, b)})
    return UNDECIDED


def row_near(row, terms: dict, point: dict) -> bool:
    coeffs, const, strict, equal = row
    total = row_value((coeffs, const, strict), terms,
                      lambda y: at_point(y, point) if isinstance(y, tuple) else Fraction(y))
    if total is None:
        return False
    return abs(total) <= NEAR if equal else total < NEAR


def place_order(where: str):
    path, _, line = where.rpartition(":")
    return (path, int(line) if line.isdigit() else 0)


def holds_always(formula) -> bool:
    return satisfy((NOT, formula)) is None
