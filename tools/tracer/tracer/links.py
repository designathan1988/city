"""Links of the program that lead nowhere, found in the whole of what the analysis read: state written and never read,
read and never written (nor given a first value); an event fired by a name no handler is registered for, a handler
registered for a name nothing fires; a key of data read that the data does not hold, a key the data holds that is
never read; a uniform of a shader nothing outside writes, one written and never read in the shader. (State read
before anything writes it in a sequence of starts is the evaluator's: `evaluate`.)

Each finding: what, where, and why it is said. "Never" is of the code read: code not read may still do it, and a
finding says so where it may.
"""
from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from .conditions import state_of
from .decide import symbols
from .languages import DATA_FORMATS
from .vocab import FIRES, FUNCTION, HANDED, OUTCALL, READS, SYMBOL, TEXT, UNIFORM, WRITE, WRITES, CHANGES

WRITTEN_NOT_READ = "state written, never read by the code read"
READ_NOT_WRITTEN = "state read, never written nor given a first value by the code read"
FIRED_NO_LISTENER = "fired by a name no handler is registered for"
LISTENER_NOT_FIRED = "a handler registered for a name nothing fires"
KEY_MISSING = "a key of data read that the data does not hold"
KEY_NOT_READ = "a key the data holds that the code never reads"
UNIFORM_NOT_WRITTEN = "a uniform nothing outside the shader writes"
UNIFORM_NOT_READ = "a uniform written and never read in the shader"
NEVER_REACHED = "code never reached: its conditions never hold together"
NEVER_HAPPENS = "an output that never happens: its conditions never hold together"


@dataclass
class Broken:
    kind: str
    where: str
    what: str
    note: str = ""


def broken(result) -> list:
    out = []
    out += states(result)
    out += events(result)
    out += data_keys(result)
    out += uniforms(result)
    out += never(result)
    return out


def never(result) -> list:
    """Code and outputs whose conditions no case makes hold together (in the inputs of their function)."""
    from .decide import satisfy
    if not result.cases:
        return []
    p, out = result.program, []
    for unit, file, line, guard in result.cases.blocks:
        if guard is not True and satisfy(guard) is None:
            out.append(Broken(NEVER_REACHED, f"{p.files[file].display}:{line}", p.label(unit)))
    for e in result.cases.effects:
        if e.kind == OUTCALL and e.guard is not True and satisfy(e.guard) is None:
            out.append(Broken(NEVER_HAPPENS, f"{p.files[e.file].display}:{e.line}", e.shown))
    return out


def states(result) -> list:
    p = result.program
    out = []
    first = (result.cases.first or {}) if result.cases else {}
    for d, units in (result.state_in or {}).items():
        hows = set(units.values())
        it = p.defs[d]
        where = f"{p.files[it.file].display}:{it.line}"
        if hows & {WRITES, CHANGES} and READS not in hows:
            writers = sorted(p.label(u) for u, how in units.items() if how in (WRITES, CHANGES))
            out.append(Broken(WRITTEN_NOT_READ, where, p.label(d), f"written by {', '.join(writers[:5])}"))
        elif READS in hows and not hows & {WRITES, CHANGES} and d not in first:
            readers = sorted(p.label(u) for u, how in units.items() if how == READS)
            out.append(Broken(READ_NOT_WRITTEN, where, p.label(d), f"read by {', '.join(readers[:5])}; code not "
                                                                       "read may set it"))
    return out


