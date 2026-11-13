# Backbone: records kept true (T037, architecture.md backbone items 4 and 5)

State: `built` (t037, 2026-11-13). Not a reference feature. It fixes how this game keeps every reference between records true
through removal, re-laying, refusal and opening a save, and how it proves that with invariants that run in the core. Each
site below was read again in the code after t036 (02beaa45). Some line numbers moved since audit.md was written
(city_core_persist.cpp:130 is now :167).

Verdict: 18 items, all 18 real today, none already fixed, none not real. The invariant runner does not exist yet: the only
runtime check is the lane graph's (`lane_graph_fault`, lane_graph_edit.cpp:231-233, compiled under `DEBUG_ENABLED`, not under
the project's `CITY_CHECKS` of core/checks.h:11-15).

## Why the references go stale

- Ids are reused most recently freed first, with no generation (core/slot_ids.h:1-4, 18-22). Road segments come from
  `free_segs.next` (road_network.cpp:78), nodes from `free_nodes.next` (road_network.cpp:44), buildings from `BuildingStore`'s
  `free_ids` (building_store.cpp:54, 77-80).
- `RoadNetwork::remove_segment` (road_network.cpp:102-121) removes the id from its nodes' `segs` and nothing else. Every table
  that names a segment, node or building by id keeps that id after the record dies, and the next record that takes the id
  inherits what the old one had.
- The one journal path removes each kind of record: `j_remove_segment` (journal_ops.cpp:243-254), `j_remove_node_if_orphan`
  (:159-168) and `j_remove_building` (:297-305). Undo and redo replay `OpKind` records (edit_history.cpp:146-151, 183-202).
  Every fix below drops or remaps the dependents through journal ops in the same command. Undo replays those ops backwards,
  so it stays exact, and edit_fuzz's hash walk (edit_fuzz_test.gd:78-88) keeps proving it.

## The items

Each item gives the defect as it stands, the paths it takes, the one owner function, the fix, the invariant that catches it
(feature.check: what it asserts, its cost, where it runs) and the case that provokes it. "Scene" means the new directed scene
`project/tests/core/records_true_test.gd`, one function per case. "Fuzz" means edit_fuzz_test.gd.

### 1. Lane links and bans through a road's removal: real

- Defect: `RoadNode.links` (road_network.h:94) holds `LaneLink{from, from_lane, to, to_lane}` (:75-78). Bans are links with
  both lanes `LINK_BANNED` (:63, :76). `remove_segment` never prunes them (road_network.cpp:102-121). `link_stands` hides a
  dead link only while its id is not an arm (lane_graph.cpp:95-102). `movement_banned` checks no arm at all
  (road_network_node.cpp:28-31). So a new road that takes the id at that node inherits the hand-made lanes and the bans
  (lane_graph.cpp:198 reads the ban).
- Paths: every `j_remove_segment` caller: demolish_segment (road_commands.cpp:252-258), remove_node (road_tabs.cpp:174-186),
  strike_roads.cpp:74, road_bend.cpp:110, merge_joint (road_commands.cpp:160-161), reverse_road (road_copies.cpp:29), the
  profile flip (road_tabs.cpp:99), the roundabout cut (roundabout.cpp:68), move_node (road_move.cpp:56) and split_segment
  (road_edit.cpp:40).
- Owner: `j_remove_segment` (journal_ops.cpp:243). Removal is the one place every path passes through.
- Fix: before `s.roads.remove_segment`, for each of `g.a` and `g.b`, if any link names `seg`, call
  `j_links(s, n, links without seg)` (journal_ops.cpp:350-359 records `NODE_LINKS` before and after). Undo puts the links back
  after the segment returns, because ops replay in reverse.
- Invariant `roads.links_on_arms`: for every live node, every link's `from` and `to` are in `node.segs`, and those segments
  are alive. This covers bans too. Cost O(links) of the scoped nodes after a command, O(nodes + links) at a test's end.
- Case (scene `bans_after_removal`): lay a cross of two roads, ban arm A to arm B (`set_movement_banned`), demolish arm A at
  its middle. The invariant names the node. Then lay a new road into the junction: before the fix its legs show it banned
  (the `legs[].banned` query that fuzz_road_shapes.gd:42 uses).

