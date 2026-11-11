# Feature trace log

How to trace one feature end to end in this project, and what each attempt taught. Feature traced first: building a road.

## Method (what works)
1. Start at the trigger the player uses (the UI data that draws the button or tab).
2. Follow every link through the mechanism that makes the call, reading the code at each step:
   - GDScript: direct calls, `signal`/`connect`/`emit`, `Callable(obj, "name")`, `callv`/`call`, `preload`/`load`.
   - Store: `CityStore.dispatch(&"name")` / `query(&"name")` -> `StoreApi.COMMANDS`/`QUERIES` -> `Callable(core or area, method)`.
   - C++: `ClassDB::bind_method(D_METHOD("name"), &Class::method)`, then direct calls.
3. Go forward from the state change to every reader of what changed, until no caller and no reader is left.
4. Write each link with file and line.

## What went wrong before (do not repeat)
- Building a "generic" tracer that guessed links from name coincidence, statistics, PageRank and graph communities, then
  patching its effects run after run. The links are not to be guessed: each one goes through a known call mechanism.
- Narrowing searches to one engine when the request was generic, and reporting "candidates" instead of answers.

## Road: chain so far
| # | Link | Where |
|---|---|---|
| 1 | Roads category, tab `draw` -> tool `ROAD` (the live trigger) | `project/data/ui/drawers.json:11` |
| 1a | Finding: `toolbar.json` road buttons (street/avenue/oneway/highway) are loaded by DataTables but read by no one | `project/data/ui/toolbar.json:5`, `project/game/data/data_tables.gd:43` |
| 2 | Tab selected -> params (`profile` from the catalog) -> `tool_requested.emit` | `project/ui/hud_drawer.gd:106-125` |
| 3 | `drawer.tool_requested.connect(_on_tool_requested)` -> `tools.set_tool(Tool["ROAD"], params)` | `project/ui/roadcraft_hud.gd:48`, `:224-226` |
| 4 | `set_tool` instantiates `SCRIPTS[ROAD]` = `road_tool.gd` | `project/game/tools/tool_controller.gd:26`, `:84-92` |
| 5 | `enter`: `dispatch road_variant`, `query road_height`, `dispatch set_road_walls` | `project/game/tools/road_tool.gd:36-45` |
| 6 | Pointer: `click`/`drag`/`hover` -> `query road_start`, `road_end`, `road_pieces`; preview `dispatch preview_road_path` | `road_tool.gd:69-105`, `:183-221` |
| 7 | `release` -> `_lay` -> `CityStore.dispatch(&"build_road_path", [pieces, drawn, px, lift])` | `road_tool.gd:111-128`, `:224-236` |
| 8 | `dispatch` -> `_routes[command].callv(args)`; non-preview commands `_mark()` the snapshot (`changed` deferred) | `project/game/store/city_store.gd:92-99`, `:145-157` |
| 9 | Route built from `StoreApi.COMMANDS["build_road_path"] = "build_road_path"` -> `Callable(_core, ...)`, core = `ClassDB.instantiate("CityCore")` | `project/game/store/store_api.gd:13`, `city_store.gd:37-55` |
| 10 | Binding `bind_method(D_METHOD("build_road_path", ...), &CityCore::build_road_path)` | `native/src/api/city_core_bind.cpp:51` |
| 11 | `CityCore::build_road_path` -> `run_edit(state, "cmd.build.road", ...)`: exclusive `map_mutex`, `history.begin/end` (undo entry; rolled back when not ok) | `native/src/api/city_core_commands.cpp:150-161`, `native/src/api/edit_result.h:30-37` |
| 12 | `road_ends` (anchors at both ends) then `build_path` | `city_core_commands.cpp:58`, `native/src/sim/commands/road_path.cpp:146-163` |
| 13 | `build_path`: per piece `place_road`, then `settle_road_edit` (money) | `road_path.cpp:153`, `:160`; `native/src/sim/economy/road_economy.cpp:93-95` |
| 14 | `place_road`: squeeze check, `lay_road`, `name_streets` (`j_add_item` street-name plates) | `native/src/sim/commands/road_commands.cpp:94-109`, `:82-92` |
| 15 | `lay_road`: `plan_road` (refusal reasons), crossings split segments (`split_segment`), `lay_pieces` -> `j_add_node`/`j_add_segment` (bridge over water), `clear_under` (buildings removed), `mark_segment`, `merge_stubs`, `settle_network` | `road_commands.cpp:174-209`, `:55-79`; `native/src/sim/commands/road_plan.cpp:185` |
| 16 | State writes and their dirty marks: `j_add_segment` -> `roads.add_segment`, `log.net_seg/net_node`, journal `record(ADD_SEG)`; `mark_segment` -> `log.road_chunks`, `log.lot_cells` | `native/src/sim/commands/journal_ops.cpp:231-241`, `native/src/sim/roads/road_network.cpp:446-455` |
| 17 | Money: `settle_road_edit` -> `charge_of(edit_value_change)`, refuse `REFUSED_FUNDS`, else `j_money(-amount)` | `native/src/sim/economy/road_economy.cpp:95-104` |
| 18 | `history.end` -> `finish_command`: `settle_network` (`settle_shapes`, `sync_grade` heights and ground, name plates), hydrant squares -> `mark_furniture`, `lots.process_dirty`, `growth.push(cell)`, `edit_revision++`, `touch()` | `native/src/sim/commands/edit_history.cpp:242`, `native/src/sim/commands/road_edit.cpp:127-168` |
| 19 | Consumer, lots: `LotCache::process_dirty` drains `lot_cells`, proposes lots per cell | `native/src/sim/lots/lot_cache.cpp:82-95` |
| 20 | Consumer, traffic (simulation thread): `traffic step` -> `apply_network_changes` drains `net_nodes`, `net_segs_*`, `furniture_segs`: lanes, junctions, signals, furniture, parking | `native/src/sim/traffic/traffic_system.cpp:222`, `native/src/sim/traffic/lane_graph_edit.cpp:155-175` |
| 21 | Consumer, render (every frame): `CityCore` -> `render_.sync` -> `drain_log` (road_chunks, prop_chunks, terrain_rects...) -> `mark_changed`: road, far road, raised (bridges), props, trees, cover, overlays, transit | `native/src/api/city_core.cpp:194`, `native/src/render/render_world.cpp:114-127`, `:145-179`, `:236-254` |
| 22 | Back to the UI: `edit_revision` in the snapshot; `CityStore._mark` -> deferred `changed`; tool hints the cost and flashes the laid road | `native/src/api/city_core_query.cpp:135`, `project/game/store/city_store.gd:145-157`, `road_tool.gd:233-236` |

