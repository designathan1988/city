"""Checks that compare copies of the repository with the plain scan: cases injected, adapters off, names changed, code rearranged, parts taken out. Each
says which copies it needs, the supervisor reads them side by side, and the judgement here only compares what came back."""
import collections
import json
import os
import random

from .. import external
from ..core import own_prefixes
from ..history import giant_cut
from ..scan import ADAPTERS
from . import inject, inject_extra, rearrange
from .checks_compare import _ablation_judge, chained, nmi, same
from .rename import Renamer, outside_vocabulary
from .supervisor import Job

BEST_OF_THE_PROTOTYPE = 0.88


def judge_injection(found):
    return bool(found) and all(found.values())


def _seed():
    return int.from_bytes(os.urandom(4), "big")


def _api_file(tracked):
    """The description the project ships, for the small repository to read."""
    for path in sorted(tracked):
        name = path.rsplit("/", 1)[-1]
        if path.endswith(".json") and "api" in name and name.startswith("extension"):
            return path
    return None


def _api_copy(ctx, tracked):
    picked = _api_file(tracked)
    if not picked:
        return None
    source = os.path.join(ctx.base, "api.json")
    if not os.path.exists(source):
        with open(source, "wb") as f:
            f.write(ctx.repo.blob(tracked[picked]))
    return source


def _fixture(ctx, seed):
    tracked = ctx.repo.files()
    api = external.load(ctx.repo, tracked)
    define = f"ONLY_{seed}"
    files, expect, profile = inject.build(seed, api, tracked, set(), define=define, small=True)
    folder = os.path.join(ctx.base, f"fixture_{seed}")
    inject.fixture(folder, files, ctx.repo, _api_copy(ctx, tracked), define, profile)
    return folder, expect


def _evolution(ctx, seed):
    """Commits of a tree of its own: what a rename, a move and a split leave in the history."""
    tracked = ctx.repo.files()
    expect = []
    first, second, third = inject_extra.evolved(inject.words(seed + 2), expect)
    encoded = lambda t: {k: v.encode("utf8") if v is not None else None for k, v in t.items()}  # noqa: E731
    folder = os.path.join(ctx.base, f"evolution_{seed}")
    inject.fixture(folder, encoded(first), ctx.repo, _api_copy(ctx, tracked), later=(encoded(second), encoded(third)))
    return folder, expect


def _copy(ctx, name, check, heavy=False, **spec):
    return Job(name, "read_copy", {"path": ctx.path, **spec}, check, heavy)


def injection_jobs(ctx):
    """Every case of every class and cost, in a small repository and in a copy of the real one, with a seed drawn now."""
    seed = _seed()
    small, expect = _fixture(ctx, seed)
    history, changes = _evolution(ctx, seed)
    run0, a0 = ctx.plain()
    left_out = {run0.graph.units[u]["file"] for u in a0["when"].left_out}
    files, cases = inject.build(seed + 1, run0.api, ctx.repo.files(), run0.scope, left_out, inject.define_of_one(ctx.repo))
    ctx.state["seed"] = seed
    return [_copy(ctx, "inject_small", "injection", folder=small, store=False, expect=expect, want=()), _copy(ctx, "inject_history", "injection", folder=history, store=False, expect=changes, want=()),
            _copy(ctx, "inject_real", "injection", view={"add": files, "tag": f"inject{seed}"}, expect=cases, want=())]


def injection(ctx, out):
    both = {}
    for name in ("small", "history", "real"):
        both.update({(name, *k): v for k, v in out(f"inject_{name}")["judged"].items()})
    missed = sorted(k for k, v in both.items() if not v)
    failed = {k[1] for k in missed}
    return {"pass": judge_injection(both), "detail": {"seed": ctx.state["seed"], "cases": len(both), "missed": missed[:80]}, "proved": sorted({k[1] for k in both} - failed)}


def ablation_jobs(ctx):
    """Each adapter off: at least one injected case must go missing, and shared words must not bring it back. Never through a store."""
    folder, expect = _fixture(ctx, _seed())
    plain = dict(folder=folder, store=False, expect=expect, want=())
    return [_copy(ctx, "ablation_none", "ablation", **plain)] + [_copy(ctx, f"ablation_{name}", "ablation", off=(name,), **plain) for name in ADAPTERS]


