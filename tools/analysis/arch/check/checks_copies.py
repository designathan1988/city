"""Checks that compare copies with what was read. Each property is proved once, where it is cheapest and still proves it: end to end on the small repository,
which holds every class of link and cost and healthy code beside each; and on the graph of the real one, in memory, which is read a single time."""
import collections
import json
import os
import random
import types

from .. import external
from ..core import IDENT, own_prefixes
from ..scan import ADAPTERS
from . import inject, inject_extra, rearrange
from .checks_compare import _ablation_judge, chained, nmi, notes, same, snapshot
from .rename import Renamer, outside_vocabulary
from .apart import groups_apart, marks_apart, touched_by
from .supervisor import Job

BEST_OF_THE_PROTOTYPE = 0.88


def judge_injection(found):
    return bool(found) and all(found.values())


def _seed():
    return int.from_bytes(os.urandom(4), "big")


def _api_copy(ctx, tracked):
    picked = next((p for p in sorted(tracked) if p.endswith(".json") and "api" in p.rsplit("/", 1)[-1] and p.rsplit("/", 1)[-1].startswith("extension")), None)
    if not picked:
        return None
    source = os.path.join(ctx.base, "api.json")
    if not os.path.exists(source):
        with open(source, "wb") as f:
            f.write(ctx.repo.blob(tracked[picked]))
    return source


def small_repository(ctx):
    """The small repository of this run, its cases drawn from a seed of this run, built once for every check that reads it."""
    if "small" not in ctx.state:
        seed = _seed()
        tracked = ctx.repo.files()
        api = external.load(ctx.repo, tracked)
        define = f"ONLY_{seed}"
        files, expect, profile = inject.build(seed, api, tracked, set(), define=define, small=True)
        folder = os.path.join(ctx.base, "small")
        inject.fixture(folder, files, ctx.repo, _api_copy(ctx, tracked), define, profile)
        changes = []
        first, second, third = inject_extra.evolved(inject.words(seed + 2), changes)
        encoded = lambda t: {k: v.encode("utf8") if v is not None else None for k, v in t.items()}  # noqa: E731
        evolving = os.path.join(ctx.base, "evolving")
        inject.fixture(evolving, encoded(first), ctx.repo, _api_copy(ctx, tracked), later=(encoded(second), encoded(third)))
        texts = {k: v.decode("utf8", "replace") for k, v in files.items()}
        ctx.state["small"] = types.SimpleNamespace(seed=seed, folder=folder, expect=expect, evolving=evolving, changes=changes, texts=texts, api=api, tracked=tracked)
    return ctx.state["small"]


def _on_small(ctx, name, check, env=None, **spec):
    s = small_repository(ctx)
    return Job(name, "read_copy", {"folder": s.folder, "store": False, "out": os.path.join(ctx.base, "small_out"), **spec}, check, env=env)


def _on_graph(ctx, name, check, step, **spec):
    return Job(name, "graph_steps", {"path": ctx.path, "run": ctx.frozen, "step": step, **spec}, check)


def injection_jobs(ctx):
    """Every case of every class and cost, on the small repository, with a seed drawn now; healthy code beside each case must stay quiet."""
    s = small_repository(ctx)
    return [_on_small(ctx, "small_cases", "injection", expect=s.expect, want=("snapshot", "facts"), publish=os.path.join(ctx.base, "reports_fast"), env={"ARCH_PCH": "1", "PYTHONHASHSEED": "11"}),
            Job("small_history", "read_copy", {"folder": s.evolving, "store": False, "out": os.path.join(ctx.base, "small_out"), "expect": s.changes, "want": ()}, "injection")]


# The two that a case cannot be without a real corpus: images and shaders are weighed against the repository's own, and a feature is a place on its map.
AGAINST_THE_SAMPLE = ("feature", "texture_bandwidth", "not:texture_bandwidth", "shader_cost")


def injection_on_graph(ctx):
    """The same cases in the middle of the real graph: found against the real distribution, not only among themselves."""
    s = small_repository(ctx)
    weighed = [c for c in s.expect if not (c[0][0] in "BD" and c[0][1:].isdigit())]
    weighed = [c for c in weighed if c[0] not in AGAINST_THE_SAMPLE]
    findings = ctx.plain()[1]["findings"]
    top = lambda kind: max((f for f in findings if f["category"] == kind), key=lambda f: (len(f["places"]), f["unit"]))  # noqa: E731
    heavy = {"god": top("god")["unit"], "decision": top("repeated_decision")["unit"], "messages": max(f["detail"]["messages"] for f in findings if f["category"] == "implicit_role") + 1}
    words = {c[0]: c[1] for c in s.expect if c[0] in ("god", "repeated_decision", "implicit_role")}
    return [_on_graph(ctx, "graph_cases", "injection_graph", "injected", facts=os.path.join(ctx.base, "small_facts.pkl"), folder=s.folder, expect=weighed, heavy=heavy, words=words)]


