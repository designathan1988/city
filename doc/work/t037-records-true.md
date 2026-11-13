# t037-records-true: every reference dropped or carried where its record goes, invariants that prove it (implementation plan)

> **For agentic workers:** REQUIRED SUB-SKILL: superpowers:subagent-driven-development. The owner's order replaces the TDD
> run cycle: each task writes its test and its code with nothing run (no test, build, gate, Godot); every check runs once at
> the block's end (`block verify`). Steps use checkbox (`- [ ]`) syntax.

**Goal:** a record's removal, re-laying or refusal leaves no table pointing at a dead or wrong record, and every test ends by
asking the core's invariants, failing on any broken record by feature, check and id.

**Architecture:** fixes live where the record goes (journal ops and the commands in sim/commands; the traffic's drain for
traffic-owned tables); a pure C++ checks module (native/src/sim/checks) with one function per feature, run scoped after every
command and batch of ticks in the debug DLL (`CITY_CHECKS`), whole at a test's end through a probe.

**Tech stack:** C++20 (godot-cpp only in api), GDScript tests, JSON data with schemas.

**Spec:** doc/specs/003-roadcraft-features/sheets/backbone-records-true.md — items 1-18 (defect file:line, owner, fix,
invariant, case) and "The invariant runner". Each task's brief names its items; the sheet holds their exact sites.

## Global constraints