### 2. Lane links and bans through move_node: real

- Defect: move_node removes every arm (road_move.cpp:56) and lays each again in order (:67-85). The new pieces take the freed
  ids in reverse (slot_ids.h:19-22), so the links and bans at the moved node and at the far nodes land on other arms. If the
  node merges into another one (`end.node != node`, :86-89), its links die with the orphaned node.
- Owner: move_node (road_move.cpp:30). Fix: before the removal, take `refs_of(s, sid)` for every arm (the links naming it at
  both its nodes, and its stops: item 4). After each `lay_road` returns its `pieces` (:77), call
  `carry_refs(s, refs, old, pieces)`, which re-points every ref to the piece that now meets the same node. That piece is the
  `far` node's, or `end.node` for the moved node. Links held by the moved node move to `end.node` through `j_links`. Item 1's
  drop in `j_remove_segment` runs first, so `carry_refs` writes the carried set as a second `NODE_LINKS` op.
- Invariant: `roads.links_on_arms` does not see this case, because every wrong id is still an arm. Without identity no cheap
  check can tell the right arm from the wrong one (see "Handles" below). The scene holds this case.
- Case (scene `bans_after_move`): build a three-arm junction with a ban from the arm toward far node P to the arm toward far
  node Q, then `move_node` 10 m. The ban must stand between the arms whose far nodes are still P and Q, read through the legs
  query by far node. Before the fix it names other arms. Fuzz also gains a `move_node` kind (store_api.gd:16 binds it; fuzz
  has none: edit_fuzz_test.gd:131-155).

### 3. Lane links and bans through split_segment: real

- Defect: split_segment removes `seg`, then lays `a -> n` first, which takes `seg`'s id again, and `n -> b` second
  (road_edit.cpp:40-43). Links at `rec.a` still match by luck. Links at `rec.b` name the first half, which does not touch
  `rec.b`, so they are dropped unsaid (lane_graph.cpp:99). Callers: anchor_node (road_edit.cpp:60, 66) for every road laid on
  another, road_crossing.cpp:39 and :83, road_tabs.cpp:73, strike_roads.cpp:64 and :70, and road_commands.cpp:52.
- Owner: split_segment (road_edit.cpp:32). Fix: `refs_of(s, seg)` before :40, then `carry_refs(s, refs, seg, {first, second})`
  after :43. The links at `rec.a` go to the first half, those at `rec.b` to the second.
- Invariant `roads.links_on_arms` catches it: after the drop in `j_remove_segment` with no carry, node b holds no link. Before
  item 1's fix, node b holds a link whose `from` is not an arm.
- Case (scene `links_after_split`): make a hand link at node b of road R (`flip_lane_link` with a built link from
  `junction_lane_map`), then `split_road` at R's middle. The invariant names node b, and `junction_lane_map(b)` must keep the link on the
  second half.

### 4. Transit stops through split: real

- Defect: `TransitStop::seg` (transit_data.h:22) is set once (transit_commands.cpp:89) and never updated. After a split the
  first half takes the id (road_edit.cpp:42). A stop on the second half then reads the first half's kerb lane, clamped to its
  end (transit_plan.cpp:19-31), so buses stop at the new junction.
- Owner: split_segment, through the same `refs_of` / `carry_refs`. A stop goes to the piece nearest its `at`
  (`closest_t`, as transit_plan.cpp:23). Journal it with a narrow op `j_stop_road(s, index, seg)` that records one
  `EDIT_STOP` (`Edited<TransitStop>`, undone by edit_history.cpp:58) and raises `transit_revision`. `j_transit`
  (journal_ops.cpp:361-372) copies and diffs the whole plan, which is too much per removed road.
- Invariant `transit.stop_on_its_road`: every bus stop with `seg >= 0` sits within `profile(seg).half() +
  transit_stop_reach_m` of its segment's curve. Cost O(bus stops), one `closest_t` each, after a command and at a test's end.
