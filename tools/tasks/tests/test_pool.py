# pool: a hung task does not block a sibling, a failure SKIPs its dependents only, results are cached by content (hit on
# unchanged inputs, miss on changed content), timeouts are never cached, outputs come back on a hit; a task's lines are heard
# as it writes them, a task that cannot start ends at its deadline, a store that fails leaves the verdict as it was.
import json
import sys
import threading
import time
import unittest
from contextlib import redirect_stderr
from io import StringIO
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parent))  # support
sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools: the tasks package

from support import RepoCase, py  # noqa: E402

from tasks import Task, key_of, memo, proc, run  # noqa: E402
from tasks.admission import LOCKS  # noqa: E402
from tasks.cache import Cache  # noqa: E402
from tasks.inputs import invalidate  # noqa: E402
from tasks.locks import FileLock  # noqa: E402
from tasks.machine import Machine  # noqa: E402
from tasks.runs import newest_run  # noqa: E402
from tasks.task import CACHED_PASS, FAIL, PASS, SKIPPED, TIMEOUT  # noqa: E402

HANG_S = 120


def stamped(seconds: float) -> list[str]:
    """A task that writes when it started and when it ended to $OUT.start and $OUT.end."""
    return py("import os, time\n"
              "open(os.environ['OUT'] + '.start', 'w').write(repr(time.time()))\n"
              f"time.sleep({seconds})\n"
              "open(os.environ['OUT'] + '.end', 'w').write(repr(time.time()))\n")


