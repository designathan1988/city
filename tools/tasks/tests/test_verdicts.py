# The Godot verdict: a scene passes on exit 0 with no failure line; a crash, a script or shader that does not compile, a FAIL
# line or a failure count fails it, whatever the exit code. Run: python -m unittest discover tools/tasks/tests
import shutil
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))

from tasks import verdicts  # noqa: E402
from tasks.task import FAIL, PASS  # noqa: E402


class GodotVerdictTest(unittest.TestCase):
    def setUp(self):
        self.dir = Path(tempfile.mkdtemp(prefix="verdicts-test-"))
        self.addCleanup(shutil.rmtree, self.dir, True)

    def verdict(self, text: str, code: int = 0):
        log = self.dir / "run.log"
        log.write_text(text, encoding="utf-8")
        return verdicts.godot(None, str(log), code, self.dir)[0]

    def test_clean_run_passes(self):
        self.assertEqual(self.verdict("PASS a check\nFAILURES: 0\n"), PASS)

    def test_each_failure_line_fails(self):
        for line in ("Program crashed with signal 11", "SCRIPT ERROR: Parse Error: x", "SHADER ERROR: Redefinition of 'X'.",
                     "FAIL wall by day", "FAILURES: 2"):
            with self.subTest(line=line):
                self.assertEqual(self.verdict(f"some output\n{line}\n"), FAIL)

    def test_exit_code_fails_a_clean_log(self):
        self.assertEqual(self.verdict("FAILURES: 0\n", code=1), FAIL)


if __name__ == "__main__":
    unittest.main()
