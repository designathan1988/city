"""JavaScript / TypeScript declarations (log/trace-rules.md, JavaScript and TypeScript rows; J3, J4, J8, J9): imports
and exports of a module, its top-level declarations, classes and their members, object literals, interfaces, enums,
namespaces, type aliases and ambient declarations (`declare`, `declare global`, `declare module "m"`).
"""
from __future__ import annotations

from . import ir_js, js_body, js_bounds, js_class, js_sigs, js_types, jsdoc
from .model import ALIAS_KIND, ClassFact
from .source import field_of, named_children, string_value
from .vocab import LEVEL_ALIAS, LEVEL_TYPE

GLOBAL_KEY = "<global>"            # the global scope's names (scripts, `declare global`): one class for the program
AMBIENT = "ambient:"               # `declare module "m" { }`: the module m's declarations
GLOBAL_WORD = "global"             # `global { }`: a global augmentation (TypeScript handbook, Declaration Merging)
DECLARATIONS = ("function_declaration", "generator_function_declaration", "function_signature", "class_declaration",
                "abstract_class_declaration", "lexical_declaration", "variable_declaration", "interface_declaration",
                "enum_declaration", "type_alias_declaration", "internal_module", "module", "ambient_declaration")


def has_module_syntax(r, root) -> bool:
    return any(st.type in ("import_statement", "export_statement") for st in named_children(root))


def module_body(r, block, owner: str, holder) -> None:
    """The statements of a module or namespace: imports, exports, declarations (members of `owner`), and the code
    run when it is evaluated (references of `holder`). A global augmentation written in an ambient module without
    `declare` (`global { ... }`, TypeScript handbook: Declaration Merging) is read where the grammar recovers it."""
    statements = named_children(block)
    for i, st in enumerate(statements):
        kind = st.type
        if kind == "statement_block" and i and global_word(r, statements[i - 1]):
            global_augmentation(r, st, holder)
        elif kind == "ERROR" and global_word(r, st):
            continue
        elif kind == "import_statement":
            imports(r, st, holder)
        elif kind == "export_statement":
            exports(r, st, owner, holder)
        elif kind in DECLARATIONS:
            declaration(r, st, owner, holder)
        elif kind == "expression_statement" and named_children(st) and named_children(st)[0].type == "internal_module":
            declaration(r, named_children(st)[0], owner, holder)
        else:
            js_body.statement(r, st, holder, ())


def global_word(r, node) -> bool:
    """The word `global` the grammar (tree-sitter-typescript 0.23) leaves as an error before a global augmentation."""
    return node.type == "ERROR" and r.src.text(node).strip() == GLOBAL_WORD


def global_augmentation(r, block, holder) -> None:
    r.facts.classes.append(ClassFact(GLOBAL_KEY, "js", "", r.key, r.src.line(block)))
    r.ambient += 1
    try:
        module_body(r, block, GLOBAL_KEY, holder)
    finally:
        r.ambient -= 1


def imports(r, st, holder) -> None:
    source = field_of(st, "source")
    spec = string_value(r.src, source)
    for clause in named_children(st):
        if clause.type == "import_clause":
            for part in named_children(clause):
                if part.type == "identifier":
                    r.facts.imports[r.src.text(part)] = (spec, "default")
                elif part.type == "namespace_import":
                    r.facts.imports[r.src.text(named_children(part)[0])] = (spec, "*")
                elif part.type == "named_imports":
                    for item in named_children(part):
                        if item.type == "import_specifier":
                            name, alias = field_of(item, "name"), field_of(item, "alias")
                            r.facts.imports[r.src.text(alias or name)] = (spec, ir_js.member_name(r.src, name))
        elif clause.type == "import_require_clause":
            required = string_value(r.src, field_of(clause, "source"))
            r.facts.imports[r.src.text(named_children(clause)[0])] = (required, "*")
            spec = required
    if spec is not None:
        r.add(holder, "import", "", st, (), text=spec)        # the imported module runs first (J3)


