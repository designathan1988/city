"""File discovery, grammar choice and name resolution on the polyglot fixture, checked against what the code means."""
import os
import unittest

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools/analysis: the ftrace package

from ftrace.graph import Graph  # noqa: E402
from ftrace.index import build_index  # noqa: E402
from ftrace.sources import list_files, read_text  # noqa: E402

ROOT = os.path.join(os.path.dirname(__file__), 'fixtures', 'notes_app')
INDEX = build_index(ROOT, cache_dir=None)
GRAPH = Graph(INDEX)


def one(spec):
    found = GRAPH.find(spec)
    assert len(found) == 1, (spec, [GRAPH.label(i) for i in found])
    return found[0]


class Discovery(unittest.TestCase):
    def test_binary_files_are_listed_but_not_read(self):
        self.assertIn('assets/icon.bin', list_files(ROOT))
        self.assertIsNone(read_text(ROOT, 'assets/icon.bin'))
        self.assertNotIn('assets/icon.bin', {f['path'] for f in INDEX.facts})

    def test_grammar_is_chosen_by_parse_quality(self):
        self.assertEqual(INDEX.choice['.cpp'], 'cpp')
        self.assertEqual(INDEX.choice['.h'], 'cpp')
        self.assertEqual(INDEX.choice['.gd'], 'gdscript')
        self.assertTrue(INDEX.choice['.ts'].startswith('typescript/'))

    def test_a_grammar_must_recognise_the_language_not_just_accept_the_text(self):
        self.assertEqual(INDEX.choice['.sh'], 'bash')
        self.assertIsNone(INDEX.choice['.md'])
        self.assertTrue(INDEX.choice['.js'].startswith('typescript/'))

    def test_prose_mentions_are_not_references(self):
        readme = one('README.md')
        self.assertEqual(GRAPH.out[readme], {})


class Resolution(unittest.TestCase):
    def test_prototype_and_definition_are_one_node(self):
        add = one('NoteBook::add')
        self.assertEqual(sorted(p for p, _, _ in GRAPH.nodes[add].locations),
                         ['native/note_book.cpp', 'native/note_book.h'])

    def test_declaration_joins_the_definition_whose_file_includes_it_despite_a_namesake_elsewhere(self):
        native = [i for i in GRAPH.find('log_line') if GRAPH.nodes[i].path.startswith('native/')]
        self.assertEqual(len(native), 1, [GRAPH.label(i) for i in native])
        self.assertEqual(sorted(p for p, _, _ in GRAPH.nodes[native[0]].locations),
                         ['native/log.cpp', 'native/log.h'])
        self.assertIn(native[0], GRAPH.out[GRAPH.find('NotesApi::store_note')[0]])

    def test_string_naming_a_definition_links_across_languages(self):
        self.assertIn(one('NotesApi::store_note'), GRAPH.out[one('MainWindow::handle_save_clicked')])

    def test_comment_mention_creates_no_edge(self):
        self.assertNotIn(one('NotesApi::store_note'), GRAPH.out[one('MainWindow::handle_clear_clicked')])
        self.assertNotIn(one('NotesApi::store_note'), GRAPH.out[one('store_note_legacy')])

    def test_member_call_resolves_to_the_receiver_type(self):
        out = GRAPH.out[one('NotesApi::store_note')]
        self.assertEqual(out[one('NoteBook::add')].weight, 1.0)
        self.assertNotIn(one('TagSet::add'), out)

    def test_import_decides_between_same_named_functions_in_two_languages(self):
        out = GRAPH.out[one('MainWindow::handle_save_clicked')]
        ts_log = [i for i in GRAPH.find('log_line') if GRAPH.nodes[i].path == 'ui/log.ts']
        cpp_log = [i for i in GRAPH.find('log_line') if GRAPH.nodes[i].path == 'native/log.cpp']
        self.assertEqual(len(ts_log), 1)
        self.assertEqual(len(cpp_log), 1)
        self.assertIn(ts_log[0], out)
        self.assertNotIn(cpp_log[0], out)

    def test_single_word_string_is_text_not_a_name(self):
        self.assertNotIn(one('TagSet::add'), GRAPH.out[one('NoteBook::add')])

    def test_path_string_links_to_the_file(self):
        layout = one('ui/main_window.layout')
        self.assertEqual(GRAPH.nodes[layout].kind, 'file')
        self.assertIn(layout, GRAPH.out[one('main')])


if __name__ == '__main__':
    unittest.main()
