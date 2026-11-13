"""What happens in each situation, asked of the effects the code read: when each happens and what it writes, put in
the inputs of a start (through every way the start reaches it: each call's condition, each parameter given the
argument); and, of the effects that do the same thing to the same target (the same field or member written, the same
call of code not read: alternatives), whether some case makes none of them happen (none true, for truths written),
whether one makes more than one happen at once, whether a value leaves a range. Each answer a case: the values of
the inputs, and the places of the conditions it passes through.
"""
from __future__ import annotations

from dataclasses import dataclass, field
from fractions import Fraction

from .conditions import conj, disj, negate, substitute
from .decide import UNDECIDED, is_truth, render, satisfy
from .limits import NONE as NO_CEILING
from .vocab import AT, GT, ITE, LT, NE, OUTCALL, SYMBOL, WRITE

PATHS = 32                       # the most ways from a start to a function one formula puts together
DEPTH = 24                       # the most calls on one way
NONE_HAPPENS = "no one of them happens"
NONE_TRUE = "no one of them writes true"
MANY = "more than one at once"
ALWAYS_ONE = "exactly one, always"
OUT_OF_RANGE = "a value out of the range"
NOT_DECIDED = "not decided (too many cases)"


@dataclass
class Finding:
    question: str
    effects: list                # the effects it is about (indices into the cases' effects)
    case: object = None          # the case found (`decide.Case`), None where none exists
    note: str = ""
    where: list = field(default_factory=list)


def reach(result, starts) -> set:
    """Every definition a start runs, by any link."""
    seen, todo = set(starts), list(starts)
    while todo:
        at = todo.pop()
        for link in result.out.get(at, ()):
            if link.dst not in seen:
                seen.add(link.dst)
                todo.append(link.dst)
    return seen


def ways(result, starts, unit: int, formula, reached: set) -> list:
    """A formula in a function's inputs, put in the inputs of the starts, one way from them to it at a time: (the
    calls' conditions on the way, the formula with each parameter given what the call gives it and each state given
    what the code before the call on the way wrote in it)."""
    if not starts:
        return [(True, formula)]
    passages = result.cases.passages
    out = []

    def up(at: int, when, phi, path: frozenset) -> None:
        if len(out) >= PATHS:
            return
        if at in starts:
            out.append((when, phi))
            return
        if len(path) > DEPTH:
            return
        for link in result.into.get(at, ()):
            src = link.src
            if src in path or src not in reached:
                continue
            for row in passages.get((src, at)) or [None]:
                if row is None:
                    up(src, when, phi, path | {src})
                    continue
                given = {**row.state, **row.args}
                up(src, conj([row.guard, substitute(when, given)]), substitute(phi, given), path | {src})
    up(unit, True, formula, frozenset({unit}))
    return out


def lifted(result, starts, unit: int, formula, reached: set):
    """When a condition holds, in the starts' inputs: on some way, the way's conditions and it."""
    return disj([conj([when, phi]) for when, phi in ways(result, starts, unit, formula, reached)])


def lifted_value(result, starts, unit: int, formula, reached: set):
    """What a value is, in the starts' inputs: on each way, the value that way gives."""
    found = ways(result, starts, unit, formula, reached)
    if not found:
        return formula
    out = found[-1][1]
    for when, phi in reversed(found[:-1]):
        out = phi if phi == out else (ITE, when, phi, out)
    return out