- Case (scene `stop_after_split`): place a stop near the b end of road R and `split_road` at R's middle. The invariant names
  the stop, and `transit_run` must give it a lane on the second half. Fuzz already mixes `transit_stop` (edit_fuzz_test.gd:178)
  with `split_road` (:148), so the debug run after every command catches it there too.

### 5. Transit stops through removal: real

- Defect: a stop on a demolished road keeps the dead id. transit_plan.cpp:20 skips it while the id is dead, but the next road
  that takes the id, anywhere, gets the stop. transit_ground.cpp:53-54 reads `segs[q.seg]` with no alive check.
- Owner: `j_remove_segment`. Fix: every stop with `seg == removed` gets `j_stop_road(s, i, -1)`. The stop stays, inert and
  unserved, as the reference skips a stop with no lane (its sim/transit/transit.ts:420), until the player moves or removes it.
  Re-laying paths re-point it through `carry_refs` (items 2 and 3). transit_ground.cpp:53 also gets the alive check. Cost:
  O(stops) per removed segment. Stops are placed by the player, so the table is small. If the scale probe shows otherwise, an
  index by segment in `TransitData` replaces the scan.
- Invariant `transit.stop_road_live`: every bus stop has `seg == -1` or a live segment. Together with item 4's check it
  catches the reuse (a live but far road). Cost O(bus stops).
- Case (scene `stop_after_removal`): place a stop on R and demolish R. The invariant names the stop. Then lay a road
  elsewhere, which takes R's id: before the fix the stop attaches to it (`transit_run` serves it).

### 6. The interior target: real

- Defect: `InteriorView::target` (view.h:15) is checked only against the id (view.h:20, `cuts`). Nothing clears it when its
  building goes. The next building that takes the id is drawn cut open. A load resets it (city_core_persist.cpp:188); a
  demolition does not.
- Paths: every building removal. `j_remove_building` has 10 call sites (demolish_box.cpp:39, road_edit.cpp:90,
  road_commands.cpp:221, strike_buildings.cpp:78, zone_commands.cpp:60, lot_commands.cpp:67, land_commands.cpp:154,
  model_place.cpp:126, body_blocks.cpp:171, api city_core_survey.cpp:56). Undo and redo remove through `exec`
  (edit_history.cpp:146-147). `BuildingStore::put` evicts overlaps unjournaled (building_store.cpp:81-89) and moves a live
  record to a new id (:73-79).
- Owner: a new `building_gone(CityState&, int id)` in sim/commands (one place for every building dependent: items 6 and 7),
  called by `j_remove_building` and by the `exec` cases. `BuildingStore::put` returns the ids it evicted and the id it moved,
  so `exec` can call `building_gone` on them, or `building_moved(from, to)` for the move. Fix here: if
  `s.interior.target == id`, close the view (`set_interior_view` with `on = false`, interior_commands.cpp:49-69, which marks
  the chunk). The view is not undone (view.h:2), so an undo leaves it closed.
- Invariant `interior.target_same_building`: `!on`, or the target is alive and `items[target].seed == interior.seed`. A new
  `seed` field is set when the view opens. Building seeds already identify a building across edits (damage_state.h:44).
  Cost O(1), after a command.
- Case (scene `interior_after_demolish`): `set_interior` on building X (interior_test.gd:42 shows the call), demolish X, then
  `draw_building` elsewhere, which takes X's id. The invariant fails before the fix, and the `interior` query must report
  `on = false`.

### 7. Fires: real

- Defect: `Fire::building` (damage_state.h:59) is erased only when a blow brings the building down
  (strike_buildings.cpp:80-81). A bulldozed or undone burning building hands its fire to the next building with its id: the
  render burns whatever is alive at the id (effect_world.cpp:134-138). Fires are not saved (damage_state.h:127).
- Owner: `building_gone` (item 6) erases fires with `building == id`, and `building_moved` re-points them.
  strike_buildings.cpp:80-81 then folds into `building_gone`. Fires are not journaled (not undone), as today.
- Invariant `damage.fire_on_its_building`: every fire's building is alive, and its `x, z` equal the fire's `x, z` (set at
  ignition, strike_buildings.cpp:97). Cost O(fires), at most `fire_max` (blast.json strike.fire_max 3).
