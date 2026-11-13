# How every ported feature is built here (the backbone), and what never comes from the reference

Specification 003, 2026-11-13. The owner's orders: the port is complete (everything the reference has) and right; the reference's
behaviour is mostly right, its code carries performance, architecture and engineering faults that are identified and never
brought; one must always know where a feature lives, who owns its state, how a change reaches what depends on it, and where an
error happened.

## What stands today (checked in the code, 2026-11-13)

- One source of truth exists: `CityState` / `CityData` hold the whole city (native/src/sim/state/city_state.h:61-136), the render
  reads it through `MapRead` (city_state.h:140-154), the C++ layers are `core -> sim -> render -> api`.
- Change propagation exists but is written by hand: `ChangeLog` is a bag of twenty lists (native/src/sim/state/change_log.h:20-53);
  each producer must know every consumer's list, and nothing checks that a change reached all who depend on it.
- Ownership is declared for 9 concepts (tools/gates/owners.json) in a tree of about 1,190 files; features have no map:
  finding one takes reading and tracing.
- Record ids are reused most recently freed first with no generation (native/src/core/slot_ids.h:1-4): a reference kept past
  its record's removal attaches to whatever takes the id next (lane links and bans after a road removal, flow statistics and
  the control advisor after a junction is rebuilt: agents D and E of sheets/roads.md).
- No invariant is checked while the game runs (no check in native/src outside the api's test probes).
- Failures go unseen: a refusal with no text key shows the raw key (`hint.road.invalid`), Apply past 256 custom roads does
  nothing, a grade-limited end is never told (sheets/roads.md).

## The backbone (each ported feature lands in it)

1. **One error channel.** Every engine and extension error and warning is caught by a `Logger` registered with `OS.add_logger`
   (Godot 4.5+, Context7 /godotengine/godot-docs, tutorials/scripting/logging: `_log_error` gets function, file, line, rationale
   and type, from any thread, so it holds a mutex). The core's own failures (an invariant broken, a reference found stale, a
   command refused for a reason with no text) go to one sink in the core read by the same channel. The count shows in a debug
   overlay; every test scene fails on any entry.
