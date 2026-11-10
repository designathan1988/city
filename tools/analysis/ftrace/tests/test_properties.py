"""The trace properties (ftrace/check.py) hold on the polyglot fixture, from a seed and from a query; and the
construct roles are given by the language, not by names."""
import os
import unittest

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools/analysis: the ftrace package

from ftrace.check import violations  # noqa: E402
from ftrace.extract import extract_source  # noqa: E402
from ftrace.grammars import load_grammars  # noqa: E402
from ftrace.graph import Graph  # noqa: E402
from ftrace.index import Index, build_index  # noqa: E402
from ftrace.seeds import select_by_query  # noqa: E402
from ftrace.trace import trace  # noqa: E402

ROOT = os.path.join(os.path.dirname(__file__), 'fixtures', 'notes_app')
GRAMMARS, _ = load_grammars()


def roles(source, language, unit_name):
    """Roles of plain names (tokens) and of each chain's last name (its members resolve through types)."""
    facts = extract_source('x', source, GRAMMARS[language].language, language)
    unit = [u for u in facts['units'] if u['name'] == unit_name][0]
    return {(r, t) for r, t, _ in unit['tokens']} | {(chain[1], chain[0]) for chain in unit['chains']}


class ConstructRoles(unittest.TestCase):
    def test_cpp_call_write_read(self):
        got = roles(b'void f(Obj& o) { o.items.push_back(1); total += 2; count++; use(level); }', 'cpp', 'f')
        self.assertLessEqual({('call', ('o', 'items', 'push_back')), ('read', 'o'), ('write', 'total'),
                              ('write', 'count'), ('call', 'use'), ('read', 'level')}, got)

    def test_gdscript_call_write_read(self):
        got = roles(b'func f():\n\tstate.notes = limit\n\temit_signal("x")\n', 'gdscript', 'f')
        self.assertLessEqual({('write', ('state', 'notes')), ('read', 'state'), ('read', 'limit'), ('call', 'emit_signal')}, got)

    def test_type_arguments_and_subscripts_are_not_calls(self):
        got = roles(b'void f(Map<int, T> &m) { auto b = std::make_shared<Batch>(); v[i] = 1; }', 'cpp', 'f')
        self.assertIn(('call', ('std', 'make_shared')), got)
        self.assertFalse({('call', 'Map'), ('call', 'Batch'), ('call', 'T')} & got)
        self.assertIn(('write', 'v'), got)
        self.assertNotIn(('write', 'i'), got)
        got = roles(b'func f():\n\tsnap[key] = g(snap[key])\n', 'gdscript', 'f')
        self.assertNotIn(('call', 'snap'), got)
        self.assertIn(('write', 'snap'), got)

    def test_a_return_type_arrow_is_not_member_access(self):
        got = roles(b'func f(x) -> void:\n\tif x:\n\t\tg(x.y)\n', 'gdscript', 'f')
        self.assertLessEqual({('call', 'g'), ('read', 'x'), ('read', ('x', 'y'))}, got)
        got = roles(b'auto f(Obj o) -> int { return use(o); }', 'cpp', 'f')
        self.assertIn(('call', 'use'), got)

    def test_only_a_bare_reference_is_registered(self):
        source = b'void f() { each(1, [&](int id) { inner(id); }); hook(handler); bind(a + b); }'
        facts = extract_source('x', source, GRAMMARS['cpp'].language, 'cpp')
        regs = [u for u in facts['units'] if u['name'] == 'f'][0]['regs']
        passed = {receiver[-1]: set(names) for receiver, names, _ in regs}
        self.assertEqual(passed.get('hook'), {'handler'})
        self.assertNotIn('each', passed)  # a closure is the caller's own code, not a name handed over
        self.assertNotIn('bind', passed)  # a + b computes a value

    def test_typescript_import_is_a_dependency(self):
        facts = extract_source('x.ts', b'import { helper } from "./util";\n', GRAMMARS['typescript/typescript'].language, 'ts')
        self.assertIn(('dep', 'helper'), {(r, t) for r, t, _ in facts['units'][0]['tokens']})


TOOL_SOURCES = {
    'panel.gd': b'class_name Panel\nsignal picked(kind)\nfunc _on_tab(kind):\n\tpicked.emit(kind)\n',
    'hud.gd': (b'var panel: Panel\nvar tools: Controller\nfunc _ready():\n\tpanel.picked.connect(_on_picked)\n'
               b'func _on_picked(kind):\n\ttools.choose(kind)\n'),
    'controller.gd': (b'class_name Controller\nvar active: ToolBase\nfunc choose(kind):\n\tactive = RoadTool.new()\n'
                      b'func _unhandled_input(event):\n\tactive.fire()\n'),
    'tool_base.gd': b'class_name ToolBase\nfunc fire():\n\tpass\n',
    'road_tool.gd': b'class_name RoadTool\nextends ToolBase\nfunc fire():\n\tbuild()\nfunc build():\n\tpass\n',
    'erase_tool.gd': b'class_name EraseTool\nextends ToolBase\nfunc fire():\n\tpass\n',
}


