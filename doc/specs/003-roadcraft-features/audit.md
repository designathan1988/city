# The architecture audit (specification 003, 2026-11-13)

Seven reading agents, one per area, each claim checked in the code before a fix rests on it (the constitution's agents rule).
Verdict and findings by area; the defects outrank every cleanup. The fixes land in the backbone (architecture.md) or with the
area's port features (sheets/<area>.md).

## The interface (project/game, project/ui)

Verdict: in part. The map tools follow it (each dispatches and shows the core's refusal through ToolBase.refusal, ctl.hint,
HudMessages: tool_base.gd:73-83); GameFlow is the only way to quit; no colour literal outside the theme; 190 tr() keys and 422 data
`*_key` values all present but one. The panels break it.

- DEFECT
  - `CityStore.command_failed` (city_store.gd:10, emitted at :101) has no listener: every refusal of a panel's dispatch is
    silent (road_panel.gd:2-3 promises the opposite).
  - Quitting or replacing the city goes on whatever the save's outcome: a failed write loses the city (game_flow.gd:50-51,
    hud_actions.gd:60-62).
  - The inspector closes on a refused command, the selection lost and no reason given (inspector_panel.gd:127-130); choices lit
    before the command answers (land_strips.gd:140-141, hud_widgets.gd:82-85).
  - road_blocks.gd:18 sends a raw key as success; `hint.road.invalid` is in no CSV.
- SILENT: results dropped at inspector_panel.gd:129,141,165,288; road_panel.gd:49,70,80,131 (55,107 drop the reason);
  approach_panel.gd:166,184; junction_panel.gd:22,52; connector_panel.gd:89; roadcraft_hud.gd:96; transit_strip.gd:52,57;
  land_strips.gd:101; land_hands.gd:78; start_screen.gd:55; session.gd:62-63. An unreadable last city quarantined and a new one
  made unsaid (play.gd:133-136); a boot failure only a push_error, the game left on a blank screen (play.gd:103-105).
- STATE-OUTSIDE: the speed to resume kept in GDScript (game_keys.gd:13,94-99); a copy of the road height (road_tool.gd:20,158-160);
  every lot copied on the tool's entry, stale after an undo (lots_tool.gd:21,56); `_zoned` polled each frame (settings.gd:36-37,
  233-241); the interior state polled each frame (interior_controller.gd:21,84).
- PROPAGATION: the transit list refreshed by a hand-fired signal, not `transit_revision` (transit_strip.gd:20,47); the inspector
  and the health panel on timers instead of `CityStore.changed` (inspector_panel.gd:273-279, health_panel.gd:151-156).
- OWNER: the camera written by hud_popover.gd:216 and render_switches.gd:88 besides CityCamera; enums copied by hand (SimSpeed
  hud_top_bar.gd:13, SegRule approach_panel.gd:139, crossing kinds road_panel.gd:18-19, transit modes transit_strip.gd:5); TOWN_MAP
  twice (play.gd:5, hud_actions.gd:7); the languages twice (game_flow.gd:17-18, start_screen.gd:10); the walls setting held in
  road_prefs.gd:9 and the core; `zone_tool` duplicating ToolController.tool (settings.gd:36); `ctl.remembered` written outside
  set_tool (roundabout_tool.gd:52).
- LIFECYCLE: signals emitted with no listener (`command_failed`, `CityStore.loaded` city_saves.gd:91, `GameFlow.state_changed`
  game_flow.gd:62, `RoadcraftHud.category_pressed`, `HudDialog.closed`); no input handler reads GameFlow.state, so keys pause,
  save and open drawers under the title screen (game_keys.gd:35); open_file skips Session.read and `loaded` (city_saves.gd:128-135).
- LAYER: set_levels and set_sim_switch outside dispatch and StoreApi (city_store.gd:134-141); pause a set_speed, not a GameFlow
  move (game_keys.gd:97, hud_top_bar.gd:45); buttons built in code (hud_top_bar.gd:12-13, start_screen.gd:43-47,
  inspector_panel.gd:95-97, road_panel.gd:114-119); text outside tr and units outside Formatters (start_screen.gd:39,
  road_panel.gd:76,92); game numbers in road_panel.gd:7-16.
- Tool note: the tracer's `--broken` and `--wrong` gave 950 entries, mostly noise (to register against tools/tracer).

## Roads, commands, terrain (native/src/sim/roads, sim/commands, sim/terrain)