- Case (scene `fire_after_demolish`): strike a building at the force strike_test.gd uses until the `fires` count
  (city_core_strike.cpp:50) is above 0, demolish it, then draw a building that takes its id. The invariant names the fire.

### 8. Parking lots: real (a narrow window)

- Defect: `forget_parking_lot` reads `buildings.items[id]` when the traffic drains (parking_sources.cpp:280-288), not when the
  building changed. If the id was freed and taken again before the drain (two commands with no tick between, or a command
  then growth), it reads the new building's box. The kerbs at the old site are never read again, so their bays stay laid
  off a gate that is gone. The log already keeps boxes for this reason (change_log.h:40-48), but `lots` is deduplicated
  apart from `fronts` (lane_graph_edit.cpp:170-186).
- Owner: the traffic's drain, `apply_network_changes` (lane_graph_edit.cpp:159). Parking is traffic state drained from the
  ChangeLog: sim/commands cannot reach it, and sim/traffic may not include sim/commands (tools/gates/rules.json:162-170).
- Fix: `BuildingStore::changed` (building_store.cpp:135-144) pushes `(id, box)` pairs into one list. The drain hands each box
  to `forget_parking_lot(s, id, box)`, which reads kerbs by the box it was given, never by `items[id]`.
- Invariant `parking.lot_sources_live`: every key of `park.lots` is an alive building, and its bays lie within that
  building's reach. Cost O(lot sources + their bays). These are window-bounded (park_window_m), so the check runs only after
  a drain: after a batch of ticks. The stale kerb itself has no cheap check, so the scene holds it.
