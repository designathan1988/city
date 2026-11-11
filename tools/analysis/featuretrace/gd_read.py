"""GDScript reader: the definitions a script makes and every name used inside each of them.

Definitions are recognised by the language's own markers (`class_name`, `extends`, `func`, `var`, `const`, `signal`,
`enum`, `class`, parameters, `for` bindings, lambdas). Bodies are read by `gd_body`. Every value is kept as an
expression tree built by `ir_gd` (log/trace-rules.md: VALUE, FLOW).
"""
from __future__ import annotations

from . import gd_body, ir_gd, narrowing, scopes
from .model import ClassFact, FileFacts, Unit, add_ref, declare, enclose
from .source import Source, field_of, named_children, string_value


class GdReader:
    def __init__(self, src: Source, res_path: str):
        self.src = src
        self.res = res_path
        self.facts = FileFacts(src.display, "gd")
        self._lambdas = {}           # lambda node start byte -> local id (each lambda one unit)
        self.scopes = []             # the blocks of the function being read (`scopes`)

    # ---- definitions -------------------------------------------------------------------------------------------
    def run(self) -> FileFacts:
        root = self.src.parse("gd")
        cls = ClassFact(self.res, "gd", "", self.src.display, 1)
        self.facts.classes.append(cls)
        self.class_body(root, cls)
        self.facts.errors = int(root.has_error)
        return self.facts

    def unit(self, kind, name, cls_key, node, **extra) -> Unit:
        unit = Unit(kind, "gd", name, f"{cls_key}.{name}", cls_key, self.src.display, self.src.line(node),
                    self.src.end_line(node), local_id=len(self.facts.units), **extra)
        self.facts.units.append(unit)
        return unit

    def class_body(self, body, cls: ClassFact) -> None:
        for st in named_children(body):
            kind = st.type
            if kind == "class_name_statement":
                cls.name = self.src.text(field_of(st, "name"))
                for sub in named_children(st):
                    if sub.type == "extends_statement":
                        cls.bases.append(self.extends_spec(sub))
            elif kind == "extends_statement":
                cls.bases.append(self.extends_spec(st))
            elif kind in ("function_definition", "constructor_definition"):
                self.function(st, cls)
            elif kind in ("variable_statement", "const_statement", "onready_variable_statement",
                          "export_variable_statement"):
                self.member(st, cls)
            elif kind == "signal_statement":
                self.unit("signal", self.src.text(field_of(st, "name")), cls.key, st)
            elif kind == "enum_definition":
                self.enum(st, cls)
            elif kind == "class_definition":
                name = self.src.text(field_of(st, "name"))
                inner = ClassFact(f"{cls.key}.{name}", "gd", name, self.src.display, self.src.line(st), outer=cls.key)
                ext = field_of(st, "extends")
                if ext is not None:
                    inner.bases.append(self.extends_spec(ext))
                self.facts.classes.append(inner)
                self.unit("class", name, cls.key, st)
                if field_of(st, "body") is not None:
                    self.class_body(field_of(st, "body"), inner)

    def extends_spec(self, node) -> str:
        """`extends Name`, `extends "file.gd"` or `extends "file.gd".Inner` (GDScript reference, Inheritance)."""
        for sub in named_children(node):
            target = named_children(sub)[0] if sub.type == "type" and named_children(sub) else sub
            parts = named_children(target) if target.type == "attribute" else [target]
            value = string_value(self.src, parts[0])
            if value is not None:
                return "path:" + value + "".join("|" + self.src.text(p) for p in parts[1:])
            return self.src.text(sub)
        return ""

    def enum(self, node, cls: ClassFact) -> None:
        name_node = field_of(node, "name")
        owner = cls.key
        if name_node is not None:
            name = self.src.text(name_node)
            self.unit("enum", name, cls.key, node)
            owner = f"{cls.key}.{name}"
            self.facts.classes.append(ClassFact(owner, "gd", name, self.src.display, self.src.line(node), outer=cls.key))
        body = field_of(node, "body")
        for item in named_children(body) if body is not None else []:
            left = field_of(item, "left") or (named_children(item) or [None])[0]
            if left is not None:
                self.unit("enumerator", self.src.text(left), owner, item)

    def member(self, node, cls: ClassFact) -> None:
        kind = "const" if node.type == "const_statement" else "var"
        unit = self.unit(kind, self.src.text(field_of(node, "name")), cls.key, node)
        unit.static = any(c.type == "static_keyword" for c in node.children)
        typ = field_of(node, "type")
        unit.type_text = self.src.text(typ) if typ is not None and typ.type == "type" else ""
        value = field_of(node, "value")
        if value is not None:
            unit.values.append(self.ir(value, unit, cls))
            self.expr(value, unit, cls, ())
        setget = field_of(node, "setget")
        if setget is not None:
            self.accessors(setget, unit, cls)

    def accessors(self, setget, prop: Unit, cls: ClassFact) -> None:
        """A property's `get:`/`set(value):` bodies, or `get = f`/`set = g` (G8): run on every read and write."""
        for i, part in enumerate(setget.children):
            which = setget.field_name_for_child(i)
            if which not in ("get", "set"):
                continue
            if part.type in ("getter", "setter"):
                prop.accessors[which] = self.src.text(part)
                continue
            body = Unit("func", "gd", f"{prop.name}.{which}", f"{prop.qual}.{which}", cls.key, self.src.display,
                        self.src.line(part), self.src.end_line(part), local_id=len(self.facts.units),
                        accessor_of=prop.name)
            self.facts.units.append(body)
            prop.accessors[which] = body.local_id
            saved = scopes.reset(self)
            self.params(part, body, cls)
            if field_of(part, "body") is not None:
                self.block(field_of(part, "body"), body, cls, ())
            self.scopes = saved

    def params(self, node, unit: Unit, cls: ClassFact) -> None:
        # a function's `parameters` field; a setter's `set(value):` writes them as an unnamed child (G8)
        params = field_of(node, "parameters") or next((c for c in named_children(node) if c.type == "parameters"), None)
        unit.required = 0
        for p in named_children(params) if params is not None else []:
            if p.type not in ("default_parameter", "typed_default_parameter"):
                unit.required += 1
            ident = p if p.type == "identifier" else next((c for c in named_children(p) if c.type == "identifier"), None)
            if ident is None:
                continue
            name = self.src.text(ident)
            unit.params.append(name)
            typ, default = field_of(p, "type"), field_of(p, "value")
            if typ is not None and typ.type == "type":
                declare(self, unit, name, [("cast", self.src.text(typ))], param=True)
            else:
                declare(self, unit, name, [self.ir(default, unit, cls)] if default is not None else [], param=True)

    def function(self, node, cls: ClassFact) -> None:
        name = field_of(node, "name")
        # the constructor (`func _init()`): the grammar writes `_init` as a keyword, the language calls it `_init`
        unit = self.unit("func", self.src.text(name) if name is not None else "_init", cls.key, node)
        unit.static = any(c.type == "static_keyword" for c in node.children)
        ret = field_of(node, "return_type")
        unit.type_text = self.src.text(ret) if ret is not None else ""
        saved = scopes.reset(self)
        self.params(node, unit, cls)
        if field_of(node, "body") is not None:
            self.block(field_of(node, "body"), unit, cls, ())
        self.scopes = saved

    def lambda_id(self, node, owner: Unit, cls: ClassFact) -> int:
        """The lambda's unit (made once, whether met while building a tree or while walking the body)."""
        if node.start_byte in self._lambdas:
            return self._lambdas[node.start_byte]
        line = self.src.line(node)
        lam = Unit("lambda", "gd", f"{owner.name}.<lambda:{line}>", f"{owner.qual}.<lambda:{line}>", cls.key,
                   self.src.display, line, self.src.end_line(node), local_id=len(self.facts.units))
        self.facts.units.append(lam)
        self._lambdas[node.start_byte] = lam.local_id
        enclose(lam, owner)
        scopes.open_scope(self)                 # its parameters: a scope of its own inside the enclosing blocks
        self.params(node, lam, cls)
        if field_of(node, "body") is not None:
            self.block(field_of(node, "body"), lam, cls, ())
        scopes.close_scope(self)
        return lam.local_id

    # ---- bodies ------------------------------------------------------------------------------------------------
    def ir(self, node, unit: Unit, cls: ClassFact) -> tuple:
        """The expression tree of a node, its names read where the reader is (`scopes`: block scope, narrowing)."""
        return scopes.scoped_tree(self, ir_gd.build(self.src, node, lambda n: self.lambda_id(n, unit, cls)))

    def add(self, unit: Unit, kind: str, name: str, node, cond, **extra) -> None:
        add_ref(self, unit, kind, name, self.src.line(node), cond, **extra)

    def block(self, node, unit: Unit, cls: ClassFact, cond: tuple) -> None:
        """A block's statements; what one proves for those after it (an `if` that always leaves) holds over them."""
        scopes.open_scope(self)
        items, opened = named_children(node), 0
        for i, st in enumerate(items):
            after = gd_body.statement(self, st, unit, cls, cond)
            if after:
                narrowing.open(self, after, items[i + 1:], "gd")
                opened += 1
        for _ in range(opened):
            narrowing.close(self)
        scopes.close_scope(self)

    def expr(self, node, unit: Unit, cls: ClassFact, cond: tuple) -> None:
        gd_body.expr(self, node, unit, cls, cond)


def read_gd(src: Source, res_path: str) -> FileFacts:
    return GdReader(src, res_path).run()
