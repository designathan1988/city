"""End-to-end trace of a feature over the typed graph: who triggers it, what it calls and mutates, who reads what it mutates.

Edge kinds come from the language's own constructs (extract.py): `call`, `string` (a name carried as a value) and
`register` (a function handed to a receiver) are control flow; `write` is a mutation; `read` is a dependency on data;
`dep` is an import, include or load.
- Upstream: what makes the seeds run, up to the entry points (nothing makes them run): control-flow callers; for an
  object a function is registered on (a signal), the units that fire it (write it); for a polymorphic call through a
  receiver, the units that assign the receiver (its value picks the override).
- Downstream: from the seeds forward through control flow. Prevalence in the whole graph decides membership: a unit
  called from more places than the graph's norm (Tukey's upper fence of the callables' caller counts) is
  infrastructure, reached but not walked on as part of the feature; upstream, a unit calling more than the norm is a
  hub where the walk up stops.
- Effect: what runs when the feature runs is the closure of direct calls from its members and the infrastructure they
  use, through everything (a shared function still changes state when the feature calls it). Every datum written in
  that closure is mutated state; every unit reading mutated state is a dependent. Registrations and names carried as
  strings fire later, not in the call: they extend membership, not the closure.
- Consumers: units outside that run which read far more of the mutated state than its other readers do (Tukey's upper
  fence of how many mutated data each reader reads), each with the data it reads and the chain from the entry that runs
  it (a frame, a thread, an engine callback): where the feature's effect is picked up later."""
import statistics
from collections import deque

FLOW = frozenset(['call', 'string', 'register'])
CALL, WRITE, READ, DEP, ASSIGN = 'call', 'write', 'read', 'dep', 'assign'


class Trace:
    def __init__(self):
        self.seeds = []
        self.upstream = set()       # every caller of the seeds, transitively
        self.up_next = {}           # caller -> the next unit toward a seed
        self.entries = []           # upstream sources: nothing in the code calls them
        self.hubs = []              # callers that call far more than the graph's norm: the walk up stops there
        self.feature = set()        # downstream units that belong to the feature (prevalent)
        self.shared = set()         # downstream units it uses that belong to the wider system
        self.executed = set()       # units run through infrastructure while the feature runs (direct-call closure)
        self.mutated = set()        # data written by the feature
        self.dependents = set()     # units reading data the feature writes
        self.inputs = set()         # data the feature reads that it does not write
        self.down_prev = {}         # unit -> (previous unit, edge kind) on its path from a seed
        self.prevalence = {}        # unit -> how many units call it (control-flow callers)
        self.consumers = []         # readers outside the run that read an outlier share of the mutated state
        self.consumer_reads = {}    # consumer -> the mutated data it reads
        self.consumer_up = {}       # consumer -> chain from the entry that runs it down to the consumer

    def chain_up(self, node):
        chain = [node]
        while self.up_next.get(chain[-1]) is not None:
            chain.append(self.up_next[chain[-1]])
        return chain

    def chain_down(self, node):
        chain = [(node, None)]
        while self.down_prev.get(chain[-1][0]) is not None:
            previous, kind = self.down_prev[chain[-1][0]]
            chain[-1] = (chain[-1][0], kind)
            chain.append((previous, None))
        return chain[::-1]


def trace(graph, seeds):
    result = Trace()
    result.seeds = list(dict.fromkeys(seeds))
    callers_fence, callees_fence = _fences(graph)
    _upstream(graph, result, lambda n: _callees(graph, n) <= callees_fence or n in result.seeds)
    infrastructure = lambda n: graph.nodes[n].kind != 'data' and _callers(graph, n) > callers_fence  # noqa: E731
    reached, prev = _reach(graph, result.seeds, lambda n: n in result.seeds or not infrastructure(n))
    result.feature = {n for n in reached if graph.nodes[n].kind != 'data' and (n in result.seeds or not infrastructure(n))}
    result.shared = {n for n in reached if graph.nodes[n].kind != 'data'} - result.feature
    result.prevalence = {n: _callers(graph, n) for n in reached}
    result.down_prev = prev
    result.executed = executed_closure(graph, result.feature | result.shared) - result.feature - result.shared
    for node in result.feature | result.shared | result.executed:
        for target, edge in graph.out[node].items():
            if graph.nodes[target].kind == 'data':
                if WRITE in edge.kinds:
                    result.mutated.add(target)
                elif READ in edge.kinds:
                    result.inputs.add(target)
    result.inputs -= result.mutated
    for datum in result.mutated:
        result.dependents.update(r for r, e in graph.inn[datum].items() if READ in e.kinds)
    _consumers(graph, result)
    return result


