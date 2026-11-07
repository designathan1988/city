"""What a task does in its own process: reads one copy of something and hands back a small summary. Comparing is left to the process that holds the plain
scan, so no task carries it. Stacks go to a file every few seconds: a task that stops answering has already said where it is."""
import os
import pickle
import sys
import threading
import time
import traceback

from ..core import digest, program_root
from ..pipeline import publish
from ..repo import Repo
from . import checks_history, checks_keys, inject, graph_steps
from .checks_compare import labels, notes, snapshot
from .view import View

STACK_EVERY = 5


def _trace(run, a):
    ways = {}
    for e in run.graph.edges.values():
        ways.setdefault(e.src, []).append(f"{e.kind}>{e.dst}:{e.conf:.6f}")
    return {"edges": {u: digest("".join(sorted(w))) for u, w in ways.items()}, "layers": {g["members"][0]: a["layer_of"].get(g["id"]) for g in a["groups"]},
            "units": {u: digest(repr((x["name"], x["kind"], x["parent"], x["shape"], x["attrs"].get("statements")))) for u, x in run.graph.units.items()}}


def _summary(run, a, spec, undo):
    out = {"errors": len(run.info.get("cpp", {}).get("errors", ()))}
    for part in spec.get("want", ("snapshot",)):
        if part == "snapshot":
            out[part] = snapshot(run, a, undo)
        elif part == "notes":
            out[part] = notes(a)
        elif part == "labels":
            out[part] = labels(a)
        elif part == "trace":
            out[part] = _trace(run, a)
        elif part == "facts":
            out[part] = run.facts
    if spec.get("expect"):
        out["judged"] = inject.judge(run, a, spec["expect"])
    return out


def read_copy(spec):
    """A repository, a commit of it or a view of it, read and judged."""
    view = spec.get("view")
    target = spec.get("folder") or Repo(spec["path"], spec.get("rev", "HEAD"))
    if view is not None:
        target = View(Repo(target) if isinstance(target, str) else target, **view)
    if spec.get("publish"):
        os.environ["ARCH_OUT"] = spec["publish"]
    run, a = inject.run_on(target, spec.get("store", True), True, off=spec.get("off", ()), base=spec.get("out"))
    if spec.get("publish"):
        run.base, run.home, run.key = spec["publish"], os.path.normpath(program_root()), ""
        publish(run, a)
    return _summary(run, a, spec, view["rename"].undo if view and view.get("rename") else (lambda s: s))


def incremental(spec):
    """A copy read through a store that knew the original, and read from nothing: the same, down to each edge. What proves every cached result."""
    folder, view, out = spec["folder"], spec["view"], spec["out"]
    run, _ = inject.run_on(folder, True, True, base=out)
    run.cache.save()
    results = []
    for store in (True, False):
        run, a = inject.run_on(View(Repo(folder), **view), store, True, base=out)
        results.append({"snapshot": snapshot(run, a), "trace": _trace(run, a)})
    return results


KINDS = {"read_copy": read_copy, "incremental": incremental, "graph_steps": graph_steps.graph_steps, "defects_at": checks_keys.defects_at, "other_repository": checks_history.other_repository, "predicted": checks_history.predicted}


def write_task(store, job):
    folder = os.path.join(store, "jobs")
    os.makedirs(folder, exist_ok=True)
    with open(os.path.join(folder, job.name + ".in"), "wb") as f:
        pickle.dump((job.kind, job.spec), f, pickle.HIGHEST_PROTOCOL)


def result_of(store, name):
    with open(os.path.join(store, "jobs", name + ".out"), "rb") as f:
        return pickle.load(f)


def _keep_stacks(path):
    """Where the task is, written over and over by a thread that holds the interpreter while it reads. The standard library's timed dump reads frames
    without holding it and can crash the very process it watches, which is what it did here."""
    me = threading.get_ident()
    while True:
        time.sleep(STACK_EVERY)
        frames = sys._current_frames()
        text = "".join("".join(traceback.format_stack(f)) for i, f in frames.items() if i != me)
        with open(path + ".tmp", "w", encoding="utf8") as f:
            f.write(text)
        os.replace(path + ".tmp", path)


def run_task(store, name):
    """The body of a task's process."""
    base = os.path.join(store, "jobs", name)
    threading.Thread(target=_keep_stacks, args=(base + ".stack",), daemon=True).start()
    with open(base + ".in", "rb") as f:
        kind, spec = pickle.load(f)
    result = KINDS[kind](spec)
    with open(base + ".tmp", "wb") as f:
        pickle.dump(result, f, pickle.HIGHEST_PROTOCOL)
    os.replace(base + ".tmp", base + ".out")
    return 0
