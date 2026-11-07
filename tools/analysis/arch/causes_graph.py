"""Findings form a graph: one explained by another becomes its child, and only a root, the node whose removal makes the others go, is queued."""
import collections
import statistics

import networkx as nx

from .core import digest
from .layers import ENTRY


def _shape(f, atoms):
    """What tells two findings apart whatever anything is called: only an order between equals, far-away change may move it."""
    return digest(f["category"] + "".join(sorted(atoms.signature.get(p[0], "") for p in f["places"])))


def identities(findings):
    """A finding is its kind and its places, and which it is among the same: the id lives as long as the thing, whatever changes around it."""
    seen = collections.Counter()
    out = []
    for f in findings:
        key = f["category"] + "".join(sorted(p[0] for p in f["places"]))
        seen[key] += 1
        out.append(digest(f"{key}{seen[key]}"))
    return out


def explain(findings, idom, atoms):
    """f explains g when every path to g's unit passes through f's, or when g lies among f's places."""
    by_unit = collections.defaultdict(list)
    for i, f in enumerate(findings):
        f.setdefault("places", [(f["unit"], f["file"], f["line"])])
        for uid, _, _ in f["places"]:
            by_unit[uid].append(i)
    g = nx.DiGraph()
    g.add_nodes_from(range(len(findings)))
    for i, f in enumerate(findings):
        # Several places have no first one: what stands above them all is what can explain them, whatever they are called.
        x = common_owner([p[0] for p in f["places"]], idom) if len(f["places"]) > 1 else idom.get(f["unit"])
        seen = set()
        while x is not None and x != ENTRY and x not in seen:
            seen.add(x)
            hits = by_unit.get(x)
            if hits:
                for j in hits:
                    if j != i:
                        g.add_edge(j, i)
                break
            nxt = idom.get(x)
            x = None if nxt == x else nxt
        if len(f["places"]) > 1:
            for uid, _, _ in f["places"]:
                for j in by_unit.get(uid, ()):
                    if j != i and len(findings[j]["places"]) == 1:
                        g.add_edge(i, j)
    return g


def roots(findings, explained):
    """Cycles count as one cause. A root explains others and nothing explains it; alone it is its own root."""
    c = nx.condensation(explained)
    out = []
    descendants = {}
    for n in reversed(list(nx.topological_sort(c))):
        below = set()
        for m in c.successors(n):
            below |= descendants[m] | c.nodes[m]["members"]
        descendants[n] = below
    for n in c:
        if c.in_degree(n) == 0:
            out.append((sorted(c.nodes[n]["members"]), sorted(descendants[n])))
    return out


def common_owner(units, idom):
    """The nearest unit every path to all the places passes through."""
    chains = []
    for u in units:
        chain, x, seen = [], u, set()
        while x in idom and x not in seen:
            seen.add(x)
            chain.append(x)
            x = idom[x]
        chains.append(chain)
    if not chains:
        return None
    common = set(chains[0])
    for c in chains[1:]:
        common &= set(c)
    for x in chains[0]:
        if x in common and x != ENTRY:
            return x
    return None


def owner_of(units, idom, graph, atoms):
    """Who should own a cause: what every path to its places passes through; with no such unit, the place the most others already use."""
    found = common_owner(units, idom)
    if found in graph.units:
        return found, "dominator"
    best = max(units, key=lambda u: (len({e.src for e in graph.into.get(u, ()) if not e.generic}), -atoms.rank.get(u, 0)), default=None)
    return best, "most_used"


def commits_of(h):
    """Per lineage, the commits that changed it."""
    where = collections.defaultdict(list)
    for i, changed in enumerate(h.commits):
        for lineage in changed:
            where[lineage].append(i)
    return where


def cost_in_commits(places, unit_lineage, where):
    """Commits that changed the cause's places together: what it cost so far."""
    lineages = {unit_lineage[u].id for u, _, _ in places if u in unit_lineage}
    if len(lineages) < 2:
        return sum(len(where.get(x, ())) for x in lineages)
    together = collections.Counter(i for x in lineages for i in where.get(x, ()))
    return sum(1 for n in together.values() if n > 1)


def _generators(run):
    """Who writes each file: a cause in made text is fixed where it is made."""
    out = {}
    for c in run.facts.costs:
        if c[1] == "writes_file" and c[6].get("path"):
            out[c[6]["path"].replace("\\", "/").rsplit("/", 1)[-1]] = {"unit": c[0], "file": c[2], "line": c[3]}
    return out


def build_queue(run, findings, explained, idom, atoms, unit_lineage, h):
    where = commits_of(h)
    costs = [cost_in_commits(f["places"], unit_lineage, where) for f in findings]
    # A cause too new to have a past still counts: as what a typical cause cost here, a whole number the sample gives and a few moved lines cannot shift.
    prior = statistics.median_low([c for c in costs if c] or [1])
    ids = identities(findings)
    makers = _generators(run)
    queue = []
    graph = run.graph
    for members, below in roots(findings, explained):
        head = findings[min(members, key=lambda m: (findings[m]["category"], _shape(findings[m], atoms)))]
        places = sorted({tuple(p) for m in members for p in findings[m]["places"]}, key=lambda p: (p[1], p[2], p[0]))
        spent = max(costs[m] for m in members)
        children = sorted((findings[b]["category"], sorted(p[0] for p in findings[b]["places"])) for b in below)
        generated = run.roles.generated
        owner, reason = owner_of([p[0] for p in places], idom, graph, atoms)
        owner_unit = graph.units.get(owner) if owner else None
        queue.append({
            "id": min(ids[m] for m in members), "shape": _shape(head, atoms), "cause": head["category"], "context": head["context"], "detail": head["detail"],
            "proof": head["path"], "entry": head["root"], "places": [{"unit": p[0], "file": p[1], "line": p[2]} for p in places],
            "owner": {"unit": owner, "file": owner_unit["file"], "line": owner_unit["line"], "by": reason} if owner_unit else None,
            "generated": sorted({p[1] for p in places if p[1] in generated}), "made_by": [makers[n] for n in sorted({p[1].rsplit("/", 1)[-1] for p in places}) if n in makers], "vanish": sorted({findings[m]["category"] for m in members}),
            "children": children, "commits": spent, "reach": 1 + len(children), "score": (1 + len(children)) * (spent + prior), "ran": head.get("ran"),
        })
    queue.sort(key=lambda q: (-q["score"], -q["reach"], q["shape"], q["id"]))
    return queue
