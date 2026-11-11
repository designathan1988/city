"""The program's building phases (log/trace-research.md, F17): reading the facts, indexing them and analysing them build
the data a run keeps, so the cyclic collector does not run while they build and what they built is frozen out of later
collections; the collector's state is the caller's again when a phase ends, even when it fails."""
from __future__ import annotations

import gc
import unittest

from .support import HEAP
from .. import program


class BuildingPhases(unittest.TestCase):
    def setUp(self):
        gc.enable()

    def test_a_deep_phase_runs_without_the_cyclic_collector(self):
        self.assertFalse(program.deep(gc.isenabled))
        self.assertTrue(gc.isenabled())

    def test_what_a_phase_built_is_frozen(self):
        before = gc.get_freeze_count()
        kept = program.deep(lambda: [[n] for n in range(1000)])
        self.assertGreaterEqual(gc.get_freeze_count(), before + len(kept))

    def test_reading_builds_without_the_cyclic_collector(self):
        seen = []
        real = program.Index

        def index(facts):
            seen.append(gc.isenabled())
            return real(facts)
        program.Index = index
        try:
            program.read([HEAP / "core.ts"], HEAP, None)
        finally:
            program.Index = real
        self.assertEqual([False], seen)
        self.assertTrue(gc.isenabled())

    def test_a_failed_phase_gives_the_collector_back(self):
        def fail():
            raise ValueError("planted")
        with self.assertRaises(ValueError):
            program.deep(fail)
        self.assertTrue(gc.isenabled())

    def test_a_caller_without_the_collector_keeps_it_off(self):
        gc.disable()
        try:
            program.deep(lambda: None)
            self.assertFalse(gc.isenabled())
        finally:
            gc.enable()


if __name__ == "__main__":
    unittest.main()
