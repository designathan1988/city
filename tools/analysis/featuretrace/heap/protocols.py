"""The language's protocols over objects (log/trace-core-plan.md, Task 4; ECMA-262 GetIterator, IteratorNext,
IteratorValue; Await, PromiseResolve, Promise Resolve Functions, NewPromiseResolveThenableJob): iterating a value (a
`for...of`, a spread, an array pattern) calls its @@iterator method, the iterator's `next`, and reads each result's
`value` — the program's own iterators and the platform's declared ones alike; besides, an array's elements. Awaiting a
value gives it as it is unless it is a thenable (its type has a `then`): then its `then` is called with a resolving
function, and what that is called with is the awaited value, itself settled the same way.
"""
from __future__ import annotations

from ..languages import ITERATOR, NEXT, THEN, VALUE
from ..vocab import CALL, ELEMENTS_FIELD, RESULT_FIELD, SITE_PROMISE, SITE_RESOLVER
from . import members, typesys

ITERATOR_CALL, NEXT_CALL, THEN_CALL, PROBE, RESOLVING, SETTLE = 40, 41, 42, 43, 44, 45      # node tags


def iterate(an, node, source) -> None:
    """`node` holds what iterating the values of `source` gives (`node` a tree's node: its context last)."""
    solver = an.solver
    first, second = (ITERATOR_CALL,) + node[1:], (NEXT_CALL,) + node[1:]
    solver.subscribe(source, node, (ELEMENTS_FIELD,), lambda t, objs: [
        solver.follow(members.field(an, o, ELEMENTS_FIELD), t) for o in objs])
    solver.subscribe(source, first, (ITERATOR,), lambda t, objs: [method(an, o, ITERATOR, t, []) for o in objs])
    solver.subscribe(first, second, (NEXT,), lambda t, objs: [method(an, o, NEXT, t, []) for o in objs])
    solver.subscribe(second, node, (VALUE,), lambda t, objs: [members.read(an, t, o, VALUE) for o in objs])


def method(an, obj: int, name: str, call, args: list) -> None:
    """`obj.name(...args)` into the call node `call`."""
    probe = (PROBE, obj, name, call)
    members.read(an, probe, obj, name)
    an.solver.subscribe(probe, call, (CALL, obj), lambda t, fns: [an.call(t, f, obj, args, CALL) for f in fns])


def await_(an, node, source) -> None:
    """`node` holds what awaiting the values of `source` gives."""
    settle(an, node, source)


def settle(an, node, source) -> None:
    """What `source`'s values resolve `node` to: a thenable's resolution, any other value itself."""
    an.solver.subscribe(source, (SETTLE, node), (SETTLE, source), lambda _, objs: [one(an, node, o) for o in objs])


def one(an, node, obj: int) -> None:
    if an.objects.kind(obj) == SITE_PROMISE:          # a promise the language made: its result
        an.solver.follow(members.field(an, obj, RESULT_FIELD), node)
        return
    if not typesys.has_member(an, obj, THEN):
        an.solver.add(node, [obj])
        return
    resolving = (RESOLVING, node)
    an.solver.add(resolving, [an.objects.make(SITE_RESOLVER, node)])
    method(an, obj, THEN, (THEN_CALL,) + node[1:], [resolving])
