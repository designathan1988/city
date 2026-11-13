"""The migration of the old trackers into the register (tools/atlas/migrate.py), on made-up ledgers and tables in a
temporary repository: every open item enters once with its origin, a closed one and a settled divergence stay out, a known
cause traces the problem, a row citing a ledger's problem is related to it, a second run adds nothing, and the photos an
open problem names are kept as its evidence."""
import json
import os
import shutil
import sys
import tempfile
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS_DIR = os.path.abspath(os.path.join(HERE, "..", ".."))
if TOOLS_DIR not in sys.path:
    sys.path.insert(0, TOOLS_DIR)
from atlas import migrate  # noqa: E402
from atlas import problems as register  # noqa: E402

LEDGER = {"slug": "made-up", "problems": [
    {"id": "P1", "status": "open", "path": "native/src/render/roads/road_segment.cpp", "line": "1",
     "what": "a dark patch on a street, see photos/patch.png", "symptom": "a torn surface",
     "cause": "not read yet", "fix": "read road_segment.cpp", "found": "2026-11-10 09:15"},
    {"id": "P2", "status": "fixed", "path": "native/src/sim/traffic/window.cpp", "what": "gone", "cause": "found"},
    {"id": "P3", "status": "open", "path": "native/src/sim/traffic/window.cpp", "what": "walkers pop in",
     "cause": "the window lists no edge", "fix": "list the edge", "found": "2026-11-10 10:00"}]}
SPECIFICATION = """# Specification

| # | Where | What it says | What is true | Status |
|---|---|---|---|---|
| D1 | CLAUDE.md | old path | new path | to fix in CLAUDE.md |
| D6 | `ped_kerb.cpp:4` | no two ways converge | they converge (trace `made-up` P3) | open |

| # | What is slow | Why | Evidence |
|---|---|---|---|
| S1 | A compaction costs re-reading | the read rule counts the whole file | a session |
"""


class Migrate(unittest.TestCase):
    def setUp(self):
        self.root = tempfile.mkdtemp(prefix="migrate-")
        self.addCleanup(shutil.rmtree, self.root, True)
        os.makedirs(os.path.join(self.root, "tools", "gates", "schemas"))
        shutil.copy(os.path.join(register.ROOT, register.SCHEMA), os.path.join(self.root, register.SCHEMA))
        trace = os.path.join(self.root, "dist", "trace", "made-up")
        os.makedirs(os.path.join(trace, "photos"))
        with open(os.path.join(trace, "ledger.json"), "w", encoding="utf-8") as fh:
            json.dump(LEDGER, fh)
        with open(os.path.join(trace, "photos", "patch.png"), "wb") as fh:
            fh.write(b"\x89PNG")
        os.makedirs(os.path.join(self.root, "doc"))
        with open(os.path.join(self.root, migrate.SPECIFICATION), "w", encoding="utf-8") as fh:
            fh.write(SPECIFICATION)
        open(os.path.join(self.root, register.REGISTER), "w").close()

    def by_origin(self, reg):
        return {o: p for p in reg.problems for o in p["origin"]}

    def test_every_open_item_once_with_its_origin(self):
        reg = register.Register(root=self.root)
        counts = migrate.migrate(reg, self.root)
        self.assertEqual({"ledgers": (2, 0), "doc/specification.md": (2, 0)}, counts)
        got = self.by_origin(reg)
        self.assertEqual("unknown", got["ledger:made-up:P1"]["cause"])
        self.assertIn("read road_segment.cpp", got["ledger:made-up:P1"]["reading"])
        self.assertEqual("traced", got["ledger:made-up:P3"]["status"], "a known cause traces it")
        self.assertEqual("2026-11-10T10:00", got["ledger:made-up:P3"]["opened"])
        self.assertNotIn("ledger:made-up:P2", got, "a fixed problem is history")
        self.assertFalse(any(o.endswith(" D1") for o in got), "a divergence CLAUDE.md settled stays out")
        d6 = next(p for o, p in got.items() if o.endswith(" D6"))
        self.assertEqual([got["ledger:made-up:P3"]["id"]], d6["related"], "a divergence citing a ledger's problem")
        reg.save()
        again = register.Register(root=self.root)
        self.assertEqual({"ledgers": (0, 2), "doc/specification.md": (0, 2)},
                         migrate.migrate(again, self.root), "a second run adds nothing")

    def test_the_photos_an_open_problem_names_are_its_evidence(self):
        reg = register.Register(root=self.root)
        migrate.migrate(reg, self.root)
        self.assertEqual(1, migrate.move_evidence(reg, self.root))
        p1 = self.by_origin(reg)["ledger:made-up:P1"]
        self.assertEqual([f"dist/evidence/{p1['id']}/patch.png"], p1["evidence"])
        self.assertTrue(os.path.isfile(os.path.join(self.root, p1["evidence"][0])))
        reg.save()


class RenameRule(Migrate):
    def test_a_rule_renamed_is_followed_by_its_problems_and_no_other(self):
        reg = register.Register(root=self.root)
        kept = dict(title="TABLE in a.json", symptom="the gate TABLE reports", area="tools", kind="architecture",
                    reading="r", owner="a.json", criterion="no TABLE", origin=[register.gate_origin("TABLE", "a.json")])
        longer = dict(kept, title="TABLE_NO_SCHEMA in b.json", symptom="the gate TABLE_NO_SCHEMA reports",
                      criterion="no TABLE_NO_SCHEMA", origin=[register.gate_origin("TABLE_NO_SCHEMA", "b.json")])
        one, other = reg.add(**kept), reg.add(**longer)
        self.assertEqual([one], reg.rename_rule("TABLE", "DATA"))
        self.assertEqual(["gate:DATA:a.json"], one["origin"])
        self.assertEqual("DATA in a.json", one["title"])
        self.assertEqual(["gate:TABLE_NO_SCHEMA:b.json"], other["origin"], "a longer name holding the old one is another rule")
        reg.save()

    def test_a_path_moved_is_followed_by_the_evidence_and_owner_naming_it(self):
        reg = register.Register(root=self.root)
        os.makedirs(os.path.join(self.root, "doc", "old"))
        open(os.path.join(self.root, "doc", "old", "map.md"), "w").close()
        one = reg.add(title="t", symptom="s", area="docs", kind="process", reading="r", criterion="c",
                      owner="doc/old/map.md", evidence=["doc/old/map.md"], origin=["o"])
        os.rename(os.path.join(self.root, "doc", "old"), os.path.join(self.root, "doc", "new"))
        self.assertEqual([one], reg.rename_path("doc/old", "doc/new"))
        self.assertEqual((["doc/new/map.md"], "doc/new/map.md"), (one["evidence"], one["owner"]))
        reg.save()


if __name__ == "__main__":
    unittest.main()
