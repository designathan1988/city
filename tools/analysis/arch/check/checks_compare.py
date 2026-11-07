"""How two scans are compared, and the proof that each comparison can say no: every judge is shown a bad input and must refuse it."""
import collections
import json
import math

from . import apart


def snapshot(run, a, undo=lambda s: s):
    """A scan in a form that does not depend on order: the map, the queue, the roots, every name passed through `undo`."""
    groups = sorted(sorted(undo(m) for m in g["members"]) for g in a["groups"])
    queue = []
    for q in a["queue"]:
        queue.append({"cause": q["cause"], "reach": q["reach"], "commits": q["commits"], "score": q["score"], "places": sorted(undo(p["unit"]) for p in q["places"]),
                      "children": sorted((c, sorted(undo(u) for u in us)) for c, us in q["children"])})
    queue.sort(key=lambda q: (q["cause"], q["places"], q["children"]))
    return {"groups": groups, "queue": queue, "roots": [(q["cause"], q["places"]) for q in queue], "order": [q["shape"] for q in a["queue"]]}


def same(a, b):
    """Byte for byte, once both are in canonical form."""
    return json.dumps(a, sort_keys=True, ensure_ascii=False) == json.dumps(b, sort_keys=True, ensure_ascii=False)


def notes(a):
    """The marks a scan gives, without a single name or line: what must not move when code is only rearranged."""
    return sorted((q["cause"], q["reach"], q["commits"], q["score"]) for q in a["queue"])


def labels(a):
    return {m: g["id"] for g in a["groups"] for m in g["members"]}


def nmi(x, y):
    """Normalised mutual information of two partitions, on what they both know."""
    keys = [k for k in x if k in y]
    n = len(keys)
    if n == 0:
        return 0.0
    ca, cb = collections.Counter(x[k] for k in keys), collections.Counter(y[k] for k in keys)
    cab = collections.Counter((x[k], y[k]) for k in keys)
    ha = -sum(v / n * math.log(v / n) for v in ca.values())
    hb = -sum(v / n * math.log(v / n) for v in cb.values())
    mi = sum(v / n * math.log(v * n / (ca[p] * cb[q])) for (p, q), v in cab.items())
    return 2 * mi / max(ha + hb, 1e-12)


def beats(rows, a, b, rng, rounds):
    """Paired sign-flip test: how often chance alone gives a difference this large."""
    diffs = [(r[a] - r[b]) * r[3] for r in rows]
    observed = sum(diffs)
    count = 0
    for _ in range(rounds):
        if sum(d if rng.random() < 0.5 else -d for d in diffs) >= observed:
            count += 1
    return observed > 0, (count + 1) / (rounds + 1)


def pairs_kept(pairs, label):
    """The share of pairs a partition keeps together."""
    both = [(a, b) for a, b in pairs if a in label and b in label]
    return sum(label[a] == label[b] for a, b in both) / len(both) if both else 0.0


def chained(steps, unit):
    """A proof is a walk that holds together and ends at the finding."""
    return bool(steps) and steps[-1]["to"] == unit and all(a["to"] == b["from"] for a, b in zip(steps, steps[1:])) and all(s["file"] and s["line"] for s in steps)


def failures(ctx):
    """Each judge given a case it must refuse. A check that cannot fail proves nothing."""
    import random
    a = {"groups": [[1, 2], [3]], "queue": [], "roots": []}
    b = {"groups": [[1], [2, 3]], "queue": [], "roots": []}
    x = {1: 0, 2: 0, 3: 1, 4: 1}
    y = {1: 0, 2: 1, 3: 0, 4: 1}
    rng = random.Random(0)
    worse = [(0.1, 0.9, 0.0, 1.0)] * 30
    tests = {
        "same refuses different maps": not same(a, b),
        "same accepts equal maps": same(a, dict(a)),
        "nmi sees two unrelated partitions": nmi(x, y) < 0.5,
        "nmi sees equal partitions": nmi(x, x) > 0.99,
        "prediction refuses a worse model": not beats(worse, 0, 1, rng, 200)[0],
        "pairs_kept sees a broken pair": pairs_kept([(1, 3)], x) == 0.0,
        "empty set of pairs is a failure": pairs_kept([], x) == 0.0,
    }
    from .checks_copies import judge_injection
    step = {"from": "a", "to": "b", "file": "f", "line": 1}
    tests["a proof must exist"] = not chained([], "b")
    tests["a proof must reach the finding"] = not chained([step], "c")
    tests["a proof must hold together"] = not chained([step, {"from": "x", "to": "c", "file": "f", "line": 1}], "c")
    tests["a good proof passes"] = chained([step], "b")
    from . import oracle
    text = {"a.gd": 'var x = t.get("used")\nvar y = 1\n', "b.json": "{}\n"}
    code = oracle.Code(text)
    finding = {"file": "b.json", "line": 1, "category": "no_reader", "unit": "k", "detail": {"key": "used"}, "path": [{"file": "a.gd", "line": 1, "kind": "x"}], "places": []}
    tests["contamination refuses a group that moved with nothing touched beside it"] = apart.moved_groups({("a", "b")}, {("a", "c")}, set()) == [("a", "b"), ("a", "c")]
    tests["contamination allows only the partner of what was touched"] = apart.moved_groups({("a", "b")}, {("a", "c")}, {"a"}) == []
    tests["contamination refuses a cause whose score moved"] = len(apart.moved_causes({("x", 1, 2, 3)}, {("x", 1, 2, 4)}, {"x": ["u"]}, {"t"})) == 2
    tests["precision refutes a key the code reads"] = oracle.no_reader(code, finding) is False
    tests["precision confirms a key nobody names"] = oracle.no_reader(code, {**finding, "detail": {"key": "gone"}}) is True
    tests["precision sees no loop where there is none"] = oracle.has_loop(code, {**finding, "file": "a.gd"}) is False
    tests["precision refuses a kind that is often wrong"] = not oracle.judged({"x": (5, 5, 0)})[0]
    tests["precision refuses a kind it cannot decide"] = not oracle.judged({"x": (1, 0, 9)})[0]
    tests["precision refuses an empty sample"] = not oracle.judged({})[0]
    tests["precision accepts a good sample"] = oracle.judged({"x": (39, 1, 0)})[0]
    from .checks_limits import explodes
    tests["a trace of nearly everything is refused"] = explodes([90], 100)
    tests["no trace at all is refused"] = explodes([], 100)
    tests["injection refuses a missed case"] = not judge_injection({("B1", "w"): False})
    tests["injection refuses an empty set"] = not judge_injection({})
    tests["ablation refuses an adapter nothing depends on"] = not _ablation_judge({"x": []})
    return {"pass": all(tests.values()), "detail": {k: v for k, v in tests.items() if not v} or "every judge refused what it must"}


def _ablation_judge(lost):
    return all(lost.values()) and bool(lost)