def groups(result, starts=None, name: str | None = None) -> list:
    """The alternatives: the effects writing the same name in two or more targets (sibling objects, sibling fields),
    or calling code not read of the same name on two or more objects, one of them at least under a condition or
    writing what a condition decides; among what the starts run (named: the effects of that name, whatever they)."""
    reached = reach(result, starts) if starts else None
    found = {}
    for i, e in enumerate(result.cases.effects):
        if e.kind not in (WRITE, OUTCALL) or (reached is not None and e.unit not in reached):
            continue
        if name is not None and e.name != name:
            continue
        found.setdefault((e.kind, e.name), []).append(i)
    out = []
    for key, rows in sorted(found.items()):
        effects = [result.cases.effects[i] for i in rows]
        targets = {e.target for e in effects}
        decided = any(e.guard is not True or (e.value is not None and not isinstance(e.value, (bool, Fraction))
                                               and is_truth(e.value)) for e in effects)
        if name is not None or (len(targets) >= 2 and decided):
            out.append((key, rows))
    return out


def truths(effects) -> bool:
    """Whether a group writes truths: one writes a truth, and every other a truth or an input."""
    values = [e.value for e in effects if e.value is not None]
    return bool(values) and any(is_truth(v) for v in values) and \
        all(is_truth(v) or (isinstance(v, tuple) and v[0] in (SYMBOL, AT)) for v in values)


def by_target(result, rows: list, starts, reached: set, boolean: bool) -> list:
    """Each target of a group: (its effects, when one of them happens, when it ends true). In one function the
    last write wins: its value, else the one before, else what the target was."""
    effects = result.cases.effects
    targets = {}
    for i in rows:
        targets.setdefault(effects[i].target, []).append(i)
    out = []
    for target, mine in targets.items():
        happens, on = [], []
        units = {}
        for i in mine:
            units.setdefault((effects[i].unit, effects[i].file), []).append(i)
        for (unit, _), here in units.items():
            here.sort(key=lambda i: effects[i].start)
            when = disj([effects[i].guard for i in here])
            happens.append(lifted(result, starts, unit, when, reached))
            if boolean:
                last = False
                for i in here:
                    e = effects[i]
                    last = e.value if e.guard is True else (ITE, e.guard, e.value, last)
                on.append(lifted(result, starts, unit, conj([when, last]), reached))
        out.append((mine, disj(happens), disj(on) if boolean else disj(happens)))
    return out


def ask(result, rows: list, starts=None, low=None, high=None) -> list:
    """The questions of one group of alternatives, answered: none (true, for truths written; happens, for calls),
    more than one at once, a value out of the range."""
    effects = [result.cases.effects[i] for i in rows]
    reached = reach(result, starts) if starts else set()
    boolean = all(e.kind == WRITE for e in effects) and truths(effects)
    targets = by_target(result, rows, starts, reached, boolean)
    out = []
    if boolean or all(e.kind == OUTCALL for e in effects):
        # None true: among the cases where the group runs; no call made: in any case.
        none = satisfy(conj(([disj([t[1] for t in targets])] if boolean else []) + [negate(t[2]) for t in targets]))
        out.append(answer(NONE_TRUE if boolean else NONE_HAPPENS, rows, none))
        many = False
        for a in range(len(targets)):
            for b in range(a + 1, len(targets)):
                case = satisfy(conj([targets[a][2], targets[b][2]]))
                if case is not None:
                    many = True
                    out.append(answer(MANY, targets[a][0] + targets[b][0], case))
        if none is None and not many and len(targets) >= 2:
            out.append(Finding(ALWAYS_ONE, rows))
    if low is not None or high is not None:
        for i, e in zip(rows, effects):
            if e.value is None or is_truth(e.value):
                continue
            value = lifted_value(result, starts, e.unit, e.value, reached)
            outside = disj(([(LT, value, low)] if low is not None else []) + ([(GT, value, high)] if high is not None
                                                                              else []))
            case = satisfy(conj([lifted(result, starts, e.unit, e.guard, reached), outside]))
            if case is not None:
                out.append(answer(OUT_OF_RANGE, [i], case))
    return [f for f in out if f is not None and (f.case is not None or f.question in (ALWAYS_ONE, NOT_DECIDED))]


def answer(question: str, rows: list, case) -> Finding | None:
    if case is None:
        return None
    if case == UNDECIDED:
        return Finding(NOT_DECIDED, rows, None, question)
    return Finding(question, rows, case, "", case.where)


