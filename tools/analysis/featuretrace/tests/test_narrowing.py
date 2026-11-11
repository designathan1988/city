"""Values through branch conditions (log/trace-rules.md, "Narrowing at branch conditions": V1, V2, G1, G2, R1, R2,
N1): the language's constants, properties written outside their class, and what a name holds inside the regions a
condition proves something in."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))
sys.path.insert(0, str(HERE))

from support import FixtureCase  # noqa: E402
from featuretrace.solver import ENGINE, NULL, TRUE, UNDEFINED  # noqa: E402


class Narrowing(FixtureCase):
    ROOTS = ("jsvalues",)
    BASE = "jsvalues"

    def returned(self, spec: str) -> set:
        typer = self.rs.typer
        return {text for text, _ in typer.current(typer.ret_node(self.ix.units[self.gid(spec)]))}

    def test_literal_constants(self):
        self.assertEqual({TRUE}, self.returned("yes"))
        self.assertEqual({NULL}, self.returned("none"))
        self.assertEqual({UNDEFINED}, self.returned("gone"))

    def test_constants_are_the_languages_values(self):
        self.assertEqual({ENGINE}, self.returned("lengthOfYes"))


if __name__ == "__main__":
    unittest.main()