## Findings on the way
- `project/data/ui/toolbar.json` (street/avenue/oneway/highway buttons) is loaded (`data_tables.gd:43`) and read by no one.
- `ChangeLog.network_changed` is set (`road_network.cpp:106,128,179,194`, `save_read.cpp:291`) and only merged
  (`change_log.h:77`); no consumer reads it.

## Remaining links, opened
| # | Link | Where |
|---|---|---|
| 23 | `plan_road`: refusals `tool.failed`, `refuse.road.tooShort`, `selfLoop`, `sharpAngle`, `road.reason.overlap`, `fullNode`, `junctionsClose`; price from `build_value` | `native/src/sim/commands/road_plan.cpp:185-215` |
| 24 | `sync_grade`: ground moved -> `grade.solve` -> `terrain.put_road_shape`, `mark_ground`, `net.mark_heights` (the terrain under the road is reshaped) | `native/src/sim/commands/road_grade_sync.cpp:59-75` |
| 25 | Render meshes: `mark_changed` marks `road_stream_`; `offer_streams` submits `roads_.submit`, `verges_.submit`, `structures_.submit` jobs per chunk | `native/src/render/render_world.cpp:151-158`, `:193-208`; `native/src/render/roads/road_renderer.cpp:80` |
| 26 | Lanes: `apply_network_changes` drops removed segments, forgets furniture/parking/doors, rebuilds junction connectors of changed nodes | `native/src/sim/traffic/lane_graph_edit.cpp:175-200` |
| 27 | Undo/redo: the journal ops recorded at 16 replayed by `apply_entry` -> `finish_command` | `native/src/sim/commands/edit_history.cpp:201`, `:236-242` |

