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
        self.assertEqual(self.block("open", "roads"), 0)
        self.assertEqual(self.block("verify"), 0)
        self.assertEqual(self.block("open"), 0, "a failed check goes back to the code")
        self.assertEqual(block.load(self.root)["name"], "roads", "opened again under its own name")
        self.assertEqual(self.block("verify"), 0)
        self.assertEqual(self.block("close"), 0)
        self.assertEqual(block.load(self.root)["state"], block.CLOSED)

    def test_refused_moves(self):
        self.assertEqual(self.block("verify"), 1, "nothing to verify while closed")
        self.assertEqual(self.block("open"), 1, "a block is opened with its name")
        self.assertEqual(self.block("open", "roads"), 0)
        self.assertEqual(self.block("close"), 1, "a block closes only after its checks")
        self.assertEqual(self.block("open", "traffic"), 1, "one block at a time")

    def test_close_appends_the_times_to_the_history(self):
        self.block("open", "signals")
        self.block("verify")
        self.block("close")
        text = (self.root / block.HISTORY).read_bytes().decode("utf-8")
        rows = [line for line in text.split("\n") if line.startswith("| signals |")]
        self.assertEqual(len(rows), 1)
        self.assertLess(text.index("| signals |"), text.index("## Later"))
        self.assertNotIn("\r", text)


class Refusals(BlockCase):
    def test_tools_refused_only_while_open(self):
        self.assertIsNone(block.refusal("build", self.root))
        self.block("open", "roads")
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
        self.block("open", "roads")
        with redirect_stdout(StringIO()), redirect_stderr(StringIO()):
            self.assertEqual(cli.main(run), 1)
        self.block("verify")
        with redirect_stdout(StringIO()), redirect_stderr(StringIO()):
            self.assertEqual(cli.main(run), 0)

    def test_serve_is_refused_while_open(self):
        self.block("open", "roads")
        err = StringIO()
        with redirect_stdout(StringIO()), redirect_stderr(err):
            self.assertEqual(cli.main(["serve", "--root", str(self.root)]), 1)
        self.assertIn("refused", err.getvalue())


class Brake(BlockCase):
    """No trial and error: a failed check is not run again as it is, the code does not change while the checks run, the block
    opens again only with each failed check's cause at a file:line that exists, and a third failed verification stops."""

    def verify(self, failed=("lane_test",)):
        self.block("verify")
        block.record({name: "FAIL" for name in failed} | {"other_test": "PASS", "build_debug": "PASS"}, self.root)

    def test_a_failed_check_is_not_run_again(self):
        self.block("open", "traffic")
        self.verify()
        self.assertIn("is not run again", block.refusal("run lane_test", self.root, ["lane_test"]))
        self.assertIsNone(block.refusal("run other_test", self.root, ["other_test"]), "what passed may run")
        self.assertEqual(self.block("close"), 1, "a block closes on checks that pass")

    def test_the_block_opens_again_only_with_the_cause(self):
        self.block("open", "traffic")
        self.verify()
        self.assertEqual(self.block("open"), 1, "no cause")
        self.assertEqual(self.block("open", "--cause", "it probably races"), 1, "no check named, no file:line")
        self.assertEqual(self.block("open", "--cause", "lane_test: gone.cpp:3: x"), 1, "a file that does not exist")
        self.assertEqual(self.block("open", "--cause", "lane_test: a.txt:9: x"), 1, "a line the file does not have")
        self.assertEqual(self.block("open", "--cause", "lane_test: a.txt:1: the gap is read after the move"), 0)
        doc = block.load(self.root)
        self.assertEqual((doc["state"], doc["round"], len(doc["causes"])), (block.OPEN, 1, 1))
        self.block("verify")
        self.assertIsNone(block.refusal("run lane_test", self.root, ["lane_test"]), "its one run after the fix")

    def test_a_third_failed_verification_stops(self):
        self.block("open", "traffic")
        for _ in range(block.MAX_ROUNDS):
            self.verify()
            self.assertEqual(self.block("open", "--cause", "lane_test: a.txt:1: why"), 0)
        self.verify()
        self.assertEqual(self.block("open", "--cause", "lane_test: a.txt:1: why"), 1, "past the rounds: the owner's word")
        self.assertEqual(self.block("open", "--cause", "lane_test: a.txt:1: why", "--owner", "go on"), 0)

    def test_a_failed_build_names_its_cause_and_is_no_round(self):
        self.block("open", "traffic")
        self.block("verify")
        block.record({"build_debug": "FAIL"}, self.root)
        self.assertEqual(self.block("open"), 1)
        self.assertEqual(self.block("open", "--cause", "build_debug: a.txt:1: a name the header lacks"), 0)
        self.assertEqual(block.load(self.root)["round"], 0)

    def test_the_code_does_not_change_while_the_checks_run(self):
        self.block("open", "traffic")
        self.block("verify")
        self.assertIsNone(block.refusal("build", self.root))
        code = self.root / "native" / "src" / "x.cpp"
        code.parent.mkdir(parents=True)
        code.write_text("int x;\n", encoding="utf-8")
        self.assertIn("changed since", block.refusal("build", self.root))
        (code.parent / "x.gd.uid").write_text("uid\n", encoding="utf-8")  # what the engine writes is no change
        self.block("open")
        self.block("verify")
        self.assertIsNone(block.refusal("build", self.root), "verified again: the code as it stands is what is checked")


if __name__ == "__main__":
    unittest.main()
