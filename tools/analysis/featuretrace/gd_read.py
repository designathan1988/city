"""GDScript reader: the definitions a script makes and every name used inside each of them.

Definitions are recognised by the language's own markers (`class_name`, `extends`, `func`, `var`, `const`, `signal`,
`enum`, `class`, parameters, `for` bindings, lambdas). Inside a body every construct is read by its rule: a call, a
member access (`a.b.c()`), an assignment, a branch (`if`/`elif`/`else`, `match`, `for`, `while`, with its condition kept),
a `return`. The engine's calls by name are kept as references too: `emit`, `connect`, `emit_signal`, `call*`,
`Callable(obj, "name")`, `preload`/`load`, `X.new()`, and every `&"name"` handed to a call (a StringName is how GDScript
writes a name as a value).
"""
from __future__ import annotations

from . import gd_body
from .model import ClassFact, FileFacts, Ref, Unit
from .source import Source, field_of, is_identifier, named_children, string_value


class GdReader:
    def __init__(self, src: Source, res_path: str):
        self.src = src
        self.res = res_path
        self.facts = FileFacts(src.display, "gd")
        self.narrowed = {}           # names an enclosing `is` test proves the type of
        self.assigned = {}           # (class, member) -> the type of what a body assigns to it

    # ---- definitions -------------------------------------------------------------------------------------------
    def run(self) -> FileFacts:
        root = self.src.parse("gd")
        cls = ClassFact(self.res, "gd", "", self.src.display, 1)
        self.facts.classes.append(cls)
        self.class_body(root, cls)
        # an untyped member holds what is assigned to it (`_core = ClassDB.instantiate(&"Core")`)
        for unit in self.facts.units:
            if unit.kind == "var" and not unit.type_text:
                unit.type_text = self.assigned.get((unit.owner, unit.name), "")
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
            elif kind == "function_definition":
                self.function(st, cls)
            elif kind in ("variable_statement", "const_statement"):
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
        unit.type_text = self.declared_type(node)
        value = field_of(node, "value")
        if value is not None:
            self.expr(value, unit, cls, ())

    def declared_type(self, node) -> str:
        typ = field_of(node, "type")
        if typ is not None and typ.type == "type":
            return self.src.text(typ)
        value = field_of(node, "value")
        return self.infer(value) if value is not None else ""

    def infer(self, value) -> str:
        """The class a value is an instance (or the script) of, when the expression says it."""
        cast = gd_body.type_test(self, value)
        if cast is not None and value is not None:
            return cast[1]
        if value.type == "call":
            callee = self.src.text(named_children(value)[0])
            args = named_children(field_of(value, "arguments")) if field_of(value, "arguments") is not None else []
            if callee in ("preload", "load") and args and string_value(self.src, args[0]) is not None:
                return "path:" + string_value(self.src, args[0])
            return gd_body.CALLED + callee if is_identifier(callee) else ""
        if value.type == "attribute":
            kids = named_children(value)
            last = kids[-1]
            if last.type == "attribute_call" and self.src.text(named_children(last)[0]) == "new":
                head = kids[0]
                if head.type == "call":
                    return self.infer(head)
                return ".".join(self.src.text(k) for k in kids[:-1])
            if last.type == "attribute_call" and self.src.text(named_children(last)[0]) == "instantiate":
                args = field_of(last, "arguments")
                first = named_children(args)[0] if args is not None and named_children(args) else None
                name = string_value(self.src, first)
                return name or ""
        return ""

    def params(self, node, unit: Unit) -> None:
        params = field_of(node, "parameters")
        unit.required = 0
        for p in named_children(params) if params is not None else []:
            if p.type not in ("default_parameter", "typed_default_parameter"):
                unit.required += 1
            ident = p if p.type == "identifier" else next((c for c in named_children(p) if c.type == "identifier"), None)
            if ident is None:
                continue
            name = self.src.text(ident)
            unit.params.append(name)
            typ = field_of(p, "type")
            unit.types[name] = self.src.text(typ) if typ is not None and typ.type == "type" else ""

    def function(self, node, cls: ClassFact) -> None:
        unit = self.unit("func", self.src.text(field_of(node, "name")), cls.key, node)
        unit.static = any(c.type == "static_keyword" for c in node.children)
        ret = field_of(node, "return_type")
        unit.type_text = self.src.text(ret) if ret is not None else ""
        self.params(node, unit)
        if field_of(node, "body") is not None:
            self.block(field_of(node, "body"), unit, cls, ())

    def lambda_unit(self, node, owner: Unit, cls: ClassFact) -> Unit:
        line = self.src.line(node)
        lam = Unit("lambda", "gd", f"{owner.name}.<lambda:{line}>", f"{owner.qual}.<lambda:{line}>", cls.key,
                   self.src.display, line, self.src.end_line(node), local_id=len(self.facts.units))
        self.facts.units.append(lam)
        lam.types = dict(owner.types)
        lam.lambdas = dict(owner.lambdas)
        self.params(node, lam)
        if field_of(node, "body") is not None:
            self.block(field_of(node, "body"), lam, cls, ())
        return lam

    # ---- bodies ------------------------------------------------------------------------------------------------
    def add(self, unit: Unit, kind: str, name: str, node, cond, **extra) -> None:
        unit.refs.append(Ref(kind, name, line=self.src.line(node), cond=cond, **extra))

    def block(self, node, unit: Unit, cls: ClassFact, cond: tuple) -> None:
        for st in named_children(node):
            self.statement(st, unit, cls, cond)

    def statement(self, st, unit: Unit, cls: ClassFact, cond: tuple) -> None:
        gd_body.statement(self, st, unit, cls, cond)

    def expr(self, node, unit: Unit, cls: ClassFact, cond: tuple) -> None:
        gd_body.expr(self, node, unit, cls, cond)


def read_gd(src: Source, res_path: str) -> FileFacts:
    return GdReader(src, res_path).run()
