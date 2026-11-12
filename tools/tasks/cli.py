# The command line of tools/tasks: run suites or tasks (suites.json), serve tasks to other tools (JSON lines), status, a
# machine var (suites.json vars: where Godot, bash and llvm-mingw are) for the shell scripts, and the block rule (block.py).
# Exit codes of `run`: 0 only when the selection is non-empty and every task passed; 1 for any FAIL, TIMEOUT, BROKEN or
# SKIPPED, and for a run refused while a block is open; 2 for a usage error, an unknown name or an empty selection.
# While tasks run, `run` prints a line of what it is doing every few seconds; `status` shows every run that lives.
# Use: python tools/tasks/cli.py run <suite|task ...> [--all] [--<variant>] [--list] [--json OUT] [--jobs N]
#      python tools/tasks/cli.py serve [--jobs N]      python tools/tasks/cli.py status [--watch]      python tools/tasks/cli.py --list
#      python tools/tasks/cli.py var <name>      python tools/tasks/cli.py block open <name>|verify|close|status
from __future__ import annotations

import argparse
import json
import sys
import threading
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from tasks import block, verdicts  # noqa: E402
from tasks.cache import remove, write_atomic  # noqa: E402
from tasks.pool import Pool, run  # noqa: E402
from tasks.runs import RUNS, live_runs, newest_run  # noqa: E402
from tasks.suites import Suites, UsageError  # noqa: E402
from tasks.task import PASSED, ROOT, Task  # noqa: E402

OK, FAILED, USAGE = 0, 1, 2
SHOWN_RUNNING = 6  # running tasks named in a progress line; the rest are counted
MB_PER_GB = 1024.0
WATCH_S = 2.0  # status --watch: the state printed again this often, until no run lives
TAIL_BYTES = 4096  # of a running task's log read for its last line
STATUS_LINE = 100  # characters of that line shown


def _print_result(r) -> None:
    line = f"{r.status:<12} {r.name}  {r.seconds:.1f} s"
    if r.message and r.status not in PASSED:
        first, *rest = r.message.splitlines() or [""]
        line += f"  {first}"
        line += "".join(f"\n    {x}" for x in rest[:verdicts.SHOWN_LINES])
    print(line, flush=True)
    if r.status not in PASSED and r.log:
        for x in verdicts.failure_lines(r.log):
            print(f"    | {x}")
        print(f"    log: {r.log}", flush=True)


def _spent(seconds: float, expected: float | None) -> str:
    return f"{seconds:.0f} s" + (f" of ~{expected:.0f}" if expected else "")


def _print_progress(p: dict) -> None:
    """One line of what the run is doing: how far it is, each running task with its seconds against what it took before,
    how many wait and for what, and the memory the machine has free."""
    running = ", ".join(f"{r['task']} {_spent(r['seconds'], r['expected'])}" for r in p["running"][:SHOWN_RUNNING])
    more = len(p["running"]) - SHOWN_RUNNING
    reasons: dict[str, int] = {}
    for why in p["waiting"].values():
        key = "the tasks before them" if why.startswith("after ") else why.split(" (")[0]
        reasons[key] = reasons.get(key, 0) + 1
    waits = ", ".join(f"{n} for {why}" for why, n in sorted(reasons.items(), key=lambda kv: -kv[1]))
    print(f"  .. {p['done']}/{p['total']} done | running {len(p['running'])}: {running}{f' (+{more})' if more > 0 else ''}"
          f" | waiting {len(p['waiting'])}{f' ({waits})' if waits else ''} | {p['memory_free_mb'] / MB_PER_GB:.1f} GB free",
          flush=True)


def _list(s: Suites, sel=None) -> int:
    if sel is None:
        print("suites:")
        for name, suite in s.doc["suites"].items():
            variants = " ".join(f"--{v}" for v in suite.get("variants", {}))
            print(f"  {name:<10} {suite.get('doc', '')}" + (f" [{variants}]" if variants else ""))
        print("groups: " + ", ".join(s.doc["groups"]))
        print("tasks:")
        for n in s.task_names():
            print(f"  {n}")
        return OK
    for t in sel.tasks:
        after = f" after {', '.join(t.after)}" if t.after else ""
        print(f"{t.name}  ({t.timeout_s:g} s{', hidden' if t.hidden else ''}){after}\n    {' '.join(t.argv)}")
    for n in sel.dropped:
        print(f"{n}  (nothing to do: no changed file)")
    return OK if sel.tasks else USAGE


