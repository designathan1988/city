"""Tests of the line-endings gate (tools/gates/gate_files.py check_line_endings) on a listed file the disk lacks when it is read: a
file removed between the listing and the read (a run's temporary folder); the gate judges no bytes there and goes on to
the files that remain. A fixture folder cannot hold such a file (the self-test lists what its folder holds), so the case is
built here in a temporary folder with the gates' own Context.
Run: python -m unittest discover -s tools/gates/tests -p "test_line_endings.py" -v"""
import os
import shutil
import sys
import tempfile
import unittest

GATES = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
if GATES not in sys.path:
    sys.path.insert(0, GATES)

import gate_files  # noqa: E402
from context import Cache, Context  # noqa: E402

GONE = "a_gone.txt"  # sorts before CRLF, so the gate meets the vanished file first
CRLF = "b_crlf.txt"
NO_BEFORE = "before"  # a folder never made: every listed file is new to the change


class VanishedFile(unittest.TestCase):
    def setUp(self):
        self.root = tempfile.mkdtemp(prefix="gates-line-endings-")

    def tearDown(self):
        shutil.rmtree(self.root, ignore_errors=True)

    def context(self, listed):
        """The gates' Context over the temporary folder: `listed` as git would list it, each path changed as a new file."""
        changed = {p: None for p in listed}
        return Context(self.root, list(listed), list(listed), {}, {}, changed, Cache(self.root, enabled=False),
                       before_root=os.path.join(self.root, NO_BEFORE))

    def findings(self, ctx):
        out = []
        gate_files.check_line_endings(ctx, lambda gid, where, msg: out.append((gid, where)))
        return out

    def test_listed_file_absent_from_disk_gives_no_finding(self):
        self.assertEqual(self.findings(self.context([GONE])), [])

    def test_files_after_the_absent_one_are_still_judged(self):
        with open(os.path.join(self.root, CRLF), "wb") as fh:
            fh.write(b"one\r\ntwo\r\n")
        self.assertEqual(self.findings(self.context([GONE, CRLF])), [("LINE_ENDINGS", CRLF)])


if __name__ == "__main__":
    unittest.main()