class LocalsAndImports(unittest.TestCase):
    """Names declared in nested blocks, clauses and loops are locals; an import defines no name."""
    def unit(self, source, language, name):
        facts = extract_source('x', source, GRAMMARS[language].language, language)
        return [u for u in facts['units'] if u['name'] == name]

    def test_cpp_local_inside_an_if_block(self):
        f = self.unit(b'int out;\nvoid f(int a) { if (a) { int out = 1; use(out); } else { int e = 2; } }', 'cpp', 'f')[0]
        self.assertLessEqual({'out', 'e'}, f['locals'])

    def test_gdscript_locals_in_elif_and_loop(self):
        f = self.unit(b'func f(a, b, xs):\n\tif a:\n\t\tpass\n\telif b:\n\t\tvar e = 1\n\tfor k in xs:\n\t\tg(k)\n',
                      'gdscript', 'f')[0]
        self.assertLessEqual({'e', 'k'}, f['locals'])

    def test_a_function_returning_a_reference_is_callable(self):
        units = extract_source('x', b'std::vector<T> &sort(std::vector<T> &v) { return v; }\nint *p(int a) { return 0; }\n',
                               GRAMMARS['cpp'].language, 'cpp')['units']
        kinds = {u['name']: u['kind'] for u in units}
        self.assertEqual((kinds.get('sort'), kinds.get('p')), ('callable', 'callable'))
        self.assertNotIn('v', kinds)  # `return v;` is a statement of the body, not a member

    def test_every_declarator_and_loop_or_catch_name_is_local(self):
        f = self.unit(b'void f(V2 a, V2 b) { V2 r = a, s = b; for (int i = 0; i < 3; i++) use(i);'
                      b' try { go(); } catch (const Err &e) { use(e); } }', 'cpp', 'f')[0]
        self.assertLessEqual({'r', 's', 'i', 'e'}, f['locals'])

    def test_every_member_declarator_is_a_unit(self):
        names = {u['name'] for u in extract_source('x', b'struct Packer { int nx = 0, ny = 0; };',
                                                   GRAMMARS['cpp'].language, 'cpp')['units']}
        self.assertLessEqual({'nx', 'ny'}, names)

    def test_a_using_declaration_is_not_a_unit(self):
        self.assertEqual(self.unit(b'using godot::Vector2;\nvoid f() {}\n', 'cpp', 'Vector2'), [])
        self.assertTrue(self.unit(b'using Vec = std::vector<int>;\n', 'cpp', 'Vec'))

    def test_a_plain_call_hands_its_own_callee_as_receiver(self):
        f = self.unit(b'func f():\n\tvar z = 1\n\tconnect_all(handler)\n\tother(z)\n', 'gdscript', 'f')[0]
        self.assertIn((('connect_all',), ('handler',)), {(c, p) for c, p, _ in f['regs']})


def mixed_graph(sources):
    facts = []
    for path, data in sorted(sources.items()):
        language = {'.gd': 'gdscript', '.h': 'cpp', '.cpp': 'cpp'}[path[path.rindex('.'):]]
        facts.append(extract_source(path, data, GRAMMARS[language].language, language))
    return Graph(Index('.', facts, {}, []))


class LanguageBoundary(unittest.TestCase):
    """A plain name never reaches another language's units; a class's bases come from its inheritance construct."""
    @classmethod
    def setUpClass(cls):
        cls.graph = mixed_graph({
            'ui/panel.gd': b'extends Control\nvar floor_count = floor(2.5)\nfunc show_it():\n\tControl.new()\n',
            'sim/types.h': b'namespace city {\nstruct Control { int lanes; };\nstruct floor { int n; };\n'
                           b'struct Base { void f(); };\nstruct Lane : Base { int width = 1; };\n}\n',
            'sim/types.cpp': b'#include "types.h"\nnamespace city { void Base::f() {} }\n',
        })

    def node(self, name, path):
        return [n for n in self.graph.nodes if n.name == name and n.path == path][0]

    def test_gdscript_names_do_not_resolve_into_cpp(self):
        for unit in self.graph.nodes:
            if unit.path.endswith('.gd'):
                targets = {self.graph.nodes[t].path for t in self.graph.out[unit.id]}
                self.assertFalse({p for p in targets if not p.endswith('.gd')}, unit.name)

    def test_a_file_level_read_is_not_a_base(self):
        self.assertEqual(self.graph.scopes.bases(('file', 'ui/panel.gd')), [])

    def test_the_inheritance_construct_gives_the_base(self):
        self.assertIn(('scope', ('city', 'Base')), self.graph.scopes.bases(('scope', ('city', 'Lane'))))

    def test_a_class_member_is_reached_by_plain_name_only_from_inside_its_class(self):
        graph = mixed_graph({
            'core/rng.h': b'namespace city {\nstruct Rng { int s = 0; void next() { s += 1; } };\nint helper();\n}\n',
            'core/use.cpp': b'#include "rng.h"\nnamespace city {\nint helper() { return 1; }\n'
                            b'void f() { s = 1; helper(); }\n}\n',
        })
        field = [n.id for n in graph.nodes if n.name == 's'][0]
        f = [n.id for n in graph.nodes if n.name == 'f'][0]
        nxt = [n.id for n in graph.nodes if n.name == 'next'][0]
        self.assertNotIn(field, graph.out[f])
        self.assertIn(field, graph.out[nxt])
        self.assertIn('helper', {graph.nodes[t].name for t in graph.out[f]})

    def test_header_and_source_are_one_language(self):
        self.assertEqual(self.graph.language_of('sim/types.h'), self.graph.language_of('sim/types.cpp'))
        self.assertNotEqual(self.graph.language_of('sim/types.h'), self.graph.language_of('ui/panel.gd'))


