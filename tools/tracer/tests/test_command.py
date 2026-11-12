"""The tracer's command on a fixture: a start named by its label, by a path and line; a trace as data; the list of
names; and what must fail (a start naming nothing, roots holding no code)."""
from __future__ import annotations

import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

HOME = Path(__file__).resolve().parent.parent
FIXTURE = HOME / "tests" / "fixture" / "gd"
SECONDS = 60


def trace(*words: str, root: Path = FIXTURE):
    done = subprocess.run([sys.executable, str(HOME / "trace.py"), *words, "--root", str(root)], capture_output=True,
                          timeout=SECONDS)
    return done.returncode, done.stdout.decode("utf-8"), done.stderr.decode("utf-8")


class Command(unittest.TestCase):
    def test_a_label_gives_what_runs_from_it_and_what_reaches_it(self):
        code, out, _ = trace("Crate.weigh")
        self.assertEqual(0, code)
        self.assertIn("Crate.weigh  crate.gd:7", out)
        self.assertIn("call   Crate.lift  crate.gd:4  @ crate.gd:5", out.split("What reaches it:")[1])

    def test_a_path_and_line_names_the_function_written_there(self):
        code, out, _ = trace("shop.gd:15", "--depth", "1")
        self.assertEqual(0, code)
        self.assertIn("call   Crate.lift", out)
        self.assertNotIn("Crate.weigh", out)

    def test_what_was_found_by_name_can_be_left_out(self):
        self.assertIn("[by name]", trace("Shop.unknown")[1])
        self.assertNotIn("Crate.lift", trace("Shop.unknown", "--exact")[1])

    def test_what_was_found_by_text_is_said_and_can_be_left_out(self):
        mix = FIXTURE.parent / "mix"
        self.assertIn("call   Core.fire  core.h:7  @ panel.gd:19  [by text]", trace("panel.send", root=mix)[1])
        self.assertNotIn("Core.fire", trace("panel.send", "--exact", root=mix)[1])

    def test_a_function_entered_with_a_name_given_runs_by_text_the_one_of_that_name(self):
        mix = FIXTURE.parent / "mix"
        named, any_name = trace("panel.ask", root=mix)[1], trace("panel.ask_any", root=mix)[1]
        self.assertIn("Core.fire", named)
        self.assertNotIn("Core.sweep", named)
        self.assertIn("Core.sweep", any_name)
        reaching = trace("Core.sweep", root=mix)[1].split("What reaches it:")[1]
        self.assertIn("panel.ask_any", reaching)
        self.assertNotIn("panel.ask ", reaching)

    def test_a_function_entered_with_a_function_given_runs_the_one_this_caller_gave(self):
        ts = FIXTURE.parent / "ts"
        given = trace("again", root=ts)[1].split("What reaches it:")[0]
        self.assertIn("call   byFirst", given)
        self.assertNotIn("later", given)
        reaching = trace("later", root=ts)[1].split("What reaches it:")[1]
        self.assertIn("main", reaching)
        self.assertNotIn("again", reaching)

    def test_a_trace_as_data(self):
        code, out, _ = trace("Shop.open", "--json")
        data = json.loads(out)
        self.assertEqual(0, code)
        self.assertIn({"depth": 1, "label": "Crate.lift", "at": "crate.gd:4", "again": False, "kind": "call",
                       "how": "exact", "written": "shop.gd:15"}, data["forward"])

    def test_the_list_names_every_definition_code_is_written_in(self):
        code, out, _ = trace("--list")
        self.assertEqual(0, code)
        self.assertIn("Shop.open  shop.gd:14", out.splitlines())

    def test_a_start_naming_nothing_fails(self):
        code, _, err = trace("no_such_name")
        self.assertEqual(1, code)
        self.assertIn("nothing is named no_such_name", err)

    def test_roots_holding_no_code_fail(self):
        with tempfile.TemporaryDirectory() as empty:
            self.assertEqual(1, trace("--list", root=Path(empty))[0])


if __name__ == "__main__":
    unittest.main()
