# SDD ledger — plan: log/trace-rules-bounds-plan.md
Ruling: execute on main, no worktree — CLAUDE.md (work directly on main) — cost if wrong: a broken commit on main (each task commits only after its tests pass).
Pre-flight: Task 2 consumes Task 1 (js_bounds.of tokens, Unit.bounds/Unit.bound, bounds.flow) — names match across the blocks.
Task 1: Ruling: TypeScript classes and interfaces are structural (handbook, Type Compatibility): a class bound admits an object of a class that declares all its members besides its chain; a host type the tracer did not read admits everything (H1: it may be an alias of a primitive, DOMHighResTimeStamp) — cost if wrong: a host-typed place keeps values its host type would refuse.
Task 1: Ruling: the bound is a filter subscription from each source into the place (no intermediate node): a place first wired after the solve keeps getting its values in the same step, as an identity did — cost if wrong: none.
Task 1: Ruling: a 'named' token is resolved before a null or a primitive is refused (an alias can admit null, a host type everything) — found by test_narrowing.test_alias_lookup_and_conditional_types — cost if wrong: none.
Task 1: complete (commits 6ac1935d..74fd8a63, tests: python -m unittest discover -s tools/analysis/featuretrace/tests -t tools/analysis -> OK 214)
Task 2: Ruling: the JSDoc comment that counts is the last `/** */` before the declaration with only modifiers and its own introduction between (`export`, `async`, `static`, `const x =`): the comment of another declaration is never taken — cost if wrong: a JSDoc separated from its declaration by other code is not read.
Task 2: Ruling: a Closure function type (`function(A): R`) is read as `Function` (any function), its signature not kept — cost if wrong: a function-typed place admits any function, as before.
Task 2: complete (commits 74fd8a6..350e63f, tests: python -m unittest discover -s tools/analysis/featuretrace/tests -t tools/analysis → OK)
Task 3: complete (commits 350e63f..e3048ca, tests: python -m unittest discover -s tools/analysis/featuretrace/tests -t tools/analysis → OK)
Final review: self-review (no subagent tool) — range 209e8823^..e3048cac, tools/analysis/featuretrace (55 files), read file by file against code-reviewer.md.
Final: finding (Important) I1: a place typed `Function` (and a JSDoc `function(...)`) refuses a class, which is a function (MDN Function; TS: a class is assignable to Function).
Final: finding (Important) I2: the structural check refuses a value lacking a member every object has (Object.prototype's `toString`, `valueOf`, ...) and a function or an array with a property code writes on it (an expando).
Final: finding (Important) I3: `Iterable`, `ArrayLike`, `Iterator`, `PromiseLike` bound places to arrays and promises (the tracer's value forms), refusing Sets, Maps, strings, generators, thenables the types admit.
Final: finding (Important) I4: J14 reads an accessor given by reference (`get: getter`) as a function node (a unit with no body, the accessor lost), skips shorthand members (`{ value }`), and ignores a descriptor held in a variable or a key that is no literal.
Final: finding (Important) I5: `this` of a plain function or a function expression is not decided by its calls (a function expression reads its enclosing function's, a plain function the module's or its constructs class), `F.prototype` is the engine's value (methods written on a function's prototype are lost), and a constructible function that writes nothing on `this` makes nothing with `new`.
Final: finding (Important) I6: JSDoc comments are found by a byte pattern over the file, so `/**` inside a string, a template or a regular expression starts a false comment.
Final: minor (fixed with the files it touches): lines joined with runs of spaces in bounds.py and js_primitives.py; flows.py line over the width.
Final: fixed I1 (`Function` admits a class) — test_bounds.Bounds.test_function_admits_a_class RED→GREEN, suite 225/225
Final: fixed I2 (members every value has) — test_bounds.Bounds.test_members_every_value_has RED→GREEN, suite 225/225
Final: fixed I3 (library types by what they admit) — test_bounds.Bounds.test_iterable_and_array_like_by_their_members RED→GREEN, suite 225/225
Final: fixed I6 (JSDoc comments are the parser's) — test_bounds.JsDoc.test_comments_are_the_trees RED→GREEN, suite 226/226
Final: fixed I4 (descriptor members by their values) — test_mixed.DefinedProperties.test_descriptor_members_by_their_values RED→GREEN, suite 227/227
Final: fixed I5 (`this` by calls; prototypes) — test_mixed.FunctionConstructors.test_prototype_of_a_function, test_this_of_a_function_by_its_calls RED→GREEN, suite 229/229
Final: fixed (found by the measurement) a callable handed because its receiver held nothing yet — test_mixed madeBump RED→GREEN (and `Counter2` hands nothing), suite 229/229
Final: fixed (found by the measurement) a block's local read as a later function of its name — test_mixed.LateWrites.test_a_block_local_is_not_a_later_function_of_its_name RED→GREEN, suite 230/230
Final: fixed (found by the measurement) a property call gives `this` the property's object — heldWrite RED→GREEN, suite 230/230
Final: Ruling: the lab with three.js takes 457 s (177 s) after N1'' — a function's own properties read by the language meet closures valued by their body (bisected: 243 s with them read as the engine's) — the rule stands; the closure value per call is the next theme (log/pendencias.md) — cost if wrong: the lab with three.js runs 2.6 times longer until that theme lands.