- Case (scene `kerbs_after_lot_reuse`): lay a street and step until its kerb bays are read (`bay_census`, N0). Draw building
  X with a car gate on it and step (N1 < N0). Demolish X and draw a building far away (it takes X's id) with no tick between,
  then step. The kerb must come back to N0; before the fix it stays at N1.

### 9. Yards: real

- Defect: `pk.yards` caches a `YardPlan` by building (parking_state.h:150). Its `DockWays::bay` holds global bay indices
  (yard.h:63). `drop_far` frees a far lot's bays (parking_sources.cpp:145-149) through `drop_source` (:115-132), which pushes
  them on `free_bays` and leaves `yards` alone. When the lot is read again its bays take other indices, so `call` matches no
  dock, or a wrong one (lorry_traffic.cpp:270-276). After the camera leaves and comes back, no lorry docks there, or one
  backs into the wrong dock.
- Owner: `drop_source` / the lot's read (parking_sources.cpp:115). Fix: a plan names its docks by their place among the
  lot's own bays (`ParkSource::bays` order), resolved to a global index at `call` time. The costly plan (`plan_ms`) is kept
  across the camera's moves. If the read's dock order is not deterministic for the same lot and roads (to be confirmed in
  the code first), the fallback is to erase `yards[building]` in `drop_source`.
- Invariant `parking.yard_docks_live`: for every `yards[b]`, each dock with a way resolves to an alive dock bay whose
  `source == b`. Cost O(planned docks), after a batch of ticks.
- Case (yard_test.gd, a new step after `_site`): with a yard planned and a lorry docked, move the camera past
  `park_window_out_m` and step, come back and step. The invariant must hold and `lorries_docked` must grow again.

### 10. Lorries: real

- Defect: `Lorry::building` and `bay` (parking_state.h:114) are never released when the building goes: `forget_parking_lot`
  (parking_sources.cpp:270-293) leaves `pk.lorries` alone. The lorry unloads at a demolished lot, and a new building with
  the id counts as busy (lorry_traffic.cpp:264).
- Owner: `forget_parking_lot`. Fix: every lorry with `building == id` gives up (`give_up`, lorry_traffic.cpp:98-106, which
  opens its lanes and sends it on a trip). A lorry already `LEAVING` or `JOINING` finishes its way out.
- Invariant `parking.lorry_target_live`: every lorry before `LEAVING` has an alive building and `bays[bay].alive`, with
  `bays[bay].source == building`. Cost O(lorries), at most `lorry_most`, after a batch of ticks.
- Case (yard_test.gd): while a lorry is `COMING`, demolish its building and step. The invariant must hold and the lorry must
  be back in the traffic.

### 11 and 12. Flow statistics and the control advisor: real

- Defect: `FlowStats::counts`, `rates` and `held` key by node and segment id (flow_stats_types.h:24-30).
  `ControlAdvisor::choices` and `changed_at` key by node (control_advisor_types.h:21-22), and `FlowControl::advised` holds
  node ids (flow_control.h:22). `keep_only`, `drop` (flow_stats_query.cpp:89-106), `forget` and `drop`
  (control_advisor_query.cpp:66-80) have no caller anywhere. A new road on a freed id inherits its predecessor's flow. A
  junction rebuilt on a freed node inherits the old choice and its `changed_at` hold. A node made again resets
  `auto_signal` (road_network.cpp:47), but the advisor still holds the old choice. This code is translated (LAB_COPY,
  Proads-11; flow_stats_types.h:2). The fix lives in the game's side, flow_control.cpp, and does not grow the translation.
- Owner: the traffic's drain (lane_graph_edit.cpp:159-228), since this is traffic state. Fix: (a) `j_remove_node_if_orphan`
  (journal_ops.cpp:159-168) also logs `log.net_nodes_removed`, a new ChangeLog list that is absorbed and cleared with the
  others (change_log.h:71-80, 83-109). (b) For each removed segment (its dead record keeps `a, b`: road_network.cpp:102-121
  never clears them), the drain erases that leg at both nodes. (c) For each removed node it calls `flow->drop`,
  `advisor->drop` and `advised.remove`. A crossing or control edit (net_nodes alone) keeps the choice.
- Invariants `flow.stats_on_live_legs` and `flow.advisor_on_live_nodes`: every node key of `rates`, `counts` and `held` is
  alive, every leg key is in its `segs`, and every key of `choices` and `advised` is an alive node. Cost O(keys). Only
  counted junctions are keyed (window-bounded: flow_control.cpp:16-21). Runs after a batch of ticks. A node freed and made
  again before one drain passes these checks: only item 3 handles would see it.
- Case (flow_control_test.gd, new step): `flow_push` a junction to a signal (city_core_flow_probe.cpp:71), demolish one arm
  and step. The invariant names the stale leg. Remove the junction, lay a new one that takes the node id, and step:
  `flow_state(n)` (:95) must show no choice.

### 13. A refused command touches the city and drops the furniture: real

- Defect: a refused command with journal ops is rolled back by `apply_entry(s, tail, false)` (edit_history.cpp:234-240),
  which ends in `finish_command(s, true)` (:201). That raises `edit_revision` and calls `touch()` (road_edit.cpp:158-160):
  `revision++` marks the city unsaved and `street_revision++` (city_state.h:98-102) drops the kept furniture of every
  street on the map (furniture_kept.h:66-69). One refused click re-places the whole map's furniture (R1). The j_* ops that
  touch by themselves (journal_ops.cpp:328, 338, 347, 358, 371) touch even when their command is then refused.
- Owner: `EditHistory::begin` / `end` (edit_history.cpp:218-245). Fix: `begin` keeps `revision`, `street_revision` and
  `edit_revision`, and `end(!ok)` restores them after the rollback. The state is byte-identical again, so nothing made for
  the old revisions is stale. No reader could have read the raised values, because the command holds the map exclusively
  (edit_result.h:39). The derived settle in `finish_command` still runs.
- Invariant `edit.refusal_inert`: after a refused `run_edit`, the three revisions equal their values at `begin`. Cost O(1).
  It runs in the debug DLL in `run_edit` (edit_result.h:38-45) and needs no test-end pass.
- Case (scene `refusal_inert`): lay road A, then a road that starts on A's middle (a split op) and runs along A (an overlap,
  refused). Before and after, `CityStore.snapshot.unsaved` (save_undo_test.gd:177) and the `furniture_spots` of a street far
  away must not change, and the debug run must report no `edit.refusal_inert`.

### 14. flip_lane_link checks its node: real