def effect_text(result, i: int) -> str:
    p, e = result.program, result.cases.effects[i]
    return f"{p.files[e.file].display}:{e.line}  {e.shown}  (in {p.label(e.unit)})"


def runs_when(result, i: int, limit: int = 3) -> list:
    """How an effect is reached and what runs it: the roots its function's callers start from, each with why it runs
    and the way from it (calls, handlers registered, handlers fired)."""
    from .gaps import path_text, triggers
    from .walk import backward
    unit = result.cases.effects[i].unit
    return [f"{result.program.where(root)} [{why}] {path_text(result, path)}"
            for root, why, path in triggers(result, backward(result, [unit]))[:limit]]


def report(result, starts=None, name=None, low=None, high=None, ceiling=NO_CEILING) -> list:
    """Every group of alternatives and its answers: [(group, [Finding])]; past the ceiling, the groups left are
    said, not answered."""
    out = []
    for key, rows in groups(result, starts, name):
        if ceiling.over():
            ceiling.skip(f"alternatives {key[0]} {key[1]} ({len(rows)} effects)")
            continue
        out.append((key, rows, ask(result, rows, starts, low, high)))
    return out


def text_report(result, starts=None, name=None, low=None, high=None, ceiling=NO_CEILING) -> str:
    lines = ["Alternatives (effects doing the same to the same target) and what happens in each situation:"]
    for (kind, named), rows, findings in report(result, starts, name, low, high, ceiling):
        lines.append(f"  {kind} {named}:")
        reached = reach(result, starts) if starts else set()
        for i in rows:
            e = result.cases.effects[i]
            lines.append(f"    {effect_text(result, i)}")
            lines.append(f"        when {render(lifted(result, starts, e.unit, e.guard, reached))}"
                         + (f"; writes {render(lifted_value(result, starts, e.unit, e.value, reached))}"
                            if e.value is not None else ""))
        for f in findings:
            said = f"    -> {f.question}"
            if f.case is not None:
                said += f": {f.case.text()}" + ("" if f.case.whole else " (not whole numbers)")
                if f.where:
                    said += f"  [conditions at {', '.join(f.where)}]"
            if len(f.effects) < len(rows):
                said += "  {" + "; ".join(effect_text(result, i) for i in f.effects) + "}"
            lines.append(said)
    return "\n".join(lines)


def data(result, starts=None, name=None, low=None, high=None, ceiling=NO_CEILING) -> list:
    out = []
    for (kind, named), rows, findings in report(result, starts, name, low, high, ceiling):
        out.append({"what is done": kind, "done to": named,
                    "the effects": [effect_text(result, i) for i in rows],
                    "the answers": [{"what is asked": f.question,
                                     "the case": {f.case.shown.get(k, str(k)): str(v) for k, v in f.case.values.items()}
                                     if f.case is not None else None,
                                     "conditions at": f.where,
                                     "the effects": [effect_text(result, i) for i in f.effects]} for f in findings]})
    return out


def conditions_text(result, starts=None) -> str:
    """Every effect the starts run: when it happens and what it writes, in the starts' inputs."""
    reached = reach(result, starts) if starts else set()
    lines = ["What each effect does and when (in the inputs of the start):"]
    for i, e in enumerate(result.cases.effects):
        if starts and e.unit not in reached:
            continue
        when = lifted(result, starts, e.unit, e.guard, reached)
        lines.append(f"  {e.kind}  {effect_text(result, i)}")
        lines.append(f"      when {render(when)}")
        if e.value is not None:
            lines.append(f"      value {render(lifted_value(result, starts, e.unit, e.value, reached))}")
        for k, arg in enumerate(e.args):
            lines.append(f"      argument {k + 1}: {render(lifted_value(result, starts, e.unit, arg, reached))}")
    return "\n".join(lines)
