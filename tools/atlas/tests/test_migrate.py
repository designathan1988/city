"""The migration of the old trackers into the register (tools/atlas/migrate.py), on made-up ledgers and tables in a
temporary repository: every open item enters once with its origin, a closed one and a row called done stay out, a known
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
PENDING = """# Pending items

| Id | Symptom | Root cause | Tried | Resolved when |
|---|---|---|---|---|
| V1 | A pole with no wires (`native/src/render/props/street_props.cpp`) | found (trace `made-up` P1): two levels | none | the wires drawn |
| M1 | No budgets | never defined | none | defined (done: log/specification.md) |
| T2 | No live monitor | unknown | a sampler | the monitor reads the run |

## Carried from before 2026-11-11

| Z9 | carried prose is no row of this table | x | y | z |
"""
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
        os.makedirs(os.path.join(self.root, "log"))
        for name, text in (("pendencias.md", PENDING), ("specification.md", SPECIFICATION)):
            with open(os.path.join(self.root, "log", name), "w", encoding="utf-8") as fh:
                fh.write(text)
        open(os.path.join(self.root, register.REGISTER), "w").close()

    def by_origin(self, reg):
        return {o: p for p in reg.problems for o in p["origin"]}

    def test_every_open_item_once_with_its_origin(self):
        reg = register.Register(root=self.root)
        counts = migrate.migrate(reg, self.root)
        self.assertEqual({"ledgers": (2, 0), "log/pendencias.md": (2, 0), "log/specification.md": (2, 0)}, counts)
        got = self.by_origin(reg)
        self.assertEqual("unknown", got["ledger:made-up:P1"]["cause"])
        self.assertIn("read road_segment.cpp", got["ledger:made-up:P1"]["reading"])
        self.assertEqual("traced", got["ledger:made-up:P3"]["status"], "a known cause traces it")
        self.assertEqual("2026-11-10T10:00", got["ledger:made-up:P3"]["opened"])
        self.assertNotIn("ledger:made-up:P2", got, "a fixed problem is history")
        self.assertFalse(any(o.endswith(" M1") for o in got), "a row called done stays out")
        self.assertFalse(any(o.endswith(" D1") or o.endswith(" Z9") for o in got), "settled, or carried prose")
        v1 = next(p for o, p in got.items() if o.endswith(" V1"))
        self.assertEqual([got["ledger:made-up:P1"]["id"]], v1["related"])
        self.assertEqual("render", v1["area"])
        d6 = next(p for o, p in got.items() if o.endswith(" D6"))
        self.assertEqual([got["ledger:made-up:P3"]["id"]], d6["related"], "a divergence citing a ledger's problem")
        reg.save()
        again = register.Register(root=self.root)
        self.assertEqual({"ledgers": (0, 2), "log/pendencias.md": (0, 2), "log/specification.md": (0, 2)},
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
        kept = dict(title="DADOS in a.json", symptom="the gate DADOS reports", area="tools", kind="architecture",
                    reading="r", owner="a.json", criterion="no DADOS", origin=[register.gate_origin("DADOS", "a.json")])
        longer = dict(kept, title="DADOS_SEM_SCHEMA in b.json", symptom="the gate DADOS_SEM_SCHEMA reports",
                      criterion="no DADOS_SEM_SCHEMA", origin=[register.gate_origin("DADOS_SEM_SCHEMA", "b.json")])
        one, other = reg.add(**kept), reg.add(**longer)
        self.assertEqual([one], reg.rename_rule("DADOS", "DATA"))
        self.assertEqual(["gate:DATA:a.json"], one["origin"])
        self.assertEqual("DATA in a.json", one["title"])
        self.assertEqual(["gate:DADOS_SEM_SCHEMA:b.json"], other["origin"], "a longer name holding the old one is another rule")
        reg.save()

    def test_a_path_moved_is_followed_by_the_evidence_and_owner_naming_it(self):
        reg = register.Register(root=self.root)
        os.makedirs(os.path.join(self.root, "log", "old"))
        open(os.path.join(self.root, "log", "old", "map.md"), "w").close()
        one = reg.add(title="t", symptom="s", area="docs", kind="process", reading="r", criterion="c",
                      owner="log/old/map.md", evidence=["log/old/map.md"], origin=["o"])
        os.rename(os.path.join(self.root, "log", "old"), os.path.join(self.root, "log", "new"))
        self.assertEqual([one], reg.rename_path("log/old", "log/new"))
        self.assertEqual((["log/new/map.md"], "log/new/map.md"), (one["evidence"], one["owner"]))
        reg.save()


if __name__ == "__main__":
    unittest.main()
