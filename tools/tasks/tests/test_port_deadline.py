# Every process the session starts ends by itself, in seconds, and none is left running (.claude/hooks/port_deadline.py, asked
# through the gate; tools/tasks/deadline.py, the run's own deadline): each command that must be refused and the one beside it
# that must pass; what still runs at a stop; a command Claude Code moved to the background; a run ended at its deadline.
import json
import subprocess
import sys
import time
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools: the tasks package

from support import RepoCase  # noqa: E402
from test_port_gate import DEADLINE, HOOKS, RUN, GateCase  # noqa: E402

import port_deadline  # noqa: E402
import port_looks  # noqa: E402

from tasks import deadline, proc  # noqa: E402

LONG = "python C:/scratch/long.py"
# (command, started in the background, refused in Bash, refused in PowerShell). The call's `timeout` is no deadline: Claude
# Code does not apply it to a background command of a local session.
DEADLINES = [
    (f"{RUN} tests", True, True, True),                             # a run with no deadline of its own
    (f"{RUN} tests {DEADLINE}", True, False, False),
    (f"{RUN} tests --seconds {port_deadline.MOST_S + 60}", True, True, True),   # a deadline of minutes is none
    ("tools/build/build.sh core-windows", True, True, True),
    (f"tools/build/build.sh core-windows --seconds {port_deadline.MOST_S}", True, False, False),
    ("python tools/tracer/port.py sheet zones-lots-growth", True, False, False),   # the tracer's own deadline (Watch)
    ("python tools/tracer/port.py sheet transit --seconds 5000", False, True, True),
    ("python tools/tracer/trace.py START --depth 3", False, True, True),          # trace.py has none unless told
    ("python tools/tracer/trace.py START --depth 3 --seconds 20", False, False, False),
    (LONG, True, True, True),                                       # anything sent to the background
    (f"timeout 60 {LONG}", True, False, True),                      # under coreutils `timeout`; PowerShell's is a wait
    (f"cd /c/x && timeout 60 {LONG}", True, False, True),
    (f"timeout {port_deadline.MOST_S + 60} {LONG}", True, True, True),
    ("python tools/tasks/cli.py block review", True, True, True),
    ("timeout 120 python tools/tasks/cli.py block review", True, False, True),
    (LONG, False, False, False),                                    # the foreground: named when it is moved, and at a stop
    ("git status", False, False, False),
]


class Deadlines(GateCase):
    def test_every_process_ends_by_itself_in_seconds(self):
        for command, background, bash, powershell in DEADLINES:
            for tool, refused in (("Bash", bash), ("PowerShell", powershell)):
                with self.subTest(tool=tool, command=command, background=background):
                    given = {"command": command, "run_in_background": background, "timeout": 5000}   # the call's: no deadline
                    self.assertEqual(self.refused(tool, given), refused)
        long = {"command": f"{RUN} tests --seconds 1800", "run_in_background": True}
        self.assertTrue(self.refused("Bash", long))
        port_looks.ORDER.write_text("{}", encoding="utf-8")
        self.assertFalse(self.refused("Bash", long), "longer on the owner's order")

    def test_nothing_is_left_running_with_no_deadline(self):
        going = [{"id": "b1", "type": "shell", "status": "running", "command": LONG},
                 {"id": "b2", "type": "shell", "status": "running", "command": f"timeout 60 {LONG}"},
                 {"id": "b3", "type": "shell", "status": "running", "command": f"cd /c/x && {RUN} near {DEADLINE}"},
                 {"id": "a1", "type": "subagent", "status": "running"}]
        self.assertEqual(port_deadline.stray(going), [f"b1: {LONG}"])
        self.assertIn("TaskStop", port_deadline.unended(going))
        self.assertEqual(port_deadline.unended(going[1:]), "")
        self.assertEqual(port_deadline.unended(None), "", "a stop with no word of its tasks")

    def test_a_command_moved_to_the_background_is_named_at_once(self):
        answer = {"stdout": "Command did not complete within its 120s timeout and was moved to the background", "taskId": "b9"}
        late = {"tool_name": "Bash", "tool_input": {"command": LONG}, "tool_response": answer}
        self.assertIn("TaskStop", port_deadline.moved(late))
        self.assertEqual(port_deadline.moved({**late, "tool_response": {"stdout": "done"}}), "")
        self.assertEqual(port_deadline.moved({**late, "tool_input": {"command": f"timeout 60 {LONG}"}}), "", "it ends itself")

    def test_the_stop_is_refused_while_a_command_runs_with_nothing_to_end_it(self):
        stop = {"hook_event_name": "Stop", "stop_hook_active": True,
                "background_tasks": [{"id": "b1", "type": "shell", "status": "running", "command": LONG}]}
        for tasks, held in ((stop["background_tasks"], True), ([], False)):
            with self.subTest(held=held):
                done = subprocess.run([sys.executable, str(HOOKS / "port_state.py"), "hook"], capture_output=True, timeout=30,
                                      input=json.dumps({**stop, "background_tasks": tasks}).encode("utf-8"))
                self.assertEqual(done.returncode, 0, done.stderr.decode("utf-8", "replace"))
                said = json.loads(done.stdout) if done.stdout.strip() else {}
                self.assertEqual(said.get("decision") == "block" and "b1" in said.get("reason", ""), held)


