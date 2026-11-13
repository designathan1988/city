"""The solvers on formulas made up on every run: with a case by construction (built around a point drawn at random:
every condition holds there), the case given must hold when checked exactly; without one by construction (a value
both below a number and above a larger one, a square that is negative, a root too large for the bounded value under
it), no case may be given. A case is checked exactly where it can be (a root of a number no square is not worked out
exactly: counted apart); an input a case leaves out may be anything (checked at 0). z3 and the tracer's
own solver both, apart. `TRACER_SEED` repeats a run.
"""
from __future__ import annotations

import os
import random
import unittest
from fractions import Fraction

import support  # noqa: F401  (the tracer on the path)
from tracer import decide
from tracer.vocab import ABS, ALL, ANY, GE, GT, ITE, LE, LT, MIN, MUL, ADD, SQRT, SUB, SYMBOL

ROUNDS = int(os.environ.get("TRACER_PROGRAMS", "40"))


def made(rng: random.Random) -> tuple:
    """A formula holding at a point drawn at random, and the point."""
    names = [(SYMBOL, k, f"x{k}", "made:1") for k in range(rng.randint(1, 3))]
    point = {k: Fraction(rng.randint(-20, 20), rng.choice([1, 2, 4])) for k in range(len(names))}

    def term(depth: int):
        r = rng.random()
        if depth <= 0 or r < 0.3:
            return rng.choice(names) if rng.random() < 0.7 else Fraction(rng.randint(-9, 9))
        if r < 0.55:
            return (rng.choice([ADD, SUB]), term(depth - 1), term(depth - 1))
        if r < 0.7:
            return (MUL, term(depth - 1), term(depth - 1))
        if r < 0.8:
            return (SQRT, (ADD, (ABS, term(depth - 1)), Fraction(1)))
        if r < 0.9:
            return (MIN, term(depth - 1), term(depth - 1))
        return (ITE, (LT, term(depth - 1), term(depth - 1)), term(depth - 1), term(depth - 1))
    parts = []
    for _ in range(rng.randint(1, 4)):
        t = term(2)
        value = decide.value_at(t, point)
        if value is None:
            continue
        rooted = SQRT in repr(t)         # a root's value is a 64-bit real, not exact: the bound gets room around it
        slack = Fraction(rng.randint(1 if rooted else 0, 3))
        parts.append(rng.choice([(LE, t, value + slack), (GE, t, value - slack)]) if slack else
                     rng.choice([(LE, t, value), (GE, t, value)]))
    if rng.random() < 0.3:
        parts.append((ANY, (GT, names[0], point[0] - 1), (LT, names[0], Fraction(-1000))))
    return ((ALL,) + tuple(parts) if len(parts) > 1 else parts[0] if parts else True), point


def impossible(rng: random.Random):
    x = (SYMBOL, 0, "x", "made:1")
    c = Fraction(rng.randint(-20, 20))
    return rng.choice([(ALL, (LT, x, c), (GT, x, c + rng.randint(1, 9))),
                       (LT, (MUL, x, x), Fraction(-rng.randint(1, 9))),
                       (ALL, (GT, (SQRT, (ADD, (ABS, x), Fraction(1))), Fraction(30)), (LT, x, Fraction(10)),
                        (GT, x, Fraction(-10)))])


class Solvers(unittest.TestCase):
    def test_cases_hold_and_impossible_formulas_have_none(self):
        seed = int(os.environ.get("TRACER_SEED") or random.SystemRandom().randrange(1 << 30))
        rng = random.Random(seed)
        counts = {"z3": [0, 0, 0, 0], "own": [0, 0, 0, 0]}    # decided, not decided, wrong, not checked exactly
        for _ in range(ROUNDS):
            formula, _ = made(rng)
            for name, solve in (("z3", decide.solve), ("own", decide.own)):
                decide.MEMO.clear()
                case = solve(formula)
                with self.subTest(seed=seed, solver=name, formula=decide.render(formula)):
                    if case is None:
                        counts[name][2] += 1
                        self.fail("no case for a formula that has one")
                    elif case == decide.UNDECIDED:
                        counts[name][1] += 1
                    else:
                        counts[name][0] += 1
                        if not case.approximate:
                            at = {k: Fraction(0) for k in decide.symbols(formula)}
                            at.update(case.values)
                            held = decide.truth_at(formula, at)
                            self.assertIsNot(False, held)
                            if held is None:
                                counts[name][3] += 1
            wrong = impossible(rng)
            for name, solve in (("z3", decide.solve), ("own", decide.own)):
                case = solve(wrong)
                with self.subTest(seed=seed, solver=name, formula=decide.render(wrong)):
                    self.assertTrue(case is None or case == decide.UNDECIDED)
                    if case is not None:
                        counts[name][1] += 1
        print(f"\n[solvers] seed {seed}: " + ", ".join(f"{k}: decided {v[0]}, not decided {v[1]}, wrong {v[2]}, "
                                                     f"not checked exactly {v[3]}" for k, v in counts.items()))
        self.assertEqual(0, counts["z3"][1], "z3 left formulas not decided")


if __name__ == "__main__":
    unittest.main()
