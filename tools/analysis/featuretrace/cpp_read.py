"""C++ reader: the definitions a file makes, in their namespaces and classes, as the language declares them.

`namespace`, `class`/`struct`/`union` (with bases), `enum` (scoped or not), functions (inline in a class, or out of line
as `A::f`), declarations (methods, fields, variables), `using namespace`, `using X = Y`, `typedef` and `#include` are each
read by their grammar rule (log/trace-rules.md, C++ rows). An anonymous namespace or a `static` free function has
internal linkage. Bodies are read by `cpp_body`; every value is kept as an expression tree built by `ir_cpp`.
"""
from __future__ import annotations

from . import cpp_body, ir_cpp
from .cpp_names import WRAPPERS, join, strip_templates, type_of, unwrap
from .model import ClassFact, FileFacts, Unit
from .source import Source, field_of, named_children

CLASS_NODES = ("class_specifier", "struct_specifier", "union_specifier")
RECURSE = ("preproc_ifdef", "preproc_if", "preproc_else", "preproc_elif", "preproc_elifdef", "field_declaration_list",
           "declaration_list", "linkage_specification", "export_declaration")


def deduced(declared: str) -> bool:
    """A placeholder type (`auto`, `auto &`, `const auto *`, `decltype(auto)`): the type comes from the value (C5)."""
    return declared.replace("const", "").replace("&", "").replace("*", "").strip() in ("auto", "", "decltype(auto)")


