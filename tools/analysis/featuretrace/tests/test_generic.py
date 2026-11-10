"""The tracer knows no project: it reads the folder it is given (or the current one), and its source names none."""
from __future__ import annotations

import re
import subprocess
import sys
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parent
TOOL = HERE.parents[1]
CLI = TOOL / "trace_feature.py"
FIXTURE = HERE / "fixture"
# the layout and names of the repository the tool happens to live in
FOREIGN = re.compile(r"native/src|project/|dist/|\bcity\b|CityCore|road|Roadcraft|parents\[2\]", re.IGNORECASE)


class Generic(unittest.TestCase):
    def test_reads_the_current_folder_by_default(self):
        run = subprocess.run([sys.executable, str(CLI), "--no-cache", "RoadTool._lay"], cwd=FIXTURE,
                             capture_output=True, text=True, timeout=120)
        self.assertEqual(run.returncode, 0, run.stderr)
        self.assertIn("fx::Core::build_road", run.stdout)

    def test_source_names_no_project(self):
        sources = [CLI] + sorted((TOOL / "featuretrace").glob("*.py"))
        found = [f"{p.name}:{i}: {line.strip()}" for p in sources
                 for i, line in enumerate(p.read_text(encoding="utf-8").splitlines(), 1) if FOREIGN.search(line)]
        self.assertEqual(found, [])


if __name__ == "__main__":
    unittest.main()
