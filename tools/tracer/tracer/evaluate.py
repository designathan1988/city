"""What a program produces in a situation, and why, worked out from its code alone (the program is never run): its
functions followed statement by statement by the rules of their language, every value carrying where it came from
(the write, the constant, the key of data, the input), every call of code not read an output (with its arguments and
the conditions it came through) whose result is an input the situation may give. What cannot be worked out is an
input of its own, and every result that depends on one is said partial, with where and why: nothing is left out
silently.

    run = evaluate(result, starts, situation)          # one start, or a sequence of them, the state passed on
    run.outputs, run.state, run.partial, why(value), why_not(run, where)

`result` comes from `analyse(..., keep=True)` (the files' facts kept).
"""
from __future__ import annotations

import math
import struct
from dataclasses import dataclass, field
from fractions import Fraction

from .conditions import number, number_function
from .vocab import ABS, CLAMP, CROSS, DISTANCE, DOT, LENGTH, MAX, MIN, MIX, NORMALIZE, POW, SIGN, SMOOTH, SQRT
from .vocab import VECTOR_FUNCTIONS
from .model import text, unquoted, child_nodes
from .vocab import (AFTER, AGAIN, ALL, ANY, ARGS, ASSIGN, BODY, BRANCH, CALL, CASE, CHAIN, CHOOSE, CLASS, CONSTRUCT,
                    DIV, DOUBLES, DYNAMIC, ELSE, EQ, EXIT, FIRES, FUNCTION, FUNCTIONS, GE, GT, IDENT, INDEX, KEY, LE,
                    LEAVE, LEFT, LITERAL, LOOP, LT, MEMBER, MOD, MODULE, MUL, NE, NEW, NOT, OBJECT, OP, OPERATE,
                    OPERATION, PAIR, PARAM, PROPERTY, RETURN, RIGHT, SELF, SHAPE, STEP, STORED, SUB, SUPER, TEST, THEN,
                    TYPE, TYPED, VALUE, VARIABLE, ADD, CALLEE)

TURNS = 100000                   # the most turns one loop is followed for (past it: partial)
CALLS = 2000                     # the deepest calls are followed (past it: partial)
STEPS = 2000000                  # the most statements one evaluation follows (past it: partial)


# ---- values -------------------------------------------------------------------------------------------------------
@dataclass(frozen=True)
class Why:
    """Where a value came from: what it is, where it is written, and the values it was made of."""
    say: str
    where: str = ""
    parts: tuple = ()


@dataclass
class Unknown:
    """A value the code read does not decide: an input (named), whose value the situation may give."""
    name: str


class Vec(tuple):
    """A vector of a shading language: its components, reals."""


@dataclass
class Obj:
    cls: int | None              # its class (None: an object written in place)
    fields: dict = field(default_factory=dict)


@dataclass
class Fn:
    d: int                       # the function
    this: object = None          # the object it runs on
    env: object = None           # the frames it was written in (a function written inside another)


@dataclass
class ClassRef:
    d: int


@dataclass(slots=True)
class V:
    """A value and why it is it; a real kept in 32 bits says so."""
    value: object
    why: Why
    bits: int = 64


@dataclass
class Output:
    """A call of code not read: what is called, where, with which values, under which conditions taken."""
    where: str
    callee: str
    args: list
    path: list                   # [(where, the test's value, taken)] from the start to it
    partial: bool = False


@dataclass
class Run:
    outputs: list = field(default_factory=list)
    partial: list = field(default_factory=list)      # (where, why) what could not be worked out
    skipped: list = field(default_factory=list)      # (file, start line, end line, where the test is, its value)
    steps: list = field(default_factory=list)        # for a sequence: (the start, its outputs, the state after)
    returned: object = None
    fixpoint: int | None = None                      # the step after which nothing changes (a sequence)
    cycle: tuple | None = None                       # (first step, step it comes back at)
    reads_before_writes: list = field(default_factory=list)   # (where, the state read before anything wrote it)
    state: dict = field(default_factory=dict)        # the state at the end, by label


class Return(Exception):
    def __init__(self, value):
        self.value = value


class Leave(Exception):
    pass


class Again(Exception):
    pass


class Stop(Exception):
    """A limit reached: the evaluation stops there (said partial)."""


