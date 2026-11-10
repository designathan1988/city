"""Seed ranking: a unit's name document is its qualified name, so the container tells same-named methods apart."""
import unittest

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools/analysis: the ftrace package

from ftrace.extract import extract_source  # noqa: E402
from ftrace.grammars import load_grammars  # noqa: E402
from ftrace.graph import Graph  # noqa: E402
from ftrace.index import Index  # noqa: E402
from ftrace.seeds import select_by_query  # noqa: E402

GRAMMARS, _ = load_grammars()
TS = GRAMMARS['typescript/typescript']

SOURCES = {
    'lib/commands/config.js': (b'import { lookup, all } from "../lookup";\n'
                               b'class Config { get(key) { return lookup(key); } list() { return all(); } }\n'),
    'lib/commands/owner.js': (b'import { lookup } from "../lookup";\n'
                              b'class Owner { get(key) { return lookup(key); } add(name) { return lookup(name); } }\n'),
    'lib/lookup.js': b'function lookup(key) { return table[key]; }\nfunction all() { return table; }\n',
}


def graph_of(sources):
    facts = [extract_source(path, data, TS.language, TS.name) for path, data in sorted(sources.items())]
    return Graph(Index('.', facts, {}, []))


class QualifiedNames(unittest.TestCase):
    def test_the_container_name_picks_the_method(self):
        graph = graph_of(SOURCES)
        seeds = select_by_query(graph, 'config get')
        self.assertEqual([graph.label(s) for s in seeds], [graph.label(graph.find('Config::get')[0])])

    def test_word_order_tells_a_method_from_a_function_with_the_same_words(self):
        sources = dict(SOURCES)
        sources['lib/defaults.js'] = (b'import { lookup } from "./lookup";\nconst config = {};\n'
                                      b'function getConfig(key) { return config[key] || lookup(key, config); }\n')
        graph = graph_of(sources)
        seeds = select_by_query(graph, 'config get')
        self.assertEqual([graph.label(s) for s in seeds], [graph.label(graph.find('Config::get')[0])])

    def test_behaviour_outranks_a_constant_of_the_same_name(self):
        graph = graph_of({
            'lib/tar.js': b'const PREFIX = Symbol("prefix");\nfunction pad(x) { return x[PREFIX]; }\n',
            'lib/commands/prefix.js': b'import { lookup } from "../lookup";\nclass Prefix { exec(args) { return lookup(args); } }\n',
            'lib/lookup.js': b'function lookup(key) { return key; }\n',
        })
        seeds = {graph.nodes[s].name for s in select_by_query(graph, 'prefix')}
        self.assertTrue(seeds)
        self.assertLessEqual(seeds, {'Prefix', 'exec'})


if __name__ == '__main__':
    unittest.main()
