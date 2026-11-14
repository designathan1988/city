# The hook's refusals (.claude/hooks/port_gate.py), each with the command or the edit that must be refused and the one beside
# it that must pass; what a compaction may not lose (the session stands on the skill and the note again before it changes or
# runs anything; the note is never far behind the work and always has its state; every answer fits what Claude Code gives
# whole); and the hook's own entry (port_state.py), which refuses everything when the gate does not load.
import json
import os
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
HOOKS = ROOT / ".claude" / "hooks"
sys.path.insert(0, str(HOOKS))

import port_gate  # noqa: E402
import port_note  # noqa: E402

RUN = "python tools/tasks/cli.py run"
COMPACTED = ("compact", None)   # a compaction's boundary in a session's record
NOTE = "# traffic\n\n## Now\n- the block is open; next: the far traffic.\n\n## Read\n- a fact.\n"
# (command, started in the background, refused)
SHELL = [
    (f"{RUN} tests", True, False),
    (f"cd /c/Users/x/city && {RUN} near", True, False),
    (f"{RUN} tests", False, True),                                  # held in the foreground
    (f"{RUN} tests 2>&1 | grep FAIL", True, True),                  # muted
    (f"{RUN} lab_test --jobs 1 > out.txt", True, True),
    ("tools/build/build.sh core-windows | tail -3", True, True),
    (f"python tools/tasks/cli.py block verify && {RUN} tests", True, True),   # joined
    (f"{RUN} near; {RUN} lab_test", True, True),
    (f"{RUN} tests --list", False, False),
    ("python tools/tasks/cli.py status", False, False),
    ("python tools/tasks/cli.py block status | tail -1", False, False),
    ("git log | head -3", False, False),
    ('git commit -m "the block: build.sh named in a message"', False, False),
    ("git add tools/build/build.sh", False, False),
    ("python - <<'EOF'\nprint(1)\nEOF", False, True),               # a script typed into the shell
    ("cd x && python3 - <<EOF\nprint(1)\nEOF", False, True),
    ("cat > a.txt <<'EOF'\ntext\nEOF", False, True),
    ("@'\nprint(1)\n'@ | python -", False, True),
    ('git commit -m "$(cat <<\'EOF\'\nthe message\nEOF\n)"', False, False),
    ("python C:/scratch/fix.py", False, False),
    ("sed -i 's/a/b/' native/src/sim/x.cpp", False, True),          # the project's files written by the shell
    ("echo x > .claude/hooks/port_gate.py", False, True),
    ("echo x >> tools/tasks/block.py", False, True),
    ("Set-Content -Path project/data/x.json -Value y", False, True),
    ('grep -n "v->x" native/src/sim/x.cpp', False, False),
    ('grep -c ">" tools/tasks/block.py', False, False),
    ("godot --headless", False, True),
    ("ninja -C build", False, True),
]


def transcript(folder: Path, *tools: tuple) -> str:
    """A session's record in which these tools were called: (name, input) each, COMPACTED where it was compacted."""
    path = folder / "session.jsonl"
    path.write_text("".join(json.dumps(
        {"type": "system", "subtype": "compact_boundary"} if (name, given) == COMPACTED else
        {"type": "assistant", "message": {"content": [{"type": "tool_use", "name": name, "input": given}]}}) + "\n"
        for name, given in tools), encoding="utf-8")
    return str(path)


class GateCase(unittest.TestCase):
    def setUp(self):
        self.folder = Path(tempfile.mkdtemp())
        self.held = port_gate.BLOCK, port_note.WORK
        port_gate.BLOCK = self.folder / "state.json"
        port_note.WORK = self.folder / "work"
        port_note.WORK.mkdir()
        self.note = port_note.WORK / "traffic.md"
        self.note.write_text(NOTE, encoding="utf-8")
        # the session stood on its records: the skill invoked, the block's note Read
        self.stood = (("Skill", {"skill": "port"}), ("Read", {"file_path": str(self.note)}))

    def tearDown(self):
        port_gate.BLOCK, port_note.WORK = self.held

    def state(self, state: str, runs: dict | None = None) -> None:
        """The block in `state`; in verify its run photographed the look, unless `runs` says what ran."""
        ran = {"zoom_ladder": "PASS"} if runs is None else runs
        port_gate.BLOCK.write_text(json.dumps({"state": state, "name": "traffic", "runs": ran}), encoding="utf-8")

    def refused(self, tool: str, given: dict, record: str = "") -> bool:
        return port_gate.gate({"tool_name": tool, "tool_input": given, "transcript_path": record}) is not None


