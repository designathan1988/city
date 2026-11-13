"""The 24 km city measured on two builds, one after the other, and judged against the median profile's budgets and against each
other: python tools/tasks/km24.py [--previous REV] [--moves still,cross].

The previous build is a clean copy of REV's project/ (git archive: the versioned DLLs come with it; REV defaults to HEAD, the
build before the change in the working tree), imported, under dist/km24/previous; this build is the working tree. Each run is
the measuring scene of tools/tasks/km24.json (tests/perf/camera_pan.gd on km24) at its resolution, hidden, on the median
profile's GPU and processors, with a deadline, its log under dist/km24. Every movement's frame p90, frames over 33 ms, GPU p90,
main-thread core p90 and video memory are printed for both builds beside the budget. Exit 1 when a run fails or misses a
movement, or when this build breaks a budget the previous build kept on that movement; a budget both builds break is printed
and left to the register. Refused while a block is open (tools/tasks/block.py). Standard library and tools/tasks only."""
from __future__ import annotations

import argparse
import io
import json
import re
import shutil
import subprocess
import sys
import tarfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))  # tools: the tasks package

from tasks import block, locations, proc  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
CONFIG = Path(__file__).with_suffix(".json")
OUT = Path("dist/km24")
FRAMES = re.compile(r"^(\w+): (\d+) frames, p50 [\d.]+ ms, p90 ([\d.]+) ms.*, >33 ms: (\d+)")
STATS = re.compile(r"^  gpu mean [\d.]+ ms, p90 ([\d.]+) ms.* \| core mean [\d.]+ ms, p90 ([\d.]+) ms")
MEMORY = re.compile(r"^  memory: static \d+ MiB, video (\d+) MiB")
# What a budget reads of a movement, and whether a larger number is the worse one (all of them are).
MEASURES = (("frame_p90_ms", "frame p90 ms"), ("over_33_ms", "frames over 33 ms"), ("gpu_p90_ms", "GPU p90 ms"),
            ("core_p90_ms", "core p90 ms"), ("video_mb", "video MiB"))


def parse(text: str) -> dict:
    """{movement: {measure: number}} of a camera_pan log: its report lines, in the order the scene prints them."""
    moves, current = {}, None
    for line in text.splitlines():
        m = FRAMES.match(line)
        if m:
            current = m.group(1)
            moves[current] = {"frames": int(m.group(2)), "frame_p90_ms": float(m.group(3)), "over_33_ms": int(m.group(4))}
            continue
        if current is None:
            continue
        m = STATS.match(line)
        if m:
            moves[current].update(gpu_p90_ms=float(m.group(1)), core_p90_ms=float(m.group(2)))
        m = MEMORY.match(line)
        if m:
            moves[current]["video_mb"] = int(m.group(1))
    return moves


def judge(previous: dict, current: dict, budgets: dict) -> tuple[list[str], list[str]]:
    """(the lines of the report, the failures): each movement of either run, each measure beside its budget; a failure is a
    movement this build missed, or a budget this build breaks where the previous build kept it."""
    lines, failures = [], []
    for move in sorted(set(previous) | set(current)):
        now, before = current.get(move), previous.get(move)
        if now is None:
            failures.append(f"{move}: not measured on this build")
            continue
        for key, shown in MEASURES:
            limit = budgets[key]
            value, old = now.get(key), (before or {}).get(key)
            if value is None:
                failures.append(f"{move}: {shown} missing from this build's log")
                continue
            breaks, kept = value > limit, old is not None and old <= limit
            mark = "over the budget" + (" (the previous build kept it)" if kept else " (the previous build too)") if breaks else "within"
            lines.append(f"{move:10} {shown:18} this {value:>9} | previous {old if old is not None else '-':>9} | budget {limit:>8} "
                         f"| {mark}")
            if breaks and kept:
                failures.append(f"{move}: {shown} {value} over {limit}, where the previous build had {old}")
    return lines, failures


def previous_copy(rev: str, godot: str, cfg: dict) -> tuple[Path | None, str]:
    """(the previous build's project folder, what went wrong): REV's project/ extracted fresh and imported."""
    where = ROOT / OUT / "previous"
    if where.exists():
        shutil.rmtree(where)  # an absolute path under dist/km24, this tool's own folder
    where.mkdir(parents=True)
    archive = subprocess.run(["git", "-C", str(ROOT), "archive", "--format=tar", rev, "project"], capture_output=True, timeout=600)
    if archive.returncode != 0:
        return None, f"git archive {rev}: {archive.stderr.decode('utf-8', 'replace').strip()}"
    with tarfile.open(fileobj=io.BytesIO(archive.stdout)) as tar:
        tar.extractall(where, filter="data")
    project = where / "project"
    code, late = proc.run([godot, "--headless", "--path", str(project), "--import"], cfg["import_timeout_s"],
                          log=ROOT / OUT / "previous_import.log", hidden=True)
    if late or code != 0:
        return None, f"import of {rev}: {'past its deadline' if late else f'exit {code}'} (dist/km24/previous_import.log)"
    return project, ""


def measure(godot: str, project: Path, name: str, cfg: dict, moves: str) -> tuple[dict, str]:
    """({movement: measures}, what went wrong) of one build's run."""
    profile = cfg["profile"]
    argv = [godot, "--path", str(project), "--gpu-index", str(profile["gpu_index"]), "--resolution", cfg["resolution"],
            cfg["scene"], "--", "--no-flow", f"--map={cfg['map']}"] + ([f"--moves={moves}"] if moves else [])
    log = ROOT / OUT / f"{name}.log"
    code, late = proc.run(argv, cfg["run_timeout_s"], log=log, hidden=True, cores=proc.usable_cores()[:profile["cores"]])
    text = log.read_text(encoding="utf-8", errors="replace") if log.is_file() else ""
    if late or code != 0:
        return parse(text), f"{name}: {'past its deadline' if late else f'exit {code}'} ({OUT}/{name}.log)"
    return parse(text), ""


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(prog="km24.py", description=__doc__.split("\n\n", 1)[0])
    ap.add_argument("--previous", default="HEAD", help="the commit of the previous build (default HEAD)")
    ap.add_argument("--moves", default="", help="only these movements of the scene, comma separated")
    a = ap.parse_args(argv)
    why = block.refusal("km24", ROOT)
    if why:
        print(why, file=sys.stderr)
        return 1
    cfg = json.loads(CONFIG.read_text(encoding="utf-8"))
    godot = locations.locate("godot")
    (ROOT / OUT).mkdir(parents=True, exist_ok=True)
    project, wrong = previous_copy(a.previous, godot, cfg)
    if project is None:
        print(f"km24: {wrong}", file=sys.stderr)
        return 1
    before, wrong_before = measure(godot, project, "previous", cfg, a.moves)  # one engine at a time: the previous build first
    now, wrong_now = measure(godot, ROOT / "project", "this", cfg, a.moves)
    lines, failures = judge(before, now, cfg["budgets"])
    print(f"km24 on the {cfg['profile']['name']} profile: this build (the working tree) and the previous ({a.previous})")
    for line in lines:
        print("  " + line)
    failures = [w for w in (wrong_before, wrong_now) if w] + failures
    for f in failures:
        print(f"FAIL {f}")
    print(f"km24 {'FAILED' if failures else 'OK'}: {len(now)} movement(s) measured on this build")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
