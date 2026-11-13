"""What happens in each situation: for every effect of the code read (a write of state or of a member of an object,
a call of code not read, a value given back), the formula of when it happens and of what it writes or gives, in the
inputs of the function it is written in: its parameters, the state as the last run left it, what code not read gives
back, a value nothing more is known of (each an input, `SYMBOL`). A condition is every test the code checks before
the effect: the branch it is in (its test holds, or, in the rest of the branch, does not), the cases of a choice, the
left side of `&&` and `||`, a branch before it in its block that leaves the function. A value follows the operators,
the constants written once, the variables set once, and what the function called gives back, its parameters given
the arguments. For every call between two functions, its condition and what it gives each parameter: a question
asked from a start puts the effects in the start's inputs through them. Nothing here names a program, a library or a
platform: only the words of the languages, read from their patterns and their registry.
"""
from __future__ import annotations

import bisect
import math
import struct
from dataclasses import dataclass, field
from fractions import Fraction
from pathlib import Path

from .decide import linear
from .languages import DATA_FORMATS
from .model import text, unquoted, child_nodes
from .read import listed
from .vocab import (ABS, ATAN, ATAN2, CEIL, CLAMP, COS, EXP, FLOOR, FRACT, FUNCTIONS, LOG, MAX, MIN, MIX, POW, REAL32,
                    ROUND, SIGN, SIN, SMOOTH, SQRT, STEP, TAN, TRUNC, UNIFORM)
from .vocab import (ADD, ALL, ANY, ASSIGN, AT, BRANCH, CALL, CASE, CHAIN, CHANGES, CHOOSE, CLS, DATA, DIV, ELEM, ELSE, EQ, EXIT, FIXED,
                    EXPR, FIRES, FUNCTION, GE, GIVES, GT, IDENT, INDEX, INST, ITE, KEY, LE, LEFT, LITERAL, LOOP, LT, MEMBER, READ_AT,
                    MOD, MUL, NE, NEG,
                    NEW, NOT, OBJECT, OP, OPERATION, OPERATIONS, OUT, OUTCALL, PARAM, PROP, RETURN, RIGHT, STORED, SUB,
                    PROPERTY, SHAPE, SYMBOL, TEST, TEXT, THEN, VALUE, VAR, VARIABLE, WRITE, WRITES)

DEPTH = 4                        # how many calls deep a value follows what the function called gives back
STOPPED = "stopped at the ceiling: what is left is not weighed"
LARGEST = 400                    # the most parts a state's value followed through a function's writes may have


@dataclass(slots=True)
class Effect:
    """One thing a function does that a question may be asked of."""
    kind: str                    # WRITE, OUTCALL, GIVES
    unit: int                    # the function (or class, or file) it is written in
    target: object               # what it is done to: the state definition (a number), a field and the object written
    #                              through ((the field, the object as written)), else as named (a text)
    name: str                    # the name written, called or given back by: what makes effects one group
    shown: str                   # as written
    file: int
    line: int
    guard: object                # when it happens
    value: object                # what it writes or gives back (a call: None)
    args: tuple = ()             # what a call gives each argument
    start: int = 0               # where in its file it is written (the byte it starts at)


@dataclass(slots=True)
class Passage:
    """One call from a function to another, as a question puts the second's inputs in the first's."""
    src: int
    dst: int
    kind: str
    file: int
    line: int
    guard: object
    args: dict                   # the parameter's definition -> what the call gives it
    state: dict = None           # the state definition -> what the calling function wrote in it before the call
    start: int = 0               # where in its file the call is written (the byte it starts at)


@dataclass(slots=True)
class Loop:
    """A loop summed up by intervals."""
    unit: int
    file: int
    start: int
    end: int
    body: tuple                  # where its body starts and ends
    where: str
    state: dict                  # a state definition it writes -> the way it moves it (+1, -1, 0)
    local: dict                  # a variable of its function it writes -> (the way, the first value or None)
    entry: dict = field(default_factory=dict)    # a state -> its value when the loop begins
    turn: dict = field(default_factory=dict)     # a state -> its value at a turn (an input)
    done: bool = False
    turns: object = None         # how many turns it makes, where known
    steps: dict = field(default_factory=dict)    # a state -> what each turn adds to it, where it is a number


@dataclass
class Cases:
    effects: list
    passages: dict               # (src, dst) -> [Passage]
    first: dict = None           # a state definition -> the value it is declared with
    partial: list = field(default_factory=list)  # (where, what) not followed on: the answers through it are partial
    uniforms: dict = field(default_factory=dict)  # a uniform's name -> where shaders declare it
    blocks: list = field(default_factory=list)   # (unit, file, line, when the code a branch runs is reached)
    keys_read: list = field(default_factory=list)    # (data file, keys) read
    keys_missing: list = field(default_factory=list)  # (where, data file, keys) read and not in the data
    data_files: list = field(default_factory=list)   # the data files read
    loops: list = field(default_factory=list)    # Turning: each loop, what it goes over, and whether that grows
    makes: list = field(default_factory=list)    # (unit, file, line, byte, what): an object or a collection made
    reads: list = field(default_factory=list)    # (unit, state): state read in the expressions weighed


@dataclass(slots=True)
class Turning:
    """A loop as its cost reads it."""
    unit: int
    file: int
    line: int
    start: int
    end: int
    body: tuple                  # where its body starts and ends (bytes)
    over: str                    # what it goes over, as written (its test, or the collection)
    bound: object                # the formula of what bounds it (the test, or the collection), in its inputs
    grows: bool                  # what bounds it comes from inputs, state or data (it grows with them)
    turns: object = None         # how many turns, where known
    lines: tuple = ()            # the lines its body starts and ends on


def conj(parts) -> object:
    parts = [x for x in parts if x is not True]
    if any(x is False for x in parts):
        return False
    return True if not parts else parts[0] if len(parts) == 1 else (ALL,) + tuple(parts)


def disj(parts) -> object:
    parts = [x for x in parts if x is not False]
    if any(x is True for x in parts):
        return True
    return False if not parts else parts[0] if len(parts) == 1 else (ANY,) + tuple(parts)


def negate(x) -> object:
    if isinstance(x, bool):
        return not x
    return x[1] if isinstance(x, tuple) and x[0] == NOT else (NOT, x)


def substitute(x, given: dict):
    """The formula with the inputs `given` names replaced by what it gives them."""
    if not given or not isinstance(x, tuple):
        return x
    if x[0] == SYMBOL:
        return given.get(x[1], x)
    if x[0] in (TEXT, DATA, READ_AT):            # (what is read where a variable is set: put in order apart)
        return x
    if x[0] == AT:
        return (AT, x[1], substitute(x[2], given))
    return (x[0],) + tuple(substitute(y, given) for y in x[1:])