# ---- the machine --------------------------------------------------------------------------------------------------
class Machine:
    def __init__(self, result, situation: dict | None = None):
        self.r, self.p, self.sc = result, result.program, result.scopes
        self.situation = dict(situation or {})
        self.run = Run()
        self.globals = {}                            # a state definition (of a file, a namespace, a class) -> V
        self.instances = {}                          # a class run as a file (a script): its one object
        self.path = []                               # the conditions taken on the way to here
        self.depth = 0
        self.steps = 0
        self.facts = {}                              # (file, node id) -> {anchor: parts}
        for f in self.p.files:
            for anchor, rows in f.facts.items():
                for node, parts in rows:
                    self.facts.setdefault((f.index, node.id), {})[anchor] = parts
        self.sites = result.sites or {}

    # ---- helpers ------------------------------------------------------------------------------------------------
    def where(self, f, node) -> str:
        return f"{f.display}:{node.start_point[0] + 1}"

    def fact(self, f, node, anchor):
        return self.facts.get((f.index, node.id), {}).get(anchor)

    def partial(self, f, node, why: str) -> None:
        self.run.partial.append((self.where(f, node) if node is not None else "", why))

    def unknown(self, name: str, f=None, node=None, why: str = "") -> V:
        """An input: given by the situation, else unknown (and said)."""
        for key in (name, name.split(".")[-1]):
            if key in self.situation:
                return self.given(self.situation[key], Why(f"given: {name}", self.where(f, node) if node else ""))
        if f is not None:
            self.partial(f, node, why or f"{name}: not given, not decided by the code read")
        return V(Unknown(name), Why(f"input {name} (not given)", self.where(f, node) if node else ""))

    @staticmethod
    def given(value, why: Why) -> V:
        if isinstance(value, list):
            return V([Machine.given(v, why) for v in value], why)
        if isinstance(value, dict):
            return V(Obj(None, {k: Machine.given(v, why) for k, v in value.items()}), why)
        return V(value, why)

    def tick(self, f, node) -> None:
        self.steps += 1
        if self.steps > STEPS:
            self.partial(f, node, f"stopped after {STEPS} statements")
            raise Stop()

    # ---- numbers by the language ----------------------------------------------------------------------------------
    def number(self, f, node) -> V:
        written = text(node).replace("_", "")
        lang = f.lang
        value = number(written)
        if value is None:
            return self.unknown(written, f, node, f"the number {written} is not read")
        why = Why(f"constant {written}", self.where(f, node))
        if lang.numbers == DOUBLES:
            return V(float(value), why)
        hexadecimal = written[:2].lower() == "0x"
        if not hexadecimal and any(c in written for c in ".eE"):
            bits = 32 if lang.real32_types and (written[-1:] in lang.real32_marks or self.all_reals32(lang)) else 64
            return V(self.real(float(value), bits), why, bits)
        return V(int(value), why)

    @staticmethod
    def all_reals32(lang) -> bool:
        """Whether every real the language writes is 32 bits (a shader's: no double written)."""
        return bool(lang.entries)

    @staticmethod
    def real(value: float, bits: int) -> float:
        return struct.unpack("<f", struct.pack("<f", value))[0] if bits == 32 else value

    def kept_as(self, f, d: int | None, v: V) -> V:
        """A value kept in a variable of a declared type: made a whole number or a real of that size (TYPED)."""
        if d is None or f.lang.numbers == DOUBLES or isinstance(v.value, (Unknown, bool)) or \
                not isinstance(v.value, (int, float)):
            return v
        parts = self.p.defs[d].parts
        declared = text(parts[TYPE]).split()[-1] if TYPE in parts else ""
        if declared in f.lang.whole_types and isinstance(v.value, float):
            return V(int(math.trunc(v.value)) if math.isfinite(v.value) else v.value, v.why)
        if declared in f.lang.real32_types:
            return V(self.real(float(v.value), 32), v.why, 32)
        if isinstance(v.value, int) and (declared in f.lang.real64_types):
            return V(float(v.value), v.why, 64)
        return v

    def arith(self, f, node, head: str, a: V, b: V) -> V:
        why = Why(head, self.where(f, node), (a.why, b.why))
        x, y = a.value, b.value
        if isinstance(x, Unknown) or isinstance(y, Unknown):
            return V(Unknown(f"({unknown_name(x)} {head} {unknown_name(y)})"), why)
        if head == ADD and (isinstance(x, str) or isinstance(y, str)):
            return V(self.shown(f, x) + self.shown(f, y), why)
        if isinstance(x, Vec) or isinstance(y, Vec):
            if head in (EQ, NE):
                same = x == y
                return V(same if head == EQ else not same, why)
            if head in (ADD, SUB, MUL, DIV) and all(isinstance(z, (Vec, int, float)) for z in (x, y)):
                return self.each(f, node, [a, b], lambda parts: self.arith(f, node, head, parts[0], parts[1]))
            self.partial(f, node, f"{head} of vectors")
            return V(Unknown(f"{head}"), why)
        if isinstance(x, bool):
            x = int(x)
        if isinstance(y, bool):
            y = int(y)
        if not isinstance(x, (int, float)) or not isinstance(y, (int, float)):
            if head in (EQ, NE):
                same = x is y or x == y
                return V(same if head == EQ else not same, why)
            self.partial(f, node, f"{head} of values that are not numbers")
            return V(Unknown(f"{head}"), why)
        lang = f.lang
        bits = 32 if (a.bits == 32 or b.bits == 32) and not (isinstance(x, float) and a.bits == 64 and
                                                               not isinstance(y, int)) else 64
        if lang.numbers == DOUBLES:
            x, y = float(x), float(y)
        whole = isinstance(x, int) and isinstance(y, int)
        try:
            if head == ADD:
                out = x + y
            elif head == SUB:
                out = x - y
            elif head == MUL:
                out = x * y
            elif head == DIV:
                if whole and lang.divides_whole:
                    out = int(x / y) if abs(x) < 2 ** 52 and abs(y) < 2 ** 52 else -(-x // y) if (x < 0) != (y < 0) \
                        else x // y
                else:
                    out = x / y
            elif head == MOD:
                out = int(math.fmod(x, y)) if whole else math.fmod(x, y)
            elif head in (LT, LE, GT, GE, EQ, NE):
                out = {LT: x < y, LE: x <= y, GT: x > y, GE: x >= y, EQ: x == y, NE: x != y}[head]
                return V(out, why)
            else:
                return V(Unknown(head), why)
        except ZeroDivisionError:
            if not whole and lang.numbers != DYNAMIC:
                out = math.copysign(math.inf, x) * math.copysign(1.0, y) if x != 0 else math.nan
            else:
                self.partial(f, node, "a whole number divided by zero")
                return V(Unknown("divided by zero"), why)
        if isinstance(out, float) and bits == 32:
            out = self.real(out, 32)
        return V(out, why, bits if isinstance(out, float) else 64)

    @staticmethod
    def shown(f, x) -> str:
        """A value made a text, as the language writes it."""
        if isinstance(x, bool):
            return "true" if x else "false"
        if isinstance(x, float) and f.lang.numbers == DOUBLES and x.is_integer() and abs(x) < 1e21:
            return str(int(x))
        return str(x)

    @staticmethod
    def truth(f, v: V):
        x = v.value
        if isinstance(x, Unknown):
            return None
        if isinstance(x, (list, dict)):
            return f.lang.numbers != DYNAMIC or bool(x)
        if isinstance(x, Obj):
            return True if x.cls is not None or f.lang.numbers != DYNAMIC else bool(x.fields)
        if isinstance(x, float) and math.isnan(x):
            return False
        return bool(x)

    # ---- names --------------------------------------------------------------------------------------------------
    def frame_of(self, env, d: int):
        while env is not None:
            frame, outer = env
            if d in frame:
                return frame
            env = outer
        return None

    def read(self, f, node, env, this) -> V:
        name = text(node)
        found = self.sc.lookup(f, node, name)[0]
        if not found:
            if this is not None and isinstance(this.value, Obj) and name in this.value.fields:
                return this.value.fields[name]
            if name in self.situation:
                return self.unknown(name)
            return V(Unknown(name), Why(f"{name}: of code not read", self.where(f, node)))
        d = found[0]
        it = self.p.defs[d]
        if it.kind == FUNCTION:
            return V(Fn(d, this if it.owner >= 0 and self.p.is_class(it.owner) else None, env),
                     Why(f"function {self.p.label(d)}", self.where(f, node)))
        if it.kind == CLASS or self.p.is_class(d):
            return V(ClassRef(d), Why(f"class {self.p.label(d)}", self.where(f, node)))
        frame = self.frame_of(env, d)
        if frame is not None:
            return frame[d]
        if it.kind in STORED and it.owner >= 0 and self.p.is_class(it.owner) and it.kind != PARAM:
            target = this.value if this is not None and isinstance(this.value, Obj) else None
            if target is None:
                target = self.script(it.owner).value
            if it.name in target.fields:
                return target.fields[it.name]
            return self.unknown(self.p.label(d), f, node, f"{self.p.label(d)} read before anything gave it a value")
        if it.kind in STORED:
            return self.global_value(d, f, node)
        return self.unknown(self.p.label(d), f, node)

    def global_value(self, d: int, f=None, node=None) -> V:
        """A state of a file or a namespace: given by the situation, else its first value (once)."""
        if d in self.globals:
            return self.globals[d]
        label = self.p.label(d)
        if label in self.situation or self.p.defs[d].name in self.situation:
            value = self.kept_as(self.p.files[self.p.defs[d].file], d, self.unknown(label))
        else:
            it = self.p.defs[d]
            if VALUE in it.parts:
                g = self.p.files[it.file]
                value = self.kept_as(g, d, self.ev(g, it.parts[VALUE], None, None))
            elif it.elsewhere:                   # declared here, defined by code not read: an input to give
                value = self.unknown(label, f, node, f"{label}: declared here, its value given by code not read")
            else:
                self.run.reads_before_writes.append((self.where(f, node) if f else "", label))
                value = self.default(self.p.files[it.file], d)
        self.globals[d] = value
        return value

    def default(self, f, d: int) -> V:
        """What a variable declared with no value holds: zero of its declared number type, else nothing."""
        parts = self.p.defs[d].parts
        declared = text(parts[TYPE]).split()[-1] if TYPE in parts else ""
        why = Why(f"declared with no value: {self.p.label(d)}", f"{f.display}:{self.p.defs[d].line}")
        size = f.lang.vectors_table.get(declared)
        if size:
            return V(Vec([0.0] * size), why, 32)
        if declared in f.lang.whole_types:
            return V(0, why)
        if declared in f.lang.real32_types:
            return V(0.0, why, 32)
        if declared in f.lang.real64_types:
            return V(0.0, why)
        cls = self.class_named(f, parts.get(TYPE))
        if cls is not None and f.lang.numbers == TYPED:     # an object held by value: made, as declared
            return self.make(f, None, cls, [], None)
        return V(None, why)

    def class_named(self, f, node):
        if node is None:
            return None
        named = text(node).split("<")[0].strip().rstrip("*&").strip()
        for c in self.sc.classes_named(named.split("::")[-1]):
            return c
        return None

    def script(self, c: int) -> V:
        """The one object of a class that is a file (a script) or that a start runs on: made once."""
        if c not in self.instances:
            self.instances[c] = self.make(self.p.files[self.p.defs[c].file], None, c, [], None, run=False)
        return self.instances[c]

    # ---- objects ------------------------------------------------------------------------------------------------
    def make(self, f, node, c: int, args: list, env, run: bool = True) -> V:
        """An object of a class: its fields given their first values (bases first), then what makes it runs."""
        obj = Obj(c)
        v = V(obj, Why(f"a new {self.p.label(c)}", self.where(f, node) if node is not None else ""))
        for k in reversed(self.ancestry(c)):
            for held in self.p.names.get(k, {}).values():
                for d in held:
                    it = self.p.defs[d]
                    if it.kind in STORED and it.kind != PARAM and it.owner == k:
                        g = self.p.files[it.file]
                        obj.fields[it.name] = self.kept_as(g, d, self.ev(g, it.parts[VALUE], None, v)) \
                            if VALUE in it.parts else self.default(g, d)
        makers = [m for m in self.sc.makers(c) if self.p.defs[m].kind == FUNCTION and self.p.defs[m].body]
        if makers and run:
            self.call(f, node, makers[0], args, v, None)
        elif args and f is not None and f.lang.aggregate:                  # values in order, as the fields
            for name, a in zip(list(obj.fields), args):
                obj.fields[name] = a
        return v

    def ancestry(self, c: int) -> list:
        out, todo = [], [c]
        while todo:
            k = todo.pop(0)
            if k in out:
                continue
            out.append(k)
            todo += list(self.sc.bases(k))
        return out

    def method(self, obj: Obj, name: str):
        """The function a call on an object runs: its class's, by the class it is (as it runs)."""
        if obj.cls is None:
            return None
        for d in self.sc.members(obj.cls, name):
            if self.p.defs[d].kind == FUNCTION:
                bodies = [b for b in self.p.bodies([d]) if self.p.defs[b].body]
                return bodies[0] if bodies else d
        return None

    # ---- calls --------------------------------------------------------------------------------------------------
    def call(self, f, node, d: int, args: list, this, env) -> V:
        """A function of the code read run with these values."""
        it = self.p.defs[d]
        g = self.p.files[it.file]
        self.depth += 1
        if self.depth > CALLS:
            self.depth -= 1
            self.partial(f, node, f"calls deeper than {CALLS}")
            raise Stop()
        frame = {}
        for i, params in sorted(self.sc.params[d].items()):
            for p in params:
                if i < len(args):
                    frame[p] = self.kept_as(g, p, args[i])
                elif VALUE in self.p.defs[p].parts:
                    frame[p] = self.ev(g, self.p.defs[p].parts[VALUE], (frame, env), this)
                else:
                    frame[p] = V(None, Why("not given", f"{g.display}:{self.p.defs[p].line}"))
        scope = (frame, env)
        try:
            body = it.parts.get(BODY)
            if body is None:
                return V(None, Why(f"{self.p.label(d)} has no body read"))
            if self.is_expression(g, body):
                return self.ev(g, body, scope, this)
            self.exec(g, body, scope, this, d)
            return V(None, Why(f"{self.p.label(d)} gives nothing back", self.where(g, body)))
        except Return as back:
            return back.value
        finally:
            self.depth -= 1

    def is_expression(self, f, node) -> bool:
        return f.role(node) is not None or node.id in f.passes or self.fact(f, node, OPERATION) is not None

    def output(self, f, node, callee: str, args: list) -> V:
        """A call of code not read: an output; what it gives back is an input."""
        partial = any(isinstance(a.value, Unknown) for a in args) or any(t is None for _, t, _ in self.path)
        self.run.outputs.append(Output(self.where(f, node), callee, args, list(self.path), partial))
        key = self.where(f, node)
        if key in self.situation:
            return self.given(self.situation[key], Why(f"given: what {callee} gives back", key))
        if callee in self.situation:
            return self.given(self.situation[callee], Why(f"given: what {callee} gives back", key))
        return V(Unknown(f"{callee}() at {key}"), Why(f"what {callee} gives back (code not read)", key))

    def invoke(self, f, node, parts, env, this) -> V:
        callee = parts.get(CALLEE)
        args = [self.ev(f, a, env, this) for a in (child_nodes(parts[ARGS]) if ARGS in parts else [])]
        targets = self.sites.get((f.index, node.id), [])
        fired = [d for d, kind in targets if kind == FIRES]
        shown = " ".join(text(callee).split()) if callee is not None else ""
        size = f.lang.vectors_table.get(shown)
        if size and not self.sc.lookup(f, callee, shown)[0]:
            return self.vector(f, node, size, args)
        known = all(isinstance(a.value, (int, float, Vec)) and not isinstance(a.value, bool) for a in args)
        vhead = f.lang.vector_math_table.get(shown)
        if vhead is not None and known and len(args) == VECTOR_FUNCTIONS[vhead] and not self.sc.lookup(f, callee,
                                                                                                        shown)[0]:
            return self.vector_function(f, node, vhead, args)
        head = f.lang.math_table.get(shown)
        if head is not None and known and any(isinstance(a.value, Vec) for a in args):
            return self.each(f, node, args, lambda parts: self.function(f, node, head, parts))
        if head is not None and all(isinstance(a.value, (int, float)) and not isinstance(a.value, bool)
                                    for a in args):
            return self.function(f, node, head, args)
        role = f.role(callee) if callee is not None else None
        target = None
        receiver = None
        if role is not None and role[0] == MEMBER:
            receiver = self.ev(f, role[1][OBJECT], env, this)
            name = text(role[1][PROPERTY])
            if isinstance(receiver.value, Obj):
                held = receiver.value.fields.get(name)
                if held is not None and isinstance(held.value, Fn):
                    target = held
                else:
                    m = self.method(receiver.value, name)
                    if m is not None:
                        target = V(Fn(m, receiver), receiver.why)
            elif isinstance(receiver.value, ClassRef):
                for m in self.sc.members(receiver.value.d, name):
                    if self.p.defs[m].kind == FUNCTION:
                        target = V(Fn(m), receiver.why)
                        break
            elif isinstance(receiver.value, Fn) and name in f.lang.invokes_table:
                start = f.lang.invokes_table[name]
                return self.apply(f, node, receiver, args[start:] if start is not None else
                                  (args[0].value if args and isinstance(args[0].value, list) else []))
        elif callee is not None and role is not None and role[0] == IDENT and \
                not self.sc.lookup(f, callee, text(callee))[0] and not (
                this is not None and isinstance(this.value, Obj) and text(callee) in this.value.fields):
            target = None                    # a name no definition gives: code not read
        elif callee is not None:
            target = self.ev(f, callee, env, this)
        if target is not None and isinstance(target.value, Fn):
            bodies = [b for b in self.p.bodies([target.value.d]) if self.p.defs[b].body]
            if not bodies:                       # declared only: what it does is not read (an output)
                return self.output(f, node, shown, args)
            if bodies[0] != target.value.d:
                target = V(Fn(bodies[0], target.value.this, target.value.env), target.why)
            return self.apply(f, node, target, args)
        if target is not None and isinstance(target.value, ClassRef):
            return self.make(f, node, target.value.d, args, env)
        bodies = [d for d, kind in targets if kind == CALL and self.p.defs[d].kind == FUNCTION and self.p.defs[d].body]
        if len(bodies) == 1 and target is None:
            return self.call(f, node, bodies[0], args, receiver if receiver is not None else this, None)
        back = self.output(f, node, shown, args)
        for d in fired:                              # handlers the code not read runs here
            given = [a for a in args if not isinstance(a.value, str)]
            self.call(f, node, d, given, None, None)
        if receiver is not None and isinstance(receiver.value, (list, Obj)) and not fired:
            self.partial(f, node, f"{shown}: code not read called on a value of the program may change it")
        return back

    def apply(self, f, node, target: V, args: list) -> V:
        fn = target.value
        return self.call(f, node, fn.d, args, fn.this, fn.env)

    # ---- vectors (the shading languages) ----------------------------------------------------------------------
    def vector(self, f, node, size: int, args: list) -> V:
        """A vector made by its type's name: its components in order from the values given (a vector gives each of
        its own), one number given: every component it (GLSL 4.60 §5.4.2)."""
        why = Why(f"a vector of {size}", self.where(f, node), tuple(a.why for a in args))
        parts = []
        for a in args:
            if isinstance(a.value, Vec):
                parts += list(a.value)
            elif isinstance(a.value, (int, float)) and not isinstance(a.value, bool):
                parts.append(self.real(float(a.value), 32))
            else:
                return V(Unknown(f"a vector of {size}"), why)
        if len(parts) == 1:
            parts = parts * size
        if len(parts) < size:
            self.partial(f, node, f"a vector of {size} made of {len(parts)} values")
            return V(Unknown(f"a vector of {size}"), why)
        return V(Vec(parts[:size]), why, 32)

    def each(self, f, node, args: list, done) -> V:
        """An operation on vectors done on each component (a number given beside a vector: the same for each)."""
        sizes = {len(a.value) for a in args if isinstance(a.value, Vec)}
        size = max(sizes)
        if len(sizes) > 1:                           # vectors of different sizes: the languages do not allow it
            self.partial(f, node, f"vectors of sizes {sorted(sizes)} together")
            return V(Unknown("a vector"), Why("vectors of different sizes", self.where(f, node)))
        out = []
        for i in range(size):
            parts = [V(a.value[i] if isinstance(a.value, Vec) else a.value, a.why, 32) for a in args]
            got = done(parts)
            if not isinstance(got.value, (int, float)):
                return V(Unknown("a vector"), got.why)
            out.append(self.real(float(got.value), 32))
        return V(Vec(out), Why("each component", self.where(f, node), tuple(a.why for a in args)), 32)

    def vector_function(self, f, node, head: str, args: list) -> V:
        """dot, length, distance, normalize, cross, each operation rounded to 32 bits as written in GLSL 4.60 §8.5."""
        r = lambda x: self.real(x, 32)                                         # noqa: E731
        why = Why(head, self.where(f, node), tuple(a.why for a in args))
        vs = [a.value if isinstance(a.value, Vec) else Vec([float(a.value)]) for a in args]

        def dot(x, y) -> float:
            total = r(x[0] * y[0])
            for i in range(1, len(x)):
                total = r(total + r(x[i] * y[i]))
            return total

        def length(x) -> float:
            return r(math.sqrt(dot(x, x)))
        try:
            if head == DOT:
                return V(dot(vs[0], vs[1]), why, 32)
            if head == LENGTH:
                return V(length(vs[0]), why, 32)
            if head == DISTANCE:
                return V(length(Vec(r(a - b) for a, b in zip(vs[0], vs[1]))), why, 32)
            if head == NORMALIZE:
                n = length(vs[0])
                return V(Vec(r(c / n) for c in vs[0]), why, 32)
            if head == CROSS and len(vs[0]) == 3 == len(vs[1]):
                a, b = vs[0], vs[1]
                return V(Vec([r(r(a[1] * b[2]) - r(b[1] * a[2])), r(r(a[2] * b[0]) - r(b[2] * a[0])),
                              r(r(a[0] * b[1]) - r(b[0] * a[1]))]), why, 32)
        except (ZeroDivisionError, ValueError):
            pass
        self.partial(f, node, f"{head} is not defined there")
        return V(Unknown(head), why)

    def stepwise(self, f, head: str, values: list, bits: int):
        """mix and smoothstep done as the languages write them, each operation rounded to the reals' size."""
        r = lambda x: self.real(x, bits)                                       # noqa: E731
        x = [float(v) for v in values]
        if head == MIX and f.lang.mix_weights_both:
            return r(r(x[0] * r(1.0 - x[2])) + r(x[1] * x[2]))
        if head == MIX:
            return r(x[0] + r(r(x[1] - x[0]) * x[2]))
        t = min(max(r(r(x[2] - x[0]) / r(x[1] - x[0])), 0.0), 1.0)
        return r(r(t * t) * r(3.0 - r(2.0 * t)))

    def function(self, f, node, head: str, args: list) -> V:
        values = [Fraction(a.value) if isinstance(a.value, int) else Fraction(a.value) for a in args]
        if any(not math.isfinite(float(a.value)) for a in args):
            return V(Unknown(head), Why(head, self.where(f, node), tuple(a.why for a in args)))
        if head in (MIX, SMOOTH) and len(args) == 3 and any(isinstance(a.value, float) for a in args):
            bits = 32 if any(a.bits == 32 for a in args) else 64
            try:
                return V(self.stepwise(f, head, values, bits), Why(head, self.where(f, node),
                                                                   tuple(a.why for a in args)), bits)
            except ZeroDivisionError:
                self.partial(f, node, f"{head} is not defined there")
                return V(Unknown(head), Why(head, self.where(f, node)))
        done = number_function(head, values[:FUNCTIONS[head]] if head not in (MIN, MAX) else values)
        why = Why(head, self.where(f, node), tuple(a.why for a in args))
        if done is None:
            self.partial(f, node, f"{head} is not defined there")
            return V(Unknown(head), why)
        bits = 32 if any(a.bits == 32 for a in args) else 64
        whole = all(isinstance(a.value, int) for a in args) and done.denominator == 1 and \
            f.lang.numbers != DOUBLES and head in (MIN, MAX, ABS, CLAMP, SIGN)
        value = int(done) if whole else self.real(float(done), bits)
        return V(value, why, bits)

    # ---- expressions --------------------------------------------------------------------------------------------
    def ev(self, f, node, env, this) -> V:
        if node.has_error:                           # the grammar did not read it whole: not worked out, said
            self.partial(f, node, f"the grammar does not read this: {' '.join(text(node).split())[:40]}")
            return V(Unknown(text(node)[:40]), Why("not read by the grammar", self.where(f, node)))
        op = self.fact(f, node, OPERATION)
        if op is not None and OP in op:
            return self.operate(f, node, op, env, this)
        branch = self.fact(f, node, BRANCH)
        if branch is not None and ELSE in branch:
            test = self.ev(f, branch[TEST], env, this)
            held = self.truth(f, test)
            if held is None:
                self.partial(f, node, f"a choice on an unknown value ({unknown_text(test)})")
                return V(Unknown("a choice"), Why("a choice on an unknown value", self.where(f, node)))
            return self.ev(f, branch[THEN] if held else branch[ELSE], env, this)
        at = self.p.at.get((f.index, node.id))
        if at is not None and self.p.defs[at].kind == FUNCTION:
            return V(Fn(at, this, env), Why("a function written here", self.where(f, node)))
        role = f.role(node)
        kind = role[0] if role else None
        if kind == LITERAL:
            return self.literal(f, node, env, this)
        if kind == IDENT:
            return self.read(f, node, env, this)
        if kind in (SELF,):
            return this if this is not None else self.unknown("the object it runs on", f, node)
        if kind == MEMBER:
            return self.member(f, node, role[1], env, this)
        if kind == INDEX:
            holder = self.ev(f, role[1][OBJECT], env, this)
            key = self.ev(f, role[1][KEY], env, this)
            return self.element(f, node, holder, key)
        if kind == CALL:
            return self.invoke(f, node, role[1], env, this)
        if kind == NEW:
            cls = self.ev(f, role[1][CALLEE], env, this)
            args = [self.ev(f, a, env, this) for a in (child_nodes(role[1][ARGS]) if ARGS in role[1] else [])]
            if isinstance(cls.value, ClassRef):
                return self.make(f, node, cls.value.d, args, env)
            return self.output(f, node, " ".join(text(role[1][CALLEE]).split()), args)
        if kind == CHAIN:
            return self.chain(f, node, env, this)
        assign = self.fact(f, node, ASSIGN)
        if assign is not None:
            return self.assign(f, node, assign, env, this)
        passed = f.passes.get(node.id)
        if passed and len(passed) == 1:
            return self.ev(f, passed[0], env, this)
        if passed and len(passed) > 1:               # a node holding each of its parts' values: a list in place
            return V([self.ev(f, k, env, this) for k in passed], Why("a list written here", self.where(f, node)))
        kids = [k for k in child_nodes(node) if not k.is_extra]
        if len(kids) == 1:
            return self.ev(f, kids[0], env, this)
        self.partial(f, node, f"not read as a value: {' '.join(text(node).split())[:40]}")
        return V(Unknown(text(node)[:40]), Why("not read", self.where(f, node)))

    def operate(self, f, node, op, env, this) -> V:
        written = text(op[OP])
        head = f.lang.spelled_table.get(written, written)
        if LEFT not in op:
            v = self.ev(f, op[RIGHT], env, this)
            if head == NOT:
                held = self.truth(f, v)
                return V(Unknown("!") if held is None else not held, Why("!", self.where(f, node), (v.why,)))
            if head == SUB:
                if isinstance(v.value, (int, float)) and not isinstance(v.value, bool):
                    return V(-v.value, Why("-", self.where(f, node), (v.why,)), v.bits)
                if isinstance(v.value, Vec):
                    return V(Vec(-c for c in v.value), Why("-", self.where(f, node), (v.why,)), 32)
                return V(Unknown("-"), Why("-", self.where(f, node), (v.why,)))
            if head == ADD:
                return v
            self.partial(f, node, f"the operator {written}")
            return V(Unknown(written), Why(written, self.where(f, node)))
        if head in (ALL, ANY):                       # the right side only when it decides
            left = self.ev(f, op[LEFT], env, this)
            held = self.truth(f, left)
            if held is None:
                right = self.ev(f, op[RIGHT], env, this)
                return V(Unknown(head), Why(head, self.where(f, node), (left.why, right.why)))
            if (head == ALL and not held) or (head == ANY and held):
                return left if f.lang.picks_operand else V(held, Why(head, self.where(f, node), (left.why,)))
            right = self.ev(f, op[RIGHT], env, this)
            if f.lang.picks_operand:
                return right
            other = self.truth(f, right)
            return V(Unknown(head) if other is None else other, Why(head, self.where(f, node), (left.why, right.why)))
        left, right = self.ev(f, op[LEFT], env, this), self.ev(f, op[RIGHT], env, this)
        if head in (ADD, SUB, MUL, DIV, MOD, LT, LE, GT, GE, EQ, NE):
            return self.arith(f, node, head, left, right)
        if head == POW:                              # `**` is the language's pow: the number function, its operands known
            if all(isinstance(v.value, (int, float)) and not isinstance(v.value, bool) for v in (left, right)):
                return self.function(f, node, POW, [left, right])
            self.partial(f, node, f"the operator ** on an unknown value ({unknown_text(left)}, {unknown_text(right)})")
            return V(Unknown(written), Why(written, self.where(f, node), (left.why, right.why)))
        self.partial(f, node, f"the operator {written}")
        return V(Unknown(written), Why(written, self.where(f, node), (left.why, right.why)))

    def literal(self, f, node, env, this) -> V:
        written = text(node)
        truths = f.lang.truths_table
        if written in truths:
            return V(truths[written], Why(f"constant {written}", self.where(f, node)))
        inside = unquoted(node)
        if inside is not None:
            return V(inside, Why(f"constant {written[:30]}", self.where(f, node)))
        at = self.p.at.get((f.index, node.id))
        if at is not None and self.p.is_shape(at):  # an object written in place
            obj = Obj(None)
            for held in self.p.names.get(at, {}).values():
                for d in held:
                    it = self.p.defs[d]
                    if VALUE in it.parts:
                        obj.fields[it.name] = self.ev(f, it.parts[VALUE], env, this)
            return V(obj, Why("an object written here", self.where(f, node)))
        if written[:1] in "[{(" or node.named_child_count and not written[:1].isdigit():
            items = [self.ev(f, k, env, this) for k in child_nodes(node) if not k.is_extra
                     and self.fact(f, k, PAIR) is None]
            return V(items, Why("a list written here", self.where(f, node)))
        if written[:1].isdigit() or written[:1] == ".":
            return self.number(f, node)
        if written in f.lang.nothing:
            return V(None, Why(f"constant {written}", self.where(f, node)))
        return self.number(f, node)

    def member(self, f, node, parts, env, this) -> V:
        holder = self.ev(f, parts[OBJECT], env, this)
        name = text(parts[PROPERTY])
        return self.field(f, node, holder, name)

    def field(self, f, node, holder: V, name: str) -> V:
        x = holder.value
        if isinstance(x, Vec):
            for letters in f.lang.components:
                if name and all(c in letters[:len(x)] for c in name):
                    picked = [x[letters.index(c)] for c in name]
                    why = Why(f"the components {name}", self.where(f, node), (holder.why,))
                    return V(picked[0], why, 32) if len(picked) == 1 else V(Vec(picked), why, 32)
            return self.unknown(f"{text(node)}", f, node, f"{name}: no such component of a vector of {len(x)}")
        if isinstance(x, Obj):
            if name in x.fields:
                return x.fields[name]
            m = self.method(x, name)
            if m is not None:
                getter = self.p.defs[m]
                if getter.getter:
                    return self.call(f, node, m, [], holder, None)
                return V(Fn(m, holder), Why(f"method {self.p.label(m)}", self.where(f, node)))
            return self.unknown(f"{name}", f, node, f"{name}: no such field of the object here")
        if isinstance(x, ClassRef):
            for d in self.sc.members(x.d, name):
                it = self.p.defs[d]
                if it.kind == FUNCTION:
                    return V(Fn(d), Why(f"function {self.p.label(d)}", self.where(f, node)))
                if it.kind in STORED:
                    return self.global_value(d, f, node)
        if isinstance(x, list) and name in f.lang.size_words:
            return V(len(x) if f.lang.numbers != DOUBLES else float(len(x)), Why("the size", self.where(f, node)))
        if isinstance(x, Unknown):
            return V(Unknown(f"{x.name}.{name}"), Why(f"{name} of an input", self.where(f, node), (holder.why,)))
        return self.unknown(f"{text(node)}", f, node)

    def element(self, f, node, holder: V, key: V) -> V:
        x, k = holder.value, key.value
        why = Why("an element of a collection", self.where(f, node), (holder.why, key.why))
        if isinstance(x, list) and isinstance(k, (int, float)) and not isinstance(k, bool) and float(k).is_integer():
            i = int(k)
            if 0 <= i < len(x):
                return x[i]
            self.partial(f, node, f"position {i} out of the collection of {len(x)}")
            return V(Unknown(f"[{i}]"), why)
        if isinstance(x, Obj) and isinstance(k, str):
            if k in x.fields:
                return x.fields[k]
            self.partial(f, node, f"no key {k!r} here")
            return V(Unknown(k), why)
        if isinstance(x, str) and isinstance(k, (int, float)) and 0 <= int(k) < len(x):
            return V(x[int(k)], why)
        return V(Unknown("an element"), why)

    def chain(self, f, node, env, this) -> V:
        kids = child_nodes(node)
        value = self.ev(f, kids[0], env, this)
        for step in kids[1:]:
            role = f.role(step)
            if role is None or role[0] != STEP:
                continue
            sp = role[1]
            if CONSTRUCT in sp:
                args = [self.ev(f, a, env, this) for a in (child_nodes(sp[ARGS]) if ARGS in sp else [])]
                if isinstance(value.value, ClassRef):
                    value = self.make(f, step, value.value.d, args, env)
                else:
                    value = self.output(f, step, text(step), args)
                continue
            name = text(sp[PROPERTY]) if PROPERTY in sp else ""
            if ARGS in sp:
                args = [self.ev(f, a, env, this) for a in child_nodes(sp[ARGS])]
                target = None
                if isinstance(value.value, Obj):
                    held = value.value.fields.get(name)
                    if held is not None and isinstance(held.value, Fn):
                        target = held
                    else:
                        m = self.method(value.value, name)
                        target = V(Fn(m, value), value.why) if m is not None else None
                elif isinstance(value.value, Fn) and name in f.lang.invokes_table:
                    start = f.lang.invokes_table[name]
                    value = self.apply(f, step, value, args[start:] if start is not None else [])
                    continue
                if target is not None:
                    value = self.apply(f, step, target, args)
                    continue
                fired = [d for d, kind in self.sites.get((f.index, step.id), []) if kind == FIRES]
                back = self.output(f, step, name, args)
                for d in fired:
                    self.call(f, step, d, [a for a in args if not isinstance(a.value, str)], None, None)
                value = back
            elif KEY in sp:
                value = self.element(f, step, value, self.ev(f, sp[KEY], env, this))
            else:
                value = self.field(f, step, value, name)
        return value

    # ---- writing ------------------------------------------------------------------------------------------------
    def assign(self, f, node, parts, env, this) -> V:
        left = parts[LEFT]
        if RIGHT not in parts:                       # a step up or down
            old = self.ev(f, left, env, this)
            spelled = text(node)
            head = next((h for mark, h in f.lang.steps if mark in spelled), ADD)
            value = self.arith(f, node, head, old, V(1, Why("a step of one", self.where(f, node))))
            self.store(f, node, left, value, env, this)
            return old if spelled.endswith(("++", "--")) else value
        value = self.ev(f, parts[RIGHT], env, this)
        for k in child_nodes(node, False):
            if not k.is_named and k.start_byte >= left.end_byte and k.end_byte <= parts[RIGHT].start_byte:
                mark = k.type
                if len(mark) > 1 and mark.endswith("=") and mark not in ("==", "!=", "<=", ">="):
                    head = f.lang.spelled_table.get(mark[:-1], mark[:-1])
                    value = self.arith(f, node, head, self.ev(f, left, env, this), value)
                break
        self.store(f, node, left, value, env, this)
        return value

    def store(self, f, node, left, value: V, env, this) -> None:
        value = V(value.value, Why(f"written at {self.where(f, node)}", self.where(f, node), (value.why,)),
                  value.bits)
        role = f.role(left)
        kind = role[0] if role else None
        if kind == IDENT:
            name = text(left)
            found = self.sc.lookup(f, left, name)[0]
            if not found:
                if this is not None and isinstance(this.value, Obj):
                    this.value.fields[name] = value
                return
            d = found[0]
            it = self.p.defs[d]
            frame = self.frame_of(env, d)
            if frame is not None:
                frame[d] = self.kept_as(f, d, value)
            elif it.kind in STORED and it.owner >= 0 and self.p.is_class(it.owner):
                target = this.value if this is not None and isinstance(this.value, Obj) else self.script(it.owner).value
                target.fields[it.name] = self.kept_as(f, d, value)
            else:
                self.globals[d] = self.kept_as(f, d, value)
            return
        if kind == MEMBER:
            holder = self.ev(f, role[1][OBJECT], env, this)
            if isinstance(holder.value, Obj):
                holder.value.fields[text(role[1][PROPERTY])] = value
            else:
                self.output(f, node, f"{text(role[1][PROPERTY])} =", [value])
            return
        if kind == INDEX:
            holder = self.ev(f, role[1][OBJECT], env, this)
            key = self.ev(f, role[1][KEY], env, this)
            if isinstance(holder.value, list) and isinstance(key.value, (int, float)) and \
                    not isinstance(key.value, bool):
                i = int(key.value)
                if 0 <= i < len(holder.value):
                    holder.value[i] = value
                elif i == len(holder.value) and f.lang.numbers == DOUBLES:
                    holder.value.append(value)
                else:
                    self.partial(f, node, f"position {i} written out of the collection of {len(holder.value)}")
            elif isinstance(holder.value, Obj) and isinstance(key.value, str):
                holder.value.fields[key.value] = value
            else:
                self.partial(f, node, "an element written in what is not known")
            return
        if kind == CHAIN:
            kids = child_nodes(left)
            holder = self.ev(f, kids[0], env, this)
            for step in kids[1:-1]:
                sp = f.role(step)[1] if f.is_a(step, STEP) else {}
                holder = self.field(f, step, holder, text(sp[PROPERTY])) if PROPERTY in sp else holder
            last = f.role(kids[-1])[1] if f.is_a(kids[-1], STEP) else {}
            if PROPERTY in last and isinstance(holder.value, Obj):
                holder.value.fields[text(last[PROPERTY])] = value
            elif KEY in last:
                key = self.ev(f, last[KEY], env, this)
                if isinstance(holder.value, list) and isinstance(key.value, (int, float)):
                    if 0 <= int(key.value) < len(holder.value):
                        holder.value[int(key.value)] = value
                elif isinstance(holder.value, Obj):
                    holder.value.fields[str(key.value)] = value
            else:
                self.output(f, node, f"{text(left)} =", [value])
            return
        self.partial(f, node, f"a write to {text(left)[:30]} not read")

    # ---- statements ---------------------------------------------------------------------------------------------
    def exec(self, f, node, env, this, unit) -> None:
        self.tick(f, node)
        key = (f.index, node.id)
        facts = self.facts.get(key, {})
        if BRANCH in facts and ELSE not in facts[BRANCH]:
            return self.branch(f, node, facts[BRANCH], env, this, unit)
        if CHOOSE in facts:
            return self.choose(f, node, facts[CHOOSE], env, this, unit)
        if LEAVE in facts:
            raise Leave()
        if AGAIN in facts:
            raise Again()
        if EXIT in facts:
            ret = self.fact(f, node, RETURN)
            if ret is not None and VALUE in ret:
                raise Return(self.ev(f, ret[VALUE], env, this))
            if node.named_child_count and ret is None:      # a throw: leaves with what it throws, said
                self.partial(f, node, "an exception thrown: what catches it is not followed")
                raise Stop()
            raise Return(V(None, Why("it returns", self.where(f, node))))
        at = self.p.at.get(key)
        if at is not None:
            it = self.p.defs[at]
            if it.kind in (VARIABLE,) and it.owner >= 0 and self.p.defs[it.owner].kind == FUNCTION:
                frame = env[0]
                if VALUE in it.parts:
                    frame[at] = self.kept_as(f, at, self.ev(f, it.parts[VALUE], env, this))
                else:
                    frame[at] = self.default(f, at)
                return
            if it.kind == FUNCTION:                  # a function written here: a value of the frame
                env[0][at] = V(Fn(at, this, env), Why("a function written here", self.where(f, node)))
                return
            if it.kind == CLASS:
                return
        if f.role(node) is not None or ASSIGN in facts or OPERATION in facts or node.id in f.passes:
            self.ev(f, node, env, this)
            return
        for kid in child_nodes(node):
            if not kid.is_extra:
                self.exec(f, kid, env, this, unit)

    def branch(self, f, node, parts, env, this, unit) -> None:
        if LOOP in parts:
            return self.loop(f, node, parts, env, this, unit)
        test = self.ev(f, parts[TEST], env, this)
        held = self.truth(f, test)
        where = self.where(f, parts[TEST])
        if held is None:
            self.partial(f, node, f"a branch on an unknown value ({unknown_text(test)}): neither way followed")
            return
        then = parts[THEN]
        if held:
            self.path.append((where, test, True))
            try:
                self.exec(f, then, env, this, unit)
            finally:
                self.path.pop()
            return
        self.skip(f, then, where, test)
        rest = [k for k in child_nodes(node) if not k.is_extra and not overlaps(k, parts[TEST])
                and not overlaps(k, then)]
        self.path.append((where, test, False))
        try:
            for k in rest:
                other = self.fact(f, k, BRANCH)
                if other is not None and LOOP not in other and ELSE not in other:      # an elif
                    t = self.ev(f, other[TEST], env, this)
                    h = self.truth(f, t)
                    if h is None:
                        self.partial(f, k, f"a branch on an unknown value ({unknown_text(t)})")
                        return
                    if h:
                        self.path.append((self.where(f, other[TEST]), t, True))
                        try:
                            self.exec(f, other[THEN], env, this, unit)
                        finally:
                            self.path.pop()
                        return
                    self.skip(f, other[THEN], self.where(f, other[TEST]), t)
                    continue
                self.exec(f, k, env, this, unit)
                return
        finally:
            self.path.pop()

    def skip(self, f, node, where: str, test: V) -> None:
        self.run.skipped.append((f.display, node.start_point[0] + 1, node.end_point[0] + 1, where, test))

    def loop(self, f, node, parts, env, this, unit) -> None:
        then = parts[THEN]
        test = parts.get(TEST)
        if test is None and AFTER not in parts:      # over a collection, a number, an object's keys
            return self.over(f, node, parts, env, this, unit)
        before = [k for k in child_nodes(node) if not k.is_extra and test is not None and
                  k.end_byte <= test.start_byte and not overlaps(k, then)]
        after_test = [k for k in child_nodes(node) if not k.is_extra and test is not None and
                      k.start_byte >= test.end_byte and k.end_byte <= then.start_byte]
        scope = ({}, env)
        for k in before:
            self.exec(f, k, scope, this, unit)
        turns = 0
        first = AFTER in parts
        while True:
            if not first:
                cond = self.ev(f, test if test is not None else parts[AFTER], scope, this)
                held = self.truth(f, cond)
                if held is None:
                    self.partial(f, node, f"a loop on an unknown value ({unknown_text(cond)}): stopped")
                    return
                if not held:
                    self.skip(f, then, self.where(f, test if test is not None else parts[AFTER]), cond)
                    return
            first = False
            turns += 1
            if turns > TURNS:
                self.partial(f, node, f"stopped after {TURNS} turns")
                return
            try:
                self.exec(f, then, scope, this, unit)
            except Leave:
                return
            except Again:
                pass
            for k in after_test:
                self.ev(f, k, scope, this) if f.role(k) or self.fact(f, k, ASSIGN) else self.exec(f, k, scope, this,
                                                                                                  unit)
            if AFTER in parts:
                cond = self.ev(f, parts[AFTER], scope, this)
                held = self.truth(f, cond)
                if held is None:
                    self.partial(f, node, f"a loop on an unknown value ({unknown_text(cond)}): stopped")
                    return
                if not held:
                    return
                first = True

    def over(self, f, node, parts, env, this, unit) -> None:
        """A loop over what a value holds: the variable it declares takes each in turn."""
        then = parts[THEN]
        declared = self.declared(f, node, then)
        if not declared:
            self.partial(f, node, "a loop whose variable is not read")
            return
        d = declared[0]
        it = self.p.defs[d]
        source = self.ev(f, it.parts[VALUE], env, this) if VALUE in it.parts else None
        if source is None or isinstance(source.value, Unknown):
            self.partial(f, node, "a loop over an unknown collection: its turns are not followed")
            return
        x = source.value
        keys = f.lang.keys_word and any(not k.is_named and k.type == f.lang.keys_word for k in child_nodes(node, False))
        if isinstance(x, list):
            items = [V(i if f.lang.numbers != DOUBLES else float(i), Why("a position", self.where(f, node)))
                     for i in range(len(x))] if keys else list(x)
        elif isinstance(x, Obj):
            items = [V(k, Why("a key", self.where(f, node))) for k in x.fields] if keys or f.lang.numbers == DYNAMIC \
                else list(x.fields.values())
        elif isinstance(x, (int, float)) and not isinstance(x, bool) and f.lang.counts:
            items = [V(i, Why(f"turn {i}", self.where(f, node))) for i in range(int(x))]
        elif isinstance(x, str):
            items = [V(c, Why("a character", self.where(f, node))) for c in x]
        else:
            self.partial(f, node, "a loop over what is not a collection read")
            return
        if len(items) > TURNS:
            self.partial(f, node, f"stopped after {TURNS} turns")
            items = items[:TURNS]
        for item in items:
            scope = ({d: item}, env)
            try:
                self.exec(f, then, scope, this, unit)
            except Leave:
                return
            except Again:
                continue

    def declared(self, f, node, then) -> list:
        """The variables a loop's head declares."""
        out = []
        stack = [k for k in child_nodes(node) if k.end_byte <= then.start_byte]
        while stack:
            k = stack.pop()
            at = self.p.at.get((f.index, k.id))
            if at is not None and self.p.defs[at].kind == VARIABLE:
                out.append(at)
            stack.extend(child_nodes(k))
        at = self.p.at.get((f.index, node.id))
        if at is not None and self.p.defs[at].kind == VARIABLE:
            out.append(at)
        return sorted(out, key=lambda d: self.p.defs[d].node.start_byte)

    def choose(self, f, node, parts, env, this, unit) -> None:
        value = self.ev(f, parts[TEST], env, this)
        cases = []
        stack = list(child_nodes(node))
        while stack:
            k = stack.pop(0)
            got = self.facts.get((f.index, k.id), {}).get(CASE)
            if got is not None:
                cases.append((k, got))
            elif CHOOSE not in self.facts.get((f.index, k.id), {}):
                stack = list(child_nodes(k)) + stack
        start = None
        for i, (k, got) in enumerate(cases):
            if VALUE in got:
                v = self.ev(f, got[VALUE], env, this)
                same = self.arith(f, k, EQ, value, v)
                if same.value is True:
                    start = i
                    break
                if isinstance(same.value, Unknown):
                    self.partial(f, k, f"a case on an unknown value ({unknown_text(same)})")
                    return
        if start is None:
            start = next((i for i, (k, got) in enumerate(cases) if VALUE not in got), None)
        if start is None:
            return
        try:
            for k, got in cases[start:]:
                for s in child_nodes(k):
                    if VALUE in got and s.id == got[VALUE].id:
                        continue
                    self.exec(f, s, env, this, unit)
                if not f.lang.falls_through:
                    return
        except Leave:
            return

    # ---- a start ------------------------------------------------------------------------------------------------
    def start(self, d: int, args: list) -> V:
        """A function run as a start: on the one object of its class, where it is a method."""
        it = self.p.defs[d]
        this = self.script(it.owner) if it.owner >= 0 and self.p.is_class(it.owner) and not self.p.is_shape(
            it.owner) else None
        f = self.p.files[it.file]
        given = []
        for i, params in sorted(self.sc.params[d].items()):
            p = params[0]
            label, name = self.p.label(p), self.p.defs[p].name
            if i < len(args):
                given.append(self.given(args[i], Why(f"given: {name}")))
            elif label in self.situation or name in self.situation:
                given.append(self.unknown(label))
            else:
                given.append(self.unknown(label, f, it.node, f"{name}: the start's parameter, not given"))
        try:
            return self.call(f, it.node, d, given, this, None)
        except Stop:
            return V(Unknown("it stopped"), Why("it stopped"))

    def state(self) -> dict:
        """The state now: every state of a file or a namespace, and the fields of the objects run as scripts."""
        out = {}
        for d, v in self.globals.items():
            out[self.p.label(d)] = plain(v)
        for c, v in self.instances.items():
            for name, held in v.value.fields.items():
                out[f"{self.p.label(c)}.{name}"] = plain(held)
        return out




def overlaps(k, part) -> bool:
    return k.start_byte <= part.start_byte and part.end_byte <= k.end_byte or \
        part.start_byte <= k.start_byte and k.end_byte <= part.end_byte


def unknown_name(x) -> str:
    return x.name if isinstance(x, Unknown) else repr(x)


def unknown_text(v) -> str:
    """An unknown value said so a reader can give it: the input it is, or what it was made of, and where (a partial
    names what it waits for: `--given NAME=VALUE` gives it)."""
    if not isinstance(v, V):
        return repr(v)
    if not isinstance(v.value, Unknown):
        return repr(plain(v))
    inputs = []
    stack = [v.why]
    while stack and len(inputs) < 3:
        w = stack.pop()
        if w.say.startswith("input "):
            inputs.append(f"{w.say[len('input '):]}{f' at {w.where}' if w.where else ''}")
        stack.extend(reversed(w.parts))
    return "; ".join(inputs) if inputs else f"{v.value.name}{f' at {v.why.where}' if v.why.where else ''}"


def plain(v):
    """A value as plain data (lists, objects as their fields)."""
    if not isinstance(v, V):
        return v
    x = v.value
    if isinstance(x, list):
        return [plain(i) for i in x]
    if isinstance(x, Obj):
        return {k: plain(i) for k, i in x.fields.items()}
    if isinstance(x, Unknown):
        return f"<{x.name}>"
    if isinstance(x, (Fn, ClassRef)):
        return f"<{type(x).__name__}>"
    return x



# ---- the questions ------------------------------------------------------------------------------------------------
def evaluate(result, steps: list, situation: dict | None = None, repeat: int = 1) -> Run:
    """A sequence of starts run in turn, the state passed on (each step: (start definition, [argument values])),
    repeated; where the state after a step is the state after the one before, a fixed point; where it is the state
    after an earlier one, a cycle."""
    m = Machine(result, situation)
    seen = []
    for turn in range(repeat):
        for d, args in steps:
            before = len(m.run.outputs)
            m.path = []
            back = m.start(d, args)
            m.run.returned = back
            now = m.state()
            m.run.steps.append((result.program.label(d), m.run.outputs[before:], now))
            key = repr(sorted(now.items(), key=lambda kv: kv[0]))
            if seen and seen[-1] == key and m.run.fixpoint is None and len(m.run.outputs) == before:
                m.run.fixpoint = len(m.run.steps) - 1
            elif key in seen and m.run.cycle is None:
                m.run.cycle = (seen.index(key) + 1, len(m.run.steps))
            seen.append(key)
    m.run.state = m.state()
    return m.run


def why_lines(why: Why, depth: int = 0, limit: int = 60, out=None) -> list:
    """The derivation of a value: what it is and where, down to the inputs and constants it came from."""
    out = [] if out is None else out
    if len(out) >= limit:
        return out
    out.append("  " * depth + why.say + (f"  [{why.where}]" if why.where else ""))
    for part in why.parts:
        why_lines(part, depth + 1, limit, out)
    return out


def why_not(run: Run, at: str) -> list:
    """Why the code at `file:line` did not run: the conditions that were false on the way to it (innermost first),
    each where it is written and the value it had, and that value's derivation."""
    path, _, line = at.rpartition(":")
    line = int(line) if line.isdigit() else 0
    found = [(end - start, where, test) for display, start, end, where, test in run.skipped
             if (display == path or display.endswith(path) or path.endswith(display)) and start <= line <= end]
    found.sort(key=lambda row: row[0])
    out = []
    for _, where, test in found[:3]:
        out.append(f"the condition at {where} was {plain(test)!r}")
        out += ["    " + row for row in why_lines(test.why, limit=20)]
    if not found:
        out.append(f"no condition on the way to {at} was false in this run: the function it is in did not run "
                   "(nothing called it from the start)")
    return out


def run_text(result, run: Run) -> str:
    lines = []
    for label, outputs, state in run.steps:
        lines.append(f"Start {label}:")
        for o in outputs:
            args = ", ".join(repr(plain(a)) for a in o.args)
            lines.append(f"  {o.where}  {o.callee}({args})" + ("  (partial)" if o.partial else ""))
            for where, test, taken in o.path[-4:]:
                lines.append(f"      because {where} was {'true' if taken else 'false'}")
    if run.returned is not None:                 # what the last start gave back: a pure function's output
        lines.append(f"Returned: {plain(run.returned)!r}" + (f"  [{run.returned.why.where}]" if isinstance(
            run.returned, V) and run.returned.why.where else ""))
        if isinstance(run.returned, V):
            lines += ["    " + row for row in why_lines(run.returned.why, limit=20)]
    lines.append("State at the end:")
    lines += [f"  {k} = {v!r}" for k, v in sorted(run.state.items())]
    if run.fixpoint is not None:
        lines.append(f"After step {run.fixpoint} nothing changes (a fixed point).")
    if run.cycle is not None:
        lines.append(f"The state at step {run.cycle[1]} is the state at step {run.cycle[0]} (a cycle).")
    if run.reads_before_writes:
        lines.append("Read before anything wrote it:")
        lines += [f"  {where}  {what}" for where, what in run.reads_before_writes]
    if run.partial:
        lines.append(f"Partial ({len(run.partial)}): not worked out here, and the results through it:")
        lines += [f"  {where}  {why}" for where, why in run.partial[:40]]
    return "\n".join(lines)


def run_data(result, run: Run) -> dict:
    return {"the steps": [{"the start": label,
                           "the outputs": [{"where it is": o.where, "what is called": o.callee,
                                            "the values": [plain(a) for a in o.args], "is partial": o.partial,
                                            "the conditions taken": [{"where it is": w, "it held": bool(t)} for w, _, t in o.path],
                                            "the values come from": [why_lines(a.why, limit=12) for a in o.args]}
                                           for o in outputs],
                           "the state after": state} for label, outputs, state in run.steps],
            "what it gave back": plain(run.returned) if run.returned is not None else None,
            "where what it gave back comes from": why_lines(run.returned.why, limit=20) if isinstance(run.returned, V)
            else [],
            "the state at the end": run.state, "a fixed point after step": run.fixpoint, "a cycle": run.cycle,
            "read before written": run.reads_before_writes,
            "not worked out": [{"where it is": w, "the reason": y} for w, y in run.partial]}
