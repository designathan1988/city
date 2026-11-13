"""Programs made up with defects planted by construction, and clean ones (tests/defects.py), new on every run: the
tracer must find every planted defect, on its lines, by what it is (a check missing among peers, alternatives with a
gap or an overlap, a write breaking what the other writes keep, one object in two containers), and nothing anywhere
else. The seed is said on failure; `TRACER_SEED=<seed>` runs it again, `TRACER_PROGRAMS=<n>` sets how many.
"""
from __future__ import annotations

import os
import pickle
import random
import shutil
import tempfile
import time
import unittest
from pathlib import Path

from support import HERE, analyse
from defects import BELIEF, BOUND, GAP, HYSTERESIS, ORDER, OVERLAP, RELATION, TWO, WRITERS, make
from tracer import beliefs, index, invariants, situations

PROGRAMS = int(os.environ.get("TRACER_PROGRAMS", "4"))      # programs made per language
# What finding each kind of planted defect must give.
EXPECTED = {RELATION: {"invariant"}, BELIEF: {"belief"}, GAP: {situations.NONE_TRUE}, OVERLAP: {situations.MANY},
            HYSTERESIS: {situations.MANY}, BOUND: {"invariant"}, ORDER: {"invariant"}, TWO: {"two"}}


def findings(result) -> list:
    """Every finding, as (what, file, line)."""
    p, effects, out = result.program, result.cases.effects, []

    def at(i):
        return p.files[effects[i].file].display, effects[i].line
    for found in beliefs.deviants(result):
        out.append(("belief",) + at(found.effect))
    _, _, broken, both = invariants.report(result)
    out += [("invariant",) + at(found.effect) for found in broken]
    out += [("two",) + at(i) for a, b, _ in both for i in (a, b)]
    for _, _, answers in situations.report(result):
        for found in answers:
            if found.question in (situations.NONE_TRUE, situations.NONE_HAPPENS, situations.MANY):
                out += [(found.question,) + at(i) for i in found.effects]
    return out


class Defects(unittest.TestCase):
    def test_every_planted_defect_is_found_and_nothing_else(self):
        seed = int(os.environ.get("TRACER_SEED") or random.SystemRandom().randrange(1 << 30))
        rng = random.Random(seed)
        work = Path(tempfile.mkdtemp(prefix=".defects-", dir=HERE))
        hits = misses = false = 0
        began = time.monotonic()
        try:
            for language in WRITERS:
                for i in range(PROGRAMS + 1):
                    made = make(language, rng, clean=i == PROGRAMS)     # the last one of each language all clean
                    folder = work / f"{language}{i}"
                    made.write(folder)
                    result = analyse([folder], folder)
                    found = findings(result)
                    if i == 0:                   # the analysis saved answers as the analysis
                        kept = pickle.loads(pickle.dumps(index.kept(analyse([folder], folder))))
                        self.assertEqual(sorted(found), sorted(findings(kept)))
                    planted = {(s.path, line) for s in made.scenarios for line in s.lines}
                    wrong = [f for f in found if (f[1], f[2]) not in planted]
                    missed = []
                    for s in made.scenarios:
                        if not s.planted:
                            continue
                        mine = {(f[0], f[2]) for f in found if f[1] == s.path}
                        if not all(any((what, line) in mine for what in EXPECTED[s.kind]) for line in s.lines):
                            missed.append(s)
                    hits += sum(1 for s in made.scenarios if s.planted) - len(missed)
                    misses += len(missed)
                    false += len(wrong)
                    with self.subTest(seed=seed, program=made.describe()):
                        self.assertEqual([], missed, f"missed; found {sorted(found)}")
                        self.assertEqual([], wrong, "found where nothing is planted")
        finally:
            shutil.rmtree(work, ignore_errors=True)
            print(f"\n[defects] seed {seed}: {hits} found, {misses} missed, {false} false, "
                  f"{time.monotonic() - began:.1f} s")


if __name__ == "__main__":
    unittest.main()