def injection(ctx, out):
    both = {}
    for name in ("small_cases", "small_history"):
        both.update({(name, *k): v for k, v in out(name)["judged"].items()})
    missed = sorted(k for k, v in both.items() if not v)
    s = ctx.state["small"]
    return {"pass": judge_injection(both), "detail": {"seed": s.seed, "cases": len(both), "healthy": sum(1 for k in both if k[1].startswith("not:")), "missed": missed[:80]}, "proved": sorted({k[1] for k in both} - {k[1] for k in missed})}


def injection_graph(ctx, out):
    found = out("graph_cases")["judged"]
    missed = sorted(k for k, v in found.items() if not v)
    return {"pass": judge_injection(found), "detail": {"cases": len(found), "missed": missed[:40]}}


def ablation_jobs(ctx):
    """Each adapter off: at least one case must go missing, and shared words must not bring it back."""
    s = small_repository(ctx)
    return [_on_small(ctx, "ablation_none", "ablation", expect=s.expect, want=())] + [_on_small(ctx, f"ablation_{name}", "ablation", off=(name,), expect=s.expect, want=()) for name in ADAPTERS]


def ablation(ctx, out):
    full = out("ablation_none")["judged"]
    lost = {name: sorted(k[0] for k, v in out(f"ablation_{name}")["judged"].items() if full[k] and not v) for name in ADAPTERS}
    return {"pass": _ablation_judge(lost), "detail": {k: len(v) for k, v in lost.items()}}


def invariance_jobs(ctx):
    """Every word and path changed by a bijection: read end to end on the small repository, whose text is renamed and compiled again."""
    s = small_repository(ctx)
    project = set()
    for text in s.texts.values():
        project |= set(IDENT.findall(text))
    no_roles = types.SimpleNamespace(foreign=(), tokens={})
    renamer = Renamer(project, outside_vocabulary(ctx.repo, s.tracked, no_roles, s.api))
    return [_on_small(ctx, "small_plain", "invariance"), _on_small(ctx, "small_renamed", "invariance", view={"rename": renamer, "tag": "renamed"})]


def invariance(ctx, out):
    a, b = out("small_plain"), out("small_renamed")
    equal = same(a["snapshot"], b["snapshot"]) and a["errors"] == b["errors"]
    return {"pass": equal and bool(a["snapshot"]["queue"]), "detail": {"causes": (len(a["snapshot"]["queue"]), len(b["snapshot"]["queue"])), "compile_errors": (a["errors"], b["errors"]), **_apart(a["snapshot"], b["snapshot"])}}


def invariance_on_graph(ctx):
    return [_on_graph(ctx, "graph_renamed", "invariance_graph", "renamed")]


def invariance_graph(ctx, out):
    plain_run, plain = ctx.plain()
    left, got = snapshot(plain_run, plain), out("graph_renamed")
    right = got["snapshot"]
    return {"pass": same(left, right) and bool(left["queue"]), "detail": {"words": got["words"], "groups": (len(left["groups"]), len(right["groups"])), "causes": (len(left["queue"]), len(right["queue"])), **_apart(left, right)}}


def _apart(a, b):
    """What two snapshots disagree on, to be read."""
    key = lambda q: json.dumps((q["cause"], q["places"]))  # noqa: E731
    la, lb = {key(x) for x in a["queue"]}, {key(x) for x in b["queue"]}
    ga, gb = {tuple(g) for g in a["groups"]}, {tuple(g) for g in b["groups"]}
    return {"only_first": sorted(la - lb)[:2], "only_second": sorted(lb - la)[:2], "groups_apart": (len(ga - gb), len(gb - ga)), "same_order": a["order"] == b["order"], "group_sample": sorted(ga ^ gb)[:2]}


def antimakeup_jobs(ctx):
    """Rearranging without separating responsibility must not move any mark."""
    s = small_repository(ctx)
    tokens = {p: set(IDENT.findall(t)) for p, t in s.texts.items()}
    shape = types.SimpleNamespace(scope=set(s.texts), texts=s.texts, roles=types.SimpleNamespace(tokens=tokens))
    ctx.state["views"] = rearrange.views(shape)
    return [_on_small(ctx, "small_plain_marks", "antimakeup", want=("notes",))] + [_on_small(ctx, f"makeup_{name}", "antimakeup", view=view, want=("notes",)) for name, view in ctx.state["views"].items()]


def _marks_moved(base, marks):
    changed, added = collections.Counter(base) - collections.Counter(marks), collections.Counter(marks) - collections.Counter(base)
    return sum(changed.values()), sum(added.values()), list(changed)[:3], list(added)[:3]


