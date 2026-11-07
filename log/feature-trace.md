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
