# The edit guard (edit_guard.py; the constitution 1.3.0, principle IV): it reads the session's transcript and refuses an edit of code
# unless the block is open and the session itself shows CLAUDE.md read and city-method loaded since the last compaction, Context7
# asked, the internet searched and (for the game's code) the tracer run since the last commit, the file read to its last line, and
# systematic-debugging loaded after a failure; it refuses a commit unless verification-before-completion was loaded and, when the
# commit carries the game's code, a photo of the change opened; it refuses a command that opens and verifies the block at once, and a
# build or check whose output is piped or sent to a file.
import datetime
import json
import subprocess
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))  # support, test_block_item
sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools: the tasks package

from support import PY  # noqa: E402
from test_block_item import ItemCase  # noqa: E402

from tasks import block, block_method  # noqa: E402

GUARD = Path(__file__).resolve().parent.parent / "edit_guard.py"
LONG = 50  # lines of a file read in two parts


def stamp(minutes: int = 0) -> str:
    """A transcript's timestamp (UTC, ISO with Z), `minutes` from now."""
    t = datetime.datetime.now(datetime.timezone.utc) + datetime.timedelta(minutes=minutes)
    return t.strftime("%Y-%m-%dT%H:%M:%S.000Z")


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

    def stage(self, path: Path) -> None:
        subprocess.run(["git", "-C", str(self.root), "add", str(path)], check=True, capture_output=True, timeout=60)

    def prepared(self) -> None:
        """Everything the method asks before code, done in the session after the last commit (the repo's own, made as the test
        began: the previous item's end), then the item opened."""
        self.did("Read", 1, file_path=str(self.root / "CLAUDE.md"))
        self.did("Skill", 1, skill="city-method")
        self.did("mcp__plugin_context7_context7__query-docs", 2, libraryId="/x/y", query="q")
        self.did("WebSearch", 2, query="q")
        self.did("Bash", 2, command="python tools/tracer/trace.py --feature native/src/a.cpp:2 --root native/src")
        self.did("Read", 3, file_path=str(self.code))
        self.item("b", "P-1")
        self.cli("block", "open", "b", "--item", "P-1")

    def test_research_before_the_last_commit_was_the_last_items(self):
        self.did("mcp__plugin_context7_context7__query-docs", -5, libraryId="/x/y", query="q")  # before the repo's commit
        self.did("Read", 1, file_path=str(self.root / "CLAUDE.md"))
        self.did("Skill", 1, skill="city-method")
        self.did("WebSearch", 2, query="q")
        self.did("Bash", 2, command="python tools/tracer/trace.py --feature native/src/a.cpp:2 --root native/src")
        self.did("Read", 3, file_path=str(self.code))
        self.item("b", "P-1")
        self.cli("block", "open", "b", "--item", "P-1")
        code, why = self.guard()
        self.assertEqual(code, 2)
        self.assertIn("Context7", why)

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
        self.compacted(4)
        code, why = self.guard()
        self.assertEqual(code, 2)
        self.assertIn("CLAUDE.md", why)

    def test_a_file_read_in_part_is_not_read(self):
        self.prepared()
        long = self.root / "native/src/long.cpp"
        long.write_text("x\n" * LONG, encoding="utf-8")
        self.did("Read", 4, file_path=str(long), offset=1, limit=20)
        self.assertEqual(self.guard(file_path=str(long))[0], 2, "read to line 20 of 50")
        self.did("Read", 5, file_path=str(long), offset=21, limit=30)
        self.assertEqual(self.guard(file_path=str(long))[0], 0, "1-20 and 21-50: read to its last line")

    def test_the_tools_code_needs_no_trace(self):
        """The tracer reads the game's languages (C++, GDScript, shaders), not the tools' Python: the trace is the game code's."""
        self.prepared()
        lines = [x for x in self.transcript.read_text(encoding="utf-8").splitlines() if "trace.py" not in x]
        self.transcript.write_text("\n".join(lines) + "\n", encoding="utf-8")
        tool = self.root / "tools/tasks/a.py"
        tool.parent.mkdir(parents=True)
        tool.write_text("x = 1\n", encoding="utf-8")
        self.did("Read", 4, file_path=str(tool))
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
        self.did("Skill", 6, skill="superpowers:systematic-debugging")
        self.assertEqual(self.guard()[0], 0)

    def test_no_open_and_verify_in_one_command(self):
        """Opening the code and verifying it in one breath writes nothing between: a run again in disguise."""
        code, why = self.guard("Bash", command="python tools/tasks/cli.py block open b && python tools/tasks/cli.py block verify && x")
        self.assertEqual(code, 2)
        self.assertIn("one command", why)
        self.assertEqual(self.guard("Bash", command="python tools/tasks/cli.py block verify")[0], 0)

    def test_no_run_with_its_output_held_back(self):
        """A build or a check shows its lines as it runs: never piped through a filter nor sent to a file."""
        for cmd in ("python tools/tasks/cli.py run shoot_test | grep FAIL", "python tools/tasks/cli.py run shoot_test > $TEMP/x.log 2>&1",
                    "tools/build/build.sh core-windows 2>&1 | tail -4"):
            with self.subTest(cmd=cmd):
                code, why = self.guard("Bash", command=cmd)
                self.assertEqual(code, 2)
                self.assertIn("output", why)
        self.assertEqual(self.guard("Bash", command="python tools/tasks/cli.py run shoot_test")[0], 0)

    def test_a_commit_of_the_tools_asks_no_photo(self):
        """The photo proves what the player sees: a commit of the tools alone asks the verification, not a photo."""
        self.prepared()
        self.cli("block", "verify")
        tool = self.root / "tools/tasks/a.py"
        tool.parent.mkdir(parents=True)
        tool.write_text("x = 1\n", encoding="utf-8")
        self.stage(tool)
        commit = {"command": "git commit -m x"}
        code, why = self.guard("Bash", **commit)
        self.assertEqual(code, 2)
        self.assertNotIn("photo", why)
        self.did("Skill", 6, skill="superpowers:verification-before-completion")
        self.assertEqual(self.guard("Bash", **commit)[0], 0)
        self.assertEqual(self.guard("Bash", command="git commit -am x")[0], 0, "nothing of the game tracked and modified")

    def test_a_commit_asks_verification_and_a_photo(self):
        self.prepared()
        self.cli("block", "verify")
        self.stage(self.code)
        commit = {"command": "git commit -m x"}
        self.assertEqual(self.guard("Bash", **commit)[0], 2)
        self.did("Skill", 6, skill="superpowers:verification-before-completion")
        code, why = self.guard("Bash", **commit)
        self.assertEqual(code, 2)
        self.assertIn("photo", why)
        self.did("Read", 6, file_path=str(self.root / "dist/captures/x/00-a.png"))
        self.assertEqual(self.guard("Bash", **commit)[0], 0)
        self.assertEqual(self.guard("Bash", command="git status")[0], 0, "only a commit is asked")


if __name__ == "__main__":
    unittest.main()
