"""JavaScript, TypeScript and a library written in JavaScript (log/trace-rules.md, JavaScript and TypeScript rows):
the web fixture read with its package folder as a root, each rule checked on the links it must make."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

import tree_sitter_typescript

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))
sys.path.insert(0, str(HERE))

from featuretrace import rules  # noqa: E402
from js_expectations import WebExpectations  # noqa: E402
from support import FixtureCase  # noqa: E402
from test_rules import log_table  # noqa: E402


class Web(FixtureCase, WebExpectations):
    ROOTS = ("web", "web/node_modules/three")
    BASE = "web"

    def test_files_parse(self):
        self.assertEqual(self.broken, [])
        self.assertEqual(self.files, 12)

    def test_specifiers_resolve(self):
        js = self.ix.js
        self.assertEqual(js.resolve("src/main.ts", "@app/events"), "src/events.ts")            # tsconfig paths
        self.assertEqual(js.resolve("src/main.ts", "./barrel"), "src/barrel.ts")                # extensionless
        self.assertEqual(js.resolve("src/barrel.ts", "./scene/index.js"), "src/scene/index.ts")  # .js -> .ts
        self.assertEqual(js.resolve("src/main.ts", "three"), "node_modules/three/build/three.module.js")

    def test_property_made_by_assignment(self):
        self.assertEqual(self.ix.units[self.gid("Actor.tag")].kind, "field")


class RuleTable(unittest.TestCase):
    def test_every_tsx_kind_has_its_row(self):
        from test_rules import grammar_kinds

        class Tsx:
            language = staticmethod(tree_sitter_typescript.language_tsx)
        self.assertEqual(set(rules.JS_RULES), grammar_kinds(Tsx))

    def test_code_table_equals_the_log_table(self):
        self.assertEqual(rules.JS_RULES, log_table("## JavaScript and TypeScript: every construct"))

    def test_rule_ids_have_handlers(self):
        used = {r for ids in rules.JS_RULES.values() for r in ids} - {"NONE"}
        self.assertLessEqual(used, set(rules.HANDLERS))


if __name__ == "__main__":
    unittest.main()
