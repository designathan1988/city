"""What dropping files may touch, and the map and the causes with that taken out: what `contamination` compares."""
from ..history import giant_cut


def touched_by(run, plain, gone, cut_after):
    """What the dropped files really touch: their units, what links to them, what a name of theirs could mean, and what changed in the same commits."""
    graph = run.graph
    lost = {u for u, x in graph.units.items() if x["file"] in gone}
    near = lost | {e.src for e in graph.edges.values() if e.dst in lost} | {e.dst for e in graph.edges.values() if e.src in lost}
    named = {graph.units[u]["name"] for u in lost} | {name for _, name, unit in run.facts.defs if unit in lost}
    near |= {x for e in graph.edges.values() if e.attrs.get("text") in named for x in (e.src, e.dst)}
    h = plain["history"]
    cut = giant_cut(h)
    away = {lin.id for lin in h.lineages if any(n[0] in gone for n in lin.names)}
    beside = set()
    for changed in h.commits:
        left = len(changed) - len(away.intersection(changed))
        # A commit counts while it is under the cut: one the drop shrinks below it counts now, and the cut itself is the sample's and moves with it.
        if (len(changed) != left and left <= max(cut, cut_after)) or min(cut, cut_after) < len(changed) <= max(cut, cut_after):
            beside.update(changed)
    return near | {u for u, lin in plain["lineage"].items() if lin.id in beside}


def groups_apart(groups, near):
    """A map with what the dropped files touched taken out of it: what remains must not have moved. A group left with one unit is no group."""
    kept = (tuple(sorted(set(g) - near)) for g in groups)
    return {g for g in kept if len(g) > 1}


def marks_apart(causes, near):
    """Every mark of each cause that touches nothing dropped: its commits, its reach and its score, children the dropped files touched not counted in either."""
    out = set()
    for i, commits, reach, score, places, children in causes:
        if not near & set(places):
            lost = sum(1 for _, units in children if near & set(units))
            out.add((i, commits, reach - lost, score - lost * (score // reach)))
    return out
