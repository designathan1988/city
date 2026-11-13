"""What happens in each situation, on fixtures with invented names: conditions in loops summed up by intervals (three
languages), a value read where a variable is set, alternatives with a gap and exactly one always, data loaded from a
file the program holds, the rounding of floating point said, the solver's answers kept by formula, findings of a
start only over what it runs.
"""
from __future__ import annotations

import unittest
from fractions import Fraction

from support import traced
from tracer import beliefs, decide, invariants, situations
from tracer.vocab import ALL, CLAMP, LT, SQRT, SYMBOL


def effect(result, shown: str, path: str):
    found = [e for e in result.cases.effects if e.shown.startswith(shown) and
             result.program.files[e.file].display.endswith(path)]
    assert found, [e.shown for e in result.cases.effects]
    return found[0]


class Loops(unittest.TestCase):
    def setUp(self):
        self.r = traced("cases/loops")

    def test_a_counter_a_loop_moves_up_keeps_above_its_first_value(self):
        for path, shown in (("turns.ts", "t.odd"), ("turns.cpp", "t.odd"), ("turns.gd", "odd")):
            with self.subTest(path=path):
                self.assertIsNone(decide.satisfy(effect(self.r, shown, path).guard))

    def test_a_state_the_loop_moves_up_ends_above_what_it_was_read_before(self):
        for path, shown in (("turns.ts", "t.low"), ("turns.cpp", "t.low"), ("turns.gd", "low")):
            with self.subTest(path=path):
                self.assertIsNone(decide.satisfy(effect(self.r, shown, path).guard))

    def test_in_the_loop_the_state_is_its_value_at_a_turn(self):
        e = effect(self.r, "t.count", "turns.ts")
        self.assertIn("(in the loop)", decide.render(e.value))


class Alternatives(unittest.TestCase):
    def setUp(self):
        self.r = traced("cases/sign")

    def answers(self, name: str) -> dict:
        found = {}
        for (_, named), rows, answers in situations.report(self.r, name=name):
            for a in answers:
                found.setdefault(a.question, []).append(a)
        return found

    def test_two_thresholds_from_the_data_leave_a_gap(self):
        gap = self.answers("shown")[situations.NONE_TRUE][0]
        self.assertTrue(any(w.endswith("sign.ts:20") for w in gap.where))
        self.assertTrue(any(w.endswith("sign.ts:21") for w in gap.where))
        value = next(v for k, v in gap.case.values.items() if gap.case.shown[k] == "dist")
        self.assertTrue(12 <= value <= 15)

    def test_a_test_and_its_opposite_light_exactly_one(self):
        self.assertIn(situations.ALWAYS_ONE, self.answers("lit"))

    def test_data_read_from_a_file_the_program_holds_are_constants(self):
        value = effect(self.r, "glow.shown", "sign.ts").value
        self.assertIn("15", decide.render(value))


class Numbers(unittest.TestCase):
    def test_a_division_says_floating_point_may_round_otherwise(self):
        r = traced("cases/sign")
        case = decide.satisfy(effect(r, "p.share", "sign.ts").guard)
        self.assertTrue(case.rounding)

    def test_the_same_formula_by_other_numbers_of_definitions_is_one_answer(self):
        one = (ALL, (LT, (SYMBOL, 4, "a.x", "f.ts:1"), Fraction(3)), (LT, Fraction(1), (SYMBOL, 4, "a.x", "f.ts:1")))
        two = (ALL, (LT, (SYMBOL, 9, "a.x", "f.ts:1"), Fraction(3)), (LT, Fraction(1), (SYMBOL, 9, "a.x", "f.ts:1")))
        self.assertEqual(repr(decide.canon(one)), repr(decide.canon(two)))
        self.assertIs(decide.satisfy(one), decide.satisfy(two))


class Functions(unittest.TestCase):
    def test_functions_of_one_and_three_values_are_written_whole_and_apart(self):
        a, b = (SYMBOL, 4, "a", "f.ts:1"), (SYMBOL, 5, "b", "f.ts:1")
        self.assertEqual("sqrt(a)", decide.render((SQRT, a)))
        self.assertNotEqual(decide.render((CLAMP, a, b, Fraction(1))), decide.render((CLAMP, a, b, Fraction(2))))

    def test_inputs_named_by_numbers_go_through_what_is_not_linear(self):
        a = (SYMBOL, 4, "a", "f.ts:1")
        case = decide.satisfy((ALL, (LT, Fraction(2), (SQRT, a)), (LT, a, Fraction(10))))
        self.assertIsNotNone(case)
        self.assertNotEqual(decide.UNDECIDED, case)
        self.assertIsNone(decide.satisfy((ALL, (LT, Fraction(4), (SQRT, a)), (LT, a, Fraction(10)))))


class Start(unittest.TestCase):
    def test_a_start_reports_only_what_it_runs(self):
        r = traced("cases/sign")
        units = situations.reach(r, [d.id for d in r.program.defs if r.program.label(d.id) == "pair"])
        self.assertEqual([], [b for b in beliefs.deviants(r, units)])
        _, _, broken, both = invariants.report(r, units)
        self.assertTrue(all(r.cases.effects[b.effect].unit in units for b in broken))


if __name__ == "__main__":
    unittest.main()
