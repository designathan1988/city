"""Findings about structure: the same responsibility in many places, gods by responsibility, the owner that is missing. Every cut comes from the
sample at hand, and nothing here looks at a file size or a line count."""
import collections
import itertools
import math
import re

from .core import natural_break, words_of

RULE_LINES = 3  # a set-and-call of two lines has the shape of half the code: it takes a body for a shape to be a rule

VALUE_FIELDS = 4
NUMBERS = {"float", "double", "int", "real"}


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


def _a_number_of_a_value(graph, field):
    """The x of a point, the r of a colour: a type of a few numbers is a value, and two functions that use one have nothing else in common."""
    fields = [graph.units[c] for c in graph.children.get(graph.units[field]["parent"], ()) if graph.units[c]["kind"] == "field"]
    return 0 < len(fields) <= VALUE_FIELDS and all(set(words_of(f["attrs"].get("type") or "")) & NUMBERS for f in fields)


def _rare_targets(graph, nodes):
    """What each function reaches, without the far tail: what nearly everyone touches says nothing about responsibility."""
    reach = {n: {e.dst for e in graph.out.get(n, ()) if e.kind in ("call", "read", "write", "virtual") and not e.generic} for n in nodes}
    df = collections.Counter(t for ts in reach.values() for t in ts)
    # State only: two callers of one helper have an owner already, the opposite of scattered. And what nearly all state users touch, the x of a point, says nothing.
    state = {t for t in df if t in graph.units and graph.units[t]["kind"] in ("field", "global") and not _a_number_of_a_value(graph, t)}
    common = [df[t] for t in state if df[t] > 1]
    cut = natural_break(common) if len(set(common)) > 1 else math.inf
    # What is done to it counts too: the loader that writes a setting and the code that reads it do two things to it.
    done = {n: {(e.dst, e.kind) for e in graph.out.get(n, ()) if e.kind in ("read", "write") and e.conf == 1.0} for n in nodes}
    return {n: {(t, how) for t, how in ts if 1 < df[t] < cut and t in state} for n, ts in done.items()}, df


def scattered(k, group_of, layer_of):
    """Units of one layer, in different groups, that do the same thing to the same things: one responsibility, several places. What counts as the same
    comes from the sample, the share of what two units touch that they touch together. One finding per unit with all who share it."""
    graph = k.graph
    nodes = [n for n in k.atoms.nodes if graph.units[n]["kind"] in ("function", "method") and n not in k.when.tests]
    rare, _ = _rare_targets(graph, nodes)
    by_target = collections.defaultdict(list)
    for n, ts in sorted(rare.items()):
        for t in sorted(ts):
            by_target[t].append(n)
    near = {}
    for a, targets in sorted(rare.items()):
        met = collections.Counter(b for t in targets for b in by_target[t] if b != a)
        ua = graph.units[a]
        place = layer_of.get(group_of.get(a))
        near[a] = [(b, n / (len(targets) + len(rare[b]) - n)) for b, n in sorted(met.items())
                   if n > 1 and graph.units[b]["parent"] != ua["parent"] and layer_of.get(group_of.get(b)) == place]
    cut = natural_break([share for found in near.values() for _, share in found])
    linked = {a: {b for b, share in found if share >= cut or cut == math.inf} for a, found in near.items()}
    out = []
    seen = set()
    for a in sorted(linked):
        if a in seen or not linked[a]:
            continue
        part, stack = {a}, [a]
        while stack:
            for b in linked.get(stack.pop(), ()):
                if b not in part:
                    part.add(b)
                    stack.append(b)
        seen |= part
        head = min(part, key=lambda u: (k.atoms.rank.get(u, 0), u))
        u = graph.units[head]
        f = k.finding("scattered", head, u["file"], u["line"], others=len(part) - 1)
        f["places"] = [(x, graph.units[x]["file"], graph.units[x]["line"]) for x in sorted(part)]
        out.append(f)
    return out


