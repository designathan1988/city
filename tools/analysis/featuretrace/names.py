"""NAME (log/trace-rules.md, G6, G7): names carried as values. A text value names a method where a method name is
expected (`Callable(obj, name)`, `obj.call(name)`), and a class where the engine constructs by name
(`ClassDB.instantiate(name)`). Each function takes the typer that evaluates the trees."""
from __future__ import annotations

from .rules import CALLABLE_CONSTRUCTORS, NAME_CONSTRUCTORS

CLASS, FN, TEXT, ENGINE = "#", "<fn>", "<text>", "<engine>"


def callable_of(typer, callee, args, unit, depth: int) -> list[tuple] | None:
    """`Callable(obj, name)` (G7): a callable of the method of obj's class named by each text `name` can hold."""
    spec = CALLABLE_CONSTRUCTORS.get(callee[1]) if callee[0] == "name" else None
    if spec is None or max(spec) >= len(args) or typer.rs.function_targets(unit, callee[1]):
        return None
    out = []
    for name in texts(typer, args[spec[1]], unit, depth):
        for value, ctx in typer.evaluate(args[spec[0]], unit, depth):
            out += [v for v in typer.member(value, ctx, name, False, depth) if v[0].startswith(FN)]
    return out or [(ENGINE, unit)]


def texts(typer, tree, unit, depth: int = 0) -> list[str]:
    """The text values a tree can hold (names carried as strings, G7)."""
    return list(dict.fromkeys(value[len(TEXT):] for value, _ in typer.evaluate(tree, unit, depth)
                              if value.startswith(TEXT)))


def made_by_name(typer, callee, args, unit) -> list[tuple] | None:
    """The engine's constructors by name (G6): `ClassDB.instantiate(&"C")` is an object of class C."""
    if callee[0] != "member" or callee[1][0] != "name":
        return None
    index = NAME_CONSTRUCTORS.get((callee[1][1], callee[2]))
    if index is None or typer.named(callee[1][1], unit, 0) != [(ENGINE, unit)] or index >= len(args):
        return None
    keys = [(typer.ix.engine_class(n) if unit.lang == "gd" else None, n) for n in texts(typer, args[index], unit)]
    return [(CLASS + k, unit) if k else (n, unit) for k, n in keys] or [(ENGINE, unit)]