def exports(r, st, owner: str, holder) -> None:
    spec = string_value(r.src, field_of(st, "source"))
    declared, value = field_of(st, "declaration"), field_of(st, "value")
    default = any(c.type == "default" for c in st.children)
    if declared is not None:
        names = declaration(r, declared, owner, holder)
        for name in names:
            r.facts.exports["default" if default else name] = name
        return
    if value is not None:
        r.facts.exports["default"] = default_value(r, value, owner, holder)
        return
    clause = next((c for c in named_children(st) if c.type == "export_clause"), None)
    star_as = next((c for c in named_children(st) if c.type == "namespace_export"), None)
    if clause is not None:
        for item in named_children(clause):
            if item.type != "export_specifier":
                continue
            name, alias = field_of(item, "name"), field_of(item, "alias")
            local = ir_js.member_name(r.src, name)
            exported = ir_js.member_name(r.src, alias) if alias is not None else local
            r.facts.exports[exported] = ("from", spec, local) if spec is not None else local
    elif star_as is not None and spec is not None:
        r.facts.exports[r.src.text(named_children(star_as)[0])] = ("from", spec, "*")
    elif spec is not None:
        r.facts.star_exports.append(spec)
    if spec is not None:
        r.add(holder, "import", "", st, (), text=spec)


def default_value(r, value, owner: str, holder) -> str:
    """`export default <value>`: a named function or class keeps its name; anything else is the name `default`."""
    if value.type == "identifier":
        return r.src.text(value)
    own = field_of(value, "name")
    name = r.src.text(own) if own is not None else "default"
    if value.type in ir_js.FUNCTIONS:
        r.function(value, name, owner, None)
    elif value.type == "class":
        js_class.class_declaration(r, value, name, owner, holder)
    else:
        unit = r.unit("const", name, owner, value)
        unit.values.append(r.ir(value, unit))
        js_body.expr(r, value, holder, ())
    return name


def declaration(r, node, owner: str, holder) -> list[str]:
    """A declaration at module or namespace level: the names it puts in `owner`."""
    kind = node.type
    name_node = field_of(node, "name")
    name = r.src.text(name_node) if name_node is not None else ""
    if kind in ("function_declaration", "generator_function_declaration", "function_signature"):
        r.function(node, name, owner, None)
        return [name]
    if kind in ("class_declaration", "abstract_class_declaration"):
        js_class.class_declaration(r, node, name, owner, holder)
        return [name]
    if kind in ("lexical_declaration", "variable_declaration"):
        return variables(r, node, owner, holder)
    if kind == "interface_declaration":
        key = new_class(r, node, name, owner)
        cls = r.facts.classes[-1]
        cls.shape = True                                             # no object of its own (`structural`)
        cls.tparams = js_sigs.open_params(r, node, LEVEL_TYPE, key)  # erased (T1), and kept (Task 4)
        try:
            for clause in (c for c in named_children(node) if c.type == "extends_type_clause"):
                cls.bases += [r.src.text(t) for t in named_children(clause)]
                cls.heritage += [js_sigs.tree(r, js_types.bare(t), key) for t in named_children(clause)]
            js_class.members(r, field_of(node, "body"), key, None)
        finally:
            js_sigs.close_params(r)
        return [name]
    if kind == "enum_declaration":
        key = new_class(r, node, name, owner)
        for item in named_children(field_of(node, "body")):
            target = field_of(item, "name") if item.type == "enum_assignment" else item
            if target is not None and item.type != "comment":
                r.unit("enumerator", ir_js.member_name(r.src, target), key, item)
        return [name]
    if kind == "type_alias_declaration":
        r.facts.aliases[scoped(owner, name)] = " ".join(r.src.text(field_of(node, "value")).split())
        alias = r.unit(ALIAS_KIND, name, owner, node, scoped(owner, name))   # the values its type admits (T1')
        alias.tparams = js_sigs.open_params(r, node, LEVEL_ALIAS, owner)     # erased (T1), and kept (Task 4)
        try:
            alias.values += js_types.values(r, field_of(node, "value"), alias)
            alias.bound = js_bounds.of(r, field_of(node, "value"))     # what a place of this type admits (S2)
            alias.result_type = js_sigs.tree(r, js_types.bare(field_of(node, "value")), owner)
        finally:
            js_sigs.close_params(r)
        return [name]
    if kind in ("internal_module", "module"):
        spec = string_value(r.src, name_node)
        key = AMBIENT + spec if spec is not None else new_class(r, node, name, owner)
        if spec is not None:
            r.facts.classes.append(ClassFact(key, "js", "", r.key, r.src.line(node)))
            r.facts.config.setdefault("ambient", []).append(key)
        else:
            r.facts.config.setdefault("namespaces", []).append(key)
        body = field_of(node, "body")
        if body is not None:
            module_body(r, body, key, holder)
        return [name] if spec is None else []
    if kind == "ambient_declaration":
        names = []
        r.ambient += 1                                          # what it declares only declares (Task 4)
        try:
            for part in named_children(node):
                if part.type == "statement_block":              # declare global { ... }
                    r.facts.classes.append(ClassFact(GLOBAL_KEY, "js", "", r.key, r.src.line(node)))
                    module_body(r, part, GLOBAL_KEY, holder)
                elif part.type in DECLARATIONS:
                    names += declaration(r, part, owner, holder)
        finally:
            r.ambient -= 1
        return names
    return []


