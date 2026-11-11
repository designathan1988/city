"""The JavaScript and TypeScript expectations both cores answer (log/trace-core-plan.md, Task 6): the web fixture's
and the jsvalues fixture's checks on the calls a definition makes, read through each case's own `names` and `reached`
(the current core's `FixtureCase`, the new core's `HeapCase`), so one set of expectations holds both while the new core
replaces the current one."""
from __future__ import annotations


class WebExpectations:
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

    def test_host_callbacks_and_events(self):
        self.assertTrue({"Game.start.<lambda:21>", "Game.start.<lambda:22>"} <= self.names("Game.start", ("handed",)))
        self.assertIn("Game.onReady", self.reached("announce"))            # dispatchEvent(new Event("ready"))

    def test_jsx_component(self):
        self.assertIn("ui.tsx.Panel", self.names("ui.tsx:5", ("handed",)))


class JsValuesExpectations:
    def test_inner_function_is_not_global(self):
        found = self.names("use")
        self.assertIn("script.js.shown", found)
        self.assertNotIn("script.js.x", found)                 # `outer`'s local, not a global (L2)

    def test_inner_function_is_not_a_member(self):
        self.assertNotIn("K.helper", self.names("viaMember"))   # `m`'s local, not a member of K (L1)

    def test_map_get_and_entries(self):
        self.assertIn("Box.open", self.names("lookup"))        # get: the value
        self.assertIn("Box.open", self.names("entries"))       # iteration: [key, value]
        self.assertIn("Box.open", self.names("types.ts:26"))   # forEach: the value (in the callback)

    def test_promise_awaited(self):
        self.assertIn("Box.open", self.names("later"))

    def test_new_calls_a_function(self):
        found = self.names("mk")
        self.assertIn("news.js.Maker", found)                  # called as a constructor
        self.assertIn("Box.open", found)                       # the object it returns

    def test_new_through_a_variable(self):
        self.assertIn("Box.open", self.names("viaVariable"))

    def test_generator_iteration_gives_its_yields(self):
        self.assertIn("Box.open", self.names("fromGenerator"))

    def test_for_in_names_the_properties(self):
        self.assertIn("Box.open", self.names("eachKey"))                   # k: "a", "b"; table[k]: the Boxes

    def test_object_keys_values_entries(self):
        self.assertIn("Box.open", self.names("keysOf"))
        self.assertIn("Box.open", self.names("valuesOf"))
        self.assertIn("Box.open", self.names("entriesOf"))

    def test_closure_sees_a_later_declaration(self):
        self.assertIn("Box.open", self.names("closures.ts:13"))         # run(): helper is declared after it

    def test_computed_key_names_its_members(self):
        self.assertIn("Box.open", self.names("pick"))                      # t["a"]: the field a
