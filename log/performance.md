# Performance: where the 24 km city spends, fix by fix

The record of the performance line on the tree taken back to `a1a771c4` (2026-11-10). Every number is the 24 km city
(map `km24`, index 4, seed 1) through `tests/visual/look.tscn` at 1920x1080 with the tests' DLL, each view read only once
`render_pending` is 0. Views: centre (`setView [0, 0, 1.2, 30, 40]`) and 9 km out (`[-9000, 4000, 1.2, 210, 40]`).

## Base (commit a1a771c4, 2026-11-10)

| View | Video | Buffers | Godot static | GPU a frame | Core allocator committed |
|---|---|---|---|---|---|
| Centre | 5,252 to 5,454 MiB | 4,084 to 4,281 MiB | 1,309 to 1,351 MiB | 16.2 to 16.6 ms | 3,463 MiB |
| 9 km out | 3,773 to 4,009 MiB | 2,622 to 2,837 MiB | 868 to 898 MiB | 14.0 to 14.5 ms | 3,239 MiB |

The city opens in 20 to 25 s. The ranges are two runs with the views taken in a different order.

## P1 (2026-11-10): the core's allocator gives memory back

- Problem: the allocator (mimalloc v2.2.4, a static library in a DLL built by clang for MinGW) registers its load and
  thread-end hooks through MSVC pragmas this compiler ignores (`src/prim/windows/prim.c`). It never left its loading state:
  freed memory was only marked reusable (`MEM_RESET`), never given back, and the heap of a thread that ended was never freed.
- Test first: `tests/core/memory_test` (probe `memory_churn`). A thread takes 256 MiB in blocks of a MiB, frees all but one
  in sixteen and gives back. Before: 256 MiB still held for 16 MiB alive, and one heap left when the thread ended. After:
  17 MiB held, none left.
- Fix: `native/CMakeLists.txt` sets `MI_WIN_USE_FLS`, the allocator's own path outside MSVC (a constructor runs its start,
  a fiber-local callback each thread's end). `core/memory.h` is the allocator's interface for the core (`allocator_hold`,
  `release_freed_memory`); the probes `memory_churn` and `allocator_probe` read it.
- Measured, same plan, the build file's change set aside for the run before:

| Core allocator committed | Before | After |
|---|---|---|
| City just opened | 1,462 MiB | 1,484 MiB |
| Centre view drawn | 3,463 MiB | 2,118 MiB |
| 9 km out drawn | 3,239 MiB | 2,051 MiB |

- Draws, primitives, video memory, static memory and the opening time are the same in both runs; the city's digest is the
  base's (`4040792916cf7d78`). `job_pool_test` and `memory_test` pass.
- Left for the next problems: 2.0 GiB still held at the 9 km view after the centre was left (what the workers' heaps keep
  until they work again, and what lives), the video buffers (4.1 GiB at the centre) and Godot's static memory (1.3 GiB).
