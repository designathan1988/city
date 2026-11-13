"""What the program believes, read off its own code (after Engler et al., "Bugs as Deviant Behavior", SOSP 2001): the
points doing the same thing to the same target (writing one field, calling one operation of code not read) are
peers; a check most of them make before it (a condition on their path, or made by every caller on the way in) is a
belief of the program, and the peer that does not make it is suspect, ranked by how strong the belief is (the z
statistic of e peers checking out of n, against the share `PRIOR`). A check is compared across peers by its shape:
the condition with each input named by its last name (a field, a parameter, a member), so the same test on the same
member of different objects is one check. Each finding comes with a case where the suspect runs without it, and the
places where the peers check it.
"""
from __future__ import annotations

import math
from dataclasses import dataclass, field

from .conditions import conj, negate, state_of
from .decide import render, satisfy, symbols
from .limits import NONE as NO_CEILING
from .vocab import ALL, ANY, AT, DATA, NOT, OUTCALL, PARAM, SYMBOL, TEXT, WRITE

PRIOR = 0.9                      # the share of peers checking that makes the check the program's rule (Engler: 0.9)
LEAST = 2                        # the fewest peers that must check for it to be a belief
SHARE = 0.6                      # and the least share of peers
UP = 2                           # how many calls up a check made by every caller counts


@dataclass
class Belief:
    effect: int                  # the suspect: an index into the cases' effects
    check: str                   # the check its peers make, as a shape
    checked: int                 # how many peers make it
    peers: int                   # how many peers in all
    z: float
    where: list = field(default_factory=list)    # where the peers make it
    case: object = None          # a case where the suspect runs without it


def z_score(e: int, n: int, p0: float = PRIOR) -> float:
    return (e / n - p0) / math.sqrt(p0 * (1 - p0) / n)


def last_name(shown: str) -> str:
    return shown.replace("()", "").split(".")[-1].split("::")[-1].split("->")[-1].strip()


def shape(x) -> str:
    """A condition with each input named by its last name."""
    if isinstance(x, tuple):
        if x[0] == SYMBOL:
            return last_name(str(x[2]))
        if x[0] == AT:
            return shape(x[2])
        if x[0] in (TEXT, DATA):
            return render(x)
        return "(" + x[0] + " " + " ".join(shape(y) for y in x[1:]) + ")"
    return render(x)


def conjuncts(x) -> list:
    """The conditions a guard is made of, each as written (with its place)."""
    if x is True:
        return []
    if isinstance(x, tuple) and x[0] == ALL:
        return [c for y in x[1:] for c in conjuncts(y)]
    if isinstance(x, tuple) and x[0] == AT and isinstance(x[2], tuple) and x[2][0] == ALL:
        return [c if isinstance(c, tuple) and c[0] == AT else (AT, x[1], c) for c in conjuncts(x[2])]
    if isinstance(x, tuple) and x[0] == NOT and isinstance(x[1], tuple) and x[1][0] == AT and \
            isinstance(x[1][2], tuple) and x[1][2][0] == ANY:        # none of them: each is not
        return [(AT, x[1][1], negate(y)) for y in x[1][2][1:]]
    return [x]


def places(x) -> list:
    if isinstance(x, tuple):
        if x[0] == AT:
            return [x[1]] + places(x[2])
        return [w for y in x[1:] for w in places(y)]
    return []


