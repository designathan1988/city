# The block rule's mechanism: the moves its table allows and refuses, the refusal every tool asks while a block is open (a run
# of the orchestrator included), the shell commands the project's hook refuses and lets through, and the block's times
# appended to the process history at its close.
import json
import sys
import unittest
from contextlib import redirect_stderr, redirect_stdout
from io import StringIO
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))  # support
sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools: the tasks package

from support import RepoCase  # noqa: E402

from tasks import block, cli  # noqa: E402

HISTORY = "# History: the process\n\n## Block times\n\n" + block.TIMES_HEADER + "\n|---|---|---|---|---|---|\n\n## Later\n"


class BlockCase(RepoCase):
    def setUp(self):
        super().setUp()
        (self.root / "log" / "history").mkdir(parents=True)
        (self.root / block.HISTORY).write_bytes(HISTORY.encode("utf-8"))

    def block(self, *args) -> int:
        with redirect_stdout(StringIO()), redirect_stderr(StringIO()):
            return cli.main(["block", *args, "--root", str(self.root)])


class Moves(BlockCase):
    def test_the_whole_way(self):
        self.assertEqual(block.load(self.root)["state"], block.CLOSED)
        self.assertEqual(self.block("open", "US1"), 0)
        self.assertEqual(self.block("verify"), 0)
        self.assertEqual(self.block("open"), 0, "a failed verification goes back to the code")
        self.assertEqual(self.block("verify"), 0)
        self.assertEqual(self.block("close"), 0)
        self.assertEqual(block.load(self.root)["state"], block.CLOSED)

    def test_refused_moves(self):
        self.assertEqual(self.block("verify"), 1, "nothing to verify while closed")
        self.assertEqual(self.block("open"), 1, "a block is opened with its name")
        self.assertEqual(self.block("open", "US1"), 0)
        self.assertEqual(self.block("close"), 1, "a block closes only after its verification")
        self.assertEqual(self.block("open", "US2"), 1, "one block at a time")

    def test_close_appends_the_times_to_the_history(self):
        self.block("open", "US9")
        self.block("verify")
        self.block("close")
        text = (self.root / block.HISTORY).read_bytes().decode("utf-8")
        rows = [line for line in text.split("\n") if line.startswith("| US9 |")]
        self.assertEqual(len(rows), 1)
        self.assertLess(text.index("| US9 |"), text.index("## Later"))
        self.assertNotIn("\r", text)


class Refusals(BlockCase):
    def test_tools_refused_only_while_open(self):
        self.assertIsNone(block.refusal("build", self.root))
        self.block("open", "US1")
        for what in block.WHAT:
            self.assertIn("refused", block.refusal(what, self.root))
        self.block("verify")
        self.assertIsNone(block.refusal("commit", self.root))

    def test_an_orchestrator_run_is_refused_while_open(self):
        suites = self.root / "suites.json"
        suites.write_text(json.dumps({"vars": {}, "changed": {"include": ["**/*.txt"]}, "sets": {}, "templates": {}, "groups": {},
                                      "tasks": {"ok": {"argv": ["{python}", "-c", "pass"], "timeout_s": 60}},
                                      "suites": {"good": {"tasks": ["ok"]}}}), encoding="utf-8")
        run = ["run", "good", "--suites", str(suites), "--root", str(self.root)]
        self.block("open", "US1")
        with redirect_stdout(StringIO()), redirect_stderr(StringIO()):
            self.assertEqual(cli.main(run), 1)
        self.block("verify")
        with redirect_stdout(StringIO()), redirect_stderr(StringIO()):
            self.assertEqual(cli.main(run), 0)

    def test_shell_commands_refused_while_open(self):
        refused = ['"C:/Programs/Godot/godot_v4.7.2.exe" --path project', "godot --headless --path project",
                   "cmake --build native/build-win", "python -m unittest discover tools/atlas/tests",
                   "python tools/gates/run_gates.py --summary", "python tools/tasks/cli.py run quick --ready",
                   "bash tools/build/build.sh core-windows", "git add -A; git commit -m x", "& Roadcraft.exe"]
        allowed = ["git status --short", "python tools/tasks/cli.py block verify", "python tools/atlas/cli.py problems --area roads",
                   "ls native/godot-cpp", "git diff --stat"]
        for command in refused + allowed:
            self.assertIsNone(block.command_refusal(command, self.root), "a closed block refuses nothing")
        self.block("open", "US1")
        for command in refused:
            with self.subTest(command=command):
                self.assertIsNotNone(block.command_refusal(command, self.root))
        for command in allowed:
            with self.subTest(command=command):
                self.assertIsNone(block.command_refusal(command, self.root))


if __name__ == "__main__":
    unittest.main()
