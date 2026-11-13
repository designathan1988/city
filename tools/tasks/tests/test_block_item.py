# The gates of one item of a block (block_item.py; the constitution 1.3.0): the item's code is opened only when its work file section
# records a file read whole, the tracer's command and research with Context7 and a source's URL; a failed check runs again only after
# a new cause (file:line), after two failures a new source too. What the session really did is the edit guard's (test_edit_guard.py).
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

READY = ("- Read: tools/x.py: 1-40 of 40, 10:00, what it does\n"
         "- Trace: python tools/tracer/trace.py --feature tools/x.py:3 --root tools\n"
         "- Research: Context7 /websites/python_library, a question, its answer; https://example.org/a-page, what it says\n")


class ItemCase(RepoCase):
    def setUp(self):
        super().setUp()
        history = self.root / block.HISTORY
        history.parent.mkdir(parents=True)
        history.write_bytes(("# History\n\n" + block.TIMES_HEADER + "\n|---|---|---|---|---|---|\n").encode("utf-8"))

    def cli(self, *args) -> int:
        with redirect_stdout(StringIO()), redirect_stderr(StringIO()):
            return cli.main([*args, "--root", str(self.root)])

    def item(self, block_name: str, item: str, body: str = READY) -> None:
        path = self.root / block_method.WORK / f"{block_name}.md"
        path.parent.mkdir(parents=True, exist_ok=True)
        old = path.read_text(encoding="utf-8") if path.is_file() else f"# {block_name}: a goal\n\n"
        path.write_text(old + f"## Item 1: a problem ({item})\n{body}\n", encoding="utf-8")

    def write(self, line: str, block_name: str = "b") -> None:
        path = self.root / block_method.WORK / f"{block_name}.md"
        path.write_text(path.read_text(encoding="utf-8") + line + "\n", encoding="utf-8")


class Opening(ItemCase):
    def test_a_block_opens_only_for_an_item(self):
        self.assertEqual(self.cli("block", "open", "b"), 1)

    def test_each_record_is_asked(self):
        for line in READY.splitlines():
            with self.subTest(missing=line.split(":")[0]):
                self.item("b", "P-1", READY.replace(line + "\n", ""))
                self.assertEqual(self.cli("block", "open", "b", "--item", "P-1"), 1)
                (self.root / block_method.WORK / "b.md").unlink()
        self.item("b", "P-1")
        self.assertEqual(self.cli("block", "open", "b", "--item", "P-1"), 0)
        self.assertEqual(block.load(self.root)["item"], "P-1")

    def test_a_record_wraps_onto_its_indented_lines(self):
        wrapped = READY.replace("; https://example.org/a-page", ";\n  https://example.org/a-page")
        self.item("b", "P-1", wrapped)
        self.assertEqual(self.cli("block", "open", "b", "--item", "P-1"), 0, "the URL on the research line's continuation")

    def test_the_next_item_is_asked_its_records_too(self):
        self.item("b", "P-1")
        self.cli("block", "open", "b", "--item", "P-1")
        self.cli("block", "verify")
        self.assertEqual(self.cli("block", "open", "--item", "P-2"), 1, "P-2 has no section")
        self.assertEqual(self.cli("block", "open"), 0, "the same item's code again, after a failure")


class Rerun(ItemCase):
    def setUp(self):
        super().setUp()
        self.item("b", "P-1")
        self.cli("block", "open", "b", "--item", "P-1")
        self.cli("block", "verify")

    def test_a_failure_asks_a_cause_before_the_next_run(self):
        block.record({"check": "FAIL"}, self.root)
        self.assertIsNotNone(block.check_refusal(["check"], self.root))
        self.assertIsNone(block.check_refusal(["other"], self.root), "only the check that failed")
        self.write("- Cause: the key was read where it is never written")
        self.assertIsNotNone(block.check_refusal(["check"], self.root), "a cause names where it is")
        self.write("- Cause: segment_at returns `seg`, read as `id` at tools/x.py:12")
        self.assertIsNone(block.check_refusal(["check"], self.root))

    def test_a_second_failure_goes_back_to_research(self):
        block.record({"check": "FAIL"}, self.root)
        self.write("- Cause: one at tools/x.py:3")
        block.record({"check": "FAIL"}, self.root)
        self.write("- Cause: another at tools/x.py:9")
        self.assertIsNotNone(block.check_refusal(["check"], self.root), "a new cause alone is not enough")
        self.write("- Research: https://example.org/another-source, what it says")
        self.assertIsNone(block.check_refusal(["check"], self.root))

    def test_a_pass_forgets_the_failures(self):
        block.record({"check": "FAIL"}, self.root)
        block.record({"check": "PASS"}, self.root)
        self.assertEqual(block.load(self.root).get("fails"), {})

    def test_the_orchestrator_asks_before_it_runs(self):
        suites = self.root / "suites.json"
        suites.write_text(json.dumps({"vars": {}, "changed": {"include": ["**/*.txt"]}, "sets": {}, "templates": {}, "groups": {},
                                      "tasks": {"check": {"argv": ["{python}", "-c", "raise SystemExit(1)"], "timeout_s": 60}},
                                      "suites": {}}), encoding="utf-8")
        run = ["run", "check", "--suites", str(suites)]
        self.assertEqual(self.cli(*run), 1, "it fails, and is remembered")
        self.assertIn("check", block.load(self.root)["fails"])
        err = StringIO()
        with redirect_stdout(StringIO()), redirect_stderr(err):
            self.assertEqual(cli.main([*run, "--root", str(self.root)]), 1)
        self.assertIn("no new cause", err.getvalue())


if __name__ == "__main__":
    unittest.main()
