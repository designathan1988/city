# The gates of one item of a block (block_item.py, edit_guard.py; the constitution 1.3.0): the item's code is opened only when its
# work file section records a file read whole, the tracer's command and research with Context7 and a source's URL; the guard refuses
# an edit of code unless the block is open and the session itself (its transcript) shows CLAUDE.md read and city-method loaded since
# the last compaction, Context7 asked, the internet searched and the tracer run since the item's preparation began, the file read to
# its last line, and systematic-debugging loaded after a failure; it refuses a commit unless verification-before-completion was loaded
# and a photo of the change opened; a failed check runs again only after a new cause (file:line), after two failures a new source too.
import datetime
import json
import subprocess
import sys
import unittest
from contextlib import redirect_stderr, redirect_stdout
from io import StringIO
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))  # support
sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools: the tasks package

from support import PY, RepoCase  # noqa: E402

from tasks import block, block_method, cli  # noqa: E402

READY = ("- Read: tools/x.py: 1-40 of 40, 10:00, what it does\n"
         "- Trace: python tools/tracer/trace.py --feature tools/x.py:3 --root tools\n"
         "- Research: Context7 /websites/python_library, a question, its answer; https://example.org/a-page, what it says\n")
GUARD = Path(__file__).resolve().parent.parent / "edit_guard.py"
LONG = 50  # lines of a file read in two parts


def stamp(minutes: int = 0) -> str:
    """A transcript's timestamp (UTC, ISO with Z), `minutes` from now."""
    t = datetime.datetime.now(datetime.timezone.utc) + datetime.timedelta(minutes=minutes)
    return t.strftime("%Y-%m-%dT%H:%M:%S.000Z")


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


