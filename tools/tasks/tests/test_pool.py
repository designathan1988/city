# pool: a hung task does not block a sibling, a failure SKIPs its dependents only, results are cached by content (hit on
# unchanged inputs, miss on changed content), timeouts are never cached, outputs come back on a hit.
import sys
import time
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))  # support
sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools: the tasks package

from support import RepoCase, py  # noqa: E402

from tasks import Task, key_of, memo, proc, run  # noqa: E402
from tasks.inputs import invalidate  # noqa: E402
from tasks.task import CACHED_PASS, FAIL, PASS, SKIPPED, TIMEOUT  # noqa: E402

HANG_S = 120


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
