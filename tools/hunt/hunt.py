"""The defect hunt, one command: `python tools/hunt/hunt.py` runs the task hunt of tools/tasks/suites.json (a verbose Godot run of
project/tests/hunt/hunt.tscn, hidden, with that task's deadline, after the debug DLL, the import and the scripts) through the
orchestrator in this process, each line of the run shown as it comes, and prints every defect the core's probes found in the
generated town, most frequent first, each with how many, where, why and the code that owns its mechanism; the engine's own
errors in the run's log are defects too. The full list, with the first cases of each class whole, is dist/hunt/hunt.json, the
task's declared output: when nothing the run reads changed, the result is the cache's and the list is restored from it.
Refused while a block is open (tools/tasks/block.py). Exit 1 when a defect is found or the hunt task did not pass, 0 when none is."""
import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))  # tools: the tasks package

from tasks import block, pool  # noqa: E402
from tasks.cli import print_line, print_progress  # noqa: E402
from tasks.task import FAIL, CACHED_FAIL, PASSED  # noqa: E402
from tasks.suites import Suites  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
TASK = "hunt"
OUT = ROOT / "dist" / "hunt" / "hunt.json"
ERRORS = ("ERROR:", "SCRIPT ERROR:", "SHADER ERROR:")  # the engine's error lines in a run's log


def show(r) -> None:
    """One line per finished task (the hunt and the tasks it runs after), with the reason of one that did not pass."""
    why = f"  {r.message.splitlines()[0]}" if r.message and r.status not in PASSED else ""
    print(f"{r.status:<12} {r.name}  {r.seconds:.1f} s{why}", flush=True)


def log_lines(path: str) -> list[str]:
    log = Path(path) if path else None
    return log.read_text(encoding="utf-8", errors="replace").splitlines() if log and log.is_file() else []


def main() -> int:
    why = block.refusal("hunt", ROOT)
    if why:
        print(why, file=sys.stderr)
        return 1
    try:
        selection = Suites(root=ROOT).select([TASK], [])
    except ValueError as e:  # a name the suites do not know, a tool missing on this machine (UsageError is one too)
        print(f"hunt: {e}", file=sys.stderr)
        return 1
    if OUT.exists():
        OUT.unlink()  # no list of an earlier run is read as this one's: the run writes it, or the cache restores it
    results = pool.run(selection.tasks, on_result=show, root=ROOT, label="hunt", on_line=print_line,
                       on_progress=print_progress)
    hunt = next(r for r in results if r.name == TASK)
    if hunt.status not in PASSED:
        print(f"hunt: the task {TASK} ended {hunt.status}: {hunt.message or 'no message'} (log {hunt.log or 'none'})")
        if hunt.status not in (FAIL, CACHED_FAIL):  # a timeout, a crash or a task before it that failed: no list to trust
            return 1
    if not OUT.is_file():
        print(f"hunt: the run wrote no list ({hunt.status}; log {hunt.log or 'none'})")
        return 1
    found = json.loads(OUT.read_text(encoding="utf-8"))["defects"]
    # the engine's own errors are defects too (a shader that does not compile draws nothing), and the run's checks must pass
    lines = log_lines(hunt.log)
    errors = [line.strip() for line in lines if line.lstrip().startswith(ERRORS)]
    failed = next((line for line in lines if line.startswith("FAILURES:")), "")
    for line in lines:
        if line.startswith("HUNT ") or line.startswith("FAIL"):
            print(line)
    for e in dict.fromkeys(errors):
        found.append({"kind": "engine_error", "count": errors.count(e), "why": e, "owner": "see the log", "where": None})
    print(f"hunt: {len(found)} defect class(es) in the town ({failed or 'no FAILURES line'}; log {hunt.log}); list: {OUT}")
    for d in found:
        where = f" at {d['where']}" if d.get("where") is not None else ""
        print(f"{d['count']:6d}  {d['kind']}{where}\n        {d['why']}\n        -> {d['owner']}")
    return 1 if found or hunt.status not in PASSED or failed != "FAILURES: 0" else 0


if __name__ == "__main__":
    sys.exit(main())