2. **Every command returns its outcome.** Done, refused with a reason key and its arguments, or done in part; the interface
   shows a refusal in one place (the store's dispatch), never per tool; a gate checks every reason key a command can return
   exists in `project/data/i18n`.
3. **Records by handle.** A slot keeps a generation that moves when it is freed; a reference to another record holds the id
   and the generation, and reading it through a stale generation is a reported fault, never a silent hit on the new record
   (the slot map with generations: P0661, https://open-std.org/JTC1/sc22/wg21/docs/papers/2017/p0661r0.pdf; generational
   indices as every ECS handle uses them, https://root-11.codeberg.page/intro-book/trunk/10_stable_ids_and_generations.html).
4. **A record's removal reaches every table that refers to it.** One removal path per kind of record: each table holding
   references to that kind is a declared dependent and drops or remaps them in the same command (the links and bans of a
   road, a junction's flow statistics and control choice, a transit stop on a segment).
5. **Invariants per feature.** Each feature declares the checks that must always hold (no link to a dead road, every
   approach lane has an exit, every junction plate at its node's height, nothing stands in a wall); they run after every
   command and every batch of ticks in the debug DLL and always in the tests, and a failure names the feature, the check and
   the records.
6. **The feature map.** A machine-read registry: each feature with its files, its state (the CityState fields it owns), its
   commands, its simulation steps, what is derived from it and how it is told, its interface entry, its data keys, its tests
   and its invariants. A gate holds it true: every file of `native/src` and `project/{game,ui,render}` belongs to one feature,
   every symbol it names exists, every derived product names the change it rebuilds on. "Where is X" is one lookup.
7. **Declared propagation.** A derived product names its inputs (the records and the ChangeLog list it rebuilds on); a
   producer marks by the kind of record changed, and the dependents of that kind are told (the separation of what is
   rebuilt from how dependencies are tracked: Mokhov, Mitchell, Peyton Jones, Build Systems a la Carte, ICFP 2018,
   https://www.microsoft.com/en-us/research/publication/build-systems-la-carte).
8. **Cost declared and measured.** Each feature states when it works (per edit, per tick, per frame, per view change) and
   what it holds, and its check at its end measures it against `doc/specification.md`'s budgets.

Order: 1, 2 and 5's runner first (they make every later check see what goes wrong), then each piece where the port first needs
it: handles and removal with the roads' links and bans, the map growing feature by feature as each is ported and closing with
the gate once every area is in, propagation declared as each area's products are rebuilt.

## What never comes from the reference (its own reports and the agents' reading)

From the reference's own reports (read whole 2026-11-13: reports/why-10km-cities-fail-2026-10-26.md 1-149, .whole-review/FINDINGS.md
1-147, ARQUITETURA.md 1-97, docs-optimization-brief.md 1-27):

- The whole map kept as loose objects and the whole map's derived data built at once: one elevation solve for the whole network
  (render/renderer.ts:2425-2432), every road tile of the network's extent after the reveal (renderer.ts:2393-2395), string keys per
  block (renderer.ts:2354-2358). Here: records in typed tables, derived products per chunk, bounded by the view and a budget.
- Every road edit's rules solving the whole map's heights again (editor/editRules.ts:66, F-0013), repeated at each pause of the
  pointer by the dry run (editor/roadTool.ts:412, F-0019). Here: a draft is solved on its own reach.
- Generation serial on one thread, its result applied whole on the page (main.ts:2179-2231: 95 s, 422 ms stalls). Here: the job
  pool, deterministic per cell.
- Every product refreshed each frame though nothing moved (core/engine.ts:100, F-0005); queues re-sorted each frame
  (editor/lotTool.ts:807, F-0014); keys rebuilt as strings each frame (lotTool.ts:941, F-0015; roadTool.ts:393).
- Swallowed errors (core/clipper.ts:134, core/derivedCache.ts:41 and nine more, editor/persistence.ts:132, editor/roadParking.ts:12:
  F-0002, F-0004, F-0016, F-0018). Here: the error channel above.
- Diagnostics running in the shipped game: the profiler on from the start capturing stack traces (core/profiler.ts:75, F-0010), a
  Map a frame for the frame timer (core/health.ts:153, F-0008).
- Quadratic work: the offset's self-intersection pruning (core/offset.ts:129, F-0009), the network's change history
  (world/networkDelta.ts:101-110), marks re-merged per shape (render/roadMarks.ts:164), street numbers (world/roads/streetNames.ts:188).
- Copies and validation on opening: the whole document validated on the page (editor/persistence.ts:191, F-0017), every square's
  table copied twice (editor/squaresDecode.ts:193, F-0021).
- A mid-range RTX x060 started on the ultra tier (core/gpuTier.ts:9, F-0007): the median player's card.
- Geometry heavier than it shows: rounded boxes, closed cylinders whose caps never show, non-indexed cards, hidden faces drawn,
  32-bit indices under 65,536 vertices (docs-optimization-brief.md; reports/polygon-geometry-audit.md, its per-file list in the
  agent's extract below).

The agents' area reports add each area's own (sheets/<area>.md, "Reference faults never brought").

### From the reference's long audits (a reading agent, every item cited in its report; A = reports/audit-f7/AUDIT.md, G =
### reports/polygon-geometry-audit.md, L = lab-reports/auditoria-2026-10-19.md)

The rules they break, each a check of every ported feature: R1 whole-map work on an edit; R2 repeated work on unchanged inputs;
R3 allocation in a hot loop; R4 quadratic proximity; R5 geometry heavier than it shows (no detail level, detail under a pixel);
R6 hidden or overlapping faces drawn; R7 buffers never freed, memory tied to the area explored; R8 buffers wider than needed
(32-bit indices under 65,536 vertices, floats where quantised fit); R9 many small draws; R10 degenerate or empty work submitted;
R11 shadow casters finer than the shadow map's texel; R12 shaders compiled or uploaded on first view; R13 simulation cost, or the
simulation tied to the camera.

- Roads: straight lines sampled every metre (world/parkingLayout.ts:172-191, G:84); a roundabout disc as 18 overlapping rings
  (world/markings.ts:452-464, G:3703); one 8 mm arc tolerance at every distance (core/scalar.ts:39, G:75); overlapping helper rings
  (junction/polygon.ts:63-106, G:3640); one mesh per material per block, paint as polygons, ~320 of 448 draws (G:1563, G:2052);
  flat ground meshed as finely as hills, 6 M triangles (roadSurfacePass.ts:26, G:2038).
- Buildings: lot surfaces as instanced boxes, 4.44 ms and 2.30 M triangles (buildingMesh.ts:902-905, L:93, A:173); facade parts at
  full model however small, 6.13 ms, 37.5 % of the GPU (kit.ts:64-84, L:105); no detail level for facade details (L:95-96); rounded
  boxes for interior furniture, a bookshelf ~54,600 triangles (furnitureKit.ts:51, G:66); joined buildings kept whole, inner faces
  drawn and casting (world/buildings.ts:805-860, G:273); the ghost rebuilt on every pointer move (editor/buildingTool.ts:1806,
  G:248); landmarks unindexed, 20-40 draws each (signature.ts:377-1772, G:1283); wall batches widened to 32-bit (gpuBatchedMesh.ts:44,
  G:1230); far parts one draw per kind over the town, the whole tail re-uploaded on one removal (farParts.ts:76, G:1039); rubble
  kept forever (fracture.ts:68-417, G:1076); lot elements 1-10 M boxes with no level (G:3407).
- Terrain: skirts drawn on four sides in both windings (terrainLod.ts:43, G:2430); unindexed ground (terrainLod.ts:35, G:2418); far
  ground at the full grid on plains, 240,640 triangles where 1,568 serve (A:108, G:1501); water rebuilt whole when land moves
  (terrain.ts:2580, G:2463); fog and scatter brushes scanning every dab (fogPaint.ts:68-88, elements.ts:78-101).
- Traffic: vehicle levels never requested, 11-12 k triangles a car (render/agents.ts rung(), A:100); hubs under a pixel (A:107);
  unused body levels kept on the GPU (agents.ts:1443, A:218); hidden far cars still submitted, up to 6.29 M triangles
  (densityCars.ts, G:1460); signal heads with no level, closed cylinders (signals.ts:119-125, A:103); the simulation step ~10.5 ms
  holding the city at 44 % of its population (sim/clock.ts:20-47, populationGovernor.ts, L:103); per-frame agent decode
  (simClient.ts:509, L:108); the micro window following the view (meso/memory.ts:14-27, G:2785).
- People: shadows at full detail, 734 k of 1.1 M triangles (riggedCitizens.ts:212-248, A:94); buffers sized to the caps and never
  written (proceduralCrowd.ts:335-336, G:1740); closed skins double-sided (proceduralCrowd.ts:1231, G:1741); a 1.91 s stall on opening
  (L:104); garments undecimated, 155,930 triangles (G:3981); a new texture object per row write (gpuRows.ts:144, G:1771).
- Furniture, poles, vegetation: pole pieces and wires chosen per 1 km block (A:114); a pole behind the camera at the finest level
  (poleLayer.ts, A:111); wires as 64-triangle tubes, ~3 M triangles (utilities.ts:500-503, A:112); lamp cones drawn past their fade
  (scenery.ts:225-230, A:110); 46 empty shadow draws a frame (poleStores.ts, A:545); far blocks re-chosen four times a frame, 54.7 ms
  (propCells.ts:582, A:577); plants casting their full model, 4.24 M triangles in the shadow maps (cellInstances.ts, A:97); props
  without a far level, ~3.5 M triangles (streetFurnitureLayer.ts:96-98, G:2072); impostor mipmaps nothing reads (A:485); hidden caps
  (signs.ts:460, structureCells.ts:41, G:2105); unindexed leaf cards (G:1542); dead slots drawn (G:2117, G:1511, G:1485); railings and
  sleepers as dense casting boxes (barriers.ts:145, transit.ts:331); every debris matrix rewritten when one moves (blast.ts:670-706).
- Render in general: the whole explored area kept (renderer.ts:2601-2613, L:72: geometry 121.6 to 340.1 MB, textures 252.3 to
  539.8 MB); the whole document serialised on every autosave and undo (main.ts:798-812, L:102); draws over budget, 1,197
  (L:100); a sky dome sphere for a full-screen pass (environment.ts:147, G:1467); one cascade redraw 8.62 M triangles, wind casters
  redrawn every cascade every frame (wind.ts:103-125, A:144, G:2648); a whole-scene memory count every 60 frames (A:475); shader
  programs never released, 232 to 541, compile bursts mid-session (A:80, A:517); allocation churn 50-82 MB a minute at rest (A:75);
  the WebGPU path copying whole buffers on every edit (webgpuView.ts:131, G:1588). The GPU is bound by pixels more than by
  triangles (GTAO 1.23 ms; 534 k fewer triangles left the scene pass unchanged: L:98-99, A:455).

## This game's own performance faults known today

- The 24 km city opens in 69.8 s on the debug DLL, 23.6 s on the release, 473,839 buildings: `fill_all` walks every lot serially
  and builds four complete candidate bodies a lot to compare their looks (research.md R4, growth.cpp 110-198; task T014).
- The grade solver keeps four float lines a segment for good, about 390 MiB of the 24 km city (Proads-8).
- km24 on the median profile: GPU p95 up to 14.9 ms, 151 frames over 33 ms (doc/history/process.md, the mandatory pause).
- The scale probe (backbone item 8) measures the rest at each block's end: memory by system, CPU by section of the frame and by
  tick, GPU time, draws and triangles, on the 24 km map, in seconds.
