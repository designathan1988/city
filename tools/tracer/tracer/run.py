"""The whole program's run: the flow (`flow.Flow`, the values of the expressions) wired from every definition's own
words and from every assignment, return and call the files write, and run until nothing is new; then, innermost first,
each member read whose object is still unknown widened by name; then what the calls no definition answers were given,
handed to the code not read. `analyse` asks it of a flow and gets the result."""
from __future__ import annotations

from .model import spelled, text
from .vocab import (ARGS, ASSIGN, CALL, CHAIN, EXPR, FUNCTION, KEY, LEFT, NAME, NEW, OBJECT, RET, RETURN, RIGHT, STORED,
                    TYPE, VALUE, VAR)


def define(fl, d) -> None:
    """What a definition holds from its own words: its value, the property it takes, its declared type."""
    f, parts = fl.p.files[d.file], d.parts
    if d.kind not in STORED and d.kind != FUNCTION:
        return
    target = (RET if d.kind == FUNCTION else VAR, d.id)
    if d.kind in STORED and VALUE in parts:
        fl.s.follow(fl.ev(f, parts[VALUE]), target)
    if d.kind in STORED and OBJECT in parts and NAME in parts:
        fl.rd.read(f, d.node, fl.ev(f, parts[OBJECT]), spelled(parts.get(KEY, parts[NAME])), target)
    if d.remade:
        at = (EXPR, f.index, d.node.id)
        fl.seen.add(at)
        names = [one.named_children[0] for one in parts[ARGS].named_children]      # each one name: a value
        fl.calls.site(f, d.node, NEW, fl.ev(f, parts[TYPE]), names, at, text(parts[TYPE]))
        fl.s.follow(at, target)
    if TYPE in parts:
        fl.typed(f, parts[TYPE], target)
        if not f.lang.duck_typed:        # declared as values the language writes in place: no object of a class
            if not any(fl.sc.types(f, parts[TYPE])):
                fl.s.plain.add(target)


def through(fl) -> None:
    """What the calls no definition answers give back and give the functions they are handed, run out."""
    while fl.calls.through():
        fl.drain()


def build(fl) -> None:
    for d in fl.p.defs:
        define(fl, d)
    for f in fl.p.files:
        for node, parts in f.facts.get(ASSIGN, ()):
            fl.st.store(fl.st.place(f, parts[LEFT]), fl.ev(f, parts[RIGHT]), f, node)
        for node, parts in f.facts.get(RETURN, ()):
            fn = fl.sc.function_at(f, node)
            if fn is not None and VALUE in parts:
                fl.s.follow(fl.ev(f, parts[VALUE]), (RET, fn))
        for role in (CALL, NEW, CHAIN):
            for node, _ in f.facts.get(role, ()):
                fl.ev(f, node)


def solve(fl, say=lambda words: None):
    """Wires the program and runs it out; then, innermost first, widens each member read whose object is still
    unknown; then hands to the code not read what the calls no definition answers were given."""
    def count() -> str:
        return f"{sum(len(held) for held in fl.s.values.values())} values, {len(fl.calls.links)} links"
    build(fl)
    say(f"wired {len(fl.queue)} expressions to start from")
    fl.drain()
    say(f"{len(fl.seen)} expressions, {len(fl.calls.sites)} calls, {len(fl.rd.all)} member reads: {count()}")
    through(fl)
    say(f"what code not read was given, given back: {count()}")
    fl.st.unknown()
    while True:
        waiting = fl.rd.unknown(fl.closed)
        if not waiting:
            break
        fl.drain()
        through(fl)
        say(f"{len(waiting)} member reads of objects nothing is known of, found by name: {count()}")
    for site in list(fl.calls.sites):
        if not site.exact:
            fl.calls.hand(site)
    fl.drain()
    through(fl)
    fl.st.unknown()                      # what the calls handed over was written too
    fl.drain()
    return fl.calls.result()