def render_key(x) -> str:
    from .decide import render
    return render(x)


def index_key(x) -> tuple:
    """A position as a key: its linear form (the coefficients by input, sorted, and the constant), else as written."""
    from .decide import linear
    form = linear(x, {})
    if form is None:
        return (render_key(x),)
    return (tuple(sorted(((str(k), c) for k, c in form[0].items()))), form[1])


def at_index(key: tuple, step: int) -> tuple:
    """The key of the position `step` after (before, if negative) a position's."""
    if len(key) != 2:
        return (key[0] + (f" + {step}" if step >= 0 else f" - {-step}"),)
    return (key[0], key[1] + step)


def show_index(x) -> str:
    return render_key(x)


def settle(x, unit=None, file=None, then=None):
    """A formula with what is read where a variable is set put in place: in the function being put in order, as the
    state was there (`then(byte)`: the state by then); elsewhere, as the function found it."""
    if not isinstance(x, tuple) or not x or x[0] in (SYMBOL, TEXT, DATA):
        return x
    if x[0] == READ_AT:
        inner = settle(x[4], unit, file, then)
        return substitute(inner, then(x[3])) if then is not None and x[1] == unit and x[2] == file else inner
    if x[0] == AT:
        return (AT, x[1], settle(x[2], unit, file, then))
    return (x[0],) + tuple(settle(y, unit, file, then) for y in x[1:])


def larger(x, limit: int) -> bool:
    """Whether a formula, written out, has more parts than the limit (counted until it is passed)."""
    count, stack = 0, [x]
    while stack:
        y = stack.pop()
        count += 1
        if count > limit:
            return True
        if isinstance(y, tuple) and y and y[0] not in (SYMBOL, TEXT, DATA):
            stack.extend(y[1:])
    return False


def parts_of(x) -> list:
    """The conditions a guard is the conjunction of, each kept with its place."""
    if x is True:
        return []
    if isinstance(x, tuple) and x and x[0] == ALL:
        return [c for y in x[1:] for c in parts_of(y)]
    if isinstance(x, tuple) and x and x[0] == AT and isinstance(x[2], tuple) and x[2] and x[2][0] == ALL:
        return [c if isinstance(c, tuple) and c and c[0] == AT else (AT, x[1], c) for c in parts_of(x[2])]
    return [x]


def cone(guard, wanted: set):
    """The part of a guard about the inputs wanted: its conditions that share an input with them, or with a condition
    kept, step by step (the others are about other inputs and change no answer about these; one that holds never
    is kept: it says the code never runs)."""
    from .decide import symbols
    rest = [(c, set(symbols(c))) for c in parts_of(guard)]
    reach, kept = set(wanted), []
    changed = True
    while changed:
        changed = False
        for item in list(rest):
            c, read = item
            if not read and c is not False:
                rest.remove(item)
            elif c is False or read & reach:
                kept.append(c)
                reach |= read
                rest.remove(item)
                changed = True
    return conj(kept)


def strip_at(x):
    """A formula without the places it is written at, at its top."""
    while isinstance(x, tuple) and x and x[0] == AT:
        x = x[2]
    return x


def state_of(target) -> int | None:
    """The state definition an effect's target is (a field written through an object: the field)."""
    if isinstance(target, int):
        return target
    if isinstance(target, tuple) and target and isinstance(target[0], int):
        return target[0]
    return None


def number(raw: str):
    """A number as the languages write one (digits grouped by `_`, a suffix of letters saying its type), or None."""
    raw = raw.replace("_", "")
    for cut in range(len(raw), 0, -1):
        try:
            return Fraction(raw[:cut]) if cut == len(raw) or raw[cut:].isalpha() else None
        except (ValueError, ZeroDivisionError):
            try:
                return Fraction(int(raw[:cut], 0)) if cut == len(raw) or raw[cut:].isalpha() else None
            except ValueError:
                continue
    return None


def fold(head: str, a, b=None):
    """An operation on values known: done; else written as it is."""
    numbers = isinstance(a, Fraction) and (b is None or isinstance(b, Fraction))
    if head == NOT:
        return negate(a)
    if head == NEG:
        return -a if isinstance(a, Fraction) else (NEG, a)
    if head == ALL:
        return conj([a, b])
    if head == ANY:
        return disj([a, b])
    if numbers:
        try:
            return {ADD: lambda: a + b, SUB: lambda: a - b, MUL: lambda: a * b, DIV: lambda: a / b,
                    MOD: lambda: a % b, LT: lambda: a < b, LE: lambda: a <= b, GT: lambda: a > b,
                    GE: lambda: a >= b, EQ: lambda: a == b, NE: lambda: a != b}[head]()
        except (KeyError, ZeroDivisionError):
            pass
    return (head, a, b)


def number_function(head: str, values: list):
    """A function of numbers done on numbers known, by its definition (exact where it is: min, max, abs, floor, ceil,
    clamp, sign, round, trunc; else in 64-bit reals, as the languages do), or None where it is not defined there."""
    a = values[0]
    try:
        if head == MIN:
            return min(values)
        if head == MAX:
            return max(values)
        if head == ABS:
            return abs(a)
        if head == FLOOR:
            return Fraction(math.floor(a))
        if head == CEIL:
            return Fraction(math.ceil(a))
        if head == TRUNC:
            return Fraction(math.trunc(a))
        if head == ROUND:                        # half away from zero is C's; JS and GDScript round halves up
            return Fraction(math.floor(a + Fraction(1, 2)))
        if head == SIGN:
            return Fraction((a > 0) - (a < 0))
        if head == CLAMP:
            return min(max(a, values[1]), values[2])
        if head == STEP:                         # step(edge, x): 0 below the edge, else 1
            return Fraction(0) if values[1] < a else Fraction(1)
        if head == MIX:
            return a + (values[1] - a) * values[2]
        if head == FRACT:
            return a - math.floor(a)
        if head == REAL32:
            return Fraction(struct.unpack("<f", struct.pack("<f", float(a)))[0])
        if head == SMOOTH:
            t = min(max((values[2] - a) / (values[1] - a), Fraction(0)), Fraction(1))
            return t * t * (3 - 2 * t)
        real = {SQRT: lambda: math.sqrt(a), POW: lambda: math.pow(a, values[1]), SIN: lambda: math.sin(a),
                COS: lambda: math.cos(a), TAN: lambda: math.tan(a), ATAN: lambda: math.atan(a),
                ATAN2: lambda: math.atan2(a, values[1]), EXP: lambda: math.exp(a), LOG: lambda: math.log(a)}[head]()
        return Fraction(real)
    except (ValueError, ZeroDivisionError, OverflowError, KeyError):
        return None


