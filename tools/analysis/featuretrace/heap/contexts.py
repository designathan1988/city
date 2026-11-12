"""Contexts (log/trace-core-plan.md, Task 3): what one run of a function is told apart by, finite by construction.

A context is a pair (environment, call). The environment is the context of the run that made the function object —
what its free variables read — so it nests only as deep as functions are written in one another. The call is the
call's site for a function, and for a method the receiver's allocation site with the receiver's heap context (Doop's
1-call-site, and 2-object with one level of heap context: "Pick Your Contexts Well", Smaragdakis, Bravenboer, Lhotak,
POPL 2011). An object made in a context carries one part of it, its heap context: the call's site, or for a method the
receiver's site. Module code runs in ROOT.
"""
from __future__ import annotations

ROOT = ()
ONCE = -1                  # the call part of a run every caller shares (`flows.contextual` is false)


def of_call(env, site: int) -> tuple:
    """A function run by the call at `site`, its free variables read in `env`."""
    return (env, site)


def of_method(env, site: int, heap) -> tuple:
    """A method run on a receiver made at `site` in heap context `heap`."""
    return (env, (site, heap))


def environment(ctx):
    """The context the free variables of a run are read in: the run that made its function object."""
    return ctx[0] if ctx else ROOT


def heap(ctx):
    """The heap context of an object made in `ctx`: the call's site, or the receiver's site for a method."""
    if not ctx:
        return ROOT
    call = ctx[1]
    return call[0] if isinstance(call, tuple) else call
