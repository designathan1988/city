"""Extraction contract: units, kinds, scopes and token roles from real grammars and from the lexical fallback."""
import unittest

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools/analysis: the ftrace package

from ftrace.extract import extract_source  # noqa: E402
from ftrace.roles import NAME_ROLES  # noqa: E402
from ftrace.grammars import load_grammars  # noqa: E402

GRAMMARS, _SKIPPED = load_grammars()


def language(name):
    return GRAMMARS[name].language


def units_by_name(facts):
    return {(u['scope'], u['name']): u for u in facts['units'][1:]}


def tokens(unit, role):
    roles = NAME_ROLES + ('member',) if role == 'i' else (role,)
    return [t for r, t, _ in unit['tokens'] if r in roles]


CPP = b'''#ifndef A_H
#define A_H
// comment mentions ghost_name
namespace app { class Core : public Base { public: void place(int x); int entries; }; }
template <class T> T twice(T v) { return v + v; }
void app::Core::place(int x) { Foo::bar(x); entries = 2; auto s = "store_entry"; auto w = "two words"; }
#endif
'''

GD = b'''extends Node
class_name Tool
signal placed(x)
const MAX = 3
var speed := 2.0
# comment mentions ghost_name
func _on_pressed(a: int) -> void:
\tvar local = "save_note"
\tstate.notes = a
class Inner extends Ref:
\tfunc go():
\t\tpass
'''

TS = b'''// comment mentions ghost_name
import { a } from "./util";
export class Ctl extends Base { private n = 1; save(x: number): void { api.call("save_note", x); } }
const handler = (e) => { run(e); };
function top(a) { return a; }
'''


class CppExtraction(unittest.TestCase):
    def setUp(self):
        self.facts = extract_source('a.h', CPP, language('cpp'))
        self.units = units_by_name(self.facts)

    def test_class_inside_include_guard_and_namespace_is_a_container(self):
        self.assertEqual(self.units[(('app',), 'Core')]['kind'], 'container')

    def test_member_prototype_and_field_are_scoped_to_their_class(self):
        self.assertEqual(self.units[(('app', 'Core'), 'entries')]['kind'], 'data')
        protos = [u for u in self.facts['units'] if u['name'] == 'place']
        self.assertEqual(len(protos), 2)
        self.assertTrue(all(u['scope'] == ('app', 'Core') and u['kind'] == 'callable' for u in protos))
        self.assertEqual(sorted(u['body'] for u in protos), [False, True])

    def test_template_function_is_found_through_its_wrapper(self):
        self.assertEqual(self.units[((), 'twice')]['kind'], 'callable')

    def test_locals_and_parameters_are_not_units(self):
        names = {u['name'] for u in self.facts['units']}
        self.assertFalse({'x', 'v', 's', 'w'} & names)

    def test_body_tokens_name_strings_and_skip_the_defined_name(self):
        place = [u for u in self.facts['units'] if u['name'] == 'place' and u['body']][0]
        ids = tokens(place, 'i')
        self.assertIn('bar', ids)
        self.assertIn('entries', ids)
        self.assertNotIn('place', ids)
        self.assertIn('store_entry', tokens(place, 's'))
        self.assertIn('two words', tokens(place, 's'))

    def test_comment_words_never_become_identifiers(self):
        for unit in self.facts['units']:
            self.assertNotIn('ghost_name', tokens(unit, 'i'))
        self.assertTrue(any('ghost' in t for t in tokens(self.facts['units'][0], 'c')))


class GdscriptExtraction(unittest.TestCase):
    def setUp(self):
        self.facts = extract_source('tool.gd', GD, language('gdscript'))
        self.units = units_by_name(self.facts)

    def test_members_and_kinds(self):
        self.assertEqual(self.units[((), 'speed')]['kind'], 'data')
        self.assertEqual(self.units[((), 'MAX')]['kind'], 'data')
        self.assertEqual(self.units[((), '_on_pressed')]['kind'], 'callable')
        self.assertEqual(self.units[((), 'Inner')]['kind'], 'container')
        self.assertEqual(self.units[(('Inner',), 'go')]['kind'], 'callable')

    def test_function_tokens(self):
        fn = self.units[((), '_on_pressed')]
        self.assertIn('notes', tokens(fn, 'i'))
        self.assertIn('save_note', tokens(fn, 's'))
        self.assertNotIn('local', {u['name'] for u in self.facts['units']})

    def test_file_level_tokens_belong_to_the_file_unit(self):
        self.assertIn('Node', tokens(self.facts['units'][0], 'i'))


class TypescriptExtraction(unittest.TestCase):
    def setUp(self):
        self.facts = extract_source('ctl.ts', TS, language('typescript/typescript'))
        self.units = units_by_name(self.facts)

    def test_class_members(self):
        self.assertEqual(self.units[((), 'Ctl')]['kind'], 'container')
        self.assertEqual(self.units[(('Ctl',), 'n')]['kind'], 'data')
        self.assertEqual(self.units[(('Ctl',), 'save')]['kind'], 'callable')

    def test_arrow_function_bound_to_a_name_is_callable(self):
        self.assertEqual(self.units[((), 'handler')]['kind'], 'callable')
        self.assertEqual(self.units[((), 'top')]['kind'], 'callable')

    def test_import_specifier_is_a_reference_not_a_definition(self):
        self.assertNotIn(((), 'a'), self.units)
        self.assertIn('a', tokens(self.facts['units'][0], 'i'))
        self.assertIn('./util', tokens(self.facts['units'][0], 's'))


class LexicalFallback(unittest.TestCase):
    def test_without_grammar_quoted_values_are_strings_and_bare_words_are_text(self):
        facts = extract_source('w.layout', b'[button id="save" on_click="handle_save_clicked"] see `make_note`\n', None)
        self.assertEqual(facts['parser'], 'lexical')
        self.assertEqual(len(facts['units']), 1)
        self.assertIn('handle_save_clicked', tokens(facts['units'][0], 's'))
        self.assertIn('button', tokens(facts['units'][0], 'w'))
        self.assertIn('make_note', tokens(facts['units'][0], 'w'))
        self.assertEqual(tokens(facts['units'][0], 'i'), [])


class Recognition(unittest.TestCase):
    def test_keywords_come_from_the_grammar_itself(self):
        from ftrace.grammars import keywords
        self.assertLessEqual({'if', 'fi', 'in', 'for'}, keywords(language('bash')))
        self.assertLessEqual({'class', 'return', 'namespace'}, keywords(language('cpp')))

    def test_reserved_words_are_realised_in_code_and_not_in_prose(self):
        import tree_sitter
        from ftrace.grammars import keywords, recognition
        bash = language('bash')
        script = b'for f in a b; do\n  if [ -n "$f" ]; then echo "$f"; fi\ndone\n'
        prose = b'Run it in a shell if you want, for example in the morning or in the evening.\n'
        realised, spelled = recognition(tree_sitter.Parser(bash).parse(script), script, keywords(bash))
        self.assertGreater(realised / spelled, 0.5)
        realised, spelled = recognition(tree_sitter.Parser(bash).parse(prose), prose, keywords(bash))
        self.assertLess(realised / spelled, 0.5)


if __name__ == '__main__':
    unittest.main()
