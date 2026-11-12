"""The shapes of each language beyond a plain call (same fixtures as test_languages): the base called through `super`,
an inner class, an entry of a container; a folder's file with its re-exports, a getter and a setter, an object taken
apart, a JSX element, a module loaded where it is used; a namespace written under another, a template, an object made
with a variable as its argument, a function kept in a member, a name written with its scopes."""
from __future__ import annotations

import unittest

from support import BY_NAME, BY_TEXT, CALL, EXACT, HANDED, NEW, backward, find, forward, reached, traced


class GDScript(unittest.TestCase):
    def setUp(self):
        self.r = traced("gd")

    def test_super_alone_runs_the_base_s_function_of_the_same_name(self):
        self.assertLessEqual({"oven.heat", "oven.vent"}, reached(self.r, "Kiln.heat", (CALL,), EXACT))

    def test_an_inner_class_read_from_a_dictionary_s_entry(self):
        self.assertIn("Kiln.Rack", reached(self.r, "Kiln", (NEW,)))
        self.assertIn("Kiln.Rack.load", reached(self.r, "Kiln.heat", (CALL,)))

    def test_a_function_added_to_a_list_runs_where_the_list_is_gone_through(self):
        self.assertEqual({"Kiln.cool"}, reached(self.r, "Kiln.heat", (HANDED,)))


class TypeScript(unittest.TestCase):
    def setUp(self):
        self.r = traced("ts")

    def test_names_read_through_a_folder_s_file(self):
        self.assertLessEqual({"Oven.make", "bake", "Panel"}, reached(self.r, "Kitchen", (CALL,), EXACT))
        self.assertEqual({"Tools.wipe"}, reached(self.r, "pressed", (CALL,), EXACT))

    def test_a_property_read_runs_its_getter_and_written_its_setter(self):
        self.assertIn("Oven.heat", reached(self.r, "Kitchen", (CALL,), EXACT))
        self.assertEqual({"Gauge", "Gauge.reset"}, reached(self.r, "Oven.heat", (CALL, NEW), EXACT))

    def test_an_object_taken_apart_in_a_parameter_and_in_a_declaration(self):
        found = reached(self.r, "bake", (CALL,))
        self.assertLessEqual({"Oven.heat", "Gauge.read", "pressed"}, found)

    def test_a_handler_given_to_a_component_reaches_the_element_it_gives_it_to(self):
        self.assertEqual({"pressed"}, reached(self.r, "Panel", (HANDED,)))

    def test_a_module_loaded_where_it_is_used(self):
        self.assertEqual({"Tools.wipe"}, reached(self.r, "late", (CALL,), EXACT))


class Cpp(unittest.TestCase):
    def setUp(self):
        self.r = traced("cpp")

    def test_an_object_made_with_a_variable_as_its_argument(self):
        self.assertIn("fx.heat.Kiln.Kiln", reached(self.r, "fx.heat.Kiln.fire", (NEW,), EXACT))
        self.assertIn("fx.heat.Kiln.fire", reached(self.r, "fx.heat.Kiln.fire", (CALL,), EXACT))

    def test_a_template_s_method_a_field_by_reference_and_a_name_with_its_scopes(self):
        found = reached(self.r, "fx.heat.Kiln.fire", (CALL,), EXACT)
        self.assertLessEqual({"fx.heat.Rack.take", "fx.Crate.weigh", "fx.heat.fan"}, found)

    def test_a_function_kept_in_a_member_and_called_through_it(self):
        self.assertIn("fx.heat.stoke", reached(self.r, "fx.heat.Kiln.fire", (CALL,), EXACT))

    def test_what_a_template_gives_back_is_found_by_name(self):
        self.assertEqual({"fx.Crate.lift", "fx.Lamp.lift"}, reached(self.r, "fx.heat.Kiln.fire", (CALL,), BY_NAME))

    def test_an_initializer_makes_the_member(self):
        self.assertIn("fx.Crate", reached(self.r, "fx.heat.Kiln.Kiln", (NEW,)))


