"""Properties of the analysis proved on the real repository without reading it again: the graph of the one scan of the run, transformed in memory and
analysed. Reading is proved end to end on the small repository; what depends on the size and shape of the real one is proved here."""
import copy
import os
import pickle

from .. import history, link, profile_data, roles
from ..cache import Cache
from ..core import IDENT, program_root
from ..model import Facts
from ..pipeline import analyse, ignored, publish
from ..repo import Repo
from . import inject, rearrange
from .checks_compare import labels, notes, snapshot
from .rename import Renamer, outside_vocabulary
from .view import View

OWN = (".godot", ".cfg")


def load(spec):
    with open(spec["run"], "rb") as f:
        run = pickle.load(f)
    run.repo = Repo(spec["path"])
    run.cache = Cache(os.path.join(program_root(), "dist", "arch", "cache"), True)
    run.graph, run.names, run.report = link.build(run.facts)
    h = history.build(run.repo, run.history, ignored(run), run.cache, run.log)
    return run, h, profile_data.read(run)


def copy_of(run, facts, name=lambda s: s):
    """The run with other facts, and what the recording said carried over to the units that remain."""
    c = copy.copy(run)
    c.facts = facts
    c.graph, c.names, c.report = link.build(facts)
    for uid, u in run.graph.units.items():
        if "ran" in u["attrs"] and name(uid) in c.graph.units:
            c.graph.units[name(uid)]["attrs"]["ran"] = u["attrs"]["ran"]
    for e in run.graph.edges.values():
        if e.kind == "recorded_call" and name(e.src) in c.graph.units and name(e.dst) in c.graph.units:
            c.graph.add_edge(name(e.src), name(e.dst), e.kind, "recorded", 1.0, name(e.sites[0][0]), e.sites[0][1], dict(e.attrs))
    c.graph.index_children()
    return c


def _map_facts(facts, m):
    """Every name in the facts through m; what a fact is, its kind and role, stays."""
    def deep(v):
        if isinstance(v, str):
            return m(v)
        if isinstance(v, (list, tuple)):
            return type(v)(deep(x) for x in v)
        if isinstance(v, dict):
            return {k: deep(x) for k, x in v.items()}
        return v

    out = Facts()
    out.units = [(m(u[0]), u[1], m(u[2]), m(u[3]), u[4], u[5], m(u[6]) if u[6] else u[6], u[7], deep(u[8])) for u in facts.units]
    out.edges = [(m(e[0]), e[1], m(e[2]), m(e[3]), e[4], e[5], deep(e[6])) for e in facts.edges]
    out.defs = [(d[0], m(d[1]), m(d[2])) for d in facts.defs]
    out.refs = [(r[0], m(r[1]), m(r[2]), m(r[3]), r[4], r[5]) for r in facts.refs]
    out.sites = [(m(s[0]), s[1], s[2], s[3]) for s in facts.sites]
    out.costs = [(m(c[0]), c[1], m(c[2]), c[3], c[4], deep(c[5]), deep(c[6])) for c in facts.costs]
    return out


def _history_through(h, m):
    out = copy.copy(h)
    out.alive = {(m(p), m(n)): lin for (p, n), lin in h.alive.items()}
    return out


def _with_roles(c, m=lambda s: s, gone=()):
    """Tracked files, scope and roles under other names, or without some files."""
    keep = lambda paths: {m(p) for p in paths if p not in gone}  # noqa: E731
    c.tracked = {m(p): b for p, b in c.tracked.items() if p not in gone}
    c.scope = keep(c.scope)
    found = copy.copy(c.roles)
    for name in ("binary", "sidecar", "foreign", "generated", "index"):
        setattr(found, name, keep(getattr(found, name)))
    c.roles = found


def _deep_text(v, m):
    if isinstance(v, str):
        return m(v)
    if isinstance(v, (list, tuple)):
        return [_deep_text(x, m) for x in v]
    if isinstance(v, dict):
        return {k: _deep_text(x, m) for k, x in v.items()}
    return v


def renamed(run, h, recorded, spec):
    """Every project word changed by a bijection, in the graph itself."""
    project = set()
    for words in run.roles.tokens.values():
        project |= words
    r = Renamer(project, outside_vocabulary(run.repo, run.tracked, run.roles, run.api))
    memo = {}

    def m(s):
        if s not in memo:
            memo[s] = IDENT.sub(lambda x: r.token(x.group(0)), s)
        return memo[s]

    c = copy_of(run, _map_facts(run.facts, m), m)
    _with_roles(c, m)
    c.info = _deep_text(run.info, m)
    a = analyse(c, _history_through(h, m), recorded)
    return {"snapshot": snapshot(c, a, r.undo), "words": len(r.forward)}


