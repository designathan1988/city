# Camera, the lab and what no other area covers: the index (specification 003, T025, T026)

- State: read (2026-11-13, a reading agent; each feature's claims checked in the files when it is built).
- Present here: orbit, pan, zoom, tilt by mouse, keys and touch with the reference's values (camera.json:36-54); the camera's
  shape by distance (camera_profile.gd:13-62); keeping out of buildings, its rules in the core (city_core_sight.cpp:128,195-224);
  compass and camera menu; the pose saved with the game; blast shake; the lab's player-facing tabs (switches, optimizations,
  checks) and a diagnosis; the road economy; weather, clouds, fog, gullies, ecology; dynamic resolution and the starting tier.

## Features, in the order they are built

| # | Feature | State here | What differs | Evidence (here; reference) |
|---|---|---|---|---|
| 1 | The camera kept over the map | ABSENT, medium | the point looked at is not held inside the map's edge: the player pans into nothing | city_camera.gd:173-177, camera_aim.gd:69-70, city_state.h:83; isoViewport.ts:116-118 |
| 2 | Saves in the folder Steam syncs | ABSENT, medium at ship | saves in user://saves | save_slots.gd:9; desktopSaves.ts:5 |
| 3 | Reset view frames the map; perspective/isometric remembered; the building tool's frame selection and top view | PARTIAL / ABSENT, low | reset keeps the point; the projection toggle not saved; no frame or plan view in the building tool | city_camera.gd:87-93, session.gd:26, building_tool.gd:113-115; main.ts:2353-2381, buildingsWiring.ts:748-757 |
| 4 | The camera's timings | PARTIAL, low | wheel 0.16 s against 0.085, turn 0.22 against 0.12, an orbit spring the reference lacks, the pull in 0.28 and out 0.9 against 0.12 and 0.6 | camera.json:32,46,55,68-70 |
| 5 | Problems logged on the player's disk | PARTIAL, low | the F9 monitor keeps them in memory (health.json keeps the last session's) | health_panel.gd:1-3, health_log.gd:126-133; fieldReport.ts:4-6 |
| 6 | Steam achievements | ABSENT, low | none | steamAchievements.ts:7-8 |
| 7 | A name per person | PARTIAL, low | body and mood, no name | people_asset.h:40; people/spec.ts:3-12 |
| 8 | The lab's developer tabs (code coverage, console and live edit, probes, benchmarks against the budgets, investigate) | justified: developer-only | the benchmarks against the budgets become this game's scale probe (architecture.md, backbone item 8) | lab/tests/benchmarks.ts:9 |

## Reference faults never brought

Each placement of the view samples the line of sight 48 times through closures (isoViewport.ts:155-165), runs searches of 16 to
20 halvings (isoViewport.ts:195-227), and a held zoom re-runs the whole placement up to 8 times (isoViewport.ts:296-304); one JS
heap per worker (jobs/jobPool.ts:6). F7 bound in every build (editor/gameKeys.ts:110).