Verdict: mostly yes: commands change roads, buildings, lots, terrain and money only through the `j_*` journal
(journal_ops.cpp:147-420); undo replays the same records (edit_history.cpp:69-174); changes told through the ChangeLog
(road_network.cpp:266-288, compose.cpp:65,81); no layer breach; statics only thread-local scratch. The weakness is ids.

- LIFECYCLE / DEFECT: `RoadNode.links` keep segment ids never removed (road_network.h:75-94, journal_ops.cpp:243,
  road_network.cpp:102): `move_node` frees every arm and lays them again, the new pieces taking the freed ids in reverse, so
  hand-made links and bans move onto other arms (road_move.cpp:56,77); `split_segment` gives the first half the old id, so links at
  node b dangle and drop unsaid (road_edit.cpp:132-135, lane_graph.cpp:103); a new road into a junction inherits a deleted road's
  bans. FlowStats by node and segment id never forgotten (flow_stats_types.h:24-29, flow_stats_query.cpp:89,101 never called).
- DEFECT: rolling back a refused command runs finish_command(s, true) and touch(): a refused road marks the city unsaved and drops
  every kept furniture placement (edit_history.cpp:240,201, road_edit.cpp:159-160); `toggle_lane_link` indexes nodes[node] with no
  range or alive check (road_tabs.cpp:191), its `built` list read under a separate lock before the edit (city_core_lanes.cpp:112-117).
- PROPAGATION: `auto_signal` changed by apply_flow_control and read by furniture placement, but FurnitureKept drops only on
  street_revision and no chunk is marked: signal posts and plates stay stale (flow_control.cpp:86-87, ped_furniture.cpp:136,
  furniture_kept.h:66-69); `network_changed` set and never read nor cleared (road_network.cpp:98,120,171,186); RoadNetwork::version
  and transit_revision polled (transit_plan.cpp:178, city_core_inspect.cpp:230, transit_renderer.cpp:212); finish_command reads the
  traffic's `net_nodes` queue as its own input (road_edit.cpp:136).
- SILENT: lay_river ignores cut_trees_at's false at the clearing limit (land_commands.cpp:45); `hint.road.invalid` missing.
- OWNER: Heightfield's own free list beside SlotIds (heightfield.h:78); FACE_REACH_M 80 against 40 (land_commands.cpp:23,
  model_place.cpp:22); LOT_LOOK_M citing a constant gone (road_network.cpp:11); what a crossing, U-turn or marking change marks
  written in the command and again in undo (road_crossing.cpp:42, road_copies.cpp:70, edit_history.cpp:89,96,129).
- LAYER: the api writes `new_road_flags`, `new_road_lift` outside commands and the journal (city_core_street.cpp:130-150).

## Traffic and transit (native/src/sim/traffic, sim/transit)