class Peers:
    def __init__(self, result):
        self.r = result
        self.memo = {}
        self.names = {}              # a check's shape -> the names it reads
        self.params = {}             # a function -> the names of its parameters
        for d in result.program.defs:
            if d.kind == PARAM:
                self.params.setdefault(d.owner, set()).add(d.name)

    def checks(self, unit: int, depth: int = 0) -> dict:
        """The checks made on every way into a function (by every caller, `UP` calls up): shape -> places."""
        key = (unit, depth)
        if key in self.memo:
            return self.memo[key]
        self.memo[key] = {}
        rows = [row for (src, dst), found in self.r.cases.passages.items() if dst == unit for row in found]
        if not rows or depth >= UP:
            return {}
        common = None
        for row in rows:
            mine = dict(self.of(row.guard))
            for k, w in self.checks(row.src, depth + 1).items():
                mine.setdefault(k, w)
            common = mine if common is None else {k: common[k] + w for k, w in mine.items() if k in common}
        self.memo[key] = common or {}
        return self.memo[key]

    def of(self, guard) -> dict:
        out = {}
        for c in conjuncts(guard):
            k = shape(c)
            out.setdefault(k, []).extend(places(c))
            self.names.setdefault(k, {last_name(str(v)) for v in symbols(c).values()})
        return out

    def readable(self, effect) -> set:
        """The names the code of an effect can read: its function's parameters, the fields of the class whose field
        it writes, what it reads already."""
        p = self.r.program
        out = {last_name(str(v)) for x in (effect.guard, effect.value) + tuple(effect.args)
               for v in symbols(x).values()}
        out |= self.params.get(effect.unit, set())
        d = state_of(effect.target)
        if d is not None and p.defs[d].owner >= 0:
            out |= set(p.names.get(p.defs[d].owner, {}))
        return out

    def made(self, effect) -> dict:
        """Every check made before an effect: on its own path and on every way into its function."""
        out = self.of(effect.guard)
        for k, w in self.checks(effect.unit).items():
            out.setdefault(k, w)
        return out


def groups(result) -> list:
    """The peers: effects writing the same field (or the same name where no field is known), or calling the same
    operation of code not read."""
    found = {}
    for i, e in enumerate(result.cases.effects):
        if e.kind == WRITE:
            d = state_of(e.target)
            found.setdefault((WRITE, d if d is not None else e.name), []).append(i)
        elif e.kind == OUTCALL:
            found.setdefault((OUTCALL, e.name), []).append(i)
    return [rows for _, rows in sorted(found.items(), key=lambda kv: str(kv[0])) if len(rows) > LEAST]


def deviants(result, units=None, ceiling=NO_CEILING) -> list:
    """Every suspect, the strongest belief first."""
    peers = Peers(result)
    out = []
    for rows in groups(result):
        if ceiling.over():
            ceiling.skip(f"checks among {len(rows)} peers of {result.cases.effects[rows[0]].name}")
            continue
        made = {i: peers.made(result.cases.effects[i]) for i in rows}
        n = len(rows)
        shapes = {}
        for i in rows:
            for k, w in made[i].items():
                shapes.setdefault(k, []).append((i, w))
        for k, holders in shapes.items():
            e = len(holders)
            if e < LEAST or e == n or e / n < SHARE:
                continue
            where = sorted({w for _, ws in holders for w in ws})
            z = z_score(e, n)
            for i in rows:
                effect = result.cases.effects[i]
                if k not in made[i] and peers.names.get(k, set()) <= peers.readable(effect):
                    out.append(Belief(i, k, e, n, z, where, satisfy(effect.guard)))
    if units is not None:                        # only the suspects a trace runs (the peers are all of them)
        out = [b for b in out if result.cases.effects[b.effect].unit in units]
    out.sort(key=lambda b: -b.z)
    return out


def text_report(result, limit: int = 100, units=None, ceiling=NO_CEILING) -> str:
    from .situations import effect_text, runs_when
    found = deviants(result, units, ceiling)
    lines = [f"Checks most peers make before the same effect, missing at one ({len(found)}):"]
    for b in found[:limit]:
        lines.append(f"  z={b.z:.2f}  {effect_text(result, b.effect)}")
        lines.append(f"      does not check {b.check}  (checked by {b.checked} of {b.peers} peers, at "
                     f"{', '.join(b.where[:6])}{' ...' if len(b.where) > 6 else ''})")
        if b.case is not None and not isinstance(b.case, str):
            lines.append(f"      runs so when: {b.case.text()}")
        lines += [f"      reached from {way}" for way in runs_when(result, b.effect)]
    return "\n".join(lines)


def data(result, units=None, ceiling=NO_CEILING) -> list:
    from .situations import effect_text, runs_when
    return [{"the effect": effect_text(result, b.effect), "missing check": b.check, "checked by": b.checked,
             "peers in all": b.peers, "the z score": round(b.z, 3), "checked at": b.where,
             "the case": b.case.text() if b.case is not None and not isinstance(b.case, str) else None,
             "reached from": runs_when(result, b.effect)}
            for b in deviants(result, units, ceiling)]
