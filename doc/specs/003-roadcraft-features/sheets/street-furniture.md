# Street furniture, poles and signs: the area's index (specification 003, T011, T012)

- State: read (2026-11-13, a reading agent; each feature's claims checked in the files when it is built).
- This game places furniture by its own rules from the Brazilian standards (street_furniture.cpp is not translated); its
  "roadcraft" style reuses the reference's numbers. Present: lamp pools with the reference's values plus real spotlights on the
  nearest lamps; bins, benches, hydrants (one per coverage square) with a walk-past check; utility poles and wires with sag and
  wind, the pole tool and run planner; traffic plates and street names; signal posts by the traffic code with lit lenses;
  furniture kept through road edits (better than the reference, which lays it again).

## Features, in the order they are built

| # | Feature | State here | What differs | Evidence (here; reference) |
|---|---|---|---|---|
| 1 | The basic set | PARTIAL, medium | a basic street gets the wired pole row (lamp_row ignores the level), hydrants and trees, and no bins; the reference: lamp columns both sides, the left 15 m behind, bins, no wires | street_furniture.cpp:440-449, street_furniture_posts.cpp:39-60; furnitureSets.ts:173 |
| 2 | Lamps' beams at night | ABSENT, medium | the cone of lit air under each lamp, fading 60 to 140 m, switched off in the data ("no weather yet": weather exists now) | streets.json:306, street_props.cpp:162; scenery.ts:72 |
| 3 | Street trees that grow, and down planted medians | PARTIAL, medium | no planting time on a street item, so no growth; no automatic median row | street_works.h:40-48, street_snap.cpp:81; furniturePlace.ts:17-123, furnitureSets.ts:153-164 |
| 4 | Detail levels by size on screen | PARTIAL, medium | simplified meshes per chunk by distance, wires at two levels, no picture level, no rule by pixels; the reference: full from 48 px, simplified from 16, a picture, nothing under 1.5 px, wires at four levels, plates not kept past 700 m | mesh_levels.h:15, prop_renderer.cpp:73,108-121; streetFurnitureLayer.ts:87-110, propLevels.ts:17-21 |
| 5 | Rows placed by hand along the curve | PARTIAL, medium on curves | a row laid along the chord between the clicks, a plain line as preview | street_commands.cpp:111-118, streetscape_tool.gd:32-34; streetscapeTool.ts:192-246 |
| 6 | Speed plates by the marking style, none on highways | PARTIAL, low | laid whatever the style; only rail excluded | street_signs.cpp:28,45; derivedSigns.ts:21-60 |
| 7 | Signal heads' sway and halos | PARTIAL, low | no sway in props.gdshader, no halos (with the signals area) | signals.ts:35,81-82,114 |

## Translated from the reference (redone in this game's design when built)

street_signs.cpp:1, street_commands.cpp:1, pole_runs.cpp:1, streetscape_tool.gd:1, pole_tool.gd:1 cite it; street_works.h:21,50,
street_props.h:86, street_furniture.h:62-73, lamp_pools.h:1, furniture_types.h:44,112 (plate names one to one), drawers.json:305-313,
street_furniture.json:9,174,190, road_options.gd:151 carry its names or numbers.

## Reference faults never brought

Every item of the map walked (streetFurniture.ts:310-315); string-keyed grids rebuilt per call (furnitureSets.ts:75-117); a
change product per item and worker round trips re-serialising the roads (streetFurnitureLayer.ts:159-220,325-330); a canvas per
plate copied to the GPU (signs.ts:62,230); pole lines from polygon unions, winding numbers and a raster (poleLines.ts:264-384); a
route search widening to 4096 m (poles.ts:106-110); on-screen size measured per piece every frame (poleStores.ts:82-168);
simplified models built at run time (streetFurnitureLayer.ts:66-74).
