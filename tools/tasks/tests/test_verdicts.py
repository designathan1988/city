# The Godot verdict: a scene passes on exit 0 with no failure line; a crash, a script or shader that does not compile, a FAIL
# line or a failure count fails it, whatever the exit code. Run: python -m unittest discover tools/tasks/tests
import shutil
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))

from tasks import verdicts  # noqa: E402
from tasks.task import FAIL, PASS, Task  # noqa: E402


class GodotVerdictTest(unittest.TestCase):
    def setUp(self):
        self.dir = Path(tempfile.mkdtemp(prefix="verdicts-test-"))
        self.addCleanup(shutil.rmtree, self.dir, True)

    def verdict(self, text: str, code: int = 0):
        log = self.dir / "run.log"
        log.write_text(text, encoding="utf-8")
        task = Task(name="scene", argv=["godot"], timeout_s=1)  # a scene task that declares no {loads}
        return verdicts.godot(task, str(log), code, self.dir)[0]

    def test_clean_run_passes(self):
        self.assertEqual(self.verdict("PASS a check\nFAILURES: 0\n"), PASS)

    def test_each_failure_line_fails(self):
        for line in ("Program crashed with signal 11", "SCRIPT ERROR: Parse Error: x", "SHADER ERROR: Redefinition of 'X'.",
                     "FAIL wall by day", "FAILURES: 2"):
            with self.subTest(line=line):
                self.assertEqual(self.verdict(f"some output\n{line}\n"), FAIL)

    def test_exit_code_fails_a_clean_log(self):
        self.assertEqual(self.verdict("FAILURES: 0\n", code=1), FAIL)


class MachinePathVerdictTest(unittest.TestCase):
    """A build's outputs hold no folder of the machine that built them: the repository's absolute root, in UTF-8 or UTF-16."""

    def setUp(self):
        self.dir = Path(tempfile.mkdtemp(prefix="verdicts-paths-"))
        self.addCleanup(shutil.rmtree, self.dir, True)
        (self.dir / "run.log").write_text("built\n", encoding="utf-8")
        self.task = Task(name="build", argv=["cmake"], timeout_s=1, outputs=["bin/core.dll"])
        (self.dir / "bin").mkdir()

    def judge(self, content: bytes, code: int = 0):
        (self.dir / "bin" / "core.dll").write_bytes(content)
        return verdicts.no_machine_path(self.task, str(self.dir / "run.log"), code, self.dir)

    def test_clean_output_passes(self):
        self.assertEqual(self.judge(b"\x00native/src/a.cpp\x00")[0], PASS)

    def test_root_in_utf8_fails(self):
        status, message = self.judge(b"\x00" + self.dir.as_posix().encode("utf-8") + b"/native/src/a.cpp\x00")
        self.assertEqual(status, FAIL)
        self.assertIn("bin/core.dll", message)

    def test_root_in_utf16_fails(self):
        self.assertEqual(self.judge(str(self.dir).encode("utf-16-le"))[0], FAIL)

    def test_missing_output_fails(self):
        (self.dir / "bin").rmdir()
        self.assertEqual(verdicts.no_machine_path(self.task, str(self.dir / "run.log"), 0, self.dir)[0], FAIL)

    def test_exit_code_fails(self):
        self.assertEqual(self.judge(b"clean", code=2)[0], FAIL)


if __name__ == "__main__":
    unittest.main()
