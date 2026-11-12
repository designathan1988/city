# The project's hooks (.claude/hooks): on the shell, the standing authorisation's limits (deletions by a relative path, through
# "..", by a variable or outside the allowed places; force pushes; rewritten history) and the block rule; on agents, at most
# AT_ONCE at once (the places given back when one stops or fails to start) and no Bash or PowerShell inside one; each hook as
# Claude Code runs it (the event on stdin; exit 2 refuses, 0 lets the call through).
import importlib.util
import json
import os
import subprocess
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
HOOKS = ROOT / ".claude" / "hooks"
REFUSED = 2


def _load(name):
    spec = importlib.util.spec_from_file_location(name, HOOKS / f"{name}.py")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


destructive = _load("destructive_guard")
agents = _load("agent_guard")


def run_event(name, event):
    r = subprocess.run([sys.executable, str(HOOKS / f"{name}.py")], input=json.dumps(event), capture_output=True, text=True,
                       timeout=60)
    return r.returncode, r.stderr


def run_hook(name, command):
    return run_event(name, {"hook_event_name": "PreToolUse", "tool_name": "Bash", "tool_input": {"command": command}})


class Destructive(unittest.TestCase):
    roots = [os.path.normcase(os.path.abspath(str(ROOT)))]
    inside = str(ROOT / "dist" / "old")

    def test_refused(self):
        for command in ["rm -rf ../../AppData", "rm -rf dist/old", f"rm -rf {ROOT}/../elsewhere", 'rm -rf "$stage"',
                        "Remove-Item -Recurse -Force .\\dist", "rm -rf C:/Windows/Temp/x", "git push --force origin main",
                        "git push -f", "git push origin +main", "git rebase main", "git commit --amend -m x",
                        "git filter-branch --all", "rmdir /s /q build"]:
            with self.subTest(command=command):
                self.assertIsNotNone(destructive.refusal(command, self.roots))

    def test_allowed(self):
        msys = "/" + self.inside[0].lower() + self.inside[2:].replace("\\", "/") if os.name == "nt" else self.inside
        for command in [f'rm -rf "{self.inside}"', f"Remove-Item -Recurse -Force '{self.inside}'", f"rm -rf {msys}",
                        "git push origin main",
                        "git rm -q log/how-we-work.md", "git commit -m 'a new commit'", "ls -la", "git worktree prune"]:
            with self.subTest(command=command):
                self.assertIsNone(destructive.refusal(command, self.roots))

    def test_the_hook_refuses_with_exit_2(self):
        code, said = run_hook("destructive_guard", "rm -rf ../../../AppData")
        self.assertEqual(code, REFUSED)
        self.assertIn("relative path", said)
        self.assertEqual(run_hook("destructive_guard", "git status")[0], 0)


class BlockHook(unittest.TestCase):
    def test_the_hook_lets_through_while_no_block_is_open_here(self):
        state = ROOT / "dist" / "block" / "state.json"
        if state.is_file() and json.loads(state.read_text(encoding="utf-8")).get("state") == "open":
            self.skipTest("a block of this checkout is open: the hook's refusal is test_block's case")
        self.assertEqual(run_hook("block_guard", "godot --path project")[0], 0)


class AgentHook(unittest.TestCase):
    def setUp(self):
        self.session = f"test-agent-guard-{os.getpid()}"
        self.count = Path(agents._count_file(self.session))
        self.addCleanup(lambda: self.count.unlink(missing_ok=True))
        self.addCleanup(lambda: Path(f"{self.count}.lock").unlink(missing_ok=True))

    def call(self, n, **given):
        return run_event("agent_guard", {"hook_event_name": "PreToolUse", "tool_name": "Agent", "session_id": self.session,
                                         "tool_use_id": f"use-{n}", "tool_input": {"prompt": "read", **given}})

    def test_any_kind_while_places_are_free(self):
        for given in [{}, {"subagent_type": "general-purpose"}, {"subagent_type": "reader"}, {"subagent_type": "Explore"}]:
            with self.subTest(given=given):
                self.assertIsNone(agents.refusal(given, agents.AT_ONCE - 1))
                self.assertIsNotNone(agents.refusal(given, agents.AT_ONCE))

    def test_an_agent_runs_only_in_the_research_folder(self):
        lab = "cd C:/Users/Jonathan/Documents/Projects/defect-finder-research/experiments && python x.py"
        for tool in ("Bash", "PowerShell"):
            for given, allowed in [({"command": lab, "timeout": 60000}, True), ({"command": "ls"}, False),
                                   ({"command": lab}, False), ({"command": lab, "timeout": 60000, "run_in_background": True}, False),
                                   ({"command": lab + " C:/Users/Jonathan/Desktop/city", "timeout": 60000}, False)]:
                with self.subTest(tool=tool, given=given):
                    inside = {"hook_event_name": "PreToolUse", "tool_name": tool, "session_id": self.session, "agent_id": "a1",
                              "agent_type": "general-purpose", "tool_input": given}
                    self.assertEqual(run_event("agent_guard", inside)[0], 0 if allowed else REFUSED)
        main = {"hook_event_name": "PreToolUse", "tool_name": "Bash", "session_id": self.session, "tool_input": {"command": "ls"}}
        self.assertEqual(run_event("agent_guard", main)[0], 0, "the main session runs commands")

    def test_the_count_held_and_given_back(self):
        for n in range(agents.AT_ONCE):
            self.assertEqual(self.call(n, subagent_type="reader")[0], 0)
        code, said = self.call(agents.AT_ONCE, subagent_type="reader")
        self.assertEqual(code, REFUSED)
        self.assertIn("at most", said)
        stop = {"hook_event_name": "SubagentStop", "session_id": self.session, "agent_type": "reader", "agent_id": "a1"}
        self.assertEqual(run_event("agent_guard", stop)[0], 0)
        self.assertEqual(self.call(agents.AT_ONCE + 1, subagent_type="reader")[0], 0, "a stopped reader gives its place back")
        failed = {"hook_event_name": "PostToolUseFailure", "tool_name": "Agent", "session_id": self.session,
                  "tool_use_id": f"use-{agents.AT_ONCE + 1}"}
        self.assertEqual(run_event("agent_guard", failed)[0], 0)
        self.assertEqual(self.call(agents.AT_ONCE + 2, subagent_type="general-purpose")[0], 0, "an agent that never started holds none")


if __name__ == "__main__":
    unittest.main()
