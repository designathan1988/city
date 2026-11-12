# runs: run folders past their bound (kept_runs, suites.json lifecycle) are pruned by whichever run holds the prune lock; runs that start together never trip
# over each other's pruning; each run writes its tasks' times in its own folder and reads every kept run's, newest winning.
import json
import shutil
import sys
import tempfile
import threading
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools: the tasks package

from tasks import runs  # noqa: E402

DEAD_PID = 999999999  # no process has this id: its runs are a dead process's
STARTS = 8  # runs started at once over the same folder
OLD_EXTRA = STARTS * 2  # old runs past what is kept, before the runs start


class RunsTest(unittest.TestCase):
    def setUp(self):
        self.root = Path(tempfile.mkdtemp(prefix="tasks-runs-"))
        self.addCleanup(shutil.rmtree, self.root, True)
        self.base = self.root / runs.RUNS
        self.base.mkdir(parents=True)

    def old_runs(self, count: int) -> list[Path]:
        made = []
        for k in range(count):
            d = self.base / f"20000101-0000{k:02d}-{DEAD_PID}-old"
            (d / "deep").mkdir(parents=True)
            (d / "deep" / "events.jsonl").write_text("x\n", encoding="utf-8")
            made.append(d)
        return made

    def test_prunes_past_kept(self):
        old = self.old_runs(runs.kept_runs() + 3)
        runs.RunRecord(self.root, "one").close()
        left = [p for p in self.base.iterdir() if p.is_dir()]
        self.assertEqual(len(left), runs.kept_runs())
        self.assertFalse(any(d.exists() for d in old[:4]), "the oldest go, the new run's own folder counted")

    def test_runs_started_together_prune_without_error(self):
        old = self.old_runs(runs.kept_runs() + OLD_EXTRA)
        errors: list[BaseException] = []
        gate = threading.Barrier(STARTS)

        def start(k: int) -> None:
            try:
                gate.wait()
                runs.RunRecord(self.root, f"together{k}").close()
            except BaseException as e:  # noqa: BLE001 - every failure of a run is the test's answer
                errors.append(e)
        threads = [threading.Thread(target=start, args=(k,)) for k in range(STARTS)]
        for t in threads:
            t.start()
        for t in threads:
            t.join()
        self.assertEqual(errors, [])
        self.assertFalse(any(d.exists() for d in old[:OLD_EXTRA]), "the run that pruned removed the oldest")

    def test_times_are_each_runs_own_and_the_newest_wins(self):
        first = runs.RunRecord(self.root, "first")
        first.took("a", 1.0)
        first.took("b", 2.0)
        first.close()
        second = runs.RunRecord(self.root, "second")
        self.assertEqual(second.times, {"a": 1.0, "b": 2.0}, "a run reads the kept runs' times")
        second.took("a", 5.0)
        second.close()
        self.assertEqual(json.loads((second.dir / runs.RUN_TIMES).read_text(encoding="utf-8")), {"a": 5.0},
                         "a run writes only its own tasks' times, in its own folder")
        third = runs.RunRecord(self.root, "third")
        self.assertEqual(third.times["a"], 5.0, "the newest run's time wins")
        third.close()


if __name__ == "__main__":
    unittest.main()
