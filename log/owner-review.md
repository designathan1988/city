# The owner's review of the game: findings, causes and fixes

The working record of the list in `log/pendencias.md` ("The owner's review of the game", 2026-11-09): each problem as it is
found, every candidate cause with its evidence, the cause that held, the fix and its verification. Written as the work goes,
so nothing found is lost between steps. A problem leaves `pendencias.md` when its fix is committed; its record stays here.

Method (the owner's brief): locate the lab function by the ledger, compare both versions line by line, name the difference
with file:line on both sides before editing; only when they are equivalent, one measurement that separates the hypotheses.
Verification in the live game (`godot --path project res://game/play.tscn -- --live=<dir>`, tests/visual/live_driver.gd),
town map (index 0), seed 7, simulation running at 1x, photos near, mid and far, low and high.

## 1. Walkers drawn as black boxes from about 50 m

**Seen (2026-11-09, live, town seed 7, 07:30):** at 60-150 m along a street, standing black rectangles (rgb about 5, 10, 15)
twice as tall as wide on the footways; the walkers nearer (near and mid mesh levels) are right. With `agents.json`
`people.blob.from_m` set to 5 for one run, every walker became such a box, even at 10 m: the far card, not the mid mesh.

**Lab:** `src/render/people/personImpostor.ts` (createPersonImpostor, VERTEX_MAIN, MAP_MAIN), chosen by
`src/render/riggedCitizens.ts` citizenLevelOf (under IMPOSTOR_PIXELS = 8 device pixels).
**Game:** `project/render/shaders/people_far.gdshader`, the strip from `native/src/render/agents/people_asset.cpp` read_card,
set at `native/src/render/agents/people_boot.cpp:191`.

**Differences listed:**
1. The strip (x, y, a frame's width and height in uv) was a `godot::Color` set on a `vec4` uniform. Godot converts a Color
   set on any non-texture uniform from sRGB to linear unless the uniform is hinted `color_conversion_disabled`
   (servers/rendering/renderer_rd/storage_rd/material_storage.cpp, update_uniform_buffer and convert_to_vector). The file's
   strip (0, 0.9375, 0.03125, 0.0625) reached the shader as about (0, 0.863, 0.0024, 0.0050): every card read one texel near
   row 884, inside an opaque black region of the atlas. The lab passes a Vector4 (`uFrame`). **The cause.**
   Measured once: the card shader drew `vec3(v_uv, texel.a)` and the colour was the same over the whole card (the uv did not
   vary), alpha 1.
2. The lab clamps the card's mip to log2(frame texels) - 2 so the next frame never bleeds in (MAP_MAIN); the game sampled the
   whole mip chain. Followed.
3. The lab's card is as wide as its frame's aspect (`uCard.x = height * frame.width / frame.height`); the game had a constant
   CARD_ASPECT 0.5 (equal for these 32 x 64 frames, a second owner of the strip's shape). Now strip.z / strip.w.
4. The v axis: the lab flips v (`1 - (y + h) / size`, its KTX2 rows bottom first); the game's atlas PNGs hold the strip
   upside down at the bottom rows (figures' feet at row 960, heads at 1023), and the game's v grows with the card's height
   from strip.y: equivalent, checked on the decoded atlas (layer 0).
5. Level choice: the lab by the figure's pixels (card under 8 px), the game by distance (`lod_m` 40, card past `blob.from_m`
   90 m). Left: the mid mesh is right at 40-90 m in the photos and the card is now a figure.

**Fix:** `PeopleAsset::card_strip` is a `godot::Vector4` (people_asset.h/.cpp); people_far.gdshader clamps the mip and takes
the width from the strip. No other `material_set_param` in `native/src` sends a Color to a uniform that is not a colour
(checked: signature_bodies albedo/emission, agent paints and helmet colours are colours).

**Verification:** both DLLs; live photos at the same spots (street at -75.5, 175.6, yaw 90: pitch 12 and 40, distance 30,
120, 250): no black box, the far walkers are small coloured figures. Probe: people_test `_card` checks that the material's
strip is a Vector4 equal to the file's (fails with a Color). people_test passes.

## 9. The picture shakes; vehicles jerk and leap forward abruptly; the frame stalls

**Seen:** by the owner, watching the live game (2026-11-09).

**Measured (one session, town seed 7, camera still, 1800 frames, census in dist/live/result.txt):** frames of 972-1009 ms with
the core's own _process at 0.4 ms, the tier unchanged and 6 "surface" pipeline compilations in the frame (Performance
PIPELINE_COMPILATIONS_SURFACE). The 950-1250 ms gaps between pose publications are the same frame seen from the triple buffer.

**Cause 1, the stall (Godot docs, Reducing stutter from shader (pipeline) compilations, via Context7 /godotengine/godot-docs):**
Godot 4.4+ precompiles a surface's pipelines when the mesh is made ("Mesh" step, on the thread that makes it, a job here) from
the surface's own material; "modifiers such as material overrides can't be compiled on this step", so they wait for the
"Surface" step, which stalls the first frame the instance is drawn. The render gives most of its draws their material as an
instance override: `MultiMeshDraw`, `swap_base` and `ShadowTwin` (render/rs_util.h), `MeshDraw` (render/mesh_draw.h),
`MmLayer` (render/effects/mm_layer.h), the poles' casts (props/prop_renderer.cpp), the buildings' fades (a material swapped
at run time, kept from stalling by "warmers": buildings/building_renderer_reach.cpp). Vehicles, buildings, transit and ruins
already put their material on the surface. **Fix (no warming, the owner's rule 2026-11-09):** every mesh is made with its
material on its surfaces, by the job that makes it; no instance override; a mesh drawn with two materials becomes two meshes;
the shadow twins draw the depth pass of the mesh's own material (the engine substitutes its own depth shader for one that
does not move vertices or discard); the buildings' fade stops swapping materials, and its warmers go.
A first attempt (MultiMeshDraw::prepare, instances made at boot) was warming and was reverted.

**Cause 2, the shaking shadows:** the sun moves in steps of `daylight.json clock_step_h` = one game minute = one real second at
1x (0.25 degree): every second every shadow jumps (about 0.5 m for a 30 m building at 30 degrees). The lab stepped its sun to
spare its cached shadow store (sunCascades.ts SUN_STEP, sunTolerance: a step at most one texel of the near cascade); Godot
draws the directional shadow every frame anyway, so the step saves nothing and only makes the jump. Fix: the sun steps by the
lab's tolerance alone (one near texel of shadow displacement), not by a game minute.

**Cause 3, GPU:** the tier's `shadow_least_m` (2500 m at high and ultra) floored the lab's shadow reach (daylight.json
shadow_reach_m 800, sunCascades.ts SHADOW_REACH): the shadow pass drew 7.6 M primitives in 1039 draws for a view of 3.0 M in
620; GPU 10 ms. Removing the floor was tried and reverted: the frozen wide view of the town (00-cidade) lost the shadows of its far
half (the lab's look + 800 m ends some 1.7 km out from 400 m up, the town reaches past it). Open: a reach fitted to what the view
sees of the town, without the floor's cost in a street view.

**Vehicles that vanish (§2, read in the code):** `vehicle_motion.cpp leave_lane` took a car away at its goal (mid-street) or with
no way on, and `step_vehicles` one stuck 120 s, in front of the camera. The lab removes a car only out of sight and gives a car
in sight near its end somewhere else to go (sim/vehicles/spawn.ts stepDespawn, sim/ambient/ambient.ts traffic). Fixed:
`in_sight` (vehicle_motion.cpp, within the published radius); in sight, a car at its goal or without a way on takes a new
trip. Keeping a stuck car in sight too was tried and reverted: yard_test lost its docks' ways (18 of 21 against 28 of 28 with the
HEAD DLLs, the same yard): stuck cars stood in them. Taking a stuck car away hides that it got stuck: why cars stick is §2's
cause, still open; until it is fixed a car stuck 120 s still vanishes.

## 2. Vehicles standing in the lanes, over one another and on the footways

**Seen:** the owner (2026-11-09, live game): vehicles pass over one another, drive onto footways (photo: a white sedan across
the kerb on the footway, two people beside it, near a zebra), stand still in the lanes, and vanish all at once. Not yet
investigated.

## 8. Memory, GPU and CPU

**Seen:** the owner (2026-11-09): RAM near 5 GB on the small city, where it took no more than 800 MB before. tasklist of the
live game (town, seed 7, running): 4,581,448 K working set. CPU and GPU high too: the fix must not raise them.
**Leads already in pendencias.md:** the town map is 20 tiles (20 x 20 km) for a city of 640 m radius (land, paint tiles, ecology,
woods and Terrain3D's maps of 400 km²); some 720 MB of video made at run time; the sun's shadow reaching 2500 m.
The jump from 0.8 to 4.6 GB is to be dated by the commits (what changed since the 800 MB) and each MB owned in the code.

**Measured (the census, town seed 7):** working set 4.42 GB (4.61 after a minute running), private 6.0 GB. Godot's allocator
(static) 750 MB; the core's (mimalloc) 894 MB committed: the land's heights 175 MB (26.2 M samples: 20 km at 4 m), the bodies'
cache 66 MB, the rest not yet owned; the Windows heaps 2.39 GB allocated: 1.64 GB of them neither Godot's nor the core's (the
GPU driver's or a library's: the driver report needs `--extra-gpu-memory-tracking`); video 1202 MB (textures 894, buffers 214).
Terrain3D: 400 regions of 256 x 256 samples (height, control, colour) on the CPU and the GPU. A map opened after a larger one
keeps the larger one's memory (vectors are assigned, never shrunk). The land alone (no city) already holds 4.40 GB: the city
adds 0.2 GB.
**Cause (architecture):** the game keeps the whole 20 km land at 4 m in the core, in Terrain3D's maps and on the GPU for a city
of 640 m; the lab has no map edge and builds the ground round the view (render/terrainLod.ts, farGround.ts, frustumGround.ts).
To rewrite: the land kept near the view at full spacing, coarser away from it.