def ablation(ctx, out):
    full = out("ablation_none")["judged"]
    lost = {name: sorted(k[0] for k, v in out(f"ablation_{name}")["judged"].items() if full[k] and not v) for name in ADAPTERS}
    return {"pass": _ablation_judge(lost), "detail": {k: len(v) for k, v in lost.items()}}


def invariance_jobs(ctx):
    """Every project word and path changed by a bijection: the map, the queue and the roots must come back identical."""
    run, _ = ctx.plain()
    project = set()
    for words in run.roles.tokens.values():
        project |= words
    renamer = Renamer(project, outside_vocabulary(ctx.repo, run.tracked, run.roles, run.api))
    ctx.state["words"] = len(renamer.forward)
    return [_copy(ctx, "renamed", "invariance", heavy=True, store=False, view={"rename": renamer, "tag": "renamed"})]


def invariance(ctx, out):
    from .checks_compare import snapshot
    plain_run, plain = ctx.plain()
    got = out("renamed")
    left, right = snapshot(plain_run, plain), got["snapshot"]
    errors = [len(plain_run.info.get("cpp", {}).get("errors", ())), got["errors"]]
    equal = same(left, right) and errors[0] == errors[1]
    detail = {"words": ctx.state["words"], "groups": (len(left["groups"]), len(right["groups"])), "causes": (len(left["queue"]), len(right["queue"])), "compile_errors": errors}
    if not equal:
        key = lambda q: json.dumps((q["cause"], q["places"]))  # noqa: E731
        la, lb = {key(x) for x in left["queue"]}, {key(x) for x in right["queue"]}
        ga, gb = {tuple(g) for g in left["groups"]}, {tuple(g) for g in right["groups"]}
        detail.update(only_plain=len(la - lb), only_renamed=len(lb - la), groups_differ=(len(ga - gb), len(gb - ga)), same_order=left["order"] == right["order"],
                      sample=(sorted(ga - gb)[:2], sorted(gb - ga)[:2], sorted(la - lb)[:2], sorted(lb - la)[:2]))
    return {"pass": equal, "detail": detail}


def antimakeup_jobs(ctx):
    """Rearranging without separating responsibility must not move any mark."""
    run, _ = ctx.plain()
    ctx.state["views"] = rearrange.views(run)
    return [_copy(ctx, f"makeup_{name}", "antimakeup", view=view, want=("notes",)) for name, view in ctx.state["views"].items()]


def antimakeup(ctx, out):
    from .checks_compare import notes
    base = notes(ctx.plain()[1])
    detail = {}
    for name in ctx.state["views"]:
        marks = out(f"makeup_{name}")["notes"]
        detail[name] = marks == base
        if marks != base:
            changed, added = collections.Counter(base) - collections.Counter(marks), collections.Counter(marks) - collections.Counter(base)
            detail[name + "_diff"] = (sum(changed.values()), sum(added.values()), list(changed)[:3], list(added)[:3])
    return {"pass": bool(detail) and all(v for k, v in detail.items() if not k.endswith("_diff")), "detail": detail}


