"""Calibration of LAB_COPY (tools/gates/gate_lab.py) on cases written for it under tools/gates/tests/fixtures/lab_copy_*
(a bakery's oven, made up: nothing of the roadcraft), each run through the self-test with the real thresholds of rules.json:
a translation of the reference, renamed or not, by hand or by machine, is refused, and so are the reference's names in a
data table and its shader held in a string; the same behaviour designed another way, the domain's words, a comment citing the
reference, the reference's values in this game's data and locals named like the reference's pass; a missing reference fails;
lines the change does not touch are the scan's. Then this game's code bound to the engine, the allocator or the platform,
which a browser game cannot have written, judged whole against the real reference: no passage or number of it is a copy.
Run: python -m unittest discover -s tools/gates/tests -p "test_lab_*.py" -v"""
import os
import sys
import unittest

GATES = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
if GATES not in sys.path:
    sys.path.insert(0, GATES)

import gate_lab  # noqa: E402
import lab_prints  # noqa: E402
import lab_tokens  # noqa: E402
import registry  # noqa: E402
import selftest  # noqa: E402
from context import REPO, Cache, Context  # noqa: E402
from run_gates import load_config  # noqa: E402

RULES, OWNERS = load_config(REPO)
BY_NAME = {r.name: r for r in registry.all_rules(RULES)}
REFUSED = ("line_translation", "renamed_translation", "machine_translation", "shader_string")
PASSED = ("independent", "domain_words", "comment_citation", "generic_locals", "unchanged_lines")
SHARE_MARGIN = 0.1  # the least distance between min_share and a case's share, either side
NUMBER_MARGIN = 1  # the least distance between min_numbers and a case's numbers in order, either side
ENGINE_BOUND = (  # RenderingServer, Terrain3D, mimalloc, GDExtension binding, Godot's Theme, ConfigFile, Logger and scene flow
    "native/src/core/memory.cpp", "native/src/render/surface_data.cpp", "native/src/render/surface_streams.cpp",
    "native/src/render/terrain_bridge.cpp", "native/src/render/rs_util.h", "native/src/render/instance_rows.h",
    "native/src/render/mesh_draw.h", "native/src/render/row_keeper.h", "native/src/api/city_core_bind.cpp",
    "native/src/api/register_types.cpp", "project/ui/theme/theme_builder.gd", "project/ui/theme/theme_text.gd",
    "project/ui/theme/theme_art.gd", "project/game/flow/game_flow.gd", "project/game/settings/settings.gd",
    "project/game/settings/quality.gd", "project/game/settings/frame_governor.gd", "project/game/store/city_store.gd",
    "project/game/health/health_log.gd", "project/render/shaders/terrain.gdshader")


def case_dir(name):
    return os.path.join(selftest.FIXTURES, "lab_copy_" + name)


def run(name):
    """(failure ids, issues) of a calibration case run through the self-test."""
    _rule, ids, issues = selftest.run_case(case_dir(name), RULES, OWNERS, BY_NAME)
    return ids, issues


def measures(name):
    """[(share, numbers in order)] of every piece a case's change judges."""
    _case, ctx = selftest.case_context(case_dir(name), RULES, OWNERS)
    out = []
    for path in sorted(ctx.changed):
        out += [(p.share, p.numbers) for p in gate_lab.judge(ctx, ctx.source(path))]
    return out


class Calibration(unittest.TestCase):
    def refused(self, name):
        ids, issues = run(name)
        self.assertEqual(issues, [])
        self.assertEqual(ids, {"LAB_COPY"})

    def passed(self, name):
        ids, issues = run(name)
        self.assertEqual(issues, [])
        self.assertEqual(ids, set())

    def test_line_translation_fails(self):
        self.refused("line_translation")

    def test_renamed_translation_fails(self):
        self.refused("renamed_translation")

    def test_machine_translation_fails(self):
        self.refused("machine_translation")

    def test_shader_in_a_string_fails(self):
        self.refused("shader_string")

    def test_data_names_fail(self):
        self.refused("data_names")

    def test_independent_passes(self):
        self.passed("independent")

    def test_domain_words_pass(self):
        self.passed("domain_words")

    def test_comment_citation_passes(self):
        self.passed("comment_citation")

    def test_data_values_pass(self):
        self.passed("data_values")

    def test_generic_locals_pass(self):
        self.passed("generic_locals")

    def test_missing_reference_fails(self):
        self.refused("missing_reference")

    def test_unchanged_lines_not_judged(self):
        self.passed("unchanged_lines")


class Margins(unittest.TestCase):
    def test_thresholds_have_margin(self):
        cfg = RULES["lab_copy"]
        passing = [m for name in PASSED for m in measures(name)]
        self.assertTrue(passing)
        self.assertLessEqual(max(s for s, _n in passing) + SHARE_MARGIN, cfg["min_share"])
        self.assertLessEqual(max(n for _s, n in passing) + NUMBER_MARGIN, cfg["min_numbers"] - 1)
        for name in REFUSED:  # each copy has a function well past one threshold or the other
            best = max(measures(name), key=lambda m: (m[0], m[1]))
            self.assertTrue(best[0] >= cfg["min_share"] + SHARE_MARGIN or best[1] >= cfg["min_numbers"] + NUMBER_MARGIN,
                            (name, best))


class EngineBound(unittest.TestCase):
    def test_engine_bound_code_is_no_copy(self):
        files = list(ENGINE_BOUND)
        ctx = Context(REPO, files, files, RULES, OWNERS, {f: None for f in files}, Cache(REPO, enabled=True))
        found = [(f, v.line, v.matches) for f in files for v in gate_lab.judge(ctx, ctx.source(f)) if v.matches]
        self.assertEqual(found, [])


class Idioms(unittest.TestCase):
    def test_idioms_ignored(self):
        """A k-gram or a number found in more than max_spread reference units is an idiom (MOSS -m): no evidence."""
        cfg = RULES["lab_copy"]
        tokens = [f"N:{i}" for i in range(cfg["min_tokens"] + cfg["k"])]
        grams = lab_tokens.kgrams([(t, 1) for t in tokens], cfg["k"])
        numbers = [3.75 + i for i in range(cfg["min_numbers"])]

        def store(units):
            return lab_prints.Store({}, {h: [["ref.ts", u, p] for u in units] for h, p in grams},
                                    {f"ref.ts:{u}": numbers for u in units}, {f"ref.ts:{u}": " ".join(tokens) for u in units})
        piece = gate_lab.Piece(1, tokens, grams, numbers)
        self.assertEqual(gate_lab.matches(store(range(1, cfg["max_spread"] + 2)), cfg, piece), [])
        self.assertEqual({m.kind for m in gate_lab.matches(store([1]), cfg, piece)}, {"passages", "numbers"})


if __name__ == "__main__":
    unittest.main()
