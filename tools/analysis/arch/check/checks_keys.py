"""The answer keys: what the project says about itself, used only here. Half of each key tunes, the other half is kept for the final mark."""
import collections
import json
import os
import random

from ..core import digest
from ..repo import Repo
from .checks_compare import pairs_kept
from .inject import run_on
from .supervisor import Job

KEYS = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "arch_eval")
TRIALS = 500
SIGNIFICANT = 0.01


def _load(name):
    with open(os.path.join(KEYS, name), encoding="utf8") as f:
        return json.load(f)


def split(items):
    """Tuning or final, by a hash of the item: it never moves and no one picks."""
    tune, final = [], []
    for x in items:
        (tune if int(digest(json.dumps(x, sort_keys=True))[:8], 16) % 2 == 0 else final).append(x)
    return tune, final


def file_labels(run, a):
    """Each file at the group most of its units are in: the map of the key is per file."""
    votes = collections.defaultdict(collections.Counter)
    for g in a["groups"]:
        for m in g["members"]:
            votes[run.graph.units[m]["file"]][g["id"]] += 1
    return {f: c.most_common(1)[0][0] for f, c in votes.items()}


def upper_labels(run, a, level):
    """Files by the clusters of an upper level of the tree."""
    from ..groups import leaves_of
    votes = collections.defaultdict(collections.Counter)
    for node in a["upper"][level]:
        for m in leaves_of(a["tree"], node):
            votes[run.graph.units[m]["file"]][node] += 1
    return {f: c.most_common(1)[0][0] for f, c in votes.items()}


def _by_chance(label):
    """How often two files share a label if labels fall at random: one big cluster keeps every pair and says nothing."""
    counts = collections.Counter(label.values())
    total = sum(counts.values()) or 1
    return sum((c / total) ** 2 for c in counts.values())


def _chance(pairs, label, rng):
    files = sorted(label)
    values = [label[f] for f in files]
    worst = 0
    for _ in range(TRIALS):
        rng.shuffle(values)
        shuffled = dict(zip(files, values))
        if pairs_kept(pairs, shuffled) >= pairs_kept(pairs, label):
            worst += 1
    rng.shuffle(values)
    return pairs_kept(pairs, dict(zip(files, values))), (worst + 1) / (TRIALS + 1)


def score_pairs(run, a, pairs):
    """A key of pairs: the level of the map is chosen on one half, the mark comes from the other."""
    tune, final = split(pairs)
    levels = [file_labels(run, a)] + [upper_labels(run, a, i) for i in range(len(a["upper"]))]
    scored = [pairs_kept([tuple(p) for p in tune], lab) - _by_chance(lab) for lab in levels]
    best = scored.index(max(scored))
    held = [tuple(p) for p in final]
    kept = pairs_kept(held, levels[best])
    chance, p = _chance(held, levels[best], random.Random(2))
    return {"level": best, "of": len(levels), "clusters": len(set(levels[best].values())), "tuning_over_chance": round(scored[best], 3), "final": round(kept, 3), "chance": round(chance, 3), "p": p, "pairs": len(held)}


def keys(ctx):
    """The map, the layers and the owners against what the project wrote down, on the half kept for the final mark."""
    run, a = ctx.plain()
    mapa = score_pairs(run, a, _load("map_pairs.json"))
    layers = _order_of_layers(run, a)
    owners = _owners(run, a)
    ok = mapa["final"] > mapa["chance"] and mapa["p"] < SIGNIFICANT and layers["agree"] > layers["chance"] and owners["found"] > 0
    return {"pass": ok, "detail": {"map": mapa, "layers": layers, "owners": owners}}


def _order_of_layers(run, a):
    """Each folder pair the rules order: the program's layer of the lower must not sit above the upper's."""
    rules = _load("layers.json")
    by_folder = collections.defaultdict(list)
    for g in a["groups"]:
        layer = a["layer_of"].get(g["id"])
        for m in g["members"]:
            f = run.graph.units[m]["file"]
            for folder in rules:
                if f.startswith(folder + "/") and layer is not None:
                    by_folder[folder].append(layer)
    mean = {k: sum(v) / len(v) for k, v in by_folder.items() if v}
    pairs = [(a_, b) for a_, deps in rules.items() for b in deps if a_ in mean and b in mean and a_ != b]
    agree = sum(mean[a_] >= mean[b] for a_, b in pairs) / len(pairs) if pairs else 0.0
    rng = random.Random(3)
    values = list(mean.values())
    chance = 0.0
    for _ in range(TRIALS):
        rng.shuffle(values)
        m2 = dict(zip(mean, values))
        chance += sum(m2[a_] >= m2[b] for a_, b in pairs) / max(len(pairs), 1)
    return {"pairs": len(pairs), "agree": round(agree, 3), "chance": round(chance / TRIALS, 3)}


def _owners(run, a):
    """An owner of a rule is used by others: the program must see the owner's symbol with callers from other groups."""
    owners = _load("owners.json")
    group_of = {m: g["id"] for g in a["groups"] for m in g["members"]}
    found, seen = 0, []
    for o in owners:
        short = o["symbol"].split("::")[-1]
        units = [u for u in run.graph.units.values() if u["file"] == o["file"] and u["name"] == short]
        used = any(group_of.get(e.src) != group_of.get(u["id"]) for u in units for e in run.graph.into.get(u["id"], ()))
        found += used
        seen.append((o["id"], bool(units), used))
    return {"owners": len(owners), "found": found, "detail": seen}


def defects_jobs(ctx):
    """Each defect with a recorded cause: at the commit before its fix the program must point at the place. A commit is read once, however many defects
    it holds, and commits go in the order of history: what did not change from one to the next is not read again."""
    tune, final = split(_load("defects.json"))
    commits = collections.defaultdict(list)
    for d in final + tune:
        commits[d["before"]].append({**d, "kept": d in final})
    order = {sha: i for i, (sha, _) in enumerate(ctx.repo.history())}
    ordered = sorted(commits.items(), key=lambda kv: order.get(kv[0], -1))
    return [Job("defects", "defects_at", {"path": ctx.path, "commits": ordered}, "defects", True)]


def defects_at(spec):
    out = []
    for before, items in spec["commits"]:
        _, a = run_on(Repo(spec["path"], before), True, True)
        out += [{"id": d["id"], "kept": d["kept"], "pointed": _pointed(a, d)} for d in items]
    return out


def defects(ctx, out):
    results = out("defects")
    held = [r for r in results if r["kept"]]
    return {"pass": bool(results) and all(r["pointed"] for r in results), "detail": {"final": sum(r["pointed"] for r in held), "of": len(held), "all": results}}


def _pointed(a, d):
    """Some finding in the expected place, as the record gives it."""
    wanted = d["place"]
    for f in a["findings"]:
        text = " ".join((f["unit"], f["file"], *(p[0] for p in f["places"])))
        if wanted in text and (not d.get("category") or f["category"] in d["category"]):
            return True
    return False
