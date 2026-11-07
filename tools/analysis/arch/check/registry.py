"""Which mutation proves which class, and the check that coverage cannot be inflated: every class has its own detector function and its own
mutation operator, none shared with another class, and gold of its own."""
from ..catalogue import CLASSES
from . import mutate_contracts, mutate_costs
from .gold import gold

MUTATIONS = {
    "budget_boundary": mutate_contracts.relax_admission,
    "unbudgeted_submit": mutate_contracts.drop_draw,
    "lost_update": mutate_contracts.erase_in_flight,
    "cache_identity": mutate_contracts.drop_key_field,
    "taint_alloc": mutate_contracts.remove_bound,
    "composed_quadratic": mutate_costs.wrap_in_loop,
    "redone_work": mutate_costs.remove_feeding_write,
}


def verify():
    """Problems with the registry, empty when one class is one detector, one mutation and its own gold."""
    problems = []
    ids = [cid for cid, _, _ in CLASSES]
    detectors = [fn for _, _, fn in CLASSES]
    if len(set(ids)) != len(ids):
        problems.append("a class is listed twice")
    if len({fn.__code__ for fn in detectors}) != len(detectors):
        problems.append("a detector function serves two classes")
    if set(MUTATIONS) != set(ids):
        problems.append(f"classes without a mutation or mutations without a class: {sorted(set(ids) ^ set(MUTATIONS))}")
    ops = list(MUTATIONS.values())
    if len({fn.__code__ for fn in ops}) != len(ops):
        problems.append("a mutation serves two classes")
    if set(fn.__code__ for fn in ops) & set(fn.__code__ for fn in detectors):
        problems.append("a mutation is also a detector")
    golden = {d["class"] for d in gold()["defects"]}
    if set(ids) - golden:
        problems.append(f"classes without gold: {sorted(set(ids) - golden)}")
    if golden - set(ids):
        problems.append(f"gold for no class: {sorted(golden - set(ids))}")
    return problems
