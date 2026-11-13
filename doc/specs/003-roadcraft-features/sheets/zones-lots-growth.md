# Zones, lots and growth: the area's index (specification 003, T013, T014)

- State: read (2026-11-13, a reading agent; each feature's claims checked in the files when it is built).
- Present: corner lots; the drawn lots' tools (rectangle, polygon, corner, curve, front, splits, join, delete, cut by roads, snap,
  a size tag); what grows (kind by use and density, four candidates against those within 100 m, plus a height cap and civic
  buildings); the zone overlay. Beyond the reference: residents and jobs from floor area.

## Features, in the order they are built

| # | Feature | State here | What differs | Evidence (here; reference) |
|---|---|---|---|---|
| 1 | Growth's order and pace | PARTIAL, high | the nearest free lot with no reach and no random jump, 25 buildings a second; the reference: within 70 m of the last building else a random open lot, refused lots waiting for a road change, one building per 0.5 s | city_growth.cpp:34-56,91, tuning.json:8-9; zoning.ts:88,226-253, main.ts:2656-2667 |
| 2 | The 24 km opening (research R4) | this game's own cost, high | fill_all serial, four complete bodies a lot to compare looks: 69.8 s debug, 23.6 s release (also blocks the fast scale probe) | growth.cpp:110-198; T014 |
| 3 | Rezoning keeps the building | PARTIAL, medium | any rezone knocks buildings down; the reference keeps them, removing the zone removes them | zone_commands.cpp:41-81, lot_commands.cpp:240; lots.ts:1254-1272 |
| 4 | Lots cut to the land and kept | PARTIAL, medium | rectangles only, no cut to the land nor fitting round other lots, worked out again on every street change and never saved (a road edit cuts lots under standing buildings) | lot_gen.h:32-36, lot_cache.cpp:82-100; lots.ts:691-910 |
| 5 | Buildings that age into decay | ABSENT (data present), medium | the decay days loaded and the build time saved, read by nothing but the save and the strike | growth.json:49-50, catalog_loader.cpp:187-188, city_growth.cpp:66; types.ts:453-469, decayWatch.ts:9-65 |
| 6 | A polygon lot's building fits its outline; growth tries set-backs and lower densities; eras by quarter | PARTIAL, medium | the biggest rectangle only; no set-back or density step-down; no era by quarter | lot_cache.cpp:26-35; zoning.ts:143-162,403-449,529-530 |
| 7 | Industrial densities; the generator's parks | PARTIAL, low | industrial one code; no park branch | zones.json:3-11, drawer_strips.gd:29-33, city_plan.cpp:37-55; plan.ts:137-154 |

## Translated from the reference (redone in this game's design when built)

lot_book.h/.cpp (clipLeft, withFront, lotBuildFrame), lot_land.h (onLand, cutLotsByRoads, lotSnapper), lot_commands.cpp,
block_lots.cpp (its constants); from an older reference: lot_gen.*, city_growth.h, growth.cpp, zone_commands.cpp, road_gen.h:14,
city_gen.cpp:1; city_plan.cpp:1, lots_tool.gd:1, zone_tool.gd:7.

## Faults never brought (the reference's and this game's own)

Up to 6 tries x 3 densities x 4 bodies for one lot (zoning.ts:82,159-162,425-427); the growth worker copying buildings and roads
(growthFeed.ts:22-58); the lot planner's whole-land raster and string keys (lots.ts:168-220,443-460); the zone grid's string keys
and whole-map walk (zoneGrid.ts:50-55,192-210). Here: each building scans every lot of every queued cell with a building query per
lot (city_growth.cpp:34-56, growth.cpp:77-92); free_of_lots walks every lot of the book (lot_commands.cpp:51-55).
