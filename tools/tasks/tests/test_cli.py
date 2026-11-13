# cli: exit 2 for an empty selection, an unknown name or a task with nothing to do; 1 for a failure; 0 only when all pass;
# a run prints each task's lines as they come; serve answers each JSON line as its task finishes, its stdout the answers alone.
import json
import subprocess
import sys
import threading
import time
import unittest
from contextlib import redirect_stderr, redirect_stdout
from io import StringIO
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))  # support
sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools: the tasks package

from support import PY, TOOLS, RepoCase, proc  # noqa: E402

from tasks import cli  # noqa: E402
from tasks.machine import Machine  # noqa: E402
from tasks.pool import Pool  # noqa: E402
from tasks.suites import Suites  # noqa: E402
from tasks.task import Task  # noqa: E402

SERVE_TIMEOUT_S = 120
STATUS_WAIT_S = 20  # for a started task's first line to reach its log
# One trivial task's answer, keying included (a git listing of a one-file repo): well under inputs.GIT_TIMEOUT_S (120 s),
# the deadline a blocked git is held to, so a git that blocks fails here and a git that answers passes with room.
OPEN_INPUT_ANSWER_S = 60


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

    def test_a_run_prints_each_line_of_a_task_as_it_comes_but_the_resource_trace(self):
        doc = json.loads(self.suites.read_text(encoding="utf-8"))
        doc["tasks"]["talks"] = {"argv": ["{python}", "-u", "-c", "print('Loading resource: res://x.gd'); print('working')"],
                                 "timeout_s": 60}
        self.suites.write_text(json.dumps(doc), encoding="utf-8")
        out = StringIO()
        with redirect_stdout(out), redirect_stderr(StringIO()):
            code = cli.main(["run", "--suites", str(self.suites), "--root", str(self.root), "talks"])
        said = out.getvalue()
        self.assertEqual(code, 0)
        self.assertIn("selecting talks ...", said)
        self.assertIn("1 task(s) selected in", said)
        self.assertIn("  [talks] working", said)
        self.assertNotIn("Loading resource", said, "Godot's resource trace stays in the log")

    def test_status_shows_a_running_task_with_its_time_and_what_it_last_said(self):
        pool = Pool(root=self.root, machine=Machine(self.root / "machine"), label="live")
        pool.submit(Task("slow", [PY, "-u", "-c", "import time; print('half way there', flush=True); time.sleep(4)"], timeout_s=60))
        try:
            said = ""
            end = time.monotonic() + STATUS_WAIT_S
            while "half way there" not in said and time.monotonic() < end:
                time.sleep(0.2)
                out = StringIO()
                with redirect_stdout(out):
                    code = cli.main(["status", "--root", str(self.root)])
                said = out.getvalue()
            self.assertEqual(code, 0)
            self.assertIn("(live: 0 done, 1 running)", said)
            self.assertRegex(said, r"RUNNING\s+slow\s+\d+ s, \d+ cores\s+\| half way there")
        finally:
            pool.wait_all()
            pool.close()

    def test_an_item_lays_its_own_resources_over_its_template(self):
        doc = json.loads(self.suites.read_text(encoding="utf-8"))
        doc["templates"] = {"scene": {"argv": ["{python}", "-c", "pass"],
                                      "resources": {"cores": 2, "memory_mb": 900, "locks": {"dll": "shared", "gpu": "shared"}}}}
        doc["groups"] = {"scenes": {"template": "scene", "items": [
            {"name": "plain", "timeout_s": 5}, {"name": "timed", "timeout_s": 5, "resources": {"locks": {"gpu": "exclusive"}}}]}}
        self.suites.write_text(json.dumps(doc), encoding="utf-8")
        s = Suites(self.suites, root=self.root)
        self.assertEqual(s.build("plain").resources, {"cores": 2, "memory_mb": 900, "locks": {"dll": "shared", "gpu": "shared"}})
        self.assertEqual(s.build("timed").resources, {"cores": 2, "memory_mb": 900, "locks": {"dll": "shared", "gpu": "exclusive"}})

    def test_serve_answers_each_line(self):
        lines = [json.dumps({"id": "a", "name": "ok", "argv": [PY, "-c", "pass"], "timeout_s": 60}),
                 json.dumps({"id": "b", "name": "fails", "argv": [PY, "-c", "import sys; sys.exit(1)"], "timeout_s": 60}),
                 json.dumps({"id": "c", "name": "no deadline", "argv": [PY, "-c", "pass"]})]
        r = subprocess.run([PY, str(TOOLS / "tasks" / "cli.py"), "serve", "--root", str(self.root)], input="\n".join(lines),
                           capture_output=True, text=True, timeout=SERVE_TIMEOUT_S, creationflags=proc.NO_WINDOW)
        self.assertEqual(r.returncode, 0, r.stderr)
        got = {d["id"]: d["status"] for d in map(json.loads, r.stdout.splitlines())}
        self.assertEqual(got, {"a": "PASS", "b": "FAIL", "c": "BROKEN"})

    def test_serve_answers_while_its_input_stays_open(self):
        """The converter keeps serve's input open and waits for each answer before it asks again. serve's own read of that
        input is then pending, and a git that inherited the pipe blocked in Git for Windows' start (winansi.c detect_msys_tty
        queries the name of a piped stdin, a query that waits for the pending read) until git's deadline: keying failed."""
        p = subprocess.Popen([PY, str(TOOLS / "tasks" / "cli.py"), "serve", "--root", str(self.root)], stdin=subprocess.PIPE,
                             stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True, creationflags=proc.NO_WINDOW)
        try:
            p.stdin.write(json.dumps({"id": "a", "name": "ok", "argv": [PY, "-c", "pass"], "timeout_s": 60}) + "\n")
            p.stdin.flush()
            answer: list[str] = []
            reader = threading.Thread(target=lambda: answer.append(p.stdout.readline()), daemon=True)
            reader.start()
            reader.join(OPEN_INPUT_ANSWER_S)
            self.assertTrue(answer, f"no answer in {OPEN_INPUT_ANSWER_S} s while serve's input stays open")
            self.assertEqual(json.loads(answer[0])["status"], "PASS", answer[0])
        finally:
            p.stdin.close()
            try:
                p.wait(SERVE_TIMEOUT_S)
            except subprocess.TimeoutExpired:
                p.kill()
                p.wait(SERVE_TIMEOUT_S)


if __name__ == "__main__":
    unittest.main()