class PoolTest(RepoCase):
    def run_tasks(self, tasks, **kw):
        finished = []
        kw.setdefault("machine", Machine(self.root / "machine"))  # this test's own: no other run of the machine in its way
        results = run(tasks, root=self.root, on_result=lambda r: finished.append((r.name, time.monotonic())), **kw)
        return {r.name: r for r in results}, finished

    def span(self, name: str) -> tuple[float, float]:
        return float((self.root / f"{name}.start").read_text()), float((self.root / f"{name}.end").read_text())

    def together(self, a: str, b: str) -> bool:
        (a0, a1), (b0, b1) = self.span(a), self.span(b)
        return a0 < b1 and b0 < a1

    def stamping(self, name: str, seconds: float, **resources) -> Task:
        return Task(name, stamped(seconds), timeout_s=60, env={"OUT": str(self.root / name)}, resources=resources)

    def test_tasks_run_together_while_the_memory_they_take_fits(self):
        machine = Machine(self.root / "machine", memory=lambda: (1000, 1000))  # MiB available now, MiB in all
        results, _ = self.run_tasks([self.stamping("a", 1.5, memory_mb=300), self.stamping("b", 1.5, memory_mb=300)], machine=machine)
        self.assertEqual({r.status for r in results.values()}, {PASS})
        self.assertTrue(self.together("a", "b"), "two tasks whose memory fits waited for one another")

    def test_a_task_waits_for_the_memory_another_still_has_to_take(self):
        machine = Machine(self.root / "machine", memory=lambda: (1000, 1000))
        results, _ = self.run_tasks([self.stamping("big", 1.5, memory_mb=600), self.stamping("big2", 1.5, memory_mb=600)], machine=machine)
        self.assertEqual({r.status for r in results.values()}, {PASS})
        self.assertFalse(self.together("big", "big2"), "two tasks that do not fit in the machine's memory ran at once")

    def test_a_task_that_fits_goes_ahead_of_one_that_does_not_when_it_delays_nobody(self):
        """EASY backfilling: `big` does not fit beside `first` and waits for its end; `small` fits now and takes no more than
        what will be left when `big` starts, so it runs beside `first` instead of waiting behind `big`."""
        machine = Machine(self.root / "machine", memory=lambda: (1000, 1000))
        tasks = [self.stamping("first", 2.0, memory_mb=300), self.stamping("big", 0.5, memory_mb=800),
                 self.stamping("small", 0.5, memory_mb=40)]
        results, _ = self.run_tasks(tasks, machine=machine)
        self.assertEqual({r.status for r in results.values()}, {PASS})
        self.assertFalse(self.together("first", "big"), "the task that does not fit started beside the running one")
        self.assertTrue(self.together("first", "small"), "a task that fits and delays nobody waited behind one that does not fit")

    def test_a_task_that_would_delay_the_one_ahead_waits_its_turn(self):
        machine = Machine(self.root / "machine", memory=lambda: (1000, 1000))
        # `medium` fits beside `first`, but would still run when `big` can start; it becomes ready after `gate`, when `big`
        # already waits (a task whose key is still being worked out is ahead of nobody yet)
        medium = self.stamping("medium", 0.5, memory_mb=400)
        medium.after = ["gate"]
        tasks = [self.stamping("first", 3.0, memory_mb=300), self.stamping("big", 0.5, memory_mb=800),
                 self.stamping("gate", 0.8, memory_mb=10), medium]
        results, _ = self.run_tasks(tasks, machine=machine)
        self.assertEqual({r.status for r in results.values()}, {PASS})
        self.assertFalse(self.together("first", "medium"), "a task took the memory the one ahead of it was waiting for")

    def test_a_task_larger_than_the_machine_runs_alone_not_never(self):
        machine = Machine(self.root / "machine", memory=lambda: (500, 1000))
        results, _ = self.run_tasks([self.stamping("huge", 0.2, memory_mb=5000)], machine=machine)
        self.assertEqual(results["huge"].status, PASS)

    @unittest.skipUnless(sys.platform == "win32", "the job's peak memory is read on Windows")
    def test_the_memory_a_task_took_is_kept_for_the_next_run(self):
        hold = py("import time\nb = bytearray(160 * 1024 * 1024)\ntime.sleep(0.5)\n")
        self.run_tasks([Task("holds", hold, timeout_s=60)])
        kept = json.loads((newest_run(self.root) / "memory.json").read_text(encoding="utf-8"))
        self.assertGreaterEqual(kept["holds"], 150, "the peak memory of the task's processes was not recorded")

    def test_two_runs_never_share_a_processor(self):
        cores = proc.usable_cores()[:2]
        if len(cores) < 2:
            self.skipTest("needs two processors")
        machine = Machine(self.root / "machine", cores=cores)
        done = {}

        def one(name: str) -> None:
            done[name] = run([self.stamping(name, 1.5, cores=2)], root=self.root, machine=machine, label=name)[0].status
        runs = [threading.Thread(target=one, args=(n,)) for n in ("left", "right")]
        for t in runs:
            t.start()
        for t in runs:
            t.join()
        self.assertEqual(done, {"left": PASS, "right": PASS})
        self.assertFalse(self.together("left", "right"), "two runs ran on the same processors at once")

    def test_a_run_says_what_it_is_doing_while_it_runs(self):
        seen = []
        self.run_tasks([self.stamping("slow", 2.0)], on_progress=seen.append, progress_s=0.4)
        self.assertTrue(seen, "a run with a task of two seconds said nothing while it ran")
        last = seen[-1]
        self.assertEqual((last["done"], last["total"]), (0, 1))
        self.assertEqual([r["task"] for r in last["running"]], ["slow"])
        self.assertGreater(last["running"][0]["seconds"], 0.0)

    def test_a_task_says_its_lines_while_it_runs(self):
        talks = Task("talks", py("import time\nprint('first', flush=True)\ntime.sleep(2)\nprint('second', flush=True)\n"
                                 "print('last, with no end', end='', flush=True)\n"), timeout_s=60)
        heard = []
        results, finished = self.run_tasks([talks], on_line=lambda i, line: heard.append((i, line, time.monotonic())))
        self.assertEqual(results["talks"].status, PASS)
        self.assertEqual([(i, line) for i, line, _t in heard],
                         [("talks", "first"), ("talks", "second"), ("talks", "last, with no end")])
        ended = finished[0][1]
        self.assertLess(heard[0][2], ended - 1.2, "the first line came only when the task ended, not as it was written")
        self.assertLessEqual(heard[-1][2], ended, "a line came after its task's result")

    def test_a_ready_task_that_never_gets_its_lock_ends_at_its_deadline(self):
        lock = FileLock(self.root / LOCKS / "held.lock")
        self.assertTrue(lock.try_acquire(shared=False))  # held by another holder for the whole run
        try:
            t = Task("locked", py("pass"), timeout_s=1, resources={"locks": {"held": "exclusive"}})
            started = time.monotonic()
            results, _ = self.run_tasks([t])
        finally:
            lock.release()
        self.assertEqual(results["locked"].status, TIMEOUT)
        self.assertIn("never started within its deadline of 1 s: waited for lock held", results["locked"].message)
        self.assertLess(time.monotonic() - started, 30, "the wait for a lock had no deadline")

    def test_a_store_that_fails_keeps_the_verdict(self):
        t = Task("kept", py("pass"), timeout_s=60, inputs=["a.txt"])
        err = StringIO()
        with mock.patch.object(Cache, "store", side_effect=TimeoutError("the ledger was held")), redirect_stderr(err):
            results, _ = self.run_tasks([t])
        self.assertEqual(results["kept"].status, PASS, "a failed store turned the task's own verdict")
        self.assertIn("(not kept in the cache: TimeoutError: the ledger was held)", results["kept"].message)
        self.assertIn("kept: not kept in the cache", err.getvalue())

    def test_hung_task_does_not_block_a_sibling(self):
        hang = Task("hang", py(f"import time; time.sleep({HANG_S})"), timeout_s=3)
        quick = Task("quick", py("print('done')"), timeout_s=60)
        results, finished = self.run_tasks([hang, quick], jobs=2)
        self.assertEqual(results["hang"].status, TIMEOUT)
        self.assertEqual(results["quick"].status, PASS)
        order = [n for n, _ in finished]
        self.assertEqual(order, ["quick", "hang"], "the sibling waited for the hung task: it would end after its deadline")

    def test_a_task_runs_on_the_cores_it_was_granted(self):
        if len(proc.usable_cores()) < 4:
            self.skipTest("needs four processors to tell a share from the whole")
        count = ("import os, ctypes as C\n"
                 "if os.name == 'nt':\n"
                 "    k = C.WinDLL('kernel32'); k.GetCurrentProcess.restype = C.c_void_p\n"
                 "    k.GetProcessAffinityMask.argtypes = [C.c_void_p, C.POINTER(C.c_size_t), C.POINTER(C.c_size_t)]\n"
                 "    a, b = C.c_size_t(), C.c_size_t(); k.GetProcessAffinityMask(k.GetCurrentProcess(), C.byref(a), C.byref(b))\n"
                 "    n = bin(a.value).count('1')\n"
                 "else:\n"
                 "    n = len(os.sched_getaffinity(0))\n"
                 "open(os.environ['OUT'], 'w').write(str(n))\n")
        def task(name, after=()):
            return Task(name, py(count), timeout_s=60, env={"OUT": str(self.root / f"{name}.n")},
                        resources={"cores": 2}, after=list(after))
        # `first` starts while `second` still waits for it: it gets its two and leaves two for it; `second` starts with
        # nobody waiting and gets every core of the run.
        results, _ = self.run_tasks([task("first"), task("second", after=["first"])], jobs=4)
        self.assertEqual({n: r.status for n, r in results.items()}, {"first": PASS, "second": PASS})
        self.assertEqual((self.root / "first.n").read_text(), "2", "a task ran on more cores than the run gave it")
        self.assertEqual((self.root / "second.n").read_text(), "4", "cores nobody waits for did not go to the task")

    def test_failure_skips_dependents_only(self):
        tasks = [Task("bad", py("import sys; sys.exit(1)"), timeout_s=60),
                 Task("child", py("pass"), timeout_s=60, after=["bad"]),
                 Task("grandchild", py("pass"), timeout_s=60, after=["child"]),
                 Task("other", py("pass"), timeout_s=60)]
        results, _ = self.run_tasks(tasks)
        self.assertEqual(results["bad"].status, FAIL)
        self.assertEqual(results["child"].status, SKIPPED)
        self.assertEqual(results["grandchild"].status, SKIPPED)
        self.assertEqual(results["other"].status, PASS)

    def test_cache_hit_on_unchanged_inputs_and_miss_on_changed_content(self):
        (self.root / "b.txt").write_text("untracked\n", encoding="utf-8")

        def task():
            return Task("reads", py("print(open('a.txt').read() + open('b.txt').read())"), timeout_s=60,
                        inputs=["a.txt", "b.txt"])
        first, _ = self.run_tasks([task()])
        self.assertEqual(first["reads"].status, PASS)
        again, _ = self.run_tasks([task()])
        self.assertEqual(again["reads"].status, CACHED_PASS)
        self.assertEqual(again["reads"].key, first["reads"].key)
        (self.root / "a.txt").write_text("two\n", encoding="utf-8")  # a tracked file changed on disk
        changed, _ = self.run_tasks([task()])
        self.assertEqual(changed["reads"].status, PASS)
        self.assertNotEqual(changed["reads"].key, first["reads"].key)
        (self.root / "b.txt").write_text("other\n", encoding="utf-8")  # an untracked one
        untracked, _ = self.run_tasks([task()])
        self.assertEqual(untracked["reads"].status, PASS)
        self.assertNotEqual(untracked["reads"].key, changed["reads"].key)

    def test_full_run_reads_no_cache(self):
        t = Task("plain", py("pass"), timeout_s=60, inputs=["a.txt"])
        self.run_tasks([t])
        results, _ = self.run_tasks([t], use_cache=False)
        self.assertEqual(results["plain"].status, PASS)

    def test_timeout_is_not_cached(self):
        t = Task("slow", py(f"import time; time.sleep({HANG_S})"), timeout_s=1, inputs=["a.txt"])
        for _ in range(2):
            results, _ = self.run_tasks([t])
            self.assertEqual(results["slow"].status, TIMEOUT)

    def test_outputs_come_back_on_a_hit(self):
        out = self.root / "out" / "x.txt"
        t = Task("writes", py("import os; os.makedirs('out', exist_ok=True); open('out/x.txt', 'w').write('made')"),
                 timeout_s=60, inputs=["a.txt"], outputs=["out/x.txt"])
        self.run_tasks([t])
        out.unlink()
        results, _ = self.run_tasks([t])
        self.assertEqual(results["writes"].status, CACHED_PASS)
        self.assertEqual(out.read_text(), "made")

    def test_a_missing_declared_output_fails(self):
        t = Task("forgets", py("pass"), timeout_s=60, outputs=["never.txt"])
        results, _ = self.run_tasks([t])
        self.assertEqual(results["forgets"].status, FAIL)

    def test_malformed_sets_raise(self):
        with self.assertRaises(ValueError):
            run([Task("a", py("pass"), timeout_s=5, after=["missing"])], root=self.root)
        with self.assertRaises(ValueError):
            run([Task("a", py("pass"), timeout_s=5, after=["b"]), Task("b", py("pass"), timeout_s=5, after=["a"])],
                root=self.root)
        with self.assertRaises(ValueError):
            Task("no deadline", py("pass"), timeout_s=0)

    def test_memo_computes_once_per_key(self):
        calls = []
        key = key_of(["a.txt"], root=self.root)
        for _ in range(2):
            self.assertEqual(memo("tests", key, lambda: calls.append(1) or {"v": 1}, root=self.root), {"v": 1})
        self.assertEqual(len(calls), 1)
        (self.root / "a.txt").write_text("three\n", encoding="utf-8")
        invalidate(self.root)
        self.assertNotEqual(key_of(["a.txt"], root=self.root), key)


if __name__ == "__main__":
    unittest.main()
