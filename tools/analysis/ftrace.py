"""Entry of the feature tracer: traces a feature end to end in any source tree, from explicit seeds or a free-text query
(who triggers it, what it runs and mutates, who reads what it mutates). It reads the tree on disk, as it is now.
Use: python tools/analysis/ftrace.py <tree> --query "words" | --seed "Class::method" | --seed path/file | --seed file:line
     [--json OUT] [--top N]; its checks on a repository: python tools/analysis/ftrace/check_repo.py."""
import argparse
import json
import os
import sys
import time

from ftrace.graph import Graph
from ftrace.index import build_index
from ftrace.report import render_text, to_json
from ftrace.seeds import select_by_query, select_by_specs
from ftrace.trace import trace

# Outputs live under the repository's dist/ (outside git), never beside the sources.
DEFAULT_CACHE = os.path.join(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))), 'dist', 'cache', 'ftrace')


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('root', help='source tree to read')
    parser.add_argument('--seed', action='append', default=[],
                        help="unit to trace: 'name', 'Scope::name', 'Scope.name', 'path/to/file', 'file:line'")
    parser.add_argument('--query', help='free-text description of the feature; seeds are the units above the gap')
    parser.add_argument('--json', help='also write the complete trace as JSON to this file')
    parser.add_argument('--top', type=int, help='print at most this many entry points and reached units (default: all)')
    parser.add_argument('--cache', default=DEFAULT_CACHE, help='content-keyed cache folder')
    parser.add_argument('--no-cache', action='store_true')
    parser.add_argument('--workers', type=int, help='worker processes (default: one per CPU)')
    args = parser.parse_args(argv)
    if not args.seed and not args.query:
        parser.error('give --seed or --query')

    started = time.monotonic()
    index = build_index(args.root, cache_dir=None if args.no_cache else args.cache, workers=args.workers)
    graph = Graph(index)
    indexed = time.monotonic()
    seeds = select_by_specs(graph, args.seed) if args.seed else select_by_query(graph, args.query)
    if not seeds:
        print('no unit matches the seed or the query', file=sys.stderr)
        return 2
    result = trace(graph, seeds)
    print(render_text(graph, result, top=args.top))
    if args.json:
        with open(args.json, 'w', encoding='utf-8') as handle:
            json.dump(to_json(graph, result), handle, indent=1)
    parsers = sorted({f['parser'] for f in index.facts})
    print(f'\n{len(index.facts)} files ({", ".join(parsers)}), {len(graph.nodes)} units; '
          f'index {indexed - started:.2f}s, trace {time.monotonic() - indexed:.2f}s')
    for name, reason in index.unusable:
        print(f'grammar not usable: {name}: {reason}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
