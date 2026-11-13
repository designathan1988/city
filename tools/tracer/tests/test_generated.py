"""Programs made up on every run (tests/generate.py): random names, a random chain of the language's ways for one
function to run another, spread over files, with decoys beside it. The trace from the chain's start must go into
every function of the chain and reach no decoy; the trace back from its end must reach every one. The seed is new on
every run and said on failure; `TRACER_SEED=<seed>` runs that one again.
"""
from __future__ import annotations

import os
import random
import shutil
import tempfile
import unittest
from pathlib import Path

from support import HERE, analyse
from generate import LANGUAGES
from tracer.report import readers_of, written
from tracer.walk import backward, find, forward

PROGRAMS = int(os.environ.get("TRACER_PROGRAMS", "4"))      # programs made per language and run


def named(label: str, name: str) -> bool:
    return label == name or label.endswith("." + name)


class Generated(unittest.TestCase):
    def test_every_chain_is_traced_whole(self):
        seed = int(os.environ.get("TRACER_SEED") or random.SystemRandom().randrange(1 << 30))
        rng = random.Random(seed)
        work = Path(tempfile.mkdtemp(prefix=".generated-", dir=HERE))
        try:
            for language, make in LANGUAGES.items():
                for i in range(PROGRAMS):
                    prog = make(rng)
                    folder = work / f"{language}{i}"
                    prog.write(folder)
                    with self.subTest(seed=seed, program=prog.describe()):
                        self.check(prog, analyse([folder], folder))
        finally:
            shutil.rmtree(work, ignore_errors=True)

    def check(self, prog, result):
        p = result.program
        start, end = find(result, prog.chain[0]), find(result, prog.chain[-1])
        self.assertTrue(start and end, f"start or end not found: {sorted(p.label(d.id) for d in p.defs)}")
        ahead = forward(result, start)
        into = {p.label(s.at) for s in ahead if not s.leaf}
        shown = {p.label(s.at) for s in ahead}
        missing = [c for c in prog.chain if not any(named(lab, c) for lab in into)]
        self.assertEqual([], missing, f"forward missed; reached {sorted(shown)}")
        decoys = [c for c in prog.decoys if any(named(lab, c) for lab in shown)]
        self.assertEqual([], decoys, f"forward reached decoys; reached {sorted(shown)}")
        behind = {p.label(s.at) for s in backward(result, end)}
        missing = [c for c in prog.chain if not any(named(lab, c) for lab in behind)]
        self.assertEqual([], missing, f"backward missed; reached {sorted(behind)}")
        if prog.state:                       # the state one function writes, its owner, and the end reading it
            writer, state, owner, reader = prog.state
            wrote = [d for d in written(result, find(result, writer)) if named(p.label(d), state)]
            self.assertTrue(wrote, f"{writer} writes {state}: found {[p.label(d) for d in written(result, find(result, writer))]}")
            self.assertIn(owner, p.label(p.defs[wrote[0]].owner))
            readers = {p.label(u) for u, _ in readers_of(result, wrote[0])}
            self.assertTrue(any(named(lab, reader) for lab in readers), f"{reader} reads {state}: found {readers}")
            through = {p.label(d) for d in written(result, [s.at for s in ahead if not s.leaf])}
            self.assertTrue(any(named(lab, state) for lab in through), "the trace from the start writes the state")


if __name__ == "__main__":
    unittest.main()