class Outside(unittest.TestCase):
    """Where a member is the declared type's (C++, TypeScript), a member of an object of a type the code read does not
    define is not the code's; one of an object nothing is known of (a type's parameter) can be any class's of that
    language; an alias is the type it names. Where a member is looked up in the object as it runs (GDScript), a
    declared type says nothing: any class of the language, and a class of another one below a type not defined."""

    def test_cpp(self):
        r = traced("cpp")
        for label in ("fx.drift", "fx.raise"):
            self.assertEqual(set(), reached(r, label, (CALL,)), label)
        self.assertEqual({"fx.Crate.lift", "fx.Lamp.lift"}, reached(r, "fx.hoist", (CALL,), BY_NAME))
        self.assertEqual({"fx.Crate.lift"}, reached(r, "fx.boxed", (CALL,), EXACT))
        self.assertEqual(set(), reached(r, "fx.boxed", (CALL,), BY_NAME))

    def test_typescript(self):
        r = traced("ts")
        self.assertEqual(set(), reached(r, "drift", (CALL,)))
        self.assertEqual({"Crate.lift", "Winch.lift"}, reached(r, "hoist", (CALL,), BY_NAME))
        self.assertEqual({"Crate.lift"}, reached(r, "boxed", (CALL,), EXACT))
        self.assertEqual(set(), reached(r, "boxed", (CALL,), BY_NAME))

    def test_a_script_reaches_another_language_through_a_type_not_defined(self):
        """`panel.nudge` reads `body.fire`: once it was also found `panel.lift`, the value of the entry `"fire"` of
        the dictionary `table`, through a store every literal's entries went to by their keys. An entry is a member of
        its own object only (spec 001, FR-005): `table.fire` finds it, `body.fire` does not."""
        r = traced("mix")
        self.assertEqual({"Core.fire", "panel.lift"}, reached(r, "panel.press", (CALL,), BY_NAME))
        self.assertEqual({"Core.fire"}, reached(r, "panel.nudge", (CALL,), BY_NAME))

    def test_a_compiled_language_does_not_reach_a_script_by_name(self):
        r = traced("mix")
        self.assertEqual({"Core.fire", "Plain.fire"}, reached(r, "hoist", (CALL,), BY_NAME))


class Text(unittest.TestCase):
    """A function handed over together with the text of its name is known by that text to the code not read: a
    call no definition answers, given that text or holding it, may run it. So may one given the text of a method's
    name together with an object of the method's class."""

    def setUp(self):
        self.r = traced("mix")

    def test_a_script_reaches_the_function_another_language_handed_under_its_name(self):
        self.assertEqual({"Core.fire", "Core.sweep"}, reached(self.r, "panel.send", (CALL,), BY_TEXT))

    def test_a_text_given_to_a_call_says_nothing_alone(self):
        self.assertEqual(set(), reached(self.r, "panel.setup", (CALL,), BY_TEXT))
        self.assertEqual(set(), reached(self.r, "panel.has", (CALL,), BY_TEXT))

    def test_where_it_is_handed_it_is_handed_and_no_more(self):
        self.assertEqual({"Core.fire", "Core.sweep"}, reached(self.r, "bind_all", (HANDED,)))
        self.assertEqual(set(), reached(self.r, "bind_all", (CALL,), BY_TEXT))

    def test_a_text_kept_among_data_runs_nothing(self):
        self.assertEqual(set(), reached(self.r, "panel.count", (CALL,), BY_TEXT))

    def test_what_an_answered_call_gives_back_keeps_no_text(self):
        self.assertEqual({"panel.send"}, reached(self.r, "panel.ask", (CALL,)))

    def test_a_text_given_with_an_object_names_the_object_s_method(self):
        self.assertEqual({"panel.lift"}, reached(self.r, "panel.later", (CALL,), BY_TEXT))

    def test_a_sentence_names_no_function(self):
        self.assertEqual(set(), reached(self.r, "panel.say", (CALL,), BY_TEXT))


class Forms(unittest.TestCase):
    """An object written in place is an object of its own shape (fixture `rules`, spec 001 D3): a factory's object is
    called exactly; its entries are never found by name for an object nothing is known of."""

    def setUp(self):
        self.r = traced("rules")

    def test_a_factory_s_object_is_called_exactly(self):
        self.assertLessEqual({"makeDesk.stamp", "makeDesk.file"}, reached(self.r, "office", (CALL,), EXACT))

    def test_an_entry_is_no_guess_for_an_object_nothing_is_known_of(self):
        self.assertEqual(set(), reached(self.r, "stray", (CALL,)))


class Table(unittest.TestCase):
    """A table of functions' names read by the key a caller gives (fixture `table`, spec 001 D5): the caller's key
    selects its entry, forward and back; with no caller, every entry."""

    def setUp(self):
        self.r = traced("table")

    def labels(self, steps):
        return {self.r.program.label(step.at) for step in steps}

    def test_the_key_a_caller_gives_selects_its_entry(self):
        ahead = self.labels(forward(self.r, find(self.r, "routes.seal")))
        self.assertIn("Core.seal_box", ahead)
        self.assertNotIn("Core.wipe_box", ahead)

    def test_with_no_caller_every_entry(self):
        self.assertLessEqual({"Core.seal_box", "Core.wipe_box"}, self.labels(forward(self.r, find(self.r, "routes.send"))))

    def test_back_only_to_the_callers_that_gave_its_key(self):
        self.assertNotIn("routes.seal", self.labels(backward(self.r, find(self.r, "Core.wipe_box"))))
        self.assertIn("routes.seal", self.labels(backward(self.r, find(self.r, "Core.seal_box"))))


