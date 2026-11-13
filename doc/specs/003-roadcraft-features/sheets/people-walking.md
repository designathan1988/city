# people: where people walk (footways, crossings, signals, ways into buildings)

- State: read (both games' files read whole; kerb_stops.cpp, the vehicles' side of the kerb, is the traffic sheet's, T009)
- Inventory rows: r2901 `sim/people/engine`, r2902 `sim/people/view`, r2898 `sim/peds/sidewalk`, r2899 `sim/peds/signalPostIndex`,
  r3155 `world/pedestrianAccess` | this game: native/src/sim/traffic/ped_*.cpp, kerb_walkers.cpp, kerb_stops.cpp, bus_calls.cpp

## Reference (read for behaviour; nothing copied)
- Read whole (2026-11-13): src/sim/people/engine.ts 1-79 of 79; src/sim/people/view.ts 1-115 of 115; src/sim/peds/sidewalk.ts
  1-317 of 317; src/sim/peds/signalPostIndex.ts 1-105 of 105; src/world/pedestrianAccess.ts 1-6 of 6 (what each does: the work file
  doc/work/t007-people.md).
- Outputs and rules: kerb nodes two a leg (road half width + half footway out), set back to the crosswalk distance clamped
  (sidewalk.ts:275-286) and pushed by 2 m steps clear of the junction's surface (sidewalk.ts:289-314); one crosswalk a leg over that
  leg's link lanes, none where the crosswalk distance is 0 (sidewalk.ts:151-183); footway lines kerb to kerb (sidewalk.ts:186-236); a
  local rebuild of the junctions and roads an edit touched (sidewalk.ts:74-107); signal posts in 10 m cells for a walker's look ahead
  (signalPostIndex.ts:16-104); highways and ramps carry no walkers (pedestrianAccess.ts:4-6).
- Cost and faults never to bring: string ids and Maps per kerb and crossing (sidewalk.ts:316-317), a separate walk graph rebuilt
  per edit, the posts' Map of cells.

## This game
- Read whole (2026-11-13): native/src/sim/traffic/ped_routes.cpp 1-219 of 219 (routes by A* over streets that carry walkers, cost
  length / pace plus a fixed extra at junctions, the way round a node by the rotation with the fewest crossings, kerb returns walked
  along their arc); ped_kerb.cpp 1-115 of 115 (the queue on the right of the zebra's middle line, an approach point, never on the
  asphalt); ped_ways.cpp 1-126 of 126 (the lines off a sidewalk, crossing waited for by walk phase and gap, a party over on one
  leave, mid-block crossings by ped_crossing_share); ped_crossing.cpp 1-168 of 168 (walk_ok by the stage that stops the arm, cars on
  the crossing, courtesy time, critical gap, vehicles giving way, answers cached per tick).
- Also read whole: ped_furniture.cpp 1-350 of 350, pedestrians.cpp 1-260 of 260, kerb_walkers.cpp 1-156 of 156, ped_doors.cpp 1-164 of
  164 (the doors of the walking window read a few a step, a foot clear of the furniture, drawn by who comes out at this hour),
  ped_door_walk.cpp 1-99 of 99 (into and out of a lot by its gate's bends), ped_state.h 1-146 of 146, ped_system.h 1-203 of 203.
- No separate walk graph: walkers route on the road graph itself and their ways round a node are planned when they reach it, so an
  edit rebuilds nothing for them (better than the reference's graph rebuilt locally).

## Differences
- The reference keeps walkers off highways and ramps (pedestrianAccess.ts:4-6, the CTB's via de trânsito rápido: "sem travessia de
  pedestres em nível", Anexo I, https://www.planalto.gov.br/ccivil_03/leis/l9503compilado.htm); this game routes walkers along and
  across the expressway, whose outer strips are kind sidewalk (road_profiles.json:310-358, profiles.cpp:80, ped_routes.cpp:134).
  Registered; the fix is this sheet's design.
- The reference's walkers look ahead for signal posts in 10 m cells (signalPostIndex.ts); this game's walkers keep clear of every piece
  of the sidewalk's furniture, signal posts among them (ped_furniture.cpp:68-103 the footprints, 197-241 a way and a foot kept clear),
  built per segment when a walker first meets it, dropped on an edit there and when the walkers are far (180-188, 258-267): the
  behaviour is here, wider and without a whole-map pass. Paired by behaviour.
- The reference's bridge to vehicles (engine.ts:68-79: hailable, board, alight, anyoneWithin) is here: kerb_walkers.cpp:20-23 (who may
  be called: alone, no child, doing nothing, not frightened, not called already: the same rule), 27-52 (the call), 147-153 (one getting
  out made on the footway, none with no room). Paired by behaviour.
- The reference's walker may be a resident on a trip with a person id kept for life and a companion (engine.ts:38-53, `walkTrip`);
  this game's walkers are born by the street's demand (pedestrians.cpp:219-257, spawn_walker) with no person behind them: the genre's
  citizens (genre-gaps.md, citizens' lifecycle) are what is missing, beyond this sheet: the people's lifecycle sheet of US7 (T029).
- Both: walkers live only in a window round the camera (pedestrians.cpp:230-241, beyond `ped_window_m` gone): a simulation tied to the
  camera, registered as Psimulation-54; its design is that problem's, not this sheet's.

- The reference's party has a size and an archetype (solo, family, couple, friends, colleagues, elders, tourists) and may hold a child
  (view.ts:8-16); this game's company is one companion (ped_state.h:141-142 `with`/`mate`; ped_system.h:113-114 add_company: one born
  with the leader's trip). Registered with this sheet.

## Design here
- **Walkers off the expressway** (T008, first item): a profile carries walkers by the catalog's data, a `pedestrians` field of each
  road profile (project/data/catalog/road_profiles*.json and its schema), false for the expressway; profiles.cpp reads it into
  Profile and `walkable` (ped_routes.cpp:134) asks it with the strips; the expressway's edge strips become verges, so no footway is
  drawn. Owner: native/src/sim/roads/profiles.cpp. Values: CTB Anexo I (no at-grade pedestrian crossing on a via de trânsito rápido).
- **Parties of more than two, with an archetype** (T008, second item): a party record (its members, its archetype, whether it holds a
  child) in place of the pair, the archetypes' shares by hour in traffic.json, a party crossing on one leave and going in at one door
  as the pair does now; the genre's and the reference's walking groups. Owner: native/src/sim/traffic/ped_party.cpp. Its design is
  written when ped_party.cpp and ped_spawn.cpp are read whole for that item.
- Not in this sheet's design: the camera window (Psimulation-54), the residents' trips (the citizens' lifecycle, T029).

## Proof
- Walkers off the expressway: a test scene with an expressway and a street: no walker's route, way or crossing on the expressway
  (traffic_probe), and the street's walkers as before; a photo plan of the expressway's edge at street level (no footway drawn).
- Parties: a scene counting parties by size and archetype over an hour of the clock against traffic.json's shares; photos of a family
  and a group crossing together (setViewNear walker).
