"""The files a language itself reads to resolve names (fixtures `cfg/*`, every name in them made up): a TypeScript
configuration's path aliases, written in the file it extends, with comments and trailing commas; a Godot project's
globals and the folder `res://` stands for; the include folders a compilation database gives a C++ unit."""
from __future__ import annotations

import unittest

from support import CALL, EXACT, NEW, reached, traced


class TypeScript(unittest.TestCase):
    def setUp(self):
        self.r = traced("cfg/ts")

    def test_names_imported_through_the_aliases_are_the_files_they_map_to(self):
        self.assertEqual({"temper", "hone", "Anvil.strike"}, reached(self.r, "work", (CALL,), EXACT))
        self.assertEqual({"Anvil"}, reached(self.r, "work", (NEW,), EXACT))


class JavaScript(unittest.TestCase):
    def test_another_prefix_and_the_base_folder_in_a_jsconfig(self):
        self.assertEqual({"pack", "seal"}, reached(traced("cfg/js"), "ship", (CALL,), EXACT))


class GDScript(unittest.TestCase):
    def setUp(self):
        self.r = traced("cfg/gd")

    def test_a_global_the_project_file_declares_is_an_object_of_its_script(self):
        self.assertEqual({"ledger.post"}, reached(self.r, "desk.press", (CALL,), EXACT))

    def test_a_script_not_enabled_as_a_global_is_none(self):
        self.assertNotIn("quiet.post", reached(self.r, "desk.press", (CALL,)))

    def test_res_is_the_project_s_folder(self):
        self.assertEqual({"plain.greet"}, reached(self.r, "desk.hello", (CALL,), EXACT))
        where = {self.r.program.where(link.dst) for src, links in self.r.out.items() for link in links
                 if self.r.program.label(src) == "desk.hello"}
        self.assertEqual({"base/plain.gd:4"}, where)


class Cpp(unittest.TestCase):
    def test_the_unit_s_include_folder_says_which_header(self):
        r = traced("cfg/cpp")
        where = {r.program.where(link.dst) for src, links in r.out.items() for link in links
                 if r.program.label(src) == "run"}
        self.assertEqual({"inc/kit/gear.h:3"}, where)


if __name__ == "__main__":
    unittest.main()