## Call mechanisms the road flow used (the generic logic comes from these)
1. Direct calls and reads by name (GDScript, C++), including calls inside a lambda handed to another function (`run_edit`).
2. A function value handed to a call on some receiver: a registration (`signal.connect(handler)`, `bind_method(D_METHOD("name"), &Class::method)`).
   The receiver or the name literal becomes an alias of the function: firing the receiver or naming the alias reaches it.
3. A name carried as a string value: `dispatch(&"build_road_path")`, `StoreApi` pair `&"build_road_path": "build_road_path"`, data `"tool": "ROAD"`.
4. A path string naming a file: `preload("res://game/tools/road_tool.gd")`.
5. Overriding: the controller calls `active.click()`; every class with `click` that extends the base answers.
6. Shared data: one function writes a field (`log.road_chunks`), another reads it later (`drain_log`): writer -> reader.
7. Entry points with no caller in the code: engine callbacks, the frame (`render_.sync`), the simulation thread (`traffic step`).

## Result
Traced by reading, link by link, in one pass: trigger -> tool -> store route -> binding -> undoable command -> network
and money writes -> dirty marks -> lots, traffic, render -> snapshot back to the UI. No guessing was needed: every link
went through a call mechanism listed under Method.

## Generic tracer (`C:\Users\Jonathan\Desktop\feature-trace`): causes found and fixed (2026-11-07)
Membership comes from language constructs plus graph prevalence; no names are listed anywhere. Each cause below was
found by reading the extractor (`ftrace/extract.py`) and the graph (`ftrace/graph.py`) whole, not by rerunning.
| # | Cause (design decision that produced the symptom) | Symptom | Fix |
|---|---|---|---|
| 1 | Plain-name lookup had four owners (`_resolve`, `_chains`, `_registrations`, `_receiver`); the last two searched every unit, ignoring visibility | `count(key)` in C++ registered GDScript `key`; upstream exploded to 3770 | One lookup (`Graph._lookup`); registrations reuse the edges the unit already resolved, name edges only (not strings) |
| 2 | Statement position was recognised by the `body` field only | locals in `if {}` blocks (C++ `consequence`) and in `elif`/`else` clauses resolved to global namesakes | A statement child that opens a block (`{`) or is led by a reserved word is a statement position |
| 3 | Loop bindings were not declarations | `for key in snap` resolved `key` globally | A construct with `left` and `body` (no assignment) declares the names in `left` |
| 4 | Imports were walked as definitions | `using godot::Vector2;` became a unit and swallowed `Vector2(...)` calls | A node with a dependency word and no `name` field of its own is an import; its names are referenced |
| 5 | A call was any node with an `arguments` field | `Map<int,T>` (template args) and `snap[key]` (GDScript subscript) counted as calls | A call's arguments open with the call operator `(`; the name of a callee or a target skips arguments and subscripts |
| 6 | A member access was any node holding a member-operator token | GDScript `func f() -> void:` made the whole function a chain: every name became a member | Member access = operand first, then only member operators |
| 7 | Bases were taken from every read token of a class or file | a shader reading `floor` "inherited" C++ `PaneRooms::floor`; visibility cascaded through its includes | Bases are what the inheritance construct names: `extends` or the class header (neither name nor body) |
| 8 | The anywhere-visible rules (types, script classes, declaration-definition joins) ignored language | GDScript `Control` resolved to C++ `city::Control` | A file's language comes from structure (extensions whose files include each other with one grammar); plain names never cross it, strings do |
| 9 | `_callable_part` followed only fielded links | `T &sort(...)` (reference return) became a container; `return v;` became a data member | It follows the same single-named-child chain as `_definition` |
| 10 | Shared code (fan-in outlier) stopped the walk, so its effects were lost | `road_chunks`, `lot_cells`, `finish_command`, `sync_grade` missing | Effect = closure of direct calls through everything; prevalence decides membership only |
| 11 | Fan-out counted resolved targets | `set_tool` (one `active.exit()` reaching ten overrides) looked like a hub | Fan-out counts the names called |
| 12 | Upstream followed only calls | the tool selection path was missing | Upstream also goes from a registered handler's object to whoever fires it, and from a polymorphic call to whoever assigns its receiver (`assign` edges, not mutation by method call) |
| 13 | A declaration with several declarators declared only its first; `for (int i...)` initializers and catch parameters were not statement positions | in `V2 r = a, s = b;` the `s` escaped; `int nx = 0, ny = 0;` gave one member | Every declarator is declared (locals, or one data unit each); a fielded child with a `declarator` is a declaration; a clause with parameters and a body binds them |
| 14 | A class member was visible by plain name from any file including its header | the escaped `s` resolved to `Rng::s`, `Loft::s`, `Run::s`...; mutated state and dependents inflated | A scoped member is reached by plain name only from the scopes the unit sits in, its class's bases, or a namespace its file opens with `using`; from elsewhere through `obj.x` / `A::x` |
| 15 | The effect stopped at the mutated data, mixed into all its readers | `drain_log`, `apply_network_changes` lost among 1561 dependents | Consumers: readers outside the run that read an outlier amount of the mutated state (Tukey fence), each with what it reads and the chain from the entry that runs it |