class Shell(GateCase):
    def test_each_command(self):
        for tool in port_gate.SHELLS:
            for command, background, refused in SHELL:
                with self.subTest(tool=tool, command=command):
                    self.assertEqual(self.refused(tool, {"command": command, "run_in_background": background}), refused)

    def test_a_cause_and_a_commit_ask_their_skills(self):
        self.state("verify")
        bare = transcript(self.folder, *self.stood)
        cause = {"command": 'python tools/tasks/cli.py block open --cause "x_test: a.cpp:3: why"'}
        commit = {"command": 'git commit -m "traffic"'}
        self.assertTrue(self.refused("Bash", cause, bare))
        self.assertTrue(self.refused("Bash", commit, bare))
        both = transcript(self.folder, *self.stood, ("Skill", {"skill": "superpowers:systematic-debugging"}),
                          ("Skill", {"skill": "superpowers:verification-before-completion"}))
        self.assertFalse(self.refused("Bash", cause, both))
        self.assertFalse(self.refused("Bash", commit, both))
        self.state("verify", {"flows_test": "PASS"})
        self.assertTrue(self.refused("Bash", commit, both), "a commit with the look not photographed")
        self.state("verify", {"zoom_ladder": "FAIL"})
        self.assertTrue(self.refused("Bash", commit, both), "a commit with the look's plan failed")


class Edits(GateCase):
    def test_a_file_is_not_left_unreadable(self):
        code = self.folder / "a.py"
        code.write_text('import re\nA = re.compile(r"x[^;&\\n]*")\n', encoding="utf-8")
        cut = {"file_path": str(code), "old_string": '[^;&\\n]*")', "new_string": '[^;&\n]*")'}
        self.assertTrue(self.refused("Edit", cut), "a raw string cut by a real new line")
        self.assertFalse(self.refused("Edit", {"file_path": str(code), "old_string": "A =", "new_string": "B ="}))
        self.assertTrue(self.refused("Write", {"file_path": str(self.folder / "b.json"), "content": '{"a": 1,}'}))
        self.assertFalse(self.refused("Write", {"file_path": str(self.folder / "b.json"), "content": '{"a": 1}'}))
        self.assertFalse(self.refused("Write", {"file_path": str(self.folder / "c.md"), "content": "{"}))

    def test_the_games_code_asks_its_block_and_its_sources(self):
        code = {"file_path": str(port_gate.ROOT / "native" / "src" / "sim" / "x.cpp"), "old_string": "a", "new_string": "b"}
        whole = transcript(self.folder, *self.stood, ("mcp__plugin_context7_context7__query-docs", {}), ("WebSearch", {}))
        self.assertTrue(self.refused("Edit", code, whole), "no block open")
        self.state("verify")
        self.assertTrue(self.refused("Edit", code, whole), "the checks are running")
        self.state("open")
        self.assertFalse(self.refused("Edit", code, whole))
        self.assertTrue(self.refused("Edit", code, transcript(self.folder, *self.stood)), "no source asked")


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
        self.assertFalse(self.refused("Bash", look, transcript(self.folder, COMPACTED)), "no block: nothing to stand on")

    def test_the_note_is_never_far_behind_the_work(self):
        self.state("open")
        made = transcript(self.folder, *self.stood, *self.SOURCES)
        code = {"file_path": str(port_gate.ROOT / "native" / "src" / "sim" / "x.cpp"), "old_string": "a", "new_string": "b"}
        run = {"command": f"{RUN} flows_test", "run_in_background": True}
        self.assertFalse(self.refused("Edit", code, made))
        self.assertFalse(self.refused("Bash", run, made))
        old = self.note.stat().st_mtime - port_note.NOTE_SECONDS - 60
        os.utime(self.note, (old, old))
        self.assertTrue(self.refused("Edit", code, made), "the game's code with the note behind")
        self.assertTrue(self.refused("Bash", run, made), "a run with the note behind")
        self.assertTrue(self.refused("Bash", {"command": 'git commit -m "x"'}, made), "a commit with the note behind")
        self.assertFalse(self.refused("Bash", {"command": "git status"}, made))
        self.assertFalse(self.refused("Edit", {"file_path": str(self.note), "old_string": "next:", "new_string": "then:"}, made),
                         "the note itself is what is written")
        self.note.unlink()
        self.assertTrue(self.refused("Edit", code, made), "a block with no note")

    def test_a_note_keeps_its_state(self):
        made = transcript(self.folder, *self.stood)
        self.assertTrue(self.refused("Write", {"file_path": str(self.note), "content": "# traffic\n\n## Read\n- a fact.\n"}, made))
        self.assertTrue(self.refused("Edit", {"file_path": str(self.note), "old_string": "## Now", "new_string": "## Then"}, made))
        long = "# traffic\n\n## Now\n" + "x" * (port_note.NOW_MOST + 1) + "\n\n## Read\n"
        self.assertTrue(self.refused("Write", {"file_path": str(self.note), "content": long}, made), "a state no hook gives back whole")
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