def cmd_run(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(prog="cli.py run")
    ap.add_argument("names", nargs="*", help="suites, groups (@name) or tasks; commas also separate")
    ap.add_argument("--all", action="store_true", help="the full run: nothing taken from the cache (results still stored)")
    ap.add_argument("--list", action="store_true", help="show what would run, run nothing")
    ap.add_argument("--json", help="write the results there")
    ap.add_argument("--jobs", type=int, help="cores the pool may fill (default: all)")
    ap.add_argument("--suites", help=argparse.SUPPRESS)
    ap.add_argument("--root", help=argparse.SUPPRESS)
    try:
        a, rest = ap.parse_known_args(argv)
    except SystemExit as e:
        return USAGE if e.code else OK
    bad = [x for x in rest if not x.startswith("--") or x == "--"]
    if bad:
        print(f"unexpected arguments: {' '.join(bad)}", file=sys.stderr)
        return USAGE
    variants = [x[2:] for x in rest] + (["all"] if a.all else [])
    names = [n for x in a.names for n in x.split(",") if n]
    root = Path(a.root).resolve() if a.root else ROOT
    s = Suites(a.suites, root=root, listing=a.list)
    if a.list and not names:
        return _list(s)
    if not names:
        print("nothing selected: name a suite, group or task (--list shows them)", file=sys.stderr)
        return USAGE
    try:
        sel = s.select(names, variants, no_cache=a.all)
    except UsageError as e:
        print(e, file=sys.stderr)
        return USAGE
    if a.list:
        return _list(s, sel)
    for n in sel.dropped:
        print(f"{n}: nothing to do (no changed file)")
    if not sel.tasks:
        print("empty selection: no task to run", file=sys.stderr)
        return USAGE
    why = block.refusal("run " + " ".join(names), root)
    if why:
        print(why, file=sys.stderr)
        return FAILED
    for stamp in sel.stamps:
        remove(root / stamp)  # no approval outlives the check that follows
    capacity = s.doc.get("capacity", {})
    t0 = time.monotonic()
    results = run(sel.tasks, jobs=a.jobs, on_result=_print_result, root=root, use_cache=not sel.no_cache,
                  capacity=capacity, label="-".join(names)[:40], on_progress=_print_progress)
    passed = sum(r.status in PASSED for r in results)
    counts = {}
    for r in results:
        counts[r.status] = counts.get(r.status, 0) + 1
    summary = ", ".join(f"{v} {k}" for k, v in sorted(counts.items()))
    print(f"{passed}/{len(results)} passed ({summary}) in {time.monotonic() - t0:.0f} s; logs in {root / RUNS}")
    if a.json:
        write_atomic(Path(a.json), json.dumps([r.to_json() for r in results], indent=1))
    ok = bool(results) and passed == len(results)
    if ok:
        for stamp in sel.stamps:
            write_atomic(root / stamp, time.strftime("%Y-%m-%d %H:%M:%S\n"))
    return OK if ok else FAILED


def cmd_serve(argv: list[str]) -> int:
    """JSON lines in: {"id","name","argv","cwd","inputs","extra","outputs","timeout_s","env","hidden"}; JSON lines out as
    each finishes: {"id","status","exit","seconds","log","key"} (a malformed line answers BROKEN with an "error")."""
    ap = argparse.ArgumentParser(prog="cli.py serve")
    ap.add_argument("--jobs", type=int)
    ap.add_argument("--root", help=argparse.SUPPRESS)
    a = ap.parse_args(argv)
    root = Path(a.root).resolve() if a.root else ROOT
    out_lock = threading.Lock()
    ids: dict[str, object] = {}

    def emit(doc: dict) -> None:
        with out_lock:
            sys.stdout.write(json.dumps(doc, ensure_ascii=False) + "\n")
            sys.stdout.flush()

    def finished(ident: str, r) -> None:
        emit({"id": ids[ident], "status": r.status, "exit": r.exit_code, "seconds": r.seconds, "log": r.log, "key": r.key,
              "message": r.message})

    pool = Pool(root=root, jobs=a.jobs, label="serve", on_finish=finished)
    n = 0
    for line in sys.stdin:
        line = line.strip()
        if not line:
            continue
        n += 1
        doc = {}
        try:
            doc = json.loads(line)
            ident = f"{doc.get('id', n)}"
            task = Task(name=doc["name"], argv=doc["argv"], timeout_s=doc["timeout_s"], inputs=doc.get("inputs", []),
                        extra=doc.get("extra", []), outputs=doc.get("outputs", []), cwd=doc.get("cwd"),
                        env=doc.get("env"), hidden=bool(doc.get("hidden", False)))
            key = f"serve:{ident}"
            if key in ids:
                raise ValueError(f"id {ident} submitted twice")
            ids[key] = doc.get("id", n)
            pool.submit(task, ident=key)
        except Exception as e:  # a malformed request is answered, not dropped
            emit({"id": doc.get("id") if isinstance(doc, dict) else None, "status": "BROKEN", "exit": None,
                  "seconds": 0, "log": "", "key": "", "error": f"{type(e).__name__}: {e}"})
    pool.wait_all()
    pool.close()
    return OK


def _last_line(log: Path) -> str:
    """The last line with anything on it of a log another process is still writing."""
    try:
        with open(log, "rb") as fh:
            fh.seek(max(0, log.stat().st_size - TAIL_BYTES))
            lines = [x.strip() for x in fh.read().decode("utf-8", errors="replace").splitlines() if x.strip()]
    except OSError:
        return ""
    return lines[-1] if lines else ""


def _print_run(run_dir: Path, live: bool) -> None:
    events = run_dir / "events.jsonl"
    state: dict[str, dict] = {}
    if events.is_file():
        for line in events.read_text(encoding="utf-8").splitlines():
            e = json.loads(line)
            state[e["task"]] = e
    running = [task for task, e in state.items() if e["event"] == "start"]
    done = len(state) - len(running)
    print(f"run {run_dir.name}  ({'live' if live else 'ended'}: {done} done, {len(running)} running)")
    for task, e in state.items():
        if e["event"] == "start":
            said = _last_line(run_dir / f"{task}.log") if live else ""
            print(f"RUNNING      {task}  {_spent(time.time() - e['t'], e.get('expected'))}, {len(e.get('cores', []))} cores"
                  + (f"  | {said[:STATUS_LINE]}" if said else ""))
        else:
            print(f"{e['status']:<12} {task}  {e['seconds']:.1f} s" + (f"  {e['message'].splitlines()[0]}"
                                                                      if e.get("message") and e["status"] not in PASSED
                                                                      else ""))


def cmd_status(argv: list[str]) -> int:
    """Every run whose process lives, each task with its state (a running one with its seconds against what it took before
    and the last line of its log), or the newest run when none lives. --watch prints it again until no run lives."""
    ap = argparse.ArgumentParser(prog="cli.py status")
    ap.add_argument("--watch", action="store_true", help="print the state again every few seconds while a run lives")
    ap.add_argument("--root", help=argparse.SUPPRESS)
    a = ap.parse_args(argv)
    root = Path(a.root).resolve() if a.root else ROOT
    while True:
        live = live_runs(root)
        if live:
            for run_dir in live:
                _print_run(run_dir, True)
        else:
            run_dir = newest_run(root)  # the folder of the latest name (runs.py): no file every run must win
            if run_dir is None:
                print("no run yet")
                return USAGE
            _print_run(run_dir, False)
        if not a.watch or not live:
            return OK
        print(flush=True)
        time.sleep(WATCH_S)


def cmd_var(argv: list[str]) -> int:
    """A machine var as the tasks resolve it, its environment variable first and then this platform's default."""
    ap = argparse.ArgumentParser(prog="cli.py var")
    ap.add_argument("name")
    ap.add_argument("--suites", help=argparse.SUPPRESS)
    a = ap.parse_args(argv)
    print(Suites(a.suites, root=ROOT).var(a.name), end="")  # no newline: a shell's $(...) keeps no \r of Windows' own
    return OK


def main(argv: list[str]) -> int:
    if argv[:1] == ["--list"]:
        return cmd_run(["--list"] + argv[1:])
    commands = {"run": cmd_run, "serve": cmd_serve, "status": cmd_status, "var": cmd_var, "block": block.main}
    if not argv or argv[0] not in commands:
        print(__doc__ if __doc__ else "", "usage: cli.py run|serve|status|var|block ... (or --list)", file=sys.stderr)
        return USAGE
    try:
        return commands[argv[0]](argv[1:])
    except UsageError as e:
        print(e, file=sys.stderr)
        return USAGE
    except ValueError as e:  # a malformed suite or task set
        print(f"error: {e}", file=sys.stderr)
        return USAGE


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
