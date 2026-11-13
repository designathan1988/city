# The block rule's mechanism: the moves its table allows and refuses, the refusal every tool asks while a block is open (a run
# and a serve of the orchestrator included), and the block's times appended to the process history at its close.
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
        (self.root / block.HISTORY).parent.mkdir(parents=True)
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

    def test_serve_is_refused_while_open(self):
        self.block("open", "US1")
        err = StringIO()
        with redirect_stdout(StringIO()), redirect_stderr(err):
            self.assertEqual(cli.main(["serve", "--root", str(self.root)]), 1)
        self.assertIn("refused", err.getvalue())


if __name__ == "__main__":
    unittest.main()
