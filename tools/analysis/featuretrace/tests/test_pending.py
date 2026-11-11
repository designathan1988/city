"""The rules found against a real run (log/trace-rules.md, "The pending rules found against a real run", "What the lab
run showed"): block scope, local classes, direct-initialization, destruction, call sites on one line, each checked on
the links it must make (and must not)."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))
sys.path.insert(0, str(HERE))

from support import FixtureCase  # noqa: E402


class Pending(FixtureCase):
    ROOTS = ("pending",)
    BASE = "pending"

    def test_block_scope_cpp(self):
        # the second `it` (over `newest`) does not hide the first (over `slots`): the job submitted is called
        self.assertIn("fx::use.<lambda:75>", self.names("fx::Pool::loop"))

    def test_block_scope_gdscript(self):
        self.assertTrue({"Box.open", "Can.open"} <= {n.split("scopes.gd.")[-1] for n in self.names("scopes.gd:13")})

    def test_block_scope_javascript(self):
        self.assertTrue({"Box.open", "Can.open"} <= self.names("scopes.js:7"))

    def test_renamed_local_passed_as_argument(self):
        # the second `row` (a Can) reaches the arrow's parameter: its open() is Can's, not Box's
        self.assertEqual({"Can.open"}, self.names("scopes.js:19"))

    def test_each_call_site_carries_its_argument(self):
        # place(new Left()) + place(new Right()) on one line: the parameter holds both, p.side() reaches both
        self.assertEqual({"Left.side", "Right.side"}, self.names("sites.js:4"))

    def test_var_belongs_to_its_own_function(self):
        self.assertIn("Can.open", self.names("scopes.js:29"))          # the inner function's `var it`: a Can
        outer = self.names("scopes.js:32")                              # the outer function's `var it`: a Box
        self.assertIn("Box.open", outer)
        self.assertNotIn("Can.open", outer)

    def test_no_stale_unreached_note(self):
        # linked first with no value for `p`, then again when the caller's Box arrives: not "unreached"
        self.assertNotIn(self.gid("late"), self.rs.unreached)

    def test_local_class(self):
        self.assertIn("fx::use.<lambda:77>", self.names("fx::Pool::each.<lambda:35>"))

    def test_direct_initialization_carries_the_arguments(self):
        self.assertIn("fx::use.<lambda:78>", self.names("fx::Facing::run"))

    def test_binary_star_is_multiplication(self):
        self.assertIn("fx::W::go", self.names("fx::scale"))   # (a * b) is what V::operator* returns (O2)

    def test_destruction(self):
        owner = self.names("fx::Owner::~Owner")
        self.assertTrue({"fx::Pool::~Pool", "fx::Part::~Part", "fx::Base::~Base"} <= owner)
        self.assertIn("fx::Part::~Part", self.names("fx::use"))           # Holder's implicit destructor


if __name__ == "__main__":
    unittest.main()
