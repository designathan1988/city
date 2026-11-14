"""The command line of tools/tasks: run suites or tasks (suites.json), serve tasks to other tools (JSON lines), status, the
block rule (block.py), the worktrees, the bounds of dist/ (clean) and the DLLs an open game holds set aside (free-dll).
Exit codes of `run`: 0 only when the selection is non-empty and every task passed; 1 for any FAIL, TIMEOUT, BROKEN or
SKIPPED, and for a run refused while a block is open; 2 for a usage error, an unknown name or an empty selection.
While tasks run, `run` prints each running task's lines as it writes them ("  [task] line") and a line of what the run is
doing every few seconds; `serve` writes both to stderr (its stdout is the JSON results alone); `status` shows every run
that lives. print_line and print_progress are the one way the tools print a run (km24.py, hunt.py import them).
Use: python tools/tasks/cli.py run <suite|task ...> [--all] [--<variant>] [--list] [--json OUT] [--jobs N] [--seconds N]
     python tools/tasks/cli.py serve [--jobs N]      python tools/tasks/cli.py status [--watch]      python tools/tasks/cli.py --list
     python tools/tasks/cli.py block open <name>|verify|close|status      python tools/tasks/cli.py free-dll
     python tools/tasks/cli.py worktrees [--all]      python tools/tasks/cli.py clean [--dry-run] [--unowned] [--only DIR]"""
from __future__ import annotations

import argparse
import json
import sys
import threading
import time
from collections import Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from tasks import block, cache, clean, lifecycle, verdicts  # noqa: E402
from tasks.cache import remove, write_atomic  # noqa: E402
from tasks.deadline import RunDeadline  # noqa: E402
from tasks.pool import Pool, run  # noqa: E402
from tasks.runs import RUNS, last_line, live_runs, newest_run  # noqa: E402
from tasks.suites import Suites, UsageError  # noqa: E402
from tasks.task import PASSED, ROOT, Task  # noqa: E402

OK, FAILED, USAGE = 0, 1, 2
SHOWN_RUNNING = 6  # running tasks named in a progress line; the rest are counted
MB_PER_GB = 1024.0
WATCH_S = 2.0  # status --watch: the state printed again this often, until no run lives
STATUS_LINE = 100  # characters of that line shown
LOADING = "Loading resource: "  # Godot's --verbose trace of each resource it loads: kept in the log, never printed live
HELP = f"{__doc__ or ''}\nusage: cli.py run|serve|status|block|free-dll|worktrees|clean ... (or --list, -h, --help)"
_out = threading.Lock()  # the lines of a run come from several threads (followers, delivery, scheduler): one at a time


def _emit(text: str, out=None) -> None:
    with _out:
        stream = out or sys.stdout
        stream.write(text + "\n")
        stream.flush()


def print_line(ident: str, line: str, out=None) -> None:
    """One line a running task wrote, as "  [ident] line" (to `out`, stdout by default). Godot's resource trace and empty
    lines stay in the log only."""
    if line.strip() and not line.startswith(LOADING):
        _emit(f"  [{ident}] {line}", out)


def _print_result(r, expected=()) -> None:
    line = f"{r.status:<12} {r.name}  {r.seconds:.1f} s"
    if r.message and r.status not in PASSED:
        first, *rest = r.message.splitlines() or [""]
        line += f"  {first}"
        line += "".join(f"\n    {x}" for x in rest[:verdicts.SHOWN_LINES])
    if r.status not in PASSED and r.log:
        line += "".join(f"\n    | {x}" for x in verdicts.failure_lines(r.log, expected=expected)) + f"\n    log: {r.log}"
    _emit(line)


def _spent(seconds: float, expected: float | None) -> str:
    return f"{seconds:.0f} s" + (f" of ~{expected:.0f}" if expected else "")


