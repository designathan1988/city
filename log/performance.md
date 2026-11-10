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

### C2, first part (P65): a task runs on the cores the run gave it
- Cause: a Godot task was counted as three cores by `tools/tasks/pool.py` while its process sized the core's `JobPool` (and the
  engine its own pool) by the machine's processors: eight tests at once started some two hundred workers on 24 threads.
- Change: the scheduler hands out processors, not a count: `Pool.free_cores`, a task's share goes to `proc.run(cores=)` and
  becomes the affinity of its Job Object (`win32.JobTree`, `JOB_OBJECT_LIMIT_AFFINITY`; `sched_setaffinity` elsewhere). A task
  also takes the cores no waiting task will ask for (`Pool._spare`): one test alone has the machine. The core's pool is sized by
  the process's affinity (`job_pool.cpp usable_processors`), so a confined process starts workers for its cores only.
  `JobPool`'s constructor returns once every worker waits on the stack (a starting thread no longer finds a queued job).
- Cannot return: what a process tree can run on is set by the kernel from what the scheduler counted; no pool inside it can
  take more, whatever it starts.
- Check: `tools/tasks/tests` (42 pass; the new one reads each task's affinity: 2 of 4 while another waits, 4 alone),
  `job_pool_test` three runs, `traffic_hash_test`, `digest` equal to the golden on a share of cores and on all of them.
- Open under C2 (P64): a core test still boots the whole game on a whole map.

### P1 (C4): the people's clip palette is no longer kept on the CPU
- Cause: `PeopleAsset` kept "the palette as the file has it" (`palette_bytes`, 55.3 MiB) for `clip_rows`, whose only callers
  (`BodyRenderer::add` and `::rise`) read frame 0 of `idle` and `crouchIdle` and the frames of `crouchUp`.
- Change: `people_asset.cpp keep_posed` cuts those frames from the palette at load (`POSED_CLIPS`, a line a body a frame) into
  `posed_bytes`; the palette's bytes go to the texture and are dropped. `clip_rows` reads the kept frames and fails loudly on
  a frame the asset does not keep.
- Cannot return: the asset has no member holding the palette; a new CPU reader must name its clip in `POSED_CLIPS`.
- Check: `strike_test` (bodies struck down are posed from it) and `people_test` pass, gates OK. The palette's 55 MiB of video
  memory stand (every body has its own block of every clip): open under C4/C16.

### P6 (C4): the signature looks' table is kept packed; a prepared kind is not kept
- Cause: `materials.sig` was one deflate stream of every look's record and picture, so the game unpacked all of it (33.1 MiB for
  127 looks, 92 pictures) and kept it to cut a picture from on demand; and each kind made kept its prepared surfaces (arrays,
  levels, pictures with mipmaps) for the Build tool's ghost.
- Change: `tools/assets/pack_signature_looks.py` (new) writes the records plain and each picture in a stream of its own
  ("RCSL"; the file went from 587,033 to 569,660 bytes). `signature_bodies.cpp`: `read_table` keeps the file as it is on disk
  and the records; `picture` unpacks one stream; `mesh_with` reads the kind's file again for the ghost (`made_` is gone).
- Cannot return: nothing holds an unpacked table or a prepared kind after its upload; the only bytes kept are the packed file.
- Also: `BuildingRenderer::set_detail_scale` tells the cards their range only once their material stands (a tier applied before
  the boot logged "Parameter material is null" in every capture; `init` tells them with the stored scale).
- Check: `model_test` (the church keeps its own design), `building_variety_test`, `photos`, `digest` pass; no signature error in
  the logs. Open under C4: the pictures are uploaded as plain RGBA8 (a compressed form needs the cook).

### Photos of the fixes that can reach the screen (dist/review-c4, 2026-11-10)
- Before: a copy of `project/` with the tracked files of 2d53b55e laid over it (its DLL, scripts, shaders, assets). After: the
  working tree at P6. Hidden, frozen, 1280x720, the town of seed 1: three streets from the kerb, three edges of the town where
  the streets meet the land, the bare land far and in the hills, the whole town, the town in nine quadrants near enough for the
  facades (every signature building stands in one), a street and the town at night: 20 pictures a side.
