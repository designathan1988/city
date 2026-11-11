"""Call results per argument values (log/trace-rules.md, "Call results per argument values: templates", C1, C2):
which parameters key a function's templates, and each call getting the result of its own arguments' values."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))
sys.path.insert(0, str(HERE))

from support import FixtureCase  # noqa: E402
from featuretrace import templates  # noqa: E402
from featuretrace.solver import TEXT  # noqa: E402


class Templates(FixtureCase):
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"

    def keys(self, name: str) -> tuple:
        return templates.key_params(self.rs.typer, self.ix.units[self.gid(name)])

    def test_key_parameters(self):
        self.assertEqual((0,), self.keys("same"))           # returned as it is
        self.assertEqual((0,), self.keys("inner"))          # a member of it
        self.assertEqual((0,), self.keys("wrapped"))        # through a call by name of such a function
        self.assertEqual((), self.keys("constant"))
        self.assertEqual((0,), self.keys("maker"))          # a closure that reads it (C4)
        self.assertEqual((1,), self.keys("down"))           # recursive: the parameter that comes back

    def test_each_call_gets_its_own_result(self):
        self.assertIn("Box.open", self.names("useBox"))
        self.assertNotIn("Tile.lay", self.names("useBox"))
        self.assertIn("Tile.lay", self.names("useTile"))
        self.assertNotIn("Box.open", self.names("useTile"))

    def test_text_through_a_helper(self):
        typer = self.rs.typer
        returned = {text for text, _ in typer.current(typer.ret_node(self.ix.units[self.gid("label")]))}
        self.assertEqual({TEXT + "a"}, returned)

    def test_two_key_parameters_meet(self):
        self.assertEqual((0, 1), self.keys("either"))
        self.assertIn("Box.open", self.names("pairBox"))
        self.assertNotIn("Box.open", self.names("pairTile"))     # the tiles' call gives only tiles

    def test_through_wrappers_and_recursion(self):
        self.assertIn("Box.open", self.names("deep"))
        self.assertIn("Box.open", self.names("recursive"))


if __name__ == "__main__":
    unittest.main()
