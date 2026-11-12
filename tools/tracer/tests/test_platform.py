"""What is not the code read (fixture `rules`, names made up): a name no scope gives is the platform's object, none of
whose members is the code's (spec 001, D2); code not read gives back what the object it is called on holds and runs
the functions it is handed with the objects it was given (D4); an object written in place hands over the functions its
entries hold."""
from __future__ import annotations

import unittest

from support import BY_NAME, CALL, EXACT, HANDED, reached, traced


class Platform(unittest.TestCase):
    def test_a_member_of_a_name_no_scope_gives_is_not_the_code_s(self):
        r = traced("rules")
        self.assertEqual({"Tally.max"}, reached(r, "measure", (CALL,)))


class Through(unittest.TestCase):
    def setUp(self):
        self.r = traced("rules")

    def test_an_element_of_a_container_and_a_callback_s_argument_are_its_objects(self):
        self.assertEqual({"Crate.lift", "Crate.weigh"},
                         {label for label in reached(self.r, "haul", (CALL,), EXACT) if label.startswith("Crate")})
        self.assertEqual(set(), reached(self.r, "haul", (CALL,), BY_NAME))

    def test_the_options_an_object_hands_over_are_handed(self):
        self.assertEqual({"finish"}, reached(self.r, "opts", (HANDED,)))

class Operators(unittest.TestCase):
    """An operator applied to an object of a class calls the function its class writes for it (C++ [over.oper];
    fixture `ops`): what it gives back is an object of its declared type, whose members are found exactly."""

    def test_an_operator_s_result_has_its_class_s_members(self):
        r = traced("ops")
        self.assertEqual({"Vec.operator-", "Vec.len"}, reached(r, "span", (CALL,), EXACT))
        self.assertEqual(set(), reached(r, "span", (CALL,), BY_NAME))


if __name__ == "__main__":
    unittest.main()
