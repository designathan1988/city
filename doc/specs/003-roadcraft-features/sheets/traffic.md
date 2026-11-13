# Traffic: the area's index (specification 003, T009, T010)

- State: read (2026-11-13, a reading agent; each feature's claims checked in the files when it is built).
- This game's traffic is a port of an older reference (its headers cite traffic.ts, micro.ts, laneChanges.ts, routing.ts,
  meso.ts, gone from the reference): the reference's newer driver layers (src/sim/drive, src/sim/meso) never came.
- Present: kerb stops, car parks and parking, lorries and docks, cyclists (riding at the edge beyond the reference), lane closures
  and obstacles, the improved IDM.

## Features, in the order they are built

| # | Feature | State here | What differs | Evidence (here; reference) |
|---|---|---|---|---|
| 1 | Slowing for bends | ABSENT, high | no curvature inside a lane; every junction turn at a fixed 22 km/h; the reference samples curvature every 2 m, a lateral acceleration 4.0 to 1.8 by driver, x0.7 heavy, braking for bends ahead | vehicle_speed.cpp:22-31, lane_geometry.cpp:13; curvature.ts:12-126, params.ts:134-137 |
| 2 | Lane changes: politeness, indicators, positioning | PARTIAL, high | own gain plus a bias against a fixed 0.25, no politeness, safe brake 3.5, cooldown 3 s, change 2.2 s, bicycles never change; no indicators; no slowing to reach the lane needed | lane_change.cpp:79-125, vehicle_pose.cpp:57; laneChange.ts:12-303, vehicleSignals.ts:19-74 |
| 3 | Cars leave only out of sight | PARTIAL, high | removed on screen when no trip is found, a re-route fails, or after 120 s stuck | vehicle_motion.cpp:174,252,293,299; spawn.ts:320 |
| 4 | Each driver different | PARTIAL, medium | only a share of the limit and a drift phase drawn; the reference's aggression shapes a, b, T, s0, gap acceptance, politeness, threshold, patience | demand.cpp:159-160, traffic_state.h:55; driver.ts:50-75 |
| 5 | Following that anticipates | PARTIAL, medium | no constant-acceleration heuristic (no anticipation of a braking leader); pedal change limited only easing on | vehicle_speed.cpp:43-58, vehicle_motion.cpp:20-35; operational.ts:14-64, physicalMotion.ts:7-19 |
| 6 | The mix of vehicles | PARTIAL, medium | a pickup added; 6 % of traffic as buses with no line; bus and truck accelerations about double | traffic.json classes, demand.cpp:63; archetypes.ts:50-241 |
| 7 | Lanes in the route | PARTIAL, medium | A* over directed segments, the lane fixed later by mandatory changes; the reference treats lane changes as states of the search | routing.cpp:1-47, lane_change.cpp:113-117; tactical.ts:10-38 |
| 8 | Far traffic as flows; an adaptive detailed window | PARTIAL, medium | far vehicles as individual events; a fixed 1500 m window; far cars not drawn; the reference: a link transmission model over the map, demand by 250 m cell, a window 1.35x the view within 300-750 m | meso.cpp:1-23, window.cpp:1-5; model.ts:4-15, meso/demand.ts:8-13, memory.ts:15-23 |
| 9 | Re-routing by patience | PARTIAL, low | fixed times (6 and 10 s) | vehicle_motion.cpp:116,186; router.ts:312-327 |

## Translated from the reference (redone in this game's design when built)

traffic_system.h:1, demand.cpp:1, window.cpp:1, meso.cpp:1, routing.cpp:1, lane_change.cpp:1, junction_admission.cpp:2,
vehicle_motion.cpp:3, traffic_params.h:84, traffic_state.h:28 cite it; route_admission.cpp (SearchBudget, mayRequestSearch,
noteSearch), traffic_state.h:67-68, cycling.cpp:29 carry its names; kerb_stops.cpp, lorry_traffic.cpp, lot_traffic.cpp and the
classes in traffic.json copy its values; manoeuvre.cpp, reeds_shepp.cpp (70 % shared passages), swaths.cpp (94 %), yard.cpp (50 %).

## Reference faults never brought

A second A* for plans starting with a lane change (tactical.ts:123); Sets, filters and sorts in every exit choice
(router.ts:124-263); every vehicle walked in id order each tick (laneChange.ts:52, integrate.ts:21, kerbStops.ts:193); a Map per
lane-change pass (laneChange.ts:420); a per-vehicle bend look-ahead each tick with WeakMap profiles (curvature.ts:22,84-126);
arrays and Sets per kerb stop planned (kerbStops.ts:372-381); a Map per node for crossings (obstacles.ts:62-70); the micro window
following the view (meso/memory.ts:14-27).
