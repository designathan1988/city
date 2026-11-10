"""The class behind a receiver: `x` in `x.f()`, `a.b` in `a.b.c()`, `get()` in `get().f()`.

The type comes from what the code declares: a parameter's or local's type, a field's or variable's type, a function's
return type, the class named by `self`/`this`/`super`, a class name used directly (static access, an autoload, a
`class_name`, a `preload` const), what an `auto` is initialised with. A class template's parameters are replaced by the
arguments written (`IdTable<RoadNode>`: `T` is `RoadNode`); a subscript or `->` on a class of the program goes through
its `operator[]` / `operator->`. A type that is no class of the program (engine, standard library) answers by the
library's own rules: `->` and `get()`/`value()` give what it holds, a subscript or `at()` an element, a range-for its
elements (a map's key and value pair), `find()`/`begin()` an iterator. When nothing declares a type the receiver is
unknown, and the resolver falls back to every definition of the name.
"""
from __future__ import annotations

import re

from .cpp_body import OF
from .cpp_expr import DEREF, ITEM, ITER
from .cpp_names import TYPED, strip_templates, template_args
from .gd_body import CALLED, UNKNOWN

CLASS = "#"
ENGINE = "<engine>"     # a type the code declares that is no class of the program (engine, standard library)
ITERATOR = "std::__iterator"
QUALIFIERS = re.compile(r"\b(const|volatile|static|constexpr|inline|mutable|typename|struct|class|unsigned)\b|[&*]")
STEP = re.compile(r"\[[^\]]*\]|->|\.|[^.\-\[\]]+")
ITERATOR_CALLS = ("find", "begin", "end", "cbegin", "cend", "rbegin", "rend", "lower_bound", "upper_bound")
HELD_CALLS = ("get", "value", "front", "back", "top", "lock")
OPERATORS = {ITEM: "operator[]", DEREF: "operator->", ITER: "begin"}


def parse_chain(text: str) -> tuple:
    """`a.b->c[i]` -> ("a", "b", "c", "<item>")."""
    out = []
    for token in STEP.findall(text):
        if token.startswith("["):
            out.append(ITEM)
        elif token == "->":
            out.append(DEREF)
        elif token != ".":
            out.append(token.strip())
    return tuple(t for t in out if t)


def clean(text: str) -> str:
    return " ".join(QUALIFIERS.sub(" ", text).split())


def declared(text: str | None) -> bool:
    return bool(text) and not text.startswith((CALLED, OF))


