"""The facts cache's version (CLAUDE.md: nothing is redone without a change of input): the facts depend on the readers'
modules, every module of the package they import and the grammars they parse with, and on nothing else of the tracer,
so changing the analysis keeps every cached fact."""
from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from .. import program, source


def planted(folder: Path) -> Path:
    """A package: `a` imports `b` and a subpackage's module, `c` is imported by nobody."""
    package = folder / "pkg"
    (package / "sub").mkdir(parents=True)
    (package / "__init__.py").write_text("")
    (package / "a.py").write_text("import json\nfrom .b import X\nfrom .sub import d\nimport tree_sitter\n")
    (package / "b.py").write_text("X = 1\n")
    (package / "c.py").write_text("from .a import X\n")
    (package / "sub" / "__init__.py").write_text("")
    (package / "sub" / "d.py").write_text("from ..b import X\n")
    return package


class ReaderVersion(unittest.TestCase):
    def test_a_reader_reaches_what_it_imports_and_nothing_else(self):
        with tempfile.TemporaryDirectory() as folder:
            package = planted(Path(folder))
            files, outside = source.imported(package, {"a"})
            self.assertEqual({"__init__.py", "a.py", "b.py", "sub/__init__.py", "sub/d.py"},
                             {p.relative_to(package).as_posix() for p in files})
            self.assertEqual({"tree_sitter"}, outside)                 # json is the standard library's

    def test_the_tracer_readers_reach_no_analysis(self):
        files, outside = source.imported(Path(source.__file__).parent,
                                         {r.__module__.rsplit(".", 1)[-1] for r in program.READERS})
        names = {p.name for p in files}
        self.assertTrue({"js_read.py", "gd_read.py", "cpp_read.py", "source.py"} <= names, names)
        self.assertTrue(names.isdisjoint({"program.py", "engine_input.py", "link.py", "walk.py"}), names)
        self.assertFalse(any(p.parent.name == "heap" for p in files))
        self.assertIn("tree_sitter_typescript", outside)

    def test_the_version_is_stable(self):
        self.assertEqual(source.reader_version(program.READERS), source.reader_version(program.READERS))


if __name__ == "__main__":
    unittest.main()