def _consumers(graph, result):
    running = result.feature | result.shared | result.executed
    reads = {d: {t for t, e in graph.out[d].items() if t in result.mutated and READ in e.kinds}
             for d in result.dependents - running if graph.nodes[d].kind != 'data'}
    counts = [len(v) for v in reads.values()]
    fence = 0
    if len(counts) >= 4:
        q1, _, q3 = statistics.quantiles(counts, n=4)
        fence = q3 + 1.5 * (q3 - q1)
    result.consumers = sorted((d for d in reads if len(reads[d]) > fence), key=lambda d: (-len(reads[d]), d))
    result.consumer_reads = {d: reads[d] for d in result.consumers}
    result.consumer_up = {d: entry_chain(graph, d) for d in result.consumers}


def entry_chain(graph, start):
    """The shortest chain from an entry point (nothing makes it run) down to `start`, walking `_before` breadth-first;
    just [start] when every way up loops."""
    toward = {start: None}
    queue = deque([start])
    while queue:
        node = queue.popleft()
        before = _before(graph, node)
        if not before:
            chain = [node]
            while toward[chain[-1]] is not None:
                chain.append(toward[chain[-1]])
            return chain
        for unit in before:
            if unit not in toward:
                toward[unit] = node
                queue.append(unit)
    return [start]


def executed_closure(graph, start):
    """Every callable reached from `start` through direct calls, transitively: the code that runs in those calls."""
    seen, stack = set(start), list(start)
    while stack:
        node = stack.pop()
        for target, edge in graph.out[node].items():
            if CALL in edge.kinds and target not in seen and graph.nodes[target].kind != 'data':
                seen.add(target)
                stack.append(target)
    return seen


def _flow_in(graph, node):
    return [(source, edge) for source, edge in graph.inn[node].items() if edge.kinds & FLOW]


def _before(graph, node):
    """Units that make `node` run: its control-flow callers; for a datum (a signal or object a function is registered
    on), the units that write or fire it; and for each polymorphic call into `node` made through a receiver, the units
    that assign that receiver (its value picks which override the call reaches)."""
    writers = [s for s, e in graph.inn[node].items() if WRITE in e.kinds and graph.nodes[s].kind != 'data']
    if graph.nodes[node].kind == 'data':
        return writers
    before = writers if graph.is_object(node) else []
    for caller, _ in _flow_in(graph, node):
        before.append(caller)
        for datum in graph.via.get((caller, node), ()):
            before.extend(s for s, e in graph.inn[datum].items() if ASSIGN in e.kinds and graph.nodes[s].kind != 'data')
    return before


def _upstream(graph, result, expand):
    queue = deque(result.seeds)
    seen = set(result.seeds)
    while queue:
        node = queue.popleft()
        if not expand(node):
            continue
        for caller in _before(graph, node):
            if caller not in seen:
                seen.add(caller)
                result.up_next[caller] = node
                queue.append(caller)
    result.upstream = seen - set(result.seeds)
    order = lambda n: (len(result.chain_up(n)), n)  # noqa: E731
    result.entries = sorted((n for n in seen if not _before(graph, n)), key=order)
    result.hubs = sorted((n for n in seen if _before(graph, n) and not expand(n)), key=order)


def _forward(graph, node):
    """(unit, kind) one step downstream: control flow, and the data written (state, not walked further: its readers
    are the dependents, listed apart)."""
    if graph.nodes[node].kind == 'data':
        return []
    out = []
    for target, edge in graph.out[node].items():
        if edge.kinds & FLOW:
            out.append((target, 'call'))
        elif WRITE in edge.kinds and graph.nodes[target].kind == 'data':
            out.append((target, WRITE))
    return out


def _reach(graph, seeds, expand):
    """Breadth-first reach; only units for which `expand` holds pass the walk on (the others are reached, not expanded)."""
    prev = {s: None for s in seeds}
    queue = deque(seeds)
    while queue:
        node = queue.popleft()
        if not expand(node):
            continue
        for target, kind in _forward(graph, node):
            if target not in prev:
                prev[target] = (node, kind)
                queue.append(target)
    return set(prev), prev


def _callers(graph, node):
    return sum(1 for e in graph.inn[node].values() if e.kinds & FLOW)


def _callees(graph, node):
    """How many names a unit calls: one polymorphic call reaching many overrides is one call in its code."""
    return len({e.name for e in graph.out[node].values() if e.kinds & FLOW})


def _fences(graph):
    """Tukey's upper fences (Q3 + 1.5 IQR) of how many callers and how many callees the callables of this graph have:
    above them a unit is infrastructure (called from everywhere) or a hub (calls everything)."""
    callables = [n.id for n in graph.nodes if n.kind == 'callable']
    if len(callables) < 4:
        return float('inf'), float('inf')
    fences = []
    for count in (_callers, _callees):
        q1, _, q3 = statistics.quantiles([count(graph, n) for n in callables], n=4)
        fences.append(q3 + 1.5 * (q3 - q1))
    return tuple(fences)