Verdict: mostly yes: pure C++, state in CityState (city_state.h:70,118,122), time through traffic::step, the player through
commands, the ChangeLog drained under its mutex (lane_graph_edit.cpp:159-173, transit_plan.cpp:131-135), tunables from the
catalogue, output through PoseBuffer and the TripleBuffer (traffic_system.cpp:106-182); no hidden state (thread_local scratch
only; walkers' doors guard reused ids with a seed, ped_doors.cpp:84-87).

- DEFECT / LIFECYCLE: `TransitStop::seg` set once and never updated: after split_segment the first half takes the freed id
  (LIFO), so a stop on the second half gets its kerb lane clamped to the first half's end and buses stop at the new junction; a
  stop on a deleted road attaches to whatever road takes the id; transit_ground.cpp:53 has no alive check (transit_data.h:22,
  transit_commands.cpp:89, road_edit.cpp:41-43, transit_plan.cpp:20-30).
- LIFECYCLE: forget_parking_lot reads `buildings.items[id]` when it drains, a reused id reading the new building, so the old site's
  kerb bays stay (parking_sources.cpp:279-291; the boxes taken at change time are at lane_graph_edit.cpp:176); the cached YardPlan
  keyed by bay index while drop_far frees bays LIFO without erasing `pk.yards`, so no lorry docks after the camera leaves and comes
  back (parking_state.h:150, yard.h:63, parking_sources.cpp:147, lorry_traffic.cpp:277); `Lorry::building` and `bay` not released:
  a lorry unloads at a demolished lot, a new building with the id counted as served (parking_state.h:114, lorry_traffic.cpp:264);
  `Bus::line_index` valid only until transit_revision moves (transit_sim.cpp:195).
- PROPAGATION: apply_flow_control changes `auto_signal` with no touch and no chunk marked (flow_control.cpp:86-87,
  ped_furniture.cpp:136,173); plan edits reach the run only through `transit_revision` and `roads.version` polled every tick
  (transit_plan.cpp:177-178); `network_changed` dead (change_log.h:31).
- SILENT: a bus line with fewer than two stops with a kerb lane, or a rail line with a leg without track, accepted and never run,
  unsaid (transit_plan.cpp:43, rail_run.cpp:125, transit_commands.cpp:138-152); transit_normalize drops a save's malformed stops
  and lines unsaid (transit_data.cpp:107-117, save_read.cpp:264); an unknown switch in set_sim_switch ignored.
- OWNER: cell_key three times (ped_apart.cpp:17-23, walker_cells.cpp:13-15, transit_data.cpp:15-17); the lane key
  `ds * MAX_LANES + lane` written inline 54 times in 20 files; tunables in code (lorry_traffic.cpp:20, transit_sim.cpp:19-20,
  window.cpp:21-24, demand.cpp:18-21, meso.cpp:21, junction_admission.cpp:19).
- LAYER: TrafficLevels and the camera position written by the api directly (city_core.cpp:206-229).

## Buildings, lots, citygen, simulation, economy, demolition, nature, street (native/src/sim)

Verdict: mostly yes: layers hold (no Godot, render or api included), records live in CityData, BuildingStore tells its changes to
the ChangeLog (building_store.cpp:135-143), lots rebuilt only from `lot_cells` (lot_cache.cpp:82-100), growth decides under the
shared lock and drops its decision when the revision moved (city_growth.cpp:82-113). Holes: building ids that outlive their
building, and a second change channel of polled revisions.

- LIFECYCLE: a fire follows the id, not the building: `Fire::building` (damage_state.h:59) removed only by strike_buildings.cpp:81,
  so bulldozing or undoing a burning building moves its fire to the next building with that id (effect_world.cpp:134-138);
  `InteriorView::target` never cleared (view.h:15,20); a removed raised road refunded at a fresh node's lift 0 when the same command
  frees and makes a node (road_economy.cpp:22, road_network.cpp:47).
- DEFECT: growth builds over rubble after 90 s (`lot_occupied` ignores ruins: growth.cpp:77-92; damage_state.h:25-26 says a ruin
  blocks its lot).
- STATE-OUTSIDE: `Simulation::owed_` survives a new city (simulation.h:55, simulation.cpp:39-56).
- PROPAGATION: a second channel compared every frame: the damage `Revised` lists (damage_state.h:66-119), sky and fog revisions
  (nature_state.h:44-45), the interior revision, the fire list.
- LAYER: `set_day_hour` writes the clock with no command and no touch (city_core.cpp:139); `generate_map` called from the api, not
  a command (city_core.cpp:113); sim/buildings/growth.h implemented in sim/simulation (building_store.cpp:4,129).
- OWNER: zone codes four times under two names (road_gen.h:16-17, massing_core.cpp:17-18 commercial; building_store.cpp:14,
  growth.cpp:23 industrial), the base-zone mapping twice (building_store.cpp:161, growth.cpp:34); the 160 m lot reach twice
  (lot_cache.cpp:15, road_network.cpp:11); the hour formula twice.
- SILENT: a refused speed change dropped (simulation.cpp:58-60, city_core.cpp:133).
- Tunables in code: MAX_OWED 50 (city_growth.cpp:15), OCCUPIED_SHARE 0.8 (growth.cpp:18).

## The render (native/src/render, project/render)

Verdict: mostly yes: no render function takes a non-const CityState, no const_cast; RenderWorld::sync holds MapRead
(render_world.cpp:268); road, prop, tree, cover, pool, water and building jobs copy under a short lock and build unlocked
(mesh_queue.h:56-80, building_renderer.cpp:180); every render list of the ChangeLog drained (render_world.cpp:124-134,
effect_world.cpp:171). Exceptions below.

- DEFECT / LAYER: a data race: effect_bodies.cpp:107-108 reads `s.traffic.peds[id].age` under the shared map lock alone while the
  traffic step holds traffic_mutex and may emplace_back on `peds` (simulation.cpp:84-86, ped_spawn.cpp:121); read too late as well
  (lay_dead reset the slot, pedestrians.cpp:70): undefined behaviour, a dead child drawn as an adult.
- DEFECT / STATE-OUTSIDE: a knocked-down body's time runs on frame time while the walker's `down_until` runs on simulation time
  (body_renderer.cpp:255, traffic_system.cpp:156): at speed 2 or 4 the person shows twice, paused they vanish; a second knock cuts
  the time down (body_blows.cpp:25 against pedestrians.cpp:104); vehicle occupants' corpses exist only in the render, forever,
  filling the 64 rows and pushing out corpses the simulation keeps (effect_bodies.cpp:102, body_renderer.cpp:15,87,110).
- PROPAGATION: lots on the zones overlay stale (lot commands mark only lot_cells, the overlay refreshes on road keys:
  overlay_build.cpp:128, render_world.cpp:173); crater decals after terraforming laid again only on the next blast (marks.h:50-55);
  F8 marks freeze once the undo stack reaches changes_shown (change_marks.gd:40); `set_lamp_night` dispatched as a city change every
  frame through dusk and dawn (sky_view.gd:128, city_store.gd:98-99); revisions polled (transit_renderer.cpp:212,
  effect_world.cpp:180-193, fog_field.cpp:245); with no render consumer, damage_events grow without bound (city_core.cpp:165).
- LAYER: the zones overlay job builds under the lock (overlay_renderer.cpp:101); transit meshes built on the main thread under
  MapRead (transit_renderer.cpp:219-223).
- LIFECYCLE: the odometer and RowKeeper keyed by vehicle id, no generation (agent_renderer.cpp:173-178,253); a lying body keeps a
  living walker's id (body_renderer.h:86).
- STATE-OUTSIDE: the sign atlas never evicts (sign_atlas.cpp:30, sign_sheet.gd:25,35).
- SILENT: overlay and burst/drip keys read with defaults (overlay_renderer.cpp:33-41, effect_params.cpp:48-68).
- OWNER: the ragdoll step three times (body_renderer.cpp:17, ragdoll.cpp:32, rigid_step.h:15); widest_half recomputed
  (road_cut.cpp:16-17); tunables in shader defaults (night_dim 0.4 against 0.35, marks.gdshader:21, puffs.gdshader:25); 37
  uniforms never set; 533 constexpr floats in the render.

## The tools that could hold the architecture (tools/gates, tracer, atlas, tasks)

Correction (checked 2026-11-13): the tracer does link GDScript to the C++ the core registers, by the text of the command's name:
`trace.py CityCore.build_road_path --back 6 --text-only` with project/game/store and the road tool among the roots reaches
city_store.dispatch and _route "[by text]" and on to road_tool.click and release, in 1.3 s (over-listing the commands a
dispatch may name, Ptools-8). The agent's "it does not follow bind_method" was wrong.

Verdict: only the gates enforce anything (layers by folder, forbidden constructs on syntax: one core owner, one data reader,
render read-only, no Godot in sim, named map writers, central flow, text through tr; 10 owners; 9 state machines with one setter;
magic numbers; the map; schemas for 45 tables; tuning keys; material uniforms; LAB_COPY): 1.0-5.3 s a file. The tracer, the
inventory and the tasks only report; nothing ties code, state, commands or tests to a feature id.

- Defects: every gate run fails on MEMORY_ON since the owner deleted .claude/settings.json (gate_docs.py:157-158, Ptools-121);
  the whole-program `--broken` crashes after 334 s (decide.py:874, max() of an empty box) and `--broken`, `--feature`, `--cost` never
  use the saved analysis (trace.py:174,245-246); `--broken` false positives (113 of 300 on project/game: Dictionary.get counted as
  event firings, enums as state never written, reads through getters missed, engine properties as state, reachable code as never
  reached); its data-key check never fires here (conditions.py:373-386); the tracer exempt from its own size limits (evaluate.py
  1336 lines); `--state` only for one start; the inventory walks the disk, not git.
- Gaps and their cheapest places: each file in exactly one feature (a features.json with a whole-repo rule beside owners.json);
  every refusal key a CSV row (a rule shaped like gate_tuning: every `refuse.*`, `hint.*`, `road.reason.*` literal; the live case
  `hint.road.invalid`, road_blocks.cpp:25); StoreApi, its C++ bindings and its callers (body_turn_gear, set_building,
  plan_all_yards, body_census have no caller in project/); references by id cleaned on removal (a probe of dangling ids in
  edit_fuzz_test); each ChangeLog list with a producer and a consumer (a rule parsing change_log.h); test scenes no suite runs
  (tests/perf/camera_pan, the capture_* scripts).
- Tests today: 62 scene tasks (57 scenes), 15 look plans, 1 capture; verdicts are failure lines in the log (verdicts.py:13): an
  engine `ERROR:` line does not fail a test.

## The core's surface, state, persistence and catalogue (native/src/api, core, sim/state, sim/persistence, sim/catalog)

Verdict: mostly yes, broken at the edges. One CityState; player edits through `run_edit` (edit_result.h:29-37: writer lock, ready
check, history begin, the command, end, a failing command rolled back: edit_history.cpp:228-245); the render shares the lock;
undo, redo and load restore the same ids; a load is all or nothing (decoded aside, counts checked against the bytes: bytes.h:68-131,
then take_city). Results come back as `{ok, error, ...}` (edit_result.h:15-26). No ERR_ macro in the area: errors are result keys;
rule breaches stop only the debug DLL (checks.h:17-21), the release reads record 0 (id_table.h:72-73).

- DEFECT
  - A save with more road variants than the live city fails to open as damaged, and after any open no new variant can be made:
    `load_city` copies the live catalogue (city_core_persist.cpp:130), a vector copy keeps the size and not the reserved room
    (the one reserve: catalog_loader.cpp:88), and road_variants.cpp:150 refuses when size equals capacity; the round-trip test
    passes only because new_map keeps the old city's variants (save_undo_test.gd:56,178). (Read, not run.)
  - Variants drop only when a save is read (save_read.cpp:253): a new map inherits and saves the last city's custom roads.
  - A bad map index is replaced by map 0 but the raw index stored and saved, then refused on opening (city_gen.cpp:71-72,117,
    save_write.cpp:145, save_read.cpp:242).
- LIFECYCLE (ids reused, no generation)
  - `RoadNode.links` keep segment ids never pruned on removal (journal_ops.cpp:243-254, road_network.cpp:102-121); `link_stands`
    only checks the id is an arm (lane_graph.cpp:99): the next road taking the id inherits the hand-made lanes and bans
    (road_network_node.cpp:28-31).
  - `TransitStop.seg` set once (transit_commands.cpp:89), checked only alive (transit_plan.cpp:20): a stop on a demolished road
    attaches to the next road with that id.
  - `InteriorView.target` (view.h:15): the next building with a demolished one's id is drawn cut open.
  - An undo restoring a building id that growth took since moves the grown building to a new id unsaid (building_store.cpp:73-79).
- SILENT
  - `write_atomic` checks neither store_buffer, close, get_error nor the rename: a short write (a full disk) replaces the save and
    deletes the previous copy (city_core_persist.cpp:37-49).
  - A bad profile or zone id becomes 0 (catalog.h:95,101); load checks some ids and not others (save_read.cpp:91-108,
    save_lots.cpp:38, save_links.cpp:37-44).
  - The catalogue reader type-checks numbers only; texts, flags, lists and tables of the wrong type converted quietly; several keys
    fall back to defaults though every key is said required (catalog_loader.cpp:26-39,183-184,202, furniture_loader.cpp:153-186,
    nature_loader.cpp:86).
  - `run_edit` returns `{}` with no city ready, which dispatch takes for success (edit_result.h:32); road_spec_make returns -1 for a
    rejected spec and for a full table alike (city_core_road_spec.cpp:96-100).
- LAYER: the api changes the city outside sim/commands: set_style (city_core_edit.cpp:85-91), set_day_hour with no lock
  (city_core.cpp:139), the road tool's settings (city_core_street.cpp:129-150), road_variant and road_spec_make add catalogue
  entries (city_core_commands.cpp:214-222, city_core_road_spec.cpp:93-101), road_spec_make routed as a query, skipping dispatch.
- OWNER: `marking_style` (an undoable edit) and `style` live in the city's data but are saved by GDScript in `<slot>.cfg`
  (session.gd:23-24,62-63; a missing .cfg opens with style 0 unsaid); two loaders hard-code "roadcraft" (catalog_loader.cpp:79,
  nature_loader.cpp:124); the hour formula twice, one under the writer lock and one not (simulation.cpp:37,
  city_core_people.cpp:330).
