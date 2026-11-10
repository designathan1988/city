"""The tracer checked against a real run: every call seen on a sampled stack must be a link the tracer found.

On a stack (innermost frame first) a frame is the function running there; the frame above it holds the call that
entered it. Each frame is named (`native_frames`) and turned into the tracer's definitions. Two neighbouring frames of
the program, with only frames of library code between them (or none), are one call seen in the run: the outer one
calls the inner one, directly or through library code it handed something to. The tracer answers that call with a
chain of links of a kind that runs (`walk.RUNS`) from the caller to the callee whose functions in between were inlined
by the compiler: a function can only be inlined where its body is compiled, so each one in between must have its body
in the translation unit of one of the frames the call passed through (the object file a linker map names for each
frame; for a module named without a map, the caller's own file and what it includes). A call with no such chain is
reported by name. Frames of a module without names (the engine) between two frames of the program are a call through
the engine (a callback, a signal), checked the same way and counted apart.
"""
from __future__ import annotations

from collections import Counter, defaultdict, deque
from dataclasses import dataclass, field
from pathlib import Path

from .model import FUNCTION_KINDS
from .native_frames import LinkMap, has_symbols, qualified, symbolize
from .walk import RUNS

OBJECT_SUFFIXES = (".obj", ".o")


@dataclass
class Check:
    calls: Counter = field(default_factory=Counter)        # (caller, callee, through) -> samples
    verdict: dict = field(default_factory=dict)             # (caller, callee, through) -> links in the chain, or None
    observed: set = field(default_factory=set)              # program definitions seen running
    unknown: Counter = field(default_factory=Counter)       # names seen in named modules that are no definition read
    special: set = field(default_factory=set)               # of those, implicit constructors and destructors
    samples: int = 0
    named_frames: int = 0
    frames: int = 0


def frame_names(data: dict, maps: dict, symbolizer: str | None) -> dict[tuple, tuple | None]:
    """(module, offset looked up) -> (demangled name, object file), for the modules a map names or that keep their
    symbols. A return address is looked up one byte back: the call it returns from is in the caller even when it is
    the caller's last instruction."""
    tables = {name.lower(): LinkMap(Path(path)) for name, path in maps.items()}
    wanted = defaultdict(set)
    for _, frames in data["stacks"]:
        for depth, (module, offset) in enumerate(frames):
            if module is not None:
                wanted[module].add(offset - 1 if depth else offset)
    names = {}
    for module, offsets in wanted.items():
        table = tables.get(Path(module).name.lower())
        if table is not None:
            names.update({(module, o): table.name(o) for o in offsets})
        elif symbolizer and Path(module).is_file() and has_symbols(Path(module)):
            names.update({(module, o): n for o, n in symbolize(Path(module), sorted(offsets), symbolizer).items()})
    return names


class Definitions:
    """A demangled name -> the tracer's definitions it can be (a lambda: every lambda written at that nesting in the
    enclosing function); an object file -> the source file it was compiled from."""

    def __init__(self, ix):
        self.ix = ix
        self.lambdas = [u for u in ix.units if u.kind == "lambda"]
        self.memo, self.sources = {}, {}
        self.by_name = defaultdict(list)
        for path in ix.includes:
            self.by_name[Path(path).name].append(path)

    def of(self, demangled: str) -> tuple[int, ...]:
        if demangled not in self.memo:
            qual, closures = qualified(demangled)
            gids = [g for g in self.ix.by_qual.get(qual, []) if self.ix.units[g].kind in FUNCTION_KINDS]
            for _ in range(closures):
                quals = [self.ix.units[g].qual + ".<lambda:" for g in gids]
                gids = [u.gid for u in self.lambdas for q in quals
                        if u.qual.startswith(q) and ".<lambda:" not in u.qual[len(q):]]
            self.memo[demangled] = tuple(dict.fromkeys(gids))
        return self.memo[demangled]

    def source(self, obj: str) -> str | None:
        """The file read whose path ends with the object's path less its object suffix (the most components alike)."""
        if obj not in self.sources:
            stem = next((obj[:-len(s)] for s in OBJECT_SUFFIXES if obj.endswith(s)), obj)
            parts = Path(stem.replace("\\", "/")).parts
            best, score = None, 0
            for path in self.by_name.get(parts[-1] if parts else "", []):
                mine = Path(path).parts
                same = next((i for i in range(min(len(mine), len(parts))) if mine[-1 - i] != parts[-1 - i]),
                            min(len(mine), len(parts)))
                if same > score:
                    best, score = path, same
            self.sources[obj] = best
        return self.sources[obj]

    def unit_of(self, files: set[str]) -> frozenset:
        """Every file of the translation units of these source files."""
        return frozenset(f for path in files for f in self.ix.visible(path))

    def special(self, demangled: str) -> bool:
        """A constructor or destructor of a class of the program that the code does not write: the language defines
        it implicitly ([class.default.ctor], [class.copy.ctor], [class.dtor])."""
        scope, _, last = qualified(demangled)[0].rpartition("::")
        return scope in self.ix.classes and last.lstrip("~") == scope.rpartition("::")[2]


