# cli: exit 2 for an empty selection, an unknown name or a task with nothing to do; 1 for a failure; 0 only when all pass;
# serve answers each JSON line as its task finishes.
import json
import subprocess
import sys
import threading
import unittest
from contextlib import redirect_stderr, redirect_stdout
from io import StringIO
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))  # support
sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools: the tasks package

from support import PY, TOOLS, RepoCase, proc  # noqa: E402

from tasks import cli  # noqa: E402

SERVE_TIMEOUT_S = 120
ANSWER_S = 60  # well under git's own deadline (inputs.GIT_TIMEOUT_S): a git hung on the pipe answers only past it


class CliTest(RepoCase):
    def setUp(self):
        super().setUp()
        self.suites = self.root / "suites.json"
        doc = {
            "vars": {}, "changed": {"include": ["**/*.txt"]}, "sets": {"text": ["*.txt"]}, "templates": {}, "groups": {},
            "tasks": {
                "ok": {"argv": ["{python}", "-c", "pass"], "timeout_s": 60, "inputs": ["@text"]},
                "bad": {"argv": ["{python}", "-c", "import sys; sys.exit(1)"], "timeout_s": 60},
                "on_changed": {"argv": ["{python}", "-c", "pass", "{changed_list:--file}"], "timeout_s": 60},
            },
            "suites": {"good": {"tasks": ["ok"]}, "mixed": {"tasks": ["ok", "bad"]}, "changed": {"tasks": ["on_changed"]}},
        }
        self.suites.write_text(json.dumps(doc), encoding="utf-8")

    def cli(self, *args) -> int:
        with redirect_stdout(StringIO()), redirect_stderr(StringIO()):
            return cli.main(["run", "--suites", str(self.suites), "--root", str(self.root), *args])

    def test_empty_selection_exits_2(self):
        self.assertEqual(self.cli(), 2)

    def test_unknown_name_exits_2(self):
        self.assertEqual(self.cli("nothing-of-that-name"), 2)

    def test_unknown_variant_exits_2(self):
        self.assertEqual(self.cli("good", "--frobnicate"), 2)

    def test_nothing_changed_is_an_empty_selection(self):
        self.assertEqual(self.cli("changed"), 2)
        (self.root / "new.txt").write_text("x\n", encoding="utf-8")
        from tasks.inputs import invalidate  # noqa: E402
        invalidate(self.root)
        self.assertEqual(self.cli("changed"), 0)

    def test_exit_codes_of_a_run(self):
        self.assertEqual(self.cli("good"), 0)
        self.assertEqual(self.cli("mixed"), 1)
        self.assertEqual(self.cli("bad"), 1)

    def test_serve_answers_each_line(self):
        lines = [json.dumps({"id": "a", "name": "ok", "argv": [PY, "-c", "pass"], "timeout_s": 60}),
                 json.dumps({"id": "b", "name": "fails", "argv": [PY, "-c", "import sys; sys.exit(1)"], "timeout_s": 60}),
                 json.dumps({"id": "c", "name": "no deadline", "argv": [PY, "-c", "pass"]})]
        r = subprocess.run([PY, str(TOOLS / "tasks" / "cli.py"), "serve", "--root", str(self.root)], input="\n".join(lines),
                           capture_output=True, text=True, timeout=SERVE_TIMEOUT_S, creationflags=proc.NO_WINDOW)
        self.assertEqual(r.returncode, 0, r.stderr)
        got = {d["id"]: d["status"] for d in map(json.loads, r.stdout.splitlines())}
        self.assertEqual(got, {"a": "PASS", "b": "FAIL", "c": "BROKEN"})

    def test_serve_answers_with_its_request_pipe_open(self):
        # One request and the pipe left open, as the converter keeps it: the answer comes with no other line. Its keying runs git,
        # which once inherited the pipe and hung at its start while serve waited on it for the next line.
        line = json.dumps({"id": "a", "name": "ok", "argv": [PY, "-c", "pass"], "timeout_s": 60, "inputs": ["a.txt"]})
        p = subprocess.Popen([PY, str(TOOLS / "tasks" / "cli.py"), "serve", "--root", str(self.root)], stdin=subprocess.PIPE,
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, creationflags=proc.NO_WINDOW)
        try:
            p.stdin.write(line + "\n")
            p.stdin.flush()
            got = []
            reader = threading.Thread(target=lambda: got.append(p.stdout.readline()), daemon=True)
            reader.start()
            reader.join(ANSWER_S)
            self.assertTrue(got and json.loads(got[0])["status"] == "PASS", f"no answer in {ANSWER_S} s with the pipe open")
        finally:
            p.stdin.close()
            p.wait(timeout=SERVE_TIMEOUT_S)


if __name__ == "__main__":
    unittest.main()
