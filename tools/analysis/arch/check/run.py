"""The selfcheck. Exit code 0 only if each check passed and each was shown able to fail.

It runs from a copy of the program, so the program can be edited while it works. The repository is read once, from nothing, into a store made for this
run; the checks that only look at that scan run at once in this process, while every other copy of the repository is a task of its own under the
supervisor. A check is judged the moment its last task ends, and a task that crashes or hangs is told at once and holds nothing back. `--again` keeps the
store and runs only what is named: for working. The delivery run has neither, and one scan with no store and no shortcut of any kind is among its tasks."""
import concurrent.futures
import json
import os
import pickle
import shutil
import subprocess
import sys
import threading
import time

import psutil

from ..cache import Cache
from ..core import digest, program_root, read_json, write_json
from ..pipeline import publish
from ..repo import Repo
from . import checks_compare, checks_copies, checks_history, checks_keys, checks_limits, job_kinds
from .inject import run_on
from .supervisor import Supervisor

# (name, the tasks it needs, the judgement once they ended). The first ones need nothing from the first scan and start beside it.
BESIDE_THE_SCAN = (
    ("determinism", checks_history.determinism_jobs, checks_history.determinism), ("portability", checks_history.portability_jobs, checks_history.portability),
    ("defects", checks_keys.defects_jobs, checks_keys.defects), ("prediction", checks_history.prediction_jobs, checks_history.prediction),
    ("ablation", checks_copies.ablation_jobs, checks_copies.ablation),
)
AFTER_THE_SCAN = (
    ("invariance", checks_copies.invariance_jobs, checks_copies.invariance), ("incremental", checks_copies.incremental_jobs, checks_copies.incremental),
    ("injection", checks_copies.injection_jobs, checks_copies.injection), ("antimakeup", checks_copies.antimakeup_jobs, checks_copies.antimakeup),
    ("stability", checks_copies.stability_jobs, checks_copies.stability), ("contamination", checks_copies.contamination_jobs, checks_copies.contamination),
)
ON_THE_SCAN = (
    ("failures", checks_compare.failures), ("proofs", checks_copies.proofs), ("closure", checks_limits.closure), ("keys", checks_keys.keys),
    ("cleanliness", checks_history.cleanliness), ("inert", checks_limits.inert),
)


class Context:
    """What the checks share: the repository, the plain scan they compare against, what each left for its judgement."""

    def __init__(self, args):
        self.path = args.repo
        self.cache = True
        self.repo = Repo(args.repo)
        self.base = os.path.join(program_root(), "dist", "arch", "check")
        self.store = os.path.join(self.base, "store")
        self.plain_scan = None
        self.opened = set()
        self.quiet = True
        self.state = {}
        self.began = time.time()
        self.saying = threading.Lock()
        self.ready = threading.Event()

    def plain(self):
        return self.plain_scan

    def say(self, text):
        with self.saying:
            print(f"[{round(time.time() - self.began):5d} s] {text}", flush=True)

    def _version(self):
        """The analysis the scan came from: what a kept scan must match to be used."""
        here = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        parts = []
        for folder, _, names in sorted(os.walk(here)):
            if os.path.basename(folder) != "check" and "__pycache__" not in folder:
                for name in sorted(names):
                    with open(os.path.join(folder, name), "rb") as f:
                        parts.append(digest(f.read()))
        return digest("".join(parts))

    def first_scan(self, again):
        """The scan everything starts from: from nothing, every file it opens seen by the interpreter, its reports written."""
        frozen = os.path.join(self.store, "plain.pkl")
        stamp = os.path.join(self.store, "plain.json")
        version = self._version()
        if again and os.path.exists(frozen) and read_json(stamp, {}).get("version") == version:
            with open(frozen, "rb") as f:
                self.plain_scan = pickle.load(f)
            self.plain_scan[0].repo, self.plain_scan[0].cache = self.repo, Cache(os.path.join(self.store, "cache"), True)
            self.opened = set(read_json(stamp)["opened"])
            return
        watching = [True]
        sys.addaudithook(lambda event, args: self.opened.add(args[0]) if watching[0] and event == "open" and isinstance(args[0], str) else None)
        run, a = run_on(self.repo, True, self.quiet)
        watching[0] = False
        run.home, run.key = os.path.normpath(program_root()), digest(run.repo.root)
        publish(run, a)
        run.cache.save()
        self.plain_scan = (run, a)

    def keep(self):
        """Only a run that leaves work behind pays for keeping its scan."""
        with open(os.path.join(self.store, "plain.pkl"), "wb") as f:
            pickle.dump(self.plain_scan, f, pickle.HIGHEST_PROTOCOL)
        write_json(os.path.join(self.store, "plain.json"), {"version": self._version(), "opened": sorted(self.opened)})


