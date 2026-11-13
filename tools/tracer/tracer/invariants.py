"""What stays true of the state across every write, found the way Houdini does (Flanagan & Leino, FME 2001): from
templates over the state (a field within a bound; two fields of one class in a relation; the elements next to each
other in a collection in a relation), seeded by the numbers the program itself compares that state with (its checks)
and the values it writes, every candidate is put to each point writing that state: does the write keep it (held
before, under the write's condition, it holds after)? A candidate most writes do not keep falls (and the others are
asked again without it); one that is kept by most is the program's, and a write that breaks it is reported with the
case that breaks it. Also: one value given at once to the same operation of two different objects in one function
(put in two containers).

A behaviour no point of the program states (a value wrong in the same way everywhere) is not found: an invariant is
only what most of the writes keep.
"""
from __future__ import annotations

from dataclasses import dataclass, field
from fractions import Fraction

from .conditions import at_index, cone, conj, negate, state_of, substitute
from .decide import UNDECIDED, linear, render, satisfy, symbols
from .limits import NONE as NO_CEILING
from .vocab import AT, ELEM, EQ, GE, GT, LE, LT, NE, NOT, OUTCALL, PARAM, SUB, SYMBOL, WRITE

RANGE, RELATION, NEIGHBORS = "a bound", "a relation of two fields", "neighbours in a collection"
MOST = 0.5                        # a candidate more writes than this share break falls
SEEDS = 4                         # the most constants one state is put to (the ones the program compares it with most)


@dataclass(frozen=True)
class Candidate:
    kind: str
    subject: tuple               # the fields (a bound, a relation), or the collection (neighbours)
    rel: str                     # GE or LE
    const: Fraction


@dataclass
class Broken:
    candidate: Candidate
    effect: int                  # the write breaking it (an index into the cases' effects)
    kept_by: int                 # how many writes keep it
    case: object
    seeds: list = field(default_factory=list)    # where the program compares the state so
    also: list = field(default_factory=list)     # the other way of the same relation, broken too