Result on the game (`python eval/check_repo.py C:/Users/Jonathan/Desktop/city --include project --include native/src --query "build road"`,
graph 5.3 s, trace and checks 0.9 s): 0 property violations; no name edge crosses a language; mutated state 155, dependents 867
(from 685 and 2999 before causes 2, 13 and 14). Every link of the chain above lands where it belongs: `tool_requested` ->
`_on_tool_requested` -> `set_tool` (setup) and `_unhandled_input` (entry) -> `release` -> `_lay` -> seed `build_road_path`
-> `build_path`, `lay_road`, `plan_road`, `lay_pieces` (feature) -> `j_add_segment`, `mark_segment`, `settle_network`
(infrastructure) -> `finish_command`, `sync_grade`, `put_road_shape`, `process_dirty`, `j_money` (executed) -> `road_chunks`,
`lot_cells`, `net_nodes`, `edit_revision` (mutated) -> consumers `CityCore::snapshot` (the UI's snapshot), `apply_network_changes`
(traffic), `RenderWorld::drain_log` (the frame: `_process` -> `sync` -> `drain_log`). The same run on "bulldoze" also has 0 violations.
Tests: 59, in `feature-trace/tests` (each cause above has its own).


`log/mapa.md` brought up to date in the same change (the `MAPA_DESATUALIZADO` gate failed on 59 entries left by earlier
commits: files added without a line, lines naming deleted files).

## Feature tracer by the languages' rules (`tools/analysis/trace_feature.py`), 2026-11-10 15:55
Owner's request: a script that traces any feature of this game (GDScript and C++ GDExtension) by the rules discussed:
know every reserved word that defines, imports, branches, loops and returns, resolve each name used to its definition,
enter it, follow every branch, come back through each return; no path guessing. A new tool; the existing ones untouched.

**Sources (read through context7 before the rules were fixed):** GDScript reference, Keywords table and Inheritance
(`extends "file.gd".Inner`), `is`, `as`, `when` (docs.godotengine.org/en/4.7 gdscript_basics); Signal.connect, Object.connect,
Callable(object, "method"), Object.call/call_deferred (class_signal, class_callable, class_object); GDExtension
`ClassDB::bind_method(D_METHOD)`, `ADD_SIGNAL(MethodInfo)`, `ADD_PROPERTY(PropertyInfo, set, get)` (object_class,
gdextension_cpp_example); C++ keywords, identifiers with special meaning and preprocessor tokens (cppreference keyword,
preprocessor); unqualified lookup order (block, class and bases, enclosing namespaces, global) and qualified lookup
through using-directives (cppreference unqualified_lookup, qualified_lookup).

**Design.** `featuretrace/rules.py` is the one table: every reserved word of both languages with its role in a trace
(define, import, branch, loop, jump, scope, type, value) and the engine's call-by-name mechanisms. The grammar (tree-sitter
cpp 0.23.4, gdscript 6.1.0) gives each construct; readers record definitions and references per construct; the index and
`resolve` apply the lookup rules; `typer` finds the class behind a receiver from what the code declares; `walk` goes forward
(calls, values, names, emits and their handlers, lambdas, constructors, overrides, threads) and back to the entry points.

