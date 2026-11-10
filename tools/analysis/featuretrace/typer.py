"""The classes behind a receiver: `x` in `x.f()`, `a.b` in `a.b.c()`, `get()` in `get().f()`.

A name's type is its declared type, or the union of the types of every value that flows into it (`flows`): its
initializer and assignments, the arguments at every call site of a parameter, the values a function returns, the values
put into a container. A class template's parameters are replaced by the arguments written; `[]` and `->` on a class of
the program go through its `operator[]` / `operator->`; an operator on a class goes through its `operator<op>`. A type
that is no class of the program answers by the library's rules: `->`, `get()`, `value()` give what it holds, `[]` and
`at()` an element, a range-for its elements (a map's pairs), `find()`/`begin()` an iterator. A value the program does not
create is the engine's. A function used as a value is a callable: calling it reaches that function.
"""
from __future__ import annotations

import re

from .cpp_expr import DEREF, ITEM, ITER
from .cpp_names import TYPED, strip_templates, template_args
from .flows import ANY, ENGINE, LAMBDA, OF, OP, PUT, SEP, split
from .gd_body import CALLED, UNKNOWN
from .model import FUNCTION_KINDS

CLASS = "#"
FN = "<fn>"             # a function used as a value (a Callable, a function pointer, a lambda)
ELEM = "<elem>"         # an element put into a container (what a subscript or a loop over it yields)
ITERATOR = "std::__iterator"
QUALIFIERS = re.compile(r"\b(const|volatile|static|constexpr|inline|mutable|typename|struct|class|unsigned)\b|[&*]")
STEP = re.compile(r"\[[^\]]*\]|->|\.|[^.\-\[\]]+")
ITERATOR_CALLS = ("find", "begin", "end", "cbegin", "cend", "rbegin", "rend", "lower_bound", "upper_bound")
HELD_CALLS = ("get", "value", "front", "back", "top", "lock")
OPERATORS = {ITEM: "operator[]", DEREF: "operator->", ITER: "begin"}
MARKERS = (OF, CALLED, TYPED, ANY, OP, LAMBDA, PUT)
COMPARISONS = ("and", "or", "not", "&&", "||", "==", "!=", "<", ">", "<=", ">=", "in", "is")
DEPTH = 12


def parse_chain(text: str) -> tuple:
    """`a.b->c[i]` -> ("a", "b", "c", "<item>")."""
    out = []
    for token in STEP.findall(text.replace("()", "")):
        if token.startswith("["):
            out.append(ITEM)
        elif token == "->":
            out.append(DEREF)
        elif token != ".":
            out.append(token.strip())
    return tuple(t for t in out if t)


def clean(text: str) -> str:
    return " ".join(QUALIFIERS.sub(" ", text).split())


