"""History joined to today's units: a lineage alive at the commit becomes the unit with its name in its file, and carries its changes."""
import collections


def attach(run, h):
    """unit id -> lineage. A unit with no lineage was never seen changing: it has no past, which is not the same as no cost."""
    by_file = collections.defaultdict(lambda: collections.defaultdict(list))
    for uid, u in run.graph.units.items():
        by_file[u["file"]][u["name"]].append(uid)
    out = {}
    for (path, name), lin in h.alive.items():
        table = by_file.get(path)
        if table is None:
            continue
        if path.endswith(".json"):
            uid = f"json:{path}#{name}"
            if uid in run.graph.units:
                out[uid] = lin
            continue
        if path.endswith(".csv"):
            uid = f"csv:{path}#{name}"
            if uid in run.graph.units:
                out[uid] = lin
            continue
        short = name.split("::")[-1].split(".")[-1]
        for uid in table.get(short, ()):
            out[uid] = lin
    return out


def add_cochange(atoms, unit_of_lineage, pairs):
    """Commits that changed two units together are evidence they belong together, with the weight the commits give it."""
    back = collections.defaultdict(list)
    for uid, lin in unit_of_lineage.items():
        back[lin.id].append(uid)
    for (a, b), w in pairs.items():
        for x in back.get(a, ()):
            for y in back.get(b, ()):
                atoms.add(x, y, w)