- Result: 10 pictures equal pixel by pixel (both land views, the whole town, two streets, both night views, three quadrants);
  the other 10 differ in 1 to 608 pixels of 921,600. The pixels that differ are the outlines of tree crowns (the leaf cards'
  edges under TAA: two runs of the same build differ so, suites.json `quick`). Land, streets, kerbs, buildings and signature
  facades are the same. So P49 (the final heights), C13 (the sky) and P6 (the signature looks) changed no picture.
- Live (the clock running, 433 vehicles and 1,056 walkers, a strike of force 5): before and after show the same streets,
  walkers and vehicles; no fallen body stood in the frame, so P1's posed frames are proven by `strike_test` (212 bodies posed
  with no refusal) and by construction (the same bytes copied), not by a picture.

### P4 (C5): the rooms behind the panes are budgeted in bytes, with the bodies they keep alive
- Cause: `BuildingBodies` kept up to 8192 `PaneRooms` and 1024 cut bodies "each counted as one"; a `PaneRooms` holds its whole
  body, so every building the near or mid level ever placed stayed in memory past the body cache's own budget.
- Change: tuning `render.rooms_cache_mb` (64); `BuildingBodies::set_budget` gives it to both caches; a rooms entry costs its
  body's bytes (`sizeof(Body) + heap_bytes`), a cut body its own.
- Cannot return: nothing in the render keeps bodies by count; what the rooms pin is bounded by the budget.
- Left under C5: the floors and rooms a `PaneRooms` works out after it is kept are not in its cost; they leave when the rooms
  read a facet instead of the body.
- Check: `model_test`, `digest`, `photos` pass; the frozen review plan again (dist/review-c5): 7 of 20 pictures equal pixel by
  pixel, the others differ in 1 to 934 pixels on leaf and wire edges (the same counts two runs of one build give).

### C5, the lot elements (P28, part of P45 and P50): a lot's plants, gates and bays are a list of their own
- Cause: what stands on a lot round its building (`Body::elements`) had no form but the whole generated body: every tree job
  (near and far) asked `record_body` of every lot of its chunks for the garden's plants, `street_gates` asked `record_mass` of
  every lot along a street for its car gates, and the parking read bays and docks the same way; each asking made or fetched a
  whole body through the one cache the drawn buildings share, and pushed them out.
- Change: `BodyCache::elements` (`record_elements`): the elements kept by the body's key in a cache of their own; on a miss
  they are copied from the kept body (`LruCache::find`, nothing made) or from a body made for them alone and let go, so the
  body cache never takes a lot nobody draws. `place_garden`, `street_gates`, `read_lot`, `read_lots_with_docks` read the list;
  `lot_bays` takes the elements.
- Cannot return: these readers cannot name a body; a lot's elements are made once a key and cost their own bytes.
- Left under C5: `read_bay_way` and `yard_input` still take the body (the lot's solids for the way's grid), growth's
  candidates (P43) and the sight solids.
