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
from support import FixtureCase  # noqa: E402
from test_rules import log_table  # noqa: E402


class Web(FixtureCase):
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

    def test_module_runs_its_imports(self):
        self.assertTrue({"barrel.ts.<module>", "events.ts.<module>", "three.module.js.<module>"}
                        <= self.names("main.ts:43"))

    def test_entry_constructs_and_calls(self):
        self.assertTrue({"Game.constructor", "Game.start"} <= self.names("main.ts:43"))

    def test_barrel_chain_and_renamed_export(self):
        self.assertIn("Actor.constructor", self.names("Game.frame"))       # Hero = Actor through two barrels
        self.assertIn("Actor.measure", self.names("Game.frame"))

    def test_getter_iterator_object_literal_and_bind(self):
        start = self.names("Game.start")
        self.assertIn("Actor.pace.get", start)                             # a read calls the getter
        self.assertIn("Actor.[Symbol.iterator]", start)                    # for...of
        self.assertIn("Actor.measure", start)                              # the loop variable is an Actor
        self.assertTrue(any(n.endswith(".greet") for n in start))          # an object literal's method
        self.assertIn("Game.frame", start)                                 # this.frame.bind(this) ... .call(this)

    def test_global_script_name(self):
        self.assertIn("legacy.js.legacyHelper", self.names("Game.start"))

    def test_callbacks_receive_the_language_values(self):
        self.assertIn("Actor.measure", self.names("main.ts:18"))           # forEach: the elements
        self.assertIn("Actor.measure", self.names("main.ts:28"))           # then: the value awaited

    def test_library_code_calls_back(self):
        self.assertIn("Game.frame", self.reached("WebGLRenderer.tick"))    # setAnimationLoop(cb) ... this._loop(t)
        self.assertIn("Game.start.<lambda:20>", self.names("WebGLRenderer.tick"))

    def test_host_callbacks_and_events_by_type(self):
        self.assertTrue({"Game.start.<lambda:21>", "Game.start.<lambda:22>"} <= self.names("Game.start", ("handed",)))
        self.assertIn("Game.onReady", self.reached("announce"))            # dispatchEvent(new Event("ready"))

    def test_property_made_by_assignment(self):
        self.assertEqual(self.ix.units[self.gid("Actor.tag")].kind, "field")

    def test_jsx_component(self):
        self.assertIn("ui.tsx.Panel", self.names("ui.tsx:5", ("handed",)))


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
