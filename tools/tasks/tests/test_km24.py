# The km24 measurement's reading and verdict (tools/tasks/km24.py): a camera_pan log read into its movements, and the judgement
# of two builds against the budgets: a budget this build breaks where the previous kept it fails, one both break does not, a
# movement this build misses fails. No Godot run: the logs are written here.
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools: the tasks package

from tasks import km24  # noqa: E402

BUDGETS = {"frame_p90_ms": 16.7, "over_33_ms": 0, "gpu_p90_ms": 13.0, "core_p90_ms": 8.0, "video_mb": 4096}


def log(frame_p90: float, over: int, gpu_p90: float, core_p90: float, video: int, name: str = "still") -> str:
    return (f"city: {{}}\n{name}: 600 frames, p50 16.6 ms, p90 {frame_p90} ms, p95 17.0 ms, p99 20.0 ms, >33 ms: {over}, >50 ms: 0\n"
            f"  worst: [\"21\"]\n"
            f"  gpu mean 11.0 ms, p90 {gpu_p90} ms, p95 12.5 ms, max 12.8 ms | render cpu mean 1.0 ms, p90 1.2 ms, p95 1.3 ms, "
            f"max 2.0 ms | core mean 1.0 ms, p90 {core_p90} ms, p95 2.1 ms, max 3.0 ms\n"
            f"  memory: static 668 MiB, video {video} MiB, buffers 900 MiB | chunks to draw: 0\n")


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


if __name__ == "__main__":
    unittest.main()
