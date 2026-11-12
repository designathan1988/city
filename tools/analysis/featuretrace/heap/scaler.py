"""Which runs of each function get contexts (Li, Tan, Moller, Smaragdakis, "Scalability-First Pointer Analysis with
Self-Tuning Context-Sensitivity", ESEC/FSE 2018; log/trace-research.md F44). A context-insensitive analysis first; from
it, for each function, what each kind of context would cost: the points-to facts of its nodes times the runs that kind
makes — one (ONCE), one per site calling it (CALL), one per object of its receiver or of the parameter its result
carries (OBJECT: `flows.carried`; Wei and Ryder, ECOOP 2015). Each function then takes the most precise kind whose cost
is within one bound, the largest bound under which the costs together stay within the total the analysis may hold —
so the analysis is as precise as that total allows, and never larger. A function whose runs need no context
(`flows.contextual`) runs once; one the insensitive analysis never ran never runs.
"""
from __future__ import annotations

from types import SimpleNamespace

from ..js_functions import ARROW
from ..vocab import CALL as CALLED, HANDED, NEW
from . import flows

ONCE, CALL, OBJECT = 0, 1, 2      # the levels: one run; one per calling site; one per receiver or carried object
TOTAL = 30_000_000                # the points-to facts the analysis may hold (Scaler's setting, TST = 30M)
RUNS_BY = (CALLED, NEW, HANDED)   # the links that run a function: a site calling it


def levels(ix, insensitive, total: int = TOTAL) -> dict[int, int]:
    """The level of each function, from the context-insensitive result `insensitive` (its links and `volumes`:
    gid -> (points-to facts of its nodes, objects of its `this`, objects of each parameter))."""
    light = SimpleNamespace(ix=ix, memo={})
    costs = {}
    for gid, (facts, receivers, params) in insensitive.volumes.items():
        facts = max(facts, 1)
        if not flows.contextual(light, gid):
            costs[gid] = [(ONCE, facts)]
            continue
        sites = len({(edge.src, edge.line) for edge in insensitive.inn.get(gid, ()) if edge.kind in RUNS_BY})
        index = flows.carried(light, gid)
        if receivers and ix.units[gid].this_of != ARROW:
            objects = receivers
        elif index is not None and index < len(params):
            objects = params[index]
        else:
            objects = sites
        costs[gid] = [(OBJECT, facts * max(objects, 1)), (CALL, facts * max(sites, 1)), (ONCE, facts)]
    return choose(costs, total)


def choose(costs: dict, total: int) -> dict[int, int]:
    """Each function's level: its most precise option (options in order of precision, the last ONCE) whose cost is
    within the largest bound under which the chosen costs together stay within `total`; ONCE for all where none is."""
    def pick(bound) -> dict:
        return {gid: next((level for level, cost in options if cost <= bound), options[-1][0])
                for gid, options in costs.items()}

    def spent(bound) -> int:
        return sum(next((cost for _, cost in options if cost <= bound), options[-1][1]) for options in costs.values())

    bounds = sorted({cost for options in costs.values() for _, cost in options})
    low, high, best = 0, len(bounds) - 1, None
    while low <= high:                                # the costs together only grow with the bound
        middle = (low + high) // 2
        if spent(bounds[middle]) <= total:
            best, low = bounds[middle], middle + 1
        else:
            high = middle - 1
    return pick(best) if best is not None else {gid: options[-1][0] for gid, options in costs.items()}