- Check: `digest` (the state's hash), `traffic_hash_test`, `yard_test` pass; the frozen review plan (dist/review-c5b): the
  gardens' trees stand where they stood (6 of 20 pictures equal, the others the leaf-edge noise of two runs); gates OK.

### P48, the growth's part (C11): a grown building no longer drops the kept street furniture
- Cause: `FurnitureKept` was keyed by the city's whole revision, which `city_growth.cpp` moves at each building it adds
  (`growth_per_s` 25): with the city growing, every render job and the walkers placed the furniture of every street in reach
  again, all the time (`segment_furniture`, `merge_street`), through one mutex.
- Read: a placement's inputs are the network, the junctions' control (`network_arms`, the node's fields), the player's items
  and cuts (`s.street`) and the pieces knocked out (`s.damage.gone`): no building.
- Change: `CityData::street_revision`, moved by `touch()` (every command, undo, redo, a new or opened map) and not by
  `touch_grown()`, which growth now calls; `street_furniture_of` keeps by it; `take_city` carries it on; the snapshot shows it.
- Cannot return: growth has no call that moves the furniture's key. Left under C11: any command still drops every
  segment's furniture (the key is not yet the segment's own inputs).
- Check: `growth_pick_test` (new check: buildings grew and the streets' revision stood), `digest`, `traffic_hash_test`,
  `street_works_test` pass; gates OK; the frozen review plan (dist/review-c11): lamps, trees and benches where they stood
  (8 of 20 pictures equal, the others leaf-edge noise).

### P22, the node shapes (C11): a grown building no longer drops the kept shapes of the road nodes
- Cause: `node_shape` kept each node's ends and plate corners for the city's whole revision, moved by every grown building;
  the road jobs of every chunk in reach made them again while the city grew.
- Read: `ends_of` and the corners read the network, the segments' frames (curve and grade), the profiles and the tuning.
- Change: `road_shapes.cpp node_shape` keeps by `CityData::street_revision`. The walls of a cut (`walls_of`) stay on the whole
  revision: they read the ground held beside the road, which a building's pad may move.
- Check: `digest`, `road_grade_test`, `road_commands_test` pass; gates OK; the frozen review plan (dist/review-c11b) equal
  but leaf-edge noise (7 of 20 pictures equal pixel by pixel).

### P56, P58 (C12): the seams, the atmosphere pass and the sky send a value when it moved
- Cause: `CityTerrain.feed_seams` set every look uniform of the seam material and eight server values every frame,
  `SkyView._light` six values of the atmosphere pass and four shader globals, whatever had changed; a value set makes the
  engine write the material's uniforms again and remake its set of textures.
- Change: `project/render/sent_params.gd` (new, the one owner of "sent when it moved": a material's values and the shader
  globals as last sent); the seams, `SkyView._light` and `Daylight` (its `_sky_set` is gone) send through it.
- Cannot return: these owners have no direct set left for the values they work out every frame.
- Check: `scripts`, `photos`, `digest` pass; gates OK; the frozen review plan (dist/review-c12): 9 of 20 pictures equal pixel by
  pixel; the others differ on tree-crown outlines, and in `05-edge_low` on a striped paving seen through leaves at the
  picture's corner (1,646 pixels: the same leaf and thin-line noise of two runs, there larger).

### P40 (C12): the effects tell the engine nothing while nothing of theirs lives
- Cause: `Puffs::stamp`, `Marks::update` and `Mounds::update` set their material's `now` every frame, and
  `EffectWorld::light_update` four values of the flash's light, with no effect alive: eight server calls a frame of the game.
- Change: the puffs' `now` and wind are told in `flush` while a puff lives (the layer is hidden when none does), the marks'
  and the mounds' while one stands, and the flash's light while it burns and once to hide it (`flash_lit_`).
- Check: `strike_test` passes; gates OK. Photos (dist/review-c12b/open and /night, a strike on open ground): by day the flames,
  the shock ring, the thrown rocks and their shadows at 0.1 s, the fire and dust at 1.2 s, the crater's mark and rocks at rest at
  16 s; by night the flames and the ring. Seen and not from this change (the same on the tree before it): the flash lights
  nothing round it at night, and the crater's mark stays bright in the dark; both recorded in the ledger.

### P20 (C12): a MultiMesh draw tells the engine its count, box and visibility when they change
- Cause: `MultiMeshDraw::show` made four server calls at every `set` (the MultiMesh's and the instance's box, the visible
  count, the instance's visibility), for every per-frame draw (vehicles, people, lenses, trains), changed or not.
- Change: `rs_util.h`: the draw keeps what it told last (`box_`, `told_`, `drawn_`) and tells the difference; a new
  allocation or an adopted MultiMesh forgets it. A per-frame draw costs its buffer; one that stays empty, nothing.
- Check: `digest`, `people_cast_test`, `street_crossing_test` pass; gates OK. Photos of the town running (dist/review-c12c,
  16 h and 22 h, near, street, mid, far): vehicles on the lanes with their lamps at night, walkers on the footways and the
  crossings, trees, wires and posts drawn; footways meet the lot fronts in paving. Seen, not from this change: no pool of
  light under the street lamps at night (recorded in the ledger).

### C9, first piece (2026-11-10 04:30, 4e5d2bea): the measure of the cause, and what a cut of the city stands on
- The check that fails because of C9: `tests/core/map_hold_test` (new). `MapRead` (`sim/state/city_state.h`) is the render's one
  guard of the map's shared lock and counts how deep its thread holds it; in the tests' DLL (`CITY_CHECKS`, `core/checks.h`)
  `MeshBuilder::add` and `mm_push` count every vertex and instance taken while the thread holds the map, by the lane of the job
  (`sim/state/map_hold.h`, probe `map_hold`). The town of seed 1 streamed in from three views, before any conversion: roads
  230,708, far roads 69,196, seams 473,416, props 706,682 built under the map; buildings 0 (they gather, then build). The test
  names the producers that must build nothing under the map; a lane is added to it as its producer is converted.
- P76 (new, `sim/roads/street_furniture_posts.cpp row_face`): `lamp_row` walked the whole street both ways through every node of
  two arms at every call (and again for the other arm of each joint), in every road, far road, seam and prop job and every
  furniture placement: n walks of n pieces for a street of n. The face is now a fact of the street the network settles with its
  runs (`StreetRun::face`, `RoadNetwork::settle_faces`: one walk a chain a settle, for the chains a dirty segment is on);
  `row_face` reads it; `RoadNetwork::going_on` is the chain's one definition. Cannot return: no reader of a segment has a walk
  of its street left to call.
- P75 (new, `sim/roads/road_network_shape.cpp settle_runs`): the settle allocated and zeroed a flag for every segment id of the
  map at every road command; it keeps the segments it wrote in a small sorted list.
- What a cut stands on (no producer uses it yet): `core/id_table.h` `IdTable` (the id tables of `RoadNetwork` and `RoadGrade`:
  a vector by id in the live city, a box's records under the same ids in a cut; an id a cut does not hold stops the tests'
  DLL), `SpatialHash::window` (the cells of a box, answering for that box alone; a query past it stops the tests' DLL),
  `RoadNetwork::cut` (`sim/roads/road_network_cut.cpp`), `sim/terrain/ground_patch.h` (`GroundPatch`, `GroundRead`).
- Check: `digest` equal to the golden, `traffic_hash_test`, `road_commands_test`, `road_edit_test`, `road_grade_test`,
  `street_power_test`, `street_works_test`, `map_hold_test` pass; gates OK on the changed files. No picture can change yet (the
  jobs still read the live city); photos at the close of C9.

### C9, second piece (2026-11-10 04:51, b9fedc5d): the roads, the structures, the far roads and the water build off the map lock
- Change: `sim/state/city_view.h` `CityView` (references to the stores a job-path function reads; a `CityState` converts to
  it, so a reader that holds the map calls the same functions) and `sim/state/city_cut.h` `CityCut` with `cut_city` (the one
  place a job takes the map, shared, for the copy alone): the ground of the box as a `GroundPatch`, the roads by
  `RoadNetwork::cut`, the lots' records by `BuildingRecords::cut`; the catalogue and the generator's tables by reference.
  `BuildingStore` is now `BuildingRecords` (what a cut copies: items, hash, query) plus the tables and the writers.
  `MeshQueueOf::submit_cut(gather, build)`; `render/roads/road_cut.h` `cut_for_roads` (the chunk or superchunk, the boxes of the
  segments it owns, grown by `road_reach`: casing band, wall ground, stay legs, portal wings, pier feet, the lots' car gates).
  `RoadEmit` holds a `CityView`; `RoadRenderer`, `RoadLayer` (structures), `FarRoadRenderer` and `WaterRenderer` submit by it.
  `each_owned_segment` asks the chunk's own box (a segment's middle lies inside its box): a ninth of the candidates it read.
- Cannot return for these four: their job has no `CityState` to read while it builds, only its cut; a read past the cut
  (an id, a hash query, a ground or water sample) stops the tests' DLL.
- Measured by the check: `map_hold_test`, the town streamed in: roads 230,708 -> 0 and far roads 69,196 -> 0 vertices built
  under the map (structures and water 0: the town has none; the scenic Sao Paulo capture, with its bridges, tunnels and rivers,
  ran on the tests' DLL with no stop). Left under the map: seams 473,416, props 706,682, and the producers the count does
  not see yet (cover, trees, overlays, transit, the fields).
- P77 (new, found by `edit_fuzz_test` failing after 4e5d2bea): `ByteWriter::put<T>` and `put_vec` write a record as its
  object's bytes; `PlantedTree`, `TreeClearing` (a bool before floats) and `FogDab` (a byte after floats) have three bytes of
  padding, which hold whatever lay in memory: the save and the state's hash were not functions of the state, and the test had
  passed by the accident of what the memory held. The three records name those bytes as a field (zero; `read_nature` zeroes
  them for an older file) and, in the tests' DLL, `ByteWriter::whole` asks the compiler whether a type written has padding and
  stops on one that has. Cannot return: a padded record written raw stops the first test that saves.
- Check: the 55 test scenes and `digest` (equal to the golden) pass; gates OK on the changed files. Photos at the close of C9.

### C9, third piece (2026-11-10 05:02, 4c927115 and 97f4a22d): the ground seams and the street furniture build off the map lock
- Seams (4c927115): `cut_for_seams` (`render/roads/road_cut.h`): the chunk and, whole, the segments whose paving can come
  within a terrain cell of it, with the ground under them. 473,416 -> 0 vertices built under the map. P10's other half (the
  paving outline has no owner: the seam still draws the neighbourhood's roads to find it) stays open.
- Props: `StreetWorks::cut` (the items, poles, barriers and cuts filed in the box, the poles up to two spans from its poles
  with the spans between, the name plates of its chunks; `StreetTable` holds an `IdTable`). A cut carries facts
  (`CutFacts`, `sim/state/city_cut.h`): what a box of records cannot give, read from the live city while the map is held
  (`keep_furniture`: a segment's kept furniture, whose placement reads the hydrant squares round its ends; `keep_node`: a
  junction's control and the plates its rules put up, the traffic's). The job-path readers `furniture_of`, `node_signalled`
  and `node_signs` answer from the live city or from the cut's facts, so a job writes none of the caches the simulation reads
  (`FurnitureKept`): a cut can never put a placement made from a piece of the network into the walkers' state.
  `PropRenderer::gather`: the chunk, its owned segments and the poles a span away, grown by `road_reach`, the footway's reach
  and a barrier's piece. 706,682 -> 0 vertices and instances built under the map.
- `MeshQueueOf::submit_job` (the build under the map) is gone: no queue of single-surface chunks can hold the map while it
  builds. `map_hold_test` names roads, far roads, structures, water, seams, props and the buildings: all 0.
- Check: the test scenes and `digest` pass, the scenic capture ran on the tests' DLL with no read past a cut; gates OK.
- Left in C9: the producers with a queue of their own, which the count does not see yet (cover, trees: P33; overlays: P35;
  transit on the main thread: P34; the shore, canyon, fog and ecology fields; the ruins), and P10's outline owner.

### C9, fourth piece (2026-11-10 05:20): the trees and the ground cover build off the map lock (P33)
- Cause here: the tree jobs (a chunk's models, a superchunk's cards of sixteen chunks) and the cover job placed every plant with
  the map held shared: the woods' lattice with its neighbours, the gardens of every lot, the ecosystem, the shore's search.
- What a cut gained (`CUT_NATURE`, `NatureState::cut`): the paint (`GroundPaint`: its tiles are shared, a brush stroke copies the
  tile a cut still holds, so a cut costs pointers and no texel), the planted trees and the clearings (`PlantedTrees` on `IdTable`,
  their hashes windowed), the laid elements of the box's chunks, the nature settings; and the city's seed. `GroundRead` answers
  the ecosystem and the shore (`compute_ecology`, `shore_level`, `shore_near`). `tree_place`, `garden_place` and `GroundKept`
  read a `CityView`: a chunk's ground made from a cut serves every job of that revision (the ids are the city's own).
- `CoverRenderer::submit`: the chunk grown by the ground's reach (`ground_reach`: the lists' margin and the ecosystem's corners)
  or the shore's search. `TreeRenderer::gather`: the chunk or the superchunk, the segments its chunks own (street trees stand
  along the whole segment; their furniture kept as a fact) and the lots that stand in it by their half diagonal (a garden is
  planted over the whole lot), grown by the farthest of `ground_reach`, `tree_reach` (the woods' neighbours, a cell's jitter,
  the grade's probes) and `road_reach`.
- Cannot return for these three: the job holds no `CityState` while it places; a read past its cut stops the tests' DLL.
- Check: `map_hold_test` names trees_near, trees_far and cover: 0 built under the map, the town streamed from three views with no
  stop; the 56 tasks of `tests`, `digest` (equal to the golden) and the scenic capture pass; gates OK on the changed files.
- P78 (new, C11's kind, open): the shore field copies and spreads the whole map's water grid again at any change of the land.
- Left in C9: the overlays (P35), transit (P34), the canyon, fog and ecology fields, the ruins' probe, P10's outline owner, and
  the gate that forbids `map_mutex` in `native/src/render`.

### C9, fifth piece (2026-11-10 05:40): the map layers (P35, P79, P80)
- Cause: the congestion map baked its colours into vertex colours, so every `refresh_s` each road chunk in reach had its
  ribbons copied, coloured and made a new mesh on the main thread, the old one freed (`update_congestion`); the ribbons were
  laid there too, under a raw shared lock of the map taken by `CityCore::_process` for the layers alone; and the zone job held
  the map for its whole build.
- The check first: `tests/core/overlay_test` (new) with the probe `overlay_probe`. On the code before: 64 meshes made in two
  refreshes of 32 chunks. After: 0.
- Change: a chunk's carriageways are laid once per change of its roads by a job from a cut (lane `congestion`), each vertex
  with its segment's id in UV.x; `render/overlays/flow_levels.h` (new) owns a texture of levels, a texel a segment (the
  `PoseFrame`'s speed share on the ramp, 8 bits), sent into the texture it has when a level moved and made anew only when the
  segments outgrow it; `render/shaders/overlay_flow.gdshader` (new, material `overlay_flow`) mixes the ramp's three colours
  from the level, as the vertex colours did. The zone job gathers a cut of its roads and ground with the player's zoned lots
  near its chunk copied in the same hold. Both layers' meshes are baked on the worker (`ChunkResults`), handed in the frame's
  admission, streamed nearest first; `OverlayRenderer::update` reads nothing of the map and `_process` takes no lock for it;
  `RenderWorld::pending` counts their chunks. `materials.json`'s `overlay.opacity`, which no shader read, is gone (P80).
- Cannot return: a refresh has no mesh to make (the colour is not in the mesh); the layers' jobs hold no `CityState` while
  they lay; the main thread has no reader of the map left in the layers.
- Check: `overlay_test` (refreshes make no mesh; the level texture on the GPU holds the levels sent; the ramp's ends and
  middle; zones drawn; lanes zones and congestion 0 under the map), `map_hold_test`, the test scenes and `digest` pass; gates
  OK. Photos (dist/trace/performance-root-causes/photos, town of seed 1 running): the congestion map from near, mid and far by
  day and at night, each carriageway one colour of the ramp (free green, middling yellow, jammed red) lying on its road,
  changing at the junctions; the zones beside the zoned sides and over the lots in their zone's colour.
- Left in C9: transit (P34), the canyon, fog and ecology fields, the ruins' probe, P10's outline owner, the gate.

### C9, sixth piece (2026-11-10 05:55): the fields, the bay census, and the gate that keeps the render off the raw map lock
- The ecology raster worked each chunk's ecosystem out with the map held (`compute_ecology` under the shared lock, a chunk at
  a time: some 6,100 holds after a new map of 20 km): each chunk now takes a cut of its land and paint
  (`CUT_NATURE | CUT_GROUND_BASE | CUT_WATER`, the chunk grown by `ground_reach`) and works it out with the map let go.
- The canyon, fog and shore fields already copied under a short lock and built after it: their copies are taken through
  `MapRead`, so the count sees them. `bay_census` (the lab's census, a probe) placed every building of the map on the main
  thread with the map held to its end (P81): it copies the records, then each record's ground under a lock of its own, and
  places with the map let go.
- The gate `RENDER_MAP_LOCK` (`tools/gates/rules.json`, with its passing and failing fixtures): `map_mutex` may not be named
  in `native/src/render`. The render takes the map through `MapRead` (the frame's sync, the fields' copies, the probes) and a
  job only inside `cut_city`. Cannot return: a new producer that locks the map itself fails the gates.
- Check: `map_hold_test` names seventeen lanes (every chunk producer, the layers, the four fields): 0 built under the map;
  the gates' selftest (72 cases) passes. The full gates fail only on what stood before this line (ESPELHO: the empty mirrors
  table, log/pendencias.md; TASKS.md, another session's file).
- The fields' whole-map and whole-window units (P31 shore, P32 ecology, P37 canyon) are C11's: their plans are in the ledger.
- Left in C9: transit (P34, built on the main thread under the frame's `MapRead`), P10's outline owner, P73, P74.
