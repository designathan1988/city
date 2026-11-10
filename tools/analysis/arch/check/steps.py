"""One selfcheck step, run as its own process by the orchestrator (`arch.py step <name> <folder>`): the baseline scan of the gold commit, the gold
judge, one mutation per class, determinism, the registry and cleanliness. Each writes result.json in its folder and exits 0 only when it passed;
before its deadline it prints every thread's stack, so a step killed for time says where it was."""
import faulthandler
import json
import os
import pickle
import sys

from ..catalogue import CLASSES
from ..core import program_root, read_json, write_json
from .gold import gold, recall, settings

PUBLISHED = ("queue.json", "findings.json", "entries.json", "coverage.json", "slices.json")


def _home():
    return os.path.join(program_root(), "dist", "arch")


def _scan(repo, out, cache=True):
    from ..pipeline import analyse, publish
    from ..scan import Run
    run = Run(repo.root, repo=repo, cache=cache, out=_home(), quiet=False)
    try:
        run.read()
        a = analyse(run)
        publish(run, a, out)
    finally:
        run.close()
    return run, a


def _repo():
    from ..repo import Repo
    return Repo(program_root(), gold()["commit"])


def baseline(out):
    repo, opened = _repo(), []
    sys.addaudithook(lambda event, args: opened.append(args[0]) if event == "open" and isinstance(args[0], str) else None)
    run, a = _scan(repo, out)
    run.cache.save()
    names = {f["name"] for f in a["program"].fn.values()} | {c["name"] for c in a["program"].cls.values()} | {u["name"] for u in run.graph.units.values()}
    state = {"summaries": run.facts.summaries, "entries": a["entries"], "findings": a["findings"], "tracked": run.tracked,
             "tokens": {p: w for p, w in run.roles.tokens.items() if p not in run.roles.foreign},
             "foreign_tokens": set().union(*(w for p, w in run.roles.tokens.items() if p in run.roles.foreign)), "api_words": run.api.words | run.api.props | set(run.api.classes),
             "names": names, "opened": sorted(set(opened))}
    with open(os.path.join(out, "baseline.pkl") + ".tmp", "wb") as f:
        pickle.dump(state, f, pickle.HIGHEST_PROTOCOL)
    os.replace(os.path.join(out, "baseline.pkl") + ".tmp", os.path.join(out, "baseline.pkl"))
    return True, {"findings": len(a["findings"]), "entries": len(a["entries"]), "functions": len(a["program"].fn)}


def _baseline():
    with open(os.path.join(_home(), "check", "baseline", "baseline.pkl"), "rb") as f:
        return pickle.load(f)


def judge_gold(out):
    table = recall(_baseline()["findings"], [cid for cid, _, _ in CLASSES])
    ok = all(r["gold_total"] and r["gold_found"] == r["gold_total"] for r in table.values())
    return ok, table


class _Context:
    """What a mutation reads: the baseline program, its findings, the commit's text of a file, the slices when asked."""

    def __init__(self, base, repo):
        from ..summaries import Program
        self.base, self.repo = base, repo
        self.prog = Program(base["summaries"])
        self.findings, self.entries = base["findings"], base["entries"]
        self._slices = {}

    def text(self, path):
        return self.repo.blob(self.base["tracked"][path])

    def slices(self, tracing=False):
        if tracing not in self._slices:
            from ..slices import build_all
            self._slices[tracing] = build_all(self.prog, self.entries, tracing)
        return self._slices[tracing]


def mutate(cid, out):
    from .edited import Edited
    from .registry import MUTATIONS
    base, repo = _baseline(), _repo()
    m = MUTATIONS[cid](_Context(base, repo))
    _, a = _scan(Edited(repo, {m["file"]: m["new"]}), out)
    known = {(f["class"], f["file"], f["line"], f["detail"]) for f in base["findings"]}
    file, lo, hi = m["expect"]
    new = [f for f in a["findings"] if f["class"] == cid and (f["class"], f["file"], f["line"], f["detail"]) not in known and f["file"] == file and lo <= f["line"] <= hi]
    return bool(new), {"mutation": m["note"], "file": m["file"], "expect": m["expect"], "new": [f"{f['file']}:{f['line']} {f['detail']}" for f in new][:5]}


def determinism(out):
    _scan(_repo(), out)
    first = os.path.join(_home(), "check", "baseline")
    apart = []
    for name in PUBLISHED:
        with open(os.path.join(first, name), "rb") as a, open(os.path.join(out, name), "rb") as b:
            if a.read() != b.read():
                apart.append(name)
    return not apart, {"compared": list(PUBLISHED), "apart": apart}


def registry(out):
    from .registry import verify
    problems = verify()
    return not problems, problems


def cleanliness(out):
    from .clean import check
    return check(_repo(), _baseline())


STEPS = {"baseline": baseline, "gold": judge_gold, "determinism": determinism, "registry": registry, "cleanliness": cleanliness}


def main(name, out):
    timeout = float(os.environ.get("ARCH_STEP_TIMEOUT", "0"))
    margin = float(settings()["stack_margin_s"])
    if timeout > margin:
        faulthandler.dump_traceback_later(timeout - margin, exit=False)
    os.makedirs(out, exist_ok=True)
    if name.startswith("mutation:"):
        ok, detail = mutate(name.split(":", 1)[1], out)
    else:
        ok, detail = STEPS[name](out)
    write_json(os.path.join(out, "result.json"), {"pass": bool(ok), "detail": detail})
    print(json.dumps({"step": name, "pass": bool(ok), "detail": detail}, ensure_ascii=False, default=str)[:4000], flush=True)
    faulthandler.cancel_dump_traceback_later()
    return 0 if ok else 1


__all__ = ["main", "read_json"]