def events(result) -> list:
    """Names handed with a function (registration) and names given to a call that fires handlers somewhere (a
    firing): the ones on one side only."""
    p = result.program
    effects = result.cases.effects if result.cases else []
    firing_names = set()
    fired_lines = {(link.src, link.file, link.line) for links in result.out.values() for link in links
                   if link.kind == FIRES}
    registered, fired = {}, {}
    for e in effects:
        if e.kind != OUTCALL:
            continue
        texts = [a[1] for a in e.args if isinstance(a, tuple) and a and a[0] == TEXT and isinstance(a[1], str)]
        functions = [a for a in e.args if isinstance(a, tuple) and a and a[0] == SYMBOL and isinstance(a[1], int)
                     and p.defs[a[1]].kind == FUNCTION]
        where = f"{p.files[e.file].display}:{e.line}"
        if (e.unit, e.file, e.line) in fired_lines:
            firing_names.add(e.name)
        if texts and functions:
            for t in texts:
                registered.setdefault(t, []).append((where, functions[0][1]))
        elif texts:
            for t in texts:
                fired.setdefault((e.name, t), []).append(where)
    out = []
    for (name, t), wheres in sorted(fired.items()):
        if name in firing_names and t not in registered:
            out.append(Broken(FIRED_NO_LISTENER, wheres[0], f"{name}({t!r})", f"fired at {', '.join(wheres[:5])}"))
    fired_texts = {t for (name, t) in fired if name in firing_names}
    for t, rows in sorted(registered.items()):
        if t in fired_texts or not firing_names:
            continue
        for where, d in rows:
            others = [link for link in result.into.get(d, ()) if link.kind != HANDED]
            if not others:
                out.append(Broken(LISTENER_NOT_FIRED, where, f"{t!r} -> {p.label(d)}",
                                  "nothing read fires it by that name; code not read may"))
    return out


def data_keys(result) -> list:
    out = []
    if not result.cases:
        return out
    for where, path, keys in result.cases.keys_missing:
        out.append(Broken(KEY_MISSING, where, f"{Path(path).name}{''.join(f'[{k!r}]' for k in keys)}"))
    read = {}
    for path, keys in result.cases.keys_read:
        read.setdefault(path, set()).add(tuple(keys))
    for path in result.cases.data_files:
        try:
            held = DATA_FORMATS[Path(path).suffix.lower()](Path(path).read_text(encoding="utf-8"))
        except (OSError, ValueError, KeyError):
            continue
        seen = read.get(path, set())
        whole = {r for r in seen if not any(len(o) > len(r) and o[:len(r)] == r for o in seen)}
        for keys in leaves(held):
            if not any(keys[:len(r)] == r for r in whole):
                out.append(Broken(KEY_NOT_READ, Path(path).name, f"{Path(path).name}{''.join(f'[{k!r}]' for k in keys)}"))
    return out


def leaves(held, keys=()) -> list:
    if isinstance(held, dict):
        return [k for name, v in held.items() for k in leaves(v, keys + (name,))] or [keys]
    if isinstance(held, list):
        return [keys] if not held or not isinstance(held[0], (dict, list)) else \
            [k for i, v in enumerate(held) for k in leaves(v, keys + (i,))]
    return [keys]


def uniforms(result) -> list:
    out = []
    if not result.cases or not result.cases.uniforms:
        return out
    p = result.program
    written = {e.target[1] for e in result.cases.effects if e.kind == WRITE and isinstance(e.target, tuple)
               and e.target and e.target[0] == UNIFORM}
    read = set()
    for e in result.cases.effects:
        if p.files[e.file].lang.prepare:
            for x in (e.guard, e.value) + tuple(e.args):
                read |= {str(v).split(".")[-1] for v in symbols(x).values()}
    for name, files in sorted(result.cases.uniforms.items()):
        if name not in written:
            out.append(Broken(UNIFORM_NOT_WRITTEN, files[0], name, "no call read sets it by its name; code not read "
                                                                  "may"))
        elif name not in read:
            out.append(Broken(UNIFORM_NOT_READ, files[0], name))
    return out


def text_report(result, run=None) -> str:
    found = broken(result)
    if run is not None:
        found += [Broken("state read before anything wrote it (in the sequence evaluated)", where, what)
                  for where, what in run.reads_before_writes]
    lines = [f"Links that lead nowhere ({len(found)}):"]
    for b in found:
        lines.append(f"  {b.where}  {b.kind}: {b.what}" + (f"  ({b.note})" if b.note else ""))
    return "\n".join(lines)


def data(result, run=None) -> list:
    found = broken(result)
    if run is not None:
        found += [Broken("state read before anything wrote it (in the sequence evaluated)", where, what)
                  for where, what in run.reads_before_writes]
    return [{"what is found": b.kind, "where it is": b.where, "which one": b.what, "the note": b.note}
            for b in found]
