"""Every mechanism by which code makes other code run, or hands it data, in the languages read (fixture `mech`, every
name made up): a handler given through a tagged template, written into the platform's object, bound, connected or
emitted by its name; an object carrying an object of functions; a worker made from a file's path, both ways; a queue of
functions kept in aggregate items; a node of a scene by its path; state written, its owner, who reads it. And what the
trace cannot follow, found without being told where: why each call is not followed, which functions nothing calls,
what makes a function run. The analysis kept between runs answers as the analysis does."""
from __future__ import annotations

import pickle
import unittest

from support import CALL, EXACT, FIRES, HANDED, reached, traced
from tracer import index
from tracer.gaps import NO_CALLER, UNKNOWN_CALL, summary, triggers, uncalled, unresolved
from tracer.report import readers_of, written
from tracer.vocab import BY_TEXT, WRITES
from tracer.walk import backward, find, forward


def labels(result, steps) -> set:
    return {result.program.label(step.at) for step in steps}


class Handlers(unittest.TestCase):
    def setUp(self):
        self.r = traced("mech/ts")

    def test_a_tagged_template_hands_its_substitutions_over(self):
        self.assertEqual({"panel.press"}, reached(self.r, "panel", (HANDED,)))

    def test_a_function_written_into_the_platform_s_object_is_registered(self):
        self.assertTrue(any(label.startswith("alarm.") for label in reached(self.r, "alarm", (HANDED,))))

    def test_an_object_carrying_an_object_of_functions_keeps_them(self):
        self.assertIn("host.shout", reached(self.r, "use", (CALL,), EXACT))

    def test_a_bound_method_is_the_method(self):
        self.assertIn("Clock.tick", reached(self.r, "later", (CALL,), EXACT))


class Worker(unittest.TestCase):
    def setUp(self):
        self.r = traced("mech/ts")

    def test_a_message_reaches_the_file_s_handler_and_the_answer_the_client(self):
        self.assertIn("answer", labels(self.r, forward(self.r, find(self.r, "Client.send"))))
        self.assertIn("answer", labels(self.r, backward(self.r, find(self.r, "Client.receive"))))


class Godot(unittest.TestCase):
    def setUp(self):
        self.r = traced("mech/gd")

    def test_a_signal_connected_and_emitted_by_its_name(self):
        self.assertIn("Board._on_tap", reached(self.r, "Board.fire", (FIRES,)))

    def test_a_bound_method_connected(self):
        self.assertIn("Board._pick", reached(self.r, "Board._ready", (HANDED,)))

    def test_a_node_of_the_scene_by_its_path(self):
        self.assertIn("lamp.glow", reached(self.r, "Board._ready", (CALL,), BY_TEXT))


class Jobs(unittest.TestCase):
    def setUp(self):
        self.r = traced("mech/cpp")

    def test_a_function_kept_in_a_queue_s_item_fires_where_the_item_is_run(self):
        self.assertTrue(any(label.startswith("start.<lambda") for label in reached(self.r, "Pool.loop", (FIRES,))))

    def test_state_written_its_owner_and_who_reads_it(self):
        p = self.r.program
        bump = find(self.r, "Ledger.bump")
        wrote = written(self.r, bump)
        self.assertEqual({"Tally.count"}, {p.label(d) for d in wrote})
        self.assertEqual("Tally", p.label(p.defs[next(iter(wrote))].owner))
        self.assertIn("Ledger.read", {p.label(u) for d in wrote for u, _ in readers_of(self.r, d)})
        self.assertEqual(WRITES, self.r.state_out[bump[0]][next(iter(wrote))])


class Gaps(unittest.TestCase):
    def setUp(self):
        self.r = traced("mech/ts")

    def test_a_function_nothing_calls_is_said_with_where(self):
        p = self.r.program
        self.assertIn("lonely", {p.label(d) for d, why in uncalled(self.r) if why == NO_CALLER})
        self.assertTrue(any("ui.ts:" in row and "lonely" in row for row in summary(self.r)["of them, no caller found"]))

    def test_a_call_of_which_nothing_is_known_is_said_with_where(self):
        p = self.r.program
        rows = [(p.label(u), shown) for u, f, line, shown, why in unresolved(self.r) if why == UNKNOWN_CALL]
        self.assertIn(("blind", "thing.whirl"), rows)

    def test_what_makes_a_function_run(self):
        steps = backward(self.r, find(self.r, "ring"))
        roots = {self.r.program.label(root) for root, _, _ in triggers(self.r, steps)}
        self.assertLessEqual({"alarm", "use"}, roots)


class Index(unittest.TestCase):
    def test_the_analysis_kept_answers_as_the_analysis(self):
        r = traced("mech/ts")
        before = labels(r, forward(r, find(r, "use")))
        kept = pickle.loads(pickle.dumps(index.kept(fresh("mech/ts"))))
        self.assertEqual(before, labels(kept, forward(kept, find(kept, "use"))))


def fresh(folder: str):
    from support import FIXTURE
    from tracer import analyse
    return analyse([FIXTURE / folder], FIXTURE / folder)


if __name__ == "__main__":
    unittest.main()
