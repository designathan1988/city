"""Cycles of identity subscriptions kept as one node (log/trace-rules.md, "Collapsing cycles of identical values" and
"When the cycles are looked for").

Nodes on a cycle of `same` subscriptions always hold the same set, so the solver keeps them as one: Tarjan's algorithm
over the identity edges finds the strongly connected components (Pearce, Kelly and Hankin collapse them this way once
per pass of their solver). The solver here is a worklist, so a sweep runs when the identity edges have grown
`SWEEP_GROWTH` times since the last one: the sweeps together cost a bounded multiple of the final graph.

A merge is exact: each member's subscriptions get only the values of the union that member lacked; a difference still
waiting for a member goes to that member's subscriptions as they were; a consumer that read a member is owed exactly
the values of the union it had not read, one that read the representative keeps its place in it (its list only grows).
"""
from __future__ import annotations

from collections import deque

from . import solver

SWEEP_GROWTH = 2           # identity edges multiply by this between two sweeps (geometric: linear in total)


def sweep(s) -> int:
    """Merges every cycle of identity subscriptions present; returns how many nodes were merged into others."""
    merged, before = 0, {}
    for members in components(s):
        before.update(collapse(s, members))
        merged += len(members) - 1
    if before:                             # a difference waiting for a member goes where that member's went
        s.pending = deque((s.find(node), values, before[node] if only is None and node in before else only)
                          for node, values, only in s.pending)
    s.sweep_at = SWEEP_GROWTH * s.same_edges
    return merged


def components(s) -> list[list]:
    """The strongly connected components of two or more nodes in the graph of identity subscriptions, in the order
    found (Tarjan's algorithm with an explicit stack: a chain can be longer than the interpreter's recursion)."""
    same, find = solver.SAME, s.find

    def successors(node):
        out = []
        for target, key in s.subs.get(node, ()):
            if key == same:
                target = find(target)
                if target != node:
                    out.append(target)
        return out

    index, low, on_stack, stack, found = {}, {}, set(), [], []
    for root in list(s.subs):
        if root in index or find(root) != root:
            continue
        index[root] = low[root] = len(index)
        stack.append(root)
        on_stack.add(root)
        walk = [(root, iter(successors(root)))]
        while walk:
            node, children = walk[-1]
            for child in children:
                if child not in index:
                    index[child] = low[child] = len(index)
                    stack.append(child)
                    on_stack.add(child)
                    walk.append((child, iter(successors(child))))
                    break
                if child in on_stack:
                    low[node] = min(low[node], index[child])
            else:
                walk.pop()
                if walk:
                    above = walk[-1][0]
                    low[above] = min(low[above], low[node])
                if low[node] == index[node]:
                    component = []
                    while True:
                        member = stack.pop()
                        on_stack.discard(member)
                        component.append(member)
                        if member == node:
                            break
                    if len(component) > 1:
                        found.append(component)
    return found


def collapse(s, members: list) -> dict:
    """Merges one component into its member holding the most values (the first on a tie). Returns each member's
    subscriptions as they were, the identity edges inside the component left out."""
    same, identity = solver.SAME, solver.identity
    inside = set(members)
    rep = max(members, key=lambda m: len(s.value.get(m, ())))
    before = {m: [(edge, transfer) for edge, transfer in s.subs.get(m, {}).items()
                  if not (edge[1] == same and s.find(edge[0]) in inside)] for m in members}
    held, known = s.value.setdefault(rep, []), s.known[rep]
    lacking = {m: s.known.get(m, set()) for m in members if m != rep}
    appended = []
    for m in lacking:
        for text, ctx in s.value.get(m, ()):
            key = identity(text, ctx)
            if key not in known:
                known.add(key)
                held.append((text, ctx))
                appended.append((text, ctx))
    if appended and before[rep]:
        s.pending.append((rep, appended, before[rep]))
    keys = [identity(text, ctx) for text, ctx in held]
    for m, had in lacking.items():
        lacked = [value for value, key in zip(held, keys) if key not in had]
        if lacked and before[m]:
            s.pending.append((rep, lacked, before[m]))
    for m in lacking:
        s.parent[m] = rep
    subs = {}
    for m in members:                      # the merged subscriptions, each target as the node that holds it now
        for (target, key), transfer in s.subs.pop(m, {}).items():
            target = s.find(target)
            if not (key == same and target == rep):
                subs.setdefault((target, key), transfer)
    s.subs[rep] = subs
    seen = {}                              # a reader of a member -> the identities of the union it has read
    for m in lacking:
        held_m = s.value.get(m, [])
        for reader in s.readers.pop(m, ()):
            read = s.marks.pop((reader, m), 0)
            still = {identity(text, ctx) for text, ctx in s.extra.get(reader, {}).pop(m, ())}
            seen.setdefault(reader, set()).update({identity(text, ctx) for text, ctx in held_m[:read]} - still)
            s.readers[rep].add(reader)
        s.value.pop(m, None)
        s.known.pop(m, None)
    for reader, had in seen.items():
        if (reader, rep) in s.marks:
            continue                       # it reads the representative too: the tail brings what it lacks
        owed = [value for value, key in zip(held, keys) if key not in had]
        s.marks[(reader, rep)] = len(held)
        if owed:                           # exactly the values of the union it has not read
            s.extra[reader][rep] = owed
            s.wake(reader, rep)
    if appended:
        s.grew(rep)
    return before