class Visible(unittest.TestCase):
    """A name written in a namespace is seen from the files that reach the one writing it (itself, what it includes);
    one no such file writes is found by its name alone."""

    def test_each_file_calls_the_helper_it_can_see(self):
        r = traced("cpp")
        where = {r.program.where(link.dst) for src, links in r.out.items() for link in links
                 if r.program.label(src) == "fx.north" and r.program.label(link.dst) == "fx.trim"}
        self.assertEqual({"north.cpp:7"}, where)
        self.assertEqual({"fx.trim"}, reached(r, "fx.south", (CALL,), EXACT))

    def test_a_function_no_reachable_file_declares_is_not_the_one_called(self):
        r = traced("cpp")
        self.assertEqual({"fx.trim"}, reached(r, "fx.north", (CALL,)))
        self.assertIn("polish", r.outside[next(d.id for d in r.program.defs if r.program.label(d.id) == "fx.north")])


class Held(unittest.TestCase):
    """What an expression holds: each name a declaration defines is its own; a choice between values, brackets and a
    cast hold what they are given (a cast, an object of its type); a sum holds none of it."""

    def test_every_name_of_one_declaration_is_defined(self):
        self.assertEqual(set(), reached(traced("cpp"), "fx.both", (HANDED,)))

    def test_a_choice_holds_either(self):
        self.assertEqual({"fx.drift", "fx.raise"}, reached(traced("cpp"), "fx.choose", (HANDED,)))
        self.assertEqual({"onA", "onB"}, reached(traced("ts"), "choose", (HANDED,)))
        self.assertEqual({"lamp.polish_a", "lamp.polish_b"}, reached(traced("gd"), "lamp.choose", (HANDED,)))

    def test_a_sum_holds_no_function(self):
        self.assertEqual(set(), reached(traced("ts"), "tally", (HANDED,)))
        self.assertEqual(set(), reached(traced("gd"), "lamp.tally", (HANDED,)))

    def test_a_member_read_and_not_called_is_no_function_found_by_name(self):
        self.assertEqual(set(), reached(traced("gd"), "lamp.glance", (HANDED,)))
        self.assertEqual(set(), reached(traced("ts"), "glance", (HANDED,)))

    def test_a_function_kept_in_a_member_a_class_declares_is_not_kept_under_its_name(self):
        r = traced("cpp")
        self.assertEqual(set(), reached(r, "fx.peek", (HANDED,)))
        self.assertEqual({"fx.drift"}, reached(r, "fx.poke", (CALL,), BY_NAME))

    def test_a_call_given_a_function_keeps_it_and_one_given_none_may_run_it(self):
        r = traced("gd")
        self.assertEqual({"lamp.polish_a"}, reached(r, "lamp.wire_a", (HANDED,)))
        self.assertEqual({"lamp.polish_b"}, reached(r, "lamp.wire_b", (HANDED,)))
        self.assertEqual({"lamp.polish_a", "lamp.polish_b"}, reached(r, "lamp.fire", (HANDED,)))
        self.assertEqual(set(), reached(r, "lamp.wire_c", (HANDED,)))       # given a function not defined here

    def test_a_function_an_object_keeps_is_not_the_object_s_value(self):
        r = traced("gd")
        self.assertEqual(set(), reached(r, "lamp.look", (HANDED,)))
        self.assertEqual({"lamp.polish_a", "lamp.polish_b"}, reached(r, "lamp.first", (HANDED,)))

    def test_what_an_object_keeps_by_name_is_not_run_after_it_has_travelled(self):
        self.assertEqual(set(), reached(traced("gd"), "lamp.drift", (HANDED,)))
        self.assertEqual({"Shop._on_sold"}, reached(traced("gd"), "Shop.sell", (HANDED,), EXACT))

    def test_what_an_object_keeps_runs_where_an_element_of_it_is_called(self):
        r = traced("ts")
        self.assertEqual({"onA"}, reached(r, "flush", (CALL,), BY_NAME))
        self.assertEqual({"byFirst"}, reached(r, "sorted", (HANDED,)))
        self.assertEqual(set(), reached(r, "listed", (HANDED,)))

    def test_what_is_found_by_name_is_not_kept_under_another_name(self):
        self.assertEqual(set(), reached(traced("gd"), "lamp.use", (CALL,)))

    def test_a_cast_is_an_object_of_its_type(self):
        self.assertEqual({"fx.Crate.lift", "fx.Crate.weigh"}, reached(traced("cpp"), "fx.recast", (CALL,), EXACT))
        self.assertEqual(set(), reached(traced("cpp"), "fx.recast", (CALL,), BY_NAME))
        self.assertEqual({"Crate.lift"}, reached(traced("ts"), "recast", (CALL,), EXACT))
        self.assertEqual({"Crate.lift"}, reached(traced("gd"), "lamp.recast", (CALL,), EXACT))


if __name__ == "__main__":
    unittest.main()