- Defect: `flip_lane_link` indexes `nodes[node]` with no range or alive check (road_tabs.cpp:191), and never checks that
  `link.from` and `link.to` are arms. Its `built` list is read under a separate lock before the edit
  (city_core_lanes.cpp:110-117), from a lane graph that may not have drained the last commands yet. On the debug DLL an
  out-of-range id traps (libc++ hardening, CMakeLists.txt:133); on the release DLL it is undefined behaviour.
- Owner: flip_lane_link (road_tabs.cpp:188). Fix: refuse `hint.control.notJunction` (strings.csv:497; already the refusal
  of set_movement_banned, road_tabs.cpp:161-163) for a bad node or a link whose roads are not its arms. `built_links` moves
  from api (city_core_lanes.cpp:29-38) to sim/traffic as `links_built(const CityState&, int node, out)`. The command reads
  it inside its own writer hold, where the traffic cannot step: the tick needs the map shared (simulation.cpp:84-86). The
  command keeps only the links whose roads are arms now. The api passes no list.
- Invariant: none. This guards a crash, and `roads.links_on_arms` then holds what the command writes.
- Case (scene `lane_link_bad_node`): `flip_lane_link` on node -1, on node 10^6, on a dead node, and with a `from` that is
  not an arm. Each is refused with the key, with no trap and no undo step.

### 15. auto_signal's change told to the furniture and the render: real

- Defect: `apply_flow_control` (flow_control.cpp:78-90) writes `node.auto_signal` and logs only `net_node` (:86-87). The
  walkers' furniture is forgotten in the drain (lane_graph_edit.cpp:209-216). But `FurnitureKept` drops only on
  `street_revision` (furniture_kept.h:66-69), which a derived change must not raise, and no chunk is marked. The signal
  posts and plates (ped_furniture.cpp:136, 145-149, read KEPT at :173) stay as they were until some other edit.
- Owner: `traffic::apply_flow_control`, run under the tick's writer hold (simulation.cpp:90-94). sim/traffic may not include
  sim/commands (rules.json:162-170), so the marks are written there. Fix: for each node whose `auto_signal` changed, for each
  arm: forget that segment's kept placement (a new `FurnitureKept::forget_seg(seg)`, O(1) by key, beside `forget_meeting`
  at :56-59), and push the arm's chunks to `log.prop_chunks` (change_log.h:23). No `touch()`: the value is derived and never
  saved (road_network.h:87).
- Invariant `furniture.kept_matches_control`: each kept entry stores the two `signalled` bits it was placed with
  (ped_furniture.cpp:136), and they equal `control_of` at both ends now. Cost O(kept entries), which are bounded by what the
  render and the walkers read. Runs after a batch of ticks.
- Case (flow_control_test.gd, new step): read `furniture_spots` on an arm of a junction on automatic, `flow_push` it to a
  signal and step. The invariant must hold and a signal post must stand on the arm.

### 16. `network_changed` removed: real (dead)

- Defect: written at road_network.cpp:98, 120, 171, 186 and save_read.cpp:293, carried at change_log.h:97-98, declared at
  change_log.h:31. It is never read and never cleared.
- Owner: ChangeLog (change_log.h). Fix: delete the member, its five writes and the two absorb lines.
- Invariant: none at runtime. The build proves nothing reads it. A gate rule (each ChangeLog member with a producer and a
  consumer, audit.md:172) is left to backbone item 7.

### 17. The catalogue's road variants survive opening a save: real

- Defect: `load_city` copies the live catalogue into the city read aside (city_core_persist.cpp:167). A vector copy keeps the
  size but not the room reserved at load (catalog_loader.cpp:88), and `road_variant` refuses when size equals capacity
  (road_variants.cpp:150). So a save with more variants than the live city fails as damaged (save_read.cpp:187, 253-254),
  and after any open (`take_city` moves that catalogue in) no new variant can be made.
- Owner: `Catalog` (sim/catalog/catalog.h:82, "room for every variant is reserved at load"). Fix:
  `Catalog::keep_variant_room()` reserves `own + road_max_variants`. The loader calls it (replacing catalog_loader.cpp:88)
  and so does city_core_persist.cpp:167 after the copy. A load path that copies the catalogue must call it.
