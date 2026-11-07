"""Findings about structure: the same responsibility in many places, gods by responsibility, the owner that is missing. Every cut comes from the
sample at hand, and nothing here looks at a file size or a line count."""
import collections
import itertools
import math

from .core import natural_break, words_of


OWNER_KINDS = ("implicit_role", "repeated_rule", "repeated_decision")


def _entropy(counts):
    total = sum(counts.values())
    return -sum(v / total * math.log2(v / total) for v in counts.values() if v) if total else 0.0


def tail_cut(values):
    """Where the far tail of a sample starts: the break of what lies above the sample's own break. A small sample has no tail."""
    data = [v for v in values if v > 1]
    if len(set(data)) < 2:
        return math.inf
    upper = [v for v in data if v >= natural_break(data)]
    return natural_break(upper)


def _rare_targets(graph, nodes):
    """What each function reaches, without the far tail: what nearly everyone touches says nothing about responsibility."""
    reach = {n: {e.dst for e in graph.out.get(n, ()) if e.kind in ("call", "read", "write", "virtual") and not e.generic} for n in nodes}
    df = collections.Counter(t for ts in reach.values() for t in ts)
    cut = tail_cut(list(df.values()))
    return {n: {t for t in ts if 1 < df[t] < cut} for n, ts in reach.items()}, df


def scattered(k, group_of, layer_of):
    """Units of one layer, in different groups, that do the same thing to the same things: one responsibility, several places. One finding per unit with
    all who share it: the pairs are counted and forgotten one unit at a time, so what grows with the square of a repository is never kept."""
    graph = k.graph
    nodes = [n for n in k.atoms.nodes if graph.units[n]["kind"] in ("function", "method")]
    rare, _ = _rare_targets(graph, nodes)
    by_target = collections.defaultdict(list)
    for n, ts in sorted(rare.items()):
        for t in sorted(ts):
            by_target[t].append(n)
    out = []
    for a, targets in sorted(rare.items()):
        met = collections.Counter(b for t in targets for b in by_target[t] if b != a)
        ua = graph.units[a]
        place = layer_of.get(group_of.get(a))
        others = sorted(b for b, n in met.items() if n > 1 and graph.units[b]["parent"] != ua["parent"] and layer_of.get(group_of.get(b)) == place)
        if others:
            f = k.finding("scattered", a, ua["file"], ua["line"], others=len(others))
            f["places"] = [(x, graph.units[x]["file"], graph.units[x]["line"]) for x in (a, *others)]
            out.append(f)
    return out


def gods(k, group_of, churn):
    """A class is a god by what it touches, not how long it is: groups reached, fan in and out, parts with no state in common, change."""
    graph = k.graph
    members = collections.defaultdict(list)
    for uid, u in graph.units.items():
        if u["kind"] in ("method", "function", "key", "row") and u["parent"] and uid not in k.atoms.shims:
            members[u["parent"]].append(uid)
    rows = {}
    for cls, ms in members.items():
        if cls not in graph.units or len(ms) < 2:
            continue
        reached = collections.Counter()
        fan_in, fan_out = set(), set()
        parts = {m: {e.dst for e in graph.out.get(m, ()) if e.kind in ("read", "write")} for m in ms}
        for m in ms:
            for e in graph.out.get(m, ()):
                if e.kind in ("call", "read", "write", "route", "name_ref") and e.dst in group_of and e.conf == 1.0:
                    reached[group_of[e.dst]] += 1
                    fan_out.add(e.dst)
            fan_in.update(e.src for e in graph.into.get(m, ()) if e.conf == 1.0)
        islands = _islands(parts)
        change = sum(churn.get(m, 0) for m in ms)
        rows[cls] = (_entropy(reached), len(fan_in), len(fan_out), islands, change)
    if len(rows) < 2:
        return []
    keys = list(rows)
    score = {}
    for i in range(5):
        order = sorted(keys, key=lambda c: rows[c][i])
        for r, c in enumerate(order):
            score[c] = score.get(c, 1.0) * (1 + r / len(order))
    cut = natural_break(list(score.values()))
    out = []
    for c in keys:
        if score[c] >= cut and rows[c][0] > 0:
            u = graph.units[c]
            f = k.finding("god", c, u["file"], u["line"], reach=round(rows[c][0], 3), fan_in=rows[c][1], fan_out=rows[c][2], islands=rows[c][3], change=rows[c][4])
            f["places"] = [(m, graph.units[m]["file"], graph.units[m]["line"]) for m in sorted(members[c])]
            out.append(f)
    return out


