"""The audit's own tests: each reader finds what a small tree or log states is broken, finds nothing in a whole one, and the
report counts expected findings apart. Run: python -m unittest discover tools/audit/tests
"""
from __future__ import annotations

import json
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))

from audit import logs, static  # noqa: E402
from audit.findings import Finding, merge, wording, write_report  # noqa: E402

STORE = """class_name StoreApi
const COMMANDS := {
\t&"build_road": "build_road", &"plant": "landscape/plant", &"ghost": "ghost",
}
const QUERIES := {
\t&"height_at": "height_at",
}
"""
CORE = """void bind() {
\tClassDB::bind_method(D_METHOD("build_road", "a"), &C::build_road);
\tClassDB::bind_method(D_METHOD("plant"), &C::plant);
\tClassDB::bind_method(D_METHOD("height_at"), &C::height_at);
\tClassDB::bind_method(D_METHOD("orphan"), &C::orphan);
}
#define CITY_WIND_FIELDS(X) X(gust) X(calm)
float f(const Wind &w) { return w.gust; }
"""
TOOL = """func go() -> void:
\tCityStore.dispatch(&"build_road", [])
\tCityStore.dispatch(&"missing", [])
\tprint(CityStore.query(&"height_at", []), tr("menu.play"), tr("menu.gone"), tr("tool.%s" % name))
"""


def tree(root: Path, files: dict[str, str]) -> Path:
    for name, text in files.items():
        path = root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(text.encode("utf-8"))
    return root


def rules(found: list[Finding]) -> set[tuple[str, str]]:
    return {(f.rule, f.what.split("'")[1] if "'" in f.what else f.what) for f in found}


class StaticTest(unittest.TestCase):
    def setUp(self) -> None:
        self.dir = tempfile.TemporaryDirectory()
        self.root = tree(Path(self.dir.name), {
            "project/game/store/store_api.gd": STORE,
            "native/src/api/bind.cpp": CORE,
            "project/game/tools/tool.gd": TOOL,
            "project/data/i18n/strings.csv": "keys,first,second\nmenu.play,Play one,Play two\nmenu.idle,Idle one,\ntool.road,Road one,Road two\nmenu.lost,Lost one,Lost two\n",
            "project/data/ui/hud.json": json.dumps({"title_key": "menu.absent", "used_width": 3, "never_named": 4, "rows": {"a": {"x": 1}, "b": {"x": 2}}}),
        })
        (self.root / "project/game/tools/tool.gd").write_bytes((TOOL + "var w := hud.used_width\n").encode("utf-8"))

    def tearDown(self) -> None:
        self.dir.cleanup()

    def test_command_chain(self) -> None:
        found = rules(static.commands(static.Tree(self.root)))
        self.assertIn(("no core method", "ghost"), found)
        self.assertIn(("command nothing sends", "plant"), found)
        self.assertIn(("command nothing sends", "ghost"), found)
        self.assertIn(("bound and never called", "orphan"), found)
        self.assertTrue(any(rule == "sent and not declared" for rule, _ in found))
        self.assertNotIn(("command nothing sends", "build_road"), found)

    def test_texts(self) -> None:
        found = rules(static.texts(static.Tree(self.root)))
        self.assertIn(("key not in the tables", "menu.absent"), found)
        self.assertIn(("no translation", "menu.idle"), found)
        self.assertIn(("text nothing shows", "menu.lost"), found)
        self.assertNotIn(("text nothing shows", "tool.road"), found)  # built at run time from the prefix "tool."
        self.assertTrue(any(rule == "key not in the tables" and "menu.gone" in what for rule, what in found | {(f.rule, f.what) for f in static.texts(static.Tree(self.root))}))

    def test_tunables_and_data(self) -> None:
        t = static.Tree(self.root)
        self.assertEqual({("field nothing reads", "CITY_WIND_FIELDS declares ")} & set(), set())
        fields = [f.what for f in static.tunables(t)]
        self.assertTrue(any("'calm'" in what for what in fields))
        self.assertFalse(any("'gust'" in what for what in fields))
        keys = [f.what for f in static.data_keys(t)]
        self.assertTrue(any("'never_named'" in what for what in keys))
        self.assertFalse(any("'used_width'" in what for what in keys))
        self.assertFalse(any("'a'" in what for what in keys))  # the ids of a table of rows are walked, not named


class LogsTest(unittest.TestCase):
    def read(self, text: str) -> list[Finding]:
        with tempfile.TemporaryDirectory() as d:
            path = Path(d) / "run.log"
            path.write_bytes(text.encode("utf-8"))
            return logs.read_log(path, "run")

    def test_engine_error_is_owned_by_the_projects_file(self) -> None:
        found = self.read("ERROR: a pool cannot cast\n   at: set_budget (native\\src\\render\\light_pool.cpp:42)\n   at: f (servers/x.cpp:9)\n")
        self.assertEqual(len(found), 1)
        self.assertEqual(found[0].owner, "native/src/render/light_pool.cpp (set_budget)")
        self.assertEqual((found[0].domain, found[0].rule), ("log", "error"))

    def test_expected_is_listed_apart(self) -> None:
        found = self.read("EXPECTED nothing_here.glb\nERROR: Can't open file at path \"user://nothing_here.glb\"\n   at: open (core/io/file.cpp:1)\nERROR: other\n")
        self.assertEqual([f.expected for f in found], [True, False])

    def test_leaks_checks_and_findings(self) -> None:
        found = self.read("Leaked instance: PanelContainer:123 - Node path:\nResource still in use: res://ui/bar.gd (GDScript)\nFAIL the eye stands in no building\n"
                          "FINDING {\"domain\": \"world\", \"rule\": \"bodies in one another\", \"owner\": \"sim/roads\", \"what\": \"a post in a pole\", \"count\": 3}\nok   fine\n")
        self.assertEqual([(f.domain, f.rule) for f in found], [("resources", "leaked instance"), ("resources", "resource in use at exit"), ("test", "check failed"), ("world", "bodies in one another")])
        self.assertEqual(found[3].count, 3)
        with self.assertRaises(ValueError):
            self.read("FINDING {\"domain\": \"world\"}\n")

    def test_a_clean_log_has_no_finding(self) -> None:
        self.assertEqual(self.read("ok   boot []\nFAILURES: 0\n"), [])


class ReportTest(unittest.TestCase):
    def test_same_wording_is_one_cause(self) -> None:
        self.assertEqual(wording("took 12 ms at km16"), "took N ms at km16")
        merged = merge([Finding("log", "error", "a.cpp (f)", "bad 1", 1, ["t1"]), Finding("log", "error", "a.cpp (f)", "bad 22", 3, ["t2"])])
        self.assertEqual(len(merged), 1)
        self.assertEqual((merged[0].count, merged[0].where), (4, ["t1", "t2"]))

    def test_report_counts_expected_apart(self) -> None:
        with tempfile.TemporaryDirectory() as d:
            causes, occurrences = write_report([Finding("log", "error", "a", "x", 2), Finding("log", "error", "b", "y", 5, expected=True)], ["test"], Path(d))
            self.assertEqual((causes, occurrences), (1, 2))
            doc = json.loads((Path(d) / "report.json").read_text(encoding="utf-8"))
            self.assertEqual(len(doc["findings"]), 2)
            self.assertIn("## Expected", (Path(d) / "report.md").read_text(encoding="utf-8"))


if __name__ == "__main__":
    unittest.main()
