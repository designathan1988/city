"""JavaScript / TypeScript reader: the definitions a module makes (log/trace-rules.md, JavaScript and TypeScript rows).

A module is a class of its own (key: the file) whose members are its top-level names, with a `<module>` function
holding its top-level code (run when the module is first imported, J3). A class (key `<file>#<Name>`) has its methods,
fields, getters and setters (accessor units of a field, J5), a `constructor` (the default one when none is written:
it calls the base's, J4) that runs the instance field initializers, and a `<static>` unit for static fields, static
blocks and decorators. A function written in a function is a lambda that sees the enclosing locals (closures, J7).
An object literal is a class of its own (J9). A function or class takes the name of the variable or property it is
written to (J9). Module-level statements are read by `js_decl`, statements by `js_body`, expressions by `js_expr`.
"""
from __future__ import annotations

from . import ir_js, js_body, js_class, js_decl, js_types, scopes
from .model import ClassFact, FileFacts, Unit, add_ref, declare, enclose
from .rules import CONSTRUCTOR
from .source import Source, field_of, grammar_of, named_children

FUNCTION_NODES = ir_js.FUNCTIONS

STATIC_UNIT, MODULE_UNIT = "<static>", "<module>"


class JsReader:
    def __init__(self, src: Source):
        self.src = src
        self.key = src.display
        self.facts = FileFacts(src.display, "js")
        self._lambdas = {}               # function node start byte -> local id
        self._objects = {}               # object / class expression start byte -> class key
        self.declaration_file = src.display.endswith((".d.ts", ".d.mts", ".d.cts"))
        self.scopes = []                 # the blocks of the function being read (`scopes`)
        self.base = 0                    # the index of the function's own scope among them
        self.type_params = []            # the type parameters of the generic declarations being read, innermost last

    def run(self) -> FileFacts:
        root = self.src.parse(grammar_of(self.src.path))
        self.facts.classes.append(ClassFact(self.key, "js", "", self.key, 1))
        self.module = self.unit("func", MODULE_UNIT, self.key, root, qual=f"{self.key}:{MODULE_UNIT}")
        js_decl.module_body(self, root, self.key, self.module)
        self.facts.script = not self.facts.imports and not self.facts.exports and not self.facts.star_exports \
            and not js_decl.has_module_syntax(self, root)
        self.facts.errors = int(root.has_error)
        return self.facts

    # ---- units -------------------------------------------------------------------------------------------------
    def unit(self, kind: str, name: str, owner: str, node, qual: str | None = None, **extra) -> Unit:
        unit = Unit(kind, "js", name, qual or self.qual(owner, name), owner, self.src.display, self.src.line(node),
                    self.src.end_line(node), local_id=len(self.facts.units), **extra)
        self.facts.units.append(unit)
        return unit

    def qual(self, owner: str, name: str) -> str:
        return f"{owner}.{name}" if "#" in owner else f"{owner}:{name}"

    def add(self, unit: Unit, kind: str, name: str, node, cond, **extra) -> None:
        add_ref(self, unit, kind, name, self.src.line(node), cond, **extra)

    def ir(self, node, unit: Unit) -> tuple:
        return scopes.scoped_tree(self, ir_js.build(self, node, unit))

    # ---- functions ---------------------------------------------------------------------------------------------
    def function(self, node, name: str, owner: str, outer: Unit | None, kind: str = "func") -> Unit:
        """A function, method or arrow named `name`; written inside `outer` (a function) it sees its locals."""
        if node.start_byte in self._lambdas:
            return self.facts.units[self._lambdas[node.start_byte]]
        unit = self.unit(kind, name, owner, node)
        unit.generator = any(c.type == "*" for c in node.children)     # `function*`, `*name()`, `async *name()` (J5)
        self._lambdas[node.start_byte] = unit.local_id
        nested = outer is not None and outer.name != MODULE_UNIT
        if nested:
            enclose(unit, outer)
        base = self.base
        saved = self.scopes if nested else scopes.reset(self)   # a nested function sees the enclosing blocks
        if nested:
            scopes.open_scope(self)
            self.base = len(self.scopes) - 1                    # its own scope: where its `var`s go
        self.type_params.append(js_types.parameters(self, node))
        try:
            self.function_body(node, unit)
        finally:
            self.type_params.pop()
            if nested:
                scopes.close_scope(self)
            self.scopes, self.base = saved, base
        return unit

    def function_body(self, node, unit: Unit) -> None:
        ret = field_of(node, "return_type")
        unit.type_text = self.type_text(ret) if ret is not None else ""
        unit.declared = js_types.values(self, ret, unit) if ret is not None else []      # T3
        self.params(field_of(node, "parameters") or field_of(node, "parameter"), unit)
        body = field_of(node, "body")
        if body is None:
            unit.decl_only = True
        elif body.type == "statement_block":
            js_body.block(self, body, unit, ())
        else:                                                   # an arrow's expression body is its return value
            unit.returns.append((self.src.line(body), js_body.squash(self.src.text(body)), self.ir(body, unit)))
            js_body.expr(self, body, unit, ())

    def lambda_id(self, node, unit: Unit) -> int:
        """A function written as a value inside `unit`: a lambda (named by its own name when it has one)."""
        if node.start_byte in self._lambdas:
            return self._lambdas[node.start_byte]
        own = field_of(node, "name")
        line = self.src.line(node)
        label = self.src.text(own) if own is not None else f"{unit.name}.<lambda:{line}>"
        return self.function(node, label, unit.owner, unit, kind="lambda").local_id

    def params(self, params, unit: Unit) -> None:
        unit.required = 0
        if params is None:
            return
        items = [params] if params.type == "identifier" else named_children(params)   # `x => ...`
        for i, p in enumerate(i for i in items if i.type != "comment"):
            pattern = field_of(p, "pattern") if p.type in ("required_parameter", "optional_parameter") else p
            if pattern is None or pattern.type == "this":
                continue
            default = field_of(p, "value")
            if p.type == "required_parameter" and default is None and pattern.type != "rest_pattern":
                unit.required += 1
            if pattern.type == "rest_pattern":
                unit.variadic = True
            slot = self.src.text(pattern) if pattern.type == "identifier" else f"<arg{i}>"
            unit.params.append(slot)
            declared = self.type_values(field_of(p, "type"), unit)
            declare(self, unit, slot, declared + ([self.ir(default, unit)] if default is not None else []), param=True)
            if pattern.type != "identifier":
                self.bind(pattern, ("name", slot), unit, None, ())
            if default is not None:
                js_body.expr(self, default, unit, ())

    # ---- patterns (J1): each name bound to its part of the value --------------------------------------------------
    def bind(self, pattern, tree: tuple, unit: Unit, write_cond, cond) -> None:
        """Destructuring: declare (write_cond None) or assign each name of `pattern` the part of `tree` it takes."""
        kind = pattern.type
        if kind in ("identifier", "shorthand_property_identifier_pattern"):
            text = self.src.text(pattern)
            if write_cond is None:
                declare(self, unit, text, [tree])
            else:
                js_body.write_name(self, pattern, text, tree, unit, cond)
        elif kind == "object_pattern":
            for part in named_children(pattern):
                if part.type == "shorthand_property_identifier_pattern":
                    self.bind(part, ("member", tree, self.src.text(part), False), unit, write_cond, cond)
                elif part.type == "pair_pattern":
                    key = ir_js.member_name(self.src, field_of(part, "key"))
                    self.bind(field_of(part, "value"), ("member", tree, key, False), unit, write_cond, cond)
                elif part.type == "object_assignment_pattern":
                    left, right = field_of(part, "left"), field_of(part, "right")
                    taken = ("member", tree, self.src.text(left), False)
                    self.bind(left, ("union", taken, self.ir(right, unit)), unit, write_cond, cond)
                elif part.type == "rest_pattern":
                    self.bind(named_children(part)[0], tree, unit, write_cond, cond)
        elif kind == "array_pattern":
            for part in named_children(pattern):
                self.bind(part, ("elements", tree), unit, write_cond, cond)
        elif kind == "assignment_pattern":
            left, right = field_of(pattern, "left"), field_of(pattern, "right")
            self.bind(left, ("union", tree, self.ir(right, unit)), unit, write_cond, cond)
            js_body.expr(self, right, unit, cond)
        elif kind == "rest_pattern":
            self.bind(named_children(pattern)[0], tree, unit, write_cond, cond)
        elif write_cond is not None:                            # a member or element written through a pattern
            js_body.target(self, pattern, unit, cond, tree)

    # ---- types (J8, T1-T3): a declared type names the values it allows; the text is kept for display only ---------
    def type_text(self, annotation) -> str:
        node = annotation
        if node is not None and node.type in ("type_annotation", "opting_type_annotation", "omitting_type_annotation",
                                               "adding_type_annotation"):
            node = (named_children(node) or [None])[0]
        return " ".join(self.src.text(node).split()) if node is not None else ""

    def type_values(self, annotation, unit: Unit) -> list[tuple]:
        """A type annotation as the value trees it allows (`js_types`)."""
        return js_types.values(self, annotation, unit) if annotation is not None else []

    def type_scope(self) -> set:
        """The type parameters visible where the reader is (every enclosing generic declaration's)."""
        return set().union(*self.type_params) if self.type_params else set()

    # ---- classes and objects ----------------------------------------------------------------------------------
    def class_value(self, node, unit: Unit) -> str:
        if node.start_byte not in self._objects:
            own = field_of(node, "name")
            name = self.src.text(own) if own is not None else f"<class:{self.src.line(node)}>"
            self._objects[node.start_byte] = js_class.class_declaration(self, node, name, unit.owner, unit)
        return self._objects[node.start_byte]

    def object_class(self, node, unit: Unit) -> str:
        if node.start_byte not in self._objects:
            self._objects[node.start_byte] = js_class.object_literal(self, node, unit)
        return self._objects[node.start_byte]

    def constructor_of(self, key: str, node) -> Unit:
        """The class's constructor unit (the default one when the class writes none: it calls the base's, J4)."""
        for unit in self.facts.units:
            if unit.owner == key and unit.name == CONSTRUCTOR:
                return unit
        unit = self.unit("func", CONSTRUCTOR, key, node)
        unit.variadic, unit.required = True, 0
        self.add(unit, "call", CONSTRUCTOR, node, (), base=("super",), text=js_body.IMPLICIT)
        return unit

    def static_of(self, key: str, node) -> Unit:
        for unit in self.facts.units:
            if unit.owner == key and unit.name == STATIC_UNIT:
                return unit
        return self.unit("func", STATIC_UNIT, key, node)


def read_js(src: Source) -> FileFacts:
    return JsReader(src).run()
