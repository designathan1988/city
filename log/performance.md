# Performance: the full reading of the code, by its flow (started 2026-11-10)

The owner's order: read the whole code in chunks, follow the flow, record every finding as it is found, name no cause before the
reading is done. This file is that record. A finding says where (file:line), what the code does there, what it costs and how that is
known: `read` (the lines do this), `arithmetic` (sizes multiplied out from the code and the data), `measured` (a number already in a
log of this repository or of the earlier investigation; no new run is made for this reading). Fixes are listed apart, at the end.

## 0. What was already measured (the investigation of 2026-11-09 by commit; logs kept in its scratchpad, read on 2026-11-10)

The same scene (town, seed 7, 1600x900, the tests' tier), each commit's own DLL. Process: working set at the end / private bytes.
Godot: its allocator (`static`) and the video memory by kind, read in the street view.

| Commit | Working set | Private | Godot static | Textures | Buffers | Buildings |
|---|---|---|---|---|---|---|
| bb58790e 2026-11-03, before the port (10 km town) | 1330 | 2265 | 311 | 191 | 646 | 3154 |
| 2595d766 05/11 13:40 | 1779 | 2739 | 382 | 241 | 309 | 1408 |
| fd6931fc 05/11 22:15 | 1645 | 2398 | 515 | 245 | 217 | 1408 |
| 0f4b04af 06/11 00:50 | 2094 | 3023 | 522 | 245 | 231 | 1408 |
| 5e6bd2c3 06/11 00:55 | 2002 | 3130 | 454 | 268 | 231 | 1408 |
| 036d91b8 06/11 16:15 | 2245 | 3353 | 464 | 284 | 244 | 1366 |
| c3dd7b97 06/11 16:35 (land 10 -> 20 km) | 2556 | 3847 | 717 | 544 | 248 | 1366 |
| 1af7f445 07/11 | 2894 | 4193 | 731 | 544 | 248 | 1366 |
| 388df70b 09/11 (head then) | 2974 | 4270 | 811 | 592 | 203 | 1017 |

Read step by step (working set / Godot static / textures):
- before the port -> 05/11 13:40: +449 / +71 / +50. Some 330 MiB outside Godot's allocator and the video memory.
- -> 05/11 22:15: -134 / +133 / +4.
- -> 0f4b04af: +449 / +7 / 0. All of it outside Godot's allocator and the video memory.
- -> 5e6bd2c3: -92 / -68 / +23.
- -> 036d91b8: +243 / +10 / +16. Some 200 MiB outside.
- -> c3dd7b97: +311 / +253 / +260. Inside: the land (Terrain3D's regions).
- -> 07/11: +338 / +14 / 0. Some 320 MiB outside.
- -> head: +80 / +80 / +48.

So of +1644 MiB of working set, the land's step is 311, and some 1300 MiB grew outside Godot's allocator and outside the video
memory: in the core's own allocations (C++), in four steps. The live game (the owner's Task Manager: 4.5 GB; log/owner-review.md §8:
4.42 GB, Godot static 750, the core's allocator 894 committed, 1.64 GB of Windows heaps neither Godot's nor the core's, video 1202)
stands some 1.5 GB over this scene: its tier (auto = ultra on this card), its window and its minutes of play.

The memory_probe of 2026-11-09 (before the land's change): the core's allocator held 224 MiB committed for a bare 10 km land, 454 for a
bare 20 km land, 897 for the town; mimalloc's own statistics for the town: 22 segments, 23 threads, reserved 1.1 GiB.

## 1. Findings

### P-01 The core's allocator commits 32 MiB a thread heap segment, on 23 threads
- Where: `native/src/core/memory.cpp` (new and delete on mimalloc, from `0f4b04af`), `native/CMakeLists.txt` (mimalloc v2.2.4),
  `native/src/core/job_pool.cpp:19-20` (one worker a core less one).
- Read: mimalloc `include/mimalloc/types.h:178,192` `MI_SEGMENT_SIZE` = 32 MiB on 64 bits; `src/options.c:118-139`: `eager_commit`
  on by default ("commit per segment directly"), `eager_commit_delay` 1 (only a thread's first segment is committed page by page),
  `purge_delay` 10 ms, `abandoned_page_purge` 0. Its purge runs on the heap's own activity (its docs): a worker asleep on the pool's
  condition purges nothing, and its blocks are freed by the frame's thread after the upload.
- Arithmetic against the measure: 22 segments x 32 MiB = 704 MiB, plus the land's arrays as huge blocks (175 MiB at 20 km) = 879;
  measured 897 committed. The bare lands: 44 + 6 segments = 236 (measured 224); 175 + 9 segments = 463 (measured 454).
- Measured: the step of `0f4b04af` is +449 MiB of working set with +7 of Godot static and no video: the allocator's.
- Costs: memory (some 450 MiB of working set, 625 of private bytes at the measured scene), growing with the cores of the machine,
  not with the city.
- State: `JobPool::loop` now collects a worker's heap once it is idle for the purge delay (2d53b55e, compiled, not yet read in a
  run). Whether a forced collect returns the pages of a segment that still holds a few live blocks is to read in mimalloc's
  `segment.c` before this is called fixed.

## 2. The state of the tree while this reading goes on
- Uncommitted, compiled: `FinalHeights` (sim/terrain/heightfield.h): the land's final heights kept by tile, a tile made at the first
  write; its test is the digest (the `heights` hash must not move). To run when the reading allows a test.

## 3. Reading log (what was read whole, in order)
- 2026-11-10, chunk 1 (whole): `api/city_core.cpp`, `render/render_world.{h,cpp}`, `chunk_queue.h`, `chunk_jobs.h`, `frame_admission.h`,
  `mesh_queue.h`, `chunk_results.h`, `rs_util.h`, `instance_rows.h`, `async_slot.h`, `mesh_draw.h`.

### P-02 Every mesh job holds the map's shared lock for its whole build
- Where: `render/mesh_queue.h:73-77` (`submit_job`: `std::shared_lock map(s.map_mutex); build(...)`), the queue of roads, structures,
  far roads, ground seams and props. Read.
- Costs: an exclusive request (a player's edit, growth's insertion, a save's mark) waits for the longest build; libc++'s
  `shared_mutex` then holds new shared requests behind the waiting writer, the frame's `RenderWorld::sync` among them
  (`render_world.cpp:246`, whose wait is the `lock` lap of the render_cost probe). A stall on edits and while the city grows.

### P-03 A MultiMesh instance is kept three more times on the CPU by the engine
- Where: `render/instance_rows.h:16-19` (the code's own note, citing Godot `mesh_storage.cpp multimesh_allocate_data`), every
  `MultiMeshDraw` (`rs_util.h:85-167`) and `bake_multimesh` (`mesh_draw.h:46-57`). Read (the note; Godot's source not reopened here).
- Arithmetic: 16 or 20 floats an instance, so 64 or 80 bytes on the GPU and three times that in the engine's memory: some 256 to
  320 bytes an instance. Every family that draws by MultiMesh pays it by its count: to total as each family is read (the buildings'
  part bands, bays and props; trees, woods and cover; street furniture; agents).

### P-04 The frame forces the RenderingServer's queue once a frame
- Where: `render/frame_admission.h:94-98` (`flush_uploads`: `RenderingServer::force_sync()`), called by `render_world.cpp:288`. Read.
- Costs: with a render thread the main thread waits there for the render thread to run everything queued; what it buys is that the
  uploads of the admitted bakes land at a known point. To judge with the project's thread model (project.godot), read below.

### P-05 The far level of every road of the map is asked at a new map
- Where: `render/render_world.cpp:98-111` (`full_rebuild`: `far_roads_.mark(k)` for every live segment and node; `overlays_.mark_dirty`
  for every segment). Read. Size and whether it is dropped: to read in `far_road_renderer.cpp`.
- chunk 2 (whole): `render/mesh_builder.{h,cpp}`, `surface_data.{h,cpp}`, `mesh_levels.{h,cpp}`, `mesh_order.h`, `detail.{h,cpp}`,
  `view_cull.h`, `kept_by_revision.h`, `light_pool.{h,cpp}`, `owned_segments.h`, `render_context.h`; `render/agents/agent_renderer.{h,cpp}`,
  `people_renderer.cpp`, `people_casting.cpp`, `fleet_shadow.cpp`, `people_plays.cpp`, `agent_lod.cpp`; `body_renderer.cpp` 112-211.

### P-06 A vehicle past its cabin is 666 to 1,380 triangles at any distance out to 900 m
- Where: `render/agents/agent_renderer.cpp:174` (`plain_level`: cabin, mid, body, far by fixed radii from agents.json; the far level is
  the last), `:299-315` (`draw_rows`). Read. Measured (dist/analysis/round4, the metropolis): far level 666 to 706 triangles a car,
  some 1,900 vehicles drawn, 1.1 to 1.3 million triangles a frame for the fleet, most of it cars a few pixels long.
- No level by the error on the screen (`render/detail.h` is "the one owner of level of detail" and the fleet does not ask it).
- Costs: GPU vertex work every frame, and again in each shadow pass for the levels that cast.

### P-07 The fleet and the people are drawn by MultiMeshes whose box is the whole published disc
- Where: `agent_renderer.cpp:302` (one box of 2 x `vehicle_r_` = 1,800 m for every vehicle draw and its shadow twin),
  `people_renderer.cpp:199-204` (one box of 2 x 280 m for the 48 draws of the people, the two mesh levels casting). Read.
- Costs: the engine culls a MultiMesh whole, so each of the sun's four cascades draws every casting instance of the disc; the
  vehicles' far level is spared by `FleetShadow` (sectors of their own, a hull), the people and the near vehicle levels are not.
- Per frame on the main thread: 40 vehicle draws, 30 shadow twins and 48 people draws are each set again (`MultiMeshDraw::set`:
  the buffer, two boxes, the count, the visibility: five RenderingServer calls a draw that exists, whatever it holds).

### P-08 The people's plays texture is rebuilt and sent whole each frame
- Where: `render/agents/people_plays.cpp:35-45` (`Image::create_from_data` of every row, `texture_->update`). Read. Small (rows x
  PEOPLE_PLAYS x 16 bytes), but the image shares the array, so the next frame's first write copies it: an allocation a frame.
- chunk 3 (whole, every file from its first line to its last): the rest of `render/agents` (all 47 files of the folder are read now:
  people_asset, people_boot, people_gait, people_gait_clips, body_renderer, ragdoll, body_walls, held_things, helmets, head_lights,
  fleet_shadow.h, agent_meshes.h, agent_looks, agent_parts, agent_cabin, agent_wreck, agent_renderer_parked, the shapes headers,
  car_shape, car_parts, car_body, heavy_meshes, bus_body, bicycle_mesh), `render/row_keeper.h`; `render/buildings/building_renderer.h`,
  `building_renderer.cpp`, `_build.cpp`, `_commit.cpp`, `_reach.cpp`, `_probe.cpp`, `_wear.cpp`, `building_bodies.{h,cpp}`,
  `building_job_inputs.h`.

### P-09 The people's clip palette is held twice: 55 MiB on the GPU and the same 55 MiB in the engine's memory, for ever
- Where: `render/agents/people_asset.cpp:286-303` (`texels` read from people.bin, kept as `palette_bytes`, and made the `palette`
  texture), `people_asset.h:76` (`palette_bytes; // the palette as the file has it`). Read.
- Arithmetic from the file's own header (project/assets/people/people.bin, 61.1 MiB): 16 bodies, 53 bones, 56 clips, 2,850 rows a
  body, 8 columns: the palette is 2,544 x 5,700 RGBA8 texels = 55.3 MiB. The meshes of the sixteen bodies are 4.4 MiB of it.
- `palette_bytes` has one reader: `PeopleAsset::clip_rows` (`people_asset.cpp:225-234`), called by `BodyRenderer::add` (the idle
  clip's frame 0) and `BodyRenderer::rise` (the crouch's frame 0 and the frames of crouchUp): three clips of the 56, for a body
  struck down. The other 53 clips' rows are never read on the CPU.
- Costs: 55 MiB of Godot static memory and 55 MiB of texture memory from boot, whatever the city. The file is also read whole
  into one array first (`get_file_as_bytes`, 61 MiB more while loading).
- The texture itself: every body has its own block of every clip (16 x 2,850 rows) at two RGBA8 texels a value. Whether the lab
  keeps a block a body is to read in the lab's citizen palette pass (the port's commit ranges, below).

### P-10 The cooked bodies' vertex arrays stay on the CPU after their meshes are made
- Where: `people_asset.h:54` (`PersonVariation::lods`: each level's surface arrays), read by `people_boot.cpp:209-211` (a mesh a
  body and level) and again by `body_renderer.cpp:60-65` (the near level made a second mesh with the fallen bodies' material, at
  the first body struck down of a city). Read. Arithmetic: 82,959 vertices at 88 bytes and 314,805 indices at 4: 8.2 MiB kept.
- The near level of the sixteen bodies is then on the GPU twice (people and people_body materials).

### P-11 A chunk of buildings is over a hundred instances in the scenario, each with a visibility range
- Where: `render/buildings/building_renderer.h:97-123` (`Chunk`): by sub-chunk (SUB_COUNT) a near, a parts, a mid and a flat
  MeshDraw, BAND_DRAWS (5 + BAY_PROPS) MultiMeshDraws, BAND_CASTS shadow twins, three mid MultiMeshDraws, a mid cast, a mid hull;
  and the flags. `building_renderer_reach.cpp:67-105` (`ranges`) gives each its range. Read; the counts to total once
  `core/grid.h` and `bay_props.h` are read (below).
- Costs: the engine walks every instance that has a visibility range each frame, for the camera and for each shadow pass (Godot
  `renderer_scene_cull.cpp` `_visibility_cull` over `scenario->instance_visibility`: to confirm in the engine's source when the
  fix is planned), and each instance is a node of the scenario's tree. CPU on the render side, growing with the chunks in reach.

### P-12 The rooms kept behind the panes hold a whole body each, outside the bodies' byte budget
- Where: `render/buildings/building_bodies.cpp:16` (`ROOMS_KEPT = 8192 ... each counted as one`), `:44-48` (`PaneRooms(data, whole,
  purpose)`: "it holds the body it reads"). Read. A town's every building's body is therefore held by its rooms entry whatever
  the body cache's budget says. Size of a body and of its rooms: to read in `pane_rooms.cpp` and `sim/buildings/b40` (below).
- State: `2d53b55e` stopped the far level deriving rooms; the near and mid levels still fill up to 8,192 entries.

### P-13 A far mesh is one mesh of every building of its superchunk, made whole again at any change in it
- Where: `building_renderer.cpp:104-114` (`rebuild_chunk`: the superchunk dirty), `:205-248` (`submit_super`: every record of
  CHUNKS_PER_SUPER x CHUNKS_PER_SUPER chunks gathered and built), `:318-325` (again every `far_rebuild_interval_s` while dirty),
  `building_renderer_wear.cpp:19-23` (dirty again at each wear step of any building in it). Read.
- Costs: while the city grows, every new building re-makes the far mesh of its whole superchunk (CPU on the workers, an upload of
  the whole mesh, the old one freed). Size of one: `super_bytes_max` (probe); to total with `far_mass.cpp` (below).
- chunk 4 (whole): the rest of `render/buildings` (all 78 files of the folder are read now): building_surface.h, pane_rooms,
  shell, far_mass, cast_hull, occluders, level_fade, facade.h, placed, facade_bay, facade_emit, facade_parts, facade_props, bay_kit,
  bay_props, body_stage, building_look, signature_bodies, lot_emit, lot_props, lot_parts, part_models, roof_emit, roof_plant,
  roof_pitched, roof_outline, roof_gear, roof_terrace, roof_clip, roof_util.h, roof_sizes.h, interior_emit, interior_cut,
  interior_lamps, furniture_emit, furniture_look, ruin_fracture, ruin_cells.h, ruin_body, flag_cloth, wear, the five look_shapes
  headers.

### P-11 (totalled) the instances of one chunk of buildings
- `bay_props.h:19-21`: BAY_PROPS = 10, BAY_PROP_SHADOWED = 4, so BAND_DRAWS = 15 and BAND_CASTS = 8 (`building_renderer.h:86-90`).
  A sub-chunk in the near level: near, parts, mid, flat, 15 bands, 8 shadow twins, 3 mid draws, the mid cast, the mid hull = 32
  instances; in the mid level alone 7. By SUB_COUNT sub-chunks a chunk (core/grid.h, to read), plus the flags and the occluder.

### P-14 The designs of signature buildings stay on the CPU whole, and their pictures go to the GPU uncompressed
- Where: `render/buildings/signature_bodies.cpp:69-92` (`read_table`: materials.sig unpacked into `table_`, kept), `:95-103`
  (`picture`: a look's picture cut out of the table as an RGBA8 image, its mipmaps generated), `:105-128` (`look`: an ImageTexture
  of it), `:202-222` and `signature_bodies.h:89-92` (`made_`: each kind's prepared arrays, levels and pictures "kept for another
  material (mesh_with)", the Build tool's ghost). Read.
- Arithmetic from the files' own headers (project/assets/lots, 39 files, 5.6 MiB packed): materials.sig unpacks to 31.6 MiB and
  the 38 bodies to 24.8 MiB. Held from the first signature building seen: the table (31.6 MiB); for each look used its picture
  with mipmaps on the CPU (in `Prepared::pictures`) and again on the GPU as RGBA8 (4/3 of its size each: up to 42 MiB and 42 MiB
  with every look used); for each kind seen its arrays (a file's 24 bytes a vertex are 32 in the arrays).
- Costs: Godot static memory and texture memory, tens of MiB each, for the whole run, for a use (the ghost in another material)
  that needs the arrays only while the Build tool holds that kind.

### P-15 Every job's geometry passes through three whole copies before the GPU has it
- Where: `render/buildings/shell.h:109-118` (a Shell: five std::vectors, 56 bytes a vertex, and four more arrays of 4 bytes in a
  shell with rooms: 72), `shell.cpp:292-329` (`surface`: each vector copied into a Packed array), `render/surface_data.cpp`
  (the core's own writer of the engine's surface bytes, `render/surface_streams.cpp`: the vertex, attribute and index streams made
  from those arrays), then the upload. Read. A quad is four vertices of its own (`Shell::face`): no vertex is shared between faces.
- Costs: on the worker that builds and the one that bakes, three times the level's bytes are alive at once (the Shell in the
  core's allocator, the Packed arrays and the surface bytes in the engine's), all freed after the upload: the allocation pattern
  that keeps a segment committed on every worker (P-01), and the bake's time.
- The same holds for `MeshBuilder` (`render/mesh_builder.h`: Packed arrays grown by `push_back`, then `surface_data`): two copies.

### P-16 The far level works out a cell's cover again each time a rectangle asks
- Where: `render/buildings/far_mass.cpp:42-61` (`rectangles`: `start`, then `join` for every cell of the row and of each row
  below), `:64-73` (`OpenCell`: `covered()` walks every volume of the body), `:143-154` (`lean_walls`: `open` in `start` and again
  in `join`, `alike` with two `wall_lin` and two `side_comp` lookups in hash maps a cell). Read.
- Costs: CPU on the workers for every building of a far mesh (P-13), several times a cell. Small a building; the far mesh of a
  superchunk repeats it for all of them at each rebuild.

### P-17 The occluders: one a chunk with a near level, the engine's occlusion buffer every frame
- Where: `render/buildings/occluders.{h,cpp}` (an occluder a chunk, set at each near commit, freed with the near level; the
  header's own note: the engine rebuilds its scene in the background when one comes or goes). Read. Whether occlusion culling is
  on, and at what buffer size, is the project's setting: to read in project.godot and the settings code (GDScript chunk).
- chunk 5 (whole): `render/surface_streams.{h,cpp}`, `terrain_bridge.{h,cpp}`, `data_reader.{h,cpp}`, `film_grade.{h,cpp}`,
  `render_layers.h`, `region_table.h`, `shape_reader.h`, `owned_segments.h`, `mesh_queue.h` (again), `core/grid.h`; all 28 files of
  `render/roads`: road_chunks.h, road_renderer, road_layer, far_road_renderer, ground_seam, ground_seam_layer, road_mesh.h,
  road_emit, road_segment, road_section, road_junction, road_markings, road_kerb, road_walls, road_stays, road_structures,
  road_shapes, road_look, street_shapes.h, street_shapes_parts.h.

### P-11 (closed) `core/grid.h:21`: SUBCHUNKS_PER_CHUNK = 2, so SUB_COUNT = 4
- A chunk of buildings in the near level is up to 4 x 32 + 2 = 130 instances of the scenario; in the mid level alone 4 x 7 = 28.
  `core/grid.h:19-20`: a superchunk is 4 x 4 chunks (1,024 m): a far mesh of buildings and one of roads each.

### P-18 The ground seam of a chunk draws the neighbourhood's roads again, unites every triangle of them and clips the whole union once a cell
- Where: `render/roads/ground_seam.cpp:109-148` (`read_paving`: `emit_segment` and `emit_node` of every segment near the chunk
  into a scratch builder, the same work the road job of that chunk and of its neighbours does; every up-facing triangle a path;
  `Clipper2Lib::Union` of all of them; a `height_at` a triangle), `:176-233` (`cell_seam`, once a cut cell: `RectClip` of the whole
  union, `Difference`, `Triangulate`, and `height_of` walking every ring for every corner), `:244-266` (`build_chunk`: every cell
  the outline crosses, of a 64 x 64 cell chunk), `render/mesh_queue.h:73-77` (all of it with the map held shared: P-02). Read.
- Costs: the heaviest of the road jobs (a union of thousands of triangles, then a clip of that union per cut cell: work that
  grows with cells x the union's points), repeated for a chunk whenever a road or the land near it changes, and for as long as it
  runs no command, growth step or save can take the map (P-02), nor, behind that waiting writer, the frame.
- The same road geometry is made by four jobs: the road chunk, the far road superchunk, the seam (scratch, thrown away) and the
  probe. One owner of a chunk's paving outline is missing.

### P-19 The fine ground's regions are sent whole again when the eye crosses into a region
- Where: `render/terrain_bridge.cpp:128-168` (`follow`: at any region added or removed, `update_maps(MAP_TYPE_ALL, true, false)`:
  Terrain3D makes its three texture arrays again from every region that stands). Read; written in `6dd9cbdf` (this session).
- Arithmetic: a region is 256 x 256 samples of height, control and colour, 4 bytes each: 0.75 MiB; with the regions within
  `render.road_end_m` of the eye standing (a dozen to a few dozen), each crossing re-sends 10 to 30 MiB in one frame.
- Costs: a hitch each time the camera travels a region's side (1,024 m at 4 m a sample), on the main thread.
- `push_rects` (`:170-195`): an edit of the land sends the whole far heights map again (`send_far`), 1.6 MiB at 20 km.

### P-20 A dashed line is a quad a dash, each with its own stations
- Where: `render/roads/road_markings.cpp:31-43` (`rect`: `segment_stations` for each rectangle), `:58-67` (a dash a rectangle),
  `:86-100` (a zebra's bars a rectangle each). Read. Vertices: four a rectangle and a station, unshared (56 bytes each in the
  builder). The far level leaves the lane lines to the shader; the near level carries every dash of every lane line of the chunks
  in reach as geometry.

## 4. The causes (trace `performance-root-causes`, read gate 2026-11-09 23:26: 1182 files read whole; plan gate 23:32)
The full text of every problem (P1 to P65) and cause is the ledger's report, `dist/trace/performance-root-causes/report.md`
(`python .claude/skills/trace-fix/ledger.py report`). The causes, in fixing order:

| Cause | The design decision | Owner | Problems |
|---|---|---|---|
| C1 | The fine ground follows the view through Terrain3D layer arrays remade whole; final heights by flat index | `render/terrain_bridge.cpp` | P11, P49 |
| C2 | A test's one unit is the whole game on a whole map, in a process sized for the whole machine | `tools/tasks/suites.json` | P64, P65 |
| C3 | The allocator gives every thread a heap of 32 MiB segments; workers' blocks are freed by the frame thread; an idle worker collects | `core/memory.cpp` | P41, P47 |
| C4 | Assets are kept in their file's form and derived again at every boot | `tools/assets` | P1, P2, P6, P30 |
| C5 | The whole generated Body is a building's only derived form | `sim/buildings/b40/body_cache.h` | P4, P28, P43, P45, P50 |
| C6 | Kept stores are budgeted by entry counts or not at all | `core/byte_budget.h` | P39, P46 |
| C7 | Every band, shadow twin and level is an instance or MultiMesh of its own | `render/rs_util.h` | P3, P9, P13, P19, P29 |
| C8 | Writers derive under the exclusive map lock | `sim/commands/road_edit.cpp` | P42, P51, P52 |
| C9 | Jobs read the live state under the shared lock for their whole build | `render/mesh_queue.h` | P10, P15, P33, P34, P35 |
| C10 | Scripts ask the core through the map lock each frame and pointer move | `api/city_core_sight.cpp` | P53, P54, P55 |
| C11 | The unit of a derived product is larger than the unit of change | `sim/state/change_log.h` | P5, P8, P17, P22, P26, P27, P31, P32, P37, P44, P48 |
| C12 | Per-frame senders send whole buffers and uniforms, changed or not | `render/frame_admission.h` | P16, P18, P20, P25, P36, P38, P40, P56, P58 |
| C13 | The sky shader reads POSITION: the radiance is remade as the camera moves | `render/shaders/sky.gdshader` | P61 |
| C14 | The ground's whole look is worked out per pixel from procedural noise | `render/shaders/terrain_ground.gdshaderinc` | P60 |
| C15 | Screen terms are stacked full-resolution passes; antialiasing doubled | `project/render/screen_pass.gd` | P57, P63 |
| C16 | A person's pose is worked out per vertex per pass | `render/agents/people_plays.cpp` | P59 |
| C17 | One shadow technique for every distance | `project/render/sun_cascades.gd` | P24, P62 |
| C18 | Small repeated things are geometry copied into chunk meshes | `render/props/prop_kit.cpp` | P12, P14, P23 |
| C19 | Vertex data lives in three forms on a worker | `render/mesh_builder.h` | P7, P21 |

Commits each decision came in with: mimalloc 0f4b04af; LOOK_CANDIDATES 14ee21b1; garden_place d35f6bca; signature_bodies dc146d4f;
ruin_pieces 5cfbe009; people_pose 79000324; contact c571d66b; ground_seam and feed_seams 718f821f; kept_by_revision 2f1d3570;
body_cache 61bfab07; ring_meter 819223f3; ecology_raster and rock_strata 359cbeb2; people_plays ca787b01; frame_admission 752e4e02;
sky POSITION 022b734d; atmosphere 1c604b47; occlusion c978aeeb; lot_access e7346bfa; way_search d768370f; parking_sources a10336d3.

## 5. Fixes (one entry a fix: cause, why it cannot return, what leaves with it, the check)

### P49 (C1): the final heights are named by column and row
- Cause: `FinalHeights` kept the flat sample index of the whole-map array it replaced as its interface, so every read and
  write turned the index back into tile and offset with six integer divisions by runtime values, in the hottest read of the core
  (`Heightfield::height_at`: four samples a call).
- Change: `sim/terrain/final_heights.h` (new, split from `heightfield.h`): `at(x, z)`, `put(x, z)`, `row(z, x0, count)`,
  `to_base(rect)`; a tile's side is 2^6 samples, its slot and place a shift and a mask. `grid_sample` hands its callback
  `(ix, iz)` (`height_patch.h`); the callers (`compose.cpp`, `road_shape.cpp`, `ground_seam.cpp`, `terrain_bridge.cpp`, the
  state probe) pass the column and row they already hold; a region's rows are copied a tile run at a time (`final_row`).
- Cannot return: no index is formed to be taken apart; the store has no entry by flat index.
- Leaves with it: the 105 MiB second whole-map copy of a 20 km land (tiles stand only where a road or a pad changed the ground).
- Check: `cli.py run digest` PASS, the state digest equal to the golden; gates OK on the changed files.

### C3 (P41, P47): the pool's memory follows the work that runs at once
- Cause: every thread of the core has a heap of its own (mimalloc) and the pool handed work to any of its workers (one shared
  condition, `notify_one`): a thin stream of jobs went round all 23, each keeping the pages its turn touched; mimalloc's default
  committed whole every segment of a thread but its first; and 2d53b55e made a worker collect after 10 ms without work, which the
  simulation's `parallel_for` (20 Hz) turned into a forced purge on every worker twenty times a second.
- Change: `core/job_pool.cpp`: idle workers wait in a stack, each on its own condition; a submit wakes the one that worked last
  (a thread that never worked waits at the bottom), `parallel_for` wakes a worker a helper, never the whole pool; a worker that
  finishes is on the stack before it lets the lock go. A worker gives its freed memory back only when a keyed job ran on it since
  it last did (`owes`), so a helper woken by the tick collects nothing. `native/CMakeLists.txt`: `MI_DEFAULT_EAGER_COMMIT=0`, a
  segment is committed as its pages are used.
- Cannot return: the heaps that hold pages are the workers that ran at once, by construction of the stack; a collect needs a job
  that allocated; no segment is committed ahead of use.
- Left, under C19: a mesh's vertex data built by a job is freed by the frame's thread after the upload, so it returns to the
  worker's heap only when that worker works again (now the same warm worker). It leaves when a job hands engine-owned bytes.
- Check: `job_pool_test` (twelve jobs one after the other on a pool of four run on one thread; 3 threads before the never-worked
  rule), `cli.py run digest` PASS, gates OK on the changed files. Memory itself not measured (the owner's order).

### C13 (P61): the sky's radiance follows the light, not the camera
- Cause: `render/shaders/sky.gdshader` read `POSITION.y` for the low band's share. A sky shader that reads POSITION is drawn again
  into its radiance cubemap, and the map filtered, whenever the camera's position changes (Godot's sky shader reference: POSITION,
  and the Sky's automatic process mode); and `Daylight._set_sky` set every value of the material at every call, changed or not
  (a set value does the same).
- Change: the share is worked out by `Daylight.follow_view` (it already did, for the air's colour) and handed as the uniform
  `low`; the shader reads no POSITION and lost `low_from_m`/`low_to_m`. `Daylight._sky_set` sends a value only when it differs
  from the one sent.
- Cannot return: nothing in the sky depends on where the eye stands but one number, which moves only while the eye crosses the
  band between `low_from_m` and `low_to_m` (100 to 260 m); a still light sets nothing.
- Check: scripts and gates OK; the sky photographed hidden and frozen before and after from the street (16 h and 22 h), mid and
  high: at most 934 of 921,600 pixels differ, on leaf and wire edges (two runs differ so: suites.json, `quick`).
