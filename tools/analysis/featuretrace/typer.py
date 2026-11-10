"""VALUE, LOOK and FLOW (log/trace-rules.md): the types an expression tree can hold.

`evaluate(tree, unit)` gives every (type text, unit it is read in) the tree can stand for, one rule per tree shape (the
EXPR rows). A name is looked up in its language's order (local, member with bases, outer class or namespaces, global);
a name with no declared type holds every value that flows into it (initializer, assignments, the arguments at each call
site, the values a function returns, the values put into a container). A name no code of the program defines is the
engine's or the library's. A class template's parameters are replaced by the arguments written; `[]`, `->`, `begin`
and operators on a class of the program go through its operator; a library type answers by the library's rules.

Type texts: a type as written, CLASS + key (a class of the program), FN + gid (a callable of a function), ELEM + type
(a value put into a container: what its elements hold), ENGINE (a value of the engine or the language).
"""
from __future__ import annotations

import re

from .cpp_names import template_args
from .library import DEREF, ENGINE, ITEM, ITER, clean, library_step
from .model import FUNCTION_KINDS
from .rules import NAME_CONSTRUCTORS

CLASS, FN, ELEM = "#", "<fn>", "<elem>"
CALLABLE_CALLS = ("call", "callv", "call_deferred")
OPERATORS = {ITEM: "operator[]", DEREF: "operator->", ITER: "begin"}
COMPARISONS = ("==", "!=", "<", ">", "<=", ">=", "<=>", "&&", "||", "!", "and", "or", "not", "in", "is")
NO_VALUE = ("void", "Variant", "auto", "decltype(auto)", "")
DEPTH = 24


