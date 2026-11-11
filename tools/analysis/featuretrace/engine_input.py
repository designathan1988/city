"""The native engine's input (log/trace-engine-plan.md, Task E1; log/trace-research.md F18): one pickle (protocol 5,
the facts' own format) of the units and the classes as the readers made them, and of every resolution the new core
queries, computed by the readers' and the new core's own logic so the engine needs none of it: for every name read in
a unit, what it is (definitions, the modules an import binds, a class of the scope, the enclosing function that
declares it); for every declared type name, what it names (an alias, a class merged); for every class, its bases,
merged group, prototype chain and inner names; the members by owner, the global types, where every import leads, every
module's exports, every file's JSX transform, what a `typeof` in a type names, what each operator and literal gives;
and the vocabulary: every constant the new core takes from outside itself, the new core's own tags, and the model's
field names, each from its one owner, so the engine binds to the definitions Python has.
`tables(ix)` are those values in the order of ROOT_PARTS; `write(ix, path)` writes the file, a dict of them by part.
"""
from __future__ import annotations

import ast
import dataclasses
import importlib
import pickle
from pathlib import Path

from . import truth
from .heap import jsx, names, trees, typesys
from .js_transfer import chain
from .languages import INTRINSICS, JSX_CLASSIC, JSX_FACTORY_DEFAULT, JSX_FACTORY_OPTION
from .model import ClassFact, Ref, Unit
from .solver import LANG
from .vocab import IMPORT, JSX, LITERAL, NAME, OP, T_NAMED, T_QUERY, UNARY

ROOT_PARTS = ("units", "classes", "members", "names", "types", "class_tables", "intrinsics", "modules", "exports",
              "jsx", "queries", "operators", "literals", "vocabulary")
PROTOCOL = 5                                 # the pickle protocol the engine reads (PEP 574)
HEAP = Path(__file__).resolve().parent / "heap"


class Light:
    """What the new core's resolution functions read: the index and a memo."""

    def __init__(self, ix):
        self.ix, self.memo = ix, {}


class Found:
    """What the facts' trees ask of the readers: names read, declared type names, `typeof` names, operators, literals,
    and the units a JSX element is written in (a tree of one, or a reference: each reads its factory)."""

    def __init__(self):
        self.types, self.queries, self.operators, self.literals = set(), set(), set(), set()
        self.elements = False                  # the walk met a JSX element

    def walk(self, value, names_read: set) -> None:
        todo = [value]
        while todo:
            v = todo.pop()
            if isinstance(v, (Unit, Ref, ClassFact)):
                continue
            if isinstance(v, tuple):
                tag = v[0] if v else None
                if tag == NAME and len(v) == 2 and isinstance(v[1], str):
                    names_read.add(v[1])
                elif tag == T_NAMED and len(v) == 5:
                    self.types.add((v[1], v[3], v[4]))
                elif tag == T_QUERY and len(v) == 4:
                    self.queries.add((v[2], v[3], v[1]))
                elif tag in (OP, UNARY) and len(v) >= 3 and isinstance(v[1], str):
                    self.operators.add((v[1], tag == UNARY))
                elif tag == LITERAL and len(v) == 2 and isinstance(v[1], str):
                    self.literals.add(v[1])
                elif tag == JSX:
                    self.elements = True
                todo.extend(v)
            elif isinstance(v, (list, set, frozenset)):
                todo.extend(v)
            elif isinstance(v, dict):
                todo.extend(v.values())

    def unit(self, unit) -> tuple[set, bool]:
        """The names a unit reads, and whether a JSX element is written in it."""
        found, self.elements = set(), False
        for field in ("types", "values", "returns", "yields", "param_types", "result_type", "tparams", "this_type",
                      "indexes"):
            self.walk(getattr(unit, field), found)
        for ref in unit.refs:
            self.walk((ref.base, ref.args, ref.key), found)
            if ref.base is None and ref.name:
                found.add(ref.name)
        return found, self.elements or any(ref.kind == JSX for ref in unit.refs)


