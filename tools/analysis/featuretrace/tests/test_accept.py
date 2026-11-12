"""The acceptance of the new core against the current one (log/trace-core-plan.md, Task 7): the links each core finds
in the same program, compared by what they join (the definitions' qualified names) and what the walk does with them —
on the web fixture, where the new core alone links an event to its listener through the platform's declarations."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))

from featuretrace import accept  # noqa: E402

FIXTURE = HERE / "fixture"


class Accept(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        roots = [FIXTURE / "web", FIXTURE / "web" / "node_modules" / "three"]
        cls.found = accept.compare(roots, FIXTURE / "web", None)

    def has(self, side: str, group: str, src: str, dst: str) -> bool:
        return any(a.endswith(src) and b.endswith(dst) for a, b in getattr(self.found, side)[group])

    def test_both_cores_find_the_calls_the_code_writes(self):
        self.assertTrue(self.has("both", accept.RUN, "Game.start", "Actor.measure"))

    def test_a_link_only_the_new_core_finds_is_listed_as_its_own(self):
        # `dispatchEvent(new Event("ready"))` runs the listener by what EventTarget declares: the current core has
        # no rule for it any more (log/trace-research.md F39)
        self.assertTrue(self.has("only_new", accept.RUN, "announce", "Game.start.<lambda:21>"),
                        sorted(self.found.only_new[accept.RUN]))
        self.assertFalse(self.has("both", accept.RUN, "announce", "Game.start.<lambda:21>"))

    def test_the_report_counts_each_group(self):
        text = accept.text(self.found, 3)
        self.assertIn(f"{accept.RUN}: both {len(self.found.both[accept.RUN])}", text)
        self.assertIn("only the new core", text)


if __name__ == "__main__":
    unittest.main()