class Typer:
    def __init__(self, index, resolver):
        self.ix = index
        self.rs = resolver
        self._busy = set()
        self._memo = {}

    def class_of(self, unit) -> str | None:
        return unit.owner if unit.owner in self.ix.classes else None

    # ---- trees -------------------------------------------------------------------------------------------------
    def evaluate(self, tree, unit, depth: int = 0) -> list[tuple]:
        if not tree or unit is None or depth > DEPTH:
            return []
        key = (tree, unit.gid)
        if key in self._memo:
            return self._memo[key]
        if key in self._busy:
            return []
        self._busy.add(key)
        try:
            found = self._evaluate(tree, unit, depth + 1)
        finally:
            self._busy.discard(key)
        out, seen = [], set()
        for text, ctx in found:
            if text and (text, ctx.gid) not in seen:
                seen.add((text, ctx.gid))
                out.append((text, ctx))
        self._memo[key] = out
        return out

    def _evaluate(self, tree, unit, depth: int) -> list[tuple]:
        kind = tree[0]
        if kind == "name":
            return self.named(tree[1], unit, depth)
        if kind == "member":
            base = self.evaluate(tree[1], unit, depth)
            if tree[3]:
                base = [v for t, c in base for v in self.step(DEREF, t, c, depth)]
            return [v for t, c in base for v in self.member(t, c, tree[2], False, depth)]
        if kind == "call":
            return self.call(tree[1], tree[2], unit, depth)
        if kind in ("index", "elements", "deref"):
            step = {"index": ITEM, "elements": ITER, "deref": DEREF}[kind]
            return [v for t, c in self.evaluate(tree[1], unit, depth) for v in self.step(step, t, c, depth)]
        if kind == "op":
            return [v for t, c in self.evaluate(tree[2], unit, depth) for v in self.operator(tree[1], t, c, depth)]
        if kind == "unary":
            return [v for t, c in self.evaluate(tree[2], unit, depth) for v in self.operator(tree[1], t, c, depth)]
        if kind in ("cast", "new"):
            return [(tree[1], unit)] if tree[1] else [(ENGINE, unit)]
        if kind == "callable":
            return [(FN + str(self.ix.offset[unit.file] + tree[1]), unit)]
        if kind == "union":
            return self.evaluate(tree[1], unit, depth) + self.evaluate(tree[2], unit, depth)
        if kind == "collection":
            return [(ENGINE, unit)] + [(ELEM + t, c) for v in tree[1] for t, c in self.evaluate(v, unit, depth)
                                       if not t.startswith(ELEM)]
        if kind == "put":
            return [(ELEM + t, c) for t, c in self.evaluate(tree[1], unit, depth) if not t.startswith(ELEM)]
        if kind in ("await", "addr"):
            return self.evaluate(tree[1], unit, depth)
        if kind == "self":
            key = self.class_of(unit)
            return [(CLASS + key, unit)] if key else []
        if kind == "super":
            key = self.class_of(unit)
            bases = self.ix.base_keys(self.ix.classes[key]) if key else []
            return [(CLASS + bases[0], unit)] if bases else [(ENGINE, unit)]
        return [(ENGINE, unit)]      # literal, strname, node: the engine's or the language's values

    def call(self, callee, args, unit, depth: int) -> list[tuple]:
        made = self.made_by_name(callee, args, unit)
        if made is not None:
            return made
        if callee[0] == "member":
            base = self.evaluate(callee[1], unit, depth)
            if callee[3]:
                base = [v for t, c in base for v in self.step(DEREF, t, c, depth)]
            return [v for t, c in base for v in self.member(t, c, callee[2], True, depth)]
        if callee[0] == "name":
            name = callee[1]
            key = self.ix.gd_class(name, unit.owner) if unit.lang == "gd" else self.ix.cpp_class(name, unit.owner,
                                                                                                    unit.file)
            if key:
                return [(CLASS + key, unit)]            # a class called constructs it
            if unit.lang == "gd" and name[:1].isupper() and not self.rs.function_targets(unit, name):
                return [(name, unit)]                   # an engine type constructed (Godot names types in PascalCase)
        out = []
        for text, ctx in self.evaluate(callee, unit, depth):
            if text.startswith(FN):
                out += self.returns(self.ix.units[int(text[len(FN):])], depth)
            elif not text.startswith(ELEM):
                out.append((ENGINE, ctx))       # calling what the engine gives: the engine's value
        return out

    def made_by_name(self, callee, args, unit) -> list[tuple] | None:
        """The engine's constructors by name (NAME, G6): `ClassDB.instantiate(&"C")` is an object of class C."""
        if callee[0] != "member" or callee[1][0] != "name":
            return None
        index = NAME_CONSTRUCTORS.get((callee[1][1], callee[2]))
        if index is None or self.named(callee[1][1], unit, 0) != [(ENGINE, unit)] or index >= len(args):
            return None
        if args[index][0] != "strname":
            return [(ENGINE, unit)]
        key = self.ix.engine_class(args[index][1]) if unit.lang == "gd" else None
        return [(CLASS + key, unit)] if key else [(args[index][1], unit)]

    # ---- names -------------------------------------------------------------------------------------------------
    def named(self, name: str, unit, depth: int) -> list[tuple]:
        """LOOK: local -> member (with bases) -> outer class / namespaces -> global; else the engine's."""
        if name in unit.lambdas:
            return [(FN + str(self.ix.offset[unit.file] + unit.lambdas[name]), unit)]
        if name in unit.types:
            return self.local(name, unit, depth)
        targets = self.rs.state_targets(unit, name)
        if targets:
            return [v for t in targets for v in self.held(self.ix.units[t], depth)]
        key = self.ix.gd_class(name, unit.owner) if unit.lang == "gd" else self.ix.cpp_class(name, unit.owner,
                                                                                                unit.file)
        if key:
            return [(CLASS + key, unit)]
        functions = self.rs.function_targets(unit, name)
        if functions:
            return [(FN + str(f), unit) for f in functions]
        return [(ENGINE, unit)]

    def local(self, name: str, unit, depth: int) -> list[tuple]:
        """A local or parameter: its declared type, else every value that flows into it (for a parameter, the
        argument written at every site that calls the function)."""
        trees = unit.types.get(name, [])
        found = [v for t in trees for v in self.evaluate(t, unit, depth)]
        if name in unit.params and not any(t[0] == "cast" for t in trees):
            found += self.rs.argument_values(unit, unit.params.index(name), depth)
        return found

    def held(self, unit, depth: int) -> list[tuple]:
        """What a variable or field holds: its declared type, else every value that flows into it."""
        if unit.kind in FUNCTION_KINDS:
            return [(FN + str(unit.gid), unit)]
        if unit.type_text and unit.type_text not in NO_VALUE:
            return [(unit.type_text, unit)]
        found = [v for t in unit.values for v in self.evaluate(t, unit, depth)]
        return found + self.rs.assigned_values(unit, depth)

    def returns(self, unit, depth: int) -> list[tuple]:
        """What a call of the function yields: its declared return type, else what its `return`s carry."""
        if unit.type_text == "void":
            return []
        if unit.type_text and unit.type_text not in NO_VALUE:
            return [(unit.type_text, unit)]
        return [v for _, _, tree in unit.returns for v in self.evaluate(tree, unit, depth)]

    # ---- types -------------------------------------------------------------------------------------------------
    def type_class(self, text: str, ctx) -> str | None:
        if not text or text == ENGINE or text.startswith((FN, ELEM)):
            return None
        if text.startswith(CLASS):
            return text[1:]
        if ctx.lang == "gd":
            return self.ix.gd_class(text, ctx.owner)
        return self.ix.cpp_class(clean(text), ctx.owner, ctx.file)

    def member(self, text: str, ctx, name: str, called: bool, depth: int) -> list[tuple]:
        """A member of a value: its field's value, its method's callable, or (called) the method's result."""
        if text.startswith(FN):
            target = self.ix.units[int(text[len(FN):])]
            return self.returns(target, depth) if called and name in CALLABLE_CALLS else [(text, ctx)] \
                if name in ("bind", "unbind", "bindv") else [(ENGINE, ctx)]
        if text.startswith(ELEM):
            return []
        key = self.type_class(text, ctx)
        if called and name == "new":
            return [(CLASS + key, ctx)] if key else [(text, ctx)]     # `T.new()` constructs a T (EXPR: new)
        if key is None:
            return [(library_step(name, text, called), ctx)] if text != ENGINE else [(ENGINE, ctx)]
        found = self.ix.lookup(key, name)
        if ctx.lang == "gd" and self.ix.classes[key].lang == "cpp":
            found = self.rs.engine_members(key, name, "func" if called else "read")
        if not found:
            inner = self.ix.inner[key].get(name)
            return [(CLASS + inner, ctx)] if inner else [(ENGINE, ctx)]   # an engine or library base's member
        out = []
        for gid in self.ix.definitions(found):
            target = self.ix.units[gid]
            values = self.returns(target, depth) if called else self.held(target, depth)
            for value, where in values:
                value, replaced = self.substitute(key, text, value)
                out.append((value, ctx if replaced else where))
        return out

    def step(self, step: str, text: str, ctx, depth: int) -> list[tuple]:
        """`[]`, a range-for, `->`: an element of a container, what a pointer points at."""
        if text.startswith(ELEM):
            return [(text[len(ELEM):], ctx)] if step in (ITEM, ITER) else []
        if text == ENGINE or text.startswith(FN):
            return [(ENGINE, ctx)]
        key = self.type_class(text, ctx)
        if key is None:
            return [(library_step(step, text, False), ctx)]
        found = self.ix.lookup(key, OPERATORS[step])
        if not found:
            return [(text, ctx)]           # a raw pointer's `->`, `*` on a pointer: the object itself
        out = []
        for gid in self.ix.definitions(found):
            for value, where in self.returns(self.ix.units[gid], depth):
                value, replaced = self.substitute(key, text, value)
                if step == ITER:
                    value = library_step(DEREF, value, False) if template_args(clean(value)) else value
                out.append((value, ctx if replaced else where))
        return out

    def operator(self, op: str, text: str, ctx, depth: int) -> list[tuple]:
        if op in COMPARISONS:
            return [(ENGINE, ctx)]
        if op == "*":
            return self.step(DEREF, text, ctx, depth)
        key = self.type_class(text, ctx)
        if key and ctx.lang == "cpp":
            found = self.ix.lookup(key, "operator" + op)
            if found:
                return self.returns(self.ix.units[self.ix.definitions(found)[0]], depth)
        return [(text, ctx)]

    def substitute(self, key: str, outer: str, member: str) -> tuple[str, bool]:
        params = self.ix.classes[key].params
        args = template_args(clean(outer)) if params and not outer.startswith(CLASS) else []
        if not params or not args or member.startswith((CLASS, FN, ELEM, ENGINE)):
            return member, False
        out = member
        for param, arg in zip(params, args):
            out = re.sub(rf"\b{re.escape(param)}\b", arg, out)
        return out, out != member

    def receiver(self, tree, unit) -> tuple[list[str], bool, list[int]]:
        """(classes of the program the receiver can be, whether it can be an engine value, callables it can be)."""
        keys, engine, functions = [], False, []
        for text, ctx in self.evaluate(tree, unit):
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
