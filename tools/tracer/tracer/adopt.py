"""Definitions written out of their scope, given to it (a step of building the program, `program.Program`): a
namespace under the namespaces its name writes (made where no file writes them alone), then every other one under the
class or namespace its name's scopes name, the one its file can see first; a namespace written in several places is
one scope, its parts keeping one table of names; a method named as its class makes the class's objects where the
language says so (`languages`), and every function that makes a class's objects is listed as one of its makers."""
from __future__ import annotations

from collections import defaultdict

from .vocab import CLASS, FUNCTION, NAMESPACE


def held_scopes(p, c: int, name: str) -> list:
    return [i for i in p.held(c, name) if p.defs[i].kind in (CLASS, NAMESPACE)]


def move(p, d, owner: int) -> None:
    if d.id in p.names[d.owner].get(d.name, ()):
        p.names[d.owner][d.name].remove(d.id)
    d.owner, d.adopted = owner, True
    p.names[owner][d.name].append(d.id)


def adopt(p) -> None:
    for d in [d for d in p.defs if d.kind == NAMESPACE and d.scopes]:
        holder = d.owner
        for part in d.scopes:
            found = [i for i in p.names[holder].get(part, ()) if p.defs[i].kind == NAMESPACE]
            if not found:
                made = p.new(NAMESPACE, part, p.files[d.file], d.node)
                made.owner = holder
                p.names[holder][part].append(made.id)
                found = [made.id]
            holder = found[0]
        move(p, d, holder)
    groups, known = defaultdict(list), defaultdict(list)
    for d in p.defs:
        if d.kind == NAMESPACE:
            groups[p.label_of(d)].append(d.id)
        if d.kind in (CLASS, NAMESPACE) and d.name:
            known[d.name].append(d.id)
    for ids in groups.values():
        shared = p.names[ids[0]]
        for other in ids[1:]:
            for name, held in p.names[other].items():
                shared[name] += held
            p.names[other] = shared
    for d in [d for d in p.defs if d.kind != NAMESPACE and d.scopes]:
        found = None
        for part in d.scopes:
            if found is None:
                cands = [i for c in p.chain(d.owner) for i in held_scopes(p, c, part)] or known.get(part, [])
            else:
                cands = held_scopes(p, found, part)
            seen = [i for i in cands if p.sources.sees(d.file, p.defs[i].file)]    # the one it can see first
            found = (seen or cands)[0] if cands else None
            if found is None:
                break
        if found is not None:
            move(p, d, found)
    for d in p.defs:
        if d.kind != FUNCTION:
            continue
        owner = p.defs[d.owner]
        if p.files[d.file].lang.class_named_constructor and owner.kind == CLASS and d.name == owner.name:
            d.constructor = True
            if d.id in p.names[d.owner][d.name]:
                p.names[d.owner][d.name].remove(d.id)
        if d.constructor and p.is_class(d.owner):
            p.constructors[d.owner].append(d.id)
