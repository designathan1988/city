# The hook's refusals (.claude/hooks/port_gate.py), each with the command or the edit that must be refused and the one beside
# it that must pass; and the hook's own entry (port_state.py), which refuses everything when the gate does not load.
import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
HOOKS = ROOT / ".claude" / "hooks"
sys.path.insert(0, str(HOOKS))

import port_gate  # noqa: E402

RUN = "python tools/tasks/cli.py run"
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
    """A session's record in which these tools were called: (name, input) each."""
    path = folder / "session.jsonl"
    path.write_text("".join(json.dumps({"type": "assistant", "message": {"content": [
        {"type": "tool_use", "name": name, "input": given}]}}) + "\n" for name, given in tools), encoding="utf-8")
    return str(path)


class GateCase(unittest.TestCase):
    def setUp(self):
        self.folder = Path(tempfile.mkdtemp())
        self.held = port_gate.BLOCK
        port_gate.BLOCK = self.folder / "state.json"

    def tearDown(self):
        port_gate.BLOCK = self.held

    def state(self, state: str) -> None:
        port_gate.BLOCK.write_text(json.dumps({"state": state, "name": "traffic"}), encoding="utf-8")

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
        bare = transcript(self.folder, ("Skill", {"skill": "port"}))
        cause = {"command": 'python tools/tasks/cli.py block open --cause "x_test: a.cpp:3: why"'}
        commit = {"command": 'git commit -m "traffic"'}
        self.assertTrue(self.refused("Bash", cause, bare))
        self.assertTrue(self.refused("Bash", commit, bare))
        both = transcript(self.folder, ("Skill", {"skill": "superpowers:systematic-debugging"}),
                          ("Skill", {"skill": "superpowers:verification-before-completion"}))
        self.assertFalse(self.refused("Bash", cause, both))
        self.assertFalse(self.refused("Bash", commit, both))


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
        whole = transcript(self.folder, ("Skill", {"skill": "port"}), ("mcp__plugin_context7_context7__query-docs", {}),
                           ("WebSearch", {}))
        self.assertTrue(self.refused("Edit", code, whole), "no block open")
        self.state("verify")
        self.assertTrue(self.refused("Edit", code, whole), "the checks are running")
        self.state("open")
        self.assertFalse(self.refused("Edit", code, whole))
        self.assertTrue(self.refused("Edit", code, transcript(self.folder, ("Skill", {"skill": "port"}))), "no source asked")


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

    def test_a_gate_that_does_not_load_refuses_all_but_its_repair(self):
        hooks = Path(tempfile.mkdtemp()) / ".claude" / "hooks"
        hooks.mkdir(parents=True)
        (hooks / "port_state.py").write_bytes((HOOKS / "port_state.py").read_bytes())
        (hooks / "port_gate.py").write_text('A = "cut\n', encoding="utf-8")
        said = self.ask(hooks, {"hook_event_name": "PreToolUse", "tool_name": "Bash", "tool_input": {"command": "ls"}})
        self.assertEqual(said["hookSpecificOutput"]["permissionDecision"], "deny")
        mend = {"hook_event_name": "PreToolUse", "tool_name": "Edit", "tool_input": {"file_path": str(hooks / "port_gate.py")}}
        self.assertEqual(self.ask(hooks, mend), {})
        self.assertIn("does not load", self.ask(hooks, {"hook_event_name": "SessionStart"})["hookSpecificOutput"]["additionalContext"])


if __name__ == "__main__":
    unittest.main()
