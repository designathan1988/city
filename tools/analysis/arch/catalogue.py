"""The classes of defect the analysis looks for, each with the one function that finds it. The selfcheck pairs each with its own mutation and its
own gold; nothing here is shared between two classes."""
from . import contracts, costmodel

CLASSES = (
    ("budget_boundary", "a budget admits a draw when built with nothing to give", contracts.budget_boundary),
    ("unbudgeted_submit", "jobs per item outside the quota the other consumers share", contracts.unbudgeted_submit),
    ("lost_update", "a dirty key erased while its job is in flight, no newer job asked", contracts.lost_update),
    ("cache_identity", "a cache key or storage that does not match what it caches", contracts.cache_identity),
    ("taint_alloc", "an allocation sized by bytes read with no bound", contracts.taint_alloc),
    ("composed_quadratic", "a walk over a member that grows with the loop around it", costmodel.composed_quadratic),
    ("redone_work", "a recomputation of inputs the slice never changed", costmodel.redone_work),
)


def detect(prog, slices):
    """Every class's findings, in the catalogue's order."""
    return [f for _, _, fn in CLASSES for f in fn(prog, slices)]
