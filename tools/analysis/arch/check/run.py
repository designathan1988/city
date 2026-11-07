"""The selfcheck. Exit code 0 only if each check passed and each was shown able to fail.

The real repository is read once. What needs no scan of it runs from the first second on the small repository, which holds every class of link and cost and
healthy code beside each; what depends on the real shape runs on the graph of that one scan, in memory, in processes side by side. Results of pure work are
kept by content, and `incremental` is what proves that keeping them changes nothing."""
import concurrent.futures
import json
import os
import pickle
import shutil
import sys
import threading
import time

import psutil

from ..core import digest, program_root, read_json, write_json
from ..pipeline import publish
from ..repo import Repo
from . import checks_compare, checks_copies, checks_history, checks_keys, checks_limits, job_kinds, oracle
from .inject import run_on
from .supervisor import Supervisor

# (name, the tasks it needs, the judgement once they ended). The first ones need nothing from the scan.
BESIDE_THE_SCAN = (
    ("injection", checks_copies.injection_jobs, checks_copies.injection), ("ablation", checks_copies.ablation_jobs, checks_copies.ablation),
    ("invariance", checks_copies.invariance_jobs, checks_copies.invariance), ("antimakeup", checks_copies.antimakeup_jobs, checks_copies.antimakeup),
    ("incremental", checks_copies.incremental_jobs, checks_copies.incremental), ("determinism_small", checks_history.determinism_jobs, checks_history.determinism_small),
    ("prediction", checks_history.prediction_jobs, checks_history.prediction), ("portability", checks_history.portability_jobs, checks_history.portability),
    ("defects", checks_keys.defects_jobs, checks_keys.defects),
)
AFTER_THE_SCAN = (
    ("invariance_graph", checks_copies.invariance_on_graph, checks_copies.invariance_graph), ("antimakeup_graph", checks_copies.antimakeup_on_graph, checks_copies.antimakeup_graph),
    ("injection_graph", checks_copies.injection_on_graph, checks_copies.injection_graph), ("stability", checks_copies.stability_jobs, checks_copies.stability),
    ("contamination", checks_copies.contamination_jobs, checks_copies.contamination), ("determinism", checks_history.second_seed_jobs, checks_history.determinism),
)
ON_THE_SCAN = (
    ("failures", checks_compare.failures), ("precision", oracle.precision), ("proofs", checks_copies.proofs), ("closure", checks_limits.closure), ("keys", checks_keys.keys),
    ("cleanliness", checks_history.cleanliness), ("inert", checks_limits.inert),
)


class Context:
    """What the checks share: the repository, the plain scan they compare against, what each left for its judgement."""

    def __init__(self, args):
        self.path = args.repo
        self.cache = True
        self.repo = Repo(args.repo)
        self.home = os.path.join(program_root(), "dist", "arch")
        self.base = os.path.join(self.home, "check")
        self.store = self.base
        self.frozen = os.path.join(self.base, "run.pkl")
        self.plain_scan = None
        self.opened = set()
        self.quiet = True
        self.state = {}
        self.began = time.time()
        self.saying = threading.Lock()
        self.ready = threading.Event()
        self.small_ready = threading.Event()

    def plain(self):
        return self.plain_scan

    def say(self, text):
        with self.saying:
            print(f"[{round(time.time() - self.began):5d} s] {text}", flush=True)

    def first_scan(self):
        """The one read of the repository: every file it opens seen by the interpreter, its reports written, its graph handed to whoever needs it."""
        watching = [True]
        sys.addaudithook(lambda event, args: self.opened.add(args[0]) if watching[0] and event == "open" and isinstance(args[0], str) else None)
        run, a = run_on(self.repo, True, self.quiet, base=self.home)
        watching[0] = False
        run.home, run.key = os.path.normpath(program_root()), digest(run.repo.root)
        os.environ["ARCH_OUT"] = self.home
        publish(run, a)
        del os.environ["ARCH_OUT"]
        run.cache.save()
        with open(self.frozen, "wb") as f:
            pickle.dump(run, f, pickle.HIGHEST_PROTOCOL)
        self.plain_scan = (run, a)


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
        if job.name == "small_cases":
            if not failure:
                with open(os.path.join(self.ctx.base, "small_facts.pkl"), "wb") as f:
                    pickle.dump(job_kinds.result_of(self.ctx.store, job.name)["facts"], f, pickle.HIGHEST_PROTOCOL)
            self.ctx.small_ready.set()
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


def main(args):
    ctx = Context(args)
    if args.job:
        return job_kinds.run_task(ctx.store, args.job)
    only = set(filter(None, args.only.split(",")))
    shutil.rmtree(ctx.base, ignore_errors=True)
    os.makedirs(os.path.join(ctx.store, "jobs"), exist_ok=True)
    results = {}
    tasks = _Tasks(ctx, results)
    chosen = lambda table: [c for c in table if not only or c[0] in only]  # noqa: E731
    before = chosen(BESIDE_THE_SCAN)
    if not only or "injection_graph" in only:
        before = [*before, *(c for c in BESIDE_THE_SCAN if c[0] == "injection" and c not in before)]
    tasks.add(before)
    side = threading.Thread(target=tasks.supervisor.run)
    side.start()
    ctx.first_scan()
    ctx.say("scan done")
    ctx.ready.set()
    after = chosen(AFTER_THE_SCAN)
    if any(c[0] == "injection_graph" for c in after):
        ctx.small_ready.wait()
    tasks.add(after)
    tasks.supervisor.close()
    for name, check in ON_THE_SCAN:
        if not only or name in only:
            _attempt(ctx, name, results, lambda check=check: check(ctx))
    side.join()
    tasks.judging.shutdown(wait=True)
    failed = sorted(n for n, r in results.items() if not r["pass"])
    target = os.path.join(ctx.home, "selfcheck.json")
    previous = read_json(target, {}) if only else {}
    previous.update(results)
    write_json(target, previous)
    ctx.say(f"selfcheck: {len(results) - len(failed)} passed, {len(failed)} failed {failed}, {round(time.time() - ctx.began)} s")
    return 1 if failed else 0


__all__ = ["main", "sys"]
