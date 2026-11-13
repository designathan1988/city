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

    def block(self, *args, reviewed: bool = True) -> int:
        if args[0] == "verify" and reviewed:
            block.reviewed(self.root)  # the static review passed on the code as it stands (the gates themselves have their tests)
        with redirect_stdout(StringIO()), redirect_stderr(StringIO()):
            return cli.main(["block", *args, "--root", str(self.root)])


class Moves(BlockCase):
    def test_the_whole_way(self):
        self.assertEqual(block.load(self.root)["state"], block.CLOSED)
        self.assertEqual(self.block("open", "roads"), 0)
        self.assertEqual(self.block("verify"), 0)
        self.assertEqual(self.block("open"), 1, "never opened again for nothing")
        self.assertEqual(self.block("open", "--cause", "a photo shows nothing: a.txt:1: the view is too far"), 0, "what was found, where it stands")
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
        self.assertEqual(self.block("verify", reviewed=False), 1, "no verification before the static review")

    def test_a_block_is_verified_a_few_times_at_most(self):
        self.block("open", "roads")
        for _ in range(block.MAX_VERIFIES):
            self.assertEqual(self.block("verify"), 0)
            self.assertEqual(self.block("open", "--cause", "found: a.txt:1: why"), 0)
        self.assertEqual(self.block("verify"), 1, "more verifications are trial and error by another name")
        self.assertEqual(self.block("verify", "--owner", "go on"), 0)

    def test_a_verification_has_one_run_of_checks_and_one_measure(self):
        self.block("open", "roads")
        self.block("verify")
        self.assertIsNone(block.refusal("run tests", self.root, ["build_debug", "lane_test", "near"]))
        block.record({"build_debug": "PASS", "lane_test": "PASS", "near": "PASS"}, self.root)
        self.assertIn("already had its run of checks", block.refusal("run near", self.root, ["near"]))
        self.assertIsNone(block.refusal("build", self.root, ["build_debug", "build_release"]), "a build is no check")
        self.assertIsNone(block.refusal("run lab_test", self.root, ["lab_test"]), "the measure is the second run")
        block.record({"lab_test": "PASS"}, self.root)
        self.assertIn("already had its run of measure", block.refusal("run lab_test", self.root, ["lab_test"]))
        self.block("open", "--cause", "found: a.txt:1: why")
        self.block("verify")
        self.assertIsNone(block.refusal("run tests", self.root, ["lane_test"]), "a new verification, its own run")

    def test_code_changed_after_the_review_is_reviewed_again(self):
        self.block("open", "roads")
        block.reviewed(self.root)
        code = self.root / "native" / "src" / "x.cpp"
        code.parent.mkdir(parents=True)
        code.write_text("int x;\n", encoding="utf-8")
        self.assertEqual(self.block("verify", reviewed=False), 1, "the review was of other code")

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
        self.assertIn("already had its run", block.refusal("run other_test", self.root, ["other_test"]), "one run a verification")
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

    def test_a_check_the_register_names_does_not_hold_the_block(self):
        (self.root / "doc").mkdir(exist_ok=True)
        (self.root / "doc" / "problems.jsonl").write_text(
            json.dumps({"id": "P1", "status": "open", "title": "the camera jolts", "criterion": "camera_feel_test passes"}) + "\n"
            + json.dumps({"id": "P2", "status": "open", "title": "a story", "symptom": "lane_test failed once"}) + "\n", encoding="utf-8")
        self.block("open", "traffic")
        self.block("verify")
        block.record({"camera_feel_test": "FAIL"}, self.root)
        self.assertEqual(self.block("close"), 0, "a failure the register owns is not this block's")
        self.block("open", "traffic")
        self.verify()
        self.assertEqual(self.block("close"), 1, "a check a problem only mentions is not owned by it")

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
        self.block("open", "--cause", "found: a.txt:1: why")
        self.block("verify")
        self.assertIsNone(block.refusal("build", self.root), "verified again: the code as it stands is what is checked")


if __name__ == "__main__":
    unittest.main()
