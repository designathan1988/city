"""The inventory's tests on two made-up trees written for each test (a reference with texts in two languages, a union of
tools, a registered command, a module, a constant table and a folder no area names; a game with StoreApi, an interface
table, a translation table, a tool enum, native modules, a data table and a test): every feature read with its source line,
the pairs by text (accents, case and punctuation aside), by text key and by id, the unpaired lists, the unclassified folder
reported, and the command line's exit codes and reports."""
import contextlib
import io
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
from atlas import cli  # noqa: E402
from atlas import inventory  # noqa: E402
from atlas import inventory_rules as rules  # noqa: E402
from atlas import inventory_systems as systems  # noqa: E402

ACCENTED = "Zorv" + chr(0xE1) + "n Blip!"  # an invented word with an accented letter, written so the source stays ASCII
REFERENCE = {
    "ui/i18n/en.ts": "export const ONE = {\n  'tool.lane': 'Lanes',\n  'hint.only': 'Only the reference says this',\n"
                     "  'blip.zone': 'Paint ' +\n    'zone',\n  'hand.lane': \"Lane tool\",\n};\n",
    "ui/i18n/pt-BR.ts": "export const TWO = {\n  'tool.lane': '" + ACCENTED + "',\n};\n",
    "editor/hand.ts": "/** The hand: what the player holds. */\nexport type HandTool = 'lane' | 'zone';\n"
                      "command('map.keep', () => 1);\nhint('hint.only');\n",
    "sim/traffic/flowStep.ts": "// Moves the cars one step.\nexport function step() {}\n",
    "world/catalog.ts": "export const LAMP_KINDS = ['sodium', 'led'] as const;\n"
                        "export const BLOCKS: readonly B[] = [\n  { id: 'tower', h: 3 },\n  { id: 'shed' },\n];\n",
    "mystery/thing.ts": "// Something no area names.\nexport const x = 1;\n",
}
CITY = {
    "project/game/store/store_api.gd": "## StoreApi.\nclass_name StoreApi\nconst COMMANDS := {\n\t&\"build_lane\": \"build_lane\","
                                       " &\"keep_map\": \"keep_map\",\n}\nconst QUERIES := {\n\t&\"lane_at\": \"lane_at\",\n}\n",
    "project/data/ui/drawers.json": "{\n \"categories\": [\n  {\n   \"id\": \"lanes\",\n   \"label_key\": \"tool.lane\",\n"
                                    "   \"tabs\": [\n    {\"id\": \"draw\", \"tool\": \"LANE\", \"label_key\": \"paint.zone\"}\n   ]\n"
                                    "  }\n ]\n}\n",
    "project/data/i18n/strings.csv": "keys,pt_BR,en\ntool.lane,zorvan blip,Lanes\ncity.only,Glorp,Only the city says this\n"
                                     "paint.zone,Glorp zone,\"Paint, zone\"\nlane.other,Plonk,Lanes\n",
    "project/game/tools/tool_controller.gd": "## The tool in hand.\nextends Node\nenum Tool { NONE,\n\tLANE }\n",
    "native/src/sim/traffic/car_step.h": "// Moves cars, one tick.\n#pragma once\nenum class DriveMode : int { Calm,  // slow, it waits\n"
                                         "  Hasty = 2 };\n",
    "native/src/sim/traffic/car_step.cpp": "#include \"car_step.h\"\n",
    "native/src/mystery/x.h": "// A module no area names.\n",
    "project/data/catalog/street_furniture.json": "{\n \"lamps\": [\n  {\"id\": \"sodium\"},\n  {\"id\": \"mercury\"}\n ]\n}\n",
    "project/tests/core/lane_test.gd": "## Proves the lanes.\nextends Node\n",
}


def _write(root, files):
    for rel, text in files.items():
        path = os.path.join(root, rel)
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path, "w", encoding="utf-8", newline="\n") as fh:
            fh.write(text)


class InventoryCase(unittest.TestCase):
    def setUp(self):
        self.ref = tempfile.mkdtemp(prefix="inventory-ref-")
        self.city = tempfile.mkdtemp(prefix="inventory-city-")
        _write(self.ref, REFERENCE)
        _write(self.city, CITY)
        self.doc = inventory.build(self.city, self.ref, progress=lambda _line: None)
        self.by_id = {f["id"]: f for f in self.doc["features"]}

    def tearDown(self):
        shutil.rmtree(self.ref, ignore_errors=True)
        shutil.rmtree(self.city, ignore_errors=True)

    def one(self, game, kind, name):
        found = [f for f in self.doc["features"] if (f["game"], f["kind"], f["name"]) == (game, kind, name)]
        self.assertEqual(len(found), 1, f"{game} {kind} {name}: {found}")
        return found[0]

    def pairs_of(self, feature):
        return {(self.by_id[p["city"]]["kind"], self.by_id[p["city"]]["name"]): p["evidence"]
                for p in self.doc["pairs"] if p["reference"] == feature["id"]}


