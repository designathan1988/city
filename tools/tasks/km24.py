"""The 24 km city measured on two builds, one after the other, and judged against the median profile's budgets and against each
other: python tools/tasks/km24.py [--previous REV] [--moves still,cross].

The previous build is a clean copy of REV's project/ (git archive: the versioned DLLs come with it; REV defaults to HEAD, the
build before the change in the working tree), kept by its commit under dist/km24/previous/<commit> and imported once (the
mark .imported beside it is written after a passed import), so an unchanged REV is neither extracted nor imported again; the
copies of other commits are removed. This build is the working tree. The import and each run are tasks of the orchestrator
(tools/tasks/pool.py), hidden, each with its deadline from tools/tasks/km24.json, their lines shown as they come and their logs
under dist/tasks; a run is the measuring scene (tests/perf/camera_pan.gd on km24) at its resolution, on the median profile's GPU
taken alone and its processors, never from the cache. The two runs are two separate runs of the pool, the previous build first:
a failed first never skips the second. Every movement's frame p90, frames over 33 ms, GPU p90, main-thread core p90 and video
memory are printed for both builds beside the budget. Exit 1 when a run fails or misses a movement, or when this build breaks
a budget the previous build kept on that movement; a budget both builds break is printed and left to the register. Refused
while a block is open (tools/tasks/block.py). Standard library and tools/tasks only."""
from __future__ import annotations

import argparse
import io
import json
import re
import shutil
import sys
import tarfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))  # tools: the tasks package

from tasks import block, inputs, locations, pool  # noqa: E402
from tasks.cli import print_line, print_progress  # noqa: E402
from tasks.task import PASSED, Result, Task  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
CONFIG = Path(__file__).with_suffix(".json")
PREVIOUS = ROOT / "dist" / "km24" / "previous"  # this tool's own folder: one copy per commit, the others removed
IMPORTED = ".imported"  # beside a copy's project/: written after its import passed
IMPORT_CORES = 2  # what Godot's import takes (the import task of tools/tasks/suites.json)
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


def show(r: Result) -> None:
    """One line per finished task, with the reason of one that did not pass."""
    why = f"  {r.message.splitlines()[0]}" if r.message and r.status not in PASSED else ""
    print(f"{r.status:<12} {r.name}  {r.seconds:.1f} s{why}", flush=True)


def run_one(task: Task, use_cache: bool) -> Result:
    """One task through the pool on its own: its deadline, its lines shown as they come, what the run is doing."""
    return pool.run([task], on_result=show, root=ROOT, use_cache=use_cache, label=task.name, on_line=print_line,
                    on_progress=print_progress)[0]


def failure(what: str, r: Result) -> str:
    return f"{what}: {r.status}{f' ({r.message.splitlines()[0]})' if r.message else ''}, log {r.log or 'none'}"


def keep_only(commit: str) -> Path:
    """The folder of that commit's copy; the copies of every other commit removed (absolute paths under PREVIOUS)."""
    if PREVIOUS.is_dir():
        for other in PREVIOUS.iterdir():
            if other.name == commit:
                continue
            if other.is_dir() and not other.is_symlink():
                shutil.rmtree(other)
            else:
                other.unlink()
    return PREVIOUS / commit


def previous_copy(rev: str, godot: str, cfg: dict) -> tuple[Path | None, str, str]:
    """(the previous build's project folder, its commit, what went wrong): REV's project/ extracted and imported, once per
    commit."""
    try:
        commit = inputs.git(ROOT, "rev-parse", "--verify", rev + "^{commit}").strip()
    except RuntimeError as e:
        return None, "", f"{rev} names no commit: {e}"
    where = keep_only(commit)
    project = where / "project"
    if (where / IMPORTED).is_file():
        return project, commit, ""
    if where.exists():
        shutil.rmtree(where)  # an extraction or an import that did not end: done again whole
    where.mkdir(parents=True)
    try:
        archive = inputs.git_bytes(ROOT, "archive", "--format=tar", commit, "project")
    except RuntimeError as e:
        return None, commit, str(e)
    with tarfile.open(fileobj=io.BytesIO(archive)) as tar:
        tar.extractall(where, filter="data")
    task = Task(name="km24_import_previous", argv=[godot, "--headless", "--path", str(project), "--import"],
                timeout_s=cfg["import_timeout_s"], extra=[commit, godot], hidden=True, resources={"cores": IMPORT_CORES})
    r = run_one(task, use_cache=False)  # the copy is new: its import is done here, whatever an earlier run left in the cache
    if not r.passed:
        return None, commit, failure(f"import of {rev} ({commit})", r)
    (where / IMPORTED).write_text(commit + "\n", encoding="utf-8")
    return project, commit, ""


def measure(godot: str, project: Path, name: str, cfg: dict, moves: str, key: tuple[list, list]) -> tuple[dict, str]:
    """({movement: measures}, what went wrong) of one build's run. `key`: (inputs, extra) that name the build measured."""
    profile = cfg["profile"]
    argv = [godot, "--path", str(project), "--gpu-index", str(profile["gpu_index"]), "--resolution", cfg["resolution"],
            cfg["scene"], "--", "--no-flow", f"--map={cfg['map']}"] + ([f"--moves={moves}"] if moves else [])
    task = Task(name=f"km24_{name}", argv=argv, timeout_s=cfg["run_timeout_s"], inputs=key[0], extra=key[1] + [godot],
                hidden=True, verdict="godot",
                resources={"cores": profile["cores"], "locks": {"dll": "shared", "gpu": "exclusive"}})
    r = run_one(task, use_cache=False)  # a measurement is of this machine now, never a stored one
    log = Path(r.log) if r.log else None
    text = log.read_text(encoding="utf-8", errors="replace") if log and log.is_file() else ""
    return parse(text), "" if r.passed else failure(name, r)


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
    project, commit, wrong = previous_copy(a.previous, godot, cfg)
    if project is None:
        print(f"km24: {wrong}", file=sys.stderr)
        return 1
    # one engine at a time: the previous build first, then this one, each a run of the pool of its own
    before, wrong_before = measure(godot, project, "previous", cfg, a.moves, ([], [commit]))
    now, wrong_now = measure(godot, ROOT / "project", "this", cfg, a.moves, (["project/**"], []))
    lines, failures = judge(before, now, cfg["budgets"])
    print(f"km24 on the {cfg['profile']['name']} profile: this build (the working tree) and the previous ({a.previous}, {commit})")
    for line in lines:
        print("  " + line)
    failures = [w for w in (wrong_before, wrong_now) if w] + failures
    for f in failures:
        print(f"FAIL {f}")
    print(f"km24 {'FAILED' if failures else 'OK'}: {len(now)} movement(s) measured on this build")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