def _islands(parts):
    """How many sets of methods share no field: a class that is several classes."""
    parent = {}

    def find(x):
        while parent.setdefault(x, x) != x:
            parent[x] = parent[parent[x]]
            x = parent[x]
        return x

    owner = {}
    for m, fields in parts.items():
        find(m)
        for f in fields:
            if f in owner:
                parent[find(m)] = find(owner[f])
            owner[f] = m
    return len({find(m) for m in parts})


def missing_owner(k, group_of):
    """Classes that answer the same messages with no common base; the same shape of code in several groups; one state decided in many places."""
    return _implicit_roles(k) + _repeated_rules(k, group_of) + _repeated_decisions(k, group_of)


def _implicit_roles(k):
    graph = k.graph
    methods = collections.defaultdict(set)
    for u in graph.units.values():
        if u["kind"] == "method" and u["parent"] in graph.units and graph.units[u["parent"]]["kind"] == "class":
            methods[u["parent"]].add(u["name"])
    df = collections.Counter(m for ms in methods.values() for m in ms)
    bases = collections.defaultdict(set)
    for e in graph.edges.values():
        if e.kind == "inherit":
            bases[e.src].add(e.dst)
    pairs = collections.Counter()
    by_method = collections.defaultdict(list)
    answered = {u["name"] for u in graph.units.values() if u["attrs"].get("callback")}
    for c, ms in methods.items():
        for m in ms:
            if 1 < df[m] and m not in answered:
                by_method[m].append(c)
    limit = tail_cut(list(df.values()))
    for m, cs in by_method.items():
        if df[m] < limit:
            for a, b in itertools.combinations(sorted(cs), 2):
                if not {x for x in bases[a] & bases[b] if x in graph.units}:
                    pairs[(a, b)] += 1
    if not pairs:
        return []
    out = []
    for (a, b), n in sorted(pairs.items()):
        if n > 1:
            ua, ub = graph.units[a], graph.units[b]
            f = k.finding("implicit_role", a, ua["file"], ua["line"], other=b, messages=n, names=sorted(methods[a] & methods[b]))
            f["places"] = [(a, ua["file"], ua["line"]), (b, ub["file"], ub["line"])]
            out.append(f)
    return out


def _repeated_rules(k, group_of):
    graph = k.graph
    by_shape = collections.defaultdict(list)
    for n in k.atoms.nodes:
        u = graph.units[n]
        if u["kind"] in ("function", "method") and u["shape"] and u["attrs"].get("statements", 0) > 1:
            by_shape[u["shape"]].append(n)
    out = []
    for shape, units in sorted(by_shape.items()):
        groups = {group_of.get(u, u) for u in units}
        if len(units) > 1 and len(groups) > 1:
            units = sorted(units)
            u = graph.units[units[0]]
            f = k.finding("repeated_rule", units[0], u["file"], u["line"], copies=len(units))
            f["places"] = [(x, graph.units[x]["file"], graph.units[x]["line"]) for x in units]
            out.append(f)
    return out


def _repeated_decisions(k, group_of):
    deciders = collections.defaultdict(set)
    for c in k.by_kind["decision"] + k.by_kind["guard"]:
        for field in c[6].get("on", ()):
            if field:
                deciders[field].add(k.graph.callable_of(c[0]))
    counts = {f: len({group_of.get(u, u) for u in us}) for f, us in deciders.items()}
    if len(counts) < 2:
        return []
    ordered = sorted(counts.values())
    cut = ordered[len(ordered) // 2]
    out = []
    for f, n in sorted(counts.items()):
        if n >= cut and n > 1 and f in k.graph.units:
            u = k.graph.units[f]
            finding = k.finding("repeated_decision", f, u["file"], u["line"], places=n)
            finding["places"] = [(x, k.graph.units[x]["file"], k.graph.units[x]["line"]) for x in sorted(deciders[f]) if x in k.graph.units]
            out.append(finding)
    return out


__all__ = ["scattered", "gods", "missing_owner", "words_of"]
