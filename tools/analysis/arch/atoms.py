"""The graph analysis works on: forwarding functions contracted away, edges merged into weights, and an order that comes from structure alone."""
import collections
import math

from .graph import DATA, FLOW, STRUCTURE

# Pieces of code; a file is only a place, never a node.
NODES = {"function", "method", "branch", "lambda", "field", "global", "key", "row", "entry", "uniform", "varying", "signal", "node", "enumerator", "class"}
WEIGHTED = FLOW | DATA | STRUCTURE
# What is left of an algorithm's parameters; the selfcheck moves it and the map must not move: the digits two values are called equal at. Only the
# comparison uses it: the values keep every digit, so rounding never adds up along the merges.
PRECISION = 12


def forwarders(graph, costs_of):
    """A function whose whole work is to call one other function stands for it: splitting code into such shims changes nothing."""
    out = {}
    for uid, u in graph.units.items():
        if u["kind"] not in ("function", "method") or u["attrs"].get("statements", 2) > 1 or u["attrs"].get("callback"):
            continue
        flows = {e.dst for e in graph.out.get(uid, ()) if e.kind in ("call", "virtual", "template") and e.dst in graph.units}
        other = [e for e in graph.out.get(uid, ()) if e.kind in ("write", "lock", "branch", "lambda")]
        if len(flows) == 1 and not other and not costs_of.get(uid):
            out[uid] = next(iter(flows))
    resolved = {}
    for uid in out:
        seen, t = {uid}, out[uid]
        while t in out and t not in seen:
            seen.add(t)
            t = out[t]
        resolved[uid] = t
    return resolved


def canonical_ranks(graph, nodes):
    """A rank for every node from the shape of the graph around it, so no decision ever depends on a name: colour refinement run to its fixed
    point (Weisfeiler-Lehman, as Paige and Tarjan refine partitions), where only classes next to a change are looked at again."""
    first = {n: f"{graph.units[n]['kind']}|{graph.units[n]['shape']}" for n in nodes}
    colour = {c: i for i, c in enumerate(sorted(set(first.values())))}
    sig = {n: colour[first[n]] for n in nodes}
    around = collections.defaultdict(list)
    kinds = {k: i for i, k in enumerate(sorted({e.kind for e in graph.edges.values()}))}
    for e in graph.edges.values():
        if e.src in sig and e.dst in sig:
            around[e.src].append((2 * kinds[e.kind], e.dst))
            around[e.dst].append((2 * kinds[e.kind] + 1, e.src))
    members = collections.defaultdict(list)
    for n, c in sig.items():
        members[c].append(n)
    fresh = len(colour)
    dirty = list(sig)
    while dirty:
        touched = {sig[n] for n in dirty}
        for n in dirty:
            touched.update(sig[m] for _, m in around.get(n, ()))
        dirty = []
        splits = []
        for c in sorted(touched):
            if len(members[c]) > 1:
                parts = collections.defaultdict(list)
                for n in members[c]:
                    parts[tuple(sorted((k, sig[m]) for k, m in around.get(n, ())))].append(n)
                if len(parts) > 1:
                    splits.append((c, [parts[k] for k in sorted(parts)]))
        for c, parts in splits:
            members[c] = parts[0]
            for part in parts[1:]:
                members[fresh] = part
                for n in part:
                    sig[n] = fresh
                dirty += part
                fresh += 1
    order = sorted(sig, key=sig.get)
    return {n: i for i, n in enumerate(order)}, {n: str(c) for n, c in sig.items()}


class Atoms:
    """Nodes of the analysis, each with its rank, and undirected weights between them."""

    def __init__(self, run):
        graph = run.graph
        self.graph = graph
        costs_of = collections.defaultdict(list)
        for c in run.facts.costs:
            if c[1] not in ("return", "guard"):
                costs_of[c[0]].append(c)
        self.costs_of = costs_of
        self.shims = forwarders(graph, costs_of)
        self.nodes = sorted(u for u, x in graph.units.items() if x["kind"] in NODES and u not in self.shims)
        self.rank, self.signature = canonical_ranks(graph, self.nodes)
        node_set = set(self.nodes)
        self.weights = collections.Counter()
        for e in graph.edges.values():
            if e.kind not in WEIGHTED or e.generic or e.conf < 1.0:
                continue
            a = self.shims.get(e.src, e.src)
            b = self.shims.get(e.dst, e.dst)
            if a == b or a not in node_set or b not in node_set:
                continue
            self.weights[(a, b) if a < b else (b, a)] += e.conf

    def add(self, a, b, w):
        a, b = self.shims.get(a, a), self.shims.get(b, b)
        if a != b and a in self.rank and b in self.rank:
            self.weights[(a, b) if a < b else (b, a)] += w

    def similarity(self):
        """Weight over the geometric mean of both ends' strength: only a node's own neighbours count, so far-away change cannot move it."""
        strength = collections.Counter()
        for (a, b), w in self.weights.items():
            strength[a] += w
            strength[b] += w
        return {k: w / math.sqrt(strength[k[0]] * strength[k[1]]) for k, w in self.weights.items()}
