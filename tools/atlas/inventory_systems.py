"""The systems of the two games paired through the features they hold or use (tools/atlas/inventory.py): a system (a code
file of the reference; a native module or a script of this game) holds the features written in its files and uses the text
keys its files write. Two systems pair when features they hold or use are paired: how many such pairs they share, weighed by
the size of each (the cosine of their feature sets, so a hub that holds every command pairs nothing on that alone), kept when
they share at least MIN_SHARED and weigh at least MIN_SCORE, and each is among the other's BEST. The evidence says through
which kinds of features (`via:text`, `via:action`, `via:data`). Rules only: no system of either game is named here."""
import math
import os

MIN_SHARED = 2      # paired features two systems share at the least (one shared text is a coincidence)
MIN_SCORE = 0.08    # their weight at the least: shared / sqrt(|features of one| * |features of the other|)
BEST = 3            # the most partners a system keeps, the best weighed first
VIA = "via:"        # the evidence's prefix: the kind of the paired features two systems share


def place(source):
    """A source's file without its extension: the one key a module's header and body share."""
    return os.path.splitext(source.rsplit(":", 1)[0])[0].replace("\\", "/")


def systems_of(features):
    """Each game's systems by the place of their files: {(game, place): system id}."""
    return {(f["game"], place(f["source"])): f["id"] for f in features if f["kind"] == "system"}


def held_and_used(f, systems):
    """The systems a feature is held by (the file it is written in) or used by (a file writing its key)."""
    places = [f["source"]] + list(f.get("uses") or ())
    return {systems[(f["game"], place(s))] for s in places if s and (f["game"], place(s)) in systems}


def pair_systems(features, pairs):
    """The pairs of systems the feature pairs make: [{"reference", "city", "evidence", "shared", "score"}]."""
    systems = systems_of(features)
    by_id = {f["id"]: f for f in features}
    owners, sizes = {}, {}
    for f in features:
        if f["kind"] in ("system", "test"):
            continue
        owners[f["id"]] = held_and_used(f, systems)
        for s in owners[f["id"]]:
            sizes[s] = sizes.get(s, 0) + 1
    shared, kinds = {}, {}
    for p in pairs:
        kind = by_id[p["reference"]]["kind"]
        for sr in owners.get(p["reference"], ()):
            for sc in owners.get(p["city"], ()):
                shared[(sr, sc)] = shared.get((sr, sc), 0) + 1
                kinds.setdefault((sr, sc), set()).add(kind)
    scored = {k: n / math.sqrt(sizes[k[0]] * sizes[k[1]]) for k, n in shared.items()
              if n >= MIN_SHARED and n / math.sqrt(sizes[k[0]] * sizes[k[1]]) >= MIN_SCORE}
    best = {}
    for (sr, sc), score in scored.items():
        best.setdefault(sr, []).append((score, sc))
        best.setdefault(sc, []).append((score, sr))
    keep = {s: {o for _, o in sorted(found, reverse=True)[:BEST]} for s, found in best.items()}
    return [{"reference": sr, "city": sc, "evidence": [VIA + k for k in sorted(kinds[(sr, sc)])],
             "shared": shared[(sr, sc)], "score": round(score, 3)}
            for (sr, sc), score in sorted(scored.items(), key=lambda kv: -kv[1])
            if sc in keep.get(sr, ()) and sr in keep.get(sc, ())]