def print_progress(p: dict, out=None) -> None:
    """One line of what the run is doing (to `out`, stdout by default): how far it is, each running task with its seconds
    against what it took before, how many wait and for what, and the memory the machine has free."""
    running = ", ".join(f"{r['task']} {_spent(r['seconds'], r['expected'])}" for r in p["running"][:SHOWN_RUNNING])
    more = len(p["running"]) - SHOWN_RUNNING
    reasons: dict[str, int] = {}
    for why in p["waiting"].values():
        key = "the tasks before them" if why.startswith("after ") else why.split(" (")[0]
        reasons[key] = reasons.get(key, 0) + 1
    waits = ", ".join(f"{n} for {why}" for why, n in sorted(reasons.items(), key=lambda kv: -kv[1]))
    _emit(f"  .. {p['done']}/{p['total']} done | running {len(p['running'])}: {running}{f' (+{more})' if more > 0 else ''}"
          f" | waiting {len(p['waiting'])}{f' ({waits})' if waits else ''} | {p['memory_free_mb'] / MB_PER_GB:.1f} GB free",
          out)


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
    ap.add_argument("--seconds", type=float, help="the run's own deadline: past it it ends, its unfinished tasks TIMEOUT")
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
    print(f"selecting {' '.join(names)} ...", flush=True)
    t0 = time.monotonic()
    try:
        sel = s.select(names, variants, no_cache=a.all)
    except UsageError as e:
        print(e, file=sys.stderr)
        return USAGE
    print(f"{len(sel.tasks)} task(s) selected in {time.monotonic() - t0:.1f} s", flush=True)
    if a.list:
        return _list(s, sel)
    for n in sel.dropped:
        print(f"{n}: nothing to do (no changed file)")
    if not sel.tasks:
        print("empty selection: no task to run", file=sys.stderr)
        return USAGE
    why = block.refusal("run " + " ".join(names), root, [t.name for t in sel.tasks])
    if why:
        print(why, file=sys.stderr)
        return FAILED
    for stamp in sel.stamps:
        remove(root / stamp)  # no approval outlives the check that follows
    t0 = time.monotonic()
    expected = {t.name: t.params.get("expected_errors", ()) for t in sel.tasks}  # by name: sel.tasks is every task run
    with RunDeadline(a.seconds, [t.name for t in sel.tasks], lambda statuses: block.record(statuses, root)) as late:
        results = run(sel.tasks, jobs=a.jobs, root=root, use_cache=not sel.no_cache, capacity=s.doc.get("capacity", {}),
                      on_result=lambda r: (late.done(r), _print_result(r, expected.get(r.name, ()))),
                      label="-".join(names)[:40], on_progress=print_progress, on_line=print_line)
    block.record({r.name: r.status for r in results}, root)  # a failed check is not run again before its cause is named
    passed = sum(r.status in PASSED for r in results)
    summary = ", ".join(f"{v} {k}" for k, v in sorted(Counter(r.status for r in results).items()))
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
    each finishes: {"id","status","exit","seconds","log","key"} (a malformed line answers BROKEN with an "error"). Each
    running task's lines ("  [id] line") and what the run is doing go to stderr. Refused, exit 1, while a block is open."""
    ap = argparse.ArgumentParser(prog="cli.py serve")
    ap.add_argument("--jobs", type=int)
    ap.add_argument("--root", help=argparse.SUPPRESS)
    a = ap.parse_args(argv)
    root = Path(a.root).resolve() if a.root else ROOT
    why = block.refusal("serve", root)
    if why:
        print(why, file=sys.stderr)
        return FAILED
    ids: dict[str, object] = {}

    def finished(ident: str, r) -> None:
        _emit(json.dumps({"id": ids[ident], "status": r.status, "exit": r.exit_code, "seconds": r.seconds, "log": r.log,
                          "key": r.key, "message": r.message}, ensure_ascii=False), sys.stdout)

    pool = Pool(root=root, jobs=a.jobs, label="serve", on_finish=finished,
                on_line=lambda i, line: print_line(f"{ids.get(i, i)}", line, sys.stderr),
                on_progress=lambda p: print_progress(p, sys.stderr))
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
            _emit(json.dumps({"id": doc.get("id") if isinstance(doc, dict) else None, "status": "BROKEN", "exit": None,
                              "seconds": 0, "log": "", "key": "", "error": f"{type(e).__name__}: {e}"}, ensure_ascii=False),
                  sys.stdout)
    pool.wait_all()
    pool.close()
    return OK


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
            said = last_line(run_dir / f"{task}.log") if live else ""
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


def cmd_free_dll(argv: list[str]) -> int:
    """Each DLL of project/bin set aside under dist/held and copied back, dist/held held to its bound (cache.free_dlls)."""
    ap = argparse.ArgumentParser(prog="cli.py free-dll", description=cmd_free_dll.__doc__)
    ap.add_argument("--root", help=argparse.SUPPRESS)
    a = ap.parse_args(argv)
    try:
        cache.free_dlls(Path(a.root).resolve() if a.root else ROOT)
    except OSError as e:
        print(f"free-dll failed: {e}", file=sys.stderr)
        return FAILED
    return OK


def main(argv: list[str]) -> int:
    if argv[:1] == ["--list"]:
        return cmd_run(["--list"] + argv[1:])
    commands = {"run": cmd_run, "serve": cmd_serve, "status": cmd_status, "block": block.main, "free-dll": cmd_free_dll,
                "worktrees": lifecycle.cmd_worktrees, "clean": clean.cmd_clean}
    if not argv or argv[0] not in commands:
        asked = argv[:1] in (["-h"], ["--help"])  # help asked is an answer (stdout, OK); anything else a usage error
        print(HELP, file=sys.stdout if asked else sys.stderr)
        return OK if asked else USAGE
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
