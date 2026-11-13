# The Godot verdict: a scene passes on exit 0 with no failure line; a crash, a script or shader that does not compile, an
# engine error the task does not declare (params expected_errors), a FAIL line or a failure count fails it, whatever the exit
# code; a warning does not. suites.json declares those errors as a list (a task's params, a group item), the schema refuses
# a string there, and the list reaches the verdict. Run: python -m unittest discover tools/tasks/tests
import json
import shutil
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))

from tasks import verdicts  # noqa: E402
from tasks.suites import Suites  # noqa: E402
from tasks.task import FAIL, PASS, Task  # noqa: E402

# A scene that reported no failure of its own while the engine printed an error (Godot's form: the error, then where).
ENGINE_ERROR_LOG = 'some output\nERROR: Condition "x" is true.\n   at: f (core/x.cpp:1)\nFAILURES: 0\n'
# The error reference_test provokes on purpose (a .glb that does not exist is refused), as its log holds it.
MISSING_GLB = 'ERROR: Can\'t open file at path "user://tests/nothing_here.glb"'


class GodotVerdictTest(unittest.TestCase):
    def setUp(self):
        self.dir = Path(tempfile.mkdtemp(prefix="verdicts-test-"))
        self.addCleanup(shutil.rmtree, self.dir, True)

    def judge(self, text: str, code: int = 0, params: dict | None = None):
        log = self.dir / "run.log"
        log.write_text(text, encoding="utf-8")
        task = Task(name="scene", argv=["godot"], timeout_s=1, params=params or {})  # a scene task that declares no {loads}
        return verdicts.godot(task, str(log), code, self.dir)

    def verdict(self, text: str, code: int = 0, params: dict | None = None):
        return self.judge(text, code, params)[0]

    def test_clean_run_passes(self):
        self.assertEqual(self.verdict("PASS a check\nFAILURES: 0\n"), PASS)

    def test_each_failure_line_fails(self):
        for line in ("Program crashed with signal 11", "SCRIPT ERROR: Parse Error: x", "SHADER ERROR: Redefinition of 'X'.",
                     "FAIL wall by day", "FAILURES: 2"):
            with self.subTest(line=line):
                self.assertEqual(self.verdict(f"some output\n{line}\n"), FAIL)

    def test_exit_code_fails_a_clean_log(self):
        self.assertEqual(self.verdict("FAILURES: 0\n", code=1), FAIL)

    def test_an_engine_error_fails(self):
        status, message = self.judge(ENGINE_ERROR_LOG)
        self.assertEqual(status, FAIL)
        self.assertIn('ERROR: Condition "x" is true.', message)

    def test_a_colored_error_fails(self):
        self.assertEqual(self.verdict('\x1b[1;31mERROR:\x1b[0m Condition "x" is true.\nFAILURES: 0\n'), FAIL)

    def test_a_declared_error_passes(self):
        self.assertEqual(self.verdict(ENGINE_ERROR_LOG, params={"expected_errors": ["Condition \"x\""]}), PASS)

    def test_a_warning_passes(self):
        self.assertEqual(self.verdict("WARNING: ObjectDB instances leaked at exit.\nFAILURES: 0\n"), PASS)

    def test_a_declared_error_never_hides_another_failure(self):
        # only an engine error can be expected: a FAIL line the same pattern finds still fails the scene
        log = ENGINE_ERROR_LOG + 'FAIL Condition "x" was met\n'
        self.assertEqual(self.verdict(log, params={"expected_errors": ["Condition \"x\""]}), FAIL)

    def test_one_string_is_refused_as_the_expected_errors(self):
        # a string is no list: read character by character, each character would expect almost every error
        with self.assertRaises(TypeError):
            self.verdict(ENGINE_ERROR_LOG, params={"expected_errors": "Condition \"x\""})


class DeclaredErrorsTest(unittest.TestCase):
    """suites.json declares the engine errors a scene provokes on purpose: a list of patterns in a task's params or in a group
    item (added to its template's), held to its schema, kept a list on the way to the verdict."""

    def setUp(self):
        self.dir = Path(tempfile.mkdtemp(prefix="verdicts-suites-"))
        self.addCleanup(shutil.rmtree, self.dir, True)

    def suites(self, task_params: dict | None = None, item: dict | None = None) -> Suites:
        doc = {
            "vars": {}, "changed": {"include": ["**/*.txt"]}, "sets": {}, "suites": {},
            "templates": {"scene": {"argv": ["{python}", "-c", "pass"], "verdict": "godot",
                                    "params": {"expected_errors": ["^ERROR: every scene$"]}}},
            "groups": {"scenes": {"template": "scene", "items": [
                {"name": "plain", "timeout_s": 5}, {"name": "provokes", "timeout_s": 5, **(item or {})}]}},
            "tasks": {"own": {"argv": ["{python}", "-c", "pass"], "timeout_s": 5, "verdict": "godot",
                              "params": task_params or {"golden": "dist/x.txt"}}},
        }
        path = self.dir / "suites.json"
        path.write_text(json.dumps(doc), encoding="utf-8")
        return Suites(path, root=self.dir)

    def failures(self, text: str, expected: list) -> list[str]:
        log = self.dir / "run.log"
        log.write_text(text, encoding="utf-8")
        return verdicts.failure_lines(str(log), expected=expected)

    def test_a_task_keeps_its_expected_errors_a_list(self):
        s = self.suites(task_params={"expected_errors": ["^ERROR: x$"], "golden": "dist/x.txt"})
        self.assertEqual(s.build("own").params, {"expected_errors": ["^ERROR: x$"], "golden": "dist/x.txt"})

    def test_a_group_item_adds_its_expected_errors_to_its_template_as_a_list(self):
        s = self.suites(item={"expected_errors": ['^ERROR: Can\'t open "x"$']})
        self.assertEqual(s.build("provokes").params["expected_errors"], ["^ERROR: every scene$", '^ERROR: Can\'t open "x"$'])
        self.assertEqual(s.build("plain").params["expected_errors"], ["^ERROR: every scene$"])

    def test_a_declared_item_reaches_the_verdict(self):
        s = self.suites(item={"expected_errors": ['^ERROR: Can\'t open "x"$']})
        log = self.dir / "scene.log"
        log.write_text('ERROR: Can\'t open "x"\nFAILURES: 0\n', encoding="utf-8")
        self.assertEqual(verdicts.godot(s.build("provokes"), str(log), 0, self.dir)[0], PASS)
        self.assertEqual(verdicts.godot(s.build("plain"), str(log), 0, self.dir)[0], FAIL)

    def test_a_string_is_refused_by_the_schema(self):
        for case in ({"item": {"expected_errors": "^ERROR: x$"}}, {"task_params": {"expected_errors": "^ERROR: x$"}}):
            with self.subTest(case=case), self.assertRaisesRegex(ValueError, "expected_errors"):
                self.suites(**case)

    def test_other_params_stay_strings(self):
        with self.assertRaisesRegex(ValueError, "golden"):
            self.suites(task_params={"golden": ["dist/x.txt"]})

    def test_reference_test_declares_only_its_missing_file(self):
        _template, item = Suites(listing=True).items["reference_test"]  # the repository's suites, held to the schema
        self.assertEqual(self.failures(f"ok   the .glb was written\n{MISSING_GLB}\n", item["expected_errors"]), [])
        for other in ('ERROR: Can\'t open file at path "user://tests/a_real_model.glb"', MISSING_GLB + " twice"):
            with self.subTest(line=other):
                self.assertEqual(self.failures(f"{other}\n", item["expected_errors"]), [other])


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
