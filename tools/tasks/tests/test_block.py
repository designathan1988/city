# The block rule's mechanism: the moves its table allows and refuses, the refusal every tool asks while a block is open (a run
# and a serve of the orchestrator included), the block's times appended to the process history at its close, and the close refused
# while the work file lacks the method's records (block_method.py).
import json
import sys
import unittest
from contextlib import redirect_stderr, redirect_stdout
from io import StringIO
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))  # support
sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools: the tasks package

from support import RepoCase  # noqa: E402

from tasks import block, block_method, cli  # noqa: E402

HISTORY = "# History: the process\n\n## Block times\n\n" + block.TIMES_HEADER + "\n|---|---|---|---|---|---|\n\n## Later\n"
SECTIONS = {
    "Read": "- tools/x.py: 1-40 of 40, 10:00, what it does\n",
    "Research": "- Context7 /python/cpython: a question, its answer\n- Internet: https://example.org/a-page, what it says\n",
    "Checks": "- the scene: PASS, 3 s\n",
    "Skills": "- city-method; superpowers:verification-before-completion\n",
}


class BlockCase(RepoCase):
    def setUp(self):
        super().setUp()
        (self.root / block.HISTORY).parent.mkdir(parents=True)
        (self.root / block.HISTORY).write_bytes(HISTORY.encode("utf-8"))

    def block(self, *args) -> int:
        with redirect_stdout(StringIO()), redirect_stderr(StringIO()):
            return cli.main(["block", *args, "--root", str(self.root)])

    def work(self, name: str, **changed) -> None:
        """The block's work file: every section the close asks, each replaced or removed (None) by `changed`."""
        parts = {**SECTIONS, **changed}
        text = f"# {name}: a goal\n\n" + "".join(f"## {k}\n{v}\n" for k, v in parts.items() if v is not None)
        path = self.root / block_method.WORK / f"{name}.md"
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8")

    def closes(self, name: str, code: bool = True, **changed) -> int:
        """Opens `name`, changes a file (under tools/ when `code`), writes its work file and asks the close."""
        self.block("open", name)
        changed_file = self.root / ("tools/x.py" if code else "notes.txt")
        changed_file.parent.mkdir(parents=True, exist_ok=True)
        changed_file.write_text("x = 1\n", encoding="utf-8")
        self.work(name, **changed)
        self.block("verify")
        return self.block("close")


class Moves(BlockCase):
    def test_the_whole_way(self):
        self.assertEqual(block.load(self.root)["state"], block.CLOSED)
        self.assertEqual(self.block("open", "US1"), 0)
        self.assertEqual(self.block("verify"), 0)
        self.assertEqual(self.block("open"), 0, "a failed verification goes back to the code")
        self.assertEqual(self.block("verify"), 0)
        self.work("US1")
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
        self.work("US9")
        self.block("close")
        text = (self.root / block.HISTORY).read_bytes().decode("utf-8")
        rows = [line for line in text.split("\n") if line.startswith("| US9 |")]
        self.assertEqual(len(rows), 1)
        self.assertLess(text.index("| US9 |"), text.index("## Later"))
        self.assertNotIn("\r", text)


class Method(BlockCase):
    """The close refuses a work file that lacks a record of the method, and closes a complete one."""

    def test_a_complete_work_file_closes(self):
        self.assertEqual(self.closes("ok"), 0)
        self.assertEqual(block.load(self.root)["state"], block.CLOSED)

    def test_no_work_file_refuses(self):
        self.block("open", "none")
        self.block("verify")
        self.assertEqual(self.block("close"), 1)

    def test_no_file_read_whole_refuses(self):
        self.assertEqual(self.closes("r", Read="- tools/x.py: 1-20 of 40\n"), 1)

    def test_code_changed_asks_context7_and_the_internet(self):
        self.assertEqual(self.closes("c7", Research="- Internet: https://example.org\n"), 1)
        self.block("open")
        self.block("verify")
        self.work("c7", Research="- Context7 /python/cpython: q, a\n")
        self.assertEqual(self.block("close"), 1, "no source on the internet")

    def test_no_code_changed_asks_no_research(self):
        self.assertEqual(self.closes("docs", code=False, Research=None), 0)

    def test_skills_named(self):
        self.assertEqual(self.closes("s", Skills="- city-method\n"), 1)

    def test_a_failure_asks_its_cause_and_systematic_debugging(self):
        self.assertEqual(self.closes("f", Checks="- the scene: FAIL, 3 s, cause: the key was missing\n"), 1,
                         "systematic-debugging not named")
        self.block("open")
        self.block("verify")
        self.work("f", Checks="- the scene: FAIL, 3 s\n- the scene: PASS\n",
                  Skills="- city-method; verification-before-completion; systematic-debugging\n")
        self.assertEqual(self.block("close"), 1, "a failure without its cause")
        self.block("open")
        self.block("verify")
        self.work("f", Checks="- the scene: FAIL, 3 s, cause: the key was missing\n- the scene: PASS\n",
                  Skills="- city-method; verification-before-completion; systematic-debugging\n")
        self.assertEqual(self.block("close"), 0)


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
