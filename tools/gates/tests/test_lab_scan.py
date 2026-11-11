"""Tests of the scan of what already stands (tools/gates/lab_scan.py): every file of LAB_COPY's paths judged whole against
the reference, each finding listed with this game's place and the reference's, and the exit code saying whether anything was
found. Run on the self-test's made-up repositories (a bakery's oven) with their reference.
Run: python -m unittest discover -s tools/gates/tests -p "test_lab_*.py" -v"""
import json
import os
import sys
import tempfile
import unittest

GATES = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
if GATES not in sys.path:
    sys.path.insert(0, GATES)

import lab_scan  # noqa: E402
import selftest  # noqa: E402
from context import REPO  # noqa: E402
from run_gates import load_config  # noqa: E402

RULES, OWNERS = load_config(REPO)


def case_root(name):
    """(repo, rules) of a lab_copy self-test case: its repository and the real rules with the case's reference."""
    case_dir = os.path.join(selftest.FIXTURES, "lab_copy_" + name)
    with open(os.path.join(case_dir, "case.json"), encoding="utf-8") as fh:
        rules = dict(RULES, **json.load(fh)["overrides"])
    return os.path.join(case_dir, "repo"), rules


class Scan(unittest.TestCase):
    def test_scan_reports_fixture_copy(self):
        root, rules = case_root("line_translation")
        found = lab_scan.scan(root, rules, OWNERS, lab_scan.judged_files(root, rules, lab_scan.walk(root)))
        places = {where for _gid, where, _msg in found}
        self.assertIn("native/src/bakery/oven.cpp:15", places)  # batchMinutes, its passages and its name
        self.assertTrue(all(gid == "LAB_COPY" for gid, _w, _m in found))
        self.assertTrue(any("oven.ts" in msg for _gid, _w, msg in found))

    def test_scan_exit_code(self):
        for name, code in (("line_translation", 1), ("independent", 0)):
            root, rules = case_root(name)
            with tempfile.TemporaryDirectory() as out:
                report = os.path.join(out, "scan.json")
                self.assertEqual(lab_scan.run(root, rules, OWNERS, lab_scan.walk(root), report), code, name)
                with open(report, encoding="utf-8") as fh:
                    self.assertEqual(bool(json.load(fh)["findings"]), bool(code))


if __name__ == "__main__":
    unittest.main()