def antimakeup(ctx, out):
    base = out("small_plain_marks")["notes"]
    detail = {}
    for name in ctx.state["views"]:
        marks = out(f"makeup_{name}")["notes"]
        detail[name] = marks == base
        if marks != base:
            detail[name + "_diff"] = _marks_moved(base, marks)
    return {"pass": bool(detail) and bool(base) and all(v for k, v in detail.items() if not k.endswith("_diff")), "detail": detail}


def antimakeup_on_graph(ctx):
    return [_on_graph(ctx, "graph_rearranged", "antimakeup_graph", "rearranged")]


def antimakeup_graph(ctx, out):
    base, marks = notes(ctx.plain()[1]), out("graph_rearranged")["notes"]
    return {"pass": marks == base and bool(base), "detail": {"marks": len(base), "moved": _marks_moved(base, marks) if marks != base else 0}}


def incremental_jobs(ctx):
    """What the store gives must be what nothing gives: a copy with a file added, a header changed and a unit taken out. Without this every shortcut is a promise."""
    s = small_repository(ctx)
    header = next(p for p in sorted(s.texts) if p.endswith("_more.h"))
    gone = next(p for p in sorted(s.texts) if p.endswith("_made.cpp"))
    added = {header.rsplit("/", 1)[0] + "/added_by_the_check.cpp": b"// Added by the check.\nint added_by_the_check() { return 1; }\n"}
    edit = {header: (s.texts[header] + "\ninline int changed_by_the_check() { return 2; }\n").encode("utf8")}
    view = {"add": added, "edit": edit, "drop": {gone}, "tag": "both"}
    return [Job("store_and_nothing", "incremental", {"folder": s.folder, "view": view, "out": os.path.join(ctx.base, "small_store")}, "incremental", env={"ARCH_PCH": "1"})]


def incremental(ctx, out):
    fast, full = out("store_and_nothing")
    a, b = fast["trace"], full["trace"]
    edges = sorted(u for u in set(a["edges"]) | set(b["edges"]) if a["edges"].get(u) != b["edges"].get(u))
    units = sorted(u for u in set(a["units"]) | set(b["units"]) if a["units"].get(u) != b["units"].get(u))
    return {"pass": same(fast["snapshot"], full["snapshot"]) and not edges and not units and bool(fast["snapshot"]["queue"]),
            "detail": {"causes": (len(fast["snapshot"]["queue"]), len(full["snapshot"]["queue"])), "units_apart": units[:4], "edges_apart": edges[:4], **_apart(fast["snapshot"], full["snapshot"])}}


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
    """Without the answer keys, their sources or the program, the rest must come out the same."""
    ctx.state["gone"] = _answer_files(ctx.repo.files(), ctx.repo.root)
    return [_on_graph(ctx, "graph_clean", "contamination", "clean", gone=sorted(ctx.state["gone"]))]


def contamination(ctx, out):
    plain_run, plain = ctx.plain()
    gone = ctx.state["gone"]
    got = out("graph_clean")
    touched = touched_by(plain_run, plain, gone | set(got["applied"]), got["cut"])
    left, right = groups_apart((g["members"] for g in plain["groups"]), touched), groups_apart(got["groups"], touched)
    mine = [(q["id"], q["commits"], q["reach"], q["score"], [p["unit"] for p in q["places"]], q["children"]) for q in plain["queue"]]
    causes_a, causes_b = marks_apart(mine, touched), marks_apart(got["causes"], touched)
    equal = bool(left) and bool(causes_a) and left == right and causes_a == causes_b and not got["roles_back"]
    return {"pass": equal, "detail": {"dropped": len(gone), "applied": len(got["applied"]), "units": len(plain_run.graph.units), "units_touching_them": len(touched), "groups": (len(left), len(right)), "causes": (len(causes_a), len(causes_b)),
                                      "groups_differ": len(left ^ right), "causes_differ": len(causes_a ^ causes_b), "sample": sorted(left ^ right)[:3], "causes_sample": sorted(causes_a ^ causes_b, key=str)[:3], "roles_back": got["roles_back"][:5]}}


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
    return [_on_graph(ctx, "graph_feature", "stability", "feature_out", gone=sorted(alone or files)), _on_graph(ctx, "graph_commits", "stability", "commits_out", cut=cut)]


def stability(ctx, out):
    base = {m: g["id"] for g in ctx.plain()[1]["groups"] for m in g["members"]}
    marks = {name: round(nmi(base, out(name)["labels"]), 4) for name in ("graph_feature", "graph_commits")}
    return {"pass": all(v > BEST_OF_THE_PROTOTYPE for v in marks.values()), "detail": {**marks, "dropped_files": ctx.state["taken"][0], "commits": ctx.state["taken"][1]}}


__all__ = ["injection", "ablation", "invariance", "antimakeup", "incremental", "proofs", "contamination", "stability"]
