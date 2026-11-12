"""The acceptance of the new core against the current one (log/trace-core-plan.md, Task 7): the same program read by
both — the current core as the tool runs it (`program.load`), the new one with the declaration files its TypeScript
chooses, solved by the engine — and their links compared by what they join (the two definitions' qualified names) and
what the walk does with them: a link that runs its target (`walk.RUNS`), a write, a read, any other. A link of the new
core into a declaration file has no counterpart in the current core, which reads none: counted apart.

    python -m featuretrace.accept <root> [<root> ...] [--list N] [--out report.txt]

Prints, per group, how many links both cores find, how many only the current one and how many only the new one, then
the first N of each side with where they are written; every line of progress goes to stderr as it comes.
"""
from __future__ import annotations

import argparse
import sys
import tempfile
import time
from dataclasses import dataclass, field
from pathlib import Path

from . import engine_result, program
from .vocab import READ, WRITE
from .walk import RUNS

RUN, OTHER = "run", "other"
GROUPS = (RUN, WRITE, READ, OTHER)
CACHE = Path(tempfile.gettempdir()) / "feature-trace-cache"
LISTED = 40                      # how many links of each side and group the report lists unless told


@dataclass
class Found:
    """What the two cores found: per group, the links both have and those only one has, each a pair of qualified
    names; where each link is written (its file, line and kind); the new core's links into declarations; the seconds
    each core took and what each read."""
    both: dict = field(default_factory=dict)
    only_current: dict = field(default_factory=dict)
    only_new: dict = field(default_factory=dict)
    declared: dict = field(default_factory=dict)
    where: dict = field(default_factory=dict)
    seconds: dict = field(default_factory=dict)
    sizes: dict = field(default_factory=dict)


def group_of(kind: str) -> str:
    return RUN if kind in RUNS else kind if kind in (WRITE, READ) else OTHER


def links(ix, edges, where: dict, program_files: set | None = None) -> tuple[dict, dict]:
    """The links by group, as pairs of qualified names; those whose target is outside `program_files` apart."""
    inside, outside = {g: set() for g in GROUPS}, {g: set() for g in GROUPS}
    for edge in edges:
        src, dst = ix.units[edge.src], ix.units[edge.dst]
        pair = (src.qual, dst.qual)
        apart = program_files is not None and (dst.file not in program_files or src.file not in program_files)
        (outside if apart else inside)[group_of(edge.kind)].add(pair)
        where.setdefault(pair, (edge.file or src.file, edge.line, edge.kind))
    return inside, outside


def compare(roots: list[Path], base: Path, cache: Path | None, progress=None) -> Found:
    """Both cores over the same roots, their links compared."""
    progress = progress or (lambda message: None)
    found = Found()
    began = time.perf_counter()
    ix, rs, files, _ = program.load(roots, base, cache, progress)
    found.seconds["current"] = time.perf_counter() - began
    current, _ = links(ix, rs.edges, found.where)
    program_files = {unit.file for unit in ix.units}
    found.sizes["current"] = (files, len(ix.units), len(rs.edges))
    progress(f"the current core: {len(rs.edges)} links in {found.seconds['current']:.1f} s")
    del rs
    began = time.perf_counter()
    declared_ix, files, _ = program.read(roots, base, cache, progress, declarations=True)
    result = program.deep(lambda: engine_result.analyse(declared_ix, progress))
    found.seconds["new"] = time.perf_counter() - began
    new, found.declared = links(declared_ix, result.edges, found.where, program_files)
    found.sizes["new"] = (files, len(declared_ix.units), len(result.edges))
    progress(f"the new core: {len(result.edges)} links in {found.seconds['new']:.1f} s")
    for group in GROUPS:
        found.both[group] = current[group] & new[group]
        found.only_current[group] = current[group] - new[group]
        found.only_new[group] = new[group] - current[group]
    return found


def text(found: Found, listed: int = LISTED) -> str:
    lines = []
    for name in ("current", "new"):
        files, units, count = found.sizes[name]
        lines.append(f"the {name} core: {files} files, {units} definitions, {count} links, "
                     f"{found.seconds[name]:.1f} s")
    for group in GROUPS:
        lines.append(f"{group}: both {len(found.both[group])}, only the current core "
                     f"{len(found.only_current[group])}, only the new core {len(found.only_new[group])}"
                     f" (and {len(found.declared[group])} into declarations)")
    for title, side in (("only the current core", found.only_current), ("only the new core", found.only_new)):
        for group in GROUPS:
            rows = sorted(side[group], key=lambda pair: (found.where[pair], pair))
            if not rows:
                continue
            lines.append(f"\n{title}, {group}: {len(rows)}" + (f" (the first {listed})" if len(rows) > listed else ""))
            for pair in rows[:listed]:
                file, line, kind = found.where[pair]
                lines.append(f"  {file}:{line}  {kind}  {pair[0]} -> {pair[1]}")
    return "\n".join(lines) + "\n"


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("root", nargs="+", type=Path, help="a folder or file to read")
    parser.add_argument("--list", type=int, default=LISTED, help="how many links of each side and group to list")
    parser.add_argument("--out", type=Path, help="write the report here as well")
    parser.add_argument("--no-cache", action="store_true", help="read every file again")
    args = parser.parse_args(argv)
    roots, base, missing = program.rooted(args.root)
    if missing:
        print(f"no such root: {', '.join(missing)}", file=sys.stderr)
        return 1
    progress = program.teller(time.perf_counter())
    report = text(compare(roots, base, None if args.no_cache else CACHE, progress), args.list)
    sys.stdout.reconfigure(encoding="utf-8")
    print(report, end="")
    if args.out is not None:
        args.out.parent.mkdir(parents=True, exist_ok=True)
        args.out.write_bytes(report.encode())
    return 0


if __name__ == "__main__":
    sys.exit(main())
