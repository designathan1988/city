"""Analysing again after a change: clang reads again only the units holding a file that changed (its answers for the
others are kept with the files each holds), and what the tracer says is what a fresh analysis says. Made-up files:
two units, one of them including a header; the header changes, then a unit alone changes.
"""
from __future__ import annotations

import os
import shutil
import tempfile
import time
import unittest
from pathlib import Path

from support import CALL, HERE, analyse
from tracer import compilers

UNIT_A = """#include "shared.h"
void tolk() { brisp(); }
"""
UNIT_B = """void glent() {}
void wamble() { glent(); }
"""
HEADER = """inline void brisp() {}
"""
HEADER_CHANGED = """inline void quell() {}
inline void brisp() { quell(); }
"""
UNIT_B_CHANGED = """void glent() {}
void frosk() {}
void wamble() { frosk(); }
"""


def links(result) -> set:
    p = result.program
    return {(p.label(link.src), p.label(link.dst)) for out in result.out.values() for link in out
            if link.kind == CALL}


def written(path: Path, text: str) -> None:
    path.write_text(text, encoding="utf-8")
    later = time.time() + 2                       # a time of its own, whatever the clock's grain
    os.utime(path, (later, later))


class Parts(unittest.TestCase):
    def test_only_the_units_holding_a_change_are_read_again(self):
        work = Path(tempfile.mkdtemp(prefix=".parts-", dir=HERE))
        try:
            written(work / "shared.h", HEADER)
            written(work / "a.cpp", UNIT_A)
            written(work / "b.cpp", UNIT_B)
            notes = []
            first = analyse([work], work, say=notes.append)
            if not any(n.startswith(compilers.CLANG + ":") and "(" not in n for n in notes):
                self.skipTest(f"clang did not answer: {notes}")
            again = analyse([work], work)
            self.assertEqual([], compilers.LAST_READ, "nothing changed: no unit is read again")
            self.assertEqual(links(first), links(again))

            written(work / "shared.h", HEADER_CHANGED)
            changed = analyse([work], work)
            self.assertEqual([compilers.norm(work / "a.cpp")], compilers.LAST_READ)
            self.assertIn(("brisp", "quell"), links(changed))

            written(work / "b.cpp", UNIT_B_CHANGED)
            changed = analyse([work], work)
            self.assertEqual([compilers.norm(work / "b.cpp")], compilers.LAST_READ)
            self.assertIn(("wamble", "frosk"), links(changed))
            self.assertNotIn(("wamble", "glent"), links(changed))
            fresh = work.parent / (work.name + "-fresh")       # other paths: no unit of theirs was read before
            shutil.copytree(work, fresh)
            try:
                self.assertEqual(links(changed), links(analyse([fresh], fresh)))
            finally:
                shutil.rmtree(fresh, ignore_errors=True)
        finally:
            shutil.rmtree(work, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
