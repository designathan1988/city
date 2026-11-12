"""The configuration TypeScript finds by walking up from the sources (tsc and tsserver look for `tsconfig.json` in the
folder and each of its parents): an import through a path alias it declares resolves when only the sources below it
are read (fixture `upconfig`, read from `upconfig/src`), on both cores."""
from __future__ import annotations

import unittest

from .support import ENGINE, HeapCase


class UpConfig(HeapCase):
    ROOTS = ("upconfig/src",)
    BASE = "upconfig/src"

    def test_an_alias_of_a_config_above_the_folder_read_resolves(self):
        self.assertEqual({"Wrench"}, self.classes("use"))


class UpConfigEngine(UpConfig):
    CORE = ENGINE


if __name__ == "__main__":
    unittest.main()