**Process error, recorded so it is not repeated:** the first version was built before the documentation was read, and
its gaps were found by running it on the city and patching case by case (engine-typed receivers, `->` on library types,
range-for with a declared type, structured bindings, casts, overloads by arity). The owner stopped this. The rules were
then read from the sources above, the table written, and the remaining documented constructs (`is` narrowing, `as`
casts, `when` guards and match bindings, `extends "file".Inner`, a member typed by what is assigned to it) implemented
from the documentation, each with a test.

**Result on the road (`trace_feature.py project/game/tools/road_tool.gd:229`, 4.4 s, 1123 files, 25.5k definitions):**
back to `ToolController._unhandled_input` (engine callback) through `click`/`release` and the override from `ToolBase`;
forward `CityStore.dispatch` -> by name `CityCore::build_road_path` -> `run_edit` -> `lay_road`, `plan_road`, `build_value`,
`j_add_segment`, `settle_road_edit` (overload by arity), `finish_command`, `sync_grade`, `LotCache::process_dirty`; state
written read outside the trace by `RenderWorld::drain_log` (road_chunks), `traffic::apply_network_changes` (net_segs_*),
`CityCore::snapshot` (edit_revision): every link of the hand-traced chain above. 20 links remain on receivers no code
types (flagged `candidates`; `--exact` leaves them out).

**Files created:** `tools/analysis/trace_feature.py`; `tools/analysis/featuretrace/` (`rules`, `model`, `source`, `gd_read`,
`gd_body`, `gd_calls`, `cpp_read`, `cpp_body`, `cpp_expr`, `cpp_names`, `engine_read`, `index`, `typer`, `resolve`, `link`,
`program`, `seeds`, `walk`, `report`); tests `tools/analysis/featuretrace/tests/test_trace.py` (18, one per rule) on
`tests/fixture`. **Changed:** `log/mapa.md`, this log. Tests: `python -m unittest discover -s tools/analysis/featuretrace/tests -t tools/analysis`.

## The tracer rebuilt on the trace rules (2026-11-10 16:50, commits 9905aae2..HEAD)
Owner's order: research every reserved word and construct first, record it in a table, then implement the general
rule, no case-by-case fixes, for any project large or small. Research: `log/trace-rules.md` (Godot 4.7 GDScript and
GDExtension docs, cppreference, through context7); plan: `log/trace-rules-plan.md`; progress and rulings:
`log/trace-rules-progress.md`. Skills: executing-plans, test-driven-development, verification-before-completion.

What changed, by rule: every construct of both grammars (85 + 206) has a row and a handler, enforced by tests; every
expression is an expression tree built by its construct (no text markers); an untyped name holds every value that
flows into it (assignments, arguments at each call site, returns, container puts, loop elements); a name no code
defines is the engine's; C++ adds argument-dependent lookup and the translation unit; the engine's constructor by name
(ClassDB.instantiate) was researched before it was coded (G6). There are no candidate links: a receiver no value reaches
is reported as a fact of the code.