class Houdini:
    def __init__(self, result):
        self.r, self.p = result, result.program
        self.effects = result.cases.effects
        self.seeds = {}              # candidate subject -> {constant: places}
        self.by_state = None         # a state (a field, a collection) -> the writes of it
        self.asked = {}              # (candidate, write, the candidates held) -> the answer

    # ---- the state's inputs ----------------------------------------------------------------------------------------
    def field(self, d: int) -> tuple:
        it = self.p.defs[d]
        return (SYMBOL, d, self.p.label(d), f"{self.p.files[it.file].display}:{it.line}")

    @staticmethod
    def cell(owner, key: tuple, shown: str) -> tuple:
        return (SYMBOL, (ELEM, owner, key), shown, "")

    def formula(self, c: Candidate, cells=None):
        """A candidate as a formula on the state before a write (neighbours: of the two elements given)."""
        if c.kind == RANGE:
            return (c.rel, self.field(c.subject[0]), c.const)
        if c.kind == RELATION:
            return (c.rel, (SUB, self.field(c.subject[0]), self.field(c.subject[1])), c.const)
        left, right = cells
        return (c.rel, (SUB, right, left), c.const)

    # ---- seeds -----------------------------------------------------------------------------------------------------
    def atoms(self, x, where=(), out=None) -> list:
        """Every comparison a formula makes, with its places."""
        out = [] if out is None else out
        if isinstance(x, tuple) and x and x[0] not in (SYMBOL,):
            if x[0] == AT:
                return self.atoms(x[2], where + (x[1],), out)
            if x[0] in (LT, LE, GT, GE, EQ, NE):
                out.append((x, where))
            for y in x[1:]:
                self.atoms(y, where, out)
        return out

    def seed(self, subject: tuple, const: Fraction, where) -> None:
        self.seeds.setdefault(subject, {}).setdefault(const, set()).update(where)

    def gather(self) -> None:
        """The constants the program compares its state with, the values it writes, the first values."""
        formulas = [x for e in self.effects for x in (e.guard, e.value) + tuple(e.args)]
        formulas += [row.guard for rows in self.r.cases.passages.values() for row in rows]
        for x in formulas:
            for (head, a, b), where in self.atoms(x):
                form = linear((SUB, a, b), {})
                if form is None:
                    continue
                coeffs, const = form
                keys = list(coeffs)
                # The state the comparison is about, the other inputs set aside (`s.lo + d > s.hi`: lo, hi).
                state = sorted(k for k in keys if isinstance(k, int) and self.p.defs[k].kind != PARAM)
                if len(state) == 1:
                    self.seed((state[0],), -const / coeffs[state[0]], where)
                elif len(state) == 2 and coeffs[state[0]] == -coeffs[state[1]]:
                    self.seed((state[0], state[1]), -const / coeffs[state[0]], where)
                elif len(keys) == 2 and all(isinstance(k, tuple) and k[0] == ELEM for k in keys) and \
                        keys[0][1] == keys[1][1] and coeffs[keys[0]] == -coeffs[keys[1]]:
                    one, two = keys
                    if at_index(one[2], 1) == two[2]:
                        self.seed((ELEM, one[1]), -const / coeffs[two], where)
                    elif at_index(two[2], 1) == one[2]:
                        self.seed((ELEM, one[1]), -const / coeffs[one], where)
        for e in self.effects:
            if e.kind != WRITE:
                continue
            d = state_of(e.target)
            if d is not None and isinstance(strip(e.value), Fraction):
                self.seed((d,), strip(e.value), ())
            elif isinstance(e.target, tuple) and e.target and e.target[0] == ELEM:
                self.seed((ELEM, e.target[1]), Fraction(0), ())

    def candidates(self) -> list:
        out = []
        first = self.r.cases.first or {}
        for subject, consts in self.seeds.items():
            kind = NEIGHBORS if subject[0] == ELEM else RANGE if len(subject) == 1 else RELATION
            # The constants the program compares that state with most (and its first value): a few, not every one.
            ranked = sorted(consts, key=lambda c: (-len(consts[c]), c))[:SEEDS]
            if kind == RANGE and isinstance(first.get(subject[0]), Fraction) and first[subject[0]] in consts:
                ranked = list(dict.fromkeys(ranked + [first[subject[0]]]))
            for const in ranked:
                for rel in (GE, LE):
                    out.append(Candidate(kind, subject if kind != NEIGHBORS else (subject[1],), rel, const))
        return out

    # ---- the writes ------------------------------------------------------------------------------------------------
    def writes_of(self, c: Candidate) -> list:
        if self.by_state is None:                # the writes by the state they write, once
            self.by_state = {}
            for i, e in enumerate(self.effects):
                if e.kind != WRITE:
                    continue
                if isinstance(e.target, tuple) and e.target and e.target[0] == ELEM:
                    if e.args:
                        self.by_state.setdefault(e.target, []).append(i)
                elif state_of(e.target) is not None:
                    self.by_state.setdefault(state_of(e.target), []).append(i)
        if c.kind == NEIGHBORS:
            return self.by_state.get((ELEM, c.subject[0]), [])
        return sorted({i for d in c.subject for i in self.by_state.get(d, ())})

    def first_kept(self, c: Candidate) -> bool:
        """Whether the first values the state is declared with keep a candidate (none declared: it may)."""
        if c.kind == NEIGHBORS:
            return True
        first = self.r.cases.first or {}
        given = {d: first[d] for d in c.subject if isinstance(first.get(d), Fraction)}
        if len(given) < len(c.subject):
            return True
        return satisfy((NOT, substitute(self.formula(c), given))) is None

    def breaks(self, c: Candidate, i: int, alive: list):
        """A case where a write breaks a candidate the state held before, or None."""
        e = self.effects[i]
        if c.kind == NEIGHBORS:
            key = index_of(e.args[0])
            owner = c.subject[0]
            base, named = render(e.args[0]), render_owner(self.p, owner)
            left = self.cell(owner, at_index(key, -1), f"{named}[{base} - 1]")
            old = self.cell(owner, key, f"{named}[{base}]")
            right = self.cell(owner, at_index(key, 1), f"{named}[{base} + 1]")
            before = conj([self.formula(c, (left, old)), self.formula(c, (old, right))])
            after = conj([self.formula(c, (left, e.value)), self.formula(c, (e.value, right))])
        else:
            d = state_of(e.target)
            before = self.formula(c)
            after = substitute(before, {d: e.value})
        question = conj([before, cone(e.guard, set(symbols(conj([before, after])))), negate(after)])
        return satisfy(question)

    def run(self, ceiling=NO_CEILING) -> tuple:
        """The candidates kept, and every write breaking one most writes keep. Each candidate is put to each write on
        its own (held before the write, under the part of the write's condition about what it reads, it holds after):
        one pass, each answer standing by itself. A candidate falls as soon as most writes break it, or a write
        checking that state before writing breaks it (the program's own care does not keep it)."""
        self.gather()
        alive, broken, said = [], [], {}
        for c in self.candidates():
            writes = self.writes_of(c)
            if not writes or not self.first_kept(c):
                continue
            if ceiling.over():
                ceiling.skip(f"invariant {self.shown(c)} ({len(writes)} writes)")
                continue
            breakers, fell = [], False
            for i in writes:
                case = self.breaks(c, i, None)
                if not self.found(case):
                    continue
                if self.careful(c, i) or len(breakers) + 1 > MOST * len(writes):
                    fell = True
                    break
                breakers.append((i, case))
            if fell or len(breakers) == len(writes):
                continue
            alive.append(c)
            seeds = sorted(self.seeds.get(c.subject if c.kind != NEIGHBORS else (ELEM, c.subject[0]),
                                          {}).get(c.const, ()))
            for i, case in breakers:
                again = said.get((i, c.kind, c.subject, c.const))
                if again is not None:                    # the other way of the same relation: one finding
                    again.also.append(c)
                    continue
                found = Broken(c, i, len(writes) - len(breakers), case, seeds)
                said[(i, c.kind, c.subject, c.const)] = found
                broken.append(found)
        return alive, broken

    def careful(self, c: Candidate, i: int) -> bool:
        """Whether a write checks the state a candidate is about before writing it."""
        keys = set(symbols(self.effects[i].guard))
        if c.kind == NEIGHBORS:
            return any(isinstance(k, tuple) and k and k[0] == ELEM and k[1] == c.subject[0] for k in keys)
        return bool(keys & set(c.subject))

    @staticmethod
    def found(case) -> bool:
        return case is not None and case != UNDECIDED

    def broken_text(self, b: Broken) -> str:
        return " and ".join(self.shown(c) for c in [b.candidate] + b.also)

    def shown(self, c: Candidate) -> str:
        if c.kind == NEIGHBORS:
            return f"{render_owner(self.p, c.subject[0])}[i + 1] - {render_owner(self.p, c.subject[0])}[i] " \
                   f"{c.rel} {render(c.const)}"
        return render(self.formula(c))


