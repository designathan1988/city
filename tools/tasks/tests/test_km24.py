# The km24 measurement (tools/tasks/km24.py): a camera_pan log read into its movements; the judgement of two builds against
# the budgets (a budget this build breaks where the previous kept it fails, one both break does not, a movement this build
# misses fails); the previous build kept by its commit (extracted and imported once, the copies of other commits removed, no
# mark after a failed import); each measurement a task of its own, never from the cache, on the GPU alone; a failed first
# measurement never skips the second. No Godot run and no git call: the logs are written here and the pool and git are mocked.
import contextlib
import io
import sys
import tarfile
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools: the tasks package

from tasks import km24  # noqa: E402
from tasks.task import FAIL, PASS, Result  # noqa: E402

BUDGETS = {"frame_p90_ms": 16.7, "over_33_ms": 0, "gpu_p90_ms": 13.0, "core_p90_ms": 8.0, "video_mb": 4096}
CFG = {"import_timeout_s": 900, "run_timeout_s": 1500, "resolution": "1920x1080", "scene": "res://tests/perf/camera_pan.tscn",
       "map": 4, "profile": {"name": "median", "gpu_index": 0, "cores": 12}, "budgets": BUDGETS}
COMMIT = "a" * 40
OTHER = "b" * 40


def log(frame_p90: float, over: int, gpu_p90: float, core_p90: float, video: int, name: str = "still") -> str:
    return (f"city: {{}}\n{name}: 600 frames, p50 16.6 ms, p90 {frame_p90} ms, p95 17.0 ms, p99 20.0 ms, >33 ms: {over}, >50 ms: 0\n"
            f"  worst: [\"21\"]\n"
            f"  gpu mean 11.0 ms, p90 {gpu_p90} ms, p95 12.5 ms, max 12.8 ms | render cpu mean 1.0 ms, p90 1.2 ms, p95 1.3 ms, "
            f"max 2.0 ms | core mean 1.0 ms, p90 {core_p90} ms, p95 2.1 ms, max 3.0 ms\n"
            f"  memory: static 668 MiB, video {video} MiB, buffers 900 MiB | chunks to draw: 0\n")


def tar_of(files: dict) -> bytes:
    buf = io.BytesIO()
    with tarfile.open(fileobj=buf, mode="w") as tar:
        for name, text in files.items():
            data = text.encode("utf-8")
            info = tarfile.TarInfo(name)
            info.size = len(data)
            tar.addfile(info, io.BytesIO(data))
    return buf.getvalue()


def result(name: str, status: str, log_path: str = "") -> Result:
    return Result(name, status, 0 if status == PASS else 1, 1.0, log_path, "key")


class ParseCase(unittest.TestCase):
    def test_a_movement_read_whole(self):
        moves = km24.parse(log(16.8, 1, 12.4, 1.9, 2501))
        self.assertEqual({"still": {"frames": 600, "frame_p90_ms": 16.8, "over_33_ms": 1, "gpu_p90_ms": 12.4, "core_p90_ms": 1.9,
                                    "video_mb": 2501}}, moves)

    def test_two_movements(self):
        moves = km24.parse(log(16.7, 0, 12.0, 1.0, 2000) + log(20.4, 119, 14.9, 3.8, 2664, "cross"))
        self.assertEqual(["cross", "still"], sorted(moves))
        self.assertEqual(119, moves["cross"]["over_33_ms"])

    def test_nothing_read_from_a_log_without_reports(self):
        self.assertEqual({}, km24.parse("ERROR: boot failed\n"))


class JudgeCase(unittest.TestCase):
    def test_a_budget_broken_where_the_previous_build_kept_it_fails(self):
        _lines, failures = km24.judge(km24.parse(log(16.5, 0, 12.0, 1.0, 2000)), km24.parse(log(18.0, 0, 12.0, 1.0, 2000)), BUDGETS)
        self.assertEqual(1, len(failures))
        self.assertIn("frame p90", failures[0])

    def test_a_budget_both_builds_break_is_reported_not_failed(self):
        lines, failures = km24.judge(km24.parse(log(20.0, 5, 14.0, 9.0, 2000)), km24.parse(log(19.0, 4, 14.5, 9.5, 2000)), BUDGETS)
        self.assertEqual([], failures)
        self.assertEqual(4, sum("the previous build too" in line for line in lines))

    def test_a_movement_missing_on_this_build_fails(self):
        _lines, failures = km24.judge(km24.parse(log(16.0, 0, 12.0, 1.0, 2000)), {}, BUDGETS)
        self.assertEqual(["still: not measured on this build"], failures)

    def test_a_measure_missing_from_this_log_fails(self):
        current = km24.parse(log(16.0, 0, 12.0, 1.0, 2000))
        del current["still"]["gpu_p90_ms"]
        _lines, failures = km24.judge(km24.parse(log(16.0, 0, 12.0, 1.0, 2000)), current, BUDGETS)
        self.assertEqual(["still: GPU p90 ms missing from this build's log"], failures)

    def test_within_every_budget_passes(self):
        lines, failures = km24.judge({}, km24.parse(log(16.0, 0, 12.0, 1.0, 2000)), BUDGETS)
        self.assertEqual([], failures)
        self.assertTrue(all(line.endswith("within") for line in lines))


