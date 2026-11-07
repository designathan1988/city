"""The GDScript adapter: scripts parsed by the language's grammar; classes, functions, variables, signals and tables become units."""
import tree_sitter
import tree_sitter_gdscript

from ..model import GRAMMAR, Facts
from ..shape import HASH_STYLE, shape
from .gd_body import Body, State, node_text

PARSER = tree_sitter.Parser(tree_sitter.Language(tree_sitter_gdscript.language()))


class Script:
    """One class: the script itself or a class inside it."""

    def __init__(self, uid, file, name, base):
        self.uid, self.file, self.name, self.base = uid, file, name, base
        self.funcs, self.vars, self.signals, self.setters, self.returns, self.types = {}, {}, {}, {}, {}, {}
        self.constants = set()


class Table:
    """All scripts of the repository, so a name can be followed across files before edges are written."""

    def __init__(self, api, autoloads):
        self.api, self.autoloads = api, autoloads
        self.by_class, self.by_path = {}, {}

    def lookup(self, cls, member, table):
        seen = set()
        while cls is not None and cls.uid not in seen:
            seen.add(cls.uid)
            found = getattr(cls, table).get(member)
            if found is not None:
                return found, cls
            cls = self.script_of(cls.base)
        return None, None

    def script_of(self, name):
        if not name:
            return None
        if name.startswith("res://"):
            return self.by_path.get(name[6:]) or next((s for p, s in self.by_path.items() if p.endswith("/" + name[6:])), None)
        return self.by_class.get(name) or self.autoloads.get(name)


def _child(node, kind):
    return next((c for c in node.named_children if c.type == kind), None)


def _extends(node, src):
    t = _child(node, "type") or (node.named_children[0] if node.named_children else None)
    if t is None:
        return ""
    return node_text(t, src).strip("\"")


def collect(tree, src, file):
    """First pass: what a script declares."""
    uid = "gd:" + file
    top = Script(uid, file, "", "")
    scripts = [top]
    _members(tree.root_node, src, top, scripts)
    return top, scripts


def _type_of(node, src):
    t = node.child_by_field_name("type")
    if t is not None and t.type != "inferred_type":
        return node_text(t, src)
    value = node.child_by_field_name("value")
    if value is not None:
        v = node_text(value, src)
        if v.endswith(".new()"):
            return v[:-6]
        if v.startswith(("preload(", "load(")):
            return v[v.find("(") + 1:-1].strip("\"")
    return ""


def _members(parent, src, cls, scripts):
    for n in parent.named_children:
        k = n.type
        if k == "class_name_statement":
            cls.name = node_text(n.child_by_field_name("name"), src)
            for c in n.named_children:
                if c.type == "extends_statement":
                    cls.base = _extends(c, src)
        elif k == "extends_statement":
            cls.base = _extends(n, src)
        elif k == "function_definition" or k == "constructor_definition":
            name_node = n.child_by_field_name("name")
            name = node_text(name_node, src) if name_node is not None else "_init"
            cls.funcs[name] = f"{cls.uid}#{name}"
            r = n.child_by_field_name("return_type")
            cls.returns[name] = node_text(r, src) if r is not None else ""
        elif k in ("variable_statement", "const_statement"):
            name = node_text(n.child_by_field_name("name"), src)
            cls.vars[name] = f"{cls.uid}.{name}"
            cls.types[name] = _type_of(n, src)
            if k == "const_statement":
                cls.constants.add(name)
            if n.child_by_field_name("setget") is not None:
                cls.setters[name] = f"{cls.uid}.{name}#set"
        elif k == "signal_statement":
            name = node_text(n.child_by_field_name("name"), src)
            cls.signals[name] = f"{cls.uid}!{name}"
        elif k == "enum_definition":
            name_node = n.child_by_field_name("name")
            name = node_text(name_node, src) if name_node is not None else "anonymous"
            cls.vars[name] = f"{cls.uid}.{name}"
        elif k == "class_definition":
            name = node_text(n.child_by_field_name("name"), src)
            inner = Script(f"{cls.uid}${name}", cls.file, name, "")
            e = n.child_by_field_name("extends")
            inner.base = _extends(e, src) if e is not None else ""
            scripts.append(inner)
            body = n.child_by_field_name("body")
            if body is not None:
                _members(body, src, inner, scripts)