def by_field(alive: list) -> dict:
    """The candidates alive by each field they are about."""
    out = {}
    for c in alive:
        for d in c.subject:
            out.setdefault(d, []).append(c)
    return out


def strip(x):
    while isinstance(x, tuple) and x and x[0] == AT:
        x = x[2]
    return x


def index_of(x) -> tuple:
    from .conditions import index_key
    return index_key(x)


def render_owner(p, owner) -> str:
    return p.label(owner) if isinstance(owner, int) else str(owner)



def containers(result, ceiling=NO_CEILING) -> list:
    """One value given at once to the same operation of two different objects in one function: (the two calls, the
    case)."""
    effects = result.cases.effects
    by = {}
    for i, e in enumerate(effects):
        if e.kind == OUTCALL and e.args:
            by.setdefault((e.unit, e.file, e.name), []).append(i)
    out = []
    for rows in by.values():
        if ceiling.over():
            ceiling.skip(f"one value in two: {len(rows)} calls of {effects[rows[0]].name}")
            continue
        for a in range(len(rows)):
            for b in range(a + 1, len(rows)):
                ea, eb = effects[rows[a]], effects[rows[b]]
                if ea.target == eb.target:
                    continue
                if not set(map(render, ea.args)) & set(map(render, eb.args)):
                    continue
                case = satisfy(conj([ea.guard, eb.guard]))
                if case is not None and case != UNDECIDED:
                    out.append((rows[a], rows[b], case))
    return out


def report(result, units=None, ceiling=NO_CEILING) -> tuple:
    h = Houdini(result)
    alive, broken = h.run(ceiling)               # over every write: the invariant is the whole program's
    both = containers(result, ceiling)
    if units is not None:                        # only the writes a trace runs are reported
        effects = result.cases.effects
        broken = [b for b in broken if effects[b.effect].unit in units]
        both = [one for one in both if effects[one[0]].unit in units]
    return h, alive, broken, both


def text_report(result, limit: int = 100, units=None, ceiling=NO_CEILING) -> str:
    from .situations import effect_text, runs_when
    h, alive, broken, both = report(result, units, ceiling)
    lines = [f"Invariants the writes keep ({len(alive)}):"]
    lines += [f"  {h.shown(c)}" for c in alive[:limit]]
    lines.append(f"Writes breaking an invariant the others keep ({len(broken)}):")
    for b in broken[:limit]:
        lines.append(f"  {effect_text(result, b.effect)}")
        lines.append(f"      breaks {h.broken_text(b)} ({b.candidate.kind}; kept by {b.kept_by} other writes"
                     + (f"; the program compares so at {', '.join(b.seeds[:6])}" if b.seeds else "") + ")")
        if b.case is not None and b.case != UNDECIDED:
            lines.append(f"      when: {b.case.text()}")
        lines += [f"      reached from {way}" for way in runs_when(result, b.effect)]
    lines.append(f"One value given to two objects' same operation at once ({len(both)}):")
    for a, b, case in both[:limit]:
        lines.append(f"  {effect_text(result, a)}")
        lines.append(f"  {effect_text(result, b)}")
        lines.append(f"      when: {case.text()}")
        lines += [f"      reached from {way}" for way in runs_when(result, a)]
    return "\n".join(lines)


def data(result, units=None, ceiling=NO_CEILING) -> dict:
    from .situations import effect_text, runs_when
    h, alive, broken, both = report(result, units, ceiling)
    return {"invariants kept": [h.shown(c) for c in alive],
            "writes breaking one": [{"the write": effect_text(result, b.effect), "what it breaks": h.broken_text(b),
                                     "the kind": b.candidate.kind, "kept by": b.kept_by, "compared at": b.seeds,
                                     "the case": b.case.text() if b.case not in (None, UNDECIDED) else None,
                                     "reached from": runs_when(result, b.effect)}
                                    for b in broken],
            "one value in two": [{"the calls": [effect_text(result, a), effect_text(result, b)],
                                  "the case": case.text(), "reached from": runs_when(result, a)}
                                 for a, b, case in both]}
