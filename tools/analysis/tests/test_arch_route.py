"""Tests of the architecture analyser's route (arch/route.py) on a small published reading: what a function runs, the state that run writes,
and who reads that state afterwards with the entries that run each reader; and of the compiler adapter's rule that a part written is its
whole written (`v.cur.x = 1` writes `cur`), read from a real translation unit."""
import os
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))  # tools/analysis: the arch package

from arch import route  # noqa: E402

# Five functions: `press` (an event entry) calls `build` and `mark`; `draw` runs every frame; `report` on an event.
DATA = {
    "functions": [["Game::press", "a.cpp", 1, 9], ["Game::build", "a.cpp", 11, 19], ["Game::mark", "b.cpp", 1, 9],
                  ["View::draw", "c.cpp", 1, 9], ["Hud::report", "d.cpp", 1, 9]],
    "slices": [
        {"id": "e1", "entry": "Game::press", "kind": "bound", "freq": "event", "order": [0, 1, 2], "reached": [None, [0, 3], [0, 4]]},
        {"id": "e2", "entry": "View::draw", "kind": "engine", "freq": "frame", "order": [3], "reached": [None]},
        {"id": "e3", "entry": "Hud::report", "kind": "bound", "freq": "event", "order": [4], "reached": [None]},
    ],
    "members": {"Game::roads": {"w": [1], "r": [1, 3]}, "Game::dirty": {"w": [2], "r": [4]}, "View::cache": {"w": [3], "r": [3]}},
}


class Route(unittest.TestCase):
    def test_the_state_a_run_writes_is_what_its_functions_write(self):
        self.assertEqual(route.state_of(DATA, [0, 1, 2]), ["Game::dirty", "Game::roads"])

    def test_a_reader_outside_the_run_is_a_consumer_with_the_entries_that_run_it(self):
        found = route.consumers(DATA, [0, 1, 2])
        self.assertEqual([(c["function"], c["reads"], c["entries"]) for c in found],
                         [(3, ["Game::roads"], [["View::draw", "frame"]]), (4, ["Game::dirty"], [["Hud::report", "event"]])])

    def test_a_function_of_the_run_that_reads_its_own_state_is_no_consumer(self):
        self.assertNotIn(1, [c["function"] for c in route.consumers(DATA, [0, 1, 2])])

    def test_a_run_that_writes_nothing_has_no_consumer(self):
        self.assertEqual(route.consumers(DATA, [4]), [])


SOURCE = """
struct Pose { float x; float z; };
struct Other { float x; };
struct Vehicle { Pose cur; Other *far; int id; };
void move(Vehicle &v) { v.cur.x = 1; }
void through(Vehicle *p) { p->cur.z = 2; }
void pointee(Vehicle &v) { v.far->x = 3; }
float look(const Vehicle &v) { return v.cur.x; }
"""


class Writes(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        from clang import cindex as ci

        from arch.adapters.cpp_summary import walk
        cls.folder = tempfile.mkdtemp()
        path = os.path.join(cls.folder, "unit.cpp")
        with open(path, "w", encoding="utf-8") as f:
            f.write(SOURCE)
        tu = ci.Index.create().parse(path, args=["-std=c++17"], options=ci.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD)
        facts = walk(tu, lambda name: "unit.cpp" if name and name.replace(chr(92), "/").endswith("unit.cpp") else None, "test",
                     lambda file: True, False, ())
        cls.events = {s["name"]: [(e[0], e[3].rsplit("@", 1)[-1]) for e in s["ev"] if e[0] in ("r", "w", "wp")]
                      for s in facts.summaries if s["kind"] == "function"}

    @classmethod
    def tearDownClass(cls):
        os.remove(os.path.join(cls.folder, "unit.cpp"))
        os.rmdir(cls.folder)

    def test_a_part_written_is_its_whole_written_in_part(self):
        self.assertIn(("wp", "cur"), self.events["move"])
        self.assertIn(("w", "x"), self.events["move"])
        self.assertNotIn(("w", "cur"), self.events["move"])  # field by field stays exact: the detectors' own sets do not grow

    def test_a_part_written_through_a_pointer_to_the_whole_writes_the_member(self):
        self.assertIn(("wp", "cur"), self.events["through"])

    def test_a_write_through_a_pointer_member_reads_the_pointer(self):
        self.assertIn(("r", "far"), self.events["pointee"])
        self.assertNotIn(("w", "far"), self.events["pointee"])
        self.assertNotIn(("wp", "far"), self.events["pointee"])

    def test_a_part_read_is_no_write(self):
        self.assertNotIn(("w", "cur"), self.events["look"])
        self.assertNotIn(("wp", "cur"), self.events["look"])


if __name__ == "__main__":
    unittest.main()
