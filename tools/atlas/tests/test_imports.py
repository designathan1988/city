"""The register's imports: the gates' standing findings entered once per rule and file (a second import adds nothing), each in
the area its place says; the areas by place; the statistics, the median minutes from a report to its cause included."""
import os
import sys
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.abspath(os.path.join(HERE, "..", "..")))
from test_problems import RegisterCase  # noqa: E402

from atlas import areas, imports  # noqa: E402

FINDINGS = [
    {"rule": "MAGIC_NUMBER", "where": "native/src/sim/traffic/lane_change.cpp:40", "file": "native/src/sim/traffic/lane_change.cpp",
     "line": 40, "message": "a magic number"},
    {"rule": "MAGIC_NUMBER", "where": "native/src/sim/traffic/lane_change.cpp:52", "file": "native/src/sim/traffic/lane_change.cpp",
     "line": 52, "message": "a magic number"},
    {"rule": "STATE_IN_UI", "where": "project/ui/compass.gd:13", "file": "project/ui/compass.gd", "line": 13, "message": "state"},
]


class GateImport(RegisterCase):
    def test_one_problem_per_rule_and_file_once(self):
        reg = self.reg()
        added = imports.gate_findings(reg, FINDINGS)
        self.assertEqual([(p["area"], p["kind"]) for p in added], [("traffic", "architecture"), ("interface", "architecture")])
        self.assertIn("lines 40, 52", added[0]["symptom"])
        reg.save()
        reg = self.reg()
        self.assertEqual(imports.gate_findings(reg, FINDINGS), [])

    def test_stats(self):
        reg = self.reg()
        doc = imports.stats(reg)
        self.assertEqual(doc["problems"], 3)
        self.assertEqual(doc["by_status"], {"open": 1, "traced": 2})
        self.assertEqual(doc["minutes_to_cause"], {"measured": 1, "median": 10.0})


class Areas(unittest.TestCase):
    def test_longest_place_wins(self):
        self.assertEqual(areas.area_of("native/src/sim/traffic/x.cpp"), "traffic")
        self.assertEqual(areas.area_of("native/src/sim/simulation/simulation.cpp"), "simulation")
        self.assertEqual(areas.area_of("project/data/ui/hud.json"), "interface")
        self.assertEqual(areas.area_of("CLAUDE.md"), "process")


if __name__ == "__main__":
    unittest.main()
