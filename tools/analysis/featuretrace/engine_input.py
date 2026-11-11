"""The native engine's input (log/trace-engine-plan.md, Task E1; log/trace-research.md F18): one pickle (protocol 5,
the facts' own format) of the units and the classes as the readers made them, and of every resolution the new core
queries, computed by the readers' and the new core's own logic so the engine needs none of it: for every name read in
a unit, what it is (definitions, the modules an import binds, a class of the scope, the enclosing function that
declares it); for every declared type name, what it names (an alias, a class merged); for every class, its bases,
merged group, prototype chain and inner names; the members by owner, the global types, where every import leads, every
module's exports, every file's JSX transform; and the vocabulary: every constant the new core takes from outside
itself and the model's field names, each from its one owner, so the engine binds to the definitions Python has.
`tables(ix)` are those values in the order of ROOT_PARTS; `write(ix, path)` writes the file.
"""
from __future__ import annotations

import ast
import dataclasses
import importlib
import pickle
from pathlib import Path

from .heap import jsx, names, typesys
from .js_transfer import chain
from .languages import INTRINSICS
from .model import ClassFact, Ref, Unit
from .vocab import IMPORT, JSX, NAME, T_NAMED

ROOT_PARTS = ("units", "classes", "members", "names", "types", "class_tables", "intrinsics", "modules", "exports",
              "jsx", "vocabulary")
PROTOCOL = 5                                 # the pickle protocol the engine reads (PEP 574)
HEAP = Path(__file__).resolve().parent / "heap"


class Light:
    """What the new core's resolution functions read: the index and a memo."""

    def __init__(self, ix):
        self.ix, self.memo = ix, {}


def walk(value, found_names: set, found_types: set) -> None:
    """Every name read and every declared type name inside a value of the facts."""
    todo = [value]
    while todo:
        v = todo.pop()
        if isinstance(v, (Unit, Ref, ClassFact)):
            continue
        if isinstance(v, tuple):
            if len(v) == 2 and v[0] == NAME and isinstance(v[1], str):
                found_names.add(v[1])
            elif len(v) == 5 and v[0] == T_NAMED:
                found_types.add((v[1], v[3], v[4]))
            todo.extend(v)
        elif isinstance(v, (list, set, frozenset)):
            todo.extend(v)
        elif isinstance(v, dict):
            todo.extend(v.values())


def unit_names(unit, found_types: set) -> set:
    found = set()
    for field in ("types", "values", "returns", "yields", "param_types", "result_type", "tparams", "this_type",
                  "indexes"):
        walk(getattr(unit, field), found, found_types)
    for ref in unit.refs:
        walk((ref.base, ref.args, ref.key), found, found_types)
        if ref.base is None and ref.name:
            found.add(ref.name)
    return found


def tables(ix) -> list:
    light, js = Light(ix), ix.js
    found_types = set()
    for cls in ix.classes.values():
        walk((cls.heritage, cls.tparams), set(), found_types)
    name_rows = []
    for unit in ix.units:
        for name in sorted(unit_names(unit, found_types)):
            owner, _ = names.enclosing(light, unit, name)
            gids, modules = js.binding(unit.file, name) if unit.lang == "js" else ([], [])
            name_rows.append((unit.gid, name, list(names.definitions(light, unit, name)) if unit.lang == "js" else [],
                              list(modules), js.class_of(name, unit.file, unit.owner) if unit.lang == "js" else None,
                              owner.gid if owner is not None else -1))
    type_rows = [(text, file, owner, typesys.resolve(light, text, file, owner)) for text, file, owner in
                 sorted(found_types)]
    class_rows = [(key, list(ix.base_keys(cls)), list(typesys.group(light, key)), typesys.identity(light, key),
                   list(chain(ix, key)), sorted(ix.inner.get(key, {}).items()))
                  for key, cls in sorted(ix.classes.items())]
    intrinsics = [(kind, [typesys.global_type(light, n) for n in kind_names]) for kind, kind_names in
                  sorted(INTRINSICS.items())]
    modules, exports, jsx_rows = [], [], []
    for file, facts in sorted(js.files.items()):
        imports = [(ref.text, js.resolve(file, ref.text)) for u in facts.units for ref in u.refs
                   if ref.kind == IMPORT and ref.text]
        modules.append((file, ix.offset[file], imports))
        for name in sorted(exported(js, file)):
            gids, mods = js.export(file, name)
            exports.append((file, name, list(gids), list(mods)))
        if any(ref.kind == JSX for u in facts.units for ref in u.refs):
            config = jsx.options(light, file)
            mode = config.get(jsx.JSX_MODE_OPTION)
            jsx_rows.append((file, sorted(config.items(), key=str), jsx.runtime(light, file, config, mode),
                             jsx.children_name(light, file, config, mode)))
    members = [(owner, name, list(g)) for owner, by_name in sorted(ix.members.items()) for name, g in sorted(by_name.items())]
    return [list(ix.units), sorted(ix.classes.values(), key=lambda c: c.key), members, name_rows, type_rows,
            class_rows, intrinsics, modules, exports, jsx_rows, vocabulary()]


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
    `owner.NAME` of a module `from .. import owner` gives it), read from its sources, and (model.Class, its field names)
    for each record the file holds: what the engine compares with and reads by are these values, each from its owner."""
    found = set()
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
    constants = [(f"{owner}.{name}", getattr(importlib.import_module(f"{__package__}.{owner}"), name))
                 for owner, name in sorted(found)]
    return constants + [(f"{record.__module__.rsplit('.', 1)[-1]}.{record.__name__}",
                         [f.name for f in dataclasses.fields(record)]) for record in (Unit, Ref, ClassFact)]


def write(ix, path: Path) -> int:
    """The file, written whole (pickle's own C writer): its size in bytes."""
    data = pickle.dumps(tables(ix), protocol=PROTOCOL)
    path.write_bytes(data)
    return len(data)