class Extraction(InventoryCase):
    def test_reference_features_with_sources(self):
        text = self.one("reference", "text", "tool.lane")
        self.assertEqual((text["source"], text["label"], text["texts"]["pt"]),
                         ("ui/i18n/en.ts:2", "Lanes", ACCENTED))
        self.assertEqual(self.one("reference", "text", "blip.zone")["label"], "Paint zone")
        tool = self.one("reference", "action", "HandTool.lane")
        self.assertEqual((tool["source"], tool["area"], tool["label"]), ("editor/hand.ts:2", "roads", "Lane tool"))
        self.assertEqual(self.one("reference", "action", "map.keep")["source"], "editor/hand.ts:3")
        system = self.one("reference", "system", "sim/traffic/flowStep")
        self.assertEqual((system["area"], system["description"]), ("traffic", "Moves the cars one step."))
        self.assertEqual(self.one("reference", "data", "LAMP_KINDS/sodium")["area"], "street furniture")
        self.assertEqual(self.one("reference", "data", "BLOCKS/shed")["source"], "world/catalog.ts:4")

    def test_city_features_with_sources(self):
        text = self.one("city", "text", "tool.lane")
        self.assertEqual((text["source"], text["texts"]), ("project/data/i18n/strings.csv:2", {"pt": "zorvan blip", "en": "Lanes"}))
        self.assertEqual(self.one("city", "action", "build_lane")["source"], "project/game/store/store_api.gd:4")
        self.assertEqual(self.one("city", "query", "lane_at")["source"], "project/game/store/store_api.gd:7")
        tab = self.one("city", "action", "LANE")
        self.assertEqual((tab["source"], tab["label"]), ("project/data/ui/drawers.json:7", "Paint, zone"))
        self.assertEqual(self.one("city", "action", "lanes")["label"], "Lanes")
        member = self.one("city", "action", "Tool.LANE")
        self.assertEqual((member["source"], member["label"]), ("project/game/tools/tool_controller.gd:4", "Lanes"))
        self.assertEqual(self.one("city", "action", "DriveMode.Hasty")["source"], "native/src/sim/traffic/car_step.h:4")
        self.assertEqual(self.one("city", "action", "DriveMode.Calm")["area"], "traffic")
        module = self.one("city", "system", "native/src/sim/traffic/car_step")
        self.assertEqual((module["area"], module["description"]), ("traffic", "Moves cars, one tick."))
        self.assertEqual(self.one("city", "data", "street_furniture/lamps/mercury")["source"],
                         "project/data/catalog/street_furniture.json:4")
        self.assertEqual(self.one("city", "test", "project/tests/core/lane_test")["description"], "Proves the lanes.")

    def test_a_text_first_use(self):
        self.assertEqual(self.one("reference", "text", "hint.only")["used"], "editor/hand.ts:4")
        self.assertEqual(self.one("reference", "text", "hint.only")["uses"], ["editor/hand.ts:4"])
        self.assertEqual(self.one("reference", "text", "blip.zone")["used"], "")
        self.assertEqual(self.one("city", "text", "tool.lane")["used"], "project/data/ui/drawers.json:5")

    def test_languages_read_from_the_tables(self):
        self.assertEqual(self.doc["languages"]["shared"], ["en", "pt"])


class Pairing(InventoryCase):
    def test_same_key_and_text_accents_case_and_punctuation_aside(self):
        evidence = self.pairs_of(self.one("reference", "text", "tool.lane"))[("text", "tool.lane")]
        self.assertEqual(sorted(evidence), ["key", "text:en", "text:pt"])

    def test_same_text_under_another_key(self):
        self.assertEqual(self.pairs_of(self.one("reference", "text", "blip.zone")), {("text", "paint.zone"): ["text:en"]})

    def test_a_language_that_differs_is_said(self):
        evidence = self.pairs_of(self.one("reference", "text", "tool.lane"))[("text", "lane.other")]
        self.assertEqual(evidence, ["text:en", "differs:pt"])

    def test_same_id_in_the_same_area(self):
        self.assertEqual(self.pairs_of(self.one("reference", "data", "LAMP_KINDS/sodium")),
                         {("data", "street_furniture/lamps/sodium"): ["id"]})
        tools = self.pairs_of(self.one("reference", "action", "HandTool.lane"))
        self.assertEqual(tools[("action", "Tool.LANE")], ["id"])
        self.assertEqual(tools[("action", "LANE")], ["id"])

    def test_no_pair_without_evidence(self):
        for game, kind, name in (("reference", "text", "hint.only"), ("reference", "action", "HandTool.zone"),
                                 ("city", "text", "city.only"), ("city", "data", "street_furniture/lamps/mercury")):
            with self.subTest(name=name):
                f = self.one(game, kind, name)
                self.assertEqual(inventory.select(self.doc, game=game, unpaired=True).count(dict(f, paired=[])), 1)

    def test_summary_counts_and_evidence(self):
        row = self.doc["summary"]["street furniture"]
        self.assertEqual(row["reference"], {"features": 2, "paired": 1, "only": 1})
        self.assertEqual(row["city"], {"features": 2, "paired": 1, "only": 1})
        self.assertEqual(self.doc["evidence"]["pairs"], len(self.doc["pairs"]))
        self.assertGreaterEqual(self.doc["evidence"]["only id"], 3)

    def test_unknown_folder_reported_unclassified(self):
        self.assertEqual(self.one("reference", "system", "mystery/thing")["area"], "unclassified")
        self.assertEqual(self.one("city", "system", "native/src/mystery/x")["area"], "unclassified")
        places = {(u["game"], u["place"]) for u in self.doc["unclassified"]}
        self.assertIn(("reference", "mystery"), places)
        self.assertIn(("city", "native/src/mystery"), places)