class Agents(GateCase):
    def test_no_agent_while_the_port_is_in_hand(self):
        self.assertFalse(self.refused("Agent", {"prompt": "x"}))
        self.assertTrue(self.refused("Agent", {"prompt": "x"}, transcript(self.folder, ("Skill", {"skill": "port"}))))
        self.state("open")
        self.assertTrue(self.refused("Agent", {"prompt": "x"}))


class Entry(unittest.TestCase):
    """port_state.py as Claude Code calls it."""

    def ask(self, hooks: Path, payload: dict) -> dict:
        done = subprocess.run([sys.executable, str(hooks / "port_state.py"), "hook"], input=json.dumps(payload).encode("utf-8"),
                              capture_output=True, timeout=30)
        self.assertEqual(done.returncode, 0, done.stderr.decode("utf-8", "replace"))
        return json.loads(done.stdout) if done.stdout.strip() else {}

    def test_every_event_answers(self):
        for event in ("SessionStart", "UserPromptSubmit", "SubagentStart", "PreCompact", "Stop"):
            with self.subTest(event=event):
                done = subprocess.run([sys.executable, str(HOOKS / "port_state.py"), "hook"],
                                      input=json.dumps({"hook_event_name": event, "stop_hook_active": True}).encode("utf-8"),
                                      capture_output=True, timeout=30)
                self.assertEqual(done.returncode, 0, done.stderr.decode("utf-8", "replace"))

    def test_what_a_session_is_given_at_its_start_is_given_whole(self):
        for source in ("startup", "resume", "compact"):
            with self.subTest(source=source):
                said = self.ask(HOOKS, {"hook_event_name": "SessionStart", "source": source})["hookSpecificOutput"]["additionalContext"]
                self.assertLessEqual(len(said), 10000, "Claude Code cuts a longer answer to a preview of 2,000 characters")

    def test_a_gate_that_does_not_load_refuses_all_but_its_repair(self):
        hooks = Path(tempfile.mkdtemp()) / ".claude" / "hooks"
        hooks.mkdir(parents=True)
        (hooks / "port_state.py").write_bytes((HOOKS / "port_state.py").read_bytes())
        (hooks / "port_note.py").write_bytes((HOOKS / "port_note.py").read_bytes())
        (hooks / "port_gate.py").write_text('A = "cut\n', encoding="utf-8")
        said = self.ask(hooks, {"hook_event_name": "PreToolUse", "tool_name": "Bash", "tool_input": {"command": "ls"}})
        self.assertEqual(said["hookSpecificOutput"]["permissionDecision"], "deny")
        mend = {"hook_event_name": "PreToolUse", "tool_name": "Edit", "tool_input": {"file_path": str(hooks / "port_gate.py")}}
        self.assertEqual(self.ask(hooks, mend), {})
        self.assertIn("does not load", self.ask(hooks, {"hook_event_name": "SessionStart"})["hookSpecificOutput"]["additionalContext"])


if __name__ == "__main__":
    unittest.main()