class Typer:
    def __init__(self, index, resolver):
        self.ix = index
        self.rs = resolver
        self._busy = set()

    def class_of(self, unit) -> str | None:
        return unit.owner if unit.owner in self.ix.classes else None

    # ---- values ------------------------------------------------------------------------------------------------
    def settle(self, marker: str, ctx, depth: int = 0) -> list[tuple]:
        """Every declared type (with the unit it is read in) a marker stands for."""
        if not marker or ctx is None or depth > DEPTH:
            return []
        if not marker.startswith(MARKERS):
            return [(marker, ctx)]
        key = (marker, ctx.gid)
        if key in self._busy:
            return []
        self._busy.add(key)
        try:
            return self._settle(marker, ctx, depth)
        finally:
            self._busy.discard(key)

    def _settle(self, marker: str, ctx, depth: int) -> list[tuple]:
        if marker.startswith(ANY):
            return [v for m in split(marker) for v in self.settle(m, ctx, depth + 1)]
        if marker.startswith(OF):
            return self.chain(parse_chain(marker[len(OF):]), ctx, depth + 1)
        if marker.startswith(TYPED):
            return [(marker[len(TYPED):], ctx)]
        if marker.startswith(PUT):
            return [(ELEM + t, c) for t, c in self.settle(marker[len(PUT):], ctx, depth + 1) if not t.startswith(ELEM)]
        if marker.startswith(LAMBDA):
            return [(FN + str(self.ix.offset[ctx.file] + int(marker[len(LAMBDA):])), ctx)]
        if marker.startswith(OP):
            op, _, left = marker[len(OP):].partition(SEP)
            return [v for t, c in self.settle(left, ctx, depth + 1) for v in self.operator(op, t, c, depth)]
        steps = parse_chain(marker[len(CALLED):])
        if len(steps) > 1:
            return self.chain(steps[:-1] + (CALLED + steps[-1],), ctx, depth + 1)
        return self.call_result(steps[0] if steps else "", ctx, depth)

    def call_result(self, name: str, ctx, depth: int) -> list[tuple]:
        """What a call by plain name yields: a class constructed, a function's return values, a callable's."""
        key = self.ix.gd_class(name, ctx.owner) if ctx.lang == "gd" else self.ix.cpp_class(name, ctx.owner, ctx.file)
        if key:
            return [(CLASS + key, ctx)]
        targets = self.rs.function_targets(ctx, name)
        if targets:
            return [v for t in targets for v in self.returns(self.ix.units[t], depth)]
        # not the program's: an engine type constructed (Godot names types in PascalCase) or an engine value
        return [(name, ctx)] if ctx.lang == "gd" and name[:1].isupper() else [(ENGINE, ctx)]

    def returns(self, unit, depth: int) -> list[tuple]:
        """A function's result: its declared return type, else every value its `return`s carry back."""
        if unit.type_text and unit.type_text not in ("auto", "Variant", "decltype(auto)"):
            return [(unit.type_text, unit)]
        return [v for _, _, m in unit.returns for v in self.settle(m, unit, depth + 1)]

    def operator(self, op: str, text: str, ctx, depth: int) -> list[tuple]:
        if op == "*":
            return self.step(DEREF, text, ctx, depth)
        if op in (ITEM, ITER, DEREF, "first", "second"):
            return self.step(op, text, ctx, depth)
        if op in COMPARISONS:
            return [(ENGINE, ctx)]
        key = self.type_class(text, ctx)
        if key and ctx.lang == "cpp":
            found = self.ix.lookup(key, "operator" + op)
            if found:
                return self.returns(self.ix.units[self.ix.definitions(found)[0]], depth)
        return [(text, ctx)]

    def head(self, head: str, unit, depth: int) -> list[tuple]:
        if head in ("self", "this"):
            key = self.class_of(unit)
            return [(CLASS + key, unit)] if key else []
        if head == "super":
            key = self.class_of(unit)
            bases = self.ix.base_keys(self.ix.classes[key]) if key else []
            return [(CLASS + bases[0], unit)] if bases else []
        if head in (UNKNOWN, ITEM):
            return []
        if head == ENGINE:
            return [(ENGINE, unit)]
        if head.startswith(MARKERS):
            return self.settle(head, unit, depth + 1)
        if head in unit.lambdas:
            return [(FN + str(self.ix.offset[unit.file] + unit.lambdas[head]), unit)]
        if head in unit.types:
            return self.local(head, unit, depth)
        targets = self.rs.state_targets(unit, head)
        if targets:
            return [v for t in targets for v in self.held(self.ix.units[t], depth)]
        key = self.ix.gd_class(head, unit.owner) if unit.lang == "gd" else self.ix.cpp_class(head, unit.owner, unit.file)
        if key:
            return [(CLASS + key, unit)]
        functions = self.rs.function_targets(unit, head)
        if functions:
            return [(FN + str(f), unit) for f in functions]
        # a name no code of the program defines is the engine's or the library's (a global class or singleton)
        return [(ENGINE, unit)]

    def local(self, name: str, unit, depth: int) -> list[tuple]:
        """A local or a parameter: what it is declared as, else what flows into it (for a parameter, the arguments
        written at every place that calls the function)."""
        markers = unit.types.get(name, [])
        declared = [m for m in markers if m and not m.startswith(MARKERS) and m != ENGINE]
        if declared:
            return [(m, unit) for m in declared]
        found = [v for m in markers for v in self.settle(m, unit, depth + 1)] + \
            [(ENGINE, unit) for m in markers if m == ENGINE]
        if name in unit.params:
            found += self.rs.argument_values(unit, unit.params.index(name), depth)
        return found

    def held(self, unit, depth: int) -> list[tuple]:
        """What a variable or field holds: its declared type, else every value that flows into it."""
        if unit.type_text and unit.type_text != "Variant":
            return [(unit.type_text, unit)]
        if unit.kind in FUNCTION_KINDS:
            return [(FN + str(unit.gid), unit)]
        found = [v for m in unit.values for v in self.settle(m, unit, depth + 1)]
        return found + self.rs.assigned_values(unit, depth)

    # ---- types -------------------------------------------------------------------------------------------------
    def type_class(self, text: str | None, ctx) -> str | None:
        if not text or text == ENGINE or text.startswith((FN, ELEM)) or ctx is None:
            return None
        if text.startswith(CLASS):
            return text[1:]
        if ctx.lang == "gd":
            return self.ix.gd_class(text, ctx.owner)
        return self.ix.cpp_class(clean(text), ctx.owner, ctx.file)

    def library_step(self, step: str, text: str) -> str:
        text = text.replace("[", "<").replace("]", ">")      # GDScript's Array[T] / Dictionary[K, V]
        args = [a for a in template_args(clean(text)) if not a.strip().isdigit()]
        if strip_templates(clean(text)) == ITERATOR and args:
            return self.library_step(ITER, args[0]) if step in (DEREF, ITEM) else ENGINE
        if not args:
            return ENGINE
        name = step[len(CALLED):] if step.startswith(CALLED) else ""
        if name in ITERATOR_CALLS:
            return f"{ITERATOR}<{text}>"
        if step in (DEREF, ITER) and len(args) == 1 or name in HELD_CALLS:
            return args[0]
        if step == ITER:
            return f"std::pair<{args[0]}, {args[1]}>"
        if step == ITEM or name == "at":
            return args[-1]
        if step in ("first", "second") and len(args) >= 2:
            return args[0] if step == "first" else args[1]
        return ENGINE

    def substitute(self, key: str, outer: str, member: str) -> tuple[str, bool]:
        params = self.ix.classes[key].params
        args = template_args(clean(outer)) if params and not outer.startswith(CLASS) else []
        if not params or not args:
            return member, False
        out = member
        for param, arg in zip(params, args):
            out = re.sub(rf"\b{re.escape(param)}\b", arg, out)
        return out, out != member

    def step(self, step: str, text: str, ctx, depth: int) -> list[tuple]:
        """One step of a chain from a value of type `text`."""
        if text.startswith(ELEM):
            return [(text[len(ELEM):], ctx)] if step in (ITEM, ITER) else []
        if text == ENGINE or text.startswith(FN):
            return [(ENGINE, ctx)]
        key = self.type_class(text, ctx)
        if key is None:
            return [(self.library_step(step, text), ctx)]
        if step in (DEREF, ITEM, ITER) and text.startswith(CLASS):
            return [(text, ctx)]
        name = OPERATORS.get(step) or (step[len(CALLED):] if step.startswith(CALLED) else step)
        found = self.ix.lookup(key, name)
        if not found:
            inner = self.ix.inner[key].get(name)
            if inner:
                return [(CLASS + inner, ctx)]
            return [(text, ctx)] if step in (DEREF, ITEM, ITER) else [(ENGINE, ctx)]
        out = []
        for gid in self.ix.definitions(found):
            target = self.ix.units[gid]
            called = step.startswith(CALLED) or step in OPERATORS
            values = self.returns(target, depth) if called else self.held(target, depth)
            for value, where in values:
                value, replaced = self.substitute(key, text, value)
                if step == ITER and template_args(clean(value)):
                    value = self.library_step(DEREF, value)
                out.append((value, ctx if replaced else where))
        return out

    def chain(self, recv: tuple, unit, depth: int = 0) -> list[tuple]:
        """Every type (with the unit it is read in) a receiver chain can hold."""
        if not recv or depth > DEPTH:
            return []
        values = self.expand(self.head(recv[0], unit, depth), depth)
        for step in recv[1:]:
            values = self.expand([v for t, c in values for v in self.step(step, t, c, depth)], depth)
        return values

    def expand(self, values: list[tuple], depth: int) -> list[tuple]:
        out, seen = [], set()
        for text, ctx in values:
            for t, c in (self.settle(text, ctx, depth + 1) if text.startswith(MARKERS) else [(text, ctx)]):
                if t and (t, c.gid) not in seen:
                    seen.add((t, c.gid))
                    out.append((t, c))
        return out

    def receiver(self, recv: tuple, unit) -> tuple[list[str], bool, list[int]]:
        """(classes of the program the receiver can be, whether it can be an engine value, callables it can be)."""
        keys, engine, functions = [], False, []
        for text, ctx in self.chain(recv, unit):
            if text.startswith(ELEM):
                continue
            if text.startswith(FN):
                functions.append(int(text[len(FN):]))
                continue
            key = self.type_class(text, ctx)
            if key:
                keys.append(key)
            else:
                engine = True
        return list(dict.fromkeys(keys)), engine, list(dict.fromkeys(functions))
