"""LOOK and FLOW (log/trace-rules.md): the value nodes of the places a value is kept in, and the names that read them.

A name read in a unit is the local, parameter, capture, member, variable, class or function it names (LOOK); a parameter
holds what every link into its function writes (`#arg`, `flows`) and the engine's values when the engine calls it; a
variable or field holds its declared type, its initializers and every value written to it (`#hold`, `#held`); a call
yields its function's result (`#ret`), made of parts: a template of the function reads only the parts its key reaches,
the others are the unit's (`templates`, C2'''). The typer (`typer.Typer`) is built on this.
"""
from __future__ import annotations

from . import templates, transfer
from .model import ENGINE_CALLBACKS, FUNCTION_KINDS
from .solver import CLASS, ENGINE, FN

NO_VALUE = ("void", "Variant", "auto", "decltype(auto)", "")
ARG, HELD, ENGINE_CALLED, HOLD, RET = "#arg", "#held", "#engine", "#hold", "#ret"


class Places:
    def wire_name(self, node, name: str, unit) -> None:
        """LOOK: local -> member (with bases) -> outer class / namespaces -> global; else the engine's."""
        if name in unit.lambdas:
            self.add(node, [(FN + str(self.ix.offset[unit.file] + unit.lambdas[name]), unit)])
            return
        if name in unit.types:
            self.wire_local(node, name, unit)
            return
        owner = self.enclosing(unit, name)
        if owner is not None:                     # declared by an enclosing function after the closure (J7)
            self.follow(self.node_of(("name", name), owner), node)
            return
        targets = self.rs.state_targets(unit, name)
        for gid in targets:
            self.follow(self.hold_node(self.ix.units[gid]), node)
        if unit.lang == "js":
            found = [(FN + str(f), unit) for f in self.rs.function_targets(unit, name)]
            key = self.ix.js.class_of(name, unit.file, unit.owner)
            found += [(CLASS + key, unit)] if key else []
            self.add(node, found or ([] if targets else [(ENGINE, unit)]))
            return
        if targets:
            return
        key = self.ix.gd_class(name, unit.owner) if unit.lang == "gd" else self.ix.cpp_class(name, unit.owner,
                                                                                                unit.file)
        functions = self.rs.function_targets(unit, name) if not key else []
        self.add(node, [(CLASS + key, unit)] if key else [(FN + str(f), unit) for f in functions] or [(ENGINE, unit)])

    def wire_local(self, node, name: str, unit) -> None:
        """A local or parameter: its declared type, every value written to it, for a parameter the argument of every
        link into the function (and the engine's when the engine calls it), for a capture the enclosing local. A
        template's key parameter holds its key value alone, its other key parameters what its calls write and their
        defaults: never a declared type, never the engine's calls, which are the unit's (`templates`, C2', C2'')."""
        param = unit.params.index(name) if name in unit.params else -1
        if param >= 0 and param == unit.fixed:
            self.follow(self.arg_node(unit, param), node)
            return
        bound = param >= 0 and unit.key != unit.gid               # a template's parameter
        for tree in unit.types.get(name, []):
            if not (bound and tree[0] == "cast"):
                self.follow(self.node_of(tree, unit), node)
        if param >= 0:
            self.follow(self.arg_node(unit, param), node)
        if param >= 0 and not bound:
            self.follow((ENGINE_CALLED, unit.key), node)
            if unit.name in ENGINE_CALLBACKS or unit.bound_as:
                self.add(node, [(ENGINE, unit)])
        if name in unit.captured and unit.outer >= 0:
            outer = self.ix.units[self.ix.offset[unit.file] + unit.outer]
            self.follow(self.node_of(("name", name), outer), node)
        for writer in unit.written_by.get(name, ()):      # what closures sharing it write, read where written
            self.follow(self.node_of(("name", name), self.ix.units[self.ix.offset[unit.file] + writer]), node)

    def arg_node(self, unit, index: int):
        """What the calls write in a parameter: of the unit (every call's), or of one of its templates."""
        return (ARG, unit.key, index)

    def named(self, name: str, unit, depth: int = 0) -> list[tuple]:
        return self.evaluate(("name", name), unit)

    def hold_node(self, unit):
        """What a variable or field holds: its declared type, its initializers, every value written to it."""
        node = (HOLD, unit.gid)
        if node not in self.wired:
            self.wired.add(node)
            self.value.setdefault(node, [])
            if unit.kind in FUNCTION_KINDS:
                self.add(node, [(FN + str(unit.gid), unit)])
                return node
            if unit.type_text and unit.type_text not in NO_VALUE and unit.lang != "js":
                self.add(node, [(unit.type_text, unit)])      # a JS annotation's values are trees in `values` (T3)
            for tree in unit.values:
                self.follow(self.node_of(tree, unit), node)
            self.follow((HELD, unit.gid), node)
        return node

    def ret_node(self, unit):
        """What a call of the function (or of one of its templates) yields: every part of its result."""
        node = (RET, unit.key)
        if node not in self.wired:
            self.wired.add(node)
            self.value.setdefault(node, [])
            for part in self.ret_parts(unit):
                self.follow(part, node)
        return node

    def ret_parts(self, unit) -> list:
        """The nodes a call's result is made of: its declared return type, a JS return annotation's values (T3), the
        generator object of a generator (its iteration gives the yields, J5), else each returned tree, a union opened.
        A template's part is its own node only where its key reaches the tree, else the unit's: delivered once for
        all its templates (C2''')."""
        if unit.type_text == "void":
            return []
        parts = [self.node_of(("cast", unit.type_text), unit)] \
            if unit.type_text and unit.type_text not in NO_VALUE and unit.lang != "js" else []
        parts += [self.node_of(tree, unit) for tree in unit.declared]
        if unit.generator:
            parts += [self.elements_node(self.node_of(leaf, unit)) for tree in unit.yields
                      for leaf in templates.leaves(tree)]
        else:
            parts += [self.node_of(leaf, unit) for _, _, tree in unit.returns for leaf in templates.leaves(tree)]
        return list(dict.fromkeys(parts))

    def elements_node(self, source):
        """A container whose elements hold what `source` holds."""
        node = ("#elements", source)
        if node not in self.wired:
            self.wired.add(node)
            self.value.setdefault(node, [])
            self.subscribe(source, node, ("elem",), transfer.elements(self))
        return node
