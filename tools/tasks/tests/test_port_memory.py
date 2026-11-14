# What a compaction may not lose (.claude/hooks/port_note.py, port_memory.py, port_gate.ungrounded): the session stands on the
# skill and the note again before it changes or runs anything; the note's `## Now` is never far behind the work and is
# rewritten when the block moves; every answer fits what Claude Code gives whole; the owner's words and the work since
# `## Now` come back from the session's own record, the words the text cannot hold kept whole on disk and Read.
import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from test_port_gate import COMPACTED, HOOKS, NOTE, RUN, GateCase, transcript  # noqa: E402

import port_gate  # noqa: E402
import port_looks  # noqa: E402
import port_memory  # noqa: E402
import port_note  # noqa: E402


def said(text: str, at: str) -> dict:
    """An entry of the owner's in a session's record."""
    return {"type": "user", "timestamp": at, "message": {"role": "user", "content": text}}


def called(name: str, given: dict, at: str) -> dict:
    """A call of the session's in its record."""
    return {"type": "assistant", "timestamp": at, "message": {"content": [{"type": "tool_use", "name": name, "input": given}]}}


class Compaction(GateCase):
    """What a compaction may not lose."""
    SOURCES = (("mcp__plugin_context7_context7__query-docs", {}), ("WebSearch", {}))

    def test_nothing_is_changed_or_run_before_the_session_stands_on_its_records(self):
        self.state("verify")
        look = {"command": "git status"}
        other = {"file_path": str(self.folder / "c.md"), "content": "x"}
        skill, read = self.stood
        for record, refused in (((skill, read), False), ((skill, read, COMPACTED), True), ((skill, read, COMPACTED, skill), True),
                                ((skill, read, COMPACTED, read), True), ((skill, read, COMPACTED, read, skill), False),
                                ((skill,), True), ((read,), True)):
            with self.subTest(record=[name for name, _ in record]):
                made = transcript(self.folder, *record)
                self.assertEqual(self.refused("Bash", look, made), refused)
                self.assertEqual(self.refused("Write", other, made), refused)
        self.state("closed")
        self.assertFalse(self.refused("Bash", look, transcript(self.folder, skill)), "no block and no compaction")
        self.assertTrue(self.refused("Bash", look, transcript(self.folder, COMPACTED)), "a compaction, whatever the block")
        self.assertTrue(self.refused("Bash", look, transcript(self.folder, COMPACTED, skill)))
        part = ("Read", {"file_path": str(self.note), "limit": 40})
        self.assertTrue(self.refused("Bash", look, transcript(self.folder, COMPACTED, skill, part)), "the note Read in part")
        self.assertFalse(self.refused("Bash", look, transcript(self.folder, COMPACTED, skill, read)))

    def age_now(self) -> None:
        """The note's `## Now` as last rewritten longer ago than the gate allows."""
        seen = json.loads(port_note.NOW_SEEN.read_text(encoding="utf-8"))
        seen["at"] -= port_note.NOTE_SECONDS + 60
        port_note.NOW_SEEN.write_text(json.dumps(seen), encoding="utf-8")

    def test_the_note_is_never_far_behind_the_work(self):
        self.state("open")
        made = transcript(self.folder, *self.stood, *self.SOURCES)
        code = {"file_path": str(port_gate.ROOT / "native" / "src" / "sim" / "x.cpp"), "old_string": "a", "new_string": "b"}
        run = {"command": f"{RUN} flows_test", "run_in_background": True}
        self.assertFalse(self.refused("Edit", code, made))
        self.assertFalse(self.refused("Bash", run, made))
        self.age_now()
        self.assertTrue(self.refused("Edit", code, made), "the game's code with the note behind")
        self.assertTrue(self.refused("Bash", run, made), "a run with the note behind")
        self.assertTrue(self.refused("Bash", {"command": 'git commit -m "x"'}, made), "a commit with the note behind")
        self.assertFalse(self.refused("Bash", {"command": "git status"}, made))
        self.assertFalse(self.refused("Edit", {"file_path": str(self.note), "old_string": "next:", "new_string": "then:"}, made),
                         "the note itself is what is written")
        self.note.write_text(NOTE + "- a fact read since.\n", encoding="utf-8")
        self.assertTrue(self.refused("Edit", code, made), "written below `## Now` with the state above left as it was")
        self.note.write_text(NOTE.replace("the far traffic", "the near traffic"), encoding="utf-8")
        self.assertFalse(self.refused("Edit", code, made), "`## Now` rewritten")
        self.note.unlink()
        self.assertTrue(self.refused("Edit", code, made), "a block with no note")

    def test_the_state_is_rewritten_when_the_block_moves(self):
        self.state("open")
        made = transcript(self.folder, *self.stood, *self.SOURCES)
        run = {"command": f"{RUN} flows_test", "run_in_background": True}
        self.assertFalse(self.refused("Bash", run, made))
        self.state("verify")
        self.assertTrue(self.refused("Bash", run, made), "`## Now` written while the block was open, the block in verify")
        self.assertIn("written when the block was open", "\n".join(port_note.note_back(port_gate.block())))
        self.note.write_text(NOTE.replace("is open", "is in verify"), encoding="utf-8")
        self.assertFalse(self.refused("Bash", run, made))

    def test_a_note_keeps_its_state(self):
        made = transcript(self.folder, *self.stood)
        self.assertTrue(self.refused("Write", {"file_path": str(self.note), "content": "# traffic\n\n## Read\n- a fact.\n"}, made))
        self.assertTrue(self.refused("Edit", {"file_path": str(self.note), "old_string": "## Now", "new_string": "## Then"}, made))
        long = "# traffic\n\n## Now\n" + "- a step.\n" * 2000 + "\n## Read\n"
        self.assertFalse(self.refused("Write", {"file_path": str(self.note), "content": long}, made),
                         "a long state: the hook gives back what fits and the note is Read whole")
        self.assertFalse(self.refused("Write", {"file_path": str(self.note), "content": NOTE + "- another fact.\n"}, made))
        self.assertEqual(port_note.now_of(NOTE), "- the block is open; next: the far traffic.")

    def test_an_answer_fits_what_is_given_whole(self):
        self.assertEqual(port_note.fit("x" * 100), "x" * 100)
        cut = port_note.fit("x" * 20000)
        self.assertLessEqual(len(cut), port_note.HOOK_MOST)
        self.assertTrue(cut.endswith(port_note.CUT))
        self.state("verify")
        self.note.write_text("# traffic\n\n## Now\n" + "- a step.\n" * 500 + "\n## Read\n" + "- a fact.\n" * 5000, encoding="utf-8")
        back = "\n".join(port_note.note_back(port_gate.block()))
        self.assertIn("- a step.", back)
        self.assertNotIn("- a fact.", back, "the history is Read, the state is given")


