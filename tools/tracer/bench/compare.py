"""The tracer measured against a compiler's answer (an oracle file of `bench/oracle`), call by call.

    python bench/compare.py <oracle.jsonl> <root>... [--base DIR] [--json OUT]

For every call the oracle resolves into the project's own code, whether the tracer links that call to the
declaration the compiler names or to a function of its name (its definition in another unit, an override, an
implementation: recall), and of the links the tracer writes at those calls, how many do (precision). A call is
matched by where its expression ends (chained calls start at the same place). For every call the oracle says runs
the platform's or a package's code, how many links the tracer still writes there found by name alone (noise),
and how many exact ones (an object of the code reaching where a platform type is declared). Each figure is
given for exact links alone and for every link.

Not part of the tracer: it reads the tracer's links as they are made and compares them.
"""
from __future__ import annotations

import argparse
import collections
import json
import sys
import time
from pathlib import Path

HOME = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(HOME))
sys.setrecursionlimit(100000)

from tracer import analyse  # noqa: E402
from tracer.calls import Calls  # noqa: E402
from tracer.vocab import CALL, EXACT, NEW  # noqa: E402


def links_by_call(roots, base):
    """{(file, line): {(target file, target line, target name, how)}}, every call and `new` link written."""
    seen = collections.defaultdict(set)
    original = Calls.link

    def link(self, src, dst, kind, how, f, node):
        original(self, src, dst, kind, how, f, node)
        if kind in (CALL, NEW):
            seen[(f.index, node.end_point[0] + 1, node.end_point[1])].add((src, dst, kind, how))
    Calls.link = link
    try:
        began = time.process_time()
        result = analyse(roots, base)
        seconds = time.process_time() - began
    finally:
        Calls.link = original
    p = result.program
    kept = {(link.src, link.dst, link.kind): link.how for links in result.out.values() for link in links}
    out = collections.defaultdict(set)
    for (fi, line, col), found in seen.items():
        for src, dst, kind, how in found:
            if (src, dst, kind) not in kept:         # a link the tracer took back (an ambiguous guess)
                continue
            how = kept[(src, dst, kind)] if how != kept[(src, dst, kind)] and how != EXACT else how
            d = p.defs[dst]
            out[(p.files[fi].display, line, col)].add((p.files[d.file].display, d.line, p.name_of(d), how))
    return out, result, seconds


def measure(rows, links):
    figures = {}
    for mode, keep in (("exact", lambda how: how == EXACT), ("all", lambda how: True)):
        own = hit = written = right = noise = outside_calls = 0
        missed = collections.Counter()
        wrong = collections.Counter()
        indirect = beyond = 0
        for row in rows:
            found = {one for one in links.get((row["file"], *row["end"]), ()) if keep(one[3])}
            if row.get("indirect"):              # the compiler names a function type: no function to compare with
                indirect += 1
                continue
            if row["targets"]:
                own += 1
                spots = {(t["file"], t["line"]) for t in row["targets"]}
                names = {t["name"] for t in row["targets"]}
                # the declaration the compiler names, or its definition elsewhere (the compiler of one unit may
                # only see the declaration), or what implements it: the same name at the very same call
                if any((file, line) in spots or name in names for file, line, name, _ in found):
                    hit += 1
                else:
                    missed[row["callee"].split("(")[0][-40:]] += 1
                written += len(found)
                good = [one for one in found if (one[0], one[1]) in spots or one[2] in names]
                right += len(good)
                for one in found:
                    if one not in good:
                        wrong[one[2]] += 1
            elif row.get("outside"):
                outside_calls += 1
                noise += sum(1 for one in found if one[3] != EXACT)
                beyond += sum(1 for one in found if one[3] == EXACT)
        figures[mode] = {
            "calls through a function type (not compared)": indirect,
            "calls into own code": own,
            "recall": round(hit / own, 4) if own else None,
            "links at those calls": written,
            "precision": round(right / written, 4) if written else None,
            "calls into the platform": outside_calls,
            "links at platform calls (noise)": noise,
            "exact links at platform calls (an object of the code where a platform type is declared)": beyond,
            "most missed callees": missed.most_common(15),
            "most wrong targets": wrong.most_common(15),
        }
    return figures


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("oracle")
    parser.add_argument("roots", nargs="+")
    parser.add_argument("--base", help="the folder the oracle's paths are written from (default: the first root)")
    parser.add_argument("--json", help="write the figures to this file")
    args = parser.parse_args(argv)
    rows = [json.loads(line) for line in Path(args.oracle).read_text(encoding="utf-8").splitlines() if line.strip()]
    links, result, seconds = links_by_call(args.roots, args.base or args.roots[0])
    figures = measure(rows, links)
    figures["seconds"] = round(seconds, 1)
    figures["links"] = sum(len(found) for found in result.out.values())
    text = json.dumps(figures, indent=1, ensure_ascii=False)
    if args.json:
        Path(args.json).write_text(text + "\n", encoding="utf-8")
    print(text)
    return 0


if __name__ == "__main__":
    sys.exit(main())