class CppReader:
    def __init__(self, src: Source):
        self.src = src
        self.facts = FileFacts(src.display, "cpp")
        self.template = []           # the type parameters of the template being read
        self._lambdas = {}           # lambda node start byte -> local id (each lambda one unit)

    def run(self) -> FileFacts:
        root = self.src.parse("cpp")
        self.scope(root, "", None, False)
        self.facts.errors = int(root.has_error)
        return self.facts

    def unit(self, kind, name, qual, owner, node, **extra) -> Unit:
        unit = Unit(kind, "cpp", name, qual, owner, self.src.display, self.src.line(node), self.src.end_line(node),
                    local_id=len(self.facts.units), **extra)
        self.facts.units.append(unit)
        return unit

    def ir(self, node, unit: Unit) -> tuple:
        return ir_cpp.build(self.src, node, lambda n: self.lambda_id(n, unit))

    def lambda_id(self, node, owner: Unit) -> int:
        """The lambda's unit (made once, whether met while building a tree or while walking the body)."""
        if node.start_byte not in self._lambdas:
            self._lambdas[node.start_byte] = cpp_body.lambda_unit(self, node, owner).local_id
        return self._lambdas[node.start_byte]

    def scope(self, node, prefix: str, cls: ClassFact | None, anon: bool) -> None:
        for st in named_children(node):
            kind = st.type
            if kind == "namespace_definition":
                name, body = field_of(st, "name"), field_of(st, "body")
                if body is not None:
                    inner = join(prefix, self.src.text(name)) if name is not None else prefix
                    self.scope(body, inner, None, anon or name is None)
            elif kind in CLASS_NODES:
                self.klass(st, prefix, anon)
            elif kind == "enum_specifier":
                self.enum(st, prefix)
            elif kind == "function_definition":
                self.function(st, prefix, cls, anon)
            elif kind in ("declaration", "field_declaration"):
                self.declaration(st, prefix, cls, anon)
            elif kind == "using_declaration":
                text = self.src.text(st)
                if " namespace " in f" {text} ":
                    self.facts.usings.append(strip_templates(text.split("namespace", 1)[1].rstrip(";")))
            elif kind == "preproc_include":
                path = field_of(st, "path")
                if path is not None and path.type == "string_literal":
                    self.facts.includes.append(self.src.text(path).strip('"'))
            elif kind in ("alias_declaration", "type_definition"):
                self.alias(st, prefix)
            elif kind in ("preproc_def", "preproc_function_def"):
                name = field_of(st, "name")
                if name is not None:
                    self.unit("macro", self.src.text(name), self.src.text(name), "", st)
            elif kind == "template_declaration":
                params = field_of(st, "parameters")
                self.template = [self.src.text(n) for p in named_children(params) if params is not None
                                 for n in named_children(p) if n.type == "type_identifier"]
                self.scope(st, prefix, cls, anon)
                self.template = []
            elif kind in RECURSE:
                self.scope(st, prefix, cls, anon)

    def klass(self, node, prefix: str, anon: bool) -> None:
        name_node, body = field_of(node, "name"), field_of(node, "body")
        if body is None:
            return
        name = strip_templates(self.src.text(name_node)) if name_node is not None else f"<anon:{self.src.line(node)}>"
        key = join(prefix, name)
        fact = ClassFact(key, "cpp", name.split("::")[-1], self.src.display, self.src.line(node), outer=prefix,
                         params=self.template)
        self.template = []
        for child in named_children(node):
            if child.type == "base_class_clause":
                fact.bases += [self.src.text(b) for b in named_children(child) if b.type != "access_specifier"]
        self.facts.classes.append(fact)
        self.unit("class", fact.name, key, prefix, node, file_local=anon)
        self.scope(body, key, fact, anon)

    def enum(self, node, prefix: str) -> None:
        name_node, body = field_of(node, "name"), field_of(node, "body")
        scoped = any(c.type in ("class", "struct") for c in node.children)
        owner = prefix
        if name_node is not None:
            name = self.src.text(name_node)
            owner = join(prefix, name)
            self.unit("enum", name, owner, prefix, node)
            self.facts.classes.append(ClassFact(owner, "cpp", name, self.src.display, self.src.line(node), outer=prefix))
        for item in named_children(body) if body is not None else []:
            if item.type == "enumerator":
                name = self.src.text(field_of(item, "name"))
                self.unit("enumerator", name, join(owner, name), owner, item)
                if not scoped and owner != prefix:
                    self.facts.aliases[join(prefix, name)] = join(owner, name)

    def alias(self, node, prefix: str) -> None:
        typ = field_of(node, "type")
        if node.type == "alias_declaration":
            name = field_of(node, "name")
            if name is not None and typ is not None:
                self.facts.aliases[join(prefix, self.src.text(name))] = type_of(self.src.text(typ))
            return
        for i, child in enumerate(node.children):
            if node.field_name_for_child(i) == "declarator" and typ is not None:
                name = unwrap(child)
                if name is not None:
                    self.facts.aliases[join(prefix, self.src.text(name))] = type_of(self.src.text(typ))

    def function(self, node, prefix: str, cls: ClassFact | None, anon: bool) -> None:
        decl = field_of(node, "declarator")
        while decl is not None and decl.type != "function_declarator":
            decl = field_of(decl, "declarator") or (named_children(decl) or [None])[-1]
        if decl is None:
            return
        raw = strip_templates(self.src.text(field_of(decl, "declarator")))
        qual = join(prefix, raw)
        owner, _, name = qual.rpartition("::")
        static = any(self.src.text(c) == "static" for c in node.children if c.type == "storage_class_specifier")
        unit = self.unit("func", name, qual, owner, node, static=static,
                         file_local=anon or (static and cls is None and "::" not in raw))
        unit.virtual = self.is_virtual(node, decl)
        typ = field_of(node, "type")
        declared = type_of(self.src.text(typ)) if typ is not None else ""
        unit.type_text = "" if deduced(declared) else declared
        self.params(decl, unit)
        for child in named_children(node):
            if child.type == "field_initializer_list":
                cpp_body.expr(self, child, unit, ())
        body = field_of(node, "body")
        if body is not None:
            cpp_body.block(self, body, unit, ())

    def is_virtual(self, node, decl) -> bool:
        words = {self.src.text(c) for c in node.children if c.type in ("virtual", "storage_class_specifier")}
        marks = {c.type for c in decl.children}
        return "virtual" in words or "virtual_specifier" in marks or any(c.type == "virtual" for c in node.children)

    def params(self, decl, unit: Unit) -> None:
        params = field_of(decl, "parameters")
        unit.required = 0
        for p in named_children(params) if params is not None else []:
            if p.type == "variadic_parameter_declaration" or self.src.text(p) == "...":
                unit.variadic = True
                continue
            if p.type != "optional_parameter_declaration":
                unit.required += 1
            name = unwrap(field_of(p, "declarator"))
            if name is None:
                continue
            text = self.src.text(name)
            unit.params.append(text)
            typ = field_of(p, "type")
            declared = type_of(self.src.text(typ)) if typ is not None else ""
            pointer = self.src.text(field_of(p, "declarator")).lstrip().startswith("*")
            declared = f"{declared} *" if pointer and declared else declared
            unit.types[text] = [] if deduced(declared) else [("cast", declared)]

    def declaration(self, node, prefix: str, cls: ClassFact | None, anon: bool) -> None:
        typ = field_of(node, "type")
        if typ is not None and typ.type in CLASS_NODES:
            self.klass(typ, prefix, anon)
        elif typ is not None and typ.type == "enum_specifier":
            self.enum(typ, prefix)
        type_text = type_of(self.src.text(typ)) if typ is not None else ""
        static = any(self.src.text(c) == "static" for c in node.children if c.type == "storage_class_specifier")
        for i, child in enumerate(node.children):
            if node.field_name_for_child(i) != "declarator":
                continue
            inner = child
            while inner is not None and inner.type in WRAPPERS and inner.type != "function_declarator":
                inner = field_of(inner, "declarator") or (named_children(inner) or [None])[-1]
            if inner is not None and inner.type == "function_declarator":
                raw = strip_templates(self.src.text(field_of(inner, "declarator")))
                qual = join(prefix, raw)
                owner, _, name = qual.rpartition("::")
                unit = self.unit("func", name, qual, owner, node, decl_only=True, static=static,
                                 file_local=anon or (static and cls is None))
                unit.virtual = self.is_virtual(node, inner)
                unit.type_text = "" if deduced(type_text) else type_text
                self.params(inner, unit)
                continue
            name = unwrap(child)
            if name is None or name.type not in ("identifier", "field_identifier"):
                continue
            text = self.src.text(name)
            unit = self.unit("field" if cls is not None else "var", text, join(prefix, text), prefix, node,
                             static=static, file_local=anon or (static and cls is None))
            unit.type_text = "" if deduced(type_text) else type_text
            value = field_of(child, "value") if child.type == "init_declarator" else None
            for j, sub in enumerate(node.children):
                if node.field_name_for_child(j) == "default_value" and j > i:
                    value = value or sub
            if value is not None:
                unit.values.append(self.ir(value, unit))
                cpp_body.expr(self, value, unit, ())


def read_cpp(src: Source) -> FileFacts:
    return CppReader(src).run()