class Guard(ItemCase):
    """The guard reads the session's transcript: what was really done, not what a record says."""

    def setUp(self):
        super().setUp()
        self.transcript = self.root / "session.jsonl"
        self.transcript.write_text("", encoding="utf-8")
        self.code = self.root / "native/src/a.cpp"
        self.code.parent.mkdir(parents=True)
        self.code.write_text("a\nb\nc\n", encoding="utf-8")

    def did(self, name: str, minutes: int = 0, **inp) -> None:
        """A tool call of the session, `minutes` from now."""
        entry = {"type": "assistant", "timestamp": stamp(minutes),
                 "message": {"content": [{"type": "tool_use", "name": name, "input": inp}]}}
        with open(self.transcript, "a", encoding="utf-8") as fh:
            fh.write(json.dumps(entry) + "\n")

    def compacted(self, minutes: int = 0) -> None:
        with open(self.transcript, "a", encoding="utf-8") as fh:
            fh.write(json.dumps({"type": "system", "subtype": "compact_boundary", "timestamp": stamp(minutes)}) + "\n")

    def guard(self, tool: str = "Edit", **inp) -> tuple[int, str]:
        doc = {"tool_name": tool, "tool_input": inp or {"file_path": str(self.code)}, "transcript_path": str(self.transcript)}
        done = subprocess.run([PY, str(GUARD), "--root", str(self.root)], input=json.dumps(doc), capture_output=True, text=True,
                              timeout=60)
        return done.returncode, done.stderr

    def prepared(self) -> None:
        """Everything the method asks before code, done in the session, then the item opened."""
        self.did("Read", -5, file_path=str(self.root / "CLAUDE.md"))
        self.did("Skill", -5, skill="city-method")
        self.did("mcp__plugin_context7_context7__query-docs", -4, libraryId="/x/y", query="q")
        self.did("WebSearch", -4, query="q")
        self.did("Bash", -4, command="python tools/tracer/trace.py --feature native/src/a.cpp:2 --root native/src")
        self.did("Read", -3, file_path=str(self.code))
        self.item("b", "P-1")
        self.cli("block", "open", "b", "--item", "P-1")

    def test_records_are_written_whenever(self):
        self.assertEqual(self.guard(file_path=str(self.root / "doc/work/b.md"))[0], 0)

    def test_no_code_edited_while_the_block_is_not_open(self):
        code, why = self.guard()
        self.assertEqual(code, 2)
        self.assertIn("block", why)

    def test_a_prepared_item_edits_its_code(self):
        self.prepared()
        self.assertEqual(self.guard(), (0, ""))

    def test_each_step_of_the_method_is_asked(self):
        steps = [("Read", "CLAUDE.md"), ("Skill", "city-method"), ("query-docs", "Context7"), ("WebSearch", "internet"),
                 ("Bash", "trace.py"), ("Read", "a.cpp")]
        for k, (tool, says) in enumerate(steps):
            with self.subTest(missing=says):
                self.prepared()
                lines = self.transcript.read_text(encoding="utf-8").splitlines()
                self.transcript.write_text("\n".join(lines[:k] + lines[k + 1:]) + "\n", encoding="utf-8")
                code, why = self.guard()
                self.assertEqual(code, 2)
                self.assertIn(says, why)
                self.cli("block", "verify")
                self.write("- Skills: city-method; verification-before-completion")
                (self.root / block.STATE).unlink()
                self.transcript.write_text("", encoding="utf-8")
                (self.root / block_method.WORK / "b.md").unlink()

    def test_a_compaction_asks_the_orders_again(self):
        self.prepared()
        self.compacted(-1)
        code, why = self.guard()
        self.assertEqual(code, 2)
        self.assertIn("CLAUDE.md", why)

    def test_a_file_read_in_part_is_not_read(self):
        self.prepared()
        long = self.root / "native/src/long.cpp"
        long.write_text("x\n" * LONG, encoding="utf-8")
        self.did("Read", -2, file_path=str(long), offset=1, limit=20)
        self.assertEqual(self.guard(file_path=str(long))[0], 2, "read to line 20 of 50")
        self.did("Read", -1, file_path=str(long), offset=21, limit=30)
        self.assertEqual(self.guard(file_path=str(long))[0], 0, "1-20 and 21-50: read to its last line")

    def test_the_tools_code_needs_no_trace(self):
        """The tracer reads the game's languages (C++, GDScript, shaders), not the tools' Python: the trace is the game code's."""
        self.prepared()
        lines = [x for x in self.transcript.read_text(encoding="utf-8").splitlines() if "trace.py" not in x]
        self.transcript.write_text("\n".join(lines) + "\n", encoding="utf-8")
        tool = self.root / "tools/tasks/a.py"
        tool.parent.mkdir(parents=True)
        tool.write_text("x = 1\n", encoding="utf-8")
        self.did("Read", -1, file_path=str(tool))
        self.assertEqual(self.guard(file_path=str(tool))[0], 0)
        self.assertEqual(self.guard()[0], 2, "the game's code still asks it")

    def test_a_new_file_needs_no_reading(self):
        self.prepared()
        self.assertEqual(self.guard("Write", file_path=str(self.root / "native/src/new.cpp"), content="x")[0], 0)

    def test_a_failure_asks_systematic_debugging(self):
        self.prepared()
        self.cli("block", "verify")
        block.record({"check": "FAIL"}, self.root)
        self.cli("block", "open")
        code, why = self.guard()
        self.assertEqual(code, 2)
        self.assertIn("systematic-debugging", why)
        self.did("Skill", 1, skill="superpowers:systematic-debugging")
        self.assertEqual(self.guard()[0], 0)

    def test_a_commit_asks_verification_and_a_photo(self):
        self.prepared()
        self.cli("block", "verify")
        commit = {"command": "git commit -m x"}
        self.assertEqual(self.guard("Bash", **commit)[0], 2)
        self.did("Skill", 1, skill="superpowers:verification-before-completion")
        code, why = self.guard("Bash", **commit)
        self.assertEqual(code, 2)
        self.assertIn("photo", why)
        self.did("Read", 1, file_path=str(self.root / "dist/captures/x/00-a.png"))
        self.assertEqual(self.guard("Bash", **commit)[0], 0)
        self.assertEqual(self.guard("Bash", command="git status")[0], 0, "only a commit is asked")


if __name__ == "__main__":
    unittest.main()
