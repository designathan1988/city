"""The stages after reading: joined history and recording, the feature map, layers, every detector, the cause graph and the queue."""
import collections
import os

from . import atoms as atoms_stage
from . import causes_graph, classes, classes_more, context, costs, costs_more, design, groups, history, history_units, layers, points, profile_data, workers
from .core import read_json, write_json


def analyse(run):
    h = history.build(run.repo, run.history, set(run.roles.foreign) | run.roles.binary | run.roles.sidecar | run.roles.index, run.cache, run.log)
    workers.close()
    recorded = profile_data.read(run)
    atoms = atoms_stage.Atoms(run)
    unit_lineage = history_units.attach(run, h)
    history_units.add_cochange(atoms, unit_lineage, history.cochange(h))
    found, group_of, tree, upper = groups.discover(atoms)
    run.log(f"{len(found)} groups over {len(group_of)} units")
    layer_graph = layers.condensed(run.graph, atoms, group_of)
    layer_of = layers.layer_of(layer_graph)
    when = context.When(run)
    run.log(f"repeating: {len(when.repeats)} units; test only: {len(when.tests)}")
    profile_data.rate(run, recorded, when, unit_lineage)
    k = costs.Costs(run, when, atoms)
    churn = {u: len(lin.touched) for u, lin in unit_lineage.items()}
    findings = []
    table = []
    for catalogue in (costs.CATALOGUE, costs_more.CATALOGUE):
        for category, title, detector in catalogue:
            out = detector(k)
            table.append({"category": category, "title": title, "found": len(out), "covered": True})
            findings += out
    for category, title, out in (("scattered", "same responsibility in several places", design.scattered(k, group_of, layer_of)),
                                 ("god", "god by responsibility", design.gods(k, group_of, churn)), ("missing_owner", "owner that is missing", design.missing_owner(k, group_of))):
        table.append({"category": category, "title": title, "found": len(out), "covered": True})
        findings += out
    run.log(f"{len(findings)} findings")
    roots = layers.roots_of(run) | when.roots["frame"] | when.roots["boot"] | when.roots["event"] | when.roots["thread"]
    flow, idom = layers.dominators(run.graph, roots)
    explained = causes_graph.explain(findings, idom, atoms)
    queue = causes_graph.build_queue(run, findings, explained, idom, atoms, unit_lineage, h)
    result = {"history": h, "recorded": recorded, "atoms": atoms, "groups": found, "group_of": group_of, "tree": tree, "upper": upper, "layer_of": layer_of, "when": when, "findings": findings,
            "table": table, "queue": queue, "flow": flow, "idom": idom, "lineage": unit_lineage, "layer_graph": layer_graph}
    result["roles_of"] = roles_of(run, group_of)
    return result


def roles_of(run, group_of):
    """Each unit's place among the others: nobody else uses it, others use it, it uses others, many use it, nearly everyone uses it."""
    users = collections.defaultdict(set)
    used = set()
    for e in run.graph.edges.values():
        a, b = group_of.get(e.src), group_of.get(e.dst)
        if a is not None and b is not None and a != b and e.conf == 1.0 and not e.generic:
            users[e.dst].add(a)
            used.add(e.src)
    counts = [len(v) for v in users.values()]
    cut = min(design.tail_cut(counts), max(counts, default=0))  # a sample with no tail still has what most use
    return {"own": sorted(u for u in group_of if u not in users), "used": sorted(users), "uses": sorted(used), "shared": sorted(u for u, v in users.items() if len(v) > 1),
            "infrastructure": sorted(u for u, v in users.items() if len(v) >= cut)}


def _group_rows(run, a):
    rows = []
    for g in a["groups"]:
        files = collections.Counter(run.graph.units[m]["file"] for m in g["members"])
        rows.append({"id": g["id"], "words": g["words"], "units": len(g["members"]), "layer": a["layer_of"].get(g["id"]), "files": dict(files.most_common()), "members": g["members"]})
    return rows


def publish(run, a):
    """Everything status, next and explain read. No time, no absolute path: two runs give the same bytes."""
    own = os.path.normcase(run.repo.root) == os.path.normcase(run.home) or os.environ.get("ARCH_OUT")
    base = run.base if own else os.path.join(run.base, "repos", run.key)
    totals, residue = points.account(run)
    seen = classes.Seen(run, a)
    found = [{"id": cid, "title": title, "count": len(fn(seen))} for cid, title, fn in (*classes.link_classes(), *classes_more.more_classes())]
    write_json(os.path.join(base, "queue.json"), a["queue"])
    write_json(os.path.join(base, "residual.json"), residue)
    write_json(os.path.join(base, "groups.json"), _group_rows(run, a))
    write_json(os.path.join(base, "levels.json"), groups.describe_levels(a["tree"], a["upper"], a["atoms"]))
    write_json(os.path.join(base, "atoms.json"), sorted(a["atoms"].nodes))
    write_json(os.path.join(base, "coverage.json"), {"table": a["table"], "classes": found, "points": totals, "units": len(run.graph.units), "edges": len(run.graph.edges),
                                                     "roles": run.roles.describe(), "recorded": {k: v if not isinstance(v, list) else len(v) for k, v in a["recorded"].items()},
                                                     "adapters": {k: v for k, v in run.info.items() if k != "refused"}, "commit": run.repo.head})
    done = read_json(os.path.join(base, "done.json"), {})
    return base, done