def _emit_class(table, cls, parent_node, src, facts):
    file = cls.file
    for n in parent_node.named_children:
        k = n.type
        line, end = n.start_point[0] + 1, n.end_point[0] + 1
        if k == "function_definition" or k == "constructor_definition":
            name_node = n.child_by_field_name("name")
            name = node_text(name_node, src) if name_node is not None else "_init"
            uid = cls.funcs[name]
            text = node_text(n, src)
            api = table.api
            callback = name in api.virtual
            taken = n.child_by_field_name("parameters")
            facts.unit(uid, "method", name, file, line, end, cls.uid, shape=shape(text, HASH_STYLE), statements=text.count("\n"), callback=callback,
                       repeating=name in api.repeating, input=name in api.input, inputs=len(taken.named_children) if taken is not None else 0,
                       signature=node_text(taken, src) if taken is not None else "")
            facts.define("method", name, uid)
            body = n.child_by_field_name("body")
            if body is not None:
                Body(table, cls, facts, src).block(body, State(uid))
        elif k in ("variable_statement", "const_statement"):
            name = node_text(n.child_by_field_name("name"), src)
            uid = cls.vars[name]
            facts.unit(uid, "field", name, file, line, end, cls.uid, shape=shape(node_text(n, src), HASH_STYLE), constant=k == "const_statement", type=cls.types.get(name, ""))
            facts.define("property", name, uid)
            value = n.child_by_field_name("value")
            b = Body(table, cls, facts, src)
            if value is not None:
                b.value_of_field(value, State(uid), name)
            setget = n.child_by_field_name("setget")
            if setget is not None:
                b.setget(setget, State(uid), cls.setters[name], name)
        elif k == "signal_statement":
            name = node_text(n.child_by_field_name("name"), src)
            facts.unit(cls.signals[name], "signal", name, file, line, end, cls.uid)
            facts.define("signal", name, cls.signals[name])
        elif k == "enum_definition":
            name_node = n.child_by_field_name("name")
            name = node_text(name_node, src) if name_node is not None else "anonymous"
            uid = cls.vars[name]
            facts.unit(uid, "enum", name, file, line, end, cls.uid)
            facts.define("enum_type", name, uid)
            for e in n.named_children:
                if e.type == "enumerator_list":
                    for item in e.named_children:
                        left = item.child_by_field_name("left")
                        if left is not None:
                            word = node_text(left, src)
                            facts.unit(f"{uid}.{word}", "enumerator", word, file, item.start_point[0] + 1, item.start_point[0] + 1, uid)
                            facts.define("enum", word, f"{uid}.{word}")
        elif k == "class_definition":
            name = node_text(n.child_by_field_name("name"), src)
            inner = next(s for s in table.inner[file] if s.uid == f"{cls.uid}${name}")
            facts.unit(inner.uid, "class", name, file, line, end, cls.uid)
            body = n.child_by_field_name("body")
            if body is not None:
                _emit_class(table, inner, body, src, facts)
        elif k == "expression_statement":
            Body(table, cls, facts, src).expr(n, State(cls.uid))


def scan(repo, tracked, scope, api, autoloads):
    """Facts of every script in scope. Autoloads come from the project file: their names are scripts anywhere."""
    files = sorted(p for p in tracked if p.endswith(".gd") and p in scope)
    table = Table(api, {})
    table.inner = {}
    parsed = {}
    for file in files:
        text = repo.text(tracked[file])
        if text is None:
            continue
        src = text.encode("utf8")
        tree = PARSER.parse(src)
        top, scripts = collect(tree, src, file)
        parsed[file] = (tree, src, top)
        table.by_path[file] = top
        table.inner[file] = scripts
        for s in scripts:
            if s.name:
                table.by_class[s.name] = s
    for name, path in autoloads.items():
        s = table.script_of("res://" + path)
        if s is not None:
            table.autoloads[name] = s
    facts = Facts()
    for file, (tree, src, top) in parsed.items():
        facts.unit(top.uid, "class", file.rsplit("/", 1)[-1], file, 1, tree.root_node.end_point[0] + 1, None, shape=shape(src.decode("utf8", "replace"), HASH_STYLE), script=True, base=top.base)
        facts.define("file", file, top.uid)
        if top.name:
            facts.define("class", top.name, top.uid)
        for s in table.inner[file]:
            base = table.script_of(s.base)
            facts.edge(s.uid, "inherit", base.uid if base else "ext:" + s.base, file, 1, GRAMMAR)
        _emit_class(table, top, tree.root_node, src, facts)
    return facts, {"scripts": len(parsed), "errors": sorted(f for f, (t, _, _) in parsed.items() if t.root_node.has_error)}
