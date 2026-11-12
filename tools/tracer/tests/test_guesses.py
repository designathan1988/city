"""A guess by name (fixture `rules`, spec 001 D6): narrowed to the functions taking the call's arguments, then to the
classes declaring the most of the other members read of the same object; past `among` candidates the call is said
ambiguous and linked to none; a trace follows a guess one step and not on from it."""
from __future__ import annotations

import unittest

from support import BY_NAME, CALL, reached, traced


class Narrowed(unittest.TestCase):
    def setUp(self):
        self.r = traced("rules")

    def test_by_the_count_of_arguments(self):
        self.assertEqual({"Rotor.spin"}, reached(self.r, "sized", (CALL,), BY_NAME))

    def test_by_the_other_members_read_of_the_same_object(self):
        self.assertEqual({"Top1.brake", "Top1.whirl"}, reached(self.r, "loose", (CALL,), BY_NAME))

    def test_too_many_is_ambiguous_and_linked_to_none(self):
        self.assertEqual(set(), reached(self.r, "many", (CALL,)))
        unit = next(d.id for d in self.r.program.defs if self.r.program.label(d.id) == "many")
        self.assertEqual([("thing.spin", 9)], self.r.ambiguous[unit])


if __name__ == "__main__":
    unittest.main()
