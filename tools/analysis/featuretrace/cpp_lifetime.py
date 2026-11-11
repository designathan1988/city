"""C++ object lifetime (log/trace-rules.md: CALL, C7; "The pending rules found against a real run").

A constructor runs its bases' constructors and its object members' constructors. A destructor, after its body, runs the
destructors of the members it holds by value, of the objects a library owner it holds owns (a smart pointer's pointee, a
container's or an optional's, pair's, tuple's, variant's elements: `rules.LIBRARY_OWNERS`), then of its bases
([class.dtor], [unique.ptr.single.dtor], [container.reqmts]); a class with no destructor written has the implicit one,
which does the same. An object's destructor runs at the end of its scope and on `delete`. A class declared in a function
is named by that function first (`Unit.local_types`, [class.local]).
"""
from __future__ import annotations

from .cpp_names import strip_templates, template_args
from .library import clean
from .rules import LIBRARY_OWNERS


class Site:
    """A place a link is written that is no reference of a unit (a constructor's own line, a scene connection)."""

    def __init__(self, line: int):
        self.line = line
        self.cond = ()
        self.args = ()


def class_named(ix, unit, text: str) -> str | None:
    """The class a type text names in a unit: a class of its function first, then by C++ lookup."""
    local = unit.local_types.get(strip_templates(clean(text)))
    return local or ix.cpp_class(text, unit.owner, unit.file)


def constructors(rs, unit, ref) -> list[int]:
    """`new T`, `T x(args)`, `make_unique<T>`: T's constructors."""
    key = class_named(rs.ix, unit, "::".join(p for p in (ref.scope, ref.name) if p))
    return rs.keep(rs.ix.members[key].get(ref.name, []), "func", unit.file) if key else []


def implicit_construction(rs) -> None:
    """A constructor runs its bases' constructors and its object members' constructors (C7)."""
    ix = rs.ix
    for unit in list(ix.units):
        cls = ix.classes.get(unit.owner)
        if cls is None or cls.lang != "cpp" or unit.name != cls.name or unit.decl_only:
            continue
        site = Site(unit.line)
        for base in ix.base_keys(cls):
            for gid in rs.keep(ix.members[base].get(ix.classes[base].name, []), "func", unit.file):
                rs.link(unit.gid, gid, "call", site, "exact")
        for gids in ix.members[cls.key].values():
            for member in (ix.units[g] for g in gids):
                held = member.kind == "field" and member.type_text and not set("*&") & set(member.type_text)
                key = ix.cpp_class(member.type_text, cls.key, member.file) if held else None
                for gid in rs.keep(ix.members[key].get(ix.classes[key].name, []), "func", unit.file) if key else []:
                    rs.link(unit.gid, gid, "call", site, "exact")


def implicit_destruction(rs) -> None:
    """A written destructor, after its body, runs what destroying its members and bases runs."""
    ix = rs.ix
    for unit in list(ix.units):
        cls = ix.classes.get(unit.owner)
        if cls is None or cls.lang != "cpp" or unit.name != "~" + cls.name or unit.decl_only:
            continue
        site = Site(unit.end_line)
        for gid in owned_destruction(rs, cls.key, unit.file, set()):
            rs.link(unit.gid, gid, "call", site, "exact")


def destruction_of(rs, key: str, file: str, seen: set) -> list[int]:
    """What destroying an object of a class runs: its written destructor, else the implicit one's work."""
    ix = rs.ix
    written = rs.keep(ix.members[key].get("~" + ix.classes[key].name, []), "func", file)
    return written or owned_destruction(rs, key, file, seen)


def owned_destruction(rs, key: str, file: str, seen: set) -> list[int]:
    """The destructors run for a class's members held by value (and what library owners among them own) and bases."""
    if key in seen:
        return []
    seen.add(key)
    ix, found = rs.ix, []
    for gids in ix.members[key].values():
        for member in (ix.units[g] for g in gids):
            if member.kind == "field" and not member.static and member.type_text:
                for owned in owned_classes(ix, member.type_text, key, member.file):
                    found += destruction_of(rs, owned, file, seen)
    for base in ix.base_keys(ix.classes[key]):
        found += destruction_of(rs, base, file, seen)
    return list(dict.fromkeys(found))


def owned_classes(ix, text: str, scope: str, file: str) -> list[str]:
    """The classes of the program a member of this type owns: the type itself held by value, or what a library owner
    (smart pointer, container, optional, pair, tuple, variant) holds; nothing through a raw pointer or a reference."""
    text = text.replace("const ", "").strip()
    if text.endswith(("*", "&")):
        return []
    head = strip_templates(text).split("::")[-1]
    if head in LIBRARY_OWNERS:
        return [k for arg in template_args(text) for k in owned_classes(ix, arg, scope, file)]
    key = ix.cpp_class(text, scope, file)
    return [key] if key else []


def destructors(rs, unit, ref) -> None:
    """An object's destructor at the end of its scope, or `delete` (C7): what destroying it runs."""
    ix = rs.ix
    if ref.base is not None:
        keys = rs.typer.receiver(ref.base, unit)[0]
    else:
        key = class_named(ix, unit, "::".join(p for p in (ref.scope, ref.name) if p))
        keys = [key] if key else []
    for key in keys:
        if ix.classes[key].lang != "cpp":
            continue
        for gid in destruction_of(rs, key, unit.file, set()):
            rs.link(unit.gid, gid, "call", ref, "exact")
