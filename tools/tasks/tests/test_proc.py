# proc: a deadline kills the whole tree (a grandchild included), and a tree whose process exits leaves nothing running.
import shutil
import sys
import tempfile
import time
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))  # support
sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools: the tasks package

from support import PY, proc, py  # noqa: E402

SPAWN_DEADLINE_S = 30  # how long the child may take to start its grandchild before the test gives up
SLEEP_S = 120  # far past any deadline here: only a kill ends these sleepers


def spawner(pid_file: Path, then_sleep: bool) -> list[str]:
    """A child that starts a sleeping grandchild, writes both process ids, then sleeps or exits."""
    return py(
        "import os, subprocess, sys, time\n"
        f"g = subprocess.Popen([{PY!r}, '-c', 'import time; time.sleep({SLEEP_S})'])\n"
        f"open({str(pid_file)!r}, 'w').write(f'{{os.getpid()}} {{g.pid}}')\n"
        + (f"time.sleep({SLEEP_S})\n" if then_sleep else "sys.exit(0)\n"))


def wait_for(path: Path) -> list[int]:
    end = time.monotonic() + SPAWN_DEADLINE_S
    while time.monotonic() < end:
        if path.is_file() and path.read_text().strip():
            return [int(x) for x in path.read_text().split()]
        time.sleep(0.05)
    raise AssertionError(f"{path} never written")


class ProcTest(unittest.TestCase):
    def setUp(self):
        self.dir = Path(tempfile.mkdtemp(prefix="tasks-proc-"))
        self.addCleanup(shutil.rmtree, self.dir, True)

    def test_timeout_kills_the_whole_tree(self):
        pids = self.dir / "pids.txt"
        p = proc.Process(spawner(pids, then_sleep=True), log=self.dir / "log.txt")
        try:
            child, grandchild = wait_for(pids)
            self.assertTrue(proc.alive(child) and proc.alive(grandchild))
            self.assertIsNone(p.wait(0.5), "the child should still be sleeping at its deadline")
        finally:
            p.close()
        self.assertFalse(proc.alive(child), "the child outlived the kill")
        self.assertFalse(proc.alive(grandchild), "the grandchild outlived the kill")

    def test_run_reports_the_timeout(self):
        t0 = time.monotonic()
        code, timed_out = proc.run(py(f"import time; time.sleep({SLEEP_S})"), 1.0, log=self.dir / "log.txt")
        self.assertTrue(timed_out)
        self.assertIsNone(code)
        self.assertLess(time.monotonic() - t0, SPAWN_DEADLINE_S)

    def test_exit_kills_what_the_process_left(self):
        pids = self.dir / "pids.txt"
        code, timed_out = proc.run(spawner(pids, then_sleep=False), SPAWN_DEADLINE_S, log=self.dir / "log.txt")
        self.assertEqual((code, timed_out), (0, False))
        child, grandchild = wait_for(pids)
        self.assertFalse(proc.alive(grandchild), "a grandchild left running after its parent exited")

    def test_exit_code_and_log(self):
        log = self.dir / "log.txt"
        code, timed_out = proc.run(py("import sys; print('hello'); sys.stderr.write('err\\n'); sys.exit(3)"), 30, log=log)
        self.assertEqual((code, timed_out), (3, False))
        text = log.read_text()
        self.assertIn("hello", text)
        self.assertIn("err", text)

    def test_missing_program_raises(self):
        with self.assertRaises(OSError):
            proc.run(["no-such-program-anywhere"], 5, log=self.dir / "log.txt")


if __name__ == "__main__":
    unittest.main()