Verified (51 tests, gates OK) and run on three codes: the fixture (every rule), the city (1128 files, 25673
definitions, 102655 links, 9 s; the road chain traced by hand above is all there, back to `_unhandled_input`, consumers
`RenderWorld::drain_log`, `traffic::apply_network_changes`, `CityCore::snapshot`; 0 uses on an unreached receiver), and
third-party code (`project/addons/terrain_3d`, 36 files: `_on_tool_changed` back through the `tool_changed` signal to the
engine's `pressed`, exact where the old tool gave two candidates).

## The tracer finished on the worklist solver (2026-11-10 17:50, commit d7bbdf91)
Cause fixed: the program was linked again whole in passes and cycles advanced one hop per pass. Now links and values
are nodes of one worklist solver with recorded dependencies (TD/RLD, research S5 in `log/trace-rules.md`): what read a
value or the links into a unit is computed again when it grows, until nothing grows. From the whole reading of the
code: a callable used as a value does not run where it is written (it runs where called, where a signal fires it, or
in the engine/library function it is handed to); calling a value reaches `operator()` or the function it holds; `T{}`
constructs; `=`, `++`, `--` on a class call its operators; only class types construct; match patterns are read.
The tool names no project (tests/test_generic.py): it reads the folders given, or the current one.

Verified in order: 70 tests on the fixture; third-party code (Terrain3D, 36 files, 0.2 s: back to the engine's
`pressed` signal, 0 unreached); the city (1132 files, 25770 definitions, 97948 links, fixpoint in 217.7 s, exit 0, 0
uses on an unreached receiver, 4 entry points, 1553 definitions ahead; every link of the road chain traced by hand
above is present, consumers `RenderWorld::drain_log`, `traffic::apply_network_changes`, `CityCore::snapshot`).
Open: speed at city scale (the last large cycles are recomputed whole; S2 difference propagation and S4 cycle collapse
are researched, not implemented); validation against recorded executions of the game (not done).

## Final state (2026-11-10 18:05)
The slow end had one cause, read in the code: a value's identity included the unit it was read in, so the same class
was a different value in each unit and every set grew with the program. A type that names a class of the program is
now that class everywhere; arguments are carried along links by difference (one node per argument of each link);
answers that never change are kept (the class a type names in a scope, the member a name reaches in a class). The
tool prints a line of progress every 5 s. Verified in order: 70 tests on the fixture; Terrain3D (third-party, 36
files) 0.2 s; the city (1132 files, 25770 definitions, 99492 links) fixpoint in 16.1 s, exit 0, 0 uses on an
unreached receiver, 4 entry points, 1556 definitions ahead, every link of the road chain present. Gates OK (50 files).
Not done: comparing the trace with calls recorded from a running game (the validation the literature recommends).

## Validation against a real run: tried, blocked by the build (2026-11-10 18:20)
Method (literature: recall against dynamic call graphs): sample the stacks of a real run and check every caller ->
callee pair of the program against the tracer's links. Done: one hidden run of `tests/core/road_commands_test.tscn`
(Vulkan driver, 40 s, 5 ms) through `tools/analysis/sampler.py`, with a symbolizer front that answers "no symbol" for
system DLLs (the sampler stops on `d3d12.dll`/`ntdll.dll`, which have no debug file; the tool was not changed).
Blocked: `project/bin/libcitycore.windows.template_debug.x86_64.dll` carries no debug information (only `.text`;
llvm-symbolizer returns the nearest export, `citycore_library_init`, with no file or line), so the sampled frames of
the core have no function names. The comparison needs a core DLL built with debug information (`-g`), which means
building the DLLs from a tree that holds another session's uncommitted C++ changes: not done without the owner's word.
The GDScript side has no call recorder in the engine build used here.

## Validation against a real run, done (2026-11-10 19:10)
`tools/analysis/trace_validate.py` (`record`, `check`). The debug DLL is stripped (`-s`); its objects were linked again
with `-Wl,-Map` and the same flags: every section came out byte for byte the same as `project/bin`'s, so the map names
the running code (`dist/trace/symbols/`). Run: `road_commands_test` hidden, 5853 samples; `check --root project --root
native/src --map ...`: 203 distinct calls between functions of the program, 198 answered by the tracer's links.
**Causes found by the run and fixed in the tracer:** (1) a lambda's captured name (C++ [expr.prim.lambda.capture],
GDScript: captured by value) read none of the enclosing function's values: `Unit.outer`/`captured`, `model.declare`,
`enclose`, `assign` (writes reach the enclosing function in C++ only); 43 lambdas handed to a template are now called.
(2) `std::move`/`std::forward`/`as_const`/`ref` return their argument (`rules.PASS_THROUGH`). (3) a container's held
values answer `front`/`back`/`top`/`at`/`get`, iterators and `*`/`->`; `.second` of a program value with no such member
is the mapped value of an associative container. (4) tail positions recorded (`Unit.tails`) for the check's rule.
**Validator rules:** a call seen passes through functions that leave no frame: inlined (body in the translation unit
of one of the frames between, from the map's object file) or left by a tail call; no fixed depth.
**Still open (5 calls):** `JobPool::loop` -> `LotCache::compute`, `BuildingRenderer::build_cpu`, `ShoreField::work`
(the job taken from `slots_`/`helpers_` by `std::move` into a `std::function` local and called); `block_lots` ->
`LotStrips::facing` (through `std::function`); `~CityCore` -> `~JobPool` (a `unique_ptr` member's delete). Not traced
to their cause yet. Implicit constructors/destructors seen running are listed apart (the language writes them).
Own mistakes on the way: a check without roots read 558k files (killed at its deadline); the map reader dropped names
holding `(anonymous namespace)` and cut object paths at the column width; a made-up chain limit of 4, replaced.
Files: created `trace_validate.py`, `featuretrace/runtime.py`, `native_frames.py`, `validate.py`, `operators.py`
(step/operator moved out of `typer.py`); changed `model.py`, `typer.py`, `library.py`, `rules.py`, `cpp_body.py`,
`cpp_read.py`, `cpp_expr.py`, `gd_body.py`, `gd_read.py`, `gd_calls.py`; 70 tests OK, gates OK.

## JavaScript, TypeScript and three.js in the tracer (2026-11-10 21:40)
**Order:** research JS, TS and three.js (context7: MDN, TypeScript handbook, three.js docs), write it down, code, test at the
end; generic. **Research:** `log/trace-rules.md`, sources J1-J12, the general rules applied to JS/TS, and every construct of
the TSX grammar (tree-sitter-typescript 0.23.2, 184 named kinds, one row each, equal to `rule_table.JS_RULES`).
**Plan:** `log/trace-rules-js-plan.md`. **Design:** one more reader family on the same IR and solver; three.js is never named:
it is code when its package folder is a root, else a library (callables handed to it or written into its objects run there).
**Rules coded:** modules (`<module>` top-level code run on import), imports/exports through `export ... from`/`export *`,
TypeScript module resolution (relative with `.js`->`.ts`, extensionless, `index`, tsconfig `paths`/`baseUrl`, package.json
`exports` conditions/`module`/`main`, ambient `declare module`), the global scope (scripts, `declare global`), classes
(default constructor calling the base's, field initializers in the constructor, `<static>`, getters/setters as accessors,
properties made by `this.x = v`), object literals as classes, closures, destructuring, `this` (arrows keep it),
`call`/`apply`/`bind`, implicit calls (`[Symbol.iterator]`, `then` on await, coercion), decorators, JSX components, events by
type (`addEventListener`/`on`/`on<type> =` and `dispatchEvent`/`emit`/`postMessage`), callbacks of array and promise methods
receiving the elements / the value awaited. **Found while testing, fixed as rules for every language:** a member holding a
value that is called calls what it holds (`this._loop(t)`, a `std::function` member); a container's receiver is the
language's (a call on it is the library's, not unreached).
**Tests:** 85 OK (15 new on `tests/fixture/web`: TS, JS, TSX over a small JS library package); gates OK (39 files); the gate
tool learned `.tsx/.jsx/.mts/.cts` (its own self-test OK).
**Real run (the lab, roadcraft: `src` + three r186 `build` and `examples/jsm`, start `src/frameLoop.ts:30`):** 145,626
definitions read in 28 s, 249k links on the first pass, then the solver slowed down to a few thousand nodes per 5 s and the
600 s deadline stopped it (exit 124) with 583k links and 25k nodes still waiting. **Cause, read in the code (`typer.Typer.compute`,
`solver.Solver.grew`):** difference propagation is applied to the places (parameters, fields) but not to the expression nodes:
a node is evaluated again in full, and its whole value list rebuilt, each time anything it read grew, and then every reader of
it is computed again in full. With three.js's node system (TSL) values of hundreds of classes flow through the same
parameters, so a union of thousands of values grows one value at a time and each step recomputes every reader at full
size: quadratic work. **To do (log/pendencias.md):** each tree shape as a transfer function over new values only (a node keeps
its value set and passes on only what is new), the same difference propagation the places already have.

## Difference propagation on every value node: the tracer finishes on large programs (2026-11-10 22:55)
**Cause (read in the code):** `typer.Typer.compute` evaluated an expression node again in full each time anything it read
grew, rebuilt its whole value list, and woke every reader to do the same; only parameters and fields had difference
propagation. With thousands of values meeting in one parameter (three.js's node system), the work was quadratic: the
lab did not reach its fixpoint in 600 s. **Research:** `log/trace-rules.md`, "Difference propagation on every value"
(Pearce, Kelly, Hankin SCAM 2003; Hardekopf & Lin PLDI 2007; Pereira & Berlin CGO 2009; semi-naive Datalog) with the
table of every IR shape, its distributivity and its delta rule. **Plan:** `log/trace-rules-delta-plan.md`.
**Code:** `solver.py` (value nodes as growing sets, subscriptions source -> target with a per-value transfer, a queue of
differences, consumers), `transfer.py` (the per-value rules of member, call, step, operator, substitution), `typer.py`
(each tree wired once by its shape; `#hold`, `#ret`, `#arg`, `#held`, `#engine` nodes; the two non-distributive shapes,
Callable(obj, name) and ClassDB.instantiate(name), are consumers computed whole), `types_of.py` (type naming, out of the
typer), `flows.py` (a link subscribes its arguments to the parameters, a write its value, a signal its carried values),
`link.py` (the differences drain before each consumer); `operators.py` removed (its rules are in `transfer.py`).
**Tests at the end:** 85 tracer tests OK; gates OK. City (`--root project --root native/src`), old solver against new on
the same tree, every link compared: 0 links only in the old, 39 only in the new, all of them the same caller -> callee
pairs the old has, labelled `override` besides `call` (with_overrides excludes the targets present at that relink, and the
order of relinks changed); 19.7 s -> 14.5 s, 387,347 -> 44,832 recomputations. Lab (roadcraft `src` + three r186
`build`, `examples/jsm`, start `src/frameLoop.ts:30`): fixpoint in 325 s (did not finish in 600 s before), 145,846
definitions, 595,715 links, report written (2,266 entry points, 11,641 definitions behind, 13,171 ahead).
**Still whole:** a reference is linked again over its receiver's whole value when it grows (after the differences have
drained); on the lab that is 658k relinks and most of the 325 s. Its delta form (each grown node substituted by its
difference in turn, as semi-naive evaluation does) is the next step for speed; the result is already complete.