- Files start with a comment stating their responsibility; English; LF; no magic numbers in native/src, project/game,
  project/ui (tuning in project/data/** with its schema; a new tuning field goes in CITY_TUNING_FIELDS and tuning.json).
- core -> sim/* (no Godot) -> render (read-only) -> api; only sim/commands mutates CityState; sim/traffic never includes
  sim/commands (tools/gates/rules.json cpp_layers); ChangeLog is the one dirty channel.
- Every city change a fix makes goes through journal ops in the same command so undo/redo stay exact (edit_fuzz's hash walk).
- No allocation or search inside a per-record loop of a check; no whole-map work per frame or tick; checks compile to nothing
  in release (`if constexpr (CHECKS)`, core/checks.h).
- Context7 (godot-docs 4.7, godot-cpp) before any Godot API (push_error, Dictionary/Array building in the probe).

## Shared interface (Task 1 writes it first in `native/src/sim/checks/records.h`; the other tasks code against it)

```cpp
namespace city::checks {
struct BrokenRecord { const char *feature; const char *check; int record; };
class Broken { // reserved once (tuning checks_records_kept); add never allocates; past the room it counts `dropped`
public: void reserve(size_t n); void add(const char *feature, const char *check, int record); void clear();
        const std::vector<BrokenRecord> &records() const; size_t dropped() const; };
struct CheckScope { bool whole; const std::vector<int> *nodes; const std::vector<int> *segs; const std::vector<int> *buildings; };
// City-data checks (after a command, scoped; whole at a test's end):
void check_roads(const CityState &, const CheckScope &, Broken &);     // Task 2: roads.links_on_arms
void check_transit(const CityState &, const CheckScope &, Broken &);   // Task 2: transit.stop_on_its_road, transit.stop_road_live
void check_buildings(const CityState &, const CheckScope &, Broken &); // Task 3: interior.target_same_building
void check_damage(const CityState &, const CheckScope &, Broken &);    // Task 3: damage.fire_on_its_building
void check_growth(const CityState &, const CheckScope &, Broken &);    // Task 3: growth.nothing_on_rubble
void check_catalog(const CityState &, const CheckScope &, Broken &);   // Task 3: catalog.variant_room
void check_edit(const CityState &, Broken &);                          // Task 3: edit.refusal_inert (after a refused command)
// Traffic-owned checks (after a batch of ticks, only when the ChangeLog's traffic lists are drained):
void check_parking(const CityState &, const CheckScope &, Broken &);   // Task 4: parking.lot_sources_live, yard_docks_live, lorry_target_live
void check_flow(const CityState &, const CheckScope &, Broken &);      // Task 4: flow.stats_on_live_legs, flow.advisor_on_live_nodes
void check_furniture(const CityState &, const CheckScope &, Broken &); // Task 4: furniture.kept_matches_control
void check_lane_graph(const CityState &, const CheckScope &, Broken &);// Task 1: traffic.lane_graph (lane_graph_fault)
}
```
If a traffic-owned table is not reachable from `const CityState &`, the owning task adds the parameter it needs to its own
function and says so in its report; Task 1's runner is adjusted at verify.

---

### Task 1: the invariant runner

**Files:** Create `native/src/sim/checks/records.h`, `records.cpp` (Broken, the runner `run_city_checks(state, scope, broken)`
and `run_traffic_checks(...)` calling the functions above), `lane_graph.cpp` (check_lane_graph from lane_graph_check.cpp's
`lane_graph_fault`, its guard moved from DEBUG_ENABLED to CITY_CHECKS); Modify `tools/gates/rules.json` (cpp_layers entry for
sim/checks), `native/src/sim/state/city_state.h` (BrokenSink in the channels: a mutex + reserved Broken), the tuning field
`checks_records_kept` (CITY_TUNING_FIELDS, project/data/catalog/tuning.json and its schema), `native/src/api/city_probes.h` +
`city_core_state_probe.cpp` (`broken_records()`), `project/game/store/store_api.gd` (`&"broken_records": "probes/broken_records"`),
an api `CommandHold` (new header beside edit_result.h) used by `edit_result.h` run_edit and the command holds of
`city_core_edit.cpp` and `city_core_commands.cpp`, `native/src/sim/simulation/simulation.cpp` (end of tick),
CityCore's per-frame sync (drain the sink into one push_error per check), `native/CMakeLists.txt` if sources are listed,
`project/tests/test_case.gd` (finish), `project/tests/core/edit_fuzz_test.gd` (ask after every step; a move_node kind)
**Interfaces:** Produces records.h above; `broken_records()` → `[{feature, check, records: PackedInt32Array, dropped}]`;
`CommandHold` (RAII, wraps `map_hold::WriterHold` with `Writer::COMMAND`; in its destructor, `if constexpr (CHECKS)`, runs the
scoped city-data checks and, after a refused command, check_edit, into the sink). Task 3 uses CommandHold at the load
(city_core_persist.cpp).
- [ ] test_case.gd finish: after the `checks == 0` test, when the store booted, each broken entry is a failed check naming
  feature, check and records.
- [ ] edit_fuzz_test: after every step of `_walk`, the same query, failing with seed and step; a `move_node` kind.
- [ ] The scope after a command: the ids the ChangeLog holds undrained (net_nodes, net_segs_removed, net_segs_changed,
  lots_changed); traffic-owned checks only when those lists are empty (else nothing, the probe says pending).

### Task 2: road and transit references (sheet items 1, 2, 3, 4, 5, 14)

**Files:** Modify `native/src/sim/commands/journal_ops.cpp` (the whole file is this task's: item 1's drop in j_remove_segment,
item 5's stop drop, a new `j_stop_road`, and two one-line hooks other tasks define: `building_gone(s, id)` called in
j_remove_building (Task 3, `sim/commands/building_refs.h`) and `s.log.net_nodes_removed.push_back(n)` in
j_remove_node_if_orphan (Task 4 adds the member)), its header; Create `native/src/sim/commands/road_refs.h/.cpp` (`refs_of`,
`carry_refs`); Modify `road_move.cpp`, `road_edit.cpp`, `native/src/sim/transit/transit_ground.cpp` (alive check),
`road_tabs.cpp` (flip_lane_link refusal), `native/src/api/city_core_lanes.cpp`, a sim/traffic `links_built` (moved from api);
Create `native/src/sim/checks/roads.cpp`, `transit.cpp`; Test: create `project/tests/core/records_roads_test.gd` + `.tscn`
(cases bans_after_removal, bans_after_move, links_after_split, stop_after_split, stop_after_removal, lane_link_bad_node).

### Task 3: buildings, damage, growth, refusal, catalogue (sheet items 6, 7, 13, 17, 18)

**Files:** Create `native/src/sim/commands/building_refs.h/.cpp` (`building_gone(CityState&, int id)`,
`building_moved(CityState&, int from, int to)`); Modify `native/src/sim/commands/edit_history.cpp` (+ header: exec's building
cases call building_gone/moved; begin keeps and end(!ok) restores revision, street_revision, edit_revision),
`native/src/sim/buildings/building_store.cpp/.h` (put returns evicted ids and a moved id; `changed` also pushes each
`{id, box}` into `s.log.lot_boxes`, the ChangeLog member Task 4 adds and drains), `view.h` (seed) +
`interior_commands.cpp`, `damage_state.h/.cpp` (ruin spatial hash), `native/src/sim/simulation/growth.cpp` (lot_occupied),
`strike_buildings.cpp` (folds into building_gone), `native/src/sim/catalog/catalog.h` + `catalog_loader.cpp`
(`keep_variant_room()`), `native/src/api/city_core_persist.cpp` (keep_variant_room after the copy; CommandHold at the load);
Create `native/src/sim/checks/buildings.cpp`, `damage.cpp`, `growth.cpp`, `catalog.cpp`, `edit.cpp`; Test: create
`project/tests/core/records_buildings_test.gd` + `.tscn` (interior_after_demolish, fire_after_demolish, refusal_inert); steps in
`save_undo_test.gd` (item 17) and `strike_test.gd` (item 18).

### Task 4: traffic-owned tables (sheet items 8, 9, 10, 11, 12, 15, 16)

**Files:** Modify `native/src/sim/state/change_log.h` (add `net_nodes_removed`, `lot_boxes` (pairs `{int id; Box box}` as
building_store pushes them), absorb/clear; remove `network_changed` with its writes in road_network.cpp and save_read.cpp:293),
`native/src/sim/traffic/lane_graph_edit.cpp` (the drain: lot boxes to forget_parking_lot, removed legs and nodes to the flow
statistics and advisor), `parking_sources.cpp` (forget_parking_lot by box; lorries give up; yard docks by place among the lot's
bays or plan dropped), `yard.h`, `lorry_traffic.cpp`, `flow_control.cpp` (auto_signal change: forget kept furniture per arm,
mark prop chunks), `furniture_kept.h` (`forget_seg`), `ped_furniture.cpp` (kept entries store their signalled bits); Create
`native/src/sim/checks/parking.cpp`, `flow.cpp`, `furniture.cpp`; Test: create `project/tests/core/records_traffic_test.gd` +
`.tscn` (kerbs_after_lot_reuse) and steps in `yard_test.gd` (items 9, 10) and `flow_control_test.gd` (items 11, 12, 15).

---

## Checks (block verify, once)

- Both DLLs, no warning; `run tests` (every scene ends asking broken_records), the three new scenes (suites.json entries by the
  controller), edit_fuzz; gates of each changed file; tools tests.
- Each new invariant seen failing on its defect where a revert is cheap (the scene cases assert the defect's symptom too).
- The cost of the checks measured once on a large map (the scale probe arrives with T038; until then the test-end pass time is
  read from the logs).

## Outcome (2026-11-13)

Four implementers in parallel (nothing run while coded); the build came before the task reviews (cross-task C++ interfaces).
Reviews: Tasks 1, 2, 4 approved; Task 3 had two Important findings, fixed and each shown failing without its fix.

- Runner (Task 1): native/src/sim/checks (records.h/.cpp, one file per feature), the probe `broken_records`, TestCase.finish and
  edit_fuzz (every step) failing on any broken record, CommandHold on every api command hold (debug DLL: scoped city checks after
  each command), traffic-owned checks after each batch of ticks, the sink drained on the main thread into push_error.
- Roads and transit (Task 2): links, bans and stops dropped in j_remove_segment and carried (road_refs) through split, move,
  bend, reverse, merge, profile flip and the roundabout cut; flip_lane_link (renamed from the reference's name) refuses a bad
  node or link and reads the built links in its own hold (links_built in sim/traffic); transit_ground's alive check.
- Buildings (Task 3): building_gone/building_moved (Interior view, fires) from every removal path and BuildingStore::put;
  EditHistory restores the revisions after a refused command; keep_variant_room after load; a ruin spatial hash, growth and the
  player's buildings never over rubble (refuse.rubble), an undone clearing knocks down what grew there.
- Traffic (Task 4): lot_fronts carry {id, box}; the drain forgets parking by the logged box, lorries give up, flow statistics and
  the control advisor drop removed legs and nodes (net_nodes_removed); yard docks by place; an auto_signal change re-places the
  arms' furniture without raising street_revision; network_changed removed.
- Found by the fuzz's new move_node kind (older defects the revert dcfd4c7c had brought back): a tree or paint dab that changes
  nothing recorded an undo step (nature_commands.cpp), painted tiles and elements were saved in hash order (save_nature.cpp).
- Gates: drop_node, toggle_lane_link, node_lanes renamed (forget_node_keys, flip_lane_link, junction_lane_map); traffic_system.h
  split (records_changed.h); records_roads_test split (stop_link_cases.gd); LotBox written plainly.

Checks (fresh, 2026-11-13): both DLLs, 0 warnings; tools/tasks/tests OK; tools/gates/tests OK; gates self-test 97 OK;
`run tests` 68/70 (camera_feel_test, camera_input_test: Pinterface-142, T027); photos 4/4; changed files' gates: registered
findings only (Psimulation-3, -4, -5, -6; Ptraffic-1) and CLAUDE_MD on the owner's uncommitted CLAUDE.md. edit_history.cpp and
journal_ops.cpp grew (368 -> 394, 420 -> 452) inside their registered size entries. Registered: Proads (new map inherits
variants), Ptraffic (deep lot gates). The 24 km cost of the checks waits for T038's probe.