class Typer:
    def __init__(self, index, resolver):
        self.ix = index
        self.rs = resolver
        self._busy = set()

    def class_of(self, unit) -> str | None:
        """The class a unit is written in (its `self` / `this`)."""
        return unit.owner if unit.owner in self.ix.classes else None

    def settle(self, text, ctx) -> tuple:
        """An expression's type (`<of>a.b`, `<call>f`) worked out to a declared type."""
        for _ in range(8):
            if not text or ctx is None or declared(text):
                return text, ctx
            key = (text, ctx.gid)
            if key in self._busy:
                return None, None
            self._busy.add(key)
            try:
                text, ctx = self.expression_type(text, ctx)
            finally:
                self._busy.discard(key)
        return None, None

    def type_class(self, text: str | None, ctx) -> str | None:
        text, ctx = self.settle(text, ctx)
        if not text or text == ENGINE:
            return None
        if text.startswith(CLASS):
            return text[1:]
        if ctx.lang == "gd":
            return self.ix.gd_class(text, ctx.owner)
        return self.ix.cpp_class(clean(text), ctx.owner, ctx.file)

    def library_step(self, step: str, text: str) -> str:
        """What a library type yields (the standard library's rules); nothing known without template arguments."""
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

    def substitute(self, key: str, outer_text: str, member_text: str) -> tuple[str, bool]:
        """A member's type with the class template's parameters replaced by the arguments written."""
        params = self.ix.classes[key].params
        args = template_args(clean(outer_text)) if params else []
        if not params or not args:
            return member_text, False
        out = member_text
        for param, arg in zip(params, args):
            out = re.sub(rf"\b{re.escape(param)}\b", arg, out)
        return out, out != member_text

    def expression_type(self, text: str, ctx) -> tuple:
        if text.startswith(OF):
            return self.chain_type(parse_chain(text[len(OF):]), ctx)
        body = text[len(CALLED):]
        steps = parse_chain(body)
        if len(steps) > 1:
            return self.chain_type(steps[:-1] + (CALLED + steps[-1],), ctx)
        key = self.ix.gd_class(body, ctx.owner) if ctx.lang == "gd" else self.ix.cpp_class(body, ctx.owner, ctx.file)
        if key:
            return CLASS + key, ctx
        targets = [t for t in self.rs.function_targets(ctx, body) if self.ix.units[t].kind != "lambda"]
        if targets:
            target = self.ix.units[targets[0]]
            return target.type_text or None, target
        # Not the program's: an engine type constructed (Godot names types in PascalCase, functions in snake_case)
        # is that type; what an engine function returns is not declared here.
        return (body, ctx) if ctx.lang == "gd" and body[:1].isupper() else (None, None)

    def head_type(self, head: str, unit) -> tuple:
        if head in ("self", "this"):
            key = self.class_of(unit)
            return (CLASS + key, unit) if key else (None, None)
        if head == "super":
            key = self.class_of(unit)
            bases = self.ix.base_keys(self.ix.classes[key]) if key else []
            return (CLASS + bases[0], unit) if bases else (None, None)
        if head in (UNKNOWN, ITEM):
            return None, None
        if head.startswith(CALLED):
            return self.expression_type(head, unit)
        if head.startswith(TYPED):
            return head[len(TYPED):], unit
        if head in unit.types:
            return unit.types[head] or None, unit
        state = self.rs.state_targets(unit, head)
        if state:
            target = self.ix.units[state[0]]
            return target.type_text or None, target
        key = self.ix.gd_class(head, unit.owner) if unit.lang == "gd" else self.ix.cpp_class(head, unit.owner, unit.file)
        return (CLASS + key, unit) if key else (None, None)

    def member_step(self, key: str, text: str, ctx, step: str) -> tuple:
        """One step into a class of the program: a member's type, or an operator's result."""
        name = OPERATORS.get(step) or (step[len(CALLED):] if step.startswith(CALLED) else step)
        found = self.ix.lookup(key, name)
        if not found:
            inner = self.ix.inner[key].get(name)
            if inner:
                return CLASS + inner, ctx
            # a raw pointer's `->` and an operator the class does not define leave the class as it is
            return (text, ctx) if step in (DEREF, ITEM, ITER) else (ENGINE, ctx)
        target = self.ix.units[self.ix.definitions(found)[0]]
        result, replaced = self.substitute(key, text, target.type_text or "")
        if step == ITER and result:
            result = self.library_step(DEREF, result) if template_args(clean(result)) else result
        return (result or None), (ctx if replaced else target)

    def chain_type(self, recv: tuple, unit) -> tuple:
        """The type text (and the unit it is read in) of a receiver chain."""
        if not recv:
            return None, None
        text, ctx = self.settle(*self.head_type(recv[0], unit))
        for step in recv[1:]:
            if text is None:
                return None, None
            key = self.type_class(text, ctx)
            if key is not None and text.startswith(CLASS) and step in (DEREF, ITEM, ITER):
                continue
            if key is not None:
                text, ctx = self.settle(*self.member_step(key, text, ctx, step))
            elif text == ENGINE:
                continue
            else:
                text = self.library_step(step, text)
        return text, ctx

    def receiver(self, recv: tuple, unit) -> tuple:
        """(project class key or None, the declared type text when it is no class of the program, else None)."""
        text, ctx = self.settle(*self.chain_type(recv, unit))
        key = self.type_class(text, ctx)
        if key:
            return key, None
        return None, (text if declared(text) else None)

    def receiver_class(self, recv: tuple, unit) -> str | None:
        return self.receiver(recv, unit)[0]
