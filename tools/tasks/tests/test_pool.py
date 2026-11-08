# pool: a hung task does not block a sibling, a failure SKIPs its dependents only, results are cached by content (hit on
# unchanged inputs, miss on changed content), timeouts are never cached, outputs come back on a hit, two runs never write
# one output together.
import shutil
import sys
import tempfile
import threading
import time
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))  # support
sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools: the tasks package

from support import RepoCase, py  # noqa: E402

from tasks import Task, key_of, memo, run  # noqa: E402
from tasks.pool import Pool  # noqa: E402
from tasks.inputs import invalidate  # noqa: E402
from tasks.task import CACHED_PASS, FAIL, PASS, SKIPPED, TIMEOUT  # noqa: E402

HANG_S = 120
WRITE_GAP_S = 1.5  # between the two parts of a torn write: far longer than a scheduler's turn, so the runs overlap


class PoolTest(RepoCase):
    def run_tasks(self, tasks, **kw):
        finished = []
        results = run(tasks, root=self.root, on_result=lambda r: finished.append((r.name, time.monotonic())), **kw)
        return {r.name: r for r in results}, finished

    def test_hung_task_does_not_block_a_sibling(self):
        hang = Task("hang", py(f"import time; time.sleep({HANG_S})"), timeout_s=3)
        quick = Task("quick", py("print('done')"), timeout_s=60)
        results, finished = self.run_tasks([hang, quick], jobs=2)
        self.assertEqual(results["hang"].status, TIMEOUT)
        self.assertEqual(results["quick"].status, PASS)
        order = [n for n, _ in finished]
        self.assertEqual(order, ["quick", "hang"], "the sibling waited for the hung task: it would end after its deadline")

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

    def test_two_runs_never_write_one_output_together(self):
        # Two runs at once of a task that writes its output in two parts: one runs, the other waits for the output's lock
        # and takes the stored result; the cache holds the whole file, never the first part alone.
        out = self.root / "out" / "y.txt"
        code = ("import os, time; os.makedirs('out', exist_ok=True); f = open('out/y.txt', 'w'); f.write('first'); "
                f"f.flush(); time.sleep({WRITE_GAP_S}); f.write(' second'); f.close()")
        results = {}

        def one(k):
            results[k] = run([Task("torn", py(code), timeout_s=60, inputs=["a.txt"], outputs=["out/y.txt"])],
                             root=self.root)[0]
        threads = [threading.Thread(target=one, args=(k,)) for k in range(2)]
        for th in threads:
            th.start()
        for th in threads:
            th.join()
        self.assertEqual(sorted(r.status for r in results.values()), sorted([PASS, CACHED_PASS]))
        out.unlink()
        again = run([Task("torn", py(code), timeout_s=60, inputs=["a.txt"], outputs=["out/y.txt"])], root=self.root)[0]
        self.assertEqual(again.status, CACHED_PASS)
        self.assertEqual(out.read_text(), "first second")

    def test_a_run_outside_the_repository_keeps_its_snapshot(self):
        # A task run elsewhere with no output in the repository cannot change what the next keys read: the repository is listed
        # again only after the task run inside it.
        outside = Path(tempfile.mkdtemp(prefix="tasks-outside-"))
        try:
            pool = Pool(root=self.root)
            read_again = []
            invalidate = pool.snapshot.invalidate
            pool.snapshot.invalidate = lambda: (read_again.append(1), invalidate())
            pool.submit(Task("elsewhere", py("pass"), timeout_s=60, cwd=str(outside)))
            pool.submit(Task("here", py("pass"), timeout_s=60, after=["elsewhere"]))
            pool.wait_all()
            pool.close()
            self.assertEqual(len(read_again), 1, "only the run inside the repository has it read again")
        finally:
            shutil.rmtree(outside, ignore_errors=True)

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