def rearranged(run, h, recorded, spec):
    """A function renamed, moved to a file of its own, with a shim left behind: nothing but the arrangement changes."""
    path, _, _, _, name = rearrange._script_with_functions(run)
    old = f"gd:{path}#{name}"
    target = path[:-len(".gd")] + "_piece.gd"
    fresh = f"gd:{target}#{name}_again"

    def m(s):
        if s == old or s.startswith(old + "#"):
            return fresh + s[len(old):]
        return s[:-len(name)] + name + "_again" if s == "name:method:" + name else name + "_again" if s == name else s

    moved = _map_facts(run.facts, m)
    moved.units = [(*u[:3], target if u[0].startswith(fresh) else u[3], *u[4:]) for u in moved.units]
    moved.edges = [(*e[:3], target if e[0].startswith(fresh) else e[3], *e[4:]) for e in moved.edges]
    home = next(u for u in run.facts.units if u[0] == old)
    moved.unit(f"gd:{path}#{name}_shim", "method", name + "_shim", path, home[5] + 1, home[5] + 2, home[6], "", statements=1)
    moved.edge(f"gd:{path}#{name}_shim", "call", fresh, path, home[5] + 2, "grammar")
    c = copy_of(run, moved, m)
    shifted = copy.copy(h)
    shifted.alive = {((target, name + "_again") if key == (path, name) else key): lin for key, lin in h.alive.items()}
    return {"notes": notes(analyse(c, shifted, recorded))}


def injected(run, h, recorded, spec):
    """The cases of the small repository in the middle of the real graph: found against the real distribution, and the healthy ones still quiet."""
    with open(spec["facts"], "rb") as f:
        small = pickle.load(f)
    merged = Facts()
    for name in Facts.__slots__:
        setattr(merged, name, list(getattr(run.facts, name)))
    outside = {u[3] for u in small.units if u[3].endswith(OWN)}
    merged.units += [u for u in small.units if u[3] not in outside]
    merged.edges += [e for e in small.edges if e[3] not in outside]
    merged.defs += small.defs
    merged.refs += [r for r in small.refs if r[3] not in outside]
    merged.costs += small.costs
    merged.sites += small.sites
    c = copy_of(run, merged)
    c.texts = dict(run.texts)
    for file in sorted({u[3] for u in small.units} - outside):
        path = os.path.join(spec["folder"], file)
        if os.path.isfile(path):
            with open(path, encoding="utf8", errors="replace") as f:
                c.texts[file] = f.read()
    a = analyse(c, h, recorded)
    return {"judged": inject.judge(c, a, spec["expect"])}


def _without(facts, gone):
    lost = {u[0] for u in facts.units if u[3] in gone}
    out = Facts()
    out.units = [u for u in facts.units if u[3] not in gone]
    out.edges = [e for e in facts.edges if e[3] not in gone and e[0] not in lost]
    out.defs = [d for d in facts.defs if d[2] not in lost]
    out.refs = [r for r in facts.refs if r[3] not in gone]
    out.sites = [s for s in facts.sites if s[0] not in gone]
    out.costs = [c for c in facts.costs if c[2] not in gone]
    return out


def _past_without(run, gone):
    return [(sha, [c for c in changes if c[1] not in gone]) for sha, changes in run.history]


def feature_out(run, h, recorded, spec):
    gone = set(spec["gone"])
    c = copy_of(run, _without(run.facts, gone))
    _with_roles(c, gone=gone)
    c.history = _past_without(run, gone)
    return {"labels": labels(analyse(c, history.build(run.repo, c.history, ignored(c), run.cache, run.log), recorded))}


def commits_out(run, h, recorded, spec):
    c = copy.copy(run)
    c.history = run.history[:-spec["cut"]]
    return {"labels": labels(analyse(c, history.build(run.repo, c.history, ignored(c), run.cache, run.log), recorded))}


def clean(run, h, recorded, spec):
    """Without the keys and what they come from. Roles are decided again on the smaller repository; the rest follows from the facts that remain."""
    gone = set(spec["gone"])
    c = copy_of(run, _without(run.facts, gone))
    past = _past_without(run, gone)
    view = View(run.repo, drop=gone, tag="clean")
    found, _ = roles.classify(view, {p: b for p, b in run.tracked.items() if p not in gone}, past, run.cache)
    moved = {n: sorted(set(getattr(found, n)) ^ (set(getattr(run.roles, n)) - gone)) for n in ("foreign", "index", "generated", "sidecar", "binary")}
    _with_roles(c, gone=gone)
    c.history = past
    h_clean = history.build(run.repo, past, ignored(c), run.cache, run.log)
    a = analyse(c, h_clean, recorded)
    return {"cut": history.giant_cut(h_clean), "groups": [sorted(g["members"]) for g in a["groups"]], "causes": [(q["id"], q["commits"], q["reach"], q["score"], [p["unit"] for p in q["places"]], q["children"]) for q in a["queue"]],
            "roles_moved": {k: v for k, v in moved.items() if v}}


def second_seed(run, h, recorded, spec):
    """The analysis of the same facts in another process, under another hash seed: the same bytes as the reports of the scan."""
    os.environ["ARCH_OUT"] = spec["out"]
    run.base = spec["out"]
    run.home = os.path.normpath(program_root())
    run.key = ""
    publish(run, analyse(run, h, recorded))
    return True


STEPS = {"renamed": renamed, "rearranged": rearranged, "injected": injected, "feature_out": feature_out, "commits_out": commits_out, "clean": clean, "second_seed": second_seed}


def graph_steps(spec):
    run, h, recorded = load(spec)
    return STEPS[spec["step"]](run, h, recorded, spec)
