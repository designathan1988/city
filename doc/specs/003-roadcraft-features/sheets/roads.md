# Roads: the area's index (specification 003, T015)

- State: read (2026-11-13). Seven reading agents compared the reference (`src/world`, `src/editor`, `src/ui/roads`, `src/render`,
  `src/sim/roads`) with this game by the tracer and the files, each claim with its file:line; the main session checked the ones
  each feature below rests on before building it.
- The inventory's 181 reference-only rows of the area are almost all systems, which it never pairs (Ptools-120). Most of them
  already stand here (classes, profiles, catalogue, parking, connectors, green wave, names, signs, structures, wear); the features
  below are what differs or is missing, by what the player meets.
- Translated code found here (tools/gates/lab_scan.py and the register): the grade solver (Proads-10, Proads-15), the control
  advisor and the flow statistics (Proads-11), the road economy (45% of its passages), the structures' render (56%).

## Features, in the order they are built

| # | Feature | What the player meets | Evidence |
|---|---|---|---|
| 1 | The heights of the roads, solved in this game's design | Roads over hills, ramps, decks and bores; the grade solver is the reference's translated, keeps four float lines a segment for good (about 390 MiB at 24 km) and decides by fixed pass counts; a height read at the curve's parameter instead of its length stands off the deck | Proads-8, Proads-10, Proads-15, Proads-33, Proads-35 |
| 2 | Laying a road: what it will be and cost | The draft shows no bridge, fill, cut or tunnel stretches and prices every road as ground (a bridge shown affordable is refused at the click); no tunnel for a deep cut; the grade-limited end is never told; a height edit is free; a start on a raised road does not take its height | agents A, B, E: road_draft.gd:38-40, city_core_commands.cpp:72-82, road_plan.cpp:197-213, road_bend.cpp:46-68 |
| 3 | The network's links and bans kept through edits | Removing or splitting a road leaves its lane links and movement bans behind; reused ids hand them to a new road; the connector command indexes a node unchecked; flow statistics and the control advisor never forget a node | agents D, E: road_network.cpp:102-120, road_edit.cpp:32-45, road_tabs.cpp:191, flow_stats_query.cpp |
| 4 | Markings | No turn arrows, bus legends, words or plate marks; unfiltered dashes shimmer far off; far lane lines lose their colour; no continuous line before a signal | agents D, F; Proads-6, Proads-28 |
| 5 | The profile editor | No class, carriageway, paving or centre line controls; widening one footway widens its twin (road_spec_ops.gd:86); past 256 custom roads Apply does nothing; Save ignores problems; a saved road cannot be deleted | agent C |
| 6 | Streets and their life | Roads to the map's edge bring no traffic; no street name or house numbers in the inspector; the basic furniture set differs | agent G |
| 7 | Lanes that obey the road | Bus lanes, solid lines and per-lane turns painted but never read by the traffic | agent D (traffic area, T010) |

## Reference faults never brought (from the agents' reading, each with its place)

- Whole-map work: the first structures build samples every road (render/structures.ts:73-77); the far level rebuilt whole when an
  edit cannot be described (networkFarRoads.ts:197-260); the economy re-prices the map when a journal entry has no ids
  (economy.ts:121-127).
- Per-frame work on unchanged inputs: the draft's label rebuilt every frame (main.ts:3402); the residency walk every 6 frames
  (renderer.ts:4022-4028); the network window's sets each frame (networkWindow.ts:260-287).
- Quadratic: the network's change history (networkDelta.ts:101-110); marks re-merged per shape (roadMarks.ts:164); street numbers
  (streetNames.ts:188-189).
- Allocation in inner loops: junction building's filters and string keys (junctionBuild.ts:116-186); a closure per mark
  (roadMarks.ts:460).