class Run(unittest.TestCase):
    """The run's own deadline (tools/tasks/deadline.py)."""

    def test_a_run_past_its_deadline_kills_its_trees_and_records_what_was_left(self):
        recorded, left = {}, []
        tree = proc.Process([sys.executable, "-c", "import time; time.sleep(60)"])
        self.addCleanup(tree.close)
        late = deadline.RunDeadline(30, ["a_test", "b_look"], recorded.update, left.append)

        class Done:
            name, status = "a_test", "PASS"
        late.done(Done)
        late.expire()
        self.assertEqual(recorded, {"a_test": "PASS", "b_look": "TIMEOUT"})
        self.assertEqual(left, [deadline.EXPIRED])
        self.assertIsNotNone(tree.wait(5), "the tree it started was killed")

    def test_a_run_that_ends_in_time_is_left_alone(self):
        left = []
        with deadline.RunDeadline(30, ["a_test"], dict().update, left.append):
            pass
        with deadline.RunDeadline(None, ["a_test"], dict().update, left.append) as none:
            self.assertIsNone(none.timer, "no deadline asked, none held")
        self.assertEqual(left, [])


class CommandLine(RepoCase):
    """`cli.py run --seconds N`, as a session starts it."""

    def test_a_run_is_ended_at_its_own_deadline_whatever_its_tasks_deadlines(self):
        suites = self.root / "suites.json"
        suites.write_text(json.dumps({
            "vars": {}, "changed": {"include": ["**/*.txt"]}, "sets": {}, "templates": {}, "groups": {},
            "tasks": {"sleeps": {"argv": ["{python}", "-c", "import time; time.sleep(120)"], "timeout_s": 200}},
            "suites": {"slow": {"tasks": ["sleeps"]}}}), encoding="utf-8")
        cli = Path(__file__).resolve().parent.parent / "cli.py"
        began = time.monotonic()
        done = subprocess.run([sys.executable, str(cli), "run", "--suites", str(suites), "--root", str(self.root), "sleeps",
                               "--seconds", "4"], capture_output=True, timeout=90)
        said = done.stderr.decode("utf-8", "replace")
        self.assertEqual(done.returncode, deadline.EXPIRED, said)
        self.assertLess(time.monotonic() - began, 60, "ended at the run's deadline, not at the task's 200 s")
        self.assertIn("passed its deadline of 4 s", said)
        self.assertIn("sleeps", said)


if __name__ == "__main__":
    unittest.main()
