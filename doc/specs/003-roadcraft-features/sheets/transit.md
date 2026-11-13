# Transit: the area's index (specification 003, T018)

- State: read (2026-11-13, a reading agent; each feature's claims checked in the files when it is built).
- Modes in both games: bus, train, metro (reference world/transit.ts:9; here sim/transit/transit_data.h:16). Trams and depots
  exist in neither; neither has a timetable panel nor passengers on trains or the metro.

## Features, in the order they are built

| # | Feature | State here | What differs | Evidence |
|---|---|---|---|---|
| 1 | Random city buses stop to board | PARTIAL | buses drawn at `bus_share` get no errand, so they never stop; line buses already board better than the reference | demand.cpp:63, kerb_stops.cpp:326-327; reference kerbStops.ts:59-141,365-398 |
| 2 | The transit tools' help | PARTIAL | the opening hint says Shift+click makes a terminal, it removes; the per-tool help strings exist but nothing shows them; Backspace on a track redraws only on the next move; stop markers only while the tool is on | strings.csv:361, transit_tool.gd:22-39,105-107, strings_c.csv:344-349 |
| 3 | Picking and previewing without whole-plan walks | this game's own cost | stop and track picks walk every stop and track point on each mouse move (the header claims a cell filing that does not exist); the track preview mesh is rebuilt on every move | transit_data.cpp:220-247, transit_commands.cpp:193-202, transit_data.h:2-3, transit_renderer.cpp:235-248 |
| 4 | Trains redesigned off the reference's costs | translated | every edge scanned per leg, every train compared with every other for the gap, train zones rebuilt with fresh allocations each tick (copied); level crossings at the road's middle line for every lane | transit_data.cpp:32-45, rail_run.cpp:55-83,200-205,250-282 |
| 5 | Buses leave only out of sight | PARTIAL | `bus_away_margin_m` loaded and never used: a bus leaves as it leaves the near window | traffic_params.h:63, transit_sim.cpp:96 |
| 6 | A save's lines clamped on load; trains kept through an edit only when their track is the same | PARTIAL | vehicle counts not clamped or defaulted; trains survive an edit by point count and loop length alone | transit_data.cpp:107-117, rail_run.cpp:132 |
| 7 | Transit vehicles culled by the view | PARTIAL | cars culled by a 900 m radius, not the camera | transit.json:3 |
| 8 | Bus lanes | ABSENT | (the roads area's feature 7) | sheets/roads.md |

## Translated from the reference (redone in this game's design when built)

transit_plan.h:10-12 and transit_plan.cpp:126-128 cite it; transit_data.* (TransitStop, RailTrack, RailGraph, rail_path,
nearest_track, stop_near, longest_on_road), transit_sim.* and rail_run.cpp (place_buses, pass_stop_away, send_bus, step_train,
point_at, cross_at, crossing_near_end, train_zones), transit_parts.cpp (entrance_spot), bus_calls.cpp, transit_overlay.gd and the
`hint.transit.*` keys carry its names one to one.

## Reference faults never brought

The closed-crossing check runs every train for each lane every vehicle looks at (reference sim/transit/transit.ts:533-544); the
gap search over all trains (transit.ts:486); train zones reallocated each tick (transit.ts:547-585).