def incremental_jobs(ctx):
    """What the store gives must be what nothing gives: a copy with files added, a header changed and a unit taken out, read through the store and
    from zero, must come out identical. Without this every shortcut of the run would be a promise."""
    run0, _ = ctx.plain()
    files, _ = inject.build(_seed(), run0.api, ctx.repo.files(), run0.scope, define=inject.define_of_one(ctx.repo))
    code = sorted(p for p in run0.scope if p.endswith(".cpp"))
    headers = sorted(p for p in run0.scope if p.endswith(".h"))
    changed = headers[len(headers) // 2]
    edit = {changed: (run0.texts[changed] + "\ninline int changed_by_the_check() { return 2; }\n").encode("utf8")}
    view = {"add": files, "edit": edit, "drop": {code[len(code) // 2]}, "tag": "both"}
    return [_copy(ctx, "through_store", "incremental", view=view), _copy(ctx, "through_nothing", "incremental", heavy=True, store=False, view=view)]


def incremental(ctx, out):
    fast, full = out("through_store")["snapshot"], out("through_nothing")["snapshot"]
    return {"pass": same(fast, full) and bool(fast["queue"]), "detail": {"causes": (len(fast["queue"]), len(full["queue"])), "groups": (len(fast["groups"]), len(full["groups"]))}}


def proofs(ctx):
    """A finding with no path does not exist: every finding must show a walk that holds together and ends at it."""
    _, plain = ctx.plain()
    bare = [f["unit"] for f in plain["findings"] if not chained(f["path"], f["unit"])]
    return {"pass": bool(plain["findings"]) and not bare, "detail": {"findings": len(plain["findings"]), "without": len(bare), "sample": bare[:5]}}


def _answer_files(tracked, root):
    """Everything the answer keys come from, and the program itself: what a run must not need."""
    from .checks_keys import _load
    sources = _load("sources.json")
    return {p for p in tracked if p.startswith(own_prefixes(root)) or any(p == s or p.startswith(s + "/") for s in sources)}


def contamination_jobs(ctx):
    """A copy without the answer keys, their sources or the program must give what the plain scan gives on everything else."""
    gone = _answer_files(ctx.repo.files(), ctx.repo.root)
    ctx.state["gone"] = gone
    return [_copy(ctx, "without_keys", "contamination", view={"drop": gone, "tag": "clean"}, want=("groups", "causes"))]


def _changed_with(plain, gone):
    """Units that shared a real commit with something dropped: taking a file out takes that evidence with it, and that is no contamination."""
    h = plain["history"]
    cut = giant_cut(h)
    away = {lin.id for lin in h.lineages if any(n[0] in gone for n in lin.names)}
    beside = set()
    for changed in h.commits:
        if len(changed) <= cut and away.intersection(changed):
            beside.update(changed)
    return {u for u, lin in plain["lineage"].items() if lin.id in beside}


def contamination(ctx, out):
    plain_run, plain = ctx.plain()
    gone = ctx.state["gone"]
    got = out("without_keys")
    graph = plain_run.graph
    lost = {u for u, x in graph.units.items() if x["file"] in gone}
    near = lost | {e.src for e in graph.edges.values() if e.dst in lost} | {e.dst for e in graph.edges.values() if e.src in lost}
    near |= _changed_with(plain, gone)
    left = {tuple(sorted(g["members"])) for g in plain["groups"] if not near & set(g["members"])}
    right = {tuple(g) for g in got["groups"] if not near & set(g)}
    causes_a = {(q["id"], q["reach"], q["commits"], q["score"]) for q in plain["queue"] if not near & {p["unit"] for p in q["places"]}}
    causes_b = {c[0] for c in got["causes"] if not near & set(c[1])}
    equal = bool(left) and bool(causes_a) and left == right and causes_a == causes_b
    return {"pass": equal, "detail": {"dropped": len(gone), "units_touching_them": len(near), "groups": (len(left), len(right)), "causes": (len(causes_a), len(causes_b)),
                                      "groups_differ": len(left ^ right), "causes_differ": len(causes_a ^ causes_b), "sample": sorted(left ^ right)[:3]}}


def stability_jobs(ctx):
    """A feature or a few commits taken out: the rest of the map stays. The prototype's best was 0.88."""
    plain_run, plain = ctx.plain()
    base = {m: g["id"] for g in plain["groups"] for m in g["members"]}
    sizes = sorted(plain["groups"], key=lambda g: (-len(g["members"]), g["id"]))
    ordered = [g for g in sizes if len({plain_run.graph.units[m]["file"] for m in g["members"]}) > 1]
    victim = ordered[len(ordered) // 8]
    files = {plain_run.graph.units[m]["file"] for m in victim["members"]}
    alone = {f for f in files if all(base.get(u["id"]) == victim["id"] for u in plain_run.graph.units.values() if u["file"] == f and u["id"] in base)}
    cut = random.Random(0).randint(2, 6)
    ctx.state["taken"] = (len(alone or files), cut)
    return [_copy(ctx, "feature_out", "stability", view={"drop": alone or files, "tag": "nofeature"}, want=("labels",)),
            _copy(ctx, "commits_out", "stability", view={"last_commits_out": cut, "tag": f"past{cut}"}, want=("labels",))]


def stability(ctx, out):
    base = {m: g["id"] for g in ctx.plain()[1]["groups"] for m in g["members"]}
    marks = {name: round(nmi(base, out(name)["labels"]), 4) for name in ("feature_out", "commits_out")}
    return {"pass": all(v > BEST_OF_THE_PROTOTYPE for v in marks.values()), "detail": {**marks, "dropped_files": ctx.state["taken"][0], "commits": ctx.state["taken"][1]}}
