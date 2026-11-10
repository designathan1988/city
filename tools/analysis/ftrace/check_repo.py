"""Traces a feature in any repository and checks the trace properties (ftrace/check.py) there.

python tools/analysis/ftrace/check_repo.py <repo> --query "words" [--include dir ...] [--seed spec ...] [--show N]
Exits 1 when a property is violated, 2 when no seed is found."""
import argparse
import os
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))

from ftrace.check import violations  # noqa: E402
from ftrace.graph import Graph  # noqa: E402
from ftrace.index import build_index  # noqa: E402
from ftrace.report import render_text  # noqa: E402
from ftrace.seeds import select_by_query, select_by_specs  # noqa: E402
from ftrace.trace import trace  # noqa: E402


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('repo')
    parser.add_argument('--query')
    parser.add_argument('--seed', action='append', default=[])
    parser.add_argument('--include', action='append')
    parser.add_argument('--show', type=int, default=25)
    args = parser.parse_args()
    started = time.monotonic()
    graph = Graph(build_index(args.repo, cache_dir=os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(HERE))), 'dist', 'cache', 'ftrace'), include=args.include))
    built = time.monotonic()
    seeds = select_by_specs(graph, args.seed) if args.seed else select_by_query(graph, args.query)
    if not seeds:
        print('no seed found')
        return 2
    result = trace(graph, seeds)
    found = violations(graph, result, graph.root)
    print(render_text(graph, result, top=args.show))
    print(f'\n{len(graph.nodes)} units; graph {built - started:.1f}s, trace+check {time.monotonic() - built:.1f}s')
    print(f'PROPERTY VIOLATIONS: {len(found)}')
    for line in found[:args.show]:
        print('  ' + line)
    return 0 if not found else 1


if __name__ == '__main__':
    sys.exit(main())