- Gates on the changed files: the findings left stand at the base commit and are not of this change (`JOB_SEM_CHAVE` on the
  pool probe's four submits, two `.uid` files without their script).

## P5 (2026-11-10): the memory the pool's workers hold follows the work, and what the frame frees goes back

- Problem: a keyed job went to any of the 23 workers (one condition, `notify_one`), so a thin stream of mesh jobs spread
  over every worker's heap; and what a job built is freed later by the frame's thread, after the upload, into the worker's
  heap, where the allocator gives it back only when that worker's own thread looks. A worker asleep never looked.
- Test first: `job_pool_probe` on a pool of its own (`tests/core/job_pool_test`): jobs one after the other, each building
  32 MiB its caller frees once the job ended. Before: 4 workers used, 382 MiB still held. After: 1 worker, 0 MiB, also for
  memory freed long after its job with the worker at rest.
- Fix (`core/job_pool.h`, `.cpp`): the idle workers wait in a stack, each on a condition of its own, and new work calls the
  one that worked last; a worker's thread starts the first time its place is called. A worker at rest whose heap still holds
  pages looks at it (`release_freed_memory`, `heap_held` of `core/memory.h`) a linger after its own work and again when the
  pool is told of a free (`JobPool::freed()`, called by `RenderWorld::sync` in a frame that handed built results to the
  engine), never more than once a linger (2 s); once its heap holds nothing it waits for work alone. No timer runs while
  nothing is freed.
- Two designs were tried on the way and dropped, each by what the test or the measurement showed: workers that end when idle
  (their abandoned segments are freed by nobody: 94 MiB left, with or without the allocator's reclaim-on-free option), and a
  blind timer whose wait grew (the old view's memory was looked at late: 2,735 MiB right after a view change).
- Measured, one plan, each view read 5 s after it was drawn, the pool's change set aside for the run before:

| Core allocator committed | Before | After |
|---|---|---|
| Centre | 2,057 MiB | 1,790 MiB |
| 9 km out | 1,996 MiB | 1,683 MiB |
| Centre again | 2,045 MiB | 1,744 MiB |

- The city opened in 16.4 to 16.6 s in three runs after, 19.7 to 19.9 s in three before. Draws, primitives, video and static
  memory are the same; the digest is the base's. `job_pool_test`, `memory_test`, `traffic_hash_test` and `e2e` pass.
- Gates on the changed files: what is left stands at the base (as above, and `NUMERO_MAGICO` on the indices of
  `render_world.cpp:291`).

## P9 (2026-11-10): the buildings' far level holds what its nearest distance shows

- Problem: the far level (one mesh a 1,024 m superchunk, drawn from 1.1 km to 4.5 km plus 3.5 times the eye's height) had
  no rule of its own for what it holds. It called the near level's emitters whole: a pitched roof with its undersides,
  0.18 m edge boards and ridge cap (`roof_pitched.cpp`), the roof plant as boxes, lids, doors and capped cylinders
  (`roof_plant.cpp`), the 0.3 m coping its own header said it dropped (`far_mass.cpp`); and a shell with window codes filled
  and counted four arrays a vertex where the far level sends one (`shell.cpp`). At the centre view: 31.9 million triangles.
- Test first: `tests/core/far_level_test` (probe `far_form`: the far level of the whole test town built on the spot).
  Before: 206,272 triangles for 1,368 buildings, 38,545 four-cornered faces narrower than the 0.32 m the level may be off
  where it begins (`render.detail_error_px` at `building_mid_end_m`), 2,853 faces seen only from below, 31.1 MB counted
  for 25.7 MB sent. After: 95,070 triangles, none of either, 11.6 MB counted and sent.
- Fix: for the far level (`Placed::lean`) a wall has no coping, a pitched roof is its planes and the walls under them, and
  each piece on a roof (machine room, plant) is one box in its main colour (`PlantEmit::mass`); the far shell writes a box
  only when its bar width is still drawn where the far level begins (`Shell::box_least` =
  `BuildingRenderer::far_part_least`, the part bands' own rule, one owner of the formula with the near bands' ends) and no
  four-cornered face under the error (`Shell::quad_least`); a shell fills, counts and sends `custom_used` arrays.
  `Shell::coarse`, `COARSE_MIN_H` and `CYL_SIDES_COARSE` are gone (the far level was their only user).
- A design tried and dropped by the measurement: the roof pieces as rows of one MultiMesh a superchunk. It took 1,190 MiB
  of video memory off the centre view, but the engine keeps three copies of every instance on the CPU (502,892 rows):
  Godot's static memory rose 49 MiB and the GPU frame 0.6 ms. The boxes stay in the far mesh.
- Measured, one plan (`info` and 5 s of `usage` at each view once `render_pending` is 0), the commit before against the change:

| 24 km city | 9 km out, before | after | Centre, before | after |
|---|---|---|---|---|
| Video memory | 3,773 MiB | 3,313 MiB | 5,452 MiB | 4,469 MiB |
| of it buffers | 2,622 MiB | 2,161 MiB | 4,281 MiB | 3,296 MiB |
| Godot static memory | 903 MiB | 861 MiB | 1,357 MiB | 1,315 MiB |
| GPU a frame (mean of 5 s) | 14.17 ms | 12.58 ms | 16.23 ms | 15.01 ms |
| CPU a frame (mean of 5 s) | 0.95 ms | 0.92 ms | 1.04 ms | 1.05 ms |
| Primitives a frame | 3.74 M | 3.05 M | 4.88 M | 4.19 M |
| Shadow primitives a frame | 12.55 M | 5.86 M | 15.01 M | 8.61 M |
| Far level held | 13.6 M triangles | 6.1 M | 31.9 M triangles | 15.7 M |

- The city opens in 16.6 s as before; the digest is the base's. `far_level_test`, `building_light_test` (both renderers),
  `quality_test` and `e2e` pass. Photos: `dist/trace/port-from-nov7/photos` (10- to 16-p9), analysed in `analysis.md`.
- `dist/quick/golden.txt` and `golden_photos` still held the tree from before the return to `a1a771c4`; they were written
  again from this tree (the digest equal to the base's recorded above).
- Left: the far level is still one form from 1.1 km to its end (most of its superchunks stand past 2 km); the ground
  outside the city and under far parks is dark (P10 of the trace); light and night to survey (P11). Gates on the changed
  files: what is left stands at the base, but `TAMANHO_ARQUIVO` on `building_renderer.cpp` (356 lines, 351 at the base)
  and `building_renderer.h` (264, 253 at the base), both over the limit before this change and to be split with P2.

## P12, P14, P16, P17, P18, P19 (2026-11-10): code first, one measure at the end

The owner's order: fix in code, then measure once. Each is in the trace `port-from-nov7` with its cause; the code of P17
(C13), P18 (C20) and P19 (C9) was copied back from the work before the return (tag `antes-da-volta-2026-11-10`) and fitted
to this tree.

- P12 (`0ffc4fde`): the mid level's openings are instances only within their own reach (`building_bay_end_m`), a level of
  their own; past it the walls' merged rectangles.
- P14 (`9419745b`): the noise on the walls at the level change was the dissolve of two different levels over each other,
  which TAA cannot resolve; the levels change at one distance, with no dissolve.
- P16 (`a8a9d00f`): the roads' far level was built for every superchunk of the map and held for good; it follows the view
  (the buildings' stream reach), dropped past it.
- P17 / C13 (`a8a9d00f`): the sky shader read POSITION, so the engine rendered and filtered the radiance cube again for a
  moved eye; the low band's share is a uniform.
- P18 / C20 (`19338fc6`): the final heights were a float array of the whole map; they stand by tile, made where something
  changed them.
- P19 / C9 (`5fda1c98` to `cfa6faf1`): every mesh producer built while its thread held the map shared. A job now copies a
  cut of its place under `MapRead` (`CityCut`, `cut_city`) and builds from it with the lock let go (`CityView`,
  `MeshQueueOf::submit_cut`): roads, verges, structures, far roads, water, street furniture, trees, ground cover.
  `tests/core/map_hold_test` (probe `map_hold`, counted by the tests' DLL) named, before: props 785,042, roads 262,589,
  far roads 101,125, trees 10,714, verges 8,560 vertices and instances built under the map; after: none.
  Not converted: the zone overlay's job (drawn only with the Zones layer on) and the transit plan built on the frame's
  thread.

Measured once, at `8ca7efe1`, the plan of P9 (`info` and 5 s of `usage` at each view once `render_pending` is 0):

| 24 km city | 9 km out, base | P9 | now | Centre, base | P9 | now |
|---|---|---|---|---|---|---|
| Video memory | 3,773 MiB | 3,313 MiB | 2,230 MiB | 5,454 MiB | 4,469 MiB | 3,606 MiB |
| of it buffers | 2,622 MiB | 2,161 MiB | 1,078 MiB | 4,281 MiB | 3,296 MiB | 2,436 MiB |
| Godot static memory | 898 MiB | 861 MiB | 877 MiB | 1,351 MiB | 1,315 MiB | 1,077 MiB |
| GPU a frame | 14.0 ms | 12.58 ms | 12.72 ms | 16.2 ms | 15.01 ms | 14.95 ms |
| CPU a frame | 0.95 ms | 0.92 ms | 0.98 ms | 1.04 ms | 1.05 ms | 1.27 ms |
| Primitives a frame | 3.74 M | 3.05 M | 2.83 M | 4.88 M | 4.19 M | 3.75 M |
| Shadow primitives a frame | 12.55 M | 5.86 M | 5.86 M | 15.01 M | 8.61 M | 8.61 M |

- Every number is under the base. Against the measure taken after P12 (854 and 1,056 MiB) the static memory is about 20 MiB
  higher at both views, and the CPU frame at the centre 0.2 ms higher: not yet read for its cause.
- Photos of this run: `dist/trace/port-from-nov7/photos/40-final_*.jpg`, read one by one in `analysis.md`. Nothing new is
  broken by these changes (roads, verges, street trees, parks' trees and furniture stand); the defects already recorded
  still show: the dark ground outside the city and under far parks (P10, P7), the night's flat far walls (P11).
- Tests: `map_hold_test`, the digest (equal to the golden) and the scenes each piece reaches passed as each piece went in;
  the whole suite ran at `cfa6faf1` (42 of 43; `camera_input_test` failed under six parallel processes and passes alone).
  The suite has not been run whole since the test harness and the pool's sizing changed: it runs once at the close.

## P25 to P31 (2026-11-10, night): the tag's performance and optimisation fixes, written or fitted for this tree in one batch

The owner's order: every performance and optimisation fix of the tag at once, nothing of the ports, tested, photographed
and measured once at the end. The tag's line could not be transplanted (it stands on the ports); each was written for this
tree or applied three-way and resolved to this tree's forms. Commits `0fbba89a` to the batch's close.

- P25 (`0fbba89a`): the people's clip palette is not kept on the CPU beside its texture; the three clips a fallen body
  is posed from are cut out at load.
- P26 (`1ec9d556`): every region of the land was made in Terrain3D when a map opened (576 regions of height, painted
  ground and colour for the 24 km land, on the CPU and again on the GPU), and a region added or removed made its texture
  arrays again whole. `TerrainBridge` owns the land's maps as layer stacks on the GPU alone: the fine ground for a window of
  regions round the eye (a region entering takes the layer of the one that left, nearest first, within the frame's upload
  room) and the far ground, the whole map every `render.far_land_cells` samples in tiles, read by
  `terrain_land.gdshaderinc` wherever no fine region stands. Terrain3D keeps no region. `probes/ground_drawn` reads a height
  back from the maps; `render_pending` counts the fine regions still to write.
- P27 (`d70c450b`, the tag's C7 with ec7b1925, 2d53b55e, 3e3c48e2): a record's body was made again by every reader and kept
  in four private caches by count. `b40::BodyCache` (the tables' own, `sim.body_cache_mb`) is the one owner; the render's
  rooms and cut bodies are kept in bytes, a rooms entry costing the body it pins (`render.rooms_cache_mb`); the far level
  derives no rooms; a lot's plants, gates and bays are a list of their own; one antialiasing filter a tier.
- P28 (`a9ebda4c`): the sky, the weather and the daylight send a shader value only when it moved (`SentParams`).
- P29 (`e9a2af66`): the signature looks stay packed (0.6 MiB), a picture unpacked when a building asks for it (33 MiB
  were kept unpacked).
- P30 (`2a27512c`): growth decides each building under the shared map lock and takes the exclusive one only to add it;
  the looks moved from an unbounded map in the store to the body cache.
- P31 (`4c1d932d`, the rest of the tag's C7): the profiles' derived values have an owner, a segment's street furniture is
  placed once and kept for the streets' revision (a building that grows no longer drops it), the transit reads its own
  list of the change log, the tools and panels ask the core once.

Measured once, at `4c1d932d`, the same plan (`info` and 5 s of `usage` at each view once `render_pending` is 0):

| 24 km city | 9 km out, base | before the batch | now | Centre, base | before the batch | now |
|---|---|---|---|---|---|---|
| Video memory | 3,773 MiB | 2,230 MiB | 1,770 MiB | 5,454 MiB | 3,606 MiB | 3,132 MiB |
| of it buffers | 2,622 MiB | 1,078 MiB | 1,078 MiB | 4,281 MiB | 2,436 MiB | 2,436 MiB |
| of it textures | | | 604 MiB | | | 608 MiB |
| Godot static memory | 898 MiB | 877 MiB | 382 MiB | 1,351 MiB | 1,077 MiB | 582 MiB |
| GPU a frame (mean of 5 s) | 14.0 ms | 12.72 ms | 11.75 ms | 16.2 ms | 14.95 ms | 13.67 ms |
| CPU a frame (mean of 5 s) | 0.95 ms | 0.98 ms | 1.07 ms | 1.04 ms | 1.27 ms | 1.03 ms |
| Primitives a frame | 3.74 M | 2.83 M | 2.88 M | 4.88 M | 3.75 M | 3.79 M |
| Shadow primitives a frame | 12.55 M | 5.86 M | 5.86 M | 15.01 M | 8.61 M | 8.61 M |

- The static memory fell by 495 MiB at both views and the video memory by 460 to 474 MiB: the land's regions, on the CPU
  and on the GPU, were most of it. The city opens in 23.6 s in this run (16.6 s before): not yet read for its cause.
- Tests: the whole suite, once, two Godot at most: 42 of 42. Photos of this run, the commit and the date written on each:
  `dist/trace/port-from-nov7/photos/60-batch_*.jpg`, read one by one in `analysis.md`: the land runs whole from the street
  to the horizon with no ring where the fine ground ends, the rooms stand behind the panes, the lamp pools on the avenue
  at night; the night's flat far walls (P11) still show.
- Not brought from the tag, with why (also in `log/pendencias.md`): 2f1d3570 (the render jobs' derived places kept by
  revision under the shared lock: this tree's cuts replace that design), 9944dde5 (a city opens aside: the opening, not the
  running game), 16cd2653 (pipelines made when a mesh is made: stands on the ports, to be written for this tree),
  908b3d3d and 96b2db93 (the map layers and the transit plan off the frame's thread: stand on the lab's flow levels and
  transit parts, to be written for this tree), 98a14171 (lock protocol fixes, no wait removed).

## P35, P33, P15 (2026-11-10, afternoon): no pipeline made on the frame a thing first appears

- Problem (P35): every mesh the render made was created empty and filled (`mesh_create`, `mesh_add_surface_from_arrays`),
  and most draws wore their material as an instance override. The engine precompiles a mesh's pipelines only when it is
  created whole by `mesh_create_from_surfaces` with its surfaces' materials (Godot `rendering_server_default.h`,
  `render_forward_clustered.cpp` `mesh_generate_pipelines`); everything else is compiled on the frame its instance first
  appears (the counter `PIPELINE_COMPILATIONS_SURFACE`) or while it is drawn (`..._DRAW`): a hitch each time a kind of
  thing first came into view.
- Three causes, each read in the engine's source, each with its fix:
  1. The meshes: `render/surface_data.h` writes a surface as the engine keeps it, byte for byte (`surface_data_test` lays
     it beside the engine's own), and every renderer makes its meshes whole with their materials (`MeshBuilder::surface`,
     `bake`, `create_mesh`, `mesh_of_arrays`). No drawn instance carries an override.
  2. The framebuffer the pipelines are made for: the engine reads the project's `msaa_3d` setting each frame, never the
     viewport's, so a tier with MSAA found no precompiled pipeline and made each while drawing. `QualityTier.apply` tells
     the setting what the viewport draws with.
  3. The shared unit meshes (vehicle models and their casters, tree species, the furniture kit, people, held things, cover
     variants, the buildings' bay, box, flag and balcony pieces, the train car, the signature designs): made at the start,
     before any frame told the engine the passes the tier draws (motion vectors, normal and roughness), and drawn by
     MultiMeshes made only when their first chunk or agent comes into view. The engine makes pipelines again, when a frame
     asks for more passes, only for surfaces some instance holds. `OwnedMesh::shared()` (`render/mesh_draw.h`) gives each
     an instance of its own in no scenario (never culled, never drawn), so its pipelines follow the frames, a tier change
     included.
- The one override left is the shadow twins' (`ShadowTwin`, `rs_util.h`: the facade parts', the balcony pieces' and the mid
  solids' shadows, in `shadow_caster`): taken out with the others at first, the twins cast with the parts' own shader, whose
  `vertex()` every shadow pass ran, and the centre's GPU frame rose by 0.5 ms (14.47 against 13.93 ms, the same plan). It is
  back, and `shared(cast_in)` holds a second instance wearing it, so that pair's pipelines are made off the frame as well.
- Test: `tests/core/pipeline_test` (Forward+: the counters exist where the renderer precompiles). The camera stands over
  the whole town, then tours a street from near, the blocks from mid distance, another district and the town from far.
  Before the fixes of causes 2 and 3: 31 pipelines made while drawing and 30 on the frame things first appeared. After:
  0 and 0; 22 more made with the meshes, off the frame. The 88 the first frame triggers are at load: the engine learns the
  tier's passes by drawing one.
- P33 (`6977eba4`): the railings' and brises' coverage is a grain only where a filter over the frames resolves it; blended
  where none runs. P15: the dissolve between the near and the mid level, unused since P14, is gone whole: its three twin
  shaders, `building_level.gdshaderinc`, the instance uniforms `level_keep` and `level_back` (the mid level stood 0.6 m
  back along the eye's ray for it; it stands in its true place), `lod.level_fade_m` and `level_back_m`. The glass's and the
  parts' bodies went back into their one shader each.
- Measured once at the close, the batch's plan, release DLL (`info` and 5 s of `usage` once `render_pending` is 0):

| 24 km city | 9 km out, base | the batch | now | Centre, base | the batch | now |
|---|---|---|---|---|---|---|
| Video memory | 3,773 MiB | 1,770 MiB | 1,800 MiB | 5,454 MiB | 3,132 MiB | 3,162 MiB |
| of it buffers | 2,622 MiB | 1,078 MiB | 1,080 MiB | 4,281 MiB | 2,436 MiB | 2,438 MiB |
| of it textures | | 604 MiB | 604 MiB | | 608 MiB | 608 MiB |
| Godot static memory | 898 MiB | 382 MiB | 391 MiB | 1,351 MiB | 582 MiB | 592 MiB |
| GPU a frame (mean of 5 s) | 14.0 ms | 11.75 ms | 12.25 ms | 16.2 ms | 13.67 ms | 13.93 ms |
| CPU a frame (mean of 5 s) | 0.95 ms | 1.07 ms | 0.94 ms | 1.04 ms | 1.03 ms | 1.01 ms |
| Primitives a frame | 3.74 M | 2.88 M | 2.88 M | 4.88 M | 3.79 M | 3.79 M |
| Shadow primitives a frame | 12.55 M | 5.86 M | 5.86 M | 15.01 M | 8.61 M | 8.61 M |

- Every number is under the base. Against the batch's run the video memory is 30 MiB higher at both views (outside the
  buffers and the textures: the pipelines now made ahead, 122 and 125 held) and the static memory 9 to 10 MiB. The GPU
  frame is 0.3 to 0.5 ms over the batch's run with the same primitives: two runs of the base differ by as much (16.2 to
  16.6, 14.0 to 14.5 ms), so this one run does not say whether anything costs more; not read for a cause.
- The city opened in 24.7 s in this run: still not read for its cause.
- Tests, one Godot at a time: `pipeline_test`, `surface_data_test`, `quality_test`, `far_level_test`, `people_test`,
  `transit_test`, `strike_test`, `shoot_test`, `model_test`, `e2e`, `map_hold_test`, `memory_test`, `interior_test`,
  `building_tool_test`, `street_works_test`, and on Forward+ `lamp_light_test`, `building_light_test`, `light_color_test`,
  `lab_test`. Photos: `dist/trace/port-from-nov7/photos` (64-p35 of the test town, 00 to 09 of the 24 km city), judged one
  by one in `analysis.md`.
- Left: the effects' layers (`effects/mm_layer.h`) wear their material as an override on instances made at the start,
  so theirs are made on the first frame; a ghost of a signature design overrides its material (`body_stage.cpp`); a fallen
  building's pieces, a burnt vehicle and the rubble mounds are still filled after creation (`ruin_fracture.cpp`,
  `agent_wreck.cpp`, `mounds.cpp`): each compiles on the frame it first appears, once a kind.
