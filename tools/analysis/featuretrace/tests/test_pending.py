"""The rules found against a real run (log/trace-rules.md, "The pending rules found against a real run"): block scope,
local classes, direct-initialization, destruction, each checked on the links it must make (and must not)."""
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

    def test_local_class(self):
        self.assertIn("fx::use.<lambda:77>", self.names("fx::Pool::each.<lambda:35>"))

    def test_direct_initialization_carries_the_arguments(self):
        self.assertIn("fx::use.<lambda:78>", self.names("fx::Facing::run"))

    def test_destruction(self):
        owner = self.names("fx::Owner::~Owner")
        self.assertTrue({"fx::Pool::~Pool", "fx::Part::~Part", "fx::Base::~Base"} <= owner)
        self.assertIn("fx::Part::~Part", self.names("fx::use"))           # Holder's implicit destructor


if __name__ == "__main__":
    unittest.main()