- Invariant `catalog.variant_room`: `profiles.capacity() - profiles.size() >= road_max_variants - variants.size()`. Cost
  O(1), after a command or a load.
- Case (save_undo_test.gd, new step): new map and save E (no variant), make two variants, save S, open E (the invariant
  fails here before the fix), open S (refused as `save.file.damaged` before the fix), then `road_spec_make` (refused
  `hint.road.variantsFull` before the fix). Next to it, a new map inherits the last city's variants (audit.md:190). That is
  not in T037 and goes to the register.

### 18. Growth never over rubble: real

- Defect: `lot_occupied` (growth.cpp:77-92) sees only buildings, never ruins. The only hold is global and lasts
  `strike.hold_growth_s` (90 s, blast.json:29; city_growth.cpp:90). After it, growth builds over rubble, although a ruin
  "keeps its lot blocked until the player clears it" (damage_state.h:25-26). Callers: city_growth.cpp:42 and growth.cpp:188.
- Owner: `lot_occupied` (sim/simulation/growth.cpp:77). Fix: a lot under a ruin is occupied. `ruin_at` is a scan over every
  ruin (damage_state.cpp:58-65), so `DamageState` keeps a spatial hash of its ruins. It is updated by the ruin mutators
  (damage_state.h:89-108, 164-168; rebuilt on an erase or insert, which happen once per command), so the test is
  O(nearby ruins).
- Invariant `growth.nothing_on_rubble`: no alive building's footprint covers a ruin's centre (`BuildingStore::covers`).
  Cost O(ruins x nearby buildings), after a command and after a batch of ticks.
- Case (strike_test.gd, new step): bring a building down (a ruin), turn growth on and step past `hold_growth_s`. The
  invariant must hold and no building may stand on the ruin's lot. Clear it (`demolish` on the ruin,
  road_commands.cpp:226) and the lot grows again.

## The invariant runner (backbone item 5)

- Where the checks live: a new pure C++ module `native/src/sim/checks/` (`records.h`, one `.cpp` per feature: roads,
  transit, buildings, damage, growth, parking, flow, furniture, catalog). Its layer entry (`"core/", "sim/"`) goes in
  rules.json cpp_layers (rules.json:112-210) and its files go in doc/map.md. Each feature exposes
  `void check_<feature>(const CityState&, const CheckScope&, Broken&)`, which appends one entry per broken record. The lane
  graph's own `lane_graph_fault` (lane_graph_check.cpp) becomes the `traffic.lane_graph` check, and its guard moves from
  `DEBUG_ENABLED` to `CITY_CHECKS` (core/checks.h:11-15).
- What a failure carries: `struct BrokenRecord { const char *feature, *check; int record; }`. `Broken` holds a vector
  reserved once, at `CityState::bind`, to `checks_records_kept` (a new tuning field, CITY_TUNING_FIELDS and tuning.json, no
  number in native/src), plus a `dropped` count past it. `add` never allocates, and a check allocates nothing per record:
  each one is a direct lookup in the table it reads (`node.segs` is a short vector, a ruin uses the new hash). So no search
  and no allocation run in a loop.
- Scope: `CheckScope{ whole, nodes, segs, buildings }`. After a command, the scope is the ids the command logged and the
  traffic has not drained yet (`log.net_nodes`, `net_segs_removed`, `net_segs_changed`, `lots_changed`, read under the same
  exclusive hold). At a test's end it is the whole city. City-data checks (roads, transit, interior, fires, catalog,
  growth, edit) run in both cases. Traffic-owned checks (flow, parking, yards, lorries, furniture, lane graph) run only when
  the ChangeLog's traffic lists are empty, because before a drain those tables are legitimately behind the city. Otherwise
  they report nothing and the probe says `pending`.
- The query: `CityProbes::broken_records()`, beside `graph_fault` (city_probes.h:101, city_core_state_probe.cpp:114). It
  takes the map shared and `traffic_mutex` (as traffic_hashes does, city_core_state_probe.cpp:121-122), runs every check on
  the whole scope, appends what the debug sink collected since the last call, and returns
  `[{feature, check, records: [ids], dropped}]`. Routed as `&"broken_records": "probes/broken_records"` in store_api.gd:67.
  It exists in both DLLs and costs only when asked.