class CommandLine(InventoryCase):
    def atlas(self, *argv):
        out = io.StringIO()
        with contextlib.redirect_stdout(out), contextlib.redirect_stderr(io.StringIO()):
            code = cli.main(list(argv) + ["--root", self.city, "--reference", self.ref])
        return code, out.getvalue()

    def test_summary_and_reports(self):
        code, out = self.atlas("inventory")
        self.assertEqual(code, 0, out)
        self.assertIn("street furniture", out)
        self.assertIn("reference: 6 files read", out)
        with open(os.path.join(self.city, "dist", "inventory", "inventory.json"), encoding="utf-8") as fh:
            self.assertEqual(len(json.load(fh)["features"]), len(self.doc["features"]))
        with open(os.path.join(self.city, "dist", "inventory", "inventory.md"), encoding="utf-8") as fh:
            report = fh.read()
        self.assertIn("### Reference only", report)
        self.assertIn("ui/i18n/en.ts:3", report)

    def test_filters_and_exit_codes(self):
        code, out = self.atlas("inventory", "--game", "reference", "--unpaired")
        self.assertEqual(code, 0, out)
        self.assertIn("hint.only", out)
        self.assertNotIn("city.only", out)
        code, out = self.atlas("inventory", "--area", "street furniture", "--json")
        self.assertEqual(code, 0, out)
        self.assertEqual({f["area"] for f in json.loads(out)}, {"street furniture"})
        self.assertEqual(self.atlas("inventory", "--area", "moon")[0], 2)


class Systems(unittest.TestCase):
    def test_a_files_own_name_before_its_folder(self):
        self.assertEqual(rules.module_area("native/src/sim/traffic/ped_steps.cpp"), "people")
        self.assertEqual(rules.module_area("native/src/sim/traffic/car_step.h"), "traffic")
        self.assertEqual(rules.module_area("native/src/sim/traffic/flow_step.h"), "traffic")

    def test_an_id_pair_needs_tables_that_share_a_word(self):
        self.assertTrue(inventory.same_table({"table": "LAMP_KINDS"}, {"table": "street_furniture/lamps"}))
        self.assertFalse(inventory.same_table({"table": "TOOLS"}, {"table": "tuning"}))
        self.assertFalse(inventory.same_table({"table": "TOOLS"}, {"table": "materials"}))
        self.assertTrue(inventory.same_table({}, {"table": "tuning"}))

    def test_systems_pair_through_the_features_they_share(self):
        def f(id_, game, kind, source, **extra):
            return dict(id=id_, game=game, kind=kind, source=source, area="roads", name=id_, **extra)
        features = [f("r1", "reference", "system", "sim/lanes.ts:1"), f("r2", "reference", "system", "ui/hub.ts:1"),
                    f("c1", "city", "system", "native/src/sim/roads/lanes.h:1"),
                    f("c2", "city", "system", "project/game/store/hub.gd:1"),
                    f("rt1", "reference", "text", "i18n/en.ts:1", uses=["sim/lanes.ts:5"]),
                    f("rt2", "reference", "text", "i18n/en.ts:2", uses=["sim/lanes.ts:9"]),
                    f("rt3", "reference", "text", "i18n/en.ts:3", uses=["ui/hub.ts:2"]),
                    f("ct1", "city", "text", "strings.csv:1", uses=["native/src/sim/roads/lanes.cpp:4"]),
                    f("ct2", "city", "text", "strings.csv:2", uses=["native/src/sim/roads/lanes.cpp:8"]),
                    f("ct3", "city", "text", "strings.csv:3", uses=["project/game/store/hub.gd:3"])]
        pairs = [{"reference": "rt1", "city": "ct1"}, {"reference": "rt2", "city": "ct2"},
                 {"reference": "rt3", "city": "ct3"}]
        found = systems.pair_systems(features, pairs)
        self.assertEqual([(p["reference"], p["city"], p["evidence"], p["shared"]) for p in found],
                         [("r1", "c1", ["via:text"], 2)])

    def test_a_reference_folder_that_does_not_exist_is_refused(self):
        city = tempfile.mkdtemp(prefix="inventory-city-")
        try:
            out = io.StringIO()
            with contextlib.redirect_stdout(out), contextlib.redirect_stderr(io.StringIO()):
                code = cli.main(["inventory", "--root", city, "--reference", os.path.join(city, "nowhere")])
            self.assertEqual(code, 1)
            self.assertIn("does not exist", out.getvalue())
        finally:
            shutil.rmtree(city, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