def fold_function(head: str, given: list):
    """A function of numbers: done where its values are known, else written as it is (an input of its own to the
    linear part of the solver, decided by intervals)."""
    if all(isinstance(x, Fraction) for x in given):
        done = number_function(head, given)
        if done is not None:
            return done
    return (head,) + tuple(given)


class Weigh:
    def __init__(self, flow):
        self.fl, self.p, self.sc = flow, flow.p, flow.sc
        self.branches, self.chooses, self.cases, self.exits, self.ops = {}, {}, {}, set(), {}
        for f in self.p.files:
            for node, parts in f.facts.get(BRANCH, ()):
                self.branches[(f.index, node.id)] = parts
            for node, parts in f.facts.get(CHOOSE, ()):
                self.chooses[(f.index, node.id)] = parts
            for node, parts in f.facts.get(CASE, ()):
                self.cases.setdefault((f.index, node.id), {}).update(parts)
            for node, _ in f.facts.get(EXIT, ()):
                self.exits.add((f.index, node.id))
            for node, parts in f.facts.get(OPERATION, ()):
                self.ops[(f.index, node.id)] = parts
        self.site_at = {(site.f.index, site.node.id): site for site in flow.calls.sites}
        self.written = {d for (_, d), how in flow.touched.items() if how in (WRITES, CHANGES)}
        self.set_again = set()                      # the variables of a function written after they are declared
        for f in self.p.files:
            for node, parts in f.facts.get(ASSIGN, ()):
                for place in flow.st.place(f, parts[LEFT]) if LEFT in parts else ():
                    if place[0] == VAR:
                        self.set_again.add(place[1])
        self.gives, self.making = {}, set()
        self.data_files, self.loaded = None, {}
        self.partial = []                        # what is not followed on, and where
        self.keys_read, self.keys_missing = set(), []    # the keys of data read; the ones the data does not hold
        self.results = {}                        # (the symbol of what a call of code not read gives, where) -> callee
        self.statements = set()                  # (file, byte) of the calls of code not read whose value nothing takes
        self.reads, self.unit_now = set(), None  # (unit, state) read in the expressions weighed; the unit weighed now
        self.loops = [(f, node, parts) for f in self.p.files for node, parts in f.facts.get(BRANCH, ())
                      if LOOP in parts]

    # ---- data the program loads ----------------------------------------------------------------------------------
    def data_named(self, written: str) -> str | None:
        """The data file of a known format a text names, among the files the program holds: the text's path (past a
        scheme, `x://`) is the end of the file's."""
        if self.data_files is None:
            base = Path(self.p.base) if self.p.base else None
            self.data_files = [] if base is None else \
                [path for path in listed(base) if path.suffix.lower() in DATA_FORMATS and path.is_file()]
        wanted = written.split("://")[-1].replace("\\", "/").lstrip("./")
        if not wanted or Path(wanted).suffix.lower() not in DATA_FORMATS:
            return None
        for path in self.data_files:
            if path.as_posix().endswith("/" + wanted) or path.as_posix() == wanted:
                return str(path)
        return None

    def data_given(self, site, depth: int = 0) -> str | None:
        """The data file a call of code not read is given a text naming, itself or through the calls it is given."""
        for key in site.args if depth < DEPTH else ():
            held = self.fl.node_of.get(key)
            if held is None:
                continue
            f, node = held
            written = unquoted(node)
            if written is not None:
                found = self.data_named(written)
                if found:
                    return found
            inner = self.site_at.get((f.index, node.id))
            if inner is not None and not inner.answered:
                found = self.data_given(inner, depth + 1)
                if found:
                    return found
        return None

    def descend(self, data: tuple, key, f, node):
        """An entry of data loaded from a file: a number, a truth, a text as such; more data as data; none: an
        input."""
        path, keys = data[1], data[2] + (key,)
        if path not in self.loaded:
            try:
                self.loaded[path] = DATA_FORMATS[Path(path).suffix.lower()](Path(path).read_text(encoding="utf-8"))
            except (OSError, ValueError):
                self.loaded[path] = None
        held = self.loaded[path]
        self.keys_read.add((path, keys))
        for k in keys:
            if isinstance(held, dict) and k in held:
                held = held[k]
            elif isinstance(held, list) and isinstance(k, int) and 0 <= k < len(held):
                held = held[k]
            else:                                # a key the data does not hold
                self.keys_missing.append((self.where(f, node), path, keys))
                return self.opaque(f, node)
        if isinstance(held, bool):
            return held
        if isinstance(held, (int, float)):
            return Fraction(str(held))
        if isinstance(held, str):
            return (TEXT, held)
        if held is None:
            return self.opaque(f, node)
        return (DATA, path, keys)

    # ---- where ---------------------------------------------------------------------------------------------------
    def where(self, f, node) -> str:
        return f"{f.display}:{node.start_point[0] + 1}"

    def head(self, f, node) -> str | None:
        written = text(node)
        spelled = f.lang.spelled_table
        return spelled.get(written, written if written in OPERATIONS else None)

    # ---- values --------------------------------------------------------------------------------------------------
    def opaque(self, f, node, key=None) -> tuple:
        """An input nothing more is known of: by what names it, else by where it is written."""
        shown = " ".join(text(node).split())
        return (SYMBOL, key if key is not None else f"{self.where(f, node)}:{shown}", shown, self.where(f, node))

    def of_definition(self, d: int, f, node, depth: int, seen: frozenset):
        """What a name read means: a parameter, the state as the last run left it (an input); a constant or a
        variable set once, its value; anything else, an input of its own."""
        it = self.p.defs[d]
        where = f"{self.p.files[it.file].display}:{it.line}"
        if it.kind == PARAM:
            return (SYMBOL, d, self.p.label(d), where)
        if it.kind in STORED and self.fl.is_state(d) and self.unit_now is not None:
            self.reads.add((self.unit_now, d))       # state read where the function is: noted, value taken or not
        if it.kind in STORED and self.fl.is_state(d):
            # Written nowhere, at the level of a file or a namespace: a constant. A class's field stays an input
            # (each object may be set apart, by code not read too).
            if d not in self.written and VALUE in it.parts and d not in seen and                     (FIXED in it.parts or not self.p.is_class(it.owner)):
                return self.value(self.p.files[it.file], it.parts[VALUE], depth, seen | {d})
            return (SYMBOL, d, self.p.label(d), where)
        if it.kind == VARIABLE and VALUE in it.parts and d not in self.set_again and d not in seen:
            held = self.value(self.p.files[it.file], it.parts[VALUE], depth, seen | {d})
            if it.owner >= 0 and self.p.defs[it.owner].kind == FUNCTION:     # read where it is set
                return (READ_AT, it.owner, it.file, it.node.start_byte, held)
            return held
        return (SYMBOL, d, self.p.label(d), where)

    def member(self, f, node, depth: int, seen: frozenset):
        """A member read: the one its object's class declares, else an input named by the class and the member."""
        role = f.role(node)
        if role is not None and OBJECT in role[1] and PROPERTY in role[1]:
            held = self.value(f, role[1][OBJECT], depth, seen)
            if isinstance(held, tuple) and held[0] == DATA:             # an entry of data loaded
                return self.descend(held, text(role[1][PROPERTY]), f, node)
        if role is not None and OBJECT in role[1] and (EXPR, f.index, role[1][OBJECT].id) not in self.fl.seen:
            self.fl.ev(f, role[1][OBJECT])               # a read in a test only: its object's values, now
            self.fl.drain()
        places = self.fl.st.place(f, node)
        if self.fl.queue:                            # what naming the place asked for, worked out now
            self.fl.drain()
        for place in places:
            if place[0] != PROP:
                continue
            classes = [v[1] for v in self.fl.s.of(place[1]) if v[0] in (INST, CLS)]
            for c in classes:
                for d in self.sc.members(c, place[2]):
                    if self.p.defs[d].kind in STORED:
                        return self.of_definition(d, f, node, depth, seen)
            if classes:
                return self.opaque(f, node, f"{self.p.label(classes[0])}.{place[2]}")
            return self.opaque(f, node, " ".join(text(node).split()))
        return self.opaque(f, node)

    def element(self, f, holder, index, depth: int = 0, seen: frozenset = frozenset()):
        """An element of a collection by a position: an input of its own, named by the collection and the position
        (the same position written the same way, `i + 1` and `1 + i`, is one element)."""
        whole = self.value(f, holder, depth, seen)
        at = self.value(f, index, depth, seen)
        owner = whole[1] if isinstance(whole, tuple) and whole[0] == SYMBOL else render_key(whole)
        shown = f"{whole[2] if isinstance(whole, tuple) and whole[0] == SYMBOL else text(holder)}[{show_index(at)}]"
        return (SYMBOL, (ELEM, owner, index_key(at)), shown, self.where(f, holder))

    def literal(self, f, node):
        written = text(node)
        truths = f.lang.truths_table
        if written in truths:
            return truths[written]
        found = number(written)
        if found is not None:
            return found
        inside = unquoted(node)
        return (TEXT, inside) if inside is not None else self.opaque(f, node)

    def called(self, f, node, depth: int, seen: frozenset):
        """A call: what the one function it runs gives back, its parameters given the arguments; else an input."""
        site = self.site_at.get((f.index, node.id))
        if site is None or depth >= DEPTH:
            return self.opaque(f, node)
        if not site.answered:
            loaded = self.data_given(site)
            if loaded:
                return (DATA, loaded, ())
            head = f.lang.math_table.get(" ".join(site.shown.split()))
            if head is not None:                 # a function of numbers the language defines: what it is
                given = [self.value(*self.fl.node_of[key], depth, seen) for key in site.args if key in self.fl.node_of]
                if len(given) == FUNCTIONS[head] or (head in (MIN, MAX) and len(given) >= 2):
                    return fold_function(head, given)
        bodies = {d for d, kind in site.targets or () if kind in (CALL, FIRES)
                  and self.p.defs[d].kind == FUNCTION and self.p.defs[d].body}
        if len(bodies) != 1 or next(iter(bodies)) in seen:
            return self.opaque(f, node)
        d = next(iter(bodies))
        given = self.arguments(site, d, depth + 1, seen | {d})
        return substitute(self.given_back(d, depth + 1, seen | {d}), given)

    def arguments(self, site, d: int, depth: int = 0, seen: frozenset = frozenset()) -> dict:
        out = {}
        for i, key in enumerate(site.args):
            held = self.fl.node_of.get(key)
            if held is None:
                continue
            for param in self.sc.params[d].get(i, ()):
                out[param] = self.value(held[0], held[1], depth, seen)
        return out

    def given_back(self, d: int, depth: int = 0, seen: frozenset = frozenset()):
        """What a function gives back, by its returns in order, each under its condition."""
        if (d, depth) in self.gives:
            return self.gives[(d, depth)]
        it = self.p.defs[d]
        f = self.p.files[it.file]
        rows = []
        for node, parts in f.facts.get(RETURN, ()):
            at = self.p.at.get((f.index, node.id))
            unit = at if at is not None and self.p.defs[at].kind == FUNCTION else self.sc.unit(f, node)
            if unit == d and VALUE in parts:
                rows.append((self.guard(f, node, d) if at is None else True, self.value(f, parts[VALUE], depth, seen)))
        out = (SYMBOL, f"{self.p.label(d)}()", f"{self.p.label(d)}()", f"{f.display}:{it.line}")
        for guard, value in reversed(rows):
            out = value if guard is True else (ITE, guard, value, out)
        self.gives[(d, depth)] = out
        return out

    def value(self, f, node, depth: int = 0, seen: frozenset = frozenset()):
        """What an expression is, in the inputs of the function it is written in."""
        if node.has_error:                       # the grammar did not read it whole: an input, said partial
            self.partial.append((self.where(f, node), "the grammar does not read this expression"))
            return self.opaque(f, node)
        key = (f.index, node.id)
        op = self.ops.get(key)
        if op is not None and OP in op:
            head = self.head(f, op[OP])
            if head is not None and LEFT in op:
                return fold(head, self.value(f, op[LEFT], depth, seen), self.value(f, op[RIGHT], depth, seen))
            if head in (NOT, SUB) and LEFT not in op:
                return fold(NOT if head == NOT else NEG, self.value(f, op[RIGHT], depth, seen))
            if head == ADD and LEFT not in op:
                return self.value(f, op[RIGHT], depth, seen)
            return self.opaque(f, node)
        branch = self.branches.get(key)
        if branch is not None and ELSE in branch:
            return (ITE, self.condition(f, branch[TEST], depth, seen), self.value(f, branch[THEN], depth, seen),
                    self.value(f, branch[ELSE], depth, seen))
        role = f.role(node)
        kind = role[0] if role else None
        if kind == LITERAL:
            return self.literal(f, node)
        if kind == IDENT:
            found = self.sc.lookup(f, node, text(node))[0]
            return self.of_definition(found[0], f, node, depth, seen) if found else \
                self.opaque(f, node, self.implicit(f, node))
        if kind == MEMBER:
            return self.member(f, node, depth, seen)
        if kind == CHAIN:                            # an object and steps: a member read, or what a call gives
            last = child_nodes(node)[-1] if child_nodes(node) else None
            if last is not None and (f.index, last.id) in self.site_at:
                return self.called(f, last, depth, seen)
            return self.member(f, node, depth, seen)
        if kind == INDEX and KEY in role[1]:
            held = self.value(f, role[1][OBJECT], depth, seen)
            if isinstance(held, tuple) and held[0] == DATA:             # an entry of data loaded, by its key
                key = unquoted(role[1][KEY])
                at = self.value(f, role[1][KEY], depth, seen) if key is None else None
                if key is None and isinstance(at, Fraction) and at.denominator == 1:
                    key = int(at)
                return self.descend(held, key, f, node) if key is not None else self.opaque(f, node)
            if unquoted(role[1][KEY]) is None:
                return self.element(f, role[1][OBJECT], role[1][KEY], depth, seen)
        if kind == CALL:
            return self.called(f, node, depth, seen)
        passed = f.passes.get(node.id)
        if passed and len(passed) == 1:
            return self.value(f, passed[0], depth, seen)
        kids = [k for k in child_nodes(node) if not k.is_extra]
        if kind is None and len(kids) == 1 and kids[0].start_byte == node.start_byte and \
                kids[0].end_byte == node.end_byte:
            return self.value(f, kids[0], depth, seen)
        return self.opaque(f, node)

    def implicit(self, f, node) -> str:
        """A name no scope gives: where the language reads a class's members by a plain name, the class's own (as the
        class and the name); else the name."""
        if f.lang.implicit_members:
            c = self.sc.class_around(f, node)
            if c is not None:
                return f"{self.p.label(c)}.{text(node)}"
        return text(node)

    def condition(self, f, node, depth: int = 0, seen: frozenset = frozenset()):
        return (AT, self.where(f, node), self.value(f, node, depth, seen))

    # ---- when ----------------------------------------------------------------------------------------------------
    def leaves(self, f, node) -> bool:
        """Whether code always leaves the function there: an exit; a branch whose every way leaves; a block whose
        last statement does (a function written inside is not left by its own exits)."""
        key = (f.index, node.id)
        if key in self.exits:
            return True
        at = self.p.at.get(key)
        if at is not None and self.p.defs[at].kind == FUNCTION:
            return False
        branch = self.branches.get(key)
        if branch is not None and LOOP in branch:        # a loop's body may not run: it does not always leave
            return False
        if branch is not None and ELSE not in branch:
            rest = self.rest(f, node, branch)
            if not rest or not self.leaves(f, branch[THEN]):
                return False
            if (f.index, rest[-1].id) in self.branches:            # the last way a branch of its own: none left
                return False
            return all(self.leaves(f, self.branches[(f.index, k.id)][THEN]) if (f.index, k.id) in self.branches
                       else self.leaves(f, k) for k in rest)
        kids = [k for k in child_nodes(node) if not k.is_extra]
        return bool(kids) and self.leaves(f, kids[-1])

    @staticmethod
    def rest(f, node, branch) -> list:
        """The ways of a branch other than its test and the code run when it holds."""
        def overlaps(k, part) -> bool:               # holds it, or is held in it
            return k.start_byte <= part.start_byte and part.end_byte <= k.end_byte or \
                part.start_byte <= k.start_byte and k.end_byte <= part.end_byte
        return [k for k in child_nodes(node) if not k.is_extra
                and not overlaps(k, branch[TEST]) and not overlaps(k, branch[THEN])]

    def guard(self, f, node, unit: int):
        """When code written at a node runs, once the function around it runs."""
        stop = self.p.defs[unit].node
        conds, child, up, case = [], node, node.parent, None
        while up is not None and not (up.start_byte == stop.start_byte and up.end_byte == stop.end_byte
                                      and up.type == stop.type):
            key = (f.index, up.id)
            branch = self.branches.get(key)
            if branch is not None and LOOP in branch:    # a loop: its body runs while its test holds
                if TEST in branch and child.id == branch[THEN].id:
                    conds.append(self.condition(f, branch[TEST]))
            elif branch is not None:
                test = branch[TEST]
                inside_test = child.start_byte <= test.start_byte and test.end_byte <= child.end_byte or \
                    test.start_byte <= child.start_byte and child.end_byte <= test.end_byte
                if not inside_test:
                    cond = self.condition(f, test)
                    conds.append(cond if child.id == branch[THEN].id else negate(cond))
                    for k in child_nodes(up):          # the ways before it, each a branch of its own: not taken
                        if k.id == child.id:
                            break
                        other = self.branches.get((f.index, k.id))
                        if other is not None and k.id != branch[THEN].id and k.id != test.id:
                            conds.append(negate(self.condition(f, other[TEST])))
            else:
                for k in child_nodes(up):              # the statements before it that may leave the function
                    if k.id == child.id:
                        break
                    other = self.branches.get((f.index, k.id))
                    if other is None or ELSE in other or LOOP in other:
                        continue
                    rest = self.rest(f, k, other)
                    if self.leaves(f, other[THEN]) and not rest:
                        conds.append(negate(self.condition(f, other[TEST])))
                    elif rest and not self.leaves(f, other[THEN]) and \
                            all(self.leaves(f, r) for r in rest if (f.index, r.id) not in self.branches) and \
                            not any((f.index, r.id) in self.branches for r in rest):
                        conds.append(self.condition(f, other[TEST]))
            op = self.ops.get(key)
            if op is not None and LEFT in op and child.id == op[RIGHT].id:
                head = self.head(f, op[OP])
                if head in (ALL, ANY):
                    cond = self.condition(f, op[LEFT])
                    conds.append(cond if head == ALL else negate(cond))
            if key in self.cases:
                case = up
            choose = self.chooses.get(key)
            if choose is not None and case is not None:
                conds.append(self.chosen(f, up, choose, case))
                case = None
            child, up = up, up.parent
        return conj(list(reversed(conds)))

    def chosen(self, f, node, choose, case):
        """The condition of a case of a choice: the value is the case's; with none, no other case's."""
        test = self.value(f, choose[TEST])
        where = self.where(f, case)
        parts = self.cases[(f.index, case.id)]
        if VALUE in parts:
            return (AT, where, (EQ, test, self.value(f, parts[VALUE])))
        others = []
        stack = list(child_nodes(node))
        while stack:
            k = stack.pop()
            got = self.cases.get((f.index, k.id))
            if got is not None and VALUE in got:
                others.append((NE, test, self.value(f, got[VALUE])))
            elif (f.index, k.id) not in self.chooses:
                stack.extend(child_nodes(k))
        return (AT, where, conj(others))

    # ---- effects -------------------------------------------------------------------------------------------------
    def stopped(self) -> bool:
        """Whether the ceiling is reached: what is left is not weighed (said)."""
        ceiling = self.fl.ceiling
        if ceiling is not None and ceiling.over():
            if not any(why == STOPPED for _, why in self.partial):
                self.partial.append(("", STOPPED))
            return True
        return False

    def build(self) -> Cases:
        effects, passages = [], {}
        p = self.p
        for f in p.files:
            if self.stopped():
                break
            for node, parts in f.facts.get(ASSIGN, ()):
                effects += self.writes(f, node, parts)
            for node, parts in f.facts.get(RETURN, ()):
                at = p.at.get((f.index, node.id))
                self.unit_now = at if at is not None and p.defs[at].kind == FUNCTION else self.sc.unit(f, node)
                unit = at if at is not None and p.defs[at].kind == FUNCTION else self.sc.unit(f, node)
                if VALUE in parts and p.defs[unit].kind == FUNCTION:
                    effects.append(Effect(GIVES, unit, unit, p.defs[unit].name, " ".join(text(node).split())[:80],
                                          f.index, node.start_point[0] + 1,
                                          True if at is not None else self.guard(f, node, unit),
                                          self.value(f, parts[VALUE]), (), node.start_byte))
        for k, site in enumerate(self.fl.calls.sites):
            if k % 256 == 0 and self.stopped():
                break
            f, node = site.f, site.node
            self.unit_now = site.unit
            guard = None
            if not site.answered and (site.platform or any(v[0] == OUT for v in self.fl.s.of(site.receiver))
                                   or not any(v[0] in (INST, CLS) for v in self.fl.s.of(site.receiver))
                                   if site.receiver is not None else True):     # on no object of a class read
                guard = self.guard(f, node, site.unit)
                args = tuple(self.value(*self.fl.node_of[key]) for key in site.args if key in self.fl.node_of)
                self.results[(self.opaque(f, node)[1], node.start_byte)] = site.shown
                if node.id in f.effects:         # a statement of its own: its value taken by nothing
                    self.statements.add((f.index, node.start_byte))
                effects.append(Effect(OUTCALL, site.unit, site.shown, site.name or site.shown,
                                      site.shown, f.index, node.start_point[0] + 1, guard, None, args,
                                      node.start_byte))
            for d, kind in dict.fromkeys(site.targets or ()):      # each target once
                if guard is None:
                    guard = self.guard(f, node, site.unit)
                given = self.arguments(site, d) if kind in (CALL, NEW) else {}
                passages.setdefault((site.unit, d), []).append(
                    Passage(site.unit, d, kind, f.index, node.start_point[0] + 1, guard, given, {}, node.start_byte))
        self.in_order(effects, [row for rows in passages.values() for row in rows])
        first = {d.id: settle(self.value(p.files[d.file], d.parts[VALUE])) for d in p.defs
                 if d.kind in STORED and VALUE in d.parts and self.fl.is_state(d.id)}
        effects += self.uniform_writes(effects)
        blocks = []
        for f in p.files:                        # the code each branch runs: when it is reached
            for node, parts in f.facts.get(BRANCH, ()):
                if THEN in parts and ELSE not in parts:
                    unit = self.sc.unit(f, parts[THEN])
                    blocks.append((unit, f.index, parts[THEN].start_point[0] + 1, self.guard(f, parts[THEN], unit)))
        uniforms = {}
        for f in p.files:
            for name in f.uniforms:
                uniforms.setdefault(name, []).append(f.display)
        loops = self.turnings()
        makes = []
        for f in p.files:                        # what is made: objects of classes, collections and objects in place
            for node, parts in f.facts.get(NEW, ()):
                makes.append((self.sc.unit(f, node), f.index, node.start_point[0] + 1, node.start_byte,
                              " ".join(text(node).split())[:60]))
        for d in p.defs:
            if d.kind == SHAPE and d.owner >= 0:
                f = p.files[d.file]
                makes.append((self.sc.unit(f, d.node), d.file, d.line, d.node.start_byte,
                              " ".join(text(d.node).split())[:60]))
        found = Cases(effects, passages, first, self.partial, uniforms, blocks, sorted(self.keys_read, key=repr),
                      self.keys_missing, sorted(self.loaded), loops, makes)
        found.reads = sorted(self.reads)
        return found

    def turnings(self) -> list:
        """Each loop: what bounds it (its test, or the collection it goes over) and whether that grows."""
        from .decide import symbols
        out = []
        for f, node, parts in self.loops:
            unit = self.sc.unit(f, node)
            if TEST in parts:
                bound = self.value(f, parts[TEST])
                over = " ".join(text(parts[TEST]).split())[:60]
            else:
                held, stack = [], [node] + [k for k in child_nodes(node) if k.end_byte <= parts[THEN].start_byte]
                while stack:                     # the variable the loop's head declares, and what it goes over
                    k = stack.pop()
                    d = self.p.at.get((f.index, k.id))
                    if d is not None and self.p.defs[d].kind == VARIABLE and VALUE in self.p.defs[d].parts:
                        held.append(self.p.defs[d])
                    if k.id != node.id:
                        stack.extend(child_nodes(k))
                if held:
                    bound = self.value(f, held[0].parts[VALUE])
                    over = " ".join(text(held[0].parts[VALUE]).split())[:60]
                else:
                    bound, over = True, " ".join(text(node).split())[:40]
            own = set()                          # what the loop itself writes (its counter): no size
            for n, got in f.facts.get(ASSIGN, ()):
                if LEFT in got and node.start_byte <= n.start_byte < node.end_byte:
                    own |= {place[1] for place in self.fl.st.place(f, got[LEFT]) if place[0] == VAR}
            stack = [k for k in child_nodes(node) if k.end_byte <= parts[THEN].start_byte]
            while stack:                         # and the variables its head declares
                k = stack.pop()
                d = self.p.at.get((f.index, k.id))
                if d is not None:
                    own.add(d)
                stack.extend(child_nodes(k))
            grows = bool(set(symbols(bound)) - own) or (isinstance(bound, tuple) and bound[0] == DATA)
            out.append(Turning(unit, f.index, node.start_point[0] + 1, node.start_byte, node.end_byte,
                               (parts[THEN].start_byte, parts[THEN].end_byte), over, bound, grows, None,
                               (parts[THEN].start_point[0] + 1, parts[THEN].end_point[0] + 1)))
        return out

    def uniform_writes(self, effects: list) -> list:
        """What the code outside writes in a shader's uniforms: a call of code not read given a uniform's name as a
        text writes the value it is given with it; what that call gives back stands for the uniform, and a call given
        that and a value writes it too."""
        names = {name for f in self.p.files for name in f.uniforms}
        if not names:
            return []
        out, standing = [], {}
        for e in effects:
            if e.kind != OUTCALL:
                continue
            for k, arg in enumerate(e.args):
                if isinstance(arg, tuple) and arg[0] == TEXT and arg[1] in names:
                    standing[e.start] = arg[1]
                    rest = [a for j, a in enumerate(e.args) if j != k and not (isinstance(a, tuple) and a[0] == TEXT)]
                    if rest and (e.file, e.start) in self.statements:   # a statement given the name and a value
                        out.append(Effect(WRITE, e.unit, (UNIFORM, arg[1]), arg[1], e.shown, e.file, e.line,
                                          e.guard, rest[-1], (), e.start))
        held = {key[0]: standing[key[1]] for key in self.results if key[1] in standing}
        for e in effects:
            if e.kind != OUTCALL:
                continue
            named = [held[a[1]] for a in e.args if isinstance(a, tuple) and a[0] == SYMBOL and a[1] in held]
            rest = [a for a in e.args if not (isinstance(a, tuple) and a[0] == SYMBOL and a[1] in held)]
            if named and rest:
                out.append(Effect(WRITE, e.unit, (UNIFORM, named[0]), named[0], e.shown, e.file, e.line, e.guard,
                                  rest[-1], (), e.start))
        return out

    # ---- loops ---------------------------------------------------------------------------------------------------
    def step_of(self, value, d) -> int:
        """How a write moves a variable: +1 (only up: the old value and a number not below zero), -1 (only down),
        0 (anything else)."""
        form = linear((SUB, value, (SYMBOL, d, "", "")), {})
        if form is None or form[0]:
            return 0
        return 1 if form[1] >= 0 else -1

    def summaries(self, effects: list) -> list:
        """Each loop, summed up by intervals: the state and the variables of its function it writes, each moving only
        up or only down (or either way), and a variable's first value."""
        out = []
        for f, node, parts in self.loops:
            unit = self.sc.unit(f, node)
            state, local, steps, writes = {}, {}, {}, {}
            test = strip_at(self.condition(f, parts[TEST])) if TEST in parts else None
            for e in effects:
                d = state_of(e.target) if e.kind == WRITE else None
                if d is not None and e.unit == unit and e.file == f.index and \
                        node.start_byte <= e.start < node.end_byte:
                    way = self.step_of(strip_at(e.value), d)
                    state[d] = way if state.get(d, way) == way else 0
                    writes[d] = writes.get(d, 0) + 1
                    plain = [strip_at(c) for c in parts_of(e.guard)] == ([test] if test is not None else [])
                    step = self.delta(strip_at(e.value), d)
                    steps[d] = step if plain and step is not None and writes[d] == 1 else None
            for n, got in f.facts.get(ASSIGN, ()):
                if LEFT not in got or not (node.start_byte <= n.start_byte < node.end_byte):
                    continue
                for place in self.fl.st.place(f, got[LEFT]):
                    if place[0] != VAR or self.fl.is_state(place[1]):
                        continue
                    d = place[1]
                    way = self.step_of(strip_at(self.written_value(f, n, got)), d)
                    first = self.p.defs[d]
                    init = self.value(self.p.files[first.file], first.parts[VALUE]) \
                        if VALUE in first.parts and first.node.start_byte < parts[THEN].start_byte else None
                    old = local.get(d, (way, init))
                    local[d] = (way if old[0] == way else 0, init)
            loop = Loop(unit, f.index, node.start_byte, node.end_byte,
                        (parts[THEN].start_byte, parts[THEN].end_byte), self.where(f, node), state, local)
            loop.turns = self.turns(test, local, f, node)
            loop.steps = {d: c for d, c in steps.items() if c is not None}
            out.append(loop)
        return out

    def delta(self, value, d):
        """What a write adds to a variable each time (`d = d + c`: c), or None where it is not a number added."""
        form = linear((SUB, value, (SYMBOL, d, "", "")), {})
        return form[1] if form is not None and not form[0] else None

    def turns(self, test, local: dict, f, node):
        """How many turns a loop makes, where that is known: a counter starting at a number, going up one each turn,
        tested against a number (below it, or not above it)."""
        if not (isinstance(test, tuple) and test[0] in (LT, LE) and isinstance(test[2], Fraction)
                and isinstance(test[1], tuple) and test[1][0] == SYMBOL):
            return None
        d = test[1][1]
        if d not in local:
            return None
        way, init = local[d]
        steps = [self.delta(strip_at(self.written_value(f, n, got)), d) for n, got in f.facts.get(ASSIGN, ())
                 if LEFT in got and node.start_byte <= n.start_byte < node.end_byte
                 and any(p[0] == VAR and p[1] == d for p in self.fl.st.place(f, got[LEFT]))]
        if way <= 0 or not isinstance(init, Fraction) or steps != [Fraction(1)]:
            return None
        end = test[2] + (1 if test[0] == LE else 0)
        return max(Fraction(0), math.ceil(end - init))

    @staticmethod
    def moved(way: int, now, before):
        """What a variable moving that way keeps against what it was (nothing, moving either way)."""
        return (GE, now, before) if way > 0 else (LE, now, before) if way < 0 else True

    def in_order(self, effects: list, passages: list) -> None:
        """A function's code in the order it is written: what it reads of state after writing it is what it wrote
        (each write under its condition, else what the state was); a call it makes finds the state so. In a loop, a
        state the loop writes is its value at that turn, and past the loop its value when the loop ended: inputs of
        their own, each kept against what it was before the loop by the way the loop moves it (intervals); a variable
        a loop moves one way keeps to its side of its first value."""
        loops = {}
        for L in self.summaries(effects):
            loops.setdefault((L.unit, L.file), []).append(L)
        items = sorted([(e.unit, e.file, e.start, 0, e) for e in effects] +
                       [(row.src, row.file, row.start, 1, row) for row in passages], key=lambda t: t[:4])
        state, at, kept, entered, history, starts = {}, None, [], set(), [], []
        shot = [None]                            # the state as it is now, one copy shared until it changes (memory)
        ordered = [True]                         # the history's places in order (then found by halving)

        def snapshot() -> dict:
            if shot[0] is None:
                shot[0] = dict(state)
            return shot[0]

        def remember(byte: int) -> None:
            if starts and byte < starts[-1]:
                ordered[0] = False
            history.append((byte, snapshot()))
            starts.append(byte)

        def then(byte: int) -> dict:             # the state as the function had it before that byte
            if ordered[0]:
                i = bisect.bisect_left(starts, byte)
                return history[i - 1][1] if i > 0 else {}
            found = {}
            for start_at, held in history:
                if start_at >= byte:
                    break
                found = held
            return found
        for unit, file, start, which, item in items:
            if (unit, file) != at:
                state, at, kept, entered, history, starts = {}, (unit, file), [], set(), [], []
                shot[0], ordered[0] = None, True
            inside = []
            for L in loops.get((unit, file), ()):
                if L.start >= start:
                    continue
                if id(L) not in entered:             # the loop begins: its state is the value at a turn
                    entered.add(id(L))
                    L.entry = {d: state.get(d, self.pre(d)) for d in L.state}
                    for d, way in L.state.items():
                        turn = (SYMBOL, (LOOP, d, L.start), f"{self.p.label(d)} (in the loop)", L.where)
                        L.turn[d] = turn
                        state[d] = turn
                    shot[0] = None
                    remember(L.start)
                if L.body[0] <= start < L.body[1]:
                    inside.append(L)
                elif L.end <= start and not L.done:   # the loop ended: its state is the value it left
                    L.done = True
                    for d, way in L.state.items():
                        if L.turns is not None and d in L.steps:   # turns and step known: the value, exactly
                            state[d] = fold(ADD, L.entry[d], L.steps[d] * L.turns)
                            continue
                        after = (SYMBOL, (LOOP, d, L.end), f"{self.p.label(d)} (after the loop)", L.where)
                        state[d] = after
                        kept.append(self.moved(way, after, L.entry[d]))
                    shot[0] = None
                    remember(L.end)
            bounds = list(kept)
            for L in inside:
                bounds += [self.moved(way, L.turn[d], L.entry[d]) for d, way in L.state.items()]
            for L in loops.get((unit, file), ()):
                if L.start < start:
                    bounds += [self.moved(way, (SYMBOL, d, self.p.label(d), L.where), init)
                               for d, (way, init) in L.local.items() if init is not None]
            def now(x):                          # in the state here; what was read where set, as it was there
                return settle(substitute(x, state), unit, file, then)
            if which == 1:
                item.guard = now(item.guard)
                item.args = {k: now(v) for k, v in item.args.items()}
                item.guard = conj(self.relevant(bounds, [item.guard] + list(item.args.values())) + [item.guard])
                item.state = snapshot()          # read only, shared by the passages until the state changes
                continue
            item.guard, item.value = now(item.guard), now(item.value)
            item.args = tuple(now(a) for a in item.args)
            item.guard = conj(self.relevant(bounds, [item.guard, item.value] + list(item.args)) + [item.guard])
            d = state_of(item.target) if item.kind == WRITE else None
            if d is not None:
                old = state.get(d, self.pre(d))
                state[d] = item.value if item.guard is True else (ITE, item.guard, item.value, old)
                if larger(state[d], LARGEST):    # too long to follow on: an input of its own, said
                    where = f"{self.p.files[file].display}:{item.line}"
                    state[d] = (SYMBOL, (READ_AT, d, start), f"{self.p.label(d)} (as left at {where})", where)
                    self.partial.append((where, f"{self.p.label(d)}: too long to follow past here"))
                shot[0] = None
                remember(start)

    def pre(self, d: int) -> tuple:
        """A state as the last run left it."""
        it = self.p.defs[d]
        return (SYMBOL, d, self.p.label(d), f"{self.p.files[it.file].display}:{it.line}")

    @staticmethod
    def relevant(bounds: list, formulas: list) -> list:
        """The bounds about the inputs the formulas read."""
        from .decide import symbols
        read = set()
        for x in formulas:
            read |= set(symbols(x))
        return [b for b in bounds if b is not True and set(symbols(b)) & read]

    def writes(self, f, node, parts) -> list:
        """What an assignment writes: the state it names (its object's class declares it), else the member of an
        object of a type not defined here; what it writes in the inputs of the function around it."""
        if LEFT not in parts:
            return []
        p, left = self.p, parts[LEFT]
        unit = self.sc.unit(f, node)
        self.unit_now = unit
        targets = []
        for place in self.fl.st.place(f, left):
            if place[0] == VAR and self.fl.is_state(place[1]):
                targets.append((place[1], p.defs[place[1]].name))
            elif place[0] == PROP:
                classes = [v[1] for v in self.fl.s.of(place[1]) if v[0] in (INST, CLS)]
                state = [d for c in classes for d in self.sc.members(c, place[2])
                         if p.defs[d].kind in STORED and self.fl.is_state(d)]
                if state:                        # the field, of the object written through (siblings apart)
                    held = self.fl.node_of.get(place[1])
                    through = " ".join(text(held[1]).split()) if held else ""
                    targets += [((d, through), place[2]) for d in dict.fromkeys(state)]
                elif classes:
                    targets.append((f"{p.label(classes[0])}.{place[2]}", place[2]))
                else:
                    targets.append((" ".join(text(left).split()), place[2]))
        role = f.role(left)
        if not targets and role is not None and role[0] == IDENT and not self.sc.lookup(f, left, text(left))[0]:
            targets.append((self.implicit(f, left), text(left)))
        index = ()
        if not targets and role is not None and role[0] == INDEX and KEY in role[1] and \
                unquoted(role[1][KEY]) is None:      # an element of a collection, by a position
            cell = self.element(f, role[1][OBJECT], role[1][KEY])
            targets.append(((ELEM, cell[1][1]), cell[2].split("[")[0] + "[]"))
            index = (self.value(f, role[1][KEY]),)
        if not targets:
            return []
        value = self.written_value(f, node, parts)
        if RIGHT in parts:                       # what is written is a condition too, written there
            value = (AT, self.where(f, parts[RIGHT]), value)
        guard = self.guard(f, node, unit)
        return [Effect(WRITE, unit, target, name, " ".join(text(node).split())[:80], f.index,
                       node.start_point[0] + 1, guard, value, index, node.start_byte) for target, name in targets]

    def written_value(self, f, node, parts):
        """What an assignment writes: the right side; written with an operator before `=`, the old value and the right
        side by that operator; a step up or down, the old value and one."""
        left = parts[LEFT]
        if RIGHT not in parts:
            spelled = text(node)
            for mark, head in f.lang.steps:
                if spelled.startswith(mark) or spelled.endswith(mark):
                    return fold(head, self.value(f, left), Fraction(1))
            return self.opaque(f, node)
        right = parts[RIGHT]
        for k in child_nodes(node, False):
            if not k.is_named and k.start_byte >= left.end_byte and k.end_byte <= right.start_byte:
                mark = k.type
                if len(mark) > 1 and mark.endswith("="):
                    head = f.lang.spelled_table.get(mark[:-1], mark[:-1])
                    if head in OPERATIONS:
                        return fold(head, self.value(f, left), self.value(f, right))
                break
        return self.value(f, right)


def weigh(flow) -> Cases:
    return Weigh(flow).build()