class Mocked(unittest.TestCase):
    """A temporary folder for dist/km24/previous and the logs; git and the pool mocked."""

    def setUp(self):
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        self.tmp = Path(tmp.name)
        self.previous = self.tmp / "previous"
        self.addCleanup(mock.patch.stopall)
        mock.patch.object(km24, "PREVIOUS", self.previous).start()
        self.git = mock.patch.object(km24.inputs, "git", return_value=COMMIT + "\n").start()
        self.archive = mock.patch.object(km24.inputs, "git_bytes", return_value=tar_of({"project/project.godot": "x"})).start()
        self.run = mock.patch.object(km24, "run_one", return_value=result("km24_import_previous", PASS)).start()

    def written(self, name: str, text: str) -> str:
        path = self.tmp / name
        path.write_text(text, encoding="utf-8")
        return str(path)


class PreviousCopyCase(Mocked):
    def test_a_new_commit_is_extracted_imported_and_marked(self):
        project, commit, wrong = km24.previous_copy("HEAD", "godot", CFG)
        self.assertEqual(("", COMMIT), (wrong, commit))
        self.assertEqual(self.previous / COMMIT / "project", project)
        self.assertTrue((project / "project.godot").is_file())
        self.assertTrue((self.previous / COMMIT / km24.IMPORTED).is_file())
        self.git.assert_called_once_with(km24.ROOT, "rev-parse", "--verify", "HEAD^{commit}")
        self.archive.assert_called_once_with(km24.ROOT, "archive", "--format=tar", COMMIT, "project")
        task = self.run.call_args.args[0]
        self.assertEqual(["--path", str(project), "--import"], task.argv[-3:])
        self.assertTrue(task.hidden)
        self.assertEqual(900, task.timeout_s)
        self.assertFalse(self.run.call_args.kwargs["use_cache"])

    def test_an_imported_commit_is_neither_extracted_nor_imported_again(self):
        km24.previous_copy("HEAD", "godot", CFG)
        project, _commit, wrong = km24.previous_copy("HEAD", "godot", CFG)
        self.assertEqual("", wrong)
        self.assertEqual(self.previous / COMMIT / "project", project)
        self.assertEqual(1, self.archive.call_count)
        self.assertEqual(1, self.run.call_count)

    def test_a_failed_import_leaves_no_mark_and_the_next_run_starts_again(self):
        self.run.return_value = result("km24_import_previous", FAIL)
        project, _commit, wrong = km24.previous_copy("HEAD", "godot", CFG)
        self.assertIsNone(project)
        self.assertIn("import of HEAD", wrong)
        self.assertFalse((self.previous / COMMIT / km24.IMPORTED).exists())
        km24.previous_copy("HEAD", "godot", CFG)
        self.assertEqual(2, self.archive.call_count)

    def test_the_copies_of_other_commits_are_removed(self):
        (self.previous / OTHER / "project").mkdir(parents=True)
        km24.previous_copy("HEAD", "godot", CFG)
        self.assertEqual([COMMIT], [p.name for p in self.previous.iterdir()])

    def test_a_rev_that_names_no_commit_fails_before_anything_is_written(self):
        self.git.side_effect = RuntimeError("git rev-parse failed (128): unknown revision")
        project, _commit, wrong = km24.previous_copy("nothing", "godot", CFG)
        self.assertIsNone(project)
        self.assertIn("nothing names no commit", wrong)
        self.archive.assert_not_called()
        self.assertFalse(self.previous.exists())


class MeasureCase(Mocked):
    def test_a_measurement_is_never_cached_and_takes_the_gpu_alone(self):
        self.run.return_value = result("km24_this", PASS, self.written("this.log", log(16.0, 0, 12.0, 1.0, 2000)))
        moves, wrong = km24.measure("godot", Path("project"), "this", CFG, "still", (["project/**"], []))
        self.assertEqual(("", ["still"]), (wrong, sorted(moves)))
        task = self.run.call_args.args[0]
        self.assertEqual("km24_this", task.name)
        self.assertEqual({"cores": 12, "locks": {"dll": "shared", "gpu": "exclusive"}}, task.resources)
        self.assertEqual((True, 1500, ["project/**"]), (task.hidden, task.timeout_s, task.inputs))
        self.assertIn("--moves=still", task.argv)
        self.assertFalse(self.run.call_args.kwargs["use_cache"])

    def test_a_failed_measurement_is_reported_and_its_log_still_read(self):
        self.run.return_value = result("km24_previous", FAIL, self.written("previous.log", log(16.0, 0, 12.0, 1.0, 2000)))
        moves, wrong = km24.measure("godot", Path("p"), "previous", CFG, "", ([], [COMMIT]))
        self.assertIn("previous: FAIL", wrong)
        self.assertEqual(["still"], sorted(moves))


class MainCase(Mocked):
    def test_a_failed_first_measurement_never_skips_the_second(self):
        mock.patch.object(km24.block, "refusal", return_value=None).start()
        mock.patch.object(km24.locations, "locate", return_value="godot").start()
        mock.patch.object(km24, "previous_copy", return_value=(Path("previous"), COMMIT, "")).start()
        self.run.side_effect = [result("km24_previous", FAIL, self.written("previous.log", "")),
                                result("km24_this", PASS, self.written("this.log", log(16.0, 0, 12.0, 1.0, 2000)))]
        with contextlib.redirect_stdout(io.StringIO()) as out:
            code = km24.main([])
        self.assertEqual(1, code)
        self.assertEqual(["km24_previous", "km24_this"], [c.args[0].name for c in self.run.call_args_list])
        self.assertIn("FAIL previous: FAIL", out.getvalue())


if __name__ == "__main__":
    unittest.main()
