"""C++ [dcl.ambig.res], [stmt.ambig]: `T x(a, b);` declares a function when each of a, b is a parameter declaration
(each written type names a type), else an object direct-initialized with a and b read as expressions.

The grammar reads the declaration as a function; the same parentheses read by the expression grammar (`as_arguments`:
the text parsed again as a call's argument list, its lines kept) are the object's initializers. The reader keeps both
readings: the function (at namespace scope, a unit, `Unit.objects` naming what decides; in a block, nothing) and the
object's uses (its construction, what its initializers read and call), marked with the names written where each
parameter's type goes (`Ref.object_of`). Once every file is indexed, `decide` looks the names up: one that is a value
of the program (a variable, a constant, a field, an enumerator, a function, or in a block a local) makes it an object;
all naming types (a built-in type is one) make it a function; a name the program does not declare (a library's) leaves
both readings, so no use is lost.
"""
from __future__ import annotations

from . import ir_cpp, scopes
from .cpp_names import strip_templates
from .source import Source, field_of, named_children

VALUE_KINDS = frozenset({"var", "field", "const", "enumerator", "func", "method"})
TYPE_KINDS = frozenset({"class", "enum"})
CALLER = b"void __f() { __x"          # the text read as a call's arguments: `__x(<the parameter list>);`
VALUE, TYPE = "value", "type"


class Reparsed:
    """The reader over the parameter list's text read again: its own source, the reader's scopes, facts and units."""

    def __init__(self, r, src: Source):
        self._r = r
        self.src = src

    def __getattr__(self, name):
        return getattr(self._r, name)

    def ir(self, node, unit) -> tuple:
        return scopes.scoped_tree(self, ir_cpp.build(self.src, node, lambda n: self._r.lambda_id(n, unit)))


def as_arguments(r, function_declarator):
    """The parameter list read by the expression grammar as a call's arguments: (the reader over that text, the
    argument nodes); None when the text is no argument list (a qualifier, a declared name, a default: only
    parameters) or there is no parameter (`T x();` is a function)."""
    params = field_of(function_declarator, "parameters")
    if params is None or not named_children(params):
        return None
    data = b"\n" * (r.src.line(params) - 1) + CALLER + r.src.data[params.start_byte:params.end_byte] + b"; }"
    src = Source(r.src.path, r.src.display, data)
    root = src.parse("cpp")
    if root.has_error:
        return None
    todo = [root]
    while todo:
        node = todo.pop()
        if node.type == "call_expression":
            args = field_of(node, "arguments")
            return (Reparsed(r, src), named_children(args)) if args is not None else None
        todo += reversed(named_children(node))
    return None


def deciding_names(r, function_declarator) -> list[str]:
    """The names written where each parameter's type goes, when the parameters are also an argument list; a built-in
    type is left out (it names a type). [] when the declaration can only be a function, or a template parameter is
    one of them (a type)."""
    if as_arguments(r, function_declarator) is None:
        return []
    names = []
    for p in named_children(field_of(function_declarator, "parameters")):
        typ = field_of(p, "type")
        if typ is None:
            return []
        if typ.type not in ("primitive_type", "sized_type_specifier"):
            names.append(strip_templates(r.src.text(typ)))
    return [] if any(n in r.template for n in names) else names


def mark(unit, start: int, names: list) -> None:
    """The uses recorded from `start` on are the object reading's."""
    for ref in unit.refs[start:]:
        ref.object_of = tuple(names)


def classify(ix, name: str, prefix: str, file: str) -> str | None:
    """A name seen from a scope: a value or a type of the program, or None when the program does not declare it."""
    for scope in ix.scopes(prefix) + ix.tu_usings(file):
        key = f"{scope}::{name}" if scope else name
        if key in ix.aliases:                   # a type alias, or an unscoped enumerator seen in its enclosing scope
            target = ix.aliases[key]
            return VALUE if any(ix.units[g].kind == "enumerator" for g in ix.by_qual.get(target, ())) else TYPE
        if key in ix.classes:
            return TYPE
        kinds = {ix.units[g].kind for g in ix.by_qual.get(key, ())}
        if kinds & VALUE_KINDS:
            return VALUE
        if kinds & TYPE_KINDS:
            return TYPE
    return None


def decide(ix) -> None:
    """Every declaration with two readings takes the one the names' lookup gives (the module's rule)."""
    for unit in ix.units:
        if unit.lang != "cpp" or not unit.objects and not any(ref.object_of for ref in unit.refs):
            continue
        if unit.objects:
            kinds = [classify(ix, n, unit.owner, unit.file) for n in unit.objects]
            if VALUE in kinds:
                become_object(ix, unit)
            unit.refs = [ref for ref in unit.refs if not ref.object_of or set(kinds) != {TYPE}]
        else:
            unit.refs = [ref for ref in unit.refs if not ref.object_of or any(
                n in unit.types or n in unit.lambdas or classify(ix, n, unit.owner, unit.file) != TYPE
                for n in ref.object_of)]
        for ref in unit.refs:
            ref.object_of = ()


def become_object(ix, unit) -> None:
    """A namespace-scope `T x(a, b);` read as an object: a variable (a static member's definition: a field) of type T,
    with none of a function's parameters."""
    unit.kind = "field" if unit.owner in ix.classes else "var"
    unit.decl_only = unit.virtual = unit.variadic = False
    unit.params, unit.types, unit.required, unit.objects = [], {}, -1, []
