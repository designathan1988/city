# The project's PreToolUse hooks on the shell (.claude/hooks): the standing authorisation's limits (deletions by a relative
# path, through "..", by a variable or outside the allowed places; force pushes; rewritten history) and both hooks as Claude
# Code runs them (the event on stdin; exit 2 refuses, 0 lets the call through).
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


def run_hook(name, command):
    event = json.dumps({"hook_event_name": "PreToolUse", "tool_name": "Bash", "tool_input": {"command": command}})
    r = subprocess.run([sys.executable, str(HOOKS / f"{name}.py")], input=event, capture_output=True, text=True, timeout=60)
    return r.returncode, r.stderr


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


if __name__ == "__main__":
    unittest.main()