class Memory(GateCase):
    """What a compaction gives back from the session's record (port_memory.py)."""

    def record(self, *entries: dict) -> str:
        path = self.folder / "record.jsonl"
        path.write_text("".join(json.dumps(e) + "\n" for e in entries), encoding="utf-8")
        return str(path)

    def test_the_owners_words_and_the_work_since_come_back(self):
        self.state("open")
        port_note.now_seen(port_gate.block())
        seen = json.loads(port_note.NOW_SEEN.read_text(encoding="utf-8"))
        seen["at"] = port_looks.when("2026-11-14T10:00:00Z")
        port_note.NOW_SEEN.write_text(json.dumps(seen), encoding="utf-8")
        made = self.record(
            said("port the traffic, whole", "2026-11-14T09:00:00Z"),
            {"type": "queue-operation", "content": "and keep the buses first"},
            said("and keep the buses first", "2026-11-14T09:30:00Z"),
            called("Edit", {"file_path": str(port_gate.ROOT / "native" / "src" / "old.cpp")}, "2026-11-14T09:40:00Z"),
            {"type": "system", "subtype": "compact_boundary", "timestamp": "2026-11-14T10:30:00Z"},
            called("Edit", {"file_path": str(port_gate.ROOT / "native" / "src" / "new.cpp")}, "2026-11-14T10:40:00Z"),
            called("Bash", {"command": "git status\nsecond line"}, "2026-11-14T10:41:00Z"))
        back = port_memory.given_back(made, port_gate.block(), ["native/src/new.cpp"])
        self.assertLess(back.index("[2] and keep the buses first"), back.index("[1] port the traffic, whole"), "newest first")
        self.assertEqual(back.count("keep the buses first"), 1, "a queued prompt is one message")
        self.assertIn("native/src/new.cpp (1)", back)
        self.assertNotIn("old.cpp", back, "changed before `## Now` was rewritten")
        self.assertIn("  - git status", back)
        self.assertNotIn("second line", back)
        self.assertFalse(port_memory.WORDS.is_file(), "every word fits the hook's text")

    def test_words_the_text_cannot_hold_are_kept_and_read(self):
        self.state("open")
        long = [said(f"order {n}: " + "x" * 3000, f"2026-11-14T09:0{n}:00Z") for n in range(5)]
        back = port_memory.given_back(self.record(*long), port_gate.block(), [])
        self.assertLessEqual(len(back), port_note.HOOK_MOST)
        self.assertIn("[5] order 4", back, "the newest first")
        self.assertNotIn("[1] order 0", back)
        kept = port_memory.WORDS.read_text(encoding="utf-8")
        self.assertTrue(all(f"order {n}: " in kept for n in range(5)), "all of them, in order")
        look = {"command": "git status"}
        self.assertTrue(self.refused("Bash", look, transcript(self.folder, COMPACTED, *self.stood)),
                        "the owner's words not Read after the compaction")
        words = ("Read", {"file_path": str(port_memory.WORDS)})
        self.assertFalse(self.refused("Bash", look, transcript(self.folder, COMPACTED, *self.stood, words)))
        self.assertFalse(self.refused("Bash", look, transcript(self.folder, *self.stood)), "no compaction in this session")


class Entry(unittest.TestCase):
    """port_state.py's second text at a compaction, as Claude Code calls it."""

    def test_a_compaction_gives_back_the_record_in_a_text_of_its_own(self):
        record = Path(tempfile.mkdtemp()) / "record.jsonl"
        record.write_text(json.dumps(said("x" * 30000, "2026-11-14T09:00:00Z")) + "\n", encoding="utf-8")
        done = subprocess.run([sys.executable, str(HOOKS / "port_state.py"), "hook", "memory"], capture_output=True, timeout=30,
                              input=json.dumps({"hook_event_name": "SessionStart", "source": "compact",
                                                "transcript_path": str(record)}).encode("utf-8"))
        self.assertEqual(done.returncode, 0, done.stderr.decode("utf-8", "replace"))
        given = json.loads(done.stdout)["hookSpecificOutput"]["additionalContext"]
        self.assertLessEqual(len(given), 10000, "Claude Code measures each hook's text on its own")
        self.assertIn("owner_words.md", given, "a word too long for the text is kept whole on disk")
        port_memory.WORDS.unlink(missing_ok=True)


if __name__ == "__main__":
    unittest.main()
