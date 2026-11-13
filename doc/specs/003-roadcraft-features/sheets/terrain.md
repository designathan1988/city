# Terrain and nature: the area's index (specification 003, T020)

- State: read (2026-11-13, a reading agent; each feature's claims checked in the files when it is built).
- Present: flatten toward the stroke's start; a smooth brush (beyond the reference); rock paint; woods by the same density and
  lattice; the tree brush; elements, fog, clouds, gullies; ground graded under lots; weather on the ground with wet shine.

## Features, in the order they are built

| # | Feature | State here | What differs | Evidence (here; reference) |
|---|---|---|---|---|
| 1 | The relief brush's feel | PARTIAL, high | height grows with the time held (a still pointer builds a spire), dabs only where the pointer is, no crests, an 8 m hard wall; the reference: a stroke moves the ground at most its strength (dabs combine by the largest), dabs every 0.2 x radius along the drag, ridged crests, a 20 m wall | terrain_brush.cpp:269, terrain_tool.gd:52-63, tuning.json:69; world/terrain.ts:50,256-283, terrainBrush.ts:13,204-243 |
| 2 | Rivers that follow the land | PARTIAL, high | the bed one level for the whole stroke, water only inside each dab; the reference: the bed cut under the ground, the water at the lowest bank probe, a ribbon running downhill never under 0.5 m deep with its flow, hollows flooded beside it | terrain_brush.cpp:271-276,317-332, land_commands.cpp:40-55; world/terrain.ts:249-277, render/terrain.ts:1979-2014,2662-2930 |
| 3 | The ground's colour from the ecosystem | mostly ABSENT, high | one tint per biome plus slope dirt and rock; the reference: dry grass straw, wet ground darker, bare soil where herbs are sparse, shade under scrub and forest, high ground bare, painted flowers in the ground | terrain.gdshader:119-128; render/terrain.ts:838-961, plateTexels.ts:57-66 |
| 4 | Natural relief at map start | PARTIAL, medium | an older generator (canal, lakes, edge ranges) or a plain; the reference's ridged, domain-warped "natural" relief absent | terrain_gen.cpp:14-78; world/terrain.ts:87-142 |
| 5 | The ecosystem reads all the land | PARTIAL, medium | painted sandstone and basalt never read; one painted biome a cell; altitude from the sea; other slope bands | ecology.cpp:140,243-251, ground_paint.h:22-23, nature.json; ecology.ts:230,390-406 |
| 6 | Water flows | PARTIAL, medium | ripples follow the wind, not the river's flow | water.gdshader:71; water.ts:42-56 |
| 7 | Landforms with their outline | PARTIAL, medium | fixed shapes, no noisy outline | terrain_brush.cpp:226-230,282-289; main.ts:406-411 |
| 8 | Ground paint blends | PARTIAL, low | one surface a sample, a new one fades the old below 24; rocks lay no soil | ground_paint.cpp:25,131,140-147; render/terrain.ts:356-385 |
| 9 | The ground beside a road at the footway's top | PARTIAL, low | -0.1 m under the surface; a walled cut's shoulder 6.4 m against 10 | road_shape.cpp:46,85-86; elevation.ts:224,879-887 |

## Translated from the reference (redone in this game's design when built)

ecology.cpp/.h (same formations, mixes and distance pass); road_shape.cpp/.h (cites elevation.ts, constant names one to one);
tree_place.cpp, tree_look.cpp ("Roadcraft's numbers"); planted_trees.cpp (trees.ts one to one); terrain_brush.cpp, heightfield.h
and terrain_gen.cpp/.h cite reference files that no longer exist; brush_tool.gd, terrain_tool.gd, garden_place.h, tree_kit.cpp.

## Reference faults never brought

Every height read walks its cell's stamps, the stamp index copied per dab (world/terrain.ts:173-236); the brush throttled by the
last rebuild's time (main.ts:616); paint as up to 60,000 dabs re-rastered into 8 channels (terrainPaint.ts:37); water on a 1 m grid
in JS maps with string keys, a flood search of up to 102,400 cells a body (render/terrain.ts:97,645-660,2662-2723); the ecology
looping over object keys per corner and a full recompute when the lowest ground changes (ecology.ts:345-348,409-411); the far woods
reading the relief four times a cell (farWoods.ts:136-140).