- TestCase.finish (test_case.gd:72-77): after the `checks == 0` test, so a test that checked nothing still fails as itself,
  and when the store booted, `for b in CityStore.query(&"broken_records"): check(false, "records broken: %s.%s %s" % [b.feature,
  b.check, b.records])`. Every scene ends with none broken or fails naming the feature, the check and the ids. Fuzz also asks
  after every step of `_walk` (edit_fuzz_test.gd:114-124), adding the seed and step it already prints.
- The debug DLL after every command: an api RAII hold `CommandHold`, wrapping `map_hold::WriterHold` with
  `Writer::COMMAND`, replaces the bare hold in `run_edit` (edit_result.h:39) and in undo, redo and the other command holds
  (city_core_edit.cpp:24, 29, 34, 39, 81, 86; city_core_commands.cpp:215) and in the load (city_core_persist.cpp:186). In
  its destructor, `if constexpr (CHECKS)`, it runs the scoped city-data checks and `edit.refusal_inert`, still under the
  exclusive lock, into the sink.
- The debug DLL after every batch of ticks: at the end of `Simulation::tick`, under the shared map lock and `traffic_mutex`
  it already takes (simulation.cpp:107-111), `if constexpr (CHECKS)` it runs the traffic-owned checks and
  `growth.nothing_on_rubble` into the sink. Their tables are window-bounded, so no whole-map work happens per tick.
- The sink and the error channel: `BrokenSink` lives in `CityState`'s channels, not in `CityData` (city_state.h:110-136): a
  mutex, the reserved `Broken`, filled from the main thread or the simulation thread. The api drains it on the main thread,
  at the probe and once a frame in CityCore's sync, into one `push_error` per check: "records broken: roads.links_on_arms
  nodes 12, 40 (after cmd.demolish.road)". tools/tasks/verdicts.py fails a test on an engine `ERROR:` line (t042, t036), so a
  broken record fails a scene at the command that broke it, not only at its end. Engine errors may come from any thread
  (Context7 godot-docs, class_logger: `_log_error` "will be called from threads other than the main thread"). Draining on
  the main thread keeps the lines in order and in the test's own output.
- Release: nothing runs. The hooks are `if constexpr (CHECKS)`, so the export carries no branch (core/checks.h:1-3), and
  the probe runs only when asked.
- Budgets (debug only; tests always), measured at block verify on the 24 km city by the scale probe and written into this
  sheet then:
  - after a command, scoped: O(links at the touched nodes + bus stops + fires + 1), target under 0.2 ms;
  - after a batch of ticks: O(window's flow keys + lot sources + planned docks + lorries + kept furniture entries + ruins),
    target under 0.3 ms;
  - the whole set at a test's end: O(nodes + links + stops + ruins + window tables), target under 50 ms.
  If the batch check goes over, it runs every Nth batch, with N in tuning.json and the default 1. No threshold is loosened.
- Handles (architecture item 3) as a recommendation for later. Three cases here have no cheap check without identity: a
  link re-pointed to the wrong arm by move_node (item 2), a node freed and made again before one drain (items 11 and 12),
  and a stop on a reused id at the same place. A generation in `SlotIds` (moved on `release`, carried by the journal's
  records so that undo restores the same one), stored beside the id in `LaneLink`, `TransitStop::seg`, `Fire::building`,
  `InteriorView::target`, `Lorry::building` and the flow keys, would let `links_on_arms` and its siblings compare
  generations. T037 makes do with seeds where they exist (items 6 and 7) and the directed scene cases.

## Faults never brought

- A reference kept past its record's removal, hidden by a lookup that skips dead ids (lane_graph.cpp:99,
  transit_plan.cpp:20, effect_world.cpp:137) instead of being dropped where the record goes.
- A rollback that moves the revisions or drops kept work, i.e. whole-map work on a refused click.
- A check that runs in the export, allocates per record, or walks the whole map per frame.
