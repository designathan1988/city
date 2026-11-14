# The domain skills an edit of the game's code asks (.claude/hooks/port_skills.py): each kind of file with the skill it is
# written with, on a machine of the test's own (a project's skills folder and a plugin cache made here); a skill invoked or
# its text Read whole meets it, a compaction forgets it, a kind with no skill installed asks nothing; and the gate itself
# refusing the edit (port_gate.gate) after the block, the note and the sources were met.
import json
import os
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / ".claude" / "hooks"))

import port_gate  # noqa: E402
import port_note  # noqa: E402
import port_skills  # noqa: E402

# (the file an edit writes, the skill of its kind that is installed in the test, another kind's skill)
EDITS = [
    ("project/render/shaders/road_body.gdshaderinc", "shader-basics", "godot-ui"),
    ("project/ui/road_profile_panel.gd", "godot-ui", "shader-basics"),
    ("native/src/render/roads/road_markings.cpp", "rendering-pipeline", "gdextension"),
    ("native/src/api/city_core_inspect.cpp", "gdextension", "godot-ui"),
    ("project/tests/core/road_grade_test.gd", "godot-testing", "godot-ui"),
]


class Skills(unittest.TestCase):
    def setUp(self):
        self.folder = Path(tempfile.mkdtemp())
        self.held = port_skills.ROOT, os.environ.get(port_skills.CACHE), port_gate.BLOCK, port_note.WORK
        port_skills.ROOT = self.folder / "project"
        os.environ[port_skills.CACHE] = str(self.folder / "cache")
        port_gate.BLOCK = self.folder / "state.json"
        port_gate.BLOCK.write_text(json.dumps({"state": "open", "name": "roads"}), encoding="utf-8")
        port_note.WORK = self.folder / "work"
        port_note.WORK.mkdir()
        self.note = port_note.WORK / "roads.md"
        self.note.write_text("# roads\n\n## Now\n- open; next: the heights.\n", encoding="utf-8")

    def tearDown(self):
        port_skills.ROOT, cache, port_gate.BLOCK, port_note.WORK = self.held
        if cache is None:
            del os.environ[port_skills.CACHE]
        else:
            os.environ[port_skills.CACHE] = cache

    def install(self, skill: str, plugin: str = "godot-prompter") -> Path:
        """The skill on this machine: a plugin's, or the project's own when `plugin` is empty."""
        home = self.folder / "cache" / "market" / plugin / "1.0" / "skills" if plugin else port_skills.ROOT / ".claude" / "skills"
        text = home / skill / "SKILL.md"
        text.parent.mkdir(parents=True)
        text.write_text("---\nname: x\n---\n", encoding="utf-8")
        return text

    def test_each_kind_asks_its_skill(self):
        for inside, skill, other in EDITS:
            with self.subTest(file=inside):
                text = self.install(skill)
                self.assertIn(skill, port_skills.owed(inside, {"skills": set(), "read": {}}))
                self.assertIn(skill, port_skills.owed(inside, {"skills": {"godot-prompter:" + other}, "read": {}}))
                self.assertEqual(port_skills.owed(inside, {"skills": {"godot-prompter:" + skill}, "read": {}}), "")
                self.assertEqual(port_skills.owed(inside, {"skills": set(), "read": {port_note.key(text): 0.0}}), "")

    def test_a_kind_with_no_skill_installed_asks_nothing(self):
        for inside, _, _ in EDITS:
            self.assertEqual(port_skills.owed(inside, {"skills": set(), "read": {}}), "")
        self.assertEqual(port_skills.owed("doc/work/roads.md", {"skills": set(), "read": {}}), "")

    def test_a_file_of_two_kinds_asks_both(self):
        self.install("godot-performance", "godot-skills")
        self.install("cpp-pro", "")
        inside = "native/src/render/roads/road_markings.cpp"
        self.assertIn("godot-performance", port_skills.owed(inside, {"skills": set(), "read": {}}))
        self.assertIn("cpp-pro", port_skills.owed(inside, {"skills": {"godot-skills:godot-performance"}, "read": {}}))
        self.assertEqual(port_skills.owed(inside, {"skills": {"godot-skills:godot-performance", "cpp-pro"}, "read": {}}), "")

    def test_the_gate_refuses_the_edit_and_a_compaction_forgets_the_skill(self):
        text = self.install("cpp-pro", "")
        record = self.folder / "session.jsonl"

        def refused(*calls: tuple) -> bool:
            record.write_text("".join(json.dumps(
                {"type": "system", "subtype": "compact_boundary"} if name == "compact" else
                {"type": "assistant", "message": {"content": [{"type": "tool_use", "name": name, "input": given}]}}) + "\n"
                for name, given in calls), encoding="utf-8")
            edit = {"file_path": str(port_gate.ROOT / "native" / "src" / "sim" / "x.cpp"), "old_string": "a", "new_string": "b"}
            return port_gate.gate({"tool_name": "Edit", "tool_input": edit, "transcript_path": str(record)}) is not None

        stood = (("Skill", {"skill": "port"}), ("Read", {"file_path": str(self.note)}),
                 ("mcp__plugin_context7_context7__query-docs", {}), ("WebSearch", {}))
        self.assertTrue(refused(*stood), "the C++ skill not used")
        self.assertTrue(refused(*stood, ("Read", {"file_path": str(text), "limit": 20})), "its text read in part")
        self.assertFalse(refused(*stood, ("Read", {"file_path": str(text)})))
        self.assertFalse(refused(*stood, ("Skill", {"skill": "cpp-pro"})))
        self.assertTrue(refused(("Skill", {"skill": "cpp-pro"}), ("compact", None), *stood), "used before the compaction")


if __name__ == "__main__":
    unittest.main()