def _attempt(ctx, name, results, judge):
    started = time.time()
    try:
        result = judge()
    except Exception as e:  # a check that breaks has not passed
        import traceback
        result = {"pass": False, "detail": f"{type(e).__name__}: {e} {traceback.format_exc()[-900:]}"}
    result["seconds"] = round(time.time() - started)
    results[name] = result
    ctx.say(f"{'PASS' if result['pass'] else 'FAIL'} {name}: {json.dumps(result.get('detail'), ensure_ascii=False, default=str)[:1200]}")


class _Tasks:
    """The tasks of every check, and a check judged as soon as its own are all back, on a thread of its own: no judgement holds the supervisor."""

    def __init__(self, ctx, results):
        self.ctx, self.results = ctx, results
        self.left, self.verdicts, self.broken = {}, {}, {}
        self.judging = concurrent.futures.ThreadPoolExecutor(1)
        self.supervisor = Supervisor(ctx, self.finished, psutil.Process().memory_info().rss)

    def add(self, wanted):
        jobs = []
        for name, make, verdict in wanted:
            try:
                made = make(self.ctx)
            except Exception as e:  # a check that cannot even say what it needs has not passed, and the others go on
                import traceback
                self.results[name] = {"pass": False, "detail": f"{type(e).__name__}: {e} {traceback.format_exc()[-900:]}", "seconds": 0}
                self.ctx.say(f"FAIL {name}: {self.results[name]['detail']}")
                continue
            jobs += made
            self.left[name] = {j.name for j in made}
            self.verdicts[name] = verdict
        for job in jobs:
            job_kinds.write_task(self.ctx.store, job)
        self.supervisor.add(jobs)

    def finished(self, job, failure):
        if failure:
            self.broken.setdefault(job.check, []).append(f"{job.name}: {failure}")
        self.left[job.check].discard(job.name)
        if not self.left[job.check]:
            self.judging.submit(self.judge, job.check)

    def judge(self, check):
        ctx = self.ctx
        if check in self.broken:
            self.results[check] = {"pass": False, "detail": self.broken[check], "seconds": 0}
            ctx.say(f"FAIL {check}: {self.broken[check]}")
            return
        ctx.ready.wait()
        _attempt(ctx, check, self.results, lambda: self.verdicts[check](ctx, lambda n: job_kinds.result_of(ctx.store, n)))


def _from_a_copy(args):
    """The program copied, and run from there: what is edited meanwhile touches nothing that is running."""
    here = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    base = os.path.join(program_root(), "dist", "arch", "check")
    if not args.again:
        shutil.rmtree(base, ignore_errors=True)
    copy = os.path.join(base, "program")
    shutil.rmtree(copy, ignore_errors=True)
    skip = shutil.ignore_patterns("__pycache__")
    shutil.copytree(here, os.path.join(copy, "arch"), ignore=skip)
    shutil.copytree(here + "_eval", os.path.join(copy, "arch_eval"), ignore=skip)
    shutil.copy(here + ".py", os.path.join(copy, "arch.py"))
    words = ["selfcheck", args.repo]
    words += ["--only", args.only] if args.only else []
    words += ["--again"] if args.again else []
    env = dict(os.environ, ARCH_SELF=here, ARCH_OUT=os.path.join(base, "store"))
    return subprocess.run((sys.executable, os.path.join(copy, "arch.py"), *words), check=False, env=env).returncode


def main(args):
    if not os.environ.get("ARCH_SELF"):
        return _from_a_copy(args)
    if args.job:
        return job_kinds.run_task(os.environ["ARCH_OUT"], args.job)
    ctx = Context(args)
    only = set(filter(None, args.only.split(",")))
    os.makedirs(os.path.join(ctx.store, "jobs"), exist_ok=True)
    results = {}
    tasks = _Tasks(ctx, results)
    chosen = lambda table: [c for c in table if not only or c[0] in only]  # noqa: E731
    tasks.add(chosen(BESIDE_THE_SCAN))
    side = threading.Thread(target=tasks.supervisor.run)
    side.start()
    ctx.first_scan(args.again)
    ctx.ready.set()
    ctx.say("first scan done")
    tasks.add(chosen(AFTER_THE_SCAN))
    tasks.supervisor.close()
    for name, check in ON_THE_SCAN:
        if not only or name in only:
            _attempt(ctx, name, results, lambda check=check: check(ctx))
    side.join()
    tasks.judging.shutdown(wait=True)
    failed = sorted(n for n, r in results.items() if not r["pass"])
    target = os.path.join(program_root(), "dist", "arch", "selfcheck.json")
    previous = read_json(target, {}) if only or args.again else {}
    previous.update(results)
    write_json(target, previous)
    if failed or args.again:
        ctx.keep()
    else:
        shutil.rmtree(ctx.store, ignore_errors=True)
    ctx.say(f"selfcheck: {len(results) - len(failed)} passed, {len(failed)} failed {failed}, {'store kept, not a delivery run' if args.again else 'from nothing, no store from before the run'}")
    return 1 if failed else 0


__all__ = ["main", "sys"]