def gods(k, group_of, churn):
    """A class is a god by what it touches, not how long it is: groups reached, fan in and out, parts with no state in common, change."""
    graph = k.graph
    members = collections.defaultdict(list)
    for uid, u in graph.units.items():
        if u["kind"] in ("method", "function", "key", "row") and u["parent"] and uid not in k.atoms.shims and uid not in k.when.tests:
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
    cuts = [natural_break([r[i] for r in rows.values()]) for i in range(len(next(iter(rows.values()))))]
    out = []
    for c, row in rows.items():
        # Most of the signs at once, each above the break of its own sample: one high number makes a busy class, not a god.
        if sum(v >= cut for v, cut in zip(row, cuts)) * 2 > len(row) and row[0] > 0:
            u = graph.units[c]
            f = k.finding("god", c, u["file"], u["line"], reach=round(row[0], 3), fan_in=row[1], fan_out=row[2], islands=row[3], change=row[4])
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
    """Classes that answer the same messages with no base of ours in common, one finding per set of messages. Methods a macro stamps on one line are the
    macro's, not a role; what nearly every class answers says nothing."""
    graph = k.graph
    methods = collections.defaultdict(set)
    stamped = collections.Counter((u["file"], u["line"]) for u in graph.units.values() if u["kind"] == "method")
    for u in graph.units.values():
        if u["kind"] == "method" and u["parent"] in graph.units and graph.units[u["parent"]]["kind"] == "class" and u["id"] not in k.when.tests and stamped[(u["file"], u["line"])] == 1:
            methods[u["parent"]].add(u["name"])
    df = collections.Counter(m for ms in methods.values() for m in ms)
    bases = collections.defaultdict(set)
    for e in graph.edges.values():
        if e.kind == "inherit":
            bases[e.src].add(e.dst)
    answered = {u["name"] for u in graph.units.values() if u["attrs"].get("callback")}
    limit = tail_cut(list(df.values()))
    by_method = collections.defaultdict(list)
    for c, ms in methods.items():
        for m in ms:
            if 1 < df[m] < limit and m not in answered:
                by_method[m].append(c)
    shared = collections.defaultdict(set)
    for m, cs in by_method.items():
        for a, b in itertools.combinations(sorted(cs), 2):
            if not {x for x in bases[a] & bases[b] if x in graph.units}:
                shared[(a, b)].add(m)
    if not shared:
        return []
    cut = min(tail_cut([len(v) for v in shared.values()]), max(len(v) for v in shared.values()))
    roles = collections.defaultdict(set)
    for (a, b), names in shared.items():
        if len(names) > 1 and len(names) >= cut:
            roles[tuple(sorted(names))] |= {a, b}
    out = []
    for names, classes in sorted(roles.items()):
        ordered = sorted(classes)
        u = graph.units[ordered[0]]
        f = k.finding("implicit_role", ordered[0], u["file"], u["line"], classes=len(ordered), messages=len(names), names=list(names))
        f["places"] = [(c, graph.units[c]["file"], graph.units[c]["line"]) for c in ordered]
        out.append(f)
    return out


def _skeleton(k, u):
    """A body with its names and numbers out: `a > b` and `a < b` are one shape and two rules, the operators tell them apart."""
    lines = (k.run.texts.get(u["file"]) or "").split(chr(10))[u["line"] - 1:u["end"]]
    return re.sub(r"\w+", "x", "".join("".join(lines).split()))


def _apart_from_callers(graph, units):
    """Copies that call each other are one recursion in two steps, not one rule in two places: those count once."""
    same = set(units)
    return [u for u in units if not any(e.kind == "call" and e.dst in same and e.dst < u for e in graph.out.get(u, ()))]


def _repeated_rules(k, group_of):
    graph = k.graph
    by_shape = collections.defaultdict(list)
    for n in k.atoms.nodes:
        u = graph.units[n]
        if u["kind"] in ("function", "method") and u["shape"] and u["attrs"].get("statements", 0) > 1 and u["end"] - u["line"] >= RULE_LINES and n not in k.when.tests:
            by_shape[(u["shape"], _skeleton(k, u))].append(n)
    out = []
    for _, units in sorted(by_shape.items()):
        groups = {group_of.get(u, u) for u in _apart_from_callers(graph, units)}
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
    cut = min(tail_cut(list(counts.values())), max(counts.values()))
    out = []
    for f, n in sorted(counts.items()):
        if n >= cut and n > 1 and f in k.graph.units:
            u = k.graph.units[f]
            finding = k.finding("repeated_decision", f, u["file"], u["line"], places=n)
            finding["places"] = [(x, k.graph.units[x]["file"], k.graph.units[x]["line"]) for x in sorted(deciders[f]) if x in k.graph.units]
            out.append(finding)
    return out


__all__ = ["scattered", "gods", "missing_owner", "words_of"]