CONSUMER_SOURCE = (b'var notes = []\nvar count = 0\nvar dirty = false\n'
                   b'func add(n):\n\tnotes.append(n)\n\tcount += 1\n\tdirty = true\n'
                   b'func _process(delta):\n\tredraw()\n'
                   b'func redraw():\n\tif dirty:\n\t\tshow_all(notes, count)\n'
                   + b''.join(b'func peek_%d():\n\treturn count\n' % i for i in range(8)))


class Consumers(unittest.TestCase):
    """A unit outside the run that reads far more of the mutated state than other readers is a consumer, shown with the
    data it reads and the chain from the entry that runs it."""
    def test_the_reader_of_most_mutated_state_is_a_consumer_with_its_entry(self):
        facts = [extract_source('store.gd', CONSUMER_SOURCE, GRAMMARS['gdscript'].language, 'gdscript')]
        graph = Graph(Index('.', facts, {}, []))
        result = trace(graph, [n.id for n in graph.nodes if n.name == 'add'])
        names = [graph.nodes[c].name for c in result.consumers]
        self.assertEqual(names, ['redraw'])
        redraw = result.consumers[0]
        self.assertEqual({graph.nodes[d].name for d in result.consumer_reads[redraw]}, {'notes', 'count', 'dirty'})
        self.assertEqual([graph.nodes[n].name for n in result.consumer_up[redraw]], ['_process', 'redraw'])
        self.assertEqual(violations(graph, result, '.'), [])


class UpstreamThroughData(unittest.TestCase):
    """A polymorphic call made through a receiver is set up by whoever assigns the receiver; a handler registered on a
    signal is reached by whoever fires the signal."""
    @classmethod
    def setUpClass(cls):
        facts = [extract_source(path, data, GRAMMARS['gdscript'].language, 'gdscript')
                 for path, data in sorted(TOOL_SOURCES.items())]
        cls.graph = Graph(Index('.', facts, {}, []))
        cls.result = trace(cls.graph, [n.id for n in cls.graph.nodes if n.name == 'build'])

    def names(self, ids):
        return {self.graph.nodes[n].name for n in ids}

    def test_the_input_entry_and_the_setup_chain_are_upstream(self):
        self.assertLessEqual({'fire', '_unhandled_input', 'choose', '_on_picked', 'picked', '_on_tab'},
                             self.names(self.result.upstream))
        self.assertIn('_on_tab', self.names(self.result.entries))

    def test_registration_sits_on_the_object_when_the_method_is_not_in_the_code(self):
        picked = [n.id for n in self.graph.nodes if n.name == 'picked'][0]
        handler = [n.id for n in self.graph.nodes if n.name == '_on_picked'][0]
        self.assertIn('register', self.graph.out[picked][handler].kinds)
        panel = [n.id for n in self.graph.nodes if n.name == 'panel'][0]
        self.assertNotIn(handler, self.graph.out[panel])


class FixtureProperties(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.graph = Graph(build_index(ROOT, cache_dir=None))

    def check(self, seeds):
        self.assertTrue(seeds)
        result = trace(self.graph, seeds)
        self.assertTrue(result.feature)
        self.assertEqual(violations(self.graph, result, ROOT), [])
        return result

    def test_from_a_seed(self):
        result = self.check(self.graph.find('NotesApi::store_note'))
        self.assertTrue(result.entries)
        self.assertTrue(result.mutated)
        self.assertTrue(result.dependents)

    def test_from_a_query(self):
        self.check(select_by_query(self.graph, 'save note'))


if __name__ == '__main__':
    unittest.main()