def chain(rs, callers: tuple, callees: tuple, inlinable: frozenset) -> int | None:
    """The fewest links that run leading from a caller to a callee through functions that leave no frame: inlined (the
    body in an `inlinable` file) or left by a jump (the next call in tail position). None when there is no such chain."""
    units, goal = rs.ix.units, set(callees)
    todo = deque((g, 0, True) for g in callers)      # (function, links so far, it has its own frame: any call goes on)
    seen = {(g, True) for g in callers}
    while todo:
        gid, steps, framed = todo.popleft()
        unit = units[gid]
        for edge in rs.out.get(gid, ()):
            if edge.kind not in RUNS or not (framed or unit.file in inlinable or edge.line in unit.tails):
                continue
            if edge.dst in goal:
                return steps + 1
            if (edge.dst, False) not in seen and not units[edge.dst].decl_only:
                seen.add((edge.dst, False))
                todo.append((edge.dst, steps + 1, False))
    return None


def check(rs, data: dict, names: dict) -> Check:
    out, defs = Check(), Definitions(rs.ix)
    memo = {}
    for count, frames in data["stacks"]:
        out.samples += count
        program, cuts, objects = [], [], {}
        for depth, (module, offset) in enumerate(frames):
            out.frames += 1
            if module is None:
                cuts.append(depth)
                continue
            named = names.get((module, offset - 1 if depth else offset))
            if named is None:
                continue
            out.named_frames += 1
            name, obj = named
            objects[depth] = obj
            gids = defs.of(name)
            if gids:
                program.append((depth, gids, name))
                out.observed.update(gids)
            else:
                out.unknown[name] += count
        for (inner, callees, callee), (outer, callers, caller) in zip(program, program[1:]):
            if any(inner < c < outer for c in cuts):
                continue
            between = range(inner + 1, outer)
            through = "engine" if any(d not in objects for d in between) else "library" if between else "direct"
            files = {defs.source(objects[d]) for d in range(inner + 1, outer + 1) if objects.get(d)} - {None}
            if not files:
                files = {rs.ix.units[g].file for g in callers}
            key = (caller, callee, through)
            out.calls[key] += count
            memo_key = (key, frozenset(files))
            if memo_key not in memo:
                memo[memo_key] = chain(rs, callers, callees, defs.unit_of(files))
            if out.verdict.get(key) is None:
                out.verdict[key] = memo[memo_key]
    out.special = {n for n in out.unknown if defs.special(n)}
    return out


def report(result: Check) -> str:
    lines = [f"{result.samples} samples, {result.frames} frames, {result.named_frames} named",
             f"{len(result.observed)} definitions of the program seen running",
             f"{len(result.calls)} distinct calls between frames of the program"]
    by = defaultdict(Counter)
    for key, links in result.verdict.items():
        by[key[2]]["missing" if links is None else ("link" if links == 1 else "chain")] += 1
    for through in ("direct", "library", "engine"):
        c = by.get(through, Counter())
        lines.append(f"  {through:8s} {sum(c.values()):6d} calls: {c['link']} one link, {c['chain']} a chain through "
                     f"inlined functions, {c['missing']} with no link")
    missing = sorted((k for k, v in result.verdict.items() if v is None), key=lambda k: -result.calls[k])
    if missing:
        lines.append("")
        lines.append("Calls seen in the run that the tracer has no link for (caller -> callee, through, samples):")
        for caller, callee, through in missing:
            lines.append(f"  {caller}\n    -> {callee}   [{through}, {result.calls[(caller, callee, through)]}]")
    special = sorted(result.special)
    if special:
        lines.append("")
        lines.append("Constructors and destructors the language defines implicitly (no definition written):")
        lines += [f"  {name}" for name in special]
    groups = Counter()
    for name in result.unknown:
        if name not in special:
            groups[qualified(name)[0].split("::")[0] or "(global)"] += 1
    lines.append("")
    lines.append("Other named frames that are no definition the tracer read, by outermost scope (distinct functions):")
    lines.append("  " + ", ".join(f"{scope} {n}" for scope, n in groups.most_common()))
    return "\n".join(lines) + "\n"
