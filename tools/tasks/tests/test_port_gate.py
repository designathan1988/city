# The hook's refusals (.claude/hooks/port_gate.py), each with the command or the edit that must be refused and the one beside
# it that must pass, and the case they share with test_port_memory.py (what a compaction may not lose); and the hook's own
# entry (port_state.py), which refuses everything when the gate does not load.
import json
import os
import subprocess
import sys
import tempfile
import time
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
HOOKS = ROOT / ".claude" / "hooks"
sys.path.insert(0, str(HOOKS))

import port_gate  # noqa: E402
import port_looks  # noqa: E402
import port_memory  # noqa: E402
import port_note  # noqa: E402
import port_skills  # noqa: E402

RUN ="python tools/tasks/cli.py run"
COMPACTED = ("compact", None)   # a compaction's boundary in a session's record
NOTE = "# traffic\n\n## Now\n- the block is open; next: the far traffic.\n\n## Read\n- a fact.\n"
# (command, started in the background, refused)
DEADLINE = "--seconds 60"       # a run's own deadline: what a run carries
SHELL = [
    (f"{RUN} tests {DEADLINE}", True, False),
    (f"cd /c/Users/x/city && {RUN} near {DEADLINE}", True, False),
    (f"{RUN} tests {DEADLINE}", False, True),                       # held in the foreground
    (f"{RUN} tests {DEADLINE} 2>&1 | grep FAIL", True, True),       # muted
    (f"{RUN} lab_test --jobs 1 {DEADLINE} > out.txt", True, True),
    (f"tools/build/build.sh core-windows {DEADLINE} | tail -3", True, True),
    (f"python tools/tasks/cli.py block verify && {RUN} tests {DEADLINE}", True, True),   # joined
    (f"{RUN} near {DEADLINE}; {RUN} lab_test {DEADLINE}", True, True),
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


PLAN = "flows_look"             # the functionality's own photo plan, in these cases


def stamp(at: float | None = None) -> str:
    """A record's time: `at`, or a moment from now (so a file written before the call was written before it)."""
    return time.strftime("%Y-%m-%dT%H:%M:%S", time.gmtime(time.time() + 2 if at is None else at)) + ".000Z"


def transcript(folder: Path, *tools) -> str:
    """A session's record in which these tools were called: (name, input) each, COMPACTED where it was compacted, an
    entry (a dict) as it is."""
    path = folder / "session.jsonl"
    path.write_text("".join(json.dumps(
        tool if isinstance(tool, dict) else {"type": "system", "subtype": "compact_boundary"} if tool == COMPACTED else
        {"type": "assistant", "timestamp": stamp(),
         "message": {"content": [{"type": "tool_use", "id": f"call_{n}", "name": tool[0], "input": tool[1]}]}}) + "\n"
        for n, tool in enumerate(tools)), encoding="utf-8")
    return str(path)


class GateCase(unittest.TestCase):
    def setUp(self):
        self.folder = Path(tempfile.mkdtemp())
        self.held = port_gate.BLOCK, port_note.WORK, port_note.NOW_SEEN, port_memory.WORDS
        port_note.NOW_SEEN, port_memory.WORDS = self.folder / "now_seen.json", self.folder / "owner_words.md"
        self.looks = {name: getattr(port_looks, name) for name in ("STATE", "LOOKS", "ORDER", "SHOTS", "REFS", "SUITES")}
        port_gate.BLOCK = port_looks.STATE = self.folder / "state.json"
        port_looks.LOOKS, port_looks.ORDER = self.folder / "looks.json", self.folder / "order.json"
        port_looks.SHOTS, port_looks.REFS = self.folder / "captures", self.folder / "look"
        port_looks.SUITES = self.folder / "suites.json"
        port_looks.SUITES.write_text(json.dumps({"suites": {"tests": {}}, "groups": {"looks": {}}, "tasks": [
            {"name": PLAN, "plan": "res://tests/visual/flows_plan.json"},
            {"name": port_looks.FAR, "plan": "res://tests/visual/zoom_ladder_plan.json"}]}), encoding="utf-8")
        self.written = port_looks.written
        port_looks.written = lambda file: "flows_plan" in file or "zoom_ladder" in file   # both written in the block's work
        # no domain skill on this machine (test_port_skills.py has its own)
        self.skills = port_skills.ROOT, os.environ.get(port_skills.CACHE)
        port_skills.ROOT = self.folder / "none"
        os.environ[port_skills.CACHE] = str(self.folder / "none")
        port_note.WORK = self.folder / "work"
        port_note.WORK.mkdir()
        self.note = port_note.WORK / "traffic.md"
        self.note.write_text(NOTE, encoding="utf-8")
        self.shot = port_looks.SHOTS / PLAN / "00-near.png"
        self.far = port_looks.SHOTS / port_looks.FAR / "00-far.png"
        for picture in (self.shot, self.far):
            picture.parent.mkdir(parents=True)
            picture.write_bytes(b"png")
        # the session stood on its records: the skill invoked, the block's note Read
        self.stood = (("Skill", {"skill": "port"}), ("Read", {"file_path": str(self.note)}))
        # and looked at the functionality: each photo of its own plan opened, and put before the owner
        self.opened = (("Read", {"file_path": str(self.shot)}),)
        self.seen = (*self.opened, ("SendUserFile", {"files": [str(self.shot)], "status": "normal"}))

    def tearDown(self):
        port_looks.written = self.written
        port_gate.BLOCK, port_note.WORK, port_note.NOW_SEEN, port_memory.WORDS = self.held
        for name, value in self.looks.items():
            setattr(port_looks, name, value)
        port_skills.ROOT = self.skills[0]
        if self.skills[1] is None:
            del os.environ[port_skills.CACHE]
        else:
            os.environ[port_skills.CACHE] = self.skills[1]

    def state(self, state: str, runs: dict | None = None) -> None:
        """The block in `state`; in verify its run photographed the functionality by its own plan, unless `runs` says what ran."""
        ran = {PLAN: "PASS"} if runs is None else runs
        port_gate.BLOCK.write_text(json.dumps({"state": state, "name": "traffic", "runs": ran}), encoding="utf-8")

    def refused(self, tool: str, given: dict, record: str = "") -> bool:
        return port_gate.gate({"tool_name": tool, "tool_input": given, "transcript_path": record}) is not None


class Shell(GateCase):
    def test_each_command(self):
        for tool in port_gate.SHELLS:
            for command, background, refused in SHELL:
                with self.subTest(tool=tool, command=command):
                    self.assertEqual(self.refused(tool, {"command": command, "run_in_background": background}), refused)

    def test_a_skill_whose_call_failed_was_not_invoked(self):
        self.state("open")
        failed = {"type": "user", "message": {"content": [{"type": "tool_result", "tool_use_id": "call_0", "is_error": True,
                                                           "content": "Shell command failed"}]}}
        copy = {"type": "api-request-blob", "message": {"content": [{"type": "tool_use", "id": "x", "name": "Skill",
                                                                     "input": {"skill": "port"}}]}}
        read = ("Read", {"file_path": str(self.note)})
        look = {"command": "git status"}
        self.assertTrue(self.refused("Bash", look, transcript(self.folder, ("Skill", {"skill": "port"}), failed, read)))
        self.assertTrue(self.refused("Bash", look, transcript(self.folder, copy, read)), "a copy of a request is no call")
        self.assertFalse(self.refused("Bash", look, transcript(self.folder, ("Skill", {"skill": "port"}), failed, read,
                                                               ("Skill", {"skill": "port"}))), "invoked again, and it answered")

    def test_the_registers_owner_is_no_order_of_the_owners(self):
        add = {"command": 'python tools/atlas/cli.py problems add --title "x" --owner "native/src/sim/x.cpp" --area lots'}
        self.assertFalse(self.refused("Bash", add, transcript(self.folder, *self.stood)))
        order = {"command": 'python tools/tasks/cli.py block order "RUN THE WHOLE SUITE NOW PLEASE"'}
        self.assertTrue(self.refused("Bash", order, transcript(self.folder, *self.stood)), "words the owner did not write")

    def test_a_cause_and_a_commit_ask_their_skills(self):
        self.state("verify")
        bare = transcript(self.folder, *self.stood)
        cause = {"command": 'python tools/tasks/cli.py block open --cause "x_test: a.cpp:3: why"'}
        commit = {"command": 'git commit -m "traffic"'}
        self.assertTrue(self.refused("Bash", cause, bare))
        self.assertTrue(self.refused("Bash", commit, bare))
        skills = (("Skill", {"skill": "superpowers:systematic-debugging"}),
                  ("Skill", {"skill": "superpowers:verification-before-completion"}))
        self.assertTrue(self.refused("Bash", commit, transcript(self.folder, *self.stood, *skills)), "the photos not opened")
        self.assertTrue(self.refused("Bash", commit, transcript(self.folder, *self.stood, *self.opened, *skills)),
                        "the photos opened and never put before the owner")
        both = transcript(self.folder, *self.stood, *self.seen, *skills)
        self.assertFalse(self.refused("Bash", cause, both))
        self.assertFalse(self.refused("Bash", commit, both))
        self.state("verify", {"flows_test": "PASS"})
        self.assertTrue(self.refused("Bash", commit, both), "a commit with the functionality not photographed")
        self.state("verify", {PLAN: "FAIL"})
        self.assertTrue(self.refused("Bash", commit, both), "a commit with its photo plan failed")
        self.state("verify", {port_looks.FAR: "PASS"})
        far = transcript(self.folder, *self.stood, ("Read", {"file_path": str(self.far)}),
                         ("SendUserFile", {"files": [str(self.far)], "status": "normal"}), *skills)
        self.assertTrue(self.refused("Bash", commit, far), "the far views of the town are no functionality's own photos")
        self.state("verify")
        port_looks.written = lambda file: False
        self.assertTrue(self.refused("Bash", commit, both), "a plan the block did not write or change is not its own")


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
                given = self.ask(HOOKS, {"hook_event_name": "SessionStart", "source": source})["hookSpecificOutput"]["additionalContext"]
                self.assertLessEqual(len(given), 10000, "Claude Code cuts a longer answer to a preview of 2,000 characters")

    def test_a_gate_that_does_not_load_refuses_all_but_its_repair(self):
        hooks = Path(tempfile.mkdtemp()) / ".claude" / "hooks"
        hooks.mkdir(parents=True)
        for name in ("port_state.py", "port_note.py", "port_looks.py", "port_edits.py", "port_memory.py", "port_deadline.py"):
            (hooks / name).write_bytes((HOOKS / name).read_bytes())
        (hooks / "port_gate.py").write_text('A = "cut\n', encoding="utf-8")
        said = self.ask(hooks, {"hook_event_name": "PreToolUse", "tool_name": "Bash", "tool_input": {"command": "ls"}})
        self.assertEqual(said["hookSpecificOutput"]["permissionDecision"], "deny")
        mend = {"hook_event_name": "PreToolUse", "tool_name": "Edit", "tool_input": {"file_path": str(hooks / "port_gate.py")}}
        self.assertEqual(self.ask(hooks, mend), {})
        self.assertIn("does not load", self.ask(hooks, {"hook_event_name": "SessionStart"})["hookSpecificOutput"]["additionalContext"])


if __name__ == "__main__":
    unittest.main()
