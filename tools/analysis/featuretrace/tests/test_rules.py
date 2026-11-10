"""The rule table in code is the research table: every construct of both grammars, the same rules as the log."""
from __future__ import annotations

import re
import sys
import unittest
from pathlib import Path

import tree_sitter
import tree_sitter_cpp
import tree_sitter_gdscript

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parents[1]))

from featuretrace import rules  # noqa: E402

LOG = HERE.parents[3] / "log" / "trace-rules.md"
RULE_ID = re.compile(r"\b(DEF|LOOK|VALUE|FLOW|CALL|BRANCH|JUMP|NAME|SIGNAL|BIND|ENTRY|TYPE|NONE|EXPR)\b")


def grammar_kinds(module) -> set[str]:
    lang = tree_sitter.Language(module.language())
    return {lang.node_kind_for_id(i) for i in range(lang.node_kind_count)
            if lang.node_kind_is_named(i) and lang.node_kind_is_visible(i)}


def log_table(section: str) -> dict[str, tuple[str, ...]]:
    text = LOG.read_text(encoding="utf-8")
    body = text.split(section)[1].split("\n## ")[0]
    table = {}
    for line in body.splitlines():
        cells = [c.strip() for c in line.split("|")[1:-1]]
        if len(cells) != 3 or cells[0] in ("Kind", "---") or set(cells[0]) <= {"-"}:
            continue
        ids = tuple(dict.fromkeys(RULE_ID.findall(cells[2])))
        for kind in cells[0].split(","):
            table[kind.strip()] = ids
    return table


class RuleTable(unittest.TestCase):
    def test_every_grammar_kind_has_its_row(self):
        self.assertEqual(set(rules.GD_RULES), grammar_kinds(tree_sitter_gdscript))
        self.assertEqual(set(rules.CPP_RULES), grammar_kinds(tree_sitter_cpp))

    def test_code_table_equals_the_log_table(self):
        self.assertEqual(rules.GD_RULES, log_table("## GDScript: every construct"))
        self.assertEqual(rules.CPP_RULES, log_table("## C++: every construct"))

    def test_rule_ids_are_the_logs(self):
        used = {r for table in (rules.GD_RULES, rules.CPP_RULES) for ids in table.values() for r in ids}
        self.assertLessEqual(used, rules.RULE_IDS)
        self.assertTrue(all(ids for table in (rules.GD_RULES, rules.CPP_RULES) for ids in table.values()))



class Handlers(unittest.TestCase):
    def test_every_rule_has_a_handler(self):
        import importlib
        used = {r for table in (rules.GD_RULES, rules.CPP_RULES) for ids in table.values() for r in ids} - {"NONE"}
        self.assertLessEqual(used, set(rules.HANDLERS))
        for rule, paths in rules.HANDLERS.items():
            self.assertTrue(paths, rule)
            for path in paths:
                module, _, attr = path.partition(":")
                target = importlib.import_module(f"featuretrace.{module}")
                for part in attr.split("."):
                    target = getattr(target, part)
                self.assertTrue(callable(target), path)


if __name__ == "__main__":
    unittest.main()