def tables(ix) -> list:
    light, js, found = Light(ix), ix.js, Found()
    for cls in ix.classes.values():
        found.walk((cls.heritage, cls.tparams), set())
    read, elements = {}, set()
    for unit in ix.units:
        read[unit.gid], element = found.unit(unit)
        if element:
            elements.add(unit.gid)
    modules, exports, jsx_rows = [], [], []
    for file, facts in sorted(js.files.items()):
        imports = [(ref.text, js.resolve(file, ref.text)) for u in facts.units for ref in u.refs
                   if ref.kind == IMPORT and ref.text]
        modules.append((file, ix.offset[file], imports))
        for name in sorted(exported(js, file)):
            gids, mods = js.export(file, name)
            exports.append((file, name, list(gids), list(mods)))
        writing = [u for u in facts.units if u.gid in elements]
        if writing:
            config = jsx.options(light, file)
            mode = config.get(jsx.JSX_MODE_OPTION)
            jsx_rows.append((file, sorted(config.items(), key=str), jsx.runtime(light, file, config, mode),
                             jsx.children_name(light, file, config, mode)))
            if mode in JSX_CLASSIC:                 # the classic factory is a name read where the element is
                head = jsx.dotted(config.get(JSX_FACTORY_OPTION) or JSX_FACTORY_DEFAULT)
                while head[0] != NAME:
                    head = head[1]
                for u in writing:
                    read[u.gid].add(head[1])
    name_rows = []
    for unit in ix.units:
        for name in sorted(read[unit.gid]):
            owner, _ = names.enclosing(light, unit, name)
            gids, bound = js.binding(unit.file, name)
            name_rows.append((unit.gid, name, list(names.definitions(light, unit, name)), list(bound),
                              js.class_of(name, unit.file, unit.owner), owner.gid if owner is not None else -1))
    type_rows = [(text, file, owner, typesys.resolve(light, text, file, owner)) for text, file, owner in
                 sorted(found.types)]
    class_rows = [(key, list(ix.base_keys(cls)), list(typesys.group(light, key)), typesys.identity(light, key),
                   list(chain(ix, key)), sorted(ix.inner.get(key, {}).items()))
                  for key, cls in sorted(ix.classes.items())]
    intrinsics = [(kind, [typesys.global_type(light, n) for n in kind_names]) for kind, kind_names in
                  sorted(INTRINSICS.items())]
    queries = [(file, owner, name, list(names.definitions(light, Scope(file, owner), name)))
               for file, owner, name in sorted(found.queries)]
    operators = [(op, unary, [v[len(LANG):] for v in truth.operator_values(op, unary) if v.startswith(LANG)])
                 for op, unary in sorted(found.operators)]
    literals = [(kind, trees.literal_kinds(kind)) for kind in sorted(found.literals)]
    members = [(owner, name, list(g)) for owner, by_name in ix.members.items() for name, g in by_name.items()]
    return [list(ix.units), sorted(ix.classes.values(), key=lambda c: c.key), members, name_rows, type_rows,
            class_rows, intrinsics, modules, exports, jsx_rows, queries, operators, literals, vocabulary()]


class Scope:
    """Where a `typeof` in a declared type is written, as `names.definitions` reads it."""

    def __init__(self, file: str, owner: str):
        self.file, self.owner = file, owner


def exported(js, module: str, seen=None) -> set:
    """Every name a module exports: its own exports, a script's top-level names, what its `export *` re-exports."""
    seen = seen if seen is not None else set()
    if module in seen or module not in js.files:
        return set()
    seen.add(module)
    facts = js.files[module]
    out = set(facts.exports)
    if facts.script and not facts.exports:
        out |= set(js.ix.members.get(module, {}))
    for spec in facts.star_exports:
        target = js.resolve(module, spec)
        if target is not None:
            out |= exported(js, target, seen) - {"default"}
    return out


def vocabulary() -> list:
    """(owner.NAME, value) for every constant the new core imports from outside itself (`from ..owner import NAME`, or
    `owner.NAME` of a module `from .. import owner` gives it) and every constant a module of the new core defines
    (heap.module.NAME: its node tags and site parts), read from the sources; (model.Class, its field names) for each
    record the file holds. What the engine compares with and reads by are these values, each from its one owner."""
    found, own = set(), []
    for source in sorted(HEAP.glob("*.py")):
        tree = ast.parse(source.read_text(encoding="utf-8"))
        modules, attributes = set(), set()
        for node in ast.walk(tree):
            if isinstance(node, ast.ImportFrom) and node.level == 2:
                for alias in node.names:
                    if node.module is None:
                        modules.add(alias.asname or alias.name)
                    elif alias.name.isupper():
                        found.add((node.module, alias.name))
            elif isinstance(node, ast.Attribute) and isinstance(node.value, ast.Name) and node.attr.isupper():
                attributes.add((node.value.id, node.attr))
        found |= {(owner, name) for owner, name in attributes if owner in modules}
        module = importlib.import_module(f"{__package__}.heap.{source.stem}")
        for statement in tree.body:
            targets = statement.targets if isinstance(statement, ast.Assign) else []
            for name in [n.id for t in targets for n in ([t] if isinstance(t, ast.Name) else
                                                         t.elts if isinstance(t, ast.Tuple) else [])]:
                value = getattr(module, name)
                if name.isupper() and not isinstance(value, dict):
                    own.append((f"heap.{source.stem}.{name}", value))
    constants = [(f"{owner}.{name}", getattr(importlib.import_module(f"{__package__}.{owner}"), name))
                 for owner, name in sorted(found)]
    return constants + sorted(own, key=lambda row: row[0]) + [
        (f"{record.__module__.rsplit('.', 1)[-1]}.{record.__name__}", [f.name for f in dataclasses.fields(record)])
        for record in (Unit, Ref, ClassFact)]


def write(ix, path: Path) -> int:
    """The file, written whole (pickle's own C writer): each part by its name. Its size in bytes."""
    data = pickle.dumps(dict(zip(ROOT_PARTS, tables(ix))), protocol=PROTOCOL)
    path.write_bytes(data)
    return len(data)
