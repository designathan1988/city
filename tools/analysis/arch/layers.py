"""Layers and closures. A layer is a place in the graph of groups once cycles are condensed, never a folder; a closure comes in rings,
so a trace does not grow into the whole program."""
import collections

import networkx as nx

from .atoms import PRECISION
from .graph import DATA, FLOW

ENTRY = "#entry"


def condensed(graph, atoms, group_of):
    """Who depends on whom between groups, by the net direction of their edges; a cycle is one place."""
    weight = collections.Counter()
    for e in graph.edges.values():
        if e.generic or (e.kind not in FLOW and e.kind not in DATA):
            continue
        a, b = group_of.get(atoms.shims.get(e.src, e.src)), group_of.get(atoms.shims.get(e.dst, e.dst))
        if a is not None and b is not None and a != b:
            weight[(a, b)] += e.conf
    g = nx.DiGraph()
    g.add_nodes_from(sorted(set(group_of.values())))
    for (a, b), w in sorted(weight.items()):
        # Equal weights reached by sums in another order must stay equal: no direction is read from the last digit.
        if round(w, PRECISION) > round(weight.get((b, a), 0), PRECISION):
            g.add_edge(a, b, weight=w)
    return g


def layer_of(g):
    """The longest way down from each group: what depends on nothing sits at zero."""
    c = nx.condensation(g)
    height = {}
    for n in reversed(list(nx.topological_sort(c))):
        height[n] = 1 + max((height[m] for m in c.successors(n)), default=-1)
    return {x: height[n] for n in c for x in c.nodes[n]["members"]}


def roots_of(run):
    """Where execution comes in: what only the engine, a thread, the main entry or a data file calls."""
    graph = run.graph
    out = set()
    for uid, u in graph.units.items():
        a = u["attrs"]
        if a.get("callback") or a.get("repeating") or u["kind"] == "class" and a.get("script"):
            out.add(uid)
    return out


def flow_graph(graph):
    g = nx.DiGraph()
    g.add_node(ENTRY)
    for e in graph.edges.values():
        if e.kind in FLOW and not e.generic and e.src in graph.units and e.dst in graph.units:
            g.add_edge(e.src, e.dst)
    return g


def dominators(graph, roots):
    """Who every path from the entries must pass through: what removing a unit takes with it."""
    g = flow_graph(graph)
    for r in sorted(roots):
        g.add_edge(ENTRY, r)
    return g, nx.immediate_dominators(g, ENTRY)


def rings(graph, flow, idom, members):
    """The closure of a feature in layers: own (only it reaches), executed, data, link (root down to it) and what is merely reachable."""
    inside = set(members)
    own = set()
    for n in idom:
        x = n
        while x != ENTRY and x not in inside and idom.get(x, ENTRY) != x:
            x = idom.get(x, ENTRY)
        if x in inside:
            own.add(n)
    executed = set()
    stack = [m for m in members if m in flow]
    while stack:
        n = stack.pop()
        if n in executed:
            continue
        executed.add(n)
        stack.extend(m for m in flow.successors(n) if m not in executed and _narrow(graph, n, m, inside))
    data = _thin_data(graph, executed)
    link = set()
    for m in members:
        x = m
        while x in idom and idom[x] != x and x != ENTRY:
            link.add(x)
            x = idom[x]
    reachable = set()
    stack = list(executed)
    while stack:
        n = stack.pop()
        if n in reachable or n not in flow:
            continue
        reachable.add(n)
        stack.extend(flow.successors(n))
    return {"own": sorted(own), "executed": sorted(executed), "data": sorted(data), "link": sorted(link), "reachable": len(reachable)}


def _narrow(graph, a, b, inside):
    """A step of the executed ring leaves the feature only by a call that is certain: names with many candidates stay out."""
    if b in inside:
        return True
    return all(e.conf == 1.0 for e in graph.out.get(a, ()) if e.dst == b)


def _thin_data(graph, executed):
    """A thin slice (Sridharan et al. 2007): for what the feature reads, only who writes that very field; for what it writes, who reads it."""
    out = set()
    for n in executed:
        for e in graph.out.get(n, ()):
            if e.kind == "read":
                out.update(w.src for w in graph.into.get(e.dst, ()) if w.kind == "write")
            elif e.kind == "write":
                out.update(r.src for r in graph.into.get(e.dst, ()) if r.kind == "read")
    return out - executed
