"""Tests of the feature tracer: one case per rule of `rules.py`, on a small Godot + GDExtension fixture.

Run: python -m unittest discover -s tools/analysis/featuretrace/tests -t tools/analysis
"""
from __future__ import annotations

import subprocess
import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))

from featuretrace import program, rules, seeds, walk  # noqa: E402

FIXTURE = HERE / "fixture"
CLI = HERE.parents[1] / "trace_feature.py"


class Trace(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.ix, cls.rs, cls.files, cls.broken = program.load([FIXTURE / "project", FIXTURE / "native"], FIXTURE, None)

    def gid(self, spec: str) -> int:
        found = seeds.find(self.ix, spec)
        self.assertEqual(len(found), 1, f"{spec} -> {[self.ix.units[g].qual for g in found]}")
        return found[0]

    def edges(self, src: str, kind: str | None = None) -> dict:
        """target label -> edge, for the edges leaving a definition."""
        out = {}
        for e in self.rs.out[self.gid(src)]:
            if kind is None or e.kind == kind:
                out[seeds.label(self.ix, e.dst)] = e
        return out

    # ---- the table -----------------------------------------------------------------------------------------------
    def test_every_keyword_has_a_role(self):
        for table in (rules.GDSCRIPT_KEYWORDS, rules.CPP_KEYWORDS, rules.CPP_PREPROCESSOR):
            self.assertTrue(table)
            self.assertLessEqual(set(table.values()), rules.ROLES)
        for word in ("var", "const", "func", "class", "class_name", "extends", "signal", "enum", "preload", "if",
                     "elif", "else", "match", "when", "for", "while", "return", "is", "as", "self", "super"):
            self.assertIn(word, rules.GDSCRIPT_KEYWORDS)
        for word in ("class", "struct", "namespace", "using", "template", "enum", "typedef", "if", "switch", "for",
                     "while", "return", "this", "auto", "static_cast"):
            self.assertIn(word, rules.CPP_KEYWORDS)

    def test_a_class_survives_a_macro_line(self):
        # `GDCLASS(Core, Object)` has no `;`: the preprocessor expands it, the grammar sees an error there
        core = self.ix.members["fx::Core"]
        self.assertTrue({"build_road", "settle", "state", "_bind_methods"} <= set(core))

    # ---- GDScript ------------------------------------------------------------------------------------------------
    def test_typed_receiver_and_override(self):
        calls = self.edges("Controller._unhandled_input")
        self.assertEqual(calls["ToolBase.click"].how, "exact")
        self.assertEqual(calls["RoadTool.click"].kind, "override")
        self.assertEqual(calls["ToolBase.click"].cond, ("if active",))

    def test_autoload_and_string_name_reach_the_binding(self):
        out = self.edges("RoadTool._lay")
        self.assertEqual(out["Store.dispatch"].how, "exact")
        self.assertEqual(out["fx::Core::build_road"].kind, "name")
        self.assertEqual(out["fx::Core::build_road"].cond, ("if p > 0.0",))

    def test_is_narrows_the_receiver(self):
        self.assertEqual(self.edges("Controller.narrowed")["RoadTool._lay"].how, "exact")

    def test_untyped_receiver_gives_candidates(self):
        out = self.edges("Controller.untyped", "call")
        self.assertEqual(set(out), {"ToolBase.click", "RoadTool.click"})
        self.assertTrue(all(e.how.startswith("candidates") for e in out.values()))
        exact = walk.forward(self.rs, [self.gid("Controller.untyped")], exact=True)
        self.assertEqual(len(exact.reached), 1)

    def test_signals_in_code_and_in_scene(self):
        self.assertIn("Controller.picked", self.edges("Controller._on_pressed", "emit"))
        back = walk.back(self.rs, [self.gid("Controller._on_picked")])
        reasons = [r.note for r in back.rows if r.status == "entry"]
        self.assertTrue(any("engine emits" in n for n in reasons), reasons)
        scene = walk.back(self.rs, [self.gid("Controller._on_scene_pressed")])
        self.assertEqual(seeds.label(self.ix, scene.entries[0]), "engine signal 'pressed'")

    def test_engine_object_signal_and_typed_core(self):
        self.assertEqual(self.edges("Store._ready", "read")["Store._core"].how, "exact")
        fired = [e for e in self.rs.inn[self.gid("Store._on_built")] if e.kind == "signal"]
        self.assertEqual([(seeds.label(self.ix, e.src), e.how) for e in fired], [("fx::Core::built", "exact")])
        handler = walk.back(self.rs, [self.gid("Store._on_built")])
        self.assertIn(self.gid("fx::Core::build_road"), handler.reached)

    def test_extends_an_inner_class_of_a_file(self):
        self.assertEqual(self.edges("menu.open")["outer.gd.Inner.greet"].how, "exact")

    def test_back_reaches_the_engine_callback(self):
        back = walk.back(self.rs, [self.gid("fx::Core::build_road")])
        entries = {seeds.label(self.ix, g) for g in back.entries}
        self.assertIn("Controller._unhandled_input", entries)

    # ---- C++ ------------------------------------------------------------------------------------------------------
    def test_template_operator_and_library_members(self):
        core = self.gid("fx::Core::build_road")
        self.assertIn("push_back", self.rs.external[core])
        self.assertFalse([e for e in self.rs.out[core] if e.how.startswith("candidates")])

    def test_structured_binding_over_a_map(self):
        self.assertEqual(self.edges("fx::Core::build_road")["fx::Node2::touch"].how, "exact")

    def test_smart_pointer_and_virtual_override(self):
        out = self.edges("fx::Core::build_road")
        self.assertEqual(out["fx::Base::run"].how, "exact")
        self.assertEqual(out["fx::Derived::run"].kind, "override")

    def test_overload_by_arity(self):
        out = self.rs.out[self.gid("fx::Core::build_road")]
        settles = [self.ix.units[e.dst] for e in out if self.ix.units[e.dst].name == "settle"]
        self.assertEqual([len(u.params) for u in settles], [1])

    def test_internal_linkage_stays_in_its_file(self):
        out = self.edges("fx::use_other")
        self.assertEqual([self.ix.units[e.dst].file for e in out.values() if "helper" in self.ix.units[e.dst].qual],
                         ["native/other.cpp"])

    def test_writes_conditions_and_readers(self):
        down = walk.forward(self.rs, [self.gid("fx::Core::build_road")])
        written = {seeds.label(self.ix, s): w for s, w in down.writes.items()}
        self.assertEqual(written["fx::State::revision"][0][1].cond, ("if (p > 0)",))
        rows = walk.readers(self.rs, down.writes, down.reached)
        outside = {seeds.label(self.ix, s): [seeds.label(self.ix, e.src) for e in o] for s, _, o, _ in rows}
        self.assertIn("fx::reader", outside["fx::Log::road_chunks"])

    # ---- the starting point and the command ---------------------------------------------------------------------
    def test_seed_forms(self):
        self.assertEqual(seeds.find(self.ix, "project/road_tool.gd:11"), seeds.find(self.ix, "RoadTool._lay"))
        self.assertEqual(seeds.find(self.ix, "fx::Core::build_road"), seeds.find(self.ix, "Core.build_road"))
        self.assertEqual(seeds.find(self.ix, "no_such_name"), [])

    def test_command_fails_on_an_unknown_start(self):
        args = [sys.executable, str(CLI), "--no-cache", "--root", str(FIXTURE)]
        bad = subprocess.run(args + ["no_such_name"], capture_output=True, text=True, timeout=120)
        self.assertEqual(bad.returncode, 2)
        good = subprocess.run(args + ["RoadTool._lay"], capture_output=True, text=True, timeout=120)
        self.assertEqual(good.returncode, 0, good.stderr)
        self.assertIn("fx::Core::build_road", good.stdout)


if __name__ == "__main__":
    unittest.main()
