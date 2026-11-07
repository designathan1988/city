"""What a task does in its own process: reads one copy of a repository and hands back a small summary. Comparing is left to the process that holds the
plain scan, so no task carries it. Stacks go to a file every few seconds: a task that stops answering has already said where it is."""
import faulthandler
import os
import pickle

from ..repo import Repo
from . import checks_history, checks_keys, inject
from .checks_compare import labels, notes, snapshot
from .view import View

STACK_EVERY = 20


def read_copy(spec):
    """A repository, a commit of it or a view of it, read and judged."""
    view = spec.get("view")
    target = spec.get("folder") or Repo(spec["path"], spec.get("rev", "HEAD"))
    if view is not None:
        target = View(target, **view)
    run, a = inject.run_on(target, spec.get("store", True), True, off=spec.get("off", ()))
    undo = view["rename"].undo if view and view.get("rename") else (lambda s: s)
    out = {"errors": len(run.info.get("cpp", {}).get("errors", ()))}
    for part in spec.get("want", ("snapshot",)):
        if part == "snapshot":
            out[part] = snapshot(run, a, undo)
        elif part == "notes":
            out[part] = notes(a)
        elif part == "labels":
            out[part] = labels(a)
        elif part == "causes":
            out[part] = [((q["id"], q["reach"], q["commits"], q["score"]), [p["unit"] for p in q["places"]]) for q in a["queue"]]
        elif part == "groups":
            out[part] = [sorted(g["members"]) for g in a["groups"]]
    if spec.get("expect"):
        out["judged"] = inject.judge(run, a, spec["expect"])
    return out


KINDS = {"read_copy": read_copy, "defects_at": checks_keys.defects_at, "other_repository": checks_history.other_repository, "predicted": checks_history.predicted,
         "from_nothing": checks_history.from_nothing}


def write_task(store, job):
    folder = os.path.join(store, "jobs")
    os.makedirs(folder, exist_ok=True)
    with open(os.path.join(folder, job.name + ".in"), "wb") as f:
        pickle.dump((job.kind, job.spec), f, pickle.HIGHEST_PROTOCOL)


def result_of(store, name):
    with open(os.path.join(store, "jobs", name + ".out"), "rb") as f:
        return pickle.load(f)


def run_task(store, name):
    """The body of a task's process."""
    base = os.path.join(store, "jobs", name)
    with open(base + ".stack", "w", encoding="utf8") as stacks:
        faulthandler.dump_traceback_later(STACK_EVERY, repeat=True, file=stacks)  # how often it writes, not a limit
        with open(base + ".in", "rb") as f:
            kind, spec = pickle.load(f)
        result = KINDS[kind](spec)
        faulthandler.cancel_dump_traceback_later()
    with open(base + ".tmp", "wb") as f:
        pickle.dump(result, f, pickle.HIGHEST_PROTOCOL)
    os.replace(base + ".tmp", base + ".out")
    return 0
