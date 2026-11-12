"""The selfcheck: each check a task of the shared orchestration owner (tools/tasks), in its own process tree with its own deadline from data, killed
by PID on expiry; results keyed by the program's sources, its gold and the gold commit. Exit 0 only when every check ran and passed; status shows
each class's recall on its gold and its mutation, as this run measured them."""
import os
import sys

from ..catalogue import CLASSES
from ..core import program_root, read_json, write_json
from .gold import gold, settings

PROGRAM = ("tools/analysis/arch.py", "tools/analysis/arch/**", "tools/analysis/arch_eval/**")
TOOLS = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
sys.path.insert(0, TOOLS)  # the shared orchestration owner lives beside the analysis tools

import tasks  # noqa: E402


def plan(only=()):
    """The checks as tasks: the baseline scan first, everything else after it."""
    from ..repo import Repo
    conf, commit = settings(), gold()["commit"]
    limits = conf["timeouts_s"]
    builds = Repo(program_root()).on_disk("*compile_commands.json")  # what cpp_build.py reads
    inputs = [*PROGRAM, *builds]
    entry = os.path.join(program_root(), "tools", "analysis", "arch.py")
    cores = os.cpu_count() or 1
    share = max(1, cores // len(CLASSES))

    def task(name, kind, after, use):
        out = f"dist/arch/check/{name.replace(':', '-')}"
        return tasks.Task(name=f"arch-{name.replace(':', '-')}", argv=[sys.executable, entry, "step", name, os.path.join(program_root(), out)],
                          timeout_s=float(limits[kind]), inputs=inputs, extra=[commit, name], outputs=[out], after=after, cwd=program_root(),
                          env={"ARCH_CORES": str(use), "ARCH_STEP_TIMEOUT": str(limits[kind]), "PYTHONHASHSEED": "0"}, resources={"cores": use}, fresh=True)

    out = [task("baseline", "baseline", [], cores)]
    out += [task(n, n, ["arch-baseline"], 1) for n in ("gold", "registry", "cleanliness")]
    out.append(task("determinism", "determinism", ["arch-baseline"], share))
    out += [task(f"mutation:{cid}", "mutation", ["arch-baseline"], share) for cid, _, _ in CLASSES]
    if only:
        keep = {f"arch-{n.replace(':', '-')}" for n in only} | {"arch-baseline"}
        out = [t for t in out if t.name in keep]
    return out


def main(args):
    only = [n for n in args.only.split(",") if n]
    todo = plan(only)

    def say(r):
        print(f"{r.status:<12} {r.name:<32} {r.seconds:8.1f}s  {r.log}", flush=True)

    results = tasks.run(todo, on_result=say, label="arch-selfcheck")
    by = {r.name: r for r in results}
    home = os.path.join(program_root(), "dist", "arch", "check")
    table = read_json(os.path.join(home, "gold", "result.json"), {}).get("detail", {})
    classes = {}
    for cid, _, _ in CLASSES:
        r = by.get(f"arch-mutation-{cid}")
        row = dict(table.get(cid, {"gold_found": 0, "gold_total": 0, "missed": []}))
        row["mutation"] = r.status if r else "NOT RUN"
        row["mutation_detail"] = read_json(os.path.join(home, f"mutation-{cid}", "result.json"), {}).get("detail")
        classes[cid] = row
    checks = {r.name: {"status": r.status, "seconds": round(r.seconds, 1), "log": r.log, "message": r.message,
                       "detail": read_json(os.path.join(home, r.name[len("arch-"):], "result.json"), {}).get("detail")} for r in results}
    ok = bool(results) and all(r.passed for r in results) and len(results) == len(todo)
    write_json(os.path.join(program_root(), "dist", "arch", "selfcheck.json"), {"pass": ok, "commit": gold()["commit"], "classes": classes, "checks": checks})
    for cid, row in classes.items():
        print(f"  {cid:<20} gold {row['gold_found']}/{row['gold_total']} {'missed ' + ', '.join(row['missed']) if row['missed'] else ''}  mutation {row['mutation']}")
    failed = sorted(r.name for r in results if not r.passed)
    print(f"selfcheck: {len(results) - len(failed)} of {len(results)} passed{', failed: ' + ', '.join(failed) if failed else ''}")
    return 0 if ok else 1