def scoped(owner: str, name: str) -> str:
    return f"{owner}.{name}" if "#" in owner or owner.startswith((AMBIENT, GLOBAL_KEY)) else f"{owner}#{name}"


def new_class(r, node, name: str, owner: str, outer: str | None = None) -> str:
    key = scoped(owner, name)
    r.facts.classes.append(ClassFact(key, "js", name, r.key, r.src.line(node), outer=owner if outer is None else outer))
    r.unit("class", name, owner, node, qual=key)
    return key


def variables(r, node, owner: str, holder) -> list[str]:
    """`const`/`let`/`var` at module or namespace level: each name a member, its initializer's value (J9: a
    function or class written to it takes its name); a destructuring declares each name its part."""
    const = any(c.type == "const" for c in node.children) or r.src.text(node).lstrip().startswith("const")
    names = []
    for declarator in (c for c in named_children(node) if c.type == "variable_declarator"):
        target, value = field_of(declarator, "name"), field_of(declarator, "value")
        if target.type != "identifier":
            temp = r.unit("const", f"<destructured:{r.src.line(declarator)}>", owner, declarator)
            temp.values.append(r.ir(value, temp))
            js_body.expr(r, value, holder, ())
            for part, tree in parts(r, target, ("name", temp.name)):
                unit = r.unit("const" if const else "var", part, owner, declarator)
                unit.values.append(tree)
                names.append(part)
            continue
        name = r.src.text(target)
        names.append(name)
        if value is not None and value.type in ir_js.FUNCTIONS:
            r.function(value, name, owner, None)
            continue
        if value is not None and value.type == "class":
            js_class.class_declaration(r, value, name, owner, holder)
            continue
        unit = r.unit("const" if const else "var", name, owner, declarator)
        unit.type_text = r.type_text(field_of(declarator, "type"))
        unit.values += r.type_values(field_of(declarator, "type"), unit)
        unit.bound = js_bounds.of(r, field_of(declarator, "type")) \
            or (jsdoc.declared(r, declarator) if jsdoc.is_js(r) else ())         # S2, its JSDoc `@type` (S2')
        unit.result_type = js_sigs.of(r, field_of(declarator, "type"), owner)   # kept whole (Task 4)
        if value is not None:
            unit.values.append(r.ir(value, unit))
            js_body.expr(r, value, holder, ())
    return names


def parts(r, pattern, tree: tuple) -> list[tuple[str, tuple]]:
    """The names a destructuring pattern binds, each with the part of `tree` it takes (J1)."""
    kind = pattern.type
    if kind in ("identifier", "shorthand_property_identifier_pattern"):
        return [(r.src.text(pattern), tree)]
    out = []
    for part in named_children(pattern):
        if kind == "array_pattern":
            out += parts(r, part, ("elements", tree))
        elif part.type == "shorthand_property_identifier_pattern":
            out.append((r.src.text(part), ("member", tree, r.src.text(part), False)))
        elif part.type == "pair_pattern":
            key = ir_js.member_name(r.src, field_of(part, "key"))
            out += parts(r, field_of(part, "value"), ("member", tree, key, False))
        elif part.type == "object_assignment_pattern":
            left = field_of(part, "left")
            out += parts(r, left, ("member", tree, r.src.text(left), False))
        elif part.type in ("rest_pattern", "assignment_pattern"):
            out += parts(r, field_of(part, "left") or named_children(part)[0], tree)
    return out
