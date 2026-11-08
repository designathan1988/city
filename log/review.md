# Review record: findings, problems and process errors (started 2026-11-07)

The record of the full reading of the project asked by the user on 2026-11-07: what was read, what was found (with file and
line), what is wrong in the code, the tools and the documents, and every error of execution or order made during the work.
Newest entries at the end of each section. Every claim carries its evidence; a claim without evidence is marked as such.

## 0. Method (2026-11-07, second order of the user: a full audit, every line, by reading)

- Skills used: `superpowers:using-superpowers`, `superpowers:systematic-debugging` (each finding traced back to the design
  decision that produces it), `superpowers:verification-before-completion` (no claim without its evidence).
- Official guidance read (code.claude.com/docs/en/best-practices): the context window is about 1M tokens and is compacted
  on its own; "handing the same task to a subagent doesn't make the answer any better". So the reading is done in this
  session, and every finding is written here as soon as it is made, which survives compaction.
- Reader: a script in the session scratchpad (`chunks.py`) splits each area into chunks of at most 27 KB of whole,
  numbered lines; chunks 0..n-1 cover every line of every text file once. Counts: `native/src` 4 + 93 + 66 + 19 chunks,
  `project` 104, `tools` 147 (431 in all). Binary assets (`.png`, `.svg` icons, `.bin`, `.glb`) are not code and are not read.
- External facts are checked against the official source before they enter a finding. Done so far:
  libc++ `std::shared_mutex` is writer-preferring: `__shared_mutex_base::lock()` sets `__write_entered_` before waiting for
  the readers, and `lock_shared()`/`try_lock_shared()` refuse while it is set
  (github.com/llvm/llvm-project, `libcxx/src/shared_mutex.cpp`). The lock finding of §3a holds: with any writer waiting,
  the frame's `shared_lock` waits for every mesh job already holding the map.
  Godot `RenderingServer.force_sync` is bound to `RenderingServer::sync`, which flushes the command queue
  (`servers/rendering/rendering_server.cpp`, `rendering_server_default.cpp`, github.com/godotengine/godot). Godot 4.7 has no
  partial 2D texture update (`native/godot-cpp/gdextension/extension_api-4-7.json`: only `texture_2d_update`).
  godot-cpp's `PackedByteArray::operator[]` and `ptrw()` each call into the engine (`godot-cpp/src/variant/packed_arrays.cpp:50-64`).

## 1. What has been read, and what has not

Read in full, line by line, by the reviewing session itself (no agent):
- `CLAUDE.md`; `log/mapa.md`, `log/ferramentas.md`, `log/pendencias.md`, `log/work-log.md`, `log/conversor.md`,
  `log/causa-raiz.md`, `log/lab.md`, `log/base.md`, `log/arquitetura.md`; `.claude/REGISTRO-GOAL.md`.
- `tools/process/CLAUDE.md`, `tools/process/session-rules.md`, `tools/process/GODOT-PORT-GUIDE.md`.
- `tools/converter/README.md`, `DESIGN.md`, `GOAL-LOG.md` (the `tools/converter-slice` copies differ only in the
  "Child processes" section of `README.md`, which was read in the diff).
- `tools/tasks/*.py` and `tools/tasks/suites.json` (the whole orchestrator, 2,759 lines).
- `tools/gates/run_gates.py`, `registry.py`, `context.py`, `scope.py`, `source.py`, `facts.py`, `tokens.py`,
  `gate_files.py`, `gate_code.py`, `gate_clones.py`, `gate_docs.py`, `gate_language.py`, `gate_defs.py`,
  `gate_forbidden.py`; `rules.json` by key.
- `tools/build/build.sh`, `quick.sh`, `free_dll.sh`, `pgo.sh`, `photodiff.py`, `check_scripts.gd`.
- `native/src/core/*` except the translated runtime `js_*.h` (sizes only); `native/src/sim/**` in full (93 chunks,
  every file); `native/src/render/**` in full (66 chunks, every file); `native/src/api/**` in full (19 chunks, every file).
- `project/{game,ui,render,data,tests}` in full (104 chunks, every file); `project/assets` in full (15 chunks: the 38 lot
  documents and `people/LICENSES.json`; the 54 `.import` files grouped by parameter set; the 22 `.gltf` headers);
  `project/project.godot`, `export_presets.cfg`, `override.cfg`, `bin/citycore.gdextension`, `.gitignore`.
- `project/addons/terrain_3d`: `plugin.cfg`, `terrain.gdextension`, `README.md`, `src/export_plugin.gd`, the file list
  and its git history. Its other 45 text files (334 KB of the upstream editor plugin) were not read line by line: the
  plugin is not enabled in `project.godot` and no project file refers to them, so none of them runs (§3v).
- The memory files of this project (`MEMORY.md`, `fix-cause-effort-neutral.md`, `no-reference-game.md`).

- `tools/gates/*.py` in full (the other modules: `cpp_syntax`, `gate_data`, `gate_python`, `gate_text`, `gate_tuning`,
  `gate_ui`, `gd_syntax`, `lex_c`, `lex_gd`, `lex_other`, `selftest`), `owners.json` and `rules.json` in full; the 46
  schemas by census (size, strictness, required keys, dead keys required) and the two orphans in full; the 60 fixture cases
  listed against the rule ids. `tools/build/rename_english.py`, `tools/tasks/tests/*` and `tools/assets/**` in full;
  `tools/analysis/**` in full (20 chunks). The full gate run on the tree as found (`run_gates.py`, exit 1, 552 problems).
- `tools/process`: `settings.json`, `bin/*`, `READ-ME.txt`, `.gitignore`, `extract_feature.cjs`,
  `extract_feature_cpp.cjs`, the head of `process.py`, a census of the other 16 code files (language, source-game ties,
  processes without deadline) and the classification of its 8,489 tracked files.

Classified and traced to the game, not read line by line (§3w T1 gives the reason: their purpose is porting the source
game's code, which the user's rule forbids, so the fix is their removal whatever their internal defects):
- `tools/converter` (`src` 1.2 MB, `scripts`, `test`: 82 chunks; `README.md`, `DESIGN.md`, `GOAL-LOG.md` and
  `configs/roadcraft-machine.json` read), `tools/converter-slice` (71 chunks, a worktree of the converter), `tools/ports`
  (182 chunks of port records), `tools/process/records/**`, `tools/process/features-4.0`, `tools/process/converter-old/out`,
  `tools/oracle-4.0` (4,698 tracked files, 4.1 GB: the source game itself).
- The working tree as read holds other sessions' uncommitted work (231 files of `native/src` changed, `tools/analysis` and
  `tools/process` rewritten): line numbers in this record are those of the files when they were read on 2026-11-07.

## 2. Contamination by the source game's code

The user's order: the source game (the TypeScript game, folder `tools/oracle-4.0`) gives the look and the features to
reach, never the code. The records show its code was carried into the project by design, by rule and by tool:

- The base architecture was written from another TypeScript game's code: `tools/process/GODOT-PORT-GUIDE.md:82`
  ("Porte o desenho de `N/src/shared/contracts/poses.ts`": the `PoseBuffer`), `:211` (`JobPool` from `jobs/pool.ts`),
  `:326` (the incremental lane graph from `sim/lanes.ts`), `:372` ("porte linha a linha" of `meso.ts`), `:385` (the micro
  window from `traffic.ts`), `:542-566` (block E: "leia o(s) .ts de origem inteiro(s), porte a lógica pura para C++").
- A written rule allowed copying: `log/ferramentas.md:158` ("Exceção ... a lógica pura de simulação entra traduzida, com a
  estrutura do 4.0, sem edição à mão"), still alive in `CLAUDE.md:163-165` (the converter "translates its pure simulation
  logic into C++"; "Translated code ... is regenerated, never fixed by hand").
- A tool exists to do it: `tools/converter` (TypeScript to C++ translator, its runtime of ECMAScript built-ins copied into
  `native/src/core/js_*.h`, `log/conversor.md:100-117`). Translated, never hand-written: `native/src/sim/roads/flow_stats*`,
  `control_advisor*` (`log/mapa.md:152`), included by `native/src/sim/traffic/flow_control.h`.
- The runtime was put in `native/src/core` because that folder is exempt from the magic-number gate
  (`log/conversor.md:130`, `tools/gates/rules.json:361`): a gate dodged by placement.
- Features ported from its source files, by the records: `log/base.md:99` (`lot_place.cpp` from `world/lots.ts`
  `streetPlan`, "lido inteiro ... Portado"), `:112` (`CameraAim.bring`, "cópia do `keeping`"), `:114` (building fracture
  "portada do `fracture.ts`/`destruction.ts`"), `:104` (the 38 landmarks baked by running the source game's own
  `buildSignature` in a browser), `:90` (fright reactions with "os rolos e segundos" of the source), `:91` (face
  expressions from `faceExpression.ts`, "lido inteiro"), `:73` (standing acts, the source's `standingKey`), `:21-25`
  (people, clips baked by the source's own `bake`).
- Decisions justified by the source's code even in the 2026-11-07 audit: `log/causa-raiz.md` rows 19, 20, 34, 52, 58, 60
  ("behaviour change for 4.0 parity", "kept ... as in 4.0", "charging unchanged, because the 4.0 does not charge that way").
- Size of it in the code (measured 2026-11-07 with grep): 200 of the 653 files of `native/src` name the source game or one
  of its `.ts` files; 139 files of `project/` (`.gd`, shaders, `.json`) do too; 174 files of `native/src` use the
  namespace `b40` (the building generator named after the source).
- The source game is committed inside this repository: `tools/oracle-4.0`, 4,698 tracked files, 4.1 GB on disk.
- Documents that still order porting from it: `tools/process/CLAUDE.md:7-8`, `tools/process/session-rules.md` §2, §9, §11
  ("paridade com o 4.0"), the skills `roadcraft-port` and `roadcraft-reading` in `tools/process/.claude/skills`.

## 3. Problems in the tools

- **Every test scene reruns on any change in `project/`**: the template `godot_test` keys each scene by `@project`
  (`tools/tasks/suites.json:162`, set at `:35`), so one changed GDScript or JSON invalidates all 36 scenes. The claim
  "only the tasks whose inputs changed" (`CLAUDE.md:101`) holds only literally: no scene declares what it actually loads.
  This is the cause of "the whole suite every time", not a timeout or a thread count.
- **`gates_changed` is keyed by the whole tree** (`suites.json:79`, `"inputs": ["**"]`): any change anywhere reruns it.
- **The gate diff reads the whole worktree**: `tools/gates/scope.py:70` runs `git diff HEAD --unified=0` with no path
  list, even for `--file X`. With the 3,007 uncommitted deletions of `tools/oracle-4.0` this diff walks every deleted
  blob on each gate run.
- **The orchestrator re-reads git after every task**: `tools/tasks/pool.py:249-250` drops the worktree snapshot whenever
  a task finishes (`wrote=True` for every executed task), and the next key runs `git ls-files -s` plus
  `git status --untracked-files=all` again (`tools/tasks/inputs.py:90-98`). A task's declared outputs are known
  (`Task.outputs`); invalidating everything instead of those paths makes keying cost grow with the number of tasks.
- **Small quadratic steps in the scheduler**: `pool.py:88-97` recomputes every task's users on each `submit`
  (O(n²) per submit) and `pool.py:125` sorts with `self.order.index(i)` (O(n) per comparison). Harmless at 40 tasks; a
  wrong method all the same.
- **A golden-file verdict depends on the golden being an input**: `digest` lists `dist/quick/golden.txt` among its inputs
  (`suites.json:93`) and `photos_check` lists `dist/quick/golden_photos` (`:142`), so a re-bless invalidates them. Correct;
  noted because the rule is implicit (no check enforces that a `matches_golden` task names its golden).
- **The product zip ships the source game**: `tools/build/build.sh:67` packs `git archive HEAD` into `fonte/`, and HEAD
  still tracks the 4,698 files of `tools/oracle-4.0` (their deletion is uncommitted).
- **`pgo.sh` names its own Godot path** (`tools/build/pgo.sh:8`, not overridable), beside the machine vars of
  `tools/tasks/suites.json:3-28`: two owners of where Godot is.
- **The magic-number gate is satisfied by naming**: `tools/gates/gate_defs.py:92-99` accepts any literal inside a
  named-constant declaration, so `constexpr float K = 0.37f;` passes with no source; `native/src/core/` and all of
  `project/render/` (the shaders) are outside the rule (`rules.json` `magic_numbers.paths`/`exempt`).
- **"Render reads only" is checked only by include path**: `RENDER_SO_LEITURA` (`tools/gates/gate_forbidden.py:169`)
  forbids `#include "sim/commands/..."` in `render/`; a write to `CityState` through a non-const reference passes.
- **The line limits push splitting by size**: `rules.json` `max_lines` and `max_function_lines` are the gate behind
  "Sem arquivo-deus", whose own text says "Divide-se por responsabilidade, nunca para caber num limite" (`CLAUDE.md:74-75`);
  the records show files split to pass it (`log/work-log.md:467`, `log/base.md:93`, `:102`).

## 3a. Problems in the game code (read so far: `native/src/core`)

- **The procedural world is built to reproduce the source game bit for bit**: `native/src/core/seeded.h:1-3` ("Seeded
  generators that give Roadcraft's numbers bit for bit (its src/core/rng.ts): the same seed makes the same city here and in
  the TypeScript game"), with JavaScript integer semantics (`js_int`, `:24-25`), its `Mulberry32`, its `Xoshiro128`
  ("the building stream", `:42`), its `noise2`, `valueNoise` and `natureNoise.ts` (`:70-134`). Generation was designed for
  parity with the source's code, not from this game's needs.
- **Two owners of the same hash**: `hash_u32` (`native/src/core/random.h:7-13`) and `hash32` (`seeded.h:12-17`) are the
  same lowbias32 function; two RNGs (`Rng` xorshift in `random.h:16-27`, the two in `seeded.h`) and two value noises
  (`vnoise`/`fbm` against `value_noise`/`fbm2`). The clone gate misses them (each is under its 80-token window).
- **The spatial hash is not flat**: `native/src/core/grid.h:35-91` says "flat spatial hash ... more cache/thread friendly
  than a pointer quadtree", but it is `std::unordered_map<int64_t, std::vector<Entry>>`: a node per cell plus a heap
  vector per cell, two pointer hops per lookup, and `remove_aabb` erases by linear scan of each cell (`:49-57`).
- **`std::floor` where the project's own `ifloor` exists**: `math.h:5-9` says `std::floor` is a library call on the
  baseline and adds `ifloor`, yet `grid.h:25`, `:31-32`, `:83` and `random.h:33` keep `std::floor` in per-cell paths.
- **Job priorities go stale**: `native/src/core/job_pool.cpp:126-143` fixes a job's priority (the camera distance) when it
  is submitted and the heap never reorders; when the camera moves, the queue keeps serving the order of where it was.
  One mutex guards submit, take and every `current()` poll of running jobs (`:146-150`), and every submit allocates a
  `std::function`. The pool's design was taken from the other TypeScript game's `jobs/pool.ts`
  (`tools/process/GODOT-PORT-GUIDE.md:211-226`).
- **Nested `parallel_for` runs serially**: `job_pool.cpp:159` falls back to a plain loop on any pool thread (`in_pool`),
  so a job that calls `parallel_for` gets one core.
- **`SlotIds` sits outside `namespace city`** (`native/src/core/slot_ids.h:10`), unlike every other core type.
  Fix: wrap it in `namespace city { }`.
- **A translated `throw` kills the game**: `js::fail` (`native/src/core/js_runtime.h:23-27`) prints and calls
  `std::abort()`, so any error path of the translated code takes Godot down with it. Fix: the game owns its errors;
  translated code is removed (§2), and refusals go back as results (`EditResult`) as the hand-written commands do.
- **`js::Value` breaks the semantics it claims**: `Value::length()` gives a string's bytes (`js_value.h:162`) where
  `js::length` gives UTF-16 units (`js_string.h:69`); `Value::text()` prints a non-integer with `std::to_string`, six fixed
  decimals (`js_value.h:132`), where ECMAScript prints the shortest digits (`js_number.h:133`). Two owners of "a number as
  text" inside the runtime itself.
- **JavaScript containers in a C++ game**: `js::Map` keeps `std::vector<std::optional<pair>>` plus an
  `std::unordered_map` index (`js_map.h:103-104`) to reproduce insertion order; `js::shift` erases the front of a vector,
  O(n) per call (`js_array.h:52-56`). They exist only because the translated code needs JavaScript's behaviour.
- **Plans cannot hold holes, so the boolean cuts them and gives up after four cuts**: `native/src/core/polygon_ops.cpp:24`
  (`MOST_SPLITS = 4`: "past that a hole stays filled"), `:85`. A fifth level of hole is filled in silently: a wrong plan,
  not a refusal. Cause: the plan type (`PlanRing`, `polygon_ops.h:10`) has no holes; fix the type (outer ring plus
  holes) or refuse the edit, never fill.
- **Whole core is outside the magic-number gate**, and it shows: `segments_cross` excludes the ends by a parameter margin
  0.02/0.98 (`math.h:80`), `line_line`/`segments_cross` use 1e-7 and 1e-6 (`math.h:65`, `:76`), unnamed.

Read next: `native/src/sim/state/*`, `sim/simulation/simulation.{h,cpp}`, `render/render_world.{h,cpp}`,
`api/city_core.cpp:155-202` (the frame).

- **One reader-writer lock for the whole city is shared by the frame, the simulation, the commands and the mesh jobs**:
  `CityState::map_mutex` (`native/src/sim/state/city_state.h:115-120`). The main thread takes it shared twice a frame
  (`api/city_core.cpp:194` through `render/render_world.cpp:238`, and `:197`); the simulation takes it exclusive on every
  step while growth has cells (`sim/simulation/simulation.cpp:126-129`) and for flow control (`:120-124`); commands take it
  exclusive; road, prop, tree, cover and zone jobs hold it shared while they build (`log/pendencias.md:48`, audit item 06).
  If the standard library's `shared_mutex` gives a waiting writer priority over new readers, every exclusive request makes
  the next frame wait for the slowest mesh job in flight: frame time tied to job length. Confirmed in the libc++ source
  (§0): a waiting writer blocks every new reader.
- **The simulation's only escape from that lock is to skip work**: `damage_step` runs only when `try_to_lock` wins and
  carries the missed time as debt (`simulation.cpp:130-136`); the clock drops ticks when it falls more than two behind
  (`:164-166`), so under load the city slows instead of the work being spread.
- **All traffic runs on one thread**: `traffic::step` (vehicles, pedestrians, routing, yards) is one call per step on the
  simulation thread (`simulation.cpp:114-116`); FAST and FASTEST multiply the steps on that one thread (`:99-107`);
  routes are searched synchronously in the tick (`log/pendencias.md:41`).
- **`CityState` includes the commands layer**: `city_state.h:12` includes `sim/commands/edit_journal.h` so the state can
  hold `EditHistory` (`:73`); the source of truth depends on the layer that mutates it.
- **A comment in the source of truth cites the source game**: `city_state.h:74` ("Roadcraft 4.0 roadHeightOffset").
- **`RenderWorld` is a façade of eighteen renderers and three fields** (`render/render_world.h:168-209`), with some fifty
  pass-through methods (`:69-166`); `tools/analysis/arch` already lists it among its three `god` roots
  (`log/arquitetura.md:448`).
- **The frame's main-thread work is all inside the shared lock**: drain, effects, Terrain3D uploads of edited rectangles,
  every stream offer, the building level choice, transit (`render_world.cpp:236-272`), so the frame cannot proceed on its
  own data while a writer waits.

## 3b. `native/src/sim/buildings/b40` (the building generator)

- **The folder is the source game's generator, renamed**: `b40` stands for its building stream; `blocks.h:1` ("the 4.0's
  world/buildings/blocks.ts"), `signature_kinds.h:1` (its `towerKit.ts TOWER_KINDS`), `types.h:89-100` (its `flags.ts`,
  `buildingRoofs.ts`). Fix: rename the folder and namespace to what they are (`sim/buildings/body`, `city::body`) and drop
  every citation of the source when each file is rewritten to this game's own design.
- **A random draw thrown away to keep the source's stream**: `massing_tall.cpp:162` `r.irange(0, SHOP_FUNCTIONS - 1); //
  the function name ...: drawn for the stream, not drawn differently`. The whole generator is ordered to reproduce the
  source's numbers seed for seed (`gen.h:1-2`, `paint.h:2`: "each a fixed number of draws"). Fix: remove the dead draw;
  the generator's order of draws is this game's, and the saved bodies are keyed by `body_key`, not by the stream.
- **Palette index off by one, hidden by a modulo**: `Xoshiro128::irange(lo, hi)` includes `hi` (`core/seeded.h:42`,
  `:66`), and every massing does `b.palette = r.irange(0, PALETTES)` with `PALETTES = 7` (`massing_house.cpp:102`,
  `massing_tall.cpp:141`, `:166`, `:211`, `massing_works.cpp:21`): eight values for seven palettes. The only reader
  wraps it (`lot_boundary.cpp:78` `% s.data.lot_walls.size()`), so palette 7 silently equals palette 0. Fix:
  `irange(0, PALETTES - 1)` and index without the modulo.
- **Hundreds of tunable design values in code, not in data**: every massing and lot file opens with a block of
  `constexpr float` (`civic.cpp:17-29`, `civic_services.cpp:12-27`, `facade_compose.cpp:12-20`, `lot_back.cpp:15-27`,
  `lot_boundary.cpp:13-19`, `lot_cars.cpp:14-22`, `lot_front.cpp:14-25`, `lot_plan.cpp:8-21`, `lot_yard.cpp:16-31`,
  `massing_core.cpp:13-20`, `massing_house.cpp:11-22`, `massing_tall.cpp:12-42`, `massing_works.cpp:10-13`,
  `open_lots.cpp:17-25`). They are storey heights, lot zone depths, shares and counts: exactly the "valores ajustáveis"
  CLAUDE.md puts in `project/data/**` with a schema. They pass the gate only because a named constant is accepted.
  Fix: move them to `data/render/buildings.json` (sections per kind), read by `buildings_loader.cpp` into `Data`.
- **A second owner of the zone ids**: `massing_core.cpp:17-18` (`ZONE_RESIDENTIAL = 1`, `ZONE_COM_LOW = 7`, ...) restates
  the zone table of `data/catalog/zones.json`; a new zone in data maps to `KIND_HOUSE` by the `default:` (`:32`). Fix:
  the zone table carries its building kind, read through the catalogue.
- **A second polygon engine**: `plan_area.cpp` clips, subtracts and triangulates plans in `float` (ear clipping O(n³),
  Sutherland-Hodgman) beside the exact integer one in `core/polygon_ops.cpp` (Clipper2). `blocks.cpp` uses both in one
  function (`touch` calls `overlap_area`, `range` calls `plan_intersection`/`plan_difference`). Fix: one owner,
  `core/polygon_ops` (add area, overlap and point-in-plan there), and delete the float clipper.
- **The body's per-bay data are three hash maps per volume**: `types.h:120-122` (`bays`, `side_fill`, `storey_paint` as
  `std::unordered_map<uint32_t, ...>`), read once per bay by `bay_at`/`fill_at` (`:159-166`) in every facade emitter, and
  `FaceGeometry faces[MAX_FACES]` (`:123`) is 24 entries in every volume, box or not. Every `record_body` of an edited
  record copies all of it (`record.cpp:28-30`). The plan to make these flat arrays was written and never done
  (`tools/process/GODOT-PORT-GUIDE.md:243-244`). Fix: per volume, a dense array indexed by (storey, side, bay) offsets
  (sides and bay counts are known after composition), and `faces` sized by `sides()`.
- **Named colours looked up by string, per element, per body**: `Data::color(const char *)` builds a `std::string` for
  each `find` (`data.h:84-88`); `LotSpace::paint` scans its list with `strcmp` before that (`lot_space.cpp:61-67`); every
  element of every lot of every grown body goes through it. Fix: resolve the colour names to indices once, in the loader
  (an enum of the names the generator uses, checked by the loader as it already checks presence).
- **`Body::add_paint` never deduplicates** (`types.h:225-228`): `civic.cpp:91-92` adds the same gold twice; paints grow with
  every element that asks. Fix: return the index of an equal paint already present.

## 3c. `sim/buildings` (records, interiors, lot access), `sim/catalog`, `sim/citygen`

- **The zone codes have six owners, two of them wrong**: `data/catalog/zones.json` says code 7 is `com_low` and 8 is
  `com_high`, and `Catalog::zones` (`sim/catalog/catalog.h:32-39`) carries each code's `base` and `density`. Yet the codes
  are typed again in `api/models_loader.cpp:28`, `sim/buildings/b40/massing_core.cpp:17-18`,
  `sim/buildings/building_store.cpp:12`, `sim/citygen/road_gen.h:16`, `sim/simulation/growth.cpp:22`,
  `sim/traffic/parking_sources.cpp:21`; `building_store.cpp:12` and `growth.cpp:22` call 7 and 8 `ZONE_IND_LOW`/`ZONE_IND_HIGH`
  (industrial), while `BuildingStore::occupancy` (`building_store.cpp:116`) still maps them to base 2 (commercial): the
  arithmetic is right by accident, the name is wrong. Fix: delete every copy; read `catalog.zone(code).base` and
  `.density` (add the building kind to `zones.json` for the generator).
- **Parking rebuilds whole building bodies to find car gates**: `street_gates` (`sim/buildings/lot_access.cpp:217-240`)
  runs `record_mass` (the full generator plus the block resolve) for every building within reach of a segment, each time a
  segment's bays are laid out, only to read its `EL_CAR_GATE` elements. Fix: the gates of a body are derived once per
  `body_key` (beside `BuildingStore::looks`) and read from there; `lot_access` reads the same cache.
- **Lot growth draws four whole bodies per lot**: `record_for` keeps the least alike of `LOOK_CANDIDATES = 4`
  (`sim/buildings/likeness.h:28`), each a full `make_body`; the rule and its weights are the source's (`likeness.cpp:1`
  "The weights are the 4.0's", `likeness.h:1-2`). Fix: choose by the look's inputs (style, palette, roof kind, storeys)
  before making one body, and make only the chosen one.
- **Generation is designed for parity with the source's code**: `sim/citygen/city_plan.cpp:1` ("The draws keep their order,
  so a seed lands its centres where the TypeScript game's does"), `:32` (a draw thrown away "for the stream"),
  `lot_place.cpp:17-25` (`lots.ts` constants and a JavaScript `Math.round`), `block_lots.cpp:1` ("as Roadcraft 4.0 cuts it
  (blockLots.ts)"), `city_gen.cpp:1`, `:24`, `lot_gen.h:1-3`, `lot_land.h:1`. Fix: remove the stream-keeping draws and
  the JavaScript rounding; justify each layout number by this game (data, schema).
- **Profile ids hard-coded by name in the generator**: `city_gen.cpp:22-39` names fourteen profiles of
  `road_profiles.json` in code (two pairs repeat: `collector`/`urban` are both `cityUrban`, `avenue`/`av4` both
  `cityAvenue`); `Catalog::profile_id` returns -1 for a renamed one, and the generator indexes `profiles[g.profile[s]]`
  without a check (`block_lots.cpp:242`, `lot_gen.cpp:41`). Fix: the generator's road roles go in `growth.json` (role ->
  profile id), checked by the loader like every other key.
- **The ground of every lot sample through `std::function`**: `LotGround` (`lot_gen.h:58-61`) is called for each 4 m
  point of each lot (`lot_gen.cpp:128-140`). Fix: pass the `HeightPatch` (the one bilinear sampler,
  `sim/terrain/height_patch.h`) by reference.
- **The player's lot snap scans every lot on every pointer move**: `lot_corner_near` (`lot_land.cpp:223-239`) walks all
  corners of all `LotBook` lots. Fix: a spatial index of lot corners kept by the `LotBook` edits.
- **Two copies of pi and of a turn**: `plan.h:14` (`PI_F` again, in `city::b40`), `interior/cut.cpp:10`
  (`TURN = 6.2831853f`), `city_plan.cpp:15` (`TURN`), beside `core/math.h:9-11`. Fix: use `core/math.h`.
- **Regional facts fixed in code**: biomes and plant formations are Brazilian enums (`sim/catalog/nature_types.h:73-77`),
  prices are "R$ per m2" (`profile_types.h:73`), room sizes "common Brazilian dwelling practice"
  (`interior_homes.cpp:3`). Fix: biomes, formations and currency as data rows of a regional style.
- **Every tunable is a float**: `CITY_TUNING_DECLARE` (`tuning_fields.h:47`) makes counts float too
  (`history_max_entries`, `road_max_degree`, `render_max_chunk_rebuilds_per_frame`), each read back through `int(...)`.
  Fix: a type per field in the X-macro (`X(group, name, type)`).
- **One flat struct of about 500 traffic numbers**: `CITY_TRAFFIC_FIELDS` (`traffic_params.h:11-63`) holds cars, walkers,
  buses, trains, lorries, kerbs and fright in one struct passed everywhere. Fix: one struct per owner (vehicles,
  pedestrians, transit, yard), each read by its loader section.
- **Interior layout numbers in code**: about 150 named constants in `interior_homes.cpp:13-32` and
  `interior_rooms.cpp:10-39` (furniture offsets), the same data-rule breach as §3b.

## 3d. `sim/citygen` (squares, road graph) and `sim/commands` (the Build edits)

- **Root cause shared by many findings: the sim keeps no derived body.** A building record holds only its inputs; every
  consumer that needs walls, gates or blocks calls the whole generator again: `pick_body` (`commands/body_pick.cpp:117-118`,
  `record_body` per record near the ray, on every pointer move of the Build tool), `street_gates` (§3c),
  `record_for` (§3c), `pieces_of` (`commands/body_furnish.cpp:30-39`, a whole `resolve_blocks` plus `Interior` the first
  time a floor is touched). Fix: one owner, a `BodyCache` in `sim/buildings` keyed by `body_key` (inputs + edit
  revision), filled once by the generator and invalidated by the `ChangeLog` building marks; picking, parking gates,
  likeness and furnishing read it. The render's building job then reads the same cache instead of generating again.
- **Generated squares are stored as player edits**: `set_down` (`citygen/squares.cpp:156`) puts each square's body in
  `designs.edited`, so every generated square is written into every save (`persistence/body_codec.cpp:332`) and counts as
  a player's design. Fix: a square is a generator kind (`FN_SQUARE` made by `b40/civic`), its record keeps only inputs.
- **Squares have their own LCG beside `core/random.h`**: `squares.cpp:26-27,142-143,197` (`LCG_MUL = 1664525`,
  `LCG_ADD = 1013904223`) while it includes `core/random.h` and never uses `Rng`; the tree grid starts at a constant kept
  "so it lies the same" as the source (`squares.cpp:139`). Fix: `Rng` seeded by `hash_combine(seed, index)`.
- **The road generator keeps JavaScript `Map` order for parity**: `citygen/road_gen_graph.cpp:28-29` (`EdgeMap`,
  "JavaScript Map semantics: the order decides node and segment numbering"), `road_gen_tidy.cpp:49`, `road_gen.h:4`
  ("the same seed gives the TypeScript game's graph"). An insertion-ordered map is a vector plus a hash map, rebuilt three
  times per tidy pass (`road_gen_graph.cpp:189,204,238`). Fix: number nodes and segments by a sort on position after the
  graph is built; one flat edge vector.
- **The Build tool's handle edit dispatches on strings**: `op_handle` (`commands/body_blocks.cpp:141-160`) compares
  `e.kind` with nine literals per drag tick. Fix: an `enum class HandleKind` set once where the API parses the request.
- **Removing a block points the entrance at the wrong block**: `op_remove_volume` (`commands/body_mass.cpp:279-302`)
  erases index `vol` and, in the cascade, more blocks, but only resets `entrance_volume` when it is past the end
  (`:301`). When a block before the entrance goes, the entrance moves to the next block; when the entrance block itself
  goes, the door lands on whatever slid into its index. Same in `body_blocks.cpp:247`. Fix: keep `entrance_volume` as a
  block id, or remap it through the erased indices (decrement for each erased index below it; 0 when it was erased).
- **A query that mutates the body**: `piece_at` (`commands/body_furnish.cpp:75`) takes `Body &` and, through
  `pieces_of` (`:30-39`), writes a whole generated furnishing into `b.furnishing` the first time a floor is hovered, so
  hovering turns a generated floor into an edited one. Fix: hover reads the derived furnishing (the cache above) without
  storing it; only an edit stores a floor.
- **Two owners of paint de-duplication, none in `Body`**: `paint_of` (`commands/body_finish.cpp:23-26`) and `PaintMap`
  (`commands/body_weld.cpp:26-36`) each look for an equal paint before `add_paint`, while `body_survey.cpp:399,408` and
  `model_body.cpp:119` add duplicates. Fix: `Body::add_paint` (`b40/types.h:225-228`) returns the existing index of an
  equal `{finish, rgb}`; remove `paint_of`.
- **A second convex hull**: `body_survey.cpp:55` (`convex_hull`) beside `citygen/block_lots.cpp:20`. Fix: one hull in
  `core/polygon_ops`.
- **Survey numbers in code**: `body_survey.cpp:19-26` holds about 60 tunables (storey heights 4.5/3.3 m, wall colours,
  sample grids); the data rule of CLAUDE.md applies (move to `buildings.json`).
- **The source game cited in 39 of 70 command files** (83 lines, grep `4\.0|Roadcraft|\.ts`), e.g. `commands.h:1,24,39,61`,
  `body_survey.h:1` ("the 4.0's editor/fromReference.ts"), `demolish_box.cpp:1`. Fix: describe what each command does in
  this game; remove the source paths.

## 3e. `native/src/sim`, read in full (chunks 29-93): root causes shared by many findings

Each root cause below produces several symptoms listed after it; fixing the cause removes all of them.

- **R1. The sim keeps no derived body** (§3d). More consumers that run the whole generator again: `interior_floors` and
  `interior_focus` (`commands/interior_commands.cpp:17-32`, per request of the interior view), `edit_building`
  (`commands/land_commands.cpp:187-188`), `place_body` for every landed building (`commands/model_place.cpp:113-115`),
  `body_floors` only to count storeys (`simulation/growth.cpp:45-51`), `read_bay_way` with up to three `lot_access`
  searches (`traffic/parking_lanes.cpp:88-121`), `read_lot` and `read_kerb` as the camera moves
  (`traffic/parking_sources.cpp:62-70,93`), `file_door` (`traffic/ped_doors.cpp:56-58`), the yard (`traffic/yard_input.cpp:107-108`).
  Fix: the `BodyCache` of §3d.
- **R2. `BuildingStore` files each building as a point.** `building_store.cpp:80` inserts `(b.x, b.z, b.x, b.z)` and
  `query` keeps the buildings whose centre lies in the box (`building_store.h:71-76`). Every caller that needs footprints
  touching an area pads the box by a guessed largest lot: `strike_buildings.cpp:17` `LOT_REACH_M = 80`,
  `zone_commands.cpp:16` `FRONT_REACH_M = 80`, `land_commands.cpp:22` `FACE_REACH_M = 80` (and `model_place.cpp:22` says
  40 for the same rule), `growth.cpp:77-90` (a second query only for squares). A wrong answer follows where the pad is
  small: `transit_built` (`transit/transit_ground.cpp:65-69`) queries a 1 m box, so it answers "a building's centre lies
  within 1 m"; `transit_commands.cpp:60-65` uses it to keep metro entrances off buildings, so an entrance can open inside
  any building more than 1 m from its centre. Fix: file the footprint's bounding box of the rotated rectangle, return
  overlaps, and test points with `BuildingStore::covers`; every reach constant above goes away.
- **R3. Street furniture is derived again by every reader.** `traffic::street_furniture_of`
  (`traffic/ped_furniture.cpp:80-95`) runs `segment_furniture` (`street/street_furniture.cpp:311-341`, with O(n²) slot
  loops), `merge_street` (`street/street_spots.cpp:77-115`: `footway_at` over every segment in reach for every player
  item, because a `StreetItem` keeps only x, z) and the linear `is_gone` filter (`demolition/damage_state.cpp:73-77`) on
  every call. Callers: `render/props/street_props.cpp:130,181,271` (the same segment three times per chunk build),
  `street_snap.cpp:45` (every pointer move), `strike_ground.cpp:41`, `street_commands.cpp:171`, `ped_furniture.cpp:29`,
  four probes; the walkers keep a private cache (`TrafficState::furniture`, `ped_crowd.cpp:150,259`) nobody else reads.
  The owner is also wrong: placement lives in `city::traffic`. Fix: one per-segment furniture cache in `sim/street`,
  invalidated by the `ChangeLog` furniture marks; items store their footway place (seg, side, v, u) when placed.
- **R4. Lots are derived per 256 m cell from a 576 m window, inside the commands, under the exclusive lock.**
  `lots/lot_cache.cpp:42-72` rebuilds every block once per cell it overlaps (about 5 windows of work per cell);
  `finish_command` runs `lots.process_dirty` unconditionally (`commands/road_edit.cpp:133-163`, `:157`), so a held relief
  brush (`terrain_dab` → `road_edit.cpp:98`), each zone-brush dab (`set_zone` → `mark_segment`,
  `roads/road_network.cpp:197-202,446-455`) and a parking change on the traffic thread (`forget_parking_lot`,
  `traffic/parking_sources.cpp:290-299`) rebuild lots while the frame and the sim wait. `LotIndex` rebuilds every player
  lot's stand each time (`lot_cache.cpp:88`); `LotBook::find/at` and five lot commands scan every lot
  (`lot_book.cpp:72-84`, `lot_commands.cpp:51-55,104-117,134-146,155-169`); `lot_book.cpp:122-145` ear clipping is O(n³)
  with its own turn test. Three constants for one reach: `lot_cache.cpp:15` 160, `lot_gen.h:20` 120,
  `road_network.cpp:16` 160 (its comment names a `LOT_MARGIN` that does not exist). Fix: lots derived per block (face of
  the road graph) keyed by its bounding segments, recomputed in jobs outside the exclusive lock, stands kept in the book,
  a spatial index in the `LotBook`.
- **R5. Damage is keyed by place, not by building id, and part of it lives outside the journal.** `hurt_of`
  (`damage_state.cpp:67-71`) matches `(seed, x, z)` by float equality; a building moved with the same seed
  (`model_place.cpp:136-140`) loses its hurt and scars and the old record outlives it. The reason given
  (`damage_state.h:43`, "ids move on undo") contradicts `edit_journal.h:3` ("restore the same ids"). Fires change outside
  the journal (`strike_buildings.cpp:80-81,92-97`): undo restores the building but keeps its fire; the growth hold too
  (`strike_commands.cpp:60`). Knocked-out furniture is matched by position within 0.25 m (`ped_furniture.cpp:67,87-94`):
  re-spaced kits stand again or hide another piece; its kind mapping has two owners (`ped_furniture.cpp:57-66`,
  `strike_ground.cpp:23-32`). Fix: damage keyed by building id and by furniture slot, fires and holds as journalled ops,
  one kind mapping beside `GoneKind`.
- **R6. The edit journal mixes data and opaque closures.** Every nature edit (`commands/nature_commands.cpp` edit_clouds
  `:43-56`, record_trees `:67-75`, cut_trees_at `:94-102`, set_weather `:110`, fog `:171,:184`, clear_trees `:262`,
  set_biome `:281`), water (`land_commands.cpp:39-53,83-106`, `unflood` marks every water chunk `:100`), elements
  (`element_commands.cpp:43-48`), scars (`journal_damage.cpp:70-76`) and the strike counter (`strike_commands.cpp:46`) undo
  through a `std::function` (`edit_journal.h:39,70-73`) that cannot be reduced, compared or saved. `OpPayload`
  (`edit_journal.h:86-97`) makes every op the size of two `Building`s. `trim()` re-sums both stacks on every recorded op
  (`edit_history.cpp:325-334`, via `hold()` in `journal_ops.cpp:13-19`, per tile at `:398,:416`) and erases from the front
  of a vector (`edit_journal.h:137`). `j_set_lift` and `j_set_uturn` record unchanged values (`journal_ops.cpp:181-189,202-210`).
  Fix: data payloads for every op, heavy payloads by index into typed side vectors, a running byte total, a deque, the
  same early return as the other setters.
- **R7. The state hash is not a function of the state.** `save_nature.cpp:54-58` writes paint tiles in `unordered_map`
  order and `:70` writes `elements.all()` in hash-cell order with swap-removed vectors (`nature/elements.cpp:25,43-48`);
  `ByteWriter::canonical` (`core/bytes.h:14`) does not order them; the state hash hashes these bytes
  (`api/city_core_state_probe.cpp:121-137`). Equal cities reached by different histories hash differently, which is what
  `edit_fuzz_test` must not see. Fix: one ordered-container writer in `core/bytes`, used by every section (the rule is
  already stated at `persistence/body_codec.cpp:86`).
- **R8. Catalog constants are recomputed per call.** The widest profile: `Catalog::widest_half` loops every profile per
  call (`sim/catalog/catalog.h:100-104`), `road_plan.cpp:55` and `road_snap.cpp:137` loop again per pointer move,
  `ped_crowd.cpp:57-65` per tick; hard-coded copies `footway.cpp:11` 48, `street_spots.cpp:17` 40, `road_edit.cpp:19` 40,
  `transit_ground.cpp:13` `WIDEST_STREET = 4`. `ring_profile` searches the catalog per call (`roundabout.cpp:33-37`).
  Fix: computed once at catalog load, one field.
- **R9. More dirty channels than the `ChangeLog`.** `TrafficState::roads_touched` (`traffic_state.h:196`) is filled by
  `lane_graph_edit.cpp:185-186` and drained only by transit (`transit_plan.cpp:92,129`); the lots' dirty cells and the
  network's pending settle are two more. CLAUDE.md ("`ChangeLog` é o único canal de sujo") is false for the sim. Fix: the
  `ChangeLog` with one cursor per consumer, or CLAUDE.md names the render channel and each sim channel's owner.
- **R10. Geometry primitives written several times.** `closest_t` takes 57 curve evaluations per call
  (`roads/geometry.cpp:73-91`) and is the inner loop of every pick; the closest point on a quadratic Bezier is a root of a
  cubic, solvable in closed form. The arc-table lookup three times (`geometry.cpp:40`, `parking_layout.cpp:41-45`,
  `footway.cpp:15-20`); `quad_length` allocates a table per call (`geometry.cpp:26`; callers `road_grade_sync.cpp:50`, and
  `place_snap.cpp:105` builds a fresh table per candidate though `RoadSegment::table` exists, `road_network.h:103`); RDP
  twice with different metrics (`pole_runs.cpp:55-81`, `road_path.cpp:31-56`); point in polygon three times
  (`yard.cpp:52-59`, `lot_book.cpp:149-154`, b40 `contains_point`); segment intersection three times (`rail_run.cpp:46-53`,
  `geometry.cpp:93`, `lane_graph.cpp:148`); the chunk-box loop five times beside `for_chunks` (`road_network.cpp:278-279,449-452`,
  `lot_commands.cpp:30-33`, `journal_damage.cpp:17-20`, `road_edit.cpp:117-118`); seven 2D vector types (`V2`, `Point2`,
  `LotPoint`, `RoutePoint`, `StrokePoint`, `P2`, `Vec` in `place_snap.cpp:22-28`); private hashes beside `core/random.h`
  (`growth.cpp:18-23`, `street_names.cpp:18`). Fix: one owner each in `core`.
- **R11. The source game's code and numbers inside the sim.** Ports stated in the file heads: `roads/geometry.cpp:1`,
  `junction.cpp:1`, `profiles.cpp:1`, `road_grade.h:40-46`, `road_grade_profile.cpp:1-30`, `road_grade_solve.cpp:1-29`,
  `terrain/road_shape.cpp:1-23`, `terrain/terrain_gen.cpp:1` (with a hand heap "with the same sift order as the
  TypeScript", `:182-184`), `traffic/demand.cpp:1-3`, `meso.cpp:1`, `simulation/growth.cpp:1`, `lot_book.cpp:1`,
  `ped_fright.cpp:1` ("after GTA V"), `traffic_state.h:27` (`TRAFFIC_SEED = 12345`, the same traffic stream in every city),
  and converter output in `sim/roads` (`control_advisor*.{h,cpp}`, `flow_stats*.{h,cpp}`: double lookups, `const_cast`,
  `enable_shared_from_this` for nothing, an empty `flow_stats.h`). Their constants sit in code by the source's names
  (`junction.cpp:16-19`, `junction.h:21-23`, `geometry.cpp:18-20`, `MAX_RETREAT = 60` twice, `road_grade.cpp:18-38`,
  `road_grade_solve.cpp:20-27`, `terrain_brush.cpp:17-27`, `terrain_gen.cpp:14-29`, `demand.cpp:15-19`, `meso.cpp:16-22`,
  `junction_control.cpp:15-21`, `shoot_people.cpp:17-23` "(the 4.0: 102 of 256)", `shoot_vehicles.cpp:13-15`,
  `lot_book.cpp:11-15`, `body_survey.cpp:19-26`, `lane_change.cpp:14`, `ped_crossing.cpp:16`, `damage_state.h:47,53`).
  Source files are cited in most sim files (e.g. `vehicle_motion.cpp:3,35,46`, `transit_plan.cpp:123`, `street_works.h:20-21`).
  Fix: every rule designed and justified in this game, values in `project/data` with their reason, generated code deleted.

## 3f. `native/src/sim`: wrong results (each certain from the code)

- **`duplicate_road` does not make a parallel road** (`commands/road_copies.cpp:56-62`): it moves the three control points
  by one vector normal to the chord; where the end tangent leaves the chord at angle a, the gap is offset·cos a (0.71× at
  45°), so curved copies run too close and may be refused. Fix: offset each end along its own tangent normal and put the
  control where the two offset tangents meet (`line_line`).
- **Bicycles may use every road, motorways included** (`traffic/lane_graph.cpp:198`): `d.bikes = p.walk_left > 0 ||
  p.walk_right > 0`, but `walk_left/right` are the u of the first and last strips whatever their kind
  (`roads/profiles.cpp:79-83`), positive for every profile of two strips. Fix: use `has_walk_left || has_walk_right`
  (`profiles.cpp:80-81`).
- **Lanes past 4 a direction are drawn but not driven** (`lane_graph.cpp:196`, `MAX_LANES = 4` in
  `lane_graph_types.h:10`), while `spec_issues` sets no limit and the drawer offers 6 (`data/ui/drawers.json:138-139`,
  `road_variants.cpp:81`). Fix: one limit from data, refused by the composer.
- **A bus line whose first stop is a terminal starts its new buses at a later terminal** (`transit/transit_plan.cpp:37,41`:
  `start == 0` means both "none found" and "found at 0"). Fix: `-1` as "none".
- **The smooth brush reads what it just wrote** (`terrain/terrain_brush.cpp:70-73` inside the raster loop of
  `Heightfield::brush`, `:90-102`): left and upper neighbours are already smoothed, so the result drifts with the scan
  direction. Fix: read from a copy of the dab rectangle (Jacobi).
- **A route retry stalls a whole lane for one tick** (`traffic/vehicle_motion.cpp:161-167`): moving the head to another
  lane list while iterating by index makes `step_lane` return, so the vehicles behind are not integrated while the clock
  advances. Fix: collect lane moves like `leaving` and apply them after the loop.
- **Two rules for the main road through a junction**: `traffic::main_legs` (`junction_control.cpp:218-224`, used by
  `flow_control.cpp:29`) takes the two highest-priority arms at any angle; `green_wave.cpp:29-53` takes the most aligned
  pair; `junction_control.cpp:207-215,231-234` ranks inline a third time. Fix: one owner in `sim/roads`.
- **Two rules for what kind of place a building is**: `ped_doors.cpp:21-26` reads the body's use, `parking_sources.cpp:56-60`
  the zone base; one building can send walkers as a shop and cars as a home. Fix: one `place_kind(record)`.
- **Two rules for "inside the micro window"**: `window.cpp:55-64` from the lane's middle point, `:25-29,131-132` from the
  segment box middle (`meso.cpp:210,247` ask one, `check_window` uses the other). Fix: the `DirSeg`'s stored middle for both.
- **A signal command reads the traffic's derived graph** (`commands/signal_commands.cpp:14-17,41`), which the traffic
  thread rebuilds later from the `ChangeLog`: right after an edit the command reads a stale graph. Fix: the control kind
  read from the road network.
- **Road variants are player state stored in the `Catalog`** (`roads/road_variants.cpp:128-160`), "full" detected by vector
  capacity (`:135`, reserved in `api/catalog_loader.cpp:89`), never released and not undone: after `road_max_variants`
  compositions the tool refuses every new road. Fix: a journalled `VariantTable` in `CityState`, counted by use.
- **Two `count_entry` overloads with different meanings** (`flow_control.cpp:62` per connector, `traffic_state.h:220` per
  ds; `meso.cpp:259` picks by argument type). Fix: distinct names.
- **Wound events bypass the event cap** (`demolition/wounds.cpp:22,35` push into `s.log.damage_events` directly, beside
  `emit_damage_event`, `strike_commands.cpp:19-21`).
- **The grade solver hides non-convergence** (`roads/road_grade_solve.cpp:184-195`, `MAX_SOLVE_PASSES = 64`), and its
  envelopes run fixed pass counts that either change nothing (`road_grade_profile.cpp:67-74`, `ENVELOPE_PASSES = 4`: one
  forward and one backward pass are exact) or decide the result (`SLOPE_PASSES = 6` `:179-186`, `PIN_ROUNDS = 3`
  `:224-227`). Fix: exact one-pass envelopes, a monotone solve without a cap that reports a fault if it does not settle.
- To verify in `api`: commands mutate walkers and vehicles under `map_mutex` alone (`api/city_core_strike.cpp:20-21`)
  while `api/city_core_transit.cpp:100` reads under `traffic_mutex` alone; the `traffic_hashes` probe runs
  `traffic::step` from the API thread (`api/city_core_state_probe.cpp:109-119`).

## 3g. `native/src/sim`: cost that grows with the city (per tick, per pointer move, per edit)

- Per tick: `publish_poses` walks the whole fleet (`traffic_system.cpp:153-188`, up to `FLEET_MAX = 65,536`,
  `demand.cpp:15`) and `view_vehicle` tallies every vehicle per segment and node (`traffic_view.cpp:105-117`), though
  `meso.cpp:2` claims no loop over the fleet; a `leaving` vector per active lane (`vehicle_motion.cpp:323`);
  `bleed_step` walks every walker (`wounds.cpp:12-38`); lorry calls walk every bay and lorry every 2 s
  (`lorry_traffic.cpp:246-253`). Fix: poses from `micro_list` plus off-lane cars, counts kept on enter/exit, a bleeding
  list, a free-dock list.
- Per pointer move: `ray_deck` marches 0.5 m steps with a `closest_t` each (`road_network_deck.cpp:9-61`);
  `intersect_quads` tests 1,024 piece pairs per candidate with no box rejection (`road_plan.cpp:31-46`) and
  `overlaps_road` runs `closest_t` per 4 m station (`:60-76`); the pole-run preview builds its graph twice and searches
  `Route::at` linearly (`pole_runs.cpp:86-91,146-177`); `clear_side` searches each building's street per dab
  (`zone_commands.cpp:42-60`, because a record keeps no frontage segment).
- Per edit or blow: `drop_pedestrians_on_seg` walks every walker per removed segment (`pedestrians.cpp:247-254`);
  `on_conn_removed` walks `micro_list` per connector (`lane_graph_edit.cpp:54-55`); shots and blows loop every walker and
  vehicle (`shoot_commands.cpp:57,89`, `strike_people.cpp:104-120`); `wear()` writes two whole `Building` copies per
  building in reach (`strike_buildings.cpp:106-112`); `clear_relief` settles the network once per dirty region
  (`nature_commands.cpp:295-306`); `nearest_free` scans every lot of every queued cell per building grown
  (`city_growth.cpp:21-43`); `road_blocks.cpp:28-29` plans each grid line twice.
- Per sample: `GroundPaint::cell` a hash find per sample (`ground_paint.cpp:55-60`); `HeightPatch::height_at` four integer
  divisions (`height_patch.h:32-38`); the grade reads the ground through `std::function` (`road_grade.h:38`, up to 2,800
  calls per segment, `road_grade.cpp:85-105`); `water_at_sample` probes nine lakes' hash maps (`terrain_gen.cpp:261-276`);
  `hurt_of` and `is_gone` are linear (`damage_state.cpp:67-77`).
- Smaller: allocations per call in `wanted_retreat` (`road_rules.cpp:26-36`), `refresh_node` (`road_network.cpp:328-341`),
  `elements.for_near` (`elements.h:33-41`); `along_street` O(n²) (`road_tabs.cpp:37-52`); `stop_near` and `nearest_track`
  linear (`transit_data.cpp:220-247`) while `transit_data.h:3` says they are filed by cell; a linear colour search per bus
  spawn (`transit_sim.cpp:48-49`).

## 3h. `native/src/sim`: stale comments, misnamed code, leftovers

- `road_network.cpp:16` names `LOT_MARGIN`; `transit_data.h:3` claims a cell index; `meso.cpp:2` claims no fleet loop.
  (Corrected 2026-11-07 while reading `render/cover`: `ecology.h:4-5` is right. `compute_ecology` has one call site,
  `render/trees/tree_place.cpp:79` inside `chunk_ground`, but `chunk_ground` is called by the trees,
  `tree_place.cpp:284,293`, and by the cover, `render/cover/cover_renderer.cpp:152`: the ecology of a chunk is worked out
  again by each job that reads it, with no shared result. See §3i.)
- Comments above the wrong declaration: `road_network.h:57-63`, `road_grade_internal.h:24-31`, `road_edit.h:18` (an orphan),
  `transit_commands.h:18-19`, `traffic_system.h:200` (a `ped_crossing.cpp` label over `signal_timing.cpp` functions).
- Empty anonymous namespaces left by splits: `kerb_stops.cpp:25-27,74-76`, `lane_closures.cpp:15-16`; `kerb_stops.cpp:304`
  computes `door_places` twice in one expression.
- Misnamed: `dab_chunk` is a distance (`nature_commands.cpp:58`); `settle_under` only clears (`road_tabs.cpp:56`);
  `Profile::right`, `left`, `half()` are one number (`profiles.cpp:67`).
- `junction.cpp:2,6` includes `core/math.h` twice; `lot_cache.cpp:8,10` include `<atomic>`, `<thread>` unused.
- A parked feature in a comment: `city_state.h:83` (`style`, "the save block and the single reader come with the
  mechanism"); CLAUDE.md sends pendencies to `log/pendencias.md`.
- Probe-only code inside `sim/traffic`: `read_lots_with_docks` (`parking_sources.cpp:182-195`), `walkers_in_furniture`
  (`ped_furniture.cpp:139-165`).
- Zone codes typed again (`parking_sources.cpp:21`, `street_furniture.cpp:15-16`, `growth.cpp:60` `zone > 4`); the limb
  index convention has no owner (`ped_wounds.cpp:12` vs `shoot_people.cpp:25`); a corner tie broken by node-id parity
  (`ped_routes.cpp:119`); `has_side` twice (`ped_spawn.cpp:18`, `ped_routes.cpp:64`); the centre-line kind is a string in
  three places and an index in the save (`profiles.cpp:11-16`, `road_spec.cpp:161-171`, `road_variants.cpp:155`,
  `save_format.h:25`); `kerb_of` takes "the first road with a kerb" in data order (`road_variants.cpp:103-109`);
  `CHAIN_GUARD = 4000` beside a visited set (`street_spots.cpp:18`); terrain modifiers keep their own free-id list beside
  `SlotIds` (`heightfield.cpp:122-153`); `CASING_BAND_SHAPE_M` copies `CASING_BAND_M` to dodge the layering
  (`road_shape.cpp:23`, `road_grade.h:44`).
- Data in code: the six biomes and their formations are a `switch` (`nature/ecology.cpp:58-121`); the regional style of
  street furniture is fixed to one country's standards (`street_furniture.cpp:3-4`, `street_works.h:37-38`).

## 3i. `native/src/render`, read in full (66 chunks): root causes shared by many findings

- **L1. The mesh jobs hold the map lock for their whole build.** `render/mesh_queue.h:70-74` (`submit_job`) runs
  `build(b->mb)` inside `std::shared_lock` on `map_mutex`; every one-surface renderer goes through it: roads, verges,
  structures, far roads (`far_road_renderer.cpp:46-55`, a whole superchunk), furniture (`prop_renderer.cpp:69-77`), water
  (`water_renderer.cpp:107`). The cover (`cover_renderer.cpp:190-195`), the zone overlay (`overlay_renderer.cpp:99-102`)
  and the trees (`tree_renderer.cpp:195-196`) do the same by hand. Only buildings (`BuildingRenderer::gather`), fog,
  canyon, shore and ruins copy their inputs under a short lock and build outside it. With libc++'s writer-preferring
  `shared_mutex` (§0), a command waiting for the exclusive lock waits for the longest running job, and the frame's own
  `shared_lock` (`render_world.cpp:221-294`) waits behind that command: an edit stalls the frame for a mesh build.
  `render_world.cpp:237` ("a frame never waits for the mesh jobs") is false. Fix, one for all: each producer gets a
  gather step (segments, nodes, a `HeightPatch`, the chunk's lots, paint tiles, water rows) under a short lock and builds
  outside it, as `BuildingRenderer::gather` does; `MeshQueueOf::submit_job` takes the gathered input, not `CityState`.
- **L2. Data race on the walkers.** `render/effects/effect_bodies.cpp:118-119` reads `s.traffic.peds.size()` and
  `s.traffic.peds[e.id].age` from `EffectWorld::sync` (`effect_world.cpp:148-157`), called under `map_mutex` shared only
  (`render_world.cpp:238-242`); the sim thread spawns and removes walkers (the vector reallocates) under map shared +
  `traffic_mutex`. Undefined behaviour. Fix: the damage event carries the walker's age (the sim knows it when it emits the
  casualty); the render reads traffic only through `PoseBuffer`.
- **L3. Transit meshes are built on the main thread.** `transit_renderer.cpp:213-227` builds up to `items_per_frame` whole
  items inside `RenderWorld::sync` (`render_world.cpp:260`): a track item lays every piece, sleeper and rail
  (`transit_track_parts`, a footway search per sleeper step) and commits synchronously. Fix: the job + `ChunkResults` path
  of the other renderers, items as admission units, bytes through the ledger.
- **L4. Two result protocols for one pipeline.** `render/chunk_jobs.h:125-194` (`commit_baked`, newest-seq maps, its own
  deadline) serves the buildings; `chunk_jobs.h:223-322` (`ChunkResults`, tickets with `ChunkRevisions`, ranked by
  `AdmittedQueue`) serves the rest. Buildings take the frame's credits before every producer (`render_world.cpp:263`),
  outside the "one priority order" `frame_admission.h:1-3` promises: a building hole does not outrank a road rebuild and
  two codes decide which result may still be drawn. Fix: buildings on `ChunkResults`; delete `commit_baked`, `Results` and
  the requested-seq maps.
- **L5. The single owner of level of detail is dead code.** `render/detail.h:1-5` says every family chooses its levels by
  `DetailView` and that no distance is tuned by hand; `api/city_core.cpp:150,180` fills it each frame
  (`render.detail_error_px`, `tuning.json:48`) and `render_world.h:64-67,209` stores it, but no renderer calls `view()`,
  `px_of`, `fits` or `reach_of` (grep: no caller). Every family tunes distances and hystereses by hand:
  `agent_renderer.cpp:55-61,239-246`, `people_renderer.cpp:111,241-247`, `building_renderer_reach.cpp:36-52`,
  `building_renderer.cpp:281-288`, cover and tree `reach_m`, fleet shadow. Fix: the families choose levels through
  `DetailView` from each level's error in metres (`agent_lod` already builds levels by `error_m`, cast hulls by
  `hull_error_m`) and the hand distances go; or `detail.{h,cpp}` are deleted and the documents stop promising it. The key
  `render.detail_error_px` itself is live: `api/city_core.cpp:151` sets the viewport's `mesh_lod_threshold` from it (Godot's
  own mesh LOD), every frame.
- **L6. R1 in the render: five private body caches, regenerated bodies.** `BuildingBodies` (`building_bodies.cpp:61-65,72,84`)
  empties the whole cache, rooms and cuts when it reaches 8,192 / 8,192 / 1,024 entries: a view holding more distinct
  bodies regenerates all of them in a storm each time it fills. `canyon_field.h:40` keeps a second `BuildingBodies` per job
  scratch (`canyon_field.cpp:114`). `BodyWalls` (`body_walls.cpp:10-31`) runs `record_mass` per building near a thrown body
  and clears on any revision. `GardenBook` (`garden_place.cpp:106-122`) runs `record_body` per building and clears whole at
  65,536. `interior_emit.cpp:106` and `interior_lamps.cpp:25-30` build the `Interior` again per call;
  `road_markings.cpp:121-139` (`parking_paint` → `street_gates`) generates every body along each parking street inside the
  road job. `effect_ruins.cpp:22,31-37` guesses a ruin's height from `STOREYS_PER_M` and a six-colour palette though the
  ruin carries its record. Fix: the sim-side `BodyCache` of R1 (single entries evicted LRU by memory), read by render,
  canyon, walls, gardens, interiors, parking and ruins.
- **L7. One chunk's ground is placed three times.** `chunk_ground` (`tree_place.cpp:74-79`: roads, lots, ecology) runs in
  the near trees (`:282-289`), the far trees (`place_far :291-300`, which redoes the whole placement, street trees, gardens
  and the woods lattice for each of the 16 chunks of a superchunk, `tree_renderer.cpp:122-132`) and the cover
  (`cover_renderer.cpp:152`). Fix: one placement per chunk and nature revision, kept and read by near models, far cards and
  cover.
- **L8. Shapes derived by two jobs.** The junction shape (`ends_of`, `corner`, `outside`, `sharpen`) is computed in the road
  job and again in the verge job (`road_junction.cpp:187-207` and `:288-313`); wall runs (`road_walls.cpp:35-52`) in the
  structure job and again in the verge job (`emit_wall_backfill`, `walled_at` per sample `road_verge.cpp:76`). R3 in the
  render: `prop_renderer.cpp:69-77` derives each segment's kit three times; `street_props.cpp:181`, `:126-138` (two calls
  per through node), `:267-277` derive it again; `street_ground` (`street_props.cpp:208-214`) runs `footway_at` per pole
  foot, preview pole, meadow and barrier end (`barrier_boxes.cpp:100`). Fix: per-node junction shapes and per-segment
  street layouts cached with the segment revision (R3's owner).
- **L9. God classes split across files.** `BuildingRenderer` (`building_renderer.h:41-251`, ~60 members, a 25-field
  `Chunk`) owns streaming, superchunks, jobs, baking, commit, bands, bay props, fades, interiors (`interior_cost`,
  `building_renderer_build.cpp:232-253`), the bay census, broken buildings, ruins, signature designs and probes, over
  `building_renderer{,_build,_commit,_probe,_reach}.cpp`. `EffectWorld` owns blast, bodies, bullets, ruins, heaps, thrown
  things, flash, fires and geysers over `effect_{blast,bodies,ruins,things,world}.cpp`. CLAUDE.md forbids both. Fix: one
  owner per responsibility (chunk streamer, far meshes, building jobs, probes outside; one owner per effect family under a
  thin router).
- **L10. R2's guessed pads in the render.** `canyon_field.cpp:27` `EDGE_REACH_M = 64`, `body_walls.cpp` (centre query),
  `tree_place.cpp:25,78` `LIST_MARGIN = 80`, `transit_parts.cpp:77-91` `entrance_spot` through `transit_built` (a metro
  entrance drawn inside a building). They go with R2's fix (buildings filed by footprint).
- **L11. Contracts with no owner.** (a) C++ ↔ shader packing typed twice and kept equal by comments: `shell.cpp:26-27`,
  `:224-225`, `building_renderer_build.cpp:27`, `people_renderer.cpp:22-38`, `agent_looks.cpp:21`, `shell.cpp:147-148` vs
  `building_renderer_commit.cpp:20,34-36`, `terrain_bridge.cpp:27-31` vs `ground_paint.gdshaderinc`,
  `effect_bodies.cpp:29` vs the people shader. Fix: one table generating both the C++ header and the `.gdshaderinc`, or a
  gate comparing them. (b) The sRGB transfer has five forms: `pow 2.2` in `people_asset.cpp:44`, `shell.cpp:29,46`,
  `tree_pictures.cpp:25,116`; the exact curve in `helmets.cpp:33`, `linear_mips.cpp:15-19`, `tree_pictures.cpp:34,166`.
  Fix: `core/color` owns both directions (the exact curve). (c) JSON vec3/hex/colour readers: `agent_parts.cpp:25-35`,
  `held_things.cpp:16-19,39`, `helmets.cpp:33`, `bay_props.cpp:24-27,151-156`, `cover_kit.cpp:24`, `barrier_boxes.cpp:110`,
  `prop_kit.cpp:25-28`, `road_look.cpp:17`, `transit_parts.cpp:20-32`, `tree_kit.cpp:26-34`, `overlay_build.cpp:16-25`,
  `overlay_renderer.cpp:47-49`, beside `core/color.h parse_hex_color` and `mesh_builder.cpp:33-40`. Fix: one reader in
  `render_context`. (d) Two MultiMesh owners: `effects/mm_layer.h` `MmLayer` vs `rs_util.h` `MultiMeshDraw`/`InstanceRows`;
  the 20-float row written twice (`instance_rows.h:22-26`, `mm_layer.h:79-85`). (e) Two data-driven part builders with
  two vocabularies: `prop_kit.cpp:31-70` ("beam") and `bay_props.cpp:37-67,135-149` ("bar"). (f) Order tables kept by hand:
  `agent_renderer.cpp:155-156`, `building_renderer_probe.cpp:111`, `road_look.cpp:36`, finish names twice
  (`furniture_look.cpp:14`, `api/buildings_loader.cpp:23`), people clip/act tables (`people_renderer.cpp:28-38`) vs the
  cooked file and `people_face.gdshaderinc`, the kerb door side (`car_body.cpp:26` vs `sim/traffic/kerb_stops.cpp`), the
  ragdoll part order (`body_renderer.cpp:22`, `effect_bodies.cpp:44-47` vs `ragdoll.cpp:17-20`).
- **L12. The frame's upload flush rests on an engine internal.** `frame_admission.h:93-97` calls
  `get_video_adapter_name()` because a wrapped getter runs the server's pending queue. Godot binds the documented call for
  this: `ClassDB::bind_method(D_METHOD("force_sync"), &RenderingServer::sync)` (`servers/rendering/rendering_server.cpp`),
  and `RenderingServerDefault::sync()` is `command_queue.sync()` with a render thread, `command_queue.flush_all()` without
  (`servers/rendering/rendering_server_default.cpp`); `force_sync` is in `extension_api-4-7.json`. Fix:
  `RenderingServer::get_singleton()->force_sync()`.

## 3j. `native/src/render`: wrong results (certain from the code unless marked)

- The old-building look of balconies never shows: `facade_props.cpp:199` is the only `Furnish` and passes `old = 0`;
  `standard()` (`:131`) needs `old_ > shares.old_popular` (0.35, `bay_props.json:93`), `window()` (`:189`) needs
  `rnd() < old_ * shares.ac_old`. `STANDARD_POPULAR`, the popular branches (`:109,119-122`), `plastic` colours (`:139`) and
  `ac_old` are dead, with their data. Meanwhile `roof_plant.cpp:174` derives an age from the seed. Fix: one age (the
  record's, or one function in b40) passed through `Placed` and read by both.
- Shaped blocks hide neighbours by their box: `covered()` (`placed.cpp:47-56`) ignores `Volume::outline`; so does
  `roof_covered` (`roof_emit.cpp:21-27`, cores at `:94`). Bays facing the empty part of an L's box are drawn plain or
  skipped (`facade_emit.cpp:106-107`), far cells read covered (`far_mass.cpp:65-74`). Fix: point-in-outline when set.
- `built_on` (`roof_util.h:11-15`) tests `o.base == v.top()` only, not that `o` stands over `v`: a block of the same height
  elsewhere leaves a free terrace unfurnished (`roof_emit.cpp:126,140`); `covered_by_above` (`:18-22`) tests overlap. Fix:
  one predicate with the overlap test.
- The bay census counts other bays than the drawn ones: `for_each_bay` (`facade_emit.cpp:221-237`) visits covered bays with
  `plain = false`; the drawn walk (`each_bay :86-123`) skips or plains them. `bay_kit.h:3` ("172,508 bays, 256 forms cover
  82 %") and `building_renderer_probe.cpp:45-108` count bays never drawn. Fix: one walk.
- `people_renderer.cpp:65` `acts.set(asset_.clip(f.clip), f.act)` with no check for -1: a cooked file without a face clip
  writes index -1 (an engine error each boot, the act lost); `init` checks act and seat clips (`:143-154`), not `FACE_ACTS`.
- `transit_parts.h:52` `part_of.at(name)` and `transit_parts.cpp:104,116,118` ask for literal names `read()` (`:158-207`)
  never checks: a data file without one throws `std::out_of_range` inside a chunk build. Fix: resolve at boot, fail loudly.
- `tree_parts.cpp:146,160,163-164,180-182` average the far card tone in sRGB though `tree_parts.h:20-21` says tones mix in
  linear: the card comes out darker than the crown.
- `shell.cpp:276-280` `bytes()` counts 4 custom arrays per vertex when `rooms`, though the far shell uploads 1
  (`building_renderer_build.cpp:263`): the upload ledger overstates far meshes by 12 bytes a vertex.
- `effect_params.h:1-2` says missing keys are reported; bursts and sources (`effect_params.cpp:43-50,57-63`) read
  `row.get(key, 0)`. Same in `overlay_renderer.cpp:32-40`.
- `agent_meshes.h:70` `CabinLook` carries `mutable std::vector<SeatSpot> *spots`: builders return seats through a shared
  const look (a race if two cabins build in parallel). Fix: return the spots.
- `heavy_meshes.cpp:84` the deflector side quad repeats a corner: a degenerate triangle per side (use `mb.tri`).
- `mounds.cpp:97` heaps past `MOUND_SLOTS` (2,000, `effect_world.cpp:18`) are dropped silently.
- `sign_atlas.cpp:54-66` cells are only added until a new city: renamed streets keep old plates and open layer after layer.
- `ruin_fracture.cpp:20` `LONGEST_SIDE_M = 2.8` repeats `ruin_cells.h:16` `VORONOI_EDGE_M`; `ruin_sim.cpp:23`
  `MOUND_SPREAD` repeats `effect_ruins.cpp:26`; roof sizes typed in both levels (`roof_emit.cpp:13,17-18` vs
  `far_mass.cpp:18-19`, and `roof_plant.cpp:171` `int(3 / CELL)`): a change to one makes the hand-over jump.
- Plausible, not certain: `agent_parts.cpp:265` the bicycle boss radius `r * BOSS_R * BOSS_R` (every other boss is
  `r * BOSS_R`).
- Data race (resolved in `api`, §3m): `building_renderer_probe.cpp:54-58` copies records under a shared lock, releases it,
  then reads `s.terrain.patch` (`:67`) with no lock; its caller `api/city_core_jobs.cpp:19` holds none.

## 3k. `native/src/render`: cost that grows with the city or the frame

- The congestion layer re-uploads whole meshes to change colours (`overlay_renderer.cpp:107-141`) and charges them as
  mandatory, pushing the ledger into debt. Fix: `RenderingServer::mesh_surface_update_attribute_region` on the colour
  attribute, or a per-segment speed texture read by the shader.
- `overlay_build.cpp:128-129` each zone chunk job walks every player lot of the city (R4: no spatial index on `LotBook`).
- Terrain: `terrain_bridge.cpp:76-121` re-sends each touched region whole (256 × 256 samples, 256 KB, at
  `tile_m 1024 / height_spacing_m 4`) once per frame with edits (`render_world.cpp:250-251`); Godot has no partial texture
  update (`extension_api-4-7.json`: only `texture_2d_update`), so the region is the floor. The avoidable cost is
  `write_control` (`:102`): one `GroundPaint::cell` hash find per sample, 65,536 per region. Fix: read the paint tiles of the
  region as rows.
- Water: `water_renderer.cpp:98-109` builds and keeps every wet chunk of the map at any distance, one free-standing quad
  per 8 m cell (`:79-90`, each corner level computed four times); no reach, no far level.
- `rs_util.h:101-104` `MultiMeshDraw::set(PackedFloat32Array)` copies the whole buffer whenever the count is below the
  capacity (almost always): an allocation and copy per call for props, signal lamps, cover, overlays.
- `body_renderer.cpp:226` and `ruin_pieces.cpp:215-216` make a new `Image` of the whole buffer every frame something moves.
- `fleet_shadow.cpp:61-67` frees a sector's MultiMesh after one empty frame and creates it again: RID churn at edges.
- `building_renderer.cpp:304-305,328` takes `queue_mutex_` per chunk per frame on the main thread.
- `agent_renderer.cpp:263` rolls the odometer of every published vehicle before the distance cull (`:265`); `odo_` grows
  by vehicle id (`:197`).
- `fog_field.cpp:141-144` re-buckets every dab ever painted on each fog revision; `:209-216` each job scans the whole
  512 × 512 window.
- `road_verge.cpp:42-43` samples the ground three times per ribbon vertex.
- `wear_field.cpp:104,107` two engine calls per wheel and foot sample (`PackedByteArray` `operator[]`/`ptrw()` go through
  `gdextension_interface`, `godot-cpp/src/variant/packed_arrays.cpp:50-64`). Fix: tiles in `std::vector<uint8_t>`, copied
  into the `PackedByteArray` in `upload()`.
- `pane_rooms.cpp:85` copies the floor's partitions per pane; `ruin_fracture.cpp:165-166,182` builds each cell's edge map
  twice; `agent_parts.cpp:282-286` three fresh vertices per cap triangle (boot only).

## 3l. `native/src/render`: language, source-game ties, dead code, stale comments, numbers in code

- Portuguese error text (language rule): the shared source `render_context.h:30,36`; and `agent_looks.cpp:28,71,73`,
  `agent_renderer.cpp:70,72,76,107`, `body_renderer.cpp:35`, `held_things.cpp:45`, `helmets.cpp:27`,
  `people_asset.cpp:221,268,277`, `people_renderer.cpp:113,117,136,140,145,149,153`, `bay_props.cpp:148,163`,
  `building_look.cpp:42,48`, `furniture_look.cpp:39-58,78,85,88`, `cover_kit.cpp:97,102`, `element_models.cpp:47,53,61`,
  `shore_stones.cpp:33`, `effect_bodies.cpp:56`, `effect_params.cpp:19,23`, `effect_world.cpp:41`, `overlay_renderer.cpp:43`,
  `barrier_boxes.cpp:116`, `prop_kit.cpp:69,95,211,217,220,237,245`, `prop_renderer.cpp:37`, `street_props.cpp:282`,
  `street_signs.cpp:72,82`, `road_look.cpp:21,28,107`, `road_renderer.cpp:22`, `terrain_bridge.cpp:66`,
  `transit_parts.cpp:51`, `leaf_atlas.cpp:75`, `tree_kit.cpp:38,136,140`, `tree_look.cpp:69,75,85,107`,
  `tree_pictures.cpp:80`, `tree_renderer.cpp:43`.
- Placement and geometry chosen for parity with the source game (R11, certain from the comments): `tree_look.cpp:23-24`
  `PATCH_SALTS` ("the woods stand where they stand there"), `tree_place.cpp:21-22` (`WOOD_KEEP`..`WOOD_KIND`, "the same tree
  in the same spot"), `flag_cloth.cpp:1-2` (24 × 12 cells "so the wave ... bends the same"), `region_table.h:10`
  `DEFAULT_REGION = "roadcraft"`, `road_structures.cpp:1,19-21` (colours and soffit band copied), `agent_wreck.cpp:1-2,16-19`
  (dent waves), `garden_place.cpp:1-3,30`, `lot_props.cpp:15`, `roof_gear.cpp:16-20`, `debris.cpp:22`, `wear_field.h:27`
  ("the same use in both games"), `puffs.h:18` ("as the reference does"). Assets taken from the source: people
  (`people_asset.h:1-2`), signature designs (`signature_bodies.h:1`). Source files cited as the reason: `agent_renderer.cpp:239`,
  `body_renderer.cpp:151`, `held_things.h:1`, `helmets.h:1`, `people_renderer.cpp:306`, `people_renderer.h:2`,
  `ragdoll.h:1`, `ragdoll.cpp:17`, `interior_cut.cpp:3`, `interior_lamps.h:12`, `occluders.h:1`, `part_models.cpp:101`,
  `roof_outline.cpp:1`, `ruin_fracture.h:1`, `ruin_pieces.h:1`, `ruin_sim.cpp:15,125`, `film_grade.h:2`,
  `building_renderer_commit.cpp:175`, `render_layers.h:28`, `effect_bodies.cpp:43`, `effect_blast.cpp:133`,
  `effect_params.h:14`, `effect_things.cpp:45`, `overlay_build.cpp:44`, `barrier_boxes.cpp:1`, `sign_atlas.h:1`,
  `street_props.cpp:140-141`, `street_props.h:62`, `road_junction.cpp:223`, `road_section.h:26`, `road_segment.cpp:215`,
  `road_stays.cpp:1`, `road_verge.cpp:2`, `road_walls.cpp:1`, `garden_place.h:1`, `tree_kit.cpp:23`, `tree_look.cpp:51`.
  Fix: each rule argued by this game's need; the parity salts and copied colours replaced by this game's own choices.
- Dead code: `road_renderer.cpp:93` `update_view` (empty, called every frame `render_world.cpp:258`); `road_look.h:39`
  `plate_concrete`; `agent_parts.cpp:198-203` `axis_out * 0`; `agent_cabin.cpp:80` `rod * 0`; `facade_emit.cpp:27,36`
  `(void)first; (void)last;`; `lot_emit.cpp:141,145` `Basis down`; `far_mass.cpp:190` `(void)z0`;
  `building_renderer.cpp:111-112,185-186` `(void)s`, `(void)far`; forwarders `road_emit.cpp:45-46`,
  `interior_emit.cpp:26`, `facade_emit.cpp:24`; `far_mass.cpp:131-149` a hand copy of `rectangles` (`:43-62`).
- Stale or misplaced comments: `building_renderer.cpp:2`, `building_renderer.h:7` ("512 m SUPERCHUNK"; `core/grid.h:19` is
  1024); `building_renderer.h:243` (a far bit never set); `agent_renderer.h:1-3` ("two MultiMeshes"; it reads the
  `PoseFrame`); `agent_lod.h:1-3` (history); `mesh_builder.cpp:1` (smooth normals); `prop_kit.h:51` (a light disc that is not
  built); `lot_parts.cpp:85` ("ten past ten"; `:102-103` draw three o'clock); `effect_world.h:4` ("never writes it", but it
  drains `s.log.damage_events`); `render_world.h:64-67` (DetailView), `render_world.cpp:237` (L1); `people_asset.cpp:39`
  (a truncated sentence); comments above the wrong declaration: `agent_meshes.h:117`, `people_renderer.h:80`,
  `facade.h:84-87`, `shell.h:152-155`, `effect_params.h:13`; `mesh_draw.h:1` (a header cycle with `rs_util.h`).
- Constants typed more than once: `CHUNKS_PER_SUPER` (`building_renderer.cpp:25`, `building_renderer_build.cpp:24`,
  `far_road_renderer.cpp:18`, `tree_renderer.cpp:19`); `CHUNK / SUBCHUNK` (`building_renderer_build.cpp:22`,
  `building_renderer.h:81`); the chunk half diagonal (`lamp_lights.cpp:20` 0.7072, `prop_renderer.cpp:12` 0.7071,
  `cover_renderer.cpp:20`, `tree_renderer.cpp:20`); local pi (`agent_cabin.cpp:18`, `car_shape.cpp:23`,
  `furniture_emit.cpp:14`, `road_junction.cpp:20`, `transit_parts.cpp:18`); `LINGER_SHARE` borrowed by canyon from fog;
  `casing_of` twice (`road_walls.cpp:26`, `road_structures.cpp:81`); `in_ds` twice (`traffic_system.h:18`,
  `ped_crossing.cpp:19`); value noise twice (`mounds.cpp:30-37`, `shell.cpp:40-70`); private hashes
  (`building_bodies.cpp:113`, `facade_emit.cpp:21-22`, `pane_rooms.cpp:17-19,33-48`, `placed.cpp:13,87`,
  `roof_terrace.cpp:24-29`, `effect_ruins.cpp:41`, `shell.cpp:30-31`, `transit_renderer.cpp:22,27-32`); the mesh AABB
  (`fleet_shadow.cpp:20-28`, `agent_wreck.cpp:27-33` beside `MeshBuilder::bounds`); the wear cell formula
  (`wear_field.cpp:102-103,116-117,152-156`); the water chunk cut (`water_renderer.cpp:58,75-76`); the bed "casts" test
  (`tree_renderer.cpp:115,156`); `PART_JOINT` (`body_renderer.cpp:22`) copies `ragdoll.cpp` `PART_FRAMES`; one number in two
  data files (`building_look.h:33` `far_fov_deg`, buildings.json vs camera.json); `transit_renderer.cpp:77` reads
  `heights.sidewalk` again (`StreetLook::h_sidewalk` owns it).
- Look numbers and colours in code (data rule): `facade_bay.cpp:12-20` (~45, with hash-share pick tables),
  `facade_parts.cpp:10-26,135-136` (~90 sizes, shutter colours chosen by channel comparison), `facade_props.cpp:16-26`
  (~60), `roof_plant.cpp:23-38` (catalogue and ~70 sizes; salted with `0x5f3759df`), `roof_pitched.cpp:17`,
  `agent_cabin.cpp:17-24`, `agent_renderer.cpp:19`, `body_renderer.cpp:15-20`, `lot_emit.cpp:21`, `lot_parts.cpp:12`,
  `lot_props.cpp:24`, `part_models.cpp:23`, `ruin_fracture.cpp:30-32`, `effect_blast.cpp:34-38`, `effect_things.cpp:16-24`,
  `road_stays.cpp:15`, `road_junction.cpp:17`, `fleet_shadow.cpp:17` (a guessed pad AgentRenderer can derive),
  `tree_look.cpp:102-105` (species names in code).
- Head comment `region_table.h:1-2` cites a Portuguese process file and parks a feature (with `city_state.h:83`): it goes to
  `log/pendencias.md`.

## 3m. `native/src/api`, read in full (19 chunks): root causes

- **A1. State read outside its lock: the protocol is not enforced.** The rule is "map shared, or map shared + `traffic_mutex`
  for traffic, or map exclusive". Broken in four ways, each certain from the code:
  (a) `lot_point` (`body_api.h:43-48`, reads `s.buildings.items[id]` and its lot frame) runs before `run_edit` takes the lock
  (`edit_result.h:30-31`) in fourteen commands: `city_core_blocks.cpp:37,58,67`, `city_core_body.cpp:123,223,236,241,253,261`,
  `city_core_furnish.cpp:18,23,28,36,45`, `city_core_handles.cpp:165` (`body_apply` → `edit_of :50`); growth appends buildings
  on the sim thread under the exclusive lock (the vector reallocates). (b) Traffic read under the map lock alone:
  `city_core_strike.cpp:64-97` `shot_targets` walks `t.peds` and `t.vehicles`, `:56-57` `damage_probe` (as render L2).
  (c) Traffic read under `traffic_mutex` alone while commands change traffic under the exclusive map lock without it
  (`city_core.cpp:107-111` `reset_traffic`): `city_core_park_probe.cpp:287-296` `kerb_busy`, `city_core_state_probe.cpp:104-107`
  `graph_fault`, `city_core_flow_probe.cpp:103-111` `flow_state`, `city_core_transit.cpp:99-147` `transit_run` (which also reads
  the network through `transit::crossing_gap :117`). (d) No lock: `city_core_flow_probe.cpp:81-89` `flow_push` reads nodes and
  `main_legs` (and indexes `nodes[-1]` when `flow_junction` found none); `city_core_jobs.cpp:19` `bay_census` (render §3j).
  Fix: one accessor layer in `api` that hands a script method a locked view (`ReadView`, `TrafficView`, `EditView`) so no
  method touches `CityState` without the right guard; compute lot points inside the edit.
- **A2. Read-only previews take the exclusive lock.** `city_core_edit.cpp:205` `preview_roundabout`, `:218` `segment_at`,
  `:266` `preview_lot`, all on pointer moves, against `city_core_commands.cpp:1-2`. Each waits for every mesh job holding the
  map (L1) and blocks the sim and the frame meanwhile. Fix: `shared_lock`. The Street tab's settings do the same to flip a
  toggle (`city_core_street.cpp:118-146`), and they are tool state stored inside `CityState` (`new_road_flags`,
  `new_road_lift`): they belong to the tool and travel with each command.
- **A3. Loaders are hand-written, inconsistent, and some crash on bad data.** godot-cpp's `Array::operator[]` dereferences
  what the engine returns, and the engine returns `nullptr` out of range (`godot-cpp/src/variant/packed_arrays.cpp:220-228`;
  `core/extension/gdextension_interface.cpp`); the build has `-fno-exceptions` (`native/CMakeLists.txt:11,99`), so
  `std::stoi`/`.at()` failures abort. Hence: unchecked positional arrays crash at boot in `buildings_loader.cpp:56,81,99,
  115-116,127,139,161,167,185,190`, `models_loader.cpp:84-85,98-102,112-114,134-138,162-166,181-182,205-206,234-235`,
  `interiors_loader.cpp:44-46,77-80`, `catalog_loader.cpp:185`, `city_core_grade_probe.cpp:57-67`; `models_loader.cpp:65`
  `sizes.at(kind)` aborts; `models_loader.cpp:222` `% schemes.size()` divides by zero on an empty list; `body_doc.cpp:94-95`
  `std::stoi` aborts on a malformed bay key; render `transit_parts.h:52` `.at()` aborts. The "strict" claims are false where
  types are not checked (`blast_loader.cpp:1-2,16-22`, `nature_loader.cpp:1,21`, `traffic_loader.cpp:1,41`) or numbers default
  silently (`buildings_loader.cpp:1-2,36`, `models_loader.cpp:1-2`, `body_doc.h:1-2`, `catalog_loader.h:2-3` vs `:203`,
  `furniture_loader.cpp:84-89`). Positional data (`opening [5]`, `piers [3]`, `pool [3]`, openings rows, `lit_share`, `era`,
  vehicle class rows `memcpy`'d into `VehicleClass`, `traffic_loader.cpp:18-19,53-57`) has no names for the schema to check.
  Fix: one strict reader (presence, type, size, range) used by every loader, as `catalog_loader.cpp:17-45` `Reader` half does;
  named fields in data.
- **A4. One vocabulary, many tables.** Regional style `"roadcraft"` in four C++ constants (`catalog_loader.cpp:80`,
  `nature_loader.cpp:124`, `sim/catalog/traffic_params.h:87`, render `region_table.h:10`), a fifth in GDScript
  (`project/game/data/data_tables.gd:65`) and ten data files, while `set_style` (`city_core_edit.cpp:85-91`) stores a style
  whose only reader is the snapshot echo (`city_core_query.cpp:137`), which `project/game/store/formatters.gd:38-42` uses for
  the interface's units and currency (`data/game/start.json` styles). So the start screen's choice
  (`project/ui/start_screen.gd:56`, `game/store/session.gd:63`) changes the units and the money shown and nothing else: the
  city is drawn, priced and simulated in "roadcraft" whatever was chosen. Bay names:
  `buildings_loader.cpp:21-22` ("sash") vs `body_doc.cpp:28-31` ("sashWindow"). Element names: `body_doc.cpp:41-47`,
  `city_core_query.cpp:220-222`, `models_loader.cpp:34-36` (three spellings and subsets). Roof names twice
  (`models_loader.cpp:31-32` = `body_doc.cpp:35-36`); patterns twice (4 vs 9 entries). Finish names three times
  (`body_doc.cpp:32`, `buildings_loader.cpp:23`, render `furniture_look.cpp:14`). Road classes twice (`catalog_loader.cpp:72`,
  render `road_look.cpp:36`); strip kinds in two vocabularies (`catalog_loader.cpp:64` "sidewalk/bike" vs
  `city_core_road_spec.cpp:16` "footway/cycle"). Zone codes a fourth time (`models_loader.cpp:28`). Walker act names a fifth
  (`city_core_people.cpp:119`). "mixed" is residential in `body_doc.cpp:40` and commercial in `models_loader.cpp:37,216`.
  Fix: each enum owns its name table once (beside the enum), every loader and probe reads it.
- **A5. The sim's data loads inside the render setup, from render files.** `city_core.cpp:79-83` loads buildings, models and
  interiors (`state_->buildings.designs`, simulation data by its own comment) in `setup_render`, after `catalog_ok_`;
  `new_map` needs only the catalog (`:102`). The tables live in `data/render/` and the loaders include
  `render/region_table.h` (`buildings_loader.cpp:4`, `models_loader.cpp:8`, `interiors_loader.cpp:4`). Fix: load them in
  `load_catalog`; move the files to `data/catalog/`.
- **A6. The facade is a god class with its split half done.** `city_core.h:30-228`: ~150 methods though areas exist
  (`city_core_areas.cpp:18-30`); about twenty probes stay on the core while `CityProbes` exists (`city_core.h:92-103,168-170,
  181,185-194`); `bind_queries` binds commands (`city_core_bind.cpp:93-164`); area bindings sit in arbitrary member files
  (`CityBuilder::_bind_methods` in `city_core_models.cpp:142-219`, `CityProbes::_bind_methods` in `city_core_jobs.cpp:43-72`);
  `CityBuilder` (`city_builder.h:13-103`) mixes body edits, model bytes, placement checks and a render stage.
- **A7. R1 and R2 reach the API.** `record_body`/`record_mass` per call in `body_api.h:37-41`, `city_core_body.cpp:34,93,172,
  277`, `city_core_furnish.cpp:54,75`, `city_core_handles.cpp:138-162` (every drag frame), `city_core_query.cpp:65,230,284`,
  `city_core_park_probe.cpp:37,281`, `city_core_clearance_probe.cpp:69`, `city_core_interior.cpp:43` (through render
  `interior_lamps`); a sixth private body cache, `InspectCache.solids` (`city_core_parts.h:35-58`, never evicted); `look_of`
  caches into `CityState`, so `look_twins` needs the exclusive lock (`city_core_park_probe.cpp:247-248`). R2 (buildings filed
  as points, `building_store.h:71-75`, `building_store.cpp:80`) breaks the camera and the inspector, below.

## 3n. `native/src/api`: wrong results (certain from the code)

- The camera does not see most buildings: `city_core_sight.cpp:132` `eye_free` asks a 2 m box round the eye
  (`POINT_BOX_M = 1`), so an eye inside a building is "in the open" unless that building's lot centre is within 1 m;
  `clear_reach`/`sight_pull` (`:94,106` → `solids_along :68-70`) ask the line's bounding box, a thin strip for a line near an
  axis, and miss every building whose centre is outside it. Fix: R2.
- The inspector names the building by its centre: `city_core_inspect.cpp:113-122` takes the nearest lot centre within 30 m;
  a click on a large building's wall names a neighbour or nothing. Fix: footprint containment (R2).
- Lots on a curved street are squared to its chord: `city_core_lots.cpp:76-91` `lot_angle_at` projects on the straight line
  between the Quad's ends and returns that angle (the nearest segment near bends is also wrong). Fix: the network's closest
  point on the curve (R10's owner) and the tangent there.
- The barrier preview allows every refusal but one: `city_core_street.cpp:64` `ok = why != "hint.barrier.road"`. Fix:
  `barrier_problem` returns a verdict with its key.
- `lot_clearance` cannot fail on long ways: `city_core_clearance_probe.cpp:81-82` gathers solids only within 20 m of the
  way's midpoint (by centre), then tests the whole way (`:84`).
- `body_mass_rect` indexes `items[id]` with no range check (`city_core_body.cpp:137`).
- Opening a city freezes the frame: `city_core_persist.cpp:109-136` decodes on the calling thread (the main thread); the head
  (`:2-3`) claims the open city "runs on" (only the sim does). Fix: decode on a worker, take the state when whole.
- A crash between the two renames of `write_atomic` (`city_core_persist.cpp:39-49`) leaves only `<save>.old`, which
  `load_city` (`:113`) never opens. Fix: fall back to `.old`.
- `shoot` (`city_core_strike.cpp:15-28`) changes the city outside the journal, then a burst calls the undoable `strike` in a
  second lock: undo restores the blast, not the shot.
- `step_ticks` (`city_core.cpp:250-252`) and `traffic_hashes` (`city_core_state_probe.cpp:109-119`) tick the live city from
  the main thread; "the clock paused" is a comment (`city_core.h:60`, `city_core_state_probe.cpp:97-98`, the latter above the
  wrong function), not a check. Fix: refuse unless the speed is PAUSED.
- `toggle_lane_link` (`city_core_lanes.cpp:110-118`) reads the links under one lock and edits under another (TOCTOU).
- `set_city_hour` (`city_core_people.cpp:197-201`) re-implements `CityClock::set_hour` (`simulation.cpp:24`).
- `rules_of` (`lab/city_lab.cpp:31-43`) reads keys without `has()`: a missing `budget_fps` gives an infinite budget.

## 3o. `native/src/api`: language, source-game ties, stale and misplaced text

- Portuguese error text: `blast_loader.cpp:18,26,40,42,45`; `body_doc.cpp:68,72,101,164,175,176,188,210,235`;
  `buildings_loader.cpp:44,51,73,96,97,138,141,155,161,183`; `catalog_loader.cpp:23,30,42,67,75,120,129,134,224`;
  `city_core.cpp:72,76`; `furniture_loader.cpp:22,28,33,99`; `interiors_loader.cpp:37,45,52,53,54,64,78,83,89`;
  `models_loader.cpp:47,85,170,178,215,218`; `nature_loader.cpp:18,71,96,148`; `traffic_loader.cpp:23,28,40,48,51`;
  comments `city_core_edit.cpp:132` ("Controle"), `city_core_road_spec.cpp:1` ("Personalizar, Meus modelos"); data identities
  `buildings_loader.cpp:25-26` ("sobrado"), `nature_loader.cpp:127-128` (formation names).
- Source-game code as the reason or the oracle (R11): `city_core_flow_probe.cpp:1-2` (flow_replay checks the translated
  FlowControl against `call40.mjs` on the source game), `city_core_grade_probe.cpp:1-3,19-22` (scenes from its
  `elevation.spec.ts`), `city_core_query.cpp:216` (`body_census` "to hold a city against the 4.0's records"), `:178`,
  `lab/diagnosis.cpp:1-2,13,23`, `lab/diagnosis.h:1` (the F7 lab is a port of its doctor), `body_doc.h:1` (reads "a building
  of a Roadcraft city", camelCase vocabulary kept in `function_names.h:14-21`, `interiors_loader.cpp:21-23`),
  `city_core.cpp:39`, `city_core_handles.cpp:1`, `city_core_inspect.cpp:2`, `city_core_lanes.cpp:1`, `city_core_lots.cpp:103`,
  `city_core_models.cpp:63`, `city_core_probe.cpp:19`, `city_core_sight.cpp:2`, `city_street.h:34`, `city_probes.h:47`.
- Parked features in code: `catalog_loader.cpp:79` and `nature_loader.cpp:123` ("until the style lives in the CityState"),
  `traffic_loader.cpp:47-48` (left-hand traffic "por enquanto"): they go to `log/pendencias.md`.
- Misplaced or stale comments: `city_core.h:45` (no "crowd"), `:102` (damage_probe's comment on shot_targets), `:153`
  (road_profiles' comment on load_city); `city_core_parts.h:32-34` (InspectCache's comment above SolidsKey);
  `city_core_jobs.cpp:18` (the pool comment above bay_census); `city_core_query.cpp:114-116` (snapshot's comment above
  map_revision); `city_core_state_probe.cpp:97-98`; `catalog_loader.h:15` (no blast, nature); `city_probes.h:1-3` (five of
  nine files); `city_core_probe_render.cpp:1-2` (three of five probes), `:10-16` (includes after `using`, `<algorithm>`
  twice); `city_core_query.cpp:177` (an `if` at column 0); `city_core.h:1` gate id `UM_DONO_DO_CORE` (Portuguese).
- Bare numbers and local constants: `city_core_commands.cpp:22` (pi), `interiors_loader.cpp:28` (pi/2),
  `buildings_loader.cpp:155,167`, `catalog_loader.cpp:184`, `city_core_inspect.cpp:105`, `city_core_flow_probe.cpp:76`,
  `city_core_probe.cpp:165`, `city_core_survey.cpp:18` (zone 2), `city_core_handles.cpp:22` (interface sizes),
  `city_core_park_probe.cpp:313` (the longest parked car assumed to be the pickup), `city_core_parts.h:27` (save states 0..3);
  the RGBA→RGB shift typed again (`buildings_loader.cpp:20`, `city_core_body.cpp:21`, `traffic_loader.cpp:18`) and a
  `#rrggbb` writer by hand (`city_core_body.cpp:283`); geometry copied again (`city_core_clearance_probe.cpp:31-42`,
  `city_core_query.cpp:38-46`); FNV again (`city_core_state_probe.cpp:29-36`).
- Probe cost under the locks (tests only, but they stall the sim while they run): O(n²) scans in
  `city_core_people.cpp:64-87`, `city_core_probe.cpp:83-102`, `city_core_clearance_probe.cpp:191-196`,
  `city_core_watch_probe.cpp:23-34`; furniture derived for every segment (`city_core_probe.cpp:105-117`,
  `city_core_street.cpp:152-176`, R3).

## 3p. `project/` read in full (game 13, ui 11, render 18, data 31, tests 31, assets 15 chunks; config; addons): root causes

Each item below is a design decision that produces many of the findings listed per chunk in §3q-§3v. Fixing the cause
removes the symptoms named under it; the per-chunk lists give file, line, defect and fix.

- **P1. The source game's code is the reference, against the rule "look and features only".** The style id "roadcraft"
  is typed in six places (`data_tables.gd:65`, `catalog_loader.cpp:80`, `nature_loader.cpp:124`, `region_table.h:10`,
  `traffic_params.h:87`, `start.json:4`) and its blocks hold the 4.0's code values (`street_furniture.json:170-193`,
  `streets.json:29`, `traffic.json:384`, `buildings.json:4`, `models.json:4`, `interiors.json:4`); the sourced blocks
  ("brasil", "br_norm") are never played. Tests pass by matching the 4.0's output (`city_mass_test`, `flow_control_test`
  ORACLE from `call40.mjs`, `lab_test`, `light_color_test` GRADE_TABLE, `road_grade_test`, `road_grade_seam_cases`,
  `road_money_cases`, `road_edit_test` URBAN_PRICE, `people_test`). Shaders copy its constants (flags, puffs, car doors).
  The 38 landmark lots (`assets/lots/*.sig` and `*.json`) and the people (`assets/people/LICENSES.json:2`) are cooked from
  the 4.0's files; `capture_pair.gd` stands the 4.0's town in this game through `doc_body`; the visual tooling
  (`look.gd`, `look_measure.gd`, `pair_camera.gd`) is a port of the 4.0's `look.mjs`. Fix: the style in `CityState`,
  one overlay owner, the "roadcraft" blocks deleted; tests with this game's own thresholds; assets cooked from their
  upstream sources or this game's designs.
- **P2. `StoreApi` mixes commands and queries and decides the snapshot mark by a name prefix** (`store_api.gd:47-68`,
  `city_store.gd:95-98`). State changes travel the query route without a mark or an undo boundary (`road_spec_make`,
  `set_city_hour`, `probe_set_money`, `flow_push`, `wear_paint`, `transit_xray`, `stage_*`, `look_twins`); render
  settings travel the command route and rebuild the snapshot (`set_lamp_night` every frame at dusk, `sky_view.gd:124-126`).
  Fix: three kinds (command, query, render setting), each name filed by what it does.
- **P3. C++ enums and constants are mirrored by hand** in GDScript, shaders, data and tests, because `constants()`
  (`city_core_query.cpp:93-104`) gives only two speeds, zones, minutes and layers. Wrong copies already exist:
  `capture_demolition.gd:28` NORMAL_SPEED 2 is FAST; `shoot_test.gd:24` 14 shots against the data's 12. Fix: the core
  exposes every enum by name; scripts and shaders read them (shader constants through uniforms or a generated include).
- **P4. Contracts by position or key order**: `surface_means.gd:18-21` lane tones by `streets.json` key order against
  `RoadClass`; the 18 element kinds ordered in four files; marking styles by index (`streets.json:29`); profiles by
  catalog position in tests; enum numbers in UI data (`builder.json`, `drawers.json`, `land_panels.json`). Fix: keyed
  rows resolved by name at load.
- **P5. Core queries under the map lock on every frame and every pointer move**: the camera about seven per frame
  (`city_camera.gd:241-259`), the tools' hovers (`building_tool.gd:71-82`, `lots_tool.gd:184-249`, `road_tool.gd:215`),
  and exclusive locks on hover (`roundabout_tool.gd:28-29`, `split_tool.gd:16-17`, `upgrade_tool.gd:22-23`, `lot_tool.gd:34`
  through `tool_base.gd:65-70`). Fix: with the lock design of §3a (L1), one frame query per consumer and previews that
  never take the exclusive lock.
- **P6. One value, two owners**: kerb height (`traffic.json:137` / `streets.json` heights.curb), train car length
  (`traffic.json:241` 18 m / `transit.json:7` 17.5 m), stay hours (`traffic.json:133-134` / `:369-370`), the cut-walls
  setting (`Settings` / `CityState`), zone names and colours, currency, the cycle-lane colour (`tokens.json:213` green /
  `streets.json:6` red), the fireball law (`effects.json:4` / `tuning.json` strike), window glow, finish tables, building
  module sizes (`models.json` / `tuning.json`). Fix: one owner each, the others read it.
- **P7. Three colour rules in the shaders** (`color_space.gdshaderinc`, `building_colour.gdshaderinc`,
  `vegetation.gdshaderinc:51-58`) that disagree on Compatibility: colours depend on the renderer. Fix: one include.
- **P8. Dead data and text kept alive**: 905 of 2,559 translation keys reached by no code; `toolbar.json` kept by its
  schema; dead blocks in `palette.json`; dead keys kept "while the schema asks them" (`streets.json:317`); unreachable
  profiles ("antigos"); `signal_lenses`/`signal_plan`; the Terrain3D editor scripts (plugin not enabled) and 14 binaries
  for platforms never shipped. Fix: delete; a gate that fails on an unreached key or table.
- **P9. The tests have no harness and no control of the clock**: `_check` copied into 34 scripts with five output
  formats; 47 of 57 `new_map` calls start the clock before the test holds it; `step_ticks` beside the running sim thread
  (`inspector_test.gd:29-30`); traffic counted in frames, not ticks; a missing `await` (`transit_test.gd:35`) that skips
  the save checks; a loosened check (`edit_fuzz_test.gd:28-29,88,91`); tests on private members; data counts pinned.
- **P10. Portuguese text in the repository**: errors and asserts (`data_tables.gd:92`, `game_flow.gd:49`,
  `city_store.gd:54,93,103`, `body_doc.cpp:68-235`), save file names and extensions (`save_slots.gd:11-19` ".cidade",
  "cidade-", "mapa-", "quarentena"; ".modelo"; ".via"), gate ids, test case and capture names throughout `project/tests`.
- **P11. The lot documents are in the source's 0.4 m unit and are read as metres** (`body_doc.cpp:159-162,189-194,214-215`;
  no factor in `fit_plan` or `place_body`): every landmark body is 2.5 times too big, and the build limits were widened
  to let it through (`tuning.json` build max_storey_m 9, max_ground_m 20; `city_core_models.cpp:64-67` clamps "as the
  4.0 reads it").


## 3q. `project/game` (13 chunks): findings by chunk

#### game chunk 1: camera_ahead, camera_aim, camera_damp, camera_gestures, camera_motion
- camera_ahead.gd:24-26,33,74 the deocclusion stands on api eye_free, which R2 breaks (§3n: an eye in a building is "in the open" unless the lot centre is within 1 m): the camera's "never inside a building" (:23) cannot hold. It also asks eye_free twice a frame (settle -> clear_of_buildings, then coming() -> clear_of_buildings), each a shared map lock and a solids query.
- camera_gestures.gd:128 four statements on one line with `;`; :162 a bare 0.5.
#### game chunk 2: camera_profile, camera_pull, camera_queries, camera_shake, city_camera
- SOURCE CITES: camera_pull.gd:2 "(Roadcraft isoViewport settle ...)" and city_camera.gd:208 "(the 4.0's sceneHeightAt)" (camera_pull.gd:3 "Deoccluder" is Cinemachine's name, not a source cite).
- THE CAMERA TAKES THE MAP LOCK ABOUT SEVEN TIMES A FRAME: city_camera.gd:241-259 _settle asks map_revision (camera_queries.gd:15), raycast for the focus (:243 seen_height), sight_pull (camera_pull.gd:31), raycast for the eye (:252), eye_free twice (camera_ahead.gd settle + coming) and height_at/raycast for the look-ahead base; camera_shake.gd:16 asks effects_shake; city_camera.gd:216-217 ground_below is asked again by daylight and weather. Each core query takes map_mutex shared (api): with a queued command behind a mesh job (L1), each can wait. The cache (camera_queries.gd) only helps a view at rest. Fix with L1; one camera frame query that answers all of a settle's questions under one lock.
- camera_pull.gd:31 sight_pull is also broken by R2 (§3n).
#### game chunk 3: data_tables, build_stamp, game_flow, game_keys, health_log
- FILES SPLIT TO FIT A SIZE (forbidden by CLAUDE.md "Divide-se por responsabilidade, nunca para caber num limite"): data_tables.gd:16-22 road_profiles split into road_profiles_a/b.json "only to keep each file small"; :205-206 strings.csv split into six CSVs "only to keep each file small"; :180-181 land_panels/land_elements "kept apart for the size of the file". Fix: one file each, or a split by responsibility.
- THE STYLE OVERLAY HAS TWO MECHANISMS AND A FIFTH OWNER OF "roadcraft": data_tables.gd:63-65 REGIONAL_STYLE := "roadcraft" (GDScript), :68 STYLED lays styles.<id> over two tables (street_furniture, streets) in GDScript, while the C++ loaders lay it over the others themselves (region_table(), catalog_loader STYLE, nature_loader STYLE, traffic_params REGIONAL_STYLE). One table styled here, the next in C++. Fix: one overlay owner (DataTables, before the tables reach the core) reading the city's style.
- data_tables.gd:63 cites "session-rules.md, section 10" (a process file); :1 gate id DADOS_POR_UM_LEITOR (Portuguese).
- data_tables.gd:92, :210, :222 and game_flow.gd:49 errors in Portuguese ("JSON inválido", "strings.csv ausente", "strings ausente", "transição inválida").
- game_flow.gd:37 "(Roadcraft saveOnClose)"; :73-75 quit_now sets the state around TRANSITIONS (the header :2 says "Every transition is validated").
- health_log.gd:10 COST_PARTS names must follow api render_cost's keys by hand (city_core_probe_render.cpp:30-66); :146 a const at the end of the file.
- data_tables.gd:157-162 road_profiles() parses three files again on every call (the road catalog's strips).
#### game chunk 4: interior_controller, lab/cpu_watch, doctor_run, frame_capture, frame_meter
- interior_controller.gd:142-147 _building_at asks api `inspect` (nearest lot centre within 30 m, R2, §3n): a double click on a large building opens a neighbour.
- interior_controller.gd:41 warning in Portuguese ("sem HUD, a barra do andar não aparece").
- SOURCE CITES IN THE GAME SCRIPTS: interior_controller.gd:3 ("As in the 4.0 (buildingsWiring.ts ...)"), lab/cpu_watch.gd:1 ("the 4.0's perfil de CPU": Portuguese and source), doctor_run.gd:1 ("the 4.0's doctor/measure.ts"), frame_capture.gd:1 ("the 4.0's Frame tab"), frame_meter.gd:6 ("look.mjs frameStats").
- frame_meter.gd:166-168 render_cpu_ms asks render_cost every recorded frame, and cpu_watch.gd:26 again (two core queries per frame while the lab records).
#### game chunk 5: lab_recorder, pass_clock, resource_watch, play.gd/.tscn, frame_governor, quality
- lab/lab_recorder.gd:1 "(the 4.0's lab/recorder)", resource_watch.gd:1 "(the 4.0's lab/resources/diagnostics)": the whole F7 lab (recorder, resources, frame tab, CPU tab, doctor) is laid out after the source game's lab.
- play.gd:18 error in Portuguese ("Falha ao iniciar"); :23 print("cidade: ...") (Portuguese output); :55 any user argument at all skips the title screen (`get_cmdline_user_args().size() > 0`), not only the test flags.
- frame_governor.gd:12-15 _at_percentile indexes sorted[floor(size * p)]: p = 1.0 in data reads past the end (no clamp).
#### game chunk 6: render_switches, road_prefs, settings, store/city_saves
- THE "CUT WALLS" SETTING HAS TWO OWNERS: settings.gd:108-111 Settings.road.walls (kept in settings.cfg, road_prefs.gd:9,18,25) and the core's CityState.new_road_flags SEG_CUT_WALLS (api city_core_street.cpp:118-126, set under the exclusive lock). Two stores of one tool choice, kept equal by whoever remembers to send it. With the api finding (tool state does not belong in CityState): one owner, the tool, sent with each road command.
- SOURCE-GAME KEYS AND CITES: render_switches.gd:7 "bShadows" (the source's effects-panel key, as building_renderer_commit.cpp:175 "bFlags"); settings.gd:2-3 "(Roadcraft editor/snap.ts RoadSnap)", :147 "(main.ts showLots)"; road_prefs.gd:10 "(Roadcraft toolChoices blockGridChoice)"; city_saves.gd:18 "(Roadcraft persistence.ts saveSoon)", :119, :138 "(Roadcraft quarantineStored)", :163 "as in the reference".
- city_saves.gd:54-56 map copies are named "mapa-<seed>" (a Portuguese file name; to confirm in SaveSlots.export_slot).
- city_saves.gd:81-92 load_game calls the core's load_city, which decodes on the main thread (§3n: the frame freezes while a city opens); :113-114 save_on_close spins OS.delay_msec(10) on the main thread until the save thread ends (acceptable at close).
#### game chunk 7: city_store, formatters, lane_links_edit, lot_projects, model_library, road_models, road_section_state, road_spec_edit, save_slots, saved_entries
- THE CITY STYLE IS A HALF FEATURE: the city style is not wholly dead: formatters.gd:38-42 Formatters.style() reads the snapshot's style for the interface's units and currency (start.json styles). So a chosen style changes units and money shown, while every C++ table and the GDScript overlay stay "roadcraft": a half feature.
- formatters.gd:27-30 the decimal comma is chosen by `get_locale().begins_with("pt")` in code while the thousands separator comes from a translation key (economy.group, :20): two mechanisms for one locale rule. Fix: the decimal separator from a translation key too.
- PORTUGUESE FILE NAMES AND EXTENSIONS (language rule): save_slots.gd:11-19 ".cidade", "cidade-%d", "mapa-", "cidades.cfg", "quarentena"; model_library.gd:8 ".modelo"; road_models.gd:8 ".via". (Changing them needs a migration of existing saves: note it in the fix.)
- lot_projects.gd:1 "The lots of the Roadcraft's lot lab (lotLibrary.ts)": the 38 assets/lots/*.json are the source game's lot-lab designs (asset provenance, with signature_bodies.h:1); :9-10 USE_ZONES (use x density -> zone id) is a table in code (data rule); :29 error in Portuguese ("lote %s recusado").
- city_store.gd:54,93,103 assert messages in Portuguese ("área desconhecida", "comando desconhecido", "consulta desconhecida"); :3 gate id ESTADO_NA_UI (Portuguese); :126 set_sim_switch's comment lists three systems, "crowd" missing; :95-98 whether a command marks the snapshot and reports failure is decided by the name prefix "preview".
- road_models.gd:1 "(Meus modelos)", road_spec_edit.gd:1 "Personalizar...": Portuguese UI names in comments; formatters.gd:33 "(Roadcraft crossSection.ts metresText)".
- formatters.gd:105 key "STAT_DATE" (upper case) beside dotted lower-case keys elsewhere.
#### game chunk 8: session, store_api, tools/barrier_tool, body_edits, brush_defaults, brush_tool
- COMMANDS AND QUERIES ARE MIXED, SO THE SNAPSHOT GOES STALE: store_api.gd:47-68 QUERIES (no _mark, no failure report: city_store.gd:102-104) holds calls that change state: road_spec_make (appends a profile to the Catalog, :48), probe_set_money and set_city_hour (:63), flow_push and traffic_hashes (step the live traffic, :50, :67), draw_verges and wear_paint (:51, render), stage_show/stage_move/stage_free (:56), transit_xray/transit_preview (:61), show_people_clip (:63), look_twins (exclusive lock, writes a cache). COMMANDS holds previews (preview_road, preview_road_path :12; preview_pole_line -> show_pole_preview :45), which city_store.gd:95 recognises only by the "preview" prefix. Fix: each name in the table it belongs to, by what it does (state change or not), not by its prefix.
- THE CITY'S STYLE IS NOT PART OF THE CITY: session.gd:23-24,62-63 keeps style and marking_style in <slot>.cfg beside the save (not in it): a city opened from a file elsewhere (city_saves.gd:128 open_file) comes back in the default style.
- barrier_tool.gd:1 "Muros (Roadcraft editor/barriers.ts)" and :75 "(Roadcraft barriers.ts)"; :33 a tab character in the middle of an expression line (a broken continuation).
- body_edits.gd:18 body_add_setback(id, vol, -1.0, 2) and :32 bevel corner -1: sentinels and counts typed bare; :13 asks body_info (the generator, R1) to read a storey count before the edit runs the generator again.
- brush_tool.gd:8-11 STEP_SHARE, MIN_STEP_M, MAX_STEPS: tool tunables in code (data rule).
#### game chunk 9: build_hands, building_tool, cloud_tool, control_tool, demolish_tool, element_tool, fog_tool
- THE JUNCTION CONTROL COUNT IS OWNED BY THE HUD'S DATA: control_tool.gd:15-17 cycles the control by posmod over DataTables.ui("hud").control_marks.size(); the core's control enum is the owner of how many controls there are. A hud.json with another count cycles to a control the core does not have (or skips one). Fix: the core's constants give the count; the marks table is checked against it.
- PER-POINTER-MOVE CORE CALLS: building_tool.gd:71-82 hover with a model in hand asks place_fit, height_at, stage_move and place_check each move (three map locks and a placement check against the city); demolish_tool.gd:27-28 asks height_at five times per drag move; cloud_tool.gd:53-57,92 asks the whole `landscape` dictionary (weather, every cloud, counts) to find one cloud, every drag move.
- building_tool.gd:89 the plain quarter turn is PI / 2 in code while the Shift turn is data (build_gear_turn_deg): one rule, two places.
- cloud_tool.gd:10-11 YAW_PER_M_X / YAW_PER_M_Z: a cloud's yaw from its position by two constants in code; element_tool.gd:6-7 ERASE_KIND / ERASE_ALL mode codes that must follow land_panels.json by hand.
- building_tool.gd:1 "(Massa, Fachada, Telhado, Cores)", control_tool.gd:1-2 "(tab Controle) ... (Roadcraft main.ts cycleNodeControl)", demolish_tool.gd:40 "(Roadcraft bulldozer cancel)".
#### game chunk 10: furnish_hand, handle_drag, inspect_tool, lot_proposals, lot_tool
- THE INTERIOR HAND RUNS TWO GENERATORS PER POINTER MOVE: furnish_hand.gd:86-92 hover (every move with a piece in hand) -> :89 _on_floor -> :127 body_floor_y (api: record_body) and :122 _level() -> the `interior` query, which api builds with interior_floors and render interior_lamps (record_mass + the Interior generator, city_core_interior.cpp:40-43; city_core.h:179 says "not for every frame"); plus :90 body_piece_size. click(), turn() ask `interior` again. Fix: the hand keeps the level and floor height it read when the floor opened (InteriorController already holds _state with level), and the core caches derived bodies (R1).
- furnish_hand.gd:131 and handle_drag.gd's callers: _on_floor divides by dir.y with no zero test (a level ray sends an infinite point to the core); handle_drag.gd:240 tests it.
- lot_proposals.gd:5 CORE_CELL_M = 256.0 "core/grid.h CHUNK": a C++ constant typed again in GDScript (the core's constants() should carry it).
- handle_drag.gd:6 TURN_STEP = PI / 12 (a snap step in code; build_gear_turn_deg is data); :82 body_info on every press (R1).
- furnish_hand.gd:1 "Construir › Interior, as in The Sims", :52 "(Roadcraft openInside, seeInside)"; handle_drag.gd:1 "(the 4.0's beginHandleDrag and pointerMove)"; lot_tool.gd:1 "(Build › Desenhar)".
- inspect_tool.gd:16-17 asks `inspect` every pointer move (a shared lock and two spatial searches).
#### game chunk 11: lots_tool, mass_drag, move_tool, paint_brush, paint_tool, pole_tool, reference_hand, road_blocks
- C++ ENUMS MIRRORED BY HAND IN GDSCRIPT: lots_tool.gd:6-9 CUT_ACROSS/ALONG/DRAWN ("as the core takes them (LotCutKind)"); mass_drag.gd:6-8 WING = 0, SLIDE = 3; element_tool.gd:6-7 (chunk 9); lot_proposals.gd:5 CHUNK. The core's constants() exposes only speeds, zones, minutes and layers (city_core_query.cpp:93-104): every other enum is typed again. Fix: constants() carries each enum the script uses, by name.
- PER-POINTER-MOVE CORE CALLS: lots_tool.gd:184-199 drag in "add" asks lot_land, lot_snap_point and lot_angle_at; :232-249 hover asks lot_at up to twice, lot_snap_point and lot_cut_lines; mass_drag.gd:52,68 asks body_mass_rect and, while sliding, body_outline (record_body, R1) each move; move_tool.gd:19 node_at each hover.
- pole_tool.gd:77-78 release plans the run twice (preview_pole_run, then build_pole_run plans it again in the core) only to learn where the last pole stands: the build result should carry it.
- road_blocks.gd:13-19 takes `tool: RefCounted` and calls tool.drawn and the private tool._clear_preview(): duck-typed reach into another class's privates.
- lots_tool.gd:11-16 TAKE_PX, CATCH_PX, SNAP_LEAST_M, MARK_M: interface tunables in code.
- SOURCE CITES AND PORTUGUESE UI NAMES: lots_tool.gd:1 "(Roadcraft editor/lotTool.ts)"; mass_drag.gd:1 "Massa › Ala, Empilhar e Recortar"; move_tool.gd:1 "(tab Mover) ... (Roadcraft editor/nodeMover.ts)"; paint_brush.gd:1 "Construir › Cores ... Pintar ... Copiar estilo"; pole_tool.gd:1 "Postes (Roadcraft editor/poles.ts)", :88 "(Roadcraft main.ts gestureInProgress ...)"; reference_hand.gd:1 "Construir › Massa › Referência 3D ... (the 4.0's loadReference and buildReference)".
#### game chunk 12: road_draft, road_keys, road_tool, roundabout_tool, run_hand, shoot_tool, split_tool, streetscape_tool, strike_tool
- THE EXCLUSIVE-LOCK PREVIEWS RUN ON EVERY POINTER MOVE (api A2 made concrete): roundabout_tool.gd:28-29 hover -> preview_roundabout (api city_core_edit.cpp:205 unique_lock); split_tool.gd:16-17 hover -> road_under (see tool_base) -> segment_at (city_core_edit.cpp:218 unique_lock); lot_tool.gd:34 drag -> preview_lot (:266 unique_lock). Each move waits for every mesh job holding the map shared (L1), then stops the sim tick and the frame.
- THE ROAD DRAG REBUILDS THE WHOLE PROFILE LIST EVERY MOVE: road_tool.gd:215 _preview -> road_draft.gd:31 RoadDraft.paint -> CityStore.query(&"road_profiles") (api city_core_commands.cpp:224-250 builds a Dictionary per profile, variants included) to read one profile's "painted" and "yellow"; road_tool.gd:55 does the same. Per move the road tool also asks road_end, road_pieces and preview_road_path (a grade plan): four map locks. Fix: CityStore.cached road_profiles by catalog revision.
- road_tool.gd:43-44 the tool reads its own height from the core (road_height: CityState.new_road_lift) and pushes Settings.road.walls into the core on entry: the two-owner setting (chunk 6) synchronised by hand; :38 every entry dispatches road_variant (a Catalog append outside the journal).
- road_tool.gd:12 the comment "A long stroke keeps every sample up to DENSE_SAMPLES, then one a metre, up to MOST_SAMPLES" stands above WHOLE_M; the constants live in road_draft.gd:5-7.
- road_keys.gd:16-17 static var waiting / note: interface state held in static variables of a key table.
- roundabout_tool.gd:7 SCALE_STEP = 1.2 (a tunable in code); run_hand.gd:62 spec.size[1] unchecked.
- SOURCE CITES AND PORTUGUESE UI NAMES: road_draft.gd:37 "(Roadcraft main.ts drawRoadPreview)" with "R$ 150.000"; road_keys.gd:1 "(Roadcraft ui/roads/keys.ts)"; road_tool.gd:1 "(Roadcraft editor/roadTool.ts)", :48, :157 "(roadTool.ts ...)"; roundabout_tool.gd:1 "(tab Rotatória)"; run_hand.gd:1 "Construir › Peças › Traçados"; split_tool.gd:1 "(tab Dividir)"; streetscape_tool.gd:1 "Peças da rua (Roadcraft editor/streetscapeTool.ts)".
#### game chunk 13: terrain_tool, tool_base, tool_controller, transit_tool, tree_tool, upgrade_tool, weather_tool, zone_tool
- tool_base.gd:65-70 road_under asks segment_at (api: unique_lock) and split_tool.gd:16 / upgrade_tool.gd:22-23 call it from hover: an exclusive map lock on every pointer move with those tools (A2 confirmed).
- tool_controller.gd:12 the hint signal's contract is "text already translated"; lots_tool.gd:36 ("zone.lots.help") and road_blocks.gd:18 ("tools.toast.blocksBuilt", "hint.road.invalid") emit raw keys: unless the HUD translates (to confirm in ui), the player reads the key.
- tool_controller.gd:151,159 every mouse event asks the core for a raycast (ground_at, a shared lock), before the tool's own hover queries.
- tool_controller.gd:3-4 the header's list of tools lacks LOTS (enum :22-23); :134-136 step_road_height's comment ("A step of the height ...") stands above another comment and a signal declared in the middle of the file (options_changed), and step_road_height has none.
- transit_tool.gd:7-8 MODE_BUS / MODE_TRAIN mirror the core's TransitMode by hand; :72 asks the whole transit_plan to read one stop's mode; :22,29-30,87,97 transit_xray / transit_preview change render state through queries (chunk 8).
- upgrade_tool.gd:12 asks the whole road_profiles list for one width.
- terrain_tool.gd:8 DAB_S = 0.1, zone_tool.gd:6,9 DAB_SPACING, MOST_DABS, :15,22 zone 1 by default: tunables and a zone code in code.
- SOURCE CITES AND PORTUGUESE: terrain_tool.gd:3 "(Roadcraft editor/terrainBrush.ts tick, `applied = now`)"; tool_base.gd:50 "(Roadcraft main.ts gestureInProgress)", :78 "Não dá para construir aqui"; upgrade_tool.gd:1 "(tab Melhorar)"; zone_tool.gd:5,7,12 "(zoneTool ...)", "(lotTool.ts move)", "(lotTool.ts `remove`)".

## 3r. `project/ui` (11 chunks): findings by chunk

#### ui chunk 1: approach_panel, builder_strip, compass, connector_panel, drawer_strips, effects_panel
- connector_panel.gd:89-90 after toggle_lane_link the panel re-reads the lanes after a fixed delay (get_tree().create_timer(HudWidgets.px("cn_reread_s"))): a guessed wait for the traffic's lane rebuild instead of a revision to watch; and a time in seconds read through px() (the pixel token reader).
- approach_panel.gd:6 RULES (priority 0, yield 2, stop 1, "road_network.h SegRule"): a C++ enum mirrored by hand.
- compass.gd:7-13 and effects_panel.gd:11-20 put @onready on plain initial values (Vector2.ZERO, false, 0.0, ResourceWatch.new()): @onready has no meaning there (misused annotation).
- builder_strip.gd:6-7 CHOICES (which "do" values stay lit) is a table in code beside builder.json (data rule).
- approach_panel.gd:1 "(Roadcraft ui/roads/junctionPanel.ts)", connector_panel.gd:1 "(Roadcraft ui/roads/connectorPanel.ts)".
#### ui chunk 2: graphics_panel, health_panel, hud_actions, hud_dialog
- TEXT SHOWN WITHOUT tr() (CLAUDE.md "textos só por tr"): health_panel.gd:133-139 the State tab writes raw snapshot keys and English labels ("ready", "speed", ..., "tier: %s (%s)", "fps: %d").
- hud_actions.gd:7 TOWN_MAP = 0 typed again (play.gd:5); :9 "(Roadcraft window.confirm)", :102 "(Roadcraft showImpossible)".
- @onready on plain values again: health_panel.gd:9-15, hud_actions.gd:12, hud_dialog.gd:8-9.
#### ui chunk 3: hud_drawer, hud_messages, hud_popover, hud_switch, hud_tip
- TWO CONVENTIONS FOR ONE SIGNAL: hud_messages.gd:31-39 sets the Label's text as received; Godot's Label passes its text through atr() when auto-translate is on (the default), so the raw keys of lots_tool.gd:36 and road_blocks.gd:18 still show translated. Not a visible bug; an inconsistency against tool_controller.gd:12 ("text already translated") — two conventions for one signal.
- SAVE FILE NAMES ARE SHOWN TO THE PLAYER: hud_popover.gd:129-131 lists each save by its slot name ("cidade-<seed>", "mapa-<seed>") in every language: the Portuguese file names of save_slots.gd are player-visible text outside i18n.
- STRING-BUILT METHOD CALLS FROM DATA: hud_popover.gd:45 call("_build_" + which) and :143 call("_seg_" + r.seg): a popover or row named in drawers.json with no matching method fails at run time, unchecked by any gate.
- TOKENS READ AS THE WRONG KIND: hud_tip.gd:27 HudWidgets.px("tip_wait_ms") (milliseconds through the pixel reader), as connector_panel.gd:90 ("cn_reread_s"): times stored and read as pixel tokens.
- hud_popover.gd:40 cites ".v2-pop-body.icons" (the source game's CSS class), hud_tip.gd:1 "(shell.ts tip)", hud_messages.gd:2 "as in the reference".
- hud_popover.gd:240 "X %d · Y %d" written without tr(); hud_drawer.gd:135 the keys that do not light a card are a list in code.
- @onready on plain values again: hud_drawer.gd:17-29, hud_messages.gd:6-9, hud_popover.gd:16-21, hud_tip.gd:6-8.
#### ui chunk 4: hud_top_bar, hud_widgets, inspector_panel
- (checked) hud_top_bar.gd:11 SPEEDS sends 0..3; simulation.h:15 SimSpeed is PAUSED = 0, NORMAL = 1, FAST = 2, FASTEST = 3, STOPPED = 4: the values match, but they are typed by hand (constants() gives only speed_paused and speed_fastest), and the labels "1×", "2×", "4×" are multipliers written in the interface, not read from the core's speeds. (CLAUDE.md lists SimSpeed "STOPPED, PAUSED, NORMAL, FAST, FASTEST", not the enum's order.)
- hud_top_bar.gd:112 the stats tip's "speed" is always "—": a field shown and never filled.
- inspector_panel.gd:277 the inspector's refresh period is DataTables.game("health").refresh_s: a constant borrowed from the health monitor's table.
- inspector_panel.gd:4 parks features in a comment ("what the road's height, curve, parking ... offer in the reference are the sessions road-elevation, roads and traffic's"), :37 "(.inspect-grid)" (the source's CSS class); :89-90 asks the whole road_profiles list for the class dropdown.
- hud_widgets.gd:37 comment "<name> · em construção" (Portuguese).
- @onready on plain values again: hud_top_bar.gd:13-22, inspector_panel.gd:14-17.
#### ui chunk 5: inspector_rows, interior_bar, junction_marks, junction_panel, lab_doctor, lab_tools
- (checked) data/ui/hud.json control_marks has 7 rows = road_network.h:63 NodeControl CTL_COUNT (7), and inspector_rows.gd:6 CONTROL_KEYS 3 = lane_graph_types.h:14 Control: equal today, kept equal by hand (control_tool.gd, junction_marks.gd:114 index the marks by the core's value). No gate compares them.
- inspector_rows.gd:2 "data/game/health.json is the panels' pace": the panels' refresh lives in the health monitor's table (a shared constant filed under another owner's name); :9 CLEARING = 1 mirrors the lamp value.
- junction_marks.gd:1-2 "(Roadcraft main.ts)", "the Controle tab"; lab_doctor.gd:1 "(the 4.0's doctor/index.ts)".
#### ui chunk 6: lab_views, land_hands, land_strips, lot_strip, model_shelf
- A SAVED MODEL IS ANNOUNCED AS A REFUSAL: model_shelf.gd:171 `tools.hint.emit(tr("building.blueprintSaved" if saved else "building.blueprintNotSaved"), false)`: ok is false on success too, so "saved" is drawn as a danger toast. Fix: pass `saved`.
- THE FRAME SECTIONS HAVE TWO LISTS THAT DISAGREE: lab_views.gd:9 CORE_SECTIONS (agents, stream, levels, flush, roads, props, buildings) vs health_log.gd:10 COST_PARTS (lock, stream, levels, flush, roads, props, buildings, trees, agents): the F7 CPU tab never shows "lock" or "trees" while the health log blames them; both copy api render_cost's keys by hand. One owner: the core lists its sections.
- TEXT SURGERY ON TRANSLATIONS AND UNITS BY DISPLAY TEXT: land_strips.gd:10-11,42-43 cuts every translated name at " (" (LONG_NAME) and recognises metres by the unit string " m" (METRES == sl.unit): behaviour depends on how a translation is written. Fix: a short-name key in i18n, a unit id in data.
- lab_views.gd:69-72 the inspector tab shows raw dictionary keys as labels; :1 "(the 4.0's lab/resources and lab/inspector)"; lot_strip.gd:1 "(Roadcraft shell.ts)".
#### ui chunk 7: pop_bodies, preview_tag, profiler_hud, road_catalog, road_keys_panel
- A PROFILE'S STRIPS HAVE TWO SOURCES: road_catalog.gd:25-30 takes the pictures' strips from DataTables.road_profiles() (the JSON files parsed again) and everything else from the core's road_profiles; a variant the core made (road_variant / road_spec_make) has no strips in the JSON, so its picture is drawn from an empty list. One source: the core's profile (it holds the strips).
- pop_bodies.gd:52-53 _short cuts the translated text at " (" again (land_strips.gd:42-43 is the other copy).
- profiler_hud.gd:43,49 English text written without tr() ("PROFILER %d fps ...", "video %d MiB · texture ..."); :56-62 an inner class HealthMs exists only to hold MS_PER_S.
- CONSTANTS TYPED IN MANY SCRIPTS: MS_PER_S, PERCENT, MIB are declared again file by file (grep: dozens of scripts); one owner (a units script) would hold them.
- pop_bodies.gd:8 KEY_SHARE = 0.42 (layout share in code).
- preview_tag.gd:1 "(Roadcraft main.ts drawRoadPreview)"; road_keys_panel.gd:1 "(Roadcraft ui/roads/keysPanel.ts)"; road_catalog.gd:8 "Personalizar…", :132 "Meus modelos".
- @onready on plain values again: preview_tag.gd:6-7, profiler_hud.gd:8-9, road_catalog.gd:12-21.
#### ui chunk 8: road_options, road_panel, road_profile_panel
- GAME RULES DECIDED IN THE INTERFACE: road_panel.gd:7-17 the curve's limits (CURVE_LEAST_M 20, CURVE_MOST_M 280, CURVE_SHARE 0.65 of the length, CURVE_STRAIGHT_M 0.5, CURVE_FROM/TO 0.15..0.85) and the half-metre height step; road_profile_panel.gd:11-14 the speed limits (SPEED_LEAST 10, SPEED_MOST 130, PACE_BY 10) and the 0.5 m width step. The core gives width limits (road_spec_limits) but not these: the interface clamps by its own numbers and the core may accept or refuse otherwise. Fix: the core's limits (tuning / road_spec_limits) carry them.
- THE TWO-OWNER WALLS SETTING WRITTEN TWICE: road_options.gd:140-142 Settings.set_road_walls(on) then CityStore.dispatch(&"set_road_walls", [on]) (chunk 6 game finding).
- road_options.gd:105-109 the block grid's default merge is a copy of road_blocks.gd:7-9; :59,80 _base() asks the whole road_profiles list twice per refresh (refresh runs on every tool change, height change and option change).
- road_panel.gd:18-19 CROSS_ZEBRA / CROSS_SIGNAL mirror NodeCrossing by hand.
- SOURCE CITES AND PORTUGUESE: road_options.gd:1 "(Roadcraft ui/v2/shell.ts renderRoads)", :78, :104, :129, :151 "(shell.ts ...)"; road_panel.gd:1 "(Roadcraft ui/inspector.ts)", :60 "Editar perfil", :122 "Ponta inicial [balão]"; road_profile_panel.gd:1-2 "Personalizar… (Roadcraft ui/roads/profileEditor.ts) ... Meus modelos".
- road_profile_panel.gd:146,154 and road_panel.gd:68 road_spec_make (a Catalog append) through query (the CQS finding).
#### ui chunk 9: road_section_view, road_spec_ops, road_swatch, roadcraft_hud
- SIZES IN INTERFACE CODE (CLAUDE.md: sizes only through data/ui/tokens.json): road_section_view.gd:10-11 LABEL_LEAST_PX 34, WAY_MARK_PX 7, :81 line width 1.0; road_swatch.gd:54-58 pixel offsets -2, +1, -1 and heights 1, 2.
- THE TWO STRIP VOCABULARIES TRANSLATED IN GDSCRIPT: road_spec_ops.gd:111 {"footway": "sidewalk", "cycle": "bike"} maps the editor's names to the catalog's (api: catalog_loader.cpp:64 vs city_core_road_spec.cpp:16): a third place that knows both.
- road_spec_ops.gd:41-78 where a new element goes (insertion rules) lives in the interface; the core only checks the result (spec_issues). Placement policy for a road section belongs with the rule owner (sim/roads/road_spec).
- road_section_view.gd:1 "(Roadcraft ui/roads/crossSection.ts)"; road_spec_ops.gd:1 "(Roadcraft ui/roads/profileEditor.ts)"; roadcraft_hud.gd:78 "as in the reference", :82 "Editar perfil", :204 "(Roadcraft main.ts escape)".
- @onready on plain values again: roadcraft_hud.gd:10-25.
#### ui chunk 10: selection_overlay, start_screen, street_strip, strike_strip, theme/theme_art
- CONFIRMED HALF FEATURE: start_screen.gd:51-56 offers start.json's styles and dispatches set_style; only Formatters' units and money follow it (game chunk 7).
- street_strip.gd:113 cuts translated card names at " (" (the third copy, with land_strips.gd:42-43 and pop_bodies.gd:52-53).
- start_screen.gd:10 LANGUAGES (codes and display names) in code, read by hud_popover.gd:178 too; the languages belong to data/i18n.
- THE SOURCE GAME'S WEB UI AS THE REFERENCE OF THE THEME: theme_art.gd:46 "(.v2-zone-swatch)", :62 "(.v2-start's radial gradient)", hud_popover.gd:40 ".v2-pop-body.icons", inspector_panel.gd:37 "(.inspect-grid)"; street_strip.gd:1 "(Roadcraft shell.ts renderLandscape)", :44 "Roadcraft: Poste (urban or rural) and Luzes (three choices)", :105 "(Roadcraft shell.ts card)"; strike_strip.gd:1 "(shell.ts renderSimple of the 4.0)"; selection_overlay.gd:1 "like the reference's screen layer".
- selection_overlay.gd:66-68 redraws every frame while anything is picked (whether or not the camera moved).
- @onready on plain values again: selection_overlay.gd:9-13, start_screen.gd:13-19.
#### ui chunk 11: theme_builder, theme_text, transit_strip, zone_strip
- theme_builder.gd:180-184 _wide sets content margins from the "card_radius" token: a radius token used as padding; :100 `size("pad_s") + 1.0` a bare pixel.
- THE THEME IMITATES THE SOURCE'S BROWSER: theme_builder.gd:42 "the reference asks for Inter", :47 LCD antialiasing "as the reference's browser draws text", :180 "(.v2-choice padding)"; theme_text.gd:47 "(shell.css .v2-tip)"; :1 gate id UI_SEM_ESTILO_SOLTO (Portuguese).
- transit_strip.gd:5 MODE_KEYS mirrors TransitMode by hand; :27 the RGB -> RGBA packing (`<< 8 | 0xff`) once more; :31 `.replace("{n}", ...)` where every other text uses format({...}).
- zone_strip.gd:6 DENSITIES (0, 1, 2) typed in code beside zones.json's density column.

## 3s. `project/render` scripts and shaders (18 chunks): findings by chunk

#### render (gd) chunk 1: change_marks, city_terrain, city_water, city_world, contact_pass
- A LIST KEPT ONLY TO SATISFY A GATE: city_world.gd:9-23 NATIVE_SHADERS "Kept as text so the orphan gate sees every shader the native renderers load by name from materials.json": a hand-kept copy of materials.json's shader names, used by nothing in the game. Fix: the orphan gate reads materials.json (the owner); delete the list.
- city_world.gd:7 LUMA (0.3, 0.6, 0.1) "the weights terrain_layers.gdshaderinc reads a colour's luma with": a GDScript/shader contract kept by a comment (as render L11).
- DataTables.render() (every render table) is called to read one table: city_world.gd:49, 63, 92, 99, element_effects.gd:14, lamp_radiance.gd:15, 24, 33, surface_means.gd:15 (cache hits at boot; after release_boot_tables each call re-parses all thirteen files). render_table(key) is the owner for one table.
- city_water.gd:8 WATER_EXTENT_M = 70000 (the sea plane's size in code; maps differ in size: data/catalog maps).
- change_marks.gd:39 changes_shown read from data/game/health.json (panel settings filed under the health monitor's table, as inspector_rows).
#### render (gd) chunk 2: daylight, daylight_rig, effect_materials, element_effects, film_look, handle_marks
- A SECOND LIST KEPT FOR THE ORPHAN GATE: effect_materials.gd:7-9 SHADERS "Kept as text so the orphan gate sees the shaders the native effects load by name" (with city_world.gd:9-23).
- daylight.gd:167 follow_view (every frame, city_world.gd:217) asks the core height_at through CityCamera.ground_below: one more map lock per frame.
- THE ELEMENT/EFFECT BOUNDARY IS A RENDER TABLE'S LENGTH: element_effects.gd:33 passes int(_d.props.size()) (data/render/cover.json elements.props) as the first effect kind to effects_near: the simulation's numbering of element kinds is decided by how many prop rows a render table has (api city_core_nature.cpp:186-199).
- element_effects.gd:46-73 a new ParticleProcessMaterial, QuadMesh and StandardMaterial3D each time an emitter changes kind.
- SOURCE CITES: daylight.gd:1 "(Roadcraft render/scene.ts setTimeOfDay)", :75 "as the 4.0's top bar does", :183 "as the 4.0's sunCascades.ts"; daylight_rig.gd:17 "(the 4.0's hemisphere light and sky map)".
- handle_marks.gd:22-24 redraws every frame while any handle exists.
#### render (gd) chunk 3: interior_lights, lamp_radiance, model_studio, occlusion_pass, preview_line
- BEHAVIOUR COPIED FROM SOURCE LINES (R11): interior_lights.gd:2-3 "The 4.0's way (renderer.ts:1620-1713, roomLamps.ts): a fixed set, the first lamps ... one intensity for all, 0.25 by day to 1 at night"; lamp_radiance.gd:25 "the 4.0's faint glow of an unlit lens"; preview_line.gd:1 "(Roadcraft main.ts drawRoadPreview ...)".
- lamp_radiance.gd:5 LENSES = 5 "props.gdshader lit_radiance (prop_kit.h Lens::COUNT)": one count in C++, a shader and GDScript, kept equal by a comment; :11 error in Portuguese ("ToneMap só espelha o ACES; as lentes não mostrariam a sua cor").
- model_studio.gd:16 SETTLE_FRAMES = 3 "(measured: empty at once, right after three frames)": a counted wait found by trial instead of a readiness signal; :8-15 the studio's camera, light and ambient values in code (look values: data rule).
#### render chunk 4: reference_model, road_flash, screen_pass, selection_box, shaders/agent_door.gdshaderinc, agent_glass, agents
- SHADER LOOK COPIED FROM THE SOURCE: agent_door.gdshaderinc:5 DOOR_SWING "(Roadcraft occupants.ts)", :12-17 pane_drop "three cars in ten with one or both open, half of those part-way (Roadcraft agents.ts windowsDown, windowDrop)" with its thresholds 0.9 / 0.8 / 0.7 and drop 0.45 typed in the shader; reference_model.gd:1 "(the 4.0's referenceModel.ts)"; road_flash.gd:1 "(Roadcraft renderer.ts flashRoads, blinkMaterial)".
- C++/SHADER CONTRACTS BY COMMENT (render L11): agent_door.gdshaderinc:3-4 DOOR_FLAG 16, DOOR_LEVELS 31 "(pose_buffer.h LOOK_DOOR_MASK)"; agents.gdshader:33 BRAKE_FLAG 8, :36 paints[24] (the palette size agent_looks.cpp checks).
- THE SIN HASH: agents.gdshader:41-43 pick() is fract(sin(dot(...)) * 43758.5453): its result depends on the GPU's sin precision for large arguments (seed * 251), so which cars get a black roof can differ between graphics cards; a bit-mixing integer hash is exact everywhere. (To check for other shaders using it.)
- Magic numbers in shaders (outside the CLAUDE.md gate's paths but under its data rule): agent_door.gdshaderinc:15-17 (91.7, 37.3, 0.9, 0.8, 0.7, 0.45), agents.gdshader:65-66 (0.04/0.96, 0.7 + 0.5, -0.04..0.06).
- reference_model.gd:183-184 DRAWN_FRAMES = 2 and model_studio's 3: counted frame waits.
- road_flash.gd:5-6 GLOW_IN_S, DIE_OUT_S: look timings in code.
#### render chunk 5: shaders atmosphere, building, building_bay.gdshaderinc, building_body.gdshaderinc
- THE NEAR AND MID LEVELS PAINT REVEALS AND SILLS DIFFERENTLY (certain): the near level (render facade_bay.cpp:13,72-75) shades a reveal trim x 0.68 and a sill trim x 0.92; the mid level's bay shader (building_bay.gdshaderinc:12,37-39) shades them wall x 0.82 and wall x 1.18. Other base colour, other factors: at the near/mid hand-over every reveal and sill changes tone. Two owners of one look (with ROOF_EDGE_H etc., render §3j). Fix: one table of bay shades, read by the C++ near level and passed to the shader (uniforms) for the mid level.
- C++/SHADER CONTRACTS BY COMMENT: building_bay.gdshaderinc:4-10 (BAY_MESH 1.5 "bay_kit.h BAY_MESH_MARK", packing shifts, DEPTH_STEP_M "shell.cpp BAY_DEPTH_STEP_M", SILL_PAST_M / SILL_T_M / SILL_OUT_M "facade_parts.cpp" — whose SILL_PAST is 0.14 against the shader's 0.07 "each side": to check that the C++ value is the two sides' sum); building_body.gdshaderinc:9 KIND_GROUND 7 "shell.h SK_GROUND", :21-23 arrays [13] (FIN_COUNT), :38 RUIN_PIECES_A_ROW 256, :106 `fin < 13`, :110 `fin != 8 && fin != 7` (glass and metal by their enum index).
- atmosphere.gdshader:177-187 the clouds are taken nearest first by scanning all 24 for each pass: O(n^2) per pixel per frame (up to 576 comparisons), on a full-screen pass, plus 24 x 5 shadow samples each reading every puff (shadow_density -> heap); :17 puff[168] typed instead of MAX_CLOUDS * PUFFS.
- LOOK VALUES IN SHADER CODE (data rule: tunables in data): atmosphere.gdshader:54-56, 71, 99-104, 109, 146-155, 174-175, 183, 195-203, 238-239 (cloud and fog colours as vec3 literals, densities, scales, thresholds); building_body.gdshaderinc:98-103, 116.
#### render chunk 6: shaders building_card, building_colour.gdshaderinc, building_common.gdshaderinc, building_fade, building_far, building_far_window.gdshaderinc, building_ghost, building_glass
- ONE BAY RULE IN C++ AND IN SHADERS: building_far_window.gdshaderinc:57-58 the roller shutter "in four bays of twenty" (picks 9..13 of h % 20) and :53 the dark-pane shares (5 of 20, 1 of 4, 1 of 5) repeat render facade_bay.cpp's pick tables (render notes chunk 20): the near level (C++) and the mid level (shader) must pick the same bays with the same hash, kept equal by hand.
- THE SAME SHADER CONSTANTS TWICE OR THRICE: PANE_ALBEDO vec3(0.0075, 0.0096, 0.0115) in building_far.gdshader:35 and as a literal in building_far_window.gdshaderinc:69 (and "building_pane.gdshaderinc" per the comment); the room hash (cell + 512) * 131u + * 7177u in building_far.gdshader:69 and building_far_window.gdshaderinc:52; the lit-room rule (lot, sub, room_lamp mix, rooms_awake step) in building_far.gdshader:68-73 and building_far_window.gdshaderinc:70-73.
- building_far.gdshader:32-33 FOOT_SHADE 0.62 over FOOT_STOREYS 0.47 darkens the foot of every far wall; the mid and near levels have no such shade: a step at the far hand-over.
- building_card.gdshader:14 BRISE_PITCH "part_models.cpp BRISE_FINS" and :47-49 railing proportions (0.45, 0.04, 0.25, 0.02, 0.12, 0.1, 0.015) typed in the shader (the near-level railing models are C++'s: two owners of a railing's look).
- building_far_window.gdshaderinc:10 FAR_ROOM_LEVEL 0.68 "the mean of pane_lamp's level (0.3 + 0.85 x two uniform lots)": a derived number typed by hand; :65 the shutter colour vec3(0.32, 0.36, 0.37) in code; building_common.gdshaderinc:36 `fin == 8` (metal by index); :43-46 weathering numbers.
#### render chunk 7: shaders building_glass_body.gdshaderinc, building_glass_fade, building_hash.gdshaderinc, building_lamps.gdshaderinc, building_level.gdshaderinc
- TWO HASH FAMILIES IN THE SHADERS: building_hash.gdshaderinc:4-9 is the integer PCG hash (O'Neill; Jarzynski and Olano recommend it); agents.gdshader:41-43 and people_look.gdshaderinc use the fract(sin(...) * 43758.5453) hash (grep), whose results depend on the GPU's sin precision. One owner exists and is bypassed.
- building_glass_body.gdshaderinc:90-107 light() re-implements the engine's Lambert + GGX by hand (to add the recess's shadow): the panes follow a copy of Godot's BRDF; a change in the engine's model would leave the glass on the old one (stated reason; fragile by design).
- C++/SHADER CONTRACTS BY COMMENT: building_glass_body.gdshaderinc:16 furniture_tone[120] (three floats x 40 furniture kinds), :21 KIND_CLEAR 6 "shell.h SK_PANE_CLEAR", :22 PANE_DEPTH_STEP_M "shell.cpp PANE_RECESS_STEP_M".
#### render chunk 8: shaders building_pane.gdshaderinc, building_parts, building_parts_body.gdshaderinc, building_parts_fade
- A DATA VALUE COPIED INTO A SHADER WITH ANOTHER UNIT (certain): building_pane.gdshaderinc:9 FRAME_SHARE = 0.045 "(buildings.json parts.frame_bar_m)" is used as a share of the pane's side (:25 t = FRAME_SHARE * size), while buildings.json:126 frame_bar_m = 0.045 is metres (read by render building_look.cpp:31 for the instanced frame bars). A 2.4 m pane draws a 10.8 cm frame in the shader against 4.5 cm bars in the parts; and editing the data changes only one of them. Fix: the shader takes the bar in metres as a uniform from the same table.
- CONFIRMS THE MID LEVEL IS THE ODD ONE: building_pane.gdshaderinc:14 REVEAL_SHADE 0.68, SILL_SHADE 0.92, HEAD_SHADE 0.544 "facade_bay.cpp" match the C++ near level; building_bay.gdshaderinc:12 (mid) uses 0.82 / 1.18 (chunk 5).
- PANE ALBEDO vec3(0.0075, 0.0096, 0.0115) typed again at building_pane.gdshaderinc:247 and :259 (with building_far.gdshader:35, building_far_window.gdshaderinc:69).
- THE FLAGS ARE THE SOURCE GAME'S (R11): building_parts_body.gdshaderinc:8-9 "waves as the Roadcraft's (flagLayer.ts ...)" and "designs are the Roadcraft's fourteen (flags.ts FLAG_PATTERNS ...)", :24 "the Roadcraft's flag material", :26-28 "The Roadcraft's wave (flagLayer.ts): its clock runs WIND_PACE times real time (wind.ts)", :50 "(flagLayer.ts flagColour)": wave constants and the fourteen designs copied from its code. building_pane.gdshaderinc:194 "as the 4.0's vRoomLot".
- THE FINISH LOGIC TWICE: building_parts_body.gdshaderinc:118-126 repeats building_body.gdshaderinc:105-112 (finish index parse, finish_layer, the glass/metal weathering exclusion by index 8 and 7, arrays of 13).
- building_pane.gdshaderinc:97-106, 119, 124, 127, 136, 138, 185, 188: the interiors' floor, ceiling, wall, picture, television and blind colours as literals in the shader (data rule; the furniture tones do come from materials.json).
- C++/SHADER CONTRACTS: building_pane.gdshaderinc:7-8 ROOM_SPAN_M "shell.h", FRAME_SPAN_M "shell.h".
#### render chunk 9: shaders building_props, building_room_tone.gdshaderinc, building_ruin, building_sheer, building_sky_glass.gdshaderinc, building_wash, canyon_light.gdshaderinc, cloud_field.gdshaderinc, color_space.gdshaderinc, contact, debris
- TWO COLOUR-SPACE INCLUDES THAT DISAGREE ON COMPATIBILITY (certain): building_colour.gdshaderinc:7-9,24-26 says Compatibility decodes ALBEDO/EMISSION "from sRGB", so its surface_out encodes with the exact sRGB curve; color_space.gdshaderinc:7-8,21-37,60-66 says Compatibility decodes with a cubic fit (tonemap_inc.glsl) and its linear_out encodes with gles_encode, the inverse of that cubic. Both cannot be right for the same renderer: on Compatibility the building shaders (building_body, building_far, building_parts, building_glass, building_card, building_props, building_sheer) and the others (agents, props, people, debris, wash ...) hand their colours over by two different curves (the gap is largest in the darks). Fix: one include, the one that matches the engine's decode (color_space's, checked against drivers/gles3), used by every shader.
- DERIVED CONSTANTS TYPED BY HAND: building_room_tone.gdshaderinc:8-10 HUNG_COVER 0.394, HUNG_MEAN, HUNG_DIM 0.776 are the means of building_pane's room_hung picks worked out by hand; a change to room_hung leaves the far tone wrong without a sign.
- C++/SHADER CONTRACTS: building_props.gdshader:10-13 the condenser's size AC_W 0.72, AC_H 0.48, AC_FAN_IN, AC_COIL_IN must match the bay_props mesh; canyon_light.gdshaderinc:9 CANYON_CELL_M 4.0 mirrors canyon_field.h.
- contact.gdshader:4-5 "Godot 4.8 has this built in (GH-118045); the project is on 4.7": a workaround to retire at the upgrade (belongs in log/pendencias.md), :1 "(the 4.0's canyonLight)"; building_wash.gdshader:4 "The reference game adds it after the tone mapping"; :19-24 the pool's fall "(the street lamps' stops)" typed again.
#### render chunk 10: shaders ground_paint.gdshaderinc, lamp_glow, lamp_pool.gdshaderinc, lattice_noise.gdshaderinc, lightning, marks, occlusion, overlay, people, people_blob, people_body, people_face.gdshaderinc
- THE MAP OVERLAY IS TONE-MAPPED THOUGH IT SAYS IT SHOWS SCREEN COLOURS: overlay.gdshader:8 `ALBEDO = linear_out(srgb_to_linear(COLOR.rgb)); // the tokens are screen colours` is unshaded but still passes the Environment's ACES tone mapping, so the zone and congestion colours on screen are the tokens' colours after ACES (shifted, desaturated). The shaders that do mean screen colours invert the curve first (building_wash.gdshader:28 display_to_radiance, marks.gdshader:9-11 crater_rad, effect_materials.gd TONE_INVERTED). Fix: the overlay through tone_inverse.gdshaderinc like them.
- THE SAME FUNCTION PASTED: building_wash.gdshader:19-24 pool() is lamp_pool.gdshaderinc:9-17 pool_fall() line for line; the include exists.
- HASHES AND NOISES MULTIPLY: pcg() defined twice (building_hash.gdshaderinc:4-8 and lattice_noise.gdshaderinc:4-8, "as building.gdshader": two includes that cannot share a shader); people_face.gdshaderinc:26-31 person_hash (a third integer hash); marks.gdshader:41-51 its own hash21 + vnoise; atmosphere.gdshader:83-97 fog_hash + fog_value; building_common.gdshaderinc vnoise (texture lattice); lattice_noise.gdshaderinc vnoise (pcg): four value noises in the shaders (two more in C++, render notes), two sin hashes (agents, people_look).
- C++/DATA/SHADER CONTRACTS: ground_paint.gdshaderinc:30-40 the control-map bit layout (terrain_bridge.cpp packs it), :7 biome_tint[7] (nature.json biomes + 1), :44-56 surfaces 1..9 by the ground slots of nature.json paint.kinds, their colours as literals; people_face.gdshaderinc:5 act_face[24] "(people_renderer.cpp FACE_ACTS)", :6 body_mood[32], :12 ACT_TALK..ACT_CROUCH (the act numbers' shader copy), :24 FACE_PI.
- SOURCE CITES: lamp_glow.gdshader:1 "(Roadcraft scenery.ts beam)"; lamp_pool.gdshaderinc:7 "(Roadcraft lightLevels.ts)"; marks.gdshader:9-10 "the reference game's crater colours"; people_face.gdshaderinc:1 "(faceExpression.ts faceAt)".
- lightning.gdshader:7 screen default vec2(1600, 900) (set from script, to check).
#### render chunk 11: shaders people_far, people_held, people_helmet, people_look.gdshaderinc, people_pose.gdshaderinc, people_skin.gdshaderinc, preview_outline
- (Settled in step 8, cook_people.py:1-2 and LICENSES.json: the bodies are MakeHuman (CC0) and the skeleton and walk are Microsoft Rocketbox (MIT), so people_skin.gdshaderinc:6 and people_asset.h:1-2 are both right. What is wrong is the input: the cast is cooked from the 4.0's own files, §3v.)
- C++/COOK/SHADER CONTRACTS: people_pose.gdshaderinc:11-15 clip_row[24], clip_frames[24], clip_seconds[48], walk_frames[32], body_female[32] (people_renderer.cpp CLIP_TABLE / BODY_TABLE), :19 BODY_SPAN 3.0 "cook_people.py PALETTE_SPAN", :21 CLIP_CODE_BASE and :22 ACT_SPAN 0.96 "people_renderer.cpp"; people_held.gdshader:12 palm_rows[96] and people_helmet.gdshader:9 helmet_rows[96] (32 variations x 3).
- GGX TYPED AGAIN: people_look.gdshaderinc:47-56 ggx_specular "as the engine's own" (with building_glass_body.gdshaderinc:100-106): two hand copies of the engine's BRDF; :96 v_sparkle uses the sin hash.
- SOURCE CITES: people_look.gdshaderinc:4 "(personMaterial.ts)"; people_helmet.gdshader:1 "(Roadcraft riderPoses.helmetShape)".
- people_far.gdshader:11 TAU_RAD (a local tau).
#### render chunk 12: shaders props, puffs, rain, road
- THE BLAST PUFFS MOVE AS THE SOURCE'S DID (R11): puffs.gdshader:4-5 "The motions are those of the puffs of the reference game, written in steps of STEP_M metres" with every kind's rise, spread, fling, growth and peak typed in the vertex stage (:45-73); :17 "(exhaust.ts)"; :109-111 "the reference game writes them straight to its frame buffer ... measured, ports/demolition/relatorio.md": a game shader citing a port report under tools/ports.
- puffs.gdshader:53 and :57 two gravities in one shader (9.8 for the hydrant jet, 12.25 for blood, sparks and grit).
- C++/SHADER CONTRACTS: props.gdshader:15-16 lit_radiance[5] / dark_radiance[5] (prop_kit.h Lens::COUNT, lamp_radiance.gd LENSES), :41-42 KIND_STRIDE 8 and SECONDARY_MARK 10000 (street_props.cpp push_span, prop_kit.h WIRE_SECONDARY_MARK), :114-157 the PropTag values 1..9 compared as floats, kept equal to prop_kit.h by hand; :66 "(the 4.0: wireMaterials, the depth material)".
#### render chunk 13: shaders road_body.gdshaderinc, road_detail.gdshaderinc, road_far, road_wear.gdshaderinc, rubble, screen_depth.gdshaderinc, shadow_caster
- "ONE OWNER" OF THE DEPTH NORMAL, AND A SECOND ONE: screen_depth.gdshaderinc:1-2 declares itself the one owner for the passes that read the depth (occlusion, contact, atmosphere), but canyon_light.gdshaderinc:22-30 (included by atmosphere) has its own canyon_normal, choosing the side by distance(l, c) where view_normal (:17-24) chooses by |l.z - p.z|: two rules for the same normal in one frame.
- C++/SHADER CONTRACTS: road_body.gdshaderinc:60-73 material ids M_ROAD..M_ARM_MEET (road_look.h), :28 lane_tone[8] with road_wear.gdshaderinc:3-5 WEAR_SPAN_M 25.5, WEAR_LANE_M 12.75, TONE_SLOTS 8 (road_look.h wear_color's packing), :3 the UV2 sentinels -1000 / 1000.
- SOURCE CITES: road_body.gdshaderinc:194 "the Roadcraft's skirt", :229 "SPECULAR = 0.5; // the F0 of 0.04 of the roads of the Roadcraft".
- road_body.gdshaderinc:18-19 "Mean colour of each map, computed once in project/render/city_world.gd": the means are worked out in surface_means.gd (city_world only calls it).
#### render chunk 14: shaders shore.gdshaderinc, signature, signature_glass, signature_look.gdshaderinc, sky, terrain
- A DATA VALUE DERIVED BY HAND FROM ANOTHER AND A SHADER CONSTANT: materials.json building_far window_glow = 0.42 is building window_glow 0.62 x building_far_window.gdshaderinc:10 FAR_ROOM_LEVEL 0.68 (= 0.4216), worked out by hand; change either and the far level's windows step at the hand-over. signature_look.gdshaderinc:21 ROOM_GLOW = 0.62 "window_glow of the plain buildings (materials.json)" copies the data value into a shader const. Fix: one window glow in data, the far level's factor applied in one place.
- A RULE COPIED BETWEEN SHADERS: signature_look.gdshaderinc:22 ROOMS_IN_USE 0.64 "building_pane.gdshaderinc pane_lamp" and :27-31 the lamp level 0.3 + 0.85 * fract(lot * 7.13) * fract(lot * 2.17 + 0.4) copy building_pane's pane_lamp (:196-198); :27 the slot hash constants 73856093 / 19349663 copy lattice_noise.gdshaderinc hash2.
- CONTRACTS: shore.gdshaderinc:23 SHORE_DRY -500 "(TerrainGen::NO_WATER)"; signature_look.gdshaderinc:20 GLASS_CELL_M 3.2 "cook_signatures.py ROOM_M"; terrain.gdshader:7 MAX_REGIONS 1024 "Match Terrain3DMaterial.max_regions", :29 _region_map[1024].
- sky.gdshader:1 "(Roadcraft render/scene.ts makeSky)".
#### render chunk 15: shaders terrain_layers.gdshaderinc, terrain_uniforms.gdshaderinc, tone_inverse.gdshaderinc, transit, transit_glass, transit_xray, vegetation, vegetation.gdshaderinc
- A THIRD COLOUR RULE, AND A FALSE CITATION: vegetation.gdshaderinc:51-58 defines to_linear again (the third copy, with building_colour and color_space) and says Compatibility "hands it over already decoded (road.gdshader does the same)"; color_space.gdshaderinc:4 and building_colour.gdshaderinc:4 say COLOR is sRGB on both renderers, and road_body.gdshaderinc:127 decodes it unconditionally. Passing sRGB COLOR straight to ALBEDO on Compatibility happens to give the right albedo, but vegetation.gdshader:23 then multiplies it by v_tint: in sRGB on Compatibility, in linear on Forward+, so tinted trees differ between the two renderers. Fix: color_space's rule (decode, work in linear, linear_out) as everywhere else.
- THE SAME CONSTANTS IN TWO INCLUDES: terrain_layers.gdshaderinc:48-50 DETAIL_TURN_C 0.5403, DETAIL_TURN_S 0.8415, DETAIL_SECOND 0.4137 repeat road_detail.gdshaderinc:16-17; the luma weights (0.3, 0.6, 0.1) typed at terrain_layers.gdshaderinc:80, 81, 120, 124, 146 (and city_world.gd:7 LUMA).
- AN ENGINE INTERNAL MIRRORED TWICE: tone_inverse.gdshaderinc:2-4,12-21 inverts Godot 4.7's tonemap_aces (servers/.../tonemap.glsl) and render/tone_map.gd inverts it again on the CPU: a change of the engine's ACES leaves both wrong without a sign.
- CONTRACTS: vegetation.gdshaderinc:20 PART_* "tree_part.h TreePart", :21 FAR_TINT_SPAN "far_cards.cpp"; :64, :76 float hashes (fract(dot(...)) * 47.53 / 43.17).
- SOURCE CITES: terrain_layers.gdshaderinc:46 "(Roadcraft's detail layer)"; terrain_uniforms.gdshaderinc:4 grid_m "(world/lots.ts: the 10 m cell)".
#### render chunk 16: vegetation_cards, verge, vignette, water, water_plain, wear.gdshaderinc, shore_view.gd
- TWO COLOUR RULES IN ONE SHADER: vegetation_cards.gdshader:32 far cards use color_space's rule (linear_out(source_linear(p.rgb) * v_tint)); :51 leaves use vegetation.gdshaderinc's vertex_albedo(COLOR.rgb, OUTPUT_IS_SRGB) * v_tint * leaf.r, which on Compatibility multiplies sRGB COLOR by the tint and by the sRGB-sampled atlas grey (source_color textures are not decoded there, color_space.gdshaderinc:4-5) before the renderer decodes ALBEDO: decode(c*t*g) != decode(c)*t*g, so tinted leaves differ by renderer. Fix: ALBEDO = linear_out(srgb_to_linear(COLOR.rgb) * v_tint * source_linear(vec3(leaf.r)).r), and delete vertex_albedo and vegetation.gdshaderinc's to_linear.
- The mip-level expression is typed twice in vegetation_cards.gdshader (:26-27 and :45-46); the else branch (:37-53) is not indented.
- vegetation_cards.gdshader:10 REGION_W/LEAF_U/FROND_U mirror leaf_atlas.h by comment.
- FALSE HEADER: water.gdshader:2 "Every number comes from data/render/palette.json"; :70 rough 0.5 + 2.0*waves, :87 smoothstep(0.35, 0.8, ... * 0.9 ... * 0.3) + edge * 0.6, :90 grain 0.62/1.3/2.65/0.85/1.12/1.05/0.4 are typed in the shader.
- SOURCE ARGUMENT: water.gdshader:25 absorb_m "a choice by eye held to the 4.0's blue".
- water.gdshader:58, :71, :98 normalize(vec2(wind_x, wind_z)) three times per pixel although sky_view.gd:117-118 already sends a unit vector (cos, -sin).
- water.gdshader:75-82 reconstructs view depth by hand (a third copy, with screen_depth.gdshaderinc and canyon_light).
- wear.gdshaderinc:8 WEAR_RES/TILE/TILES mirror wear_field.h; :28-44 darkening and hole/crack thresholds and colours typed in the shader; :7 wear_shown exists for one perf test.
- verge.gdshader:40 ROUGHNESS mix(0.95, 0.6, rain_wet) typed.
- TWO PROTOCOLS FOR A REVISIONED FIELD: shore_view.gd:16 builds the whole shore Dictionary (with the texture Ref) every frame to compare a revision (city_core_nature.cpp:241, shore_field.cpp:123-130); sky_view.gd:129-137 asks fog_revision/canyon_revision first and the field only on change. One owner: a revision query for shore too, or one "fields" read for all three.
#### render chunk 17: sign_sheet.gd, sky_view.gd, surface_means.gd
- A RENDER VALUE THROUGH THE COMMAND PATH, EVERY FRAME AT DUSK: sky_view.gd:124-126 dispatches set_lamp_night whenever daylight.night moves; daylight.gd:101 night = smoothstep(night_from_deg 3, night_to_deg -7, sun elevation) moves every frame through dusk and dawn in "cycle"; city_store.gd:95-96 marks the snapshot on every dispatch not named preview*, so :22-24 rebuilds _core.snapshot() and :156-158 emits `changed` (hud_top_bar.gd:76, hud_popover.gd:27, change_marks.gd:22) every frame instead of every SNAPSHOT_INTERVAL_S 0.25 s. city_core.cpp:221 set_lamp_night only sets render_. Cause: StoreApi has no class for render settings that are not CityState commands (also set_lamp_lights, set_vegetation, set_detail_scale, set_overlay...). Fix: a third StoreApi kind (render settings, routed straight to the core, no mark), or the core reads the global `night` itself; set_lamp_night out of COMMANDS.
- SKY VIEW OWNS THINGS THAT ARE NOT THE SKY: sky_view.gd:146-147 sets the world's biome and water look from its landscape read; :128 drives the contact shadow. Owner: one landscape view (or CityWorld) reads "landscape" and fans it out.
- STATE READ BACK FROM A MATERIAL: sky_view.gd:170-171 takes the horizon colour from the sky ShaderMaterial every frame (and srgb_to_linear in GDScript) although Daylight is its owner (daylight.gd:249-251 computes `horizon`). Fix: Daylight exposes the linear horizon.
- HASH COPY: sky_view.gd:178-181 repeats seeded.h:94-95 (374761393, 668265263, 1274126177) in GDScript.
- CONTRACT BY JSON KEY ORDER: surface_means.gd:18-21 fills lane_tone in the order of streets.json:4 asphalt_by_class's keys; road_junction.cpp:172 and road_look.h:35 index it by int(RoadClass) (profile_types.h:15). Matches today by key order alone; resize(8) pads missing classes with black and drops extra ones silently. Fix: the core sends the tones by RoadClass (it already loads streets in setup_render), or index by the catalog's class names.
- surface_means.gd:7 LANE_TONES mirrors road_look.h:33 TONE_SLOTS and road_body.gdshaderinc:28 lane_tone[8]; :15 DataTables.render() whole for one key; :28-47 the mean colour of each map is recomputed at every boot from textures that never change (get_image of a compressed texture, decompress, trilinear resize): a constant to bake into data with its input key.
- GPU READBACK: sign_sheet.gd:76 get_texture().get_image() reads the viewport back to the CPU each round with new cells (a pipeline sync), to keep a CPU copy for the array's growth (:108-116 recreates the whole Texture2DArray from all pictures). Cost by design; certain that it is a readback, size not measured.
- TEST DATA IN THE GAME: sign_sheet.gd:17 `drawn` keeps every cell painted for the life of a city, only for the captures.
- (sign_atlas.h:2 layers only advance, so sign_sheet.gd:64's "kept only for the layer begun" is right.)
#### render chunk 18: thunder.gd, tone_map.gd, transit_overlay.gd, weather_view.gd
- WRONG SOUND ON OVERLAPPING STRIKES: thunder.gd:59-63 sets the bus filter's cutoff and the one player's volume at the call, then awaits the sound's delay and plays; weather_view.gd:131-144 allows max_strikes at once. A near strike followed within its delay by a far one sounds with the far one's low-pass and volume, and _player.play restarts the stream, cutting a clap still sounding. Fix: set cutoff and volume after the await, one voice per strike (AudioStreamPolyphonic or a player per bolt with its own filter bus).
- weather_view.gd:83 `dt * rate / 60.0`: strikes per minute with an unnamed 60; :95 CityCamera.ground_below each frame while it rains (a map read per frame, with the camera's).
- tone_map.gd:11-19 mirrors Godot's tonemap_aces constants (with tone_inverse.gdshaderinc: two copies of an engine internal).
- transit_overlay.gd:6-8 MODE_BUS/TRAIN/METRO mirror the core's transit modes by value; :22 ThemeDB.fallback_font (not the theme's); :25-26 redraws every frame.

## 3t. `project/data` (31 chunks): findings by chunk

#### data chunk 1: catalog/blast.json, growth.json, maps.json, nature.json
- SOURCE GAME VALUES IN THE DATA (forbidden: look and features only): blast.json:2 "shot.*: the pistol of the reference game: head 100, trunk 34, limb 20 ... 12 hits burst a car", "(behaviour of the 4.0 kept as reference, listed in the report): people.knock_*, panic_s and panic_gain, sever_chance, maim_chance, strike.hold_growth_s, fire_*, wear_gain"; growth.json:4 plan "authored as the 4.0's world/cityGen/plan.ts has it: no survey behind these numbers"; growth.json:2 "world/arch.ts civicFor"; growth.json:33 green_main_roads lists the source's camelCase profile ids. Fix: each value either a sourced figure or a design decision of this game stated as such, with no 4.0 origin.
- blast.json:28 people.knock_min_s/knock_span_s 3.2/1.8 and :30 shot.knock_min_s/knock_span_s 3.2/1.8: two keys holding the source's one number (blast_loader.cpp:81-82, 110-111; strike_people.cpp:82, shoot_people.cpp:90).
- CONTRACTS BY POSITION: blast.json:30 vehicle_box_m[9] by VehicleType order (traffic_params.h:68); growth.json:41-44 civic by_zone [[kind, weight]] with bare kind numbers by zone index; nature.json:45 landform_rock and :79-80 kind_first/kind_count by index. Fix: keyed objects (by type/kind name) the loader resolves against the enum's names.
- STALE PATH: nature.json:2 and :109 cite "ports/landscape/research.md"; the file is tools/ports/landscape/research.md.
- nature.json:2 "compared with reference-minha photos" (a Portuguese folder name in the text); "(Lei 12.651 ... not read for this port)": a cited source not read.
- HALF-DONE STATE: nature.json:83-87 styles "Until the style lives in the CityState the loader reads roadcraft"; "roadcraft" and "brasil" hold the same default_biome, so the choice changes nothing (with §3m A4).
- nature.json:82 element kinds and growth.json:33 profile ids in the source's camelCase ("tallGrass", "avenue4Lanes", ...).
- maps.json: no _doc; "medium" (the city fills the map at a radius) is explained nowhere in the data.
- growth.json:49-50 decay_grace_min/decay_span_min: no doc.
#### data chunk 2: catalog/road_profiles.json
- DEAD STYLE DATA: road_profiles.json:2-31 styles: "Until the style lives in the CityState the loader reads roadcraft", so "brasil" prices (carriageway 266, sourced from DNIT) are never used and the source game's name "roadcraft" is the live style id.
- PORTUGUESE IDS IN DATA: "category": "ruas", "avenidas", "rodovias", "especiais", "antigos" (and drawers.json:147-170 catalog_tabs "trilhos"; street_furniture.json:110 prefixes keyed "avenidas"/"rodovias").
- UNREACHABLE PROFILES: category "antigos" (oneWayWithParking road_profiles.json:127, countryRoad road_profiles_b.json:130) is no catalog tab (drawers.json:147-170) and no code names them (grep of project and native/src): dead entries.
- SOURCE IDS: profile ids are the source game's camelCase (localStreet, streetWithParking, ...); city_gen.cpp:24-31 types eight of them in C++ "the three sections of the 4.0's cities (world/cityGen)" (a data name in code: a rename in the JSON breaks the generator; the loader should resolve these from data, e.g. growth.json's plan naming its profiles).
- The profile list is split over road_profiles.json, _a and _b for size (data_tables.gd road_profiles_a/b).
#### data chunk 3: catalog/road_profiles_a.json, road_profiles_b.json
- SAME SECTION TWICE IN ONE TAB: cityAvenue (road_profiles_b.json:340-365) has boulevard's strips exactly (:4-27: 2.85/0.15/4/4/0.15/1.7 pave 3/...), both in category "avenidas": two cards for one cross-section, differing only in class (tone and junction rank) and priority. The generator's sections (city_gen.cpp:24-31: cityLocal, cityUrban, cityAvenue) are kept as player profiles beside near-identical ones (cityUrban = urbanStreet with 2.85 m footways).
- UNDEFINED ORDER: road_catalog.gd:31 sorts by rank with sort_custom (not stable), and ranks repeat inside a tab: "ruas" narrowStreet (a:104) and cityLocal (b:299) both 9, sharedStreet (a:146) and cityUrban (b:321) both 10; "avenidas" boulevard (b:7) and cityAvenue (b:343) both 9, avenue (main:176) and avenue4Lanes? (avenue rank 0, avenue4Lanes 1: fine). Fix: unique ranks per category (a gate), or sort by (rank, id).
- OPAQUE NUMBERS: "surface": 0..3 and strip "pave": 2/3 are bare enum values in data (asphalt/concrete/cobble/ballast; paving kinds). Fix: names resolved by the loader.
- Three files in three layouts (main and _a one key per line, _b one strip per line): split for size only.
#### data chunk 4: catalog/street_furniture.json
- THE GAME PLAYS THE SOURCE GAME'S CODE VALUES: data_tables.gd:65 REGIONAL_STYLE := "roadcraft" is laid over street_furniture.json and streets.json for every read (:68 STYLED, :85); street_furniture.json:170-193 "roadcraft" is "What the Roadcraft 4.0 draws": the crossing "(src/world/approach.ts)" zebra 1.2-4.2 m and stop line 4.84-5.24 m, furniture rows "(world/roads/furnitureCatalog.ts)" every 60 m from 9/24 m, trees none in the generated city "(editor/cityGenerator.ts)", hydrants every 100 m "the 4.0's", poles 8.5 m "(POLE_HEIGHT)", lamps "column_every_m 30" "as the Roadcraft 4.0 lights it". The sourced block "brasil" (:194-216: MBST crossings 1.0-5.0 m and stop line 6.6-7.0 m, NPT 034 hydrants, Neoenergia 10.2 m poles) is never played. Fix: drop the "roadcraft" style; the default style is the sourced one; any look value without a norm becomes a design value of this game stated as such.
- SOURCE BEHAVIOUR AS DATA: :9 lamp pools "behaviour of the Roadcraft 4.0 (no norm found)"; :38 poles "read from the Roadcraft 4.0 tool in play"; :59 barrier heights "follow the Roadcraft 4.0 look"; :88 signs "as the Roadcraft 4.0 places them"; :98 items "as the Roadcraft 4.0 tool does"; :107 names "as Roadcraft 4.0 world/roads/streetNames.ts names them".
- STALE DOC: :173 "height_m: ... 8.4 m where the 4.0 has 8.5" while :191 height_m is 8.5 (and streets.json:351 the pole mesh 8.5).
- THE SAME DATA TWICE: the 57 street names (:172 and :195) and the signals block (:174-177 and :197-200) are copied verbatim into both styles. One owner: the shared values in the base table, the styles holding only what differs (the overlay replaces blocks, so the base holds them once).
- PLACEHOLDERS: :217-218 "eua"/"europa" hold only "To fill from ..." (a pending item, belongs in log/pendencias.md); Portuguese ids "brasil", "eua", "europa".
- :169 and data_tables.gd:63 cite "session-rules.md, section 10": the file is tools/process/session-rules.md.
- data_tables.gd:93 push_error("JSON inválido: %s"): Portuguese message.
#### data chunk 5: catalog/traffic.json
- ONE VALUE, TWO KEYS, TWO READERS: traffic.json:133-134 work_stay_h 8 / errand_stay_h 0.5 (ped_demand.cpp:30) and :369-370 stay_work_h 8 / stay_errand_h 0.5 (lot_demand.cpp:25): the same stay of a person at work or on an errand. Fix: one pair, both readers on it.
- KERB HEIGHT, TWO OWNERS: traffic.json:137 curb_m 0.15 (lane_geometry.cpp:65 lifts walkers by it) and streets.json heights.curb 0.15 (the render's kerb). A change of either floats or sinks the walkers. Fix: one owner (the street table), the sim reading it.
- SOURCE CODE AS DATA ORIGIN: :2 _transit_doc "The 4.0's numbers, no source behind them yet"; :389 _kerb_doc "the 4.0's numbers"; :390 _lorry_doc "(lorryDocks.ts PLAN_BUDGET). The rest are the 4.0's"; :391 vehicle classes "(drivers.ts ARCH, traffic/state.ts)"; :405 zone weights "(traffic.ts:938-947; peds ZONE_PULL)"; :410 people_busy "the Roadcraft 4.0's day as drawn by hand"; :384 style "The 4.0's"; :4 the source's counts as arguments ("the Roadcraft 4.0 counted 86", "9% measured in the Roadcraft 4.0").
- DEAD DATA BY ITS OWN WORDS: :383 "signal_lenses and signal_plan: read by nothing yet"; :386-387 eua/europa placeholders.
- UNDOCUMENTED: :404 style_share (6 shares of car styles, demand.cpp:63) has no doc.
- POSITIONAL MATRICES: :392-402 classes [a, b, b_emergency, T, s0, share, length, half width] x VehicleType order; :406-409 zone_* by zone index; :412 people_busy 4x24 flattened; :415 zone_kinds 5x4 flattened. Fix: keyed rows.
- STALE REFERENCE: :4 "spec sim.md §5-§6": no sim.md exists in the repository.
- :410 "United States data, the default style's": the default style is "roadcraft" (data_tables.gd:65), not a US style.
- :4 _doc is one line of about 6,000 characters: a document kept as a JSON string.
#### data chunk 6: catalog/tuning.json, zones.json; game/camera.json, daylight.json, health.json, interior.json, lab.json, optimizations.json
- TUNING WITH NO DOC, AND SOURCE CONSTANTS IN IT: tuning.json (228 lines) has no _doc at all, unlike every other table. Its grade/structure/edit/build/camera numbers are multiples of 0.4 m, the source game's unit (road_grade_test.gd:2 "in the 4.0's 0.4 m units, U, turned to metres"): pier_spacing_elevated_m 29.6 (74 U), pier_spacing_bridge_m 38.4 (96 U), deck_elevated_m 1.04, clear_elevated_m 5.6, tunnel_headroom_m 4.4, parapet_height_m 0.88, pier_abutment_gap_m 10.4, portal_wing_m 5.6, duplicate_min_m 8.8, duplicate_gap_m 4.8, world_margin_m 3.2, snap_road_reach_m 9.6; camera.json pull_full_half 4.8, pull_none_half 19.2. Fix: a _doc per block giving each value's source or design reason; the source's constants replaced by this game's decisions.
- interior.json:2 is the source code, line by line: "src/render/renderer.ts:143 ROOM_LIGHTS", "renderer.ts:1624-1626", "roomLamps.ts:20-45", "renderer.ts:3713; TUNED_UNIT = 0.4 m, src/core/units.ts:12", "day_share is the 4.0's 0.25": the lights are the source's code values converted.
- daylight.json:2 source code as origin: "Roadcraft render/scene.ts", "by the rule of the 4.0", "the cascades of the 4.0 (sunCascades.ts)", "4.0 linear (1, 0.96, 0.9)", "as the 4.0 keeps it" (moon), "#e6e4da over #6a6b4c"; :76 grade "the 4.0's own look"; :85 vignette "the 4.0's own look"; :102 "the 4.0's GTAO". (Measured photos of the look are allowed; the code-file constants are not.) :2 also holds a pending item ("look again when the engine moves on") and :66 is mis-indented; shadow_* keys are scattered (:91-98, :100, :109-117).
- camera.json:2 "Roadcraft's perspective rig and its feel"; orbit_px_per_rad 158.73, compass_* 83.3 / 166.7 are reciprocals typed to two decimals.
- lab.json:2 "(lab/recorder/analysis.ts SETTLE_MS)": a source constant.
- optimizations.json:2 the tab mirrors the source's three.js panel ("Three.js's own options have no counterpart here"); option ids in the source's camelCase (shadowMapSize, detailProps, postChain, focusDetail).
- zones.json: no _doc; residents (floor_share 0.6, m2_per_resident 28, m2_per_job_* 18/45, m2_per_person_mixed 40) unsourced; "code" repeats the index; name keys camelCase (zone.resLow).
#### data chunk 7: game/render_switches.json, saves.json, settings.json, sky.json, start.json
- THE SOURCE GAME'S WHOLE CODE IS IN THE REPOSITORY: tools/oracle-4.0 is tracked (git ls-files: 4698 files: public 2391, src 1272, tests 780, cooked, scripts, lots; 4.1 GB on disk; commit 005c01a9 "a cópia instrumentada do 4.0"), and CLAUDE.md's structure does not list it. The data cites it: settings.json:2 "ARQUITETURA.md item 8" is tools/oracle-4.0/ARQUITETURA.md (no such file in this project). Fix: out of the repository (an external reference copy, never a path the game's text can cite); the citations replaced by this game's reasons. (Settled under tools in step 8.)
- render_switches.json:4 the Diagnosis is "the 4.0's doctor", its keys named after the source's functions ("verdictOf", "severityOf", "quickAb's windows"); keys in the source's camelCase (ambientOcclusion, bMasses, bFacadeLevel).
- STALE: render_switches.json:2 "Switches of systems the game does not have yet (clouds, weather, particles, transit) come with the session that ports them": clouds, weather and transit exist (sky_view.gd, weather_view.gd, render/transit) and have no switch.
- saves.json:2 "Roadcraft game.ts:756", "Roadcraft persistence.ts onSaveFailed", "DEBOUNCE_MS, MAX_WAIT_MS": source constants.
- A SETTING WITH NO EFFECT: settings.json tiers cloud_shadows: its only reader is quality.gd:117 `clouds_drawn = q.sky_clouds or q.cloud_shadows`; every tier with cloud_shadows has sky_clouds, so medium (false) and high (true) draw the same. Fix: a real switch for the clouds' shadows in the atmosphere pass, or the key removed.
- WRONG OWNER: settings.json ("Graphics options") holds :192 road_blocks (the road-blocks tool's grid and steps) and :225 layers (map overlays shown by default).
- A BROWSER CONCEPT: settings.json:2 pixel_ratio "the most device pixels per CSS pixel drawn" (the source's devicePixelRatio).
- settings.json:12-19 filter_radius mirrors the engine's table per ShadowQuality (renderer_scene_render_rd.cpp): a copy of an engine internal.
- sky.json:7 "_doc" sits after five keys; "the 4.0's rule" (rain_mist); "compared with reference-minha" (Portuguese folder name); "the paper's table was not read ... by a secondary summary".
- start.json:35-38 sizes town and metropolis only: maps.json km10, km16, km20 reach no screen (km24 only drawers.json:236 large_maps).
#### data chunk 8: i18n/strings.csv part 1 (lines 1-407)
- BROWSER TEXT IN A DESKTOP GAME: strings.csv:234 creator.notSaved "o armazenamento do navegador está cheio ou bloqueado" / "browser storage is full or blocked" (the source's localStorage); no GDScript reads it (grep).
- CURRENCY, TWO OWNERS AND A WRONG SYMBOL: :298 economy.money "R$ {value}" / "$ {value}" (English shows reais as dollars) beside start.json styles currency "R$", which formatters.gd:72 uses; economy.money is read by no script.
- UNTRANSLATED: :176 cmd.place.roadPaint "road paint" in the pt_BR column.
- SAME TEXT UNDER TWO KEYS: :131 hint.shoot.miss and :403 game.toast.missed ("Ninguém aí: clique numa pessoa."); :155 cmd.place.junctionControl and :216 cmd.place.control ("controle do cruzamento"); :128 cmd.strike and :141 cmd.place.bomb ("bomba"); :132 hint.shoot.hit "Acertou." and :394 game.toast.hit "Atingido.". (Which are live: the key census at the end of i18n.)
- A TEST TOOL AS A PLAYER OPTION: :374-375 game.options.worstCase "(teste de desempenho)"; no script names it (census pending).
- :358 game.hint.pistol "ela cai e some" (falls and disappears) while blast.json shot.* gives health per hit and :133 hint.shoot.killed exists: the hint describes a one-shot rule.
- The file is not sorted (:128-133 hint.* among cmd.demolish.*, :216 cmd.place.control among control.*).
#### data chunk 9: i18n/strings.csv part 2 (408-503)
- BROWSER AND JAVASCRIPT TEXT: strings.csv:434 game.toast.webgl "WebGPU indisponível ... (WebGL 2)"; :445 health.copyFailed "o navegador recusou a área de transferência"; :448 health.msg.rejected "Promessa rejeitada: {reason}" (a JS Promise): the source's web runtime, nothing of Godot (census pending for readers).
- A DATA VALUE TYPED IN TEXT: :450-452 health.status.* "nos últimos 30 s" while health.json status_window_s is 30: a change of the data leaves the text wrong. Fix: {seconds} filled from the data.
- THE SAME HINT THREE TIMES: :333 game.hint.bomb, :486 help.strike.strike, :502 hint.tool.bomb (bomb); :338 game.hint.elements and :503 hint.tool.elements.
- :422 game.toast.stationOnTrack: "Via → Trilhos" in pt_BR, "Transit → Train track" in en: two different menu paths.
- English terms for one thing vary: sidewalk (:406), footway (:489, :491), pavement (:487).
#### data chunk 10: i18n/strings_b.csv part 1 (1-367)
- TWO KEY FAMILIES FOR THE SAME HINTS: strings_b.csv hint.tool.* repeat strings.csv game.hint.*: :7 paint (strings.csv:356), :8 person (:357), :9 pistol (:358), :11 river (:359, verbatim), :13 transit (:361), :14 weather (:362), :15 zone (:363). One owner per tool hint (the census will say which family is read).
- BROWSER TEXT AGAIN: :227 profile.notRemoved and :229 profile.notStored "o armazenamento do navegador está cheio ou bloqueado".
- DATA VALUES TYPED IN TEXT: :55 inspector.impossibleBody "Abaixo de 25°" (tuning inspect.min_leg_deg 25); :94/:103 junction.lower/raise "0,5 m".
- ORPHAN CANDIDATE: :148 paint.kind.mud "Lama": nature.json paint kinds have no "mud".
- :357 hint.road.clearance sits among refuse.*; dead profiles (oneWayWithParking :236, countryRoad :203) keep their names.
#### data chunk 11: i18n/strings_b.csv part 2 (368-492)
- BROWSER TABS IN A DESKTOP GAME: :448 save.nowWriter "A outra aba fechou: esta aba agora salva a cidade", :449 save.otherTab "aberta em outra aba" (the source's multi-tab writer lock); census pending.
- DATA VALUES TYPED IN TEXT: :369 road.bend.sharp "(menos de 25° não fecha o cruzamento)"; :417 roadFurniture.tip "faixa livre de 1,20 m" (street_furniture.json sidewalk.free_width_m); :419-420 roadKeys.height* "1 m"; :396-403 road.sub.* lane counts per class typed beside the profiles' data.
- DUPLICATE: :427 save.blocked repeats strings.csv:419 game.toast.saveBlocked.
- :391 road.section.title "· experimental": a feature labelled experimental to the player.
#### data chunk 12: i18n/strings_c.csv part 1 (1-420)
- WRONG CREDITS AND LICENCES: strings_c.csv:138 ui.about.credits "Feito com three.js (MIT), Vite (MIT), Electron (MIT) e steamworks.js (MIT)": the game is built on Godot 4.7 (MIT), godot-cpp (MIT), Terrain3D (MIT), meshoptimizer (MIT) and Clipper2 (Boost) (CLAUDE.md "O que um clone precisa"), none credited; MIT requires the notice. (The census shows ui.about.* is read by no code: the credits are not shown at all, so the engine's notice is shown nowhere.) Fix: an About built from the real third-party list, with each licence text.
- WEB RUNTIME TEXT: :189 ui.error.isolation "COOP/COEP ... jogar.bat (jogar.sh)"; :239 ui.options.compat "(WebGL 2)"; :146 ui.about.steam "Jogando pelo Steam" (steamworks.js).
- DATA VALUES IN TEXT: :167 "máximo 512" profiles; :287 "No máximo 8 faixas"; :289 "máximo 70 m"; :413-414 v2.snap "90°, 45°, 15°", "grade de 1 m" (tuning snap_step_deg 15, grid_m 1); :235 ui.money.unlimited "R$ ∞" / "$ ∞" (currency again).
- WRONG TEXT: :298 ui.start.sub "cidades inteiras de 10 a 24 km" while start.json offers town and metropolis only.
- KEYS NAMED BY VERSION: :363-420 "v2.*" (the source's second interface).
- SAME NAME, TWO KEYS, TWO ENGLISH WORDS: tool.* (:65-75) and ui.toolName.* (:304-333): tool.bulldoze "Bulldoze" vs ui.toolName.bulldoze "Demolish"; tool.streetscape "Landscaping" vs ui.toolName.streetscape "Streetscape".
- :228 ui.menu.testCity "Carregar o pior caso (teste)" and :291-292 ui.soon "em construção": test and placeholder entries in the player's text.
#### i18n KEY CENSUS (deterministic: scratchpad i18n_dead.py over project/**/*.gd|tscn|json and native/src, counting literal keys, the 38 prefixes the code builds keys from, and plural forms of reached bases)
- 905 of 2,559 keys (35 %) are reached by no code: the source game's string table carried whole. Groups (file prefix count): strings.csv cmd.place 48, game.toast 53, creator 20 + creator.preset 11 + design.* 38 (the source's building creator), game.hint 12 + game.hint.* 19, building.fn 21, arch 13, compass 8, connectors 8, health 6 + health.msg 3 (with "Promessa rejeitada"), game.options 4 (worstCase); strings_b.csv save 16 + save.* 12 (with the browser-tab writer lock), profileEditor 28 + 4, road.section 12, road.sub 6 + 2, road.turn 6, junction 8 + junction.control 7, inspector 12, keys.* 12, profile 10 (with the browser-storage texts), profile.element/material/materialName 12; strings_c.csv ui.toolName 29, ui.do 18, ui.insp 16 + 2, ui.options 11 (with WebGL), ui.people 10 + ui.people.* 22, ui.touch 9, ui.about 7 + 8 (the wrong credits), tools.hint 15, tools.toast 7, v2.s 6, zone 6; strings_d.csv builder.* about 120 (builder.lot 38, builder.field 21, builder.snap 10, ...); strings_e.csv 7. Full list: scratchpad i18n_dead.txt. Fix: delete every unreached key (a gate: a key no code reaches fails, as a code key with no translation already should).
- DUPLICATE KEYS ACROSS FILES: cmd.place.crossing (strings.csv:146 "crosswalk", strings_e.csv:379 "pedestrian crossing": the later file wins, two English texts); zone.lot.size (strings_c.csv:448 and strings_ui.csv:181).
- The table is split into six files (data_tables.gd:69-76 STRINGS) by size, not by owner.
#### data chunk 13: i18n/strings_c.csv part 2 (421-579), and the reverse census
- ZONE NAMES, TWO OWNERS, ONE BROKEN: zones.json:3-11 name_key ("zone.none", "zone.resLow", "zone.resHigh", "zone.comLow", "zone.comHigh" have no translation at all) is loaded (catalog_loader.cpp:137) and read by nothing; the names shown are strings_c.csv:455-463 zone.name.0..8 by code (inspector_panel.gd:75 "zone.name.%d"). Fix: one owner (zones.json name_key, complete, read by the UI), zone.name.N deleted.
- maps.json:4-7 name_key menu.size.km10/16/20/24 have no translation (with the maps reaching no screen: dead entries).
- A DATA VALUE IN TEXT, BESIDE A KEY THAT FILLS IT: strings_c.csv:540 hint.cloud.full "(24)" (nature.json clouds.max_clouds 24) while strings.csv:402 game.toast.maxClouds takes {n}.
- DUPLICATE: :556 confirm.clearTerrain repeats strings.csv:328 game.confirm.flattenMap.
#### data chunk 14: i18n/strings_d.csv (1-305)
- The builder's table: of its keys about 120 are reached by no code (census: builder.lot 38 duplicating strings_e/lot.* names, builder.field 21, builder.snap 10, builder.sel 7, builder.help 10, builder.roof.* 8, builder.quick 5, builder.blockMode 4, builder.category 6, builder.drawAction 4, ...): the source's Builder menus carried whole.
- builder.lot.* (:102-139) and lot.* (census: 38 under the "lot." prefix the code builds) name the same 38 buildings twice.
- Key naming mixes conventions: builder.tool.furn_armchair (snake + camel), builder.flagColour.0..15 by index.
#### data chunk 15: i18n/strings_e.csv part 1 (1-274)
- TWO KEY SETS FOR THE FINISHES: strings_e.csv:79-91 building.finish.* and strings.csv:260-269 design.finish.* (the latter reached by no code).
- :222-229 eight hint.builder.prim* keys hold the same sentence (one key the eight tools could share).
- :150 hint.builder.extrude pt_BR "o handle do topo": an English word in the Portuguese text.
#### data chunk 16: i18n/strings_e.csv part 2 (275-439)
- GARBLED TEXT THE PLAYER SEES: strings_e.csv:381 hint.uturn.noMedian "SÃ³ uma via de mÃ£o dupla ..." and :382 hint.uturn.narrow "O canteiro Ã© estreito ...": UTF-8 read as Latin-1 and written back (mojibake); road_crossing.cpp:69 and :73 return these keys, so a refused U-turn shows them. Fix: "Só uma via de mão dupla com canteiro tem retorno." / "O canteiro é estreito demais para um carro fazer a volta."; a gate rejecting "Ã[\x80-\xBF]"-type sequences in the CSVs.
- SAME NAME FOR DIFFERENT MODELS: model.house and model.plainHouse both "Casa" (:306, :349); model.apartments and model.plainApartments "Prédio residencial" (:308, :351); model.shop/plainShop "Loja"; model.warehouse/plainWarehouse "Galpão"; model.factory/plainFactory "Fábrica".
- THE SAME 38 BUILDINGS UNDER TWO KEY SETS WITH DIFFERENT NAMES: lot.* (:390-427) and builder.lot.* (strings_d.csv:102-139): lot.artDeco "Torre art déco" vs builder.lot.artDeco "Art déco", lot.roundGlass "Torre redonda" vs "Redonda de vidro".
- English uses "blueprint" (:359-360) and "model" (:362-364) for the same thing.
#### data chunk 17: i18n/strings_ui.csv
- WRONG NUMBERS TYPED IN TEXT: strings_ui.csv:7 confirm.newWorld "no mapa inteiro de 10 km × 10 km" (shown for new_city, hud_actions.gd:10) while maps.json:3 town is 20 tiles x tuning world.tile_m 1024 = 20.48 km; :5 confirm.newLargeWorld "cerca de 2,7 km" while maps.json:8 metropolis city_radius_m 1600 makes 3.2 km across. Fix: {km} filled from the map's data (strings_c.csv:168 ui.confirm.newWorld already takes {km} and is read by no code).
- TWO PLACEHOLDER CONVENTIONS: lab.* use printf (%s, %d, %.1f: :114-232) and the rest use {name} with format().
- fx.* labels name the source's levels and passes: :12 "(GTAO)", :13 "(L0)", :15 "(L1)".
- :81 hint.openFailed overlaps strings_b.csv save.file.notMap/save.importFailed.
#### data chunk 18: render/agents.json part 1
- THE PLAYED STYLE HAS SIX OWNERS, NONE IN THE STATE: "roadcraft" is typed in data_tables.gd:65 REGIONAL_STYLE, catalog_loader.cpp:80 STYLE, nature_loader.cpp:124 STYLE, region_table.h:10 DEFAULT_REGION, traffic_params.h:87 REGIONAL_STYLE and start.json:4 default_style; CityState holds no style (the tables say "until the style lives in the CityState"). With start.json's style picker writing only formatters.gd's units (session.gd:63 set_style), a city's style is half in the session and half compiled in. Fix: the style in CityState (saved with the city), one loader resolving every styled table from it, the five constants deleted, the "roadcraft" (4.0) block deleted.
- SOURCE CODE AS DATA: agents.json:2 "moto.shape, bicycle.rack and bicycle.spokes: the 4.0's own drawing", "Car lines (top, screen, rear, belt, doors, tumble_m, nose_r, tail_r, lifts, bed_from) ...: the 4.0's own drawing, no source behind them", "wagon ... the 4.0's estate lines", "Lamps and trim colours ...: the 4.0's too"; :10 exhaust "Every number is the 4.0's (render/exhaust.ts: ...)"; :13 roadcraft paints "the palettes of its hatch, sedan and first of its SUV"; :64 held "(heldProps.ts of the 4.0)".
- STALE REFERENCE: :2 "spec world.md §5-§6": no world.md in this project (only inside tools/oracle-4.0).
- POSITIONAL: :10 exhaust.classes repeats VehicleType's order by name list; :27 car_body arrays by level index [fine, ..., coarse].
- (agents.json:2 agrees with cook_people.py: MakeHuman (CC0) bodies, Rocketbox (MIT) skeleton and walk.)
#### data chunks 19-20: render/agents.json part 2, buildings.json, cover.json (to :6)
- buildings.json:4 all twelve architectures sit under styles."roadcraft" (the only style): the 4.0 style is the only one buildings have.
- SOURCE CODE AS DATA: buildings.json:118 openings "the openings of the 4.0 (buildingMesh.ts openingOf)" (99 as a "no cap" sentinel in :119-124); :128 lots "measured on the 4.0 small city", max_elements "(the 4.0 houses averaged 54)"; cover.json:2 "as the 4.0 draws them; reach_m 288 is its 720 units" (0.4 m units), "compared by eye with the 4.0 (ports/landscape/reference-minha)".
- A VALUE COPIED BECAUSE OF A WALL BETWEEN TABLES: buildings.json:134 lod.fov_deg 35 "mirrors data/game/camera.json far_fov_deg (native render data cannot read game tables)". Fix: setup_render receives the camera's far fov (or the core reads it from the camera each frame), one owner.
- DERIVED DATA TYPED BY HAND: buildings.json:133 finish_far.tones: each finish's map mean "times 0.91, the mean of the weathering", typed per layer: stale the moment building-finishes.map.png or the weathering changes. Fix: computed by the asset tool from the map, keyed by its input (CLAUDE.md: results keyed by their inputs).
- POSITIONAL: buildings.json:75 era_affinity 12 rows in architecture order; :133 tones by layer order; cover.json:6 props (11 lists) and effects (7) by the order of nature.json elements.kinds (two files, one order).
- "sobrado" (:61): a Portuguese id; :85 a blank line inside the object.
#### data chunk 21: render/effects.json, furniture.json
- SMOKE IGNORES THE WIND: effects.json:3 puffs wind_x 0.6, wind_z 0.8 is the only wind the puffs get (effect_params.cpp:34-35 -> effect_world.cpp:35 set_wind once -> puffs.cpp:58 "wind_dir"; puffs.gdshader:14 defaults it to the same (0.6, 0.8) again, :51-65 drift by it), while the weather's wind is the global wind_x/wind_z (sky_view.gd:161-162) that plants and water follow. Blast smoke, dust and paper always drift north-east whatever the wind tool says. Fix: puffs.gdshader reads the globals (global uniform wind_x, wind_z, wind_ms), the data pair and the uniform default deleted.
- ONE LAW, TWO OWNERS: effects.json:4 fireball base_m 2.5 + per_force_m 1.1 (the 4.0's "actionsWiring.ts" law, :2) and tuning.json strike.scar_radius_m 2.5 + scar_radius_per_force 1.1 (strike_buildings.cpp:37).
- GENERATED FILE OUT OF STEP WITH ITS GENERATOR: furniture.json:56 and :58 both hold "step" (a duplicate JSON key; the parser keeps the last) with "cab" between, while tools/assets/make_furniture.py:31-32 writes "cab" then one "step": the committed file is not the generator's output (edited by hand or left stale). Fix: regenerate; a gate comparing the file with the generator's output.
- effects.json bursts "kind": 5, 2, -1, 6, 7 are puffs.gdshader's kinds by number.
- furniture.json:25 "pewter": ["wood", ...]: a metal's name on a wood finish.
#### data chunk 22: render/interiors.json, materials.json
- interiors.json:4 everything under styles."roadcraft" (the only style).
- DOC AGAINST VALUE: materials.json:2 "pool_gain 0.2115 (road, lamp_glow)" while road's pool_gain is 0.9 (:39) and only lamp_glow holds 0.2115 (:313); :2 "vegetation roughness 0.85" while vegetation's is 0.8 (:267); :2 documents wind_speed_ms and wind_dir_x/wind_dir_z, which no material holds any more (the wind is the shader globals); :2 cites Golus 2021 where vegetation_cards.gdshader:4 cites Golus 2017.
- DERIVED NUMBERS TYPED BY HAND, ACROSS TABLES: lamp_glow pool_gain 0.2115 = the 4.0's pool opacity 0.32 x daylight.json sky_gain 0.661 (stale when sky_gain moves); building_glass furniture_tone (:112, 120 numbers) is each interiors.json/furniture.json palette colour in linear by kind order (0.3231, 0.1441, 0.0561 = oak #9a6a43), written by no tool (grep: only the two shaders read it). Fix: computed at load (the core has both tables) or by make_furniture.py with its input key.
- THE SAME TABLES IN TWO MATERIALS: finish_m, finish_normal_k, finish_metal (13 entries each) in building (:51-98) and building_parts (:167-169); glass_sky "#7f95ad" in building, building_glass, building_far; window_glow 0.62 in building and building_glass; the vegetation sway block four times (vegetation, cover_plant, vegetation_cards, cover_rock/cover_shore, the last two identical).
- SOURCE VALUE: :222 agent_glass "tint the 4.0's cool grey-blue (#7896a2), no source behind it".
#### data chunk 23: render/models.json, overlays.json
- models.json:4 everything under styles."roadcraft" again.
- ONE MEASURE, TWO OWNERS: models.json:5-7 module_m 3, ground_m 3.6, storey_m 3.1 and tuning.json build.module_m 3, primitive_ground_m 3.6, primitive_storey_m 3.1; models.json:10 part_sizes fence [2, 0.12, 1.1] and wall [4, 0.25, 1.8] repeat street_furniture.json:65-72 barrier kinds (fence 1.1/0.12, wall 1.8/0.25), while its railing 1.05 high differs from street_furniture.json:81-84 railing 1.0: the same object at two heights.
- overlays.json:2 "Roadcraft hud.ts:508-522", "Roadcraft lotTool.ts fillAlpha": source code as origin.
#### data chunk 24: render/palette.json
- DEAD DATA: palette.json:2-11 vehicle_tints, :12-21 person_tints, :22-28 foliage_round and :60-64 sky are read by no code (grep of native/src and project: only palette.water in city_water.gd:18-23 and palette.terrain in city_world.gd:99, 245-248); the dead sky colours (#5f8fc4, #c9d6e0) even disagree with the live ones in daylight.json:43-47 (#4d7fc4, #c9dcea).
- CORRUPTED DOC: palette.json:116 terrain _doc has spliced fragments: "the 4.0's strengths of the grass normal map and of the blades.22-0.27 through the inverse of ACES, the light on our ground being twice the albedo at noon ..." and "grain_*: the speckle of the map at the pixel, the 4.0's share.0, compared by the fine contrast ...": sentences cut and glued; the _doc sits at the end of the object.
- STALE REFERENCE: :30 water "the breeze U = 3 m/s (wind_speed_ms of the vegetation material)": no material holds wind_speed_ms (materials.json); the ripple spectrum is fixed at that breeze while the weather's wind runs 0-30 m/s (nature.json:5).
- DERIVED NUMBERS TYPED BY HAND: :30 the water's albedos are screen targets divided by a gain measured once through inverse ACES on one capture (2.27, 1.98, 1.57): stale with any change of light or tone mapping.
- Layout: :52 two keys on a line, :87 mis-indented, upper- and lower-case hex mixed (:3-27, :67).
#### data chunk 25: render/streets.json part 1 (1-203)
- THE DEFAULT MARKINGS ARE THE 4.0'S CODE, AND THE TABLE'S OWN DOC SAYS OTHERWISE: streets.json:29 "br is the 4.0's drawing (markingStyle.ts 'br': dashes 3, 3, 4 m with gaps 6, 9, 12 m ... main.ts) and is the game's default: where the standard and the 4.0 differ in looks the 4.0 is drawn and the standard is a second table"; :30 base widths (centre 0.18, exclusive 0.268, double 0.14 / gap 0.24, zebra period 1.0 / bar 0.5) are the 4.0's; the CONTRAN values sit in :36 "br_norm". Yet :2 states "markings follow CONTRAN's MBST vol. IV ... lines 0.10 m ... dashes 2 m dash, 4 m gap ... zebra bars 0.35 m wide 0.55 m apart": false for what is drawn. "classic" :35 is 3.2/3.2 (8 x 0.4 m, the source's unit). Fix: br_norm's values become the default, the 4.0 entries go.
- STYLES BY INDEX: :29 "it comes last so the indexes of the other styles do not move": saves and Menu > Paint refer to marking styles by position (set_marking_style(int), session.gd:62).
- SOURCE CODE AS ORIGIN: :2 plate_blend_m "the 3.2 m of the 4.0's mesh edge (GROUND_MAX_EDGE)"; :153 wires "(Roadcraft 4.0 world/utilities.ts primarySeats, secondarySeats, spanCarries)", "as the 4.0 draws it", "measured against the 4.0 in play".
- DERIVED BY HAND: :168 wear track_m 1.58 = agents.json sedan width 1.78 less its 205 mm tyre: typed, not computed.
- Layout: :28 a tab among spaces; :172-173 mis-indented.
#### data chunk 26: render/streets.json part 2 (204-433)
- DEAD DATA KEPT FOR A SCHEMA: streets.json:317 "pool_along, pool_sides, pool_lift_m: of the discs the pools were, read by nothing now and kept while the schema asks them" (:318-320): the schema is bent to dead keys instead of both being removed.
- SOURCE CODE AS ORIGIN: :279 barriers "follow the Roadcraft 4.0 look"; :329 plates "follow the Roadcraft 4.0 look: no norm was read for them"; :345 "The drawings are the Roadcraft 4.0 look in both for now"; :347 "src/render/signs.ts PLATE; src/world/utilities.ts and src/render/utilities.ts placePole".
- IN-WORLD WORDS IN RENDER DATA: :394-410 drawings words "PARE", "ESCOLA", "Centro", "Rua", "Informação" (the "Rua" also street_furniture.json:109 street_names.prefix): two owners of the street-plate prefix.
- POSITIONAL: :393 drawings "in SignType order".
- GENERATOR NOISE: :368 "width": 0.08000000000000002 (make_pole_parts.py writes unrounded floats); each style holds a whole copy of the pole (:350-376 and :418-...) differing by a height offset.
#### data chunk 27: render/streets.json part 3, transit.json
- streets.json:482-483 eua/europa placeholders again.
- SOURCE CODE AS ORIGIN: transit.json:2 "Everything else is the 4.0's own drawing (render/transit.ts), no source behind it".
- ONE TRAIN CAR, TWO LENGTHS: traffic.json:241 train_car_m 18 (the sim spaces and stops its cars by it) and transit.json:7 car_m [17.5, ...] (the render draws them): the drawn car is 0.5 m shorter than the simulated one (see the readers below).
- transit.json:6 default_line "#2f7de1" repeats traffic.json:3 line_colours[1].
- (readers) traffic train_car_m: rail_run.cpp:185, 226, 259, 274-276 place car centres every 18 m; transit_parts.cpp:204 draws car_m 17.5 m long: two owners, 0.5 m between drawn cars nobody decided.
#### data chunk 28: ui/builder.json (1-298)
- ENUMS BY NUMBER IN UI DATA: builder.json:2 "value: a bay, scope, pattern, roof or finish code of native/src/sim/buildings/b40/types.h": every tool's value (:52-252) is a C++ enum value typed as a number; extrude/offset/bevel/relief values (100, -100, 400, 60, 200, ±50) have no unit stated (centimetres by their size).
- FLAG COLOURS, THREE OWNERS: builder.json:286-298 flagColour rgb palette; body_survey.cpp:26 FLAG_WHITE 0xffffff, FLAG_DARK 0x111418, FLAG_RED 0xc8102e typed in C++; types.h:98 default rgb {0x0b5ea8, 0xffffff, 0xc8102e} "the Roadcraft's default design (flags.ts DEFAULT_FLAG)" (a source constant).
#### data chunk 29: ui/drawers.json, help.json (start)
- SOURCE CODE AS ORIGIN: drawers.json:2 "Roadcraft dock drawers (specs ui.md 4 and 6)" (see below for ui.md); :132 "(Roadcraft ui/v2/shell.ts renderRoads)".
- ENUMS BY NUMBER, AND ONE MAPPING TWICE: :31-34 zone cards "zone": 1/2/3/0; :54-64 build cards "fn": 5, 6, 1, 4, 3, 10, 11, 9, 2, 13, 0 and :182-197 fn_keys listed in fn order: the building-function enum is written by number in two lists that must agree; :87-93 transit "mode": 0/1/2 (TransitMode; again in transit_overlay.gd:6-8); :305-343 street item, barrier and sign kinds by number (ItemKind, BarrierKind, SignType), with :301 sign_kind 10 and :344 name_kind 11 repeating two of them.
- :236 large_maps lists "km24", a map no screen offers and whose name has no translation (maps.json, data chunk 13).
- (ui.md) drawers.json:2 "specs ui.md 4 and 6": no ui.md in this project (find, outside dist and tools/oracle-4.0): a stale reference to a spec that is not here.
#### data chunk 30: ui/land_elements.json, land_panels.json (1-333)
- ONE ORDER IN FOUR FILES: the 18 element kinds are ordered by nature.json:82 kinds, cover.json:6 props/effects, land_elements.json:9-26 values 0..17 and land_elements.json:43-61 kind_brush (18 positional rows): a kind added or moved in one breaks the other three silently. Fix: keyed by kind id, one table.
- land_elements.json exists only because land_panels was split for size (data_tables.gd).
- NAMES THAT DISAGREE: land_panels.json:309-326 tree kinds "oak", "cypress", "palm" (and nature.json:2 "two oaks, two cypresses, two palms") while the species drawn are streets.json "broadleaf", "broadleaf_b", "conifer", "palm" (placement.planted_models, by index).
- ENUMS BY NUMBER: terrain modes 0..8 and -1 (gully) with "when": 8 / -1 keyed on them (:38-46, :107-146); paint kinds 0..20 by nature.json order (:156-266).
#### data chunk 31: ui/tokens.json, toolbar.json
- A DEAD TABLE WITH A LIVE SCHEMA: toolbar.json is registered in data_tables.gd:43 and checked by tools/gates/rules.json:337 (toolbar.schema.json), but no script reads DataTables.ui("toolbar") (grep). Its content contradicts the live tables: label keys "GROUP_ROADS", "ROAD_STREET" ... exist in no CSV; tool "SERVICE" is no ToolController tool; zone codes res_low 1, res_high 2, com_low 3, com_high 4, industry 5, office 6 against zones.json (res_low 5, ..., no office); terrain flatten 2 / smooth 3 against land_panels.json smooth 2 / flatten 3. Fix: delete the file, its entry and its schema.
- THE ROAD CARDS SHOW ANOTHER ROAD: tokens.json:213 rc_bike "#3f8f55" (green) draws the cycle lane in the catalog cards and section view (road_swatch.gd:43 "rc_" + kind, road_section_view.gd:60) while the 3D cycle lane is streets.json:6 strip_colors.bike "#9a4a3e" (red); rc_curb "#dedbd3" vs curb "#9d9c98", rc_asphalt "#3a3f44" vs "#3a3d3f". Fix: the cards take the street table's colours (one owner); connector_panel.gd:62 also borrows rc_bike for a connector dot.
- ZONE COLOURS, TWO OWNERS: tokens.json:66-69 zone_res #58c26f, zone_com #4aa3e8, zone_ind #e6b84a, zone_mix #b07ad6 (drawer swatches) vs zones.json colors #56bb73, #5da9e9, #d9b254, #b05fd0 (the map overlay): the swatch and the map differ.
- tokens.json:4 and :111 duplicate key "bg_solid"; :2 "Roadcraft shell.css design tokens (specs: ui.md 1.2)" (the source's CSS; ui.md is not in the project); :164-166 mis-indented; world-space values in UI tokens (preview_edge_m, preview_casing_m, preview_kerb_m, preview_lift 0.35 m: the road preview's geometry); transit_bus "#2f7de1" is the third copy of the first line colour; cryptic prefixes rc_/rp_/cn_.

## 3u. `project/tests` (31 chunks): findings by chunk

#### tests chunk 1: core/boot_refusal_test, building_edit_test, building_inside_cases
- NO SHARED HARNESS: the same `func _check(ok: bool, what: String)` with a failures counter and print "ok"/"FAIL" is copied into 34 test scripts (grep), each with its own quit code: one owner (a test base class) is missing.
- THE CLOCK RUNS BEFORE THE TEST HOLDS IT: city_store.gd:69-70 new_map(map, seed, with_city, paused := false), "paused: the clock starts stopped, for a test ... no tick slips in before it says so"; of the 57 new_map calls under project/tests only 10 pass it (grep). E.g. building_edit_test.gd:55-56 new_map(TOWN_MAP, SEED, true) then dispatch set_speed 0: ticks (growth_per_s 25, traffic) run in between for however long the machine takes, so which buildings exist when the test lists them can vary by machine. Fix: every test that reads the sim's state opens paused and steps explicitly (a gate on new_map in tests without the flag).
- PORTUGUESE IN TESTS: building_edit_test.gd:15 SLOT "teste_corpo_editado"; :118-158 case names "andares", "lado", "ala", "recuo", "dividir", "alturas", "linha", "coluna", "zona", "relevo", "ritmo", "telhado", "forma do telhado", "tinta"; :191-192 "%s desfeito", "%s refeito"; :232 "salvou", :235 "abriu"; building_inside_cases.gd:47 "desfeito".
- ENUMS BY NUMBER IN TESTS: building_edit_test.gd:29-44 BAY_WIDE 6, BAY_BALCONY 7, SCOPE_* 0-2, PATTERN_OFFICE 2, ROOF_GABLE 2, ROOF_HIP 3, ROOF_TERRACE 1, FINISH_BRICK 3 (b40/types.h values by hand); building_inside_cases.gd:4 CORE_KINDS [0, 1, 2].
- boot_refusal_test.gd:19 reaches the core as CityStore.get_child(0), past the store's API.
#### tests chunk 2: core/building_light_test, building_model_cases, building_part_cases, building_pick_cases
- THE LOT FRAME REWRITTEN IN TESTS: building_part_cases.gd:105-110 _middle and building_pick_cases.gd:43-46 _on_lot both re-derive b40::lot_frame (origin mid street front, sin/cos of rot) in GDScript; building_pick_cases.gd:16 re-derives a body's height (ground_h + storey_h x (storeys - 1)). A test that recomputes the engine's geometry passes against its own copy. Fix: a probe that returns the frame (or world points) from the core.
- COPIED HELPERS: _back_to and _info identical in building_inside_cases.gd:44-51 and building_model_cases.gd:87-94.
- PORTUGUESE: building_light_test.gd:45 "renderizador:", :51 "parede perto", "parede longe"; building_model_cases.gd:20-29, 42, 46-47, 70 "extrudar", "tijolo", "recuar face", "crescer", "chanfrar", "copiar", "girar", "deslizar", "canto tirado", "ponto movido", "pontos", "cota", "desfeito"; building_part_cases.gd:26, 64 "desfeito", :83 "bandeira arriada".
- CONTRACT COPIES: building_light_test.gd:14-18 LAMP_KIND 4 (shell.h SK_LAMP), WALL_ROUGHNESS 0.86, FAR_ROUGHNESS 0.9 (shader constants by hand), NO_FINISH -1; building_pick_cases.gd:8-9 ROOF_FLAT 0, ROOF_GABLE 2; building_model_cases.gd:11 CHAMFERED 6.
#### tests chunk 3: core/building_plan_cases, building_tool_test, camera_feel_test
- A TEST THAT MEASURES WITH THE DEFECT IT SHOULD CATCH: camera_feel_test.gd:81-83 counts "eye in a building" with CityStore.query("eye_free"), the same query whose building test is by centre box (api §3n: the camera does not see buildings whose centre lies outside the box): the test cannot see the cases the game misses.
- TESTS ON PRIVATE MEMBERS: camera_feel_test.gd:87 _rig._ground, _rig._rise, _rig._eye_ground; :131, :137 _rig._apply(); :152 _rig._settle(): a refactor of the rig breaks the test, and the rig has no public surface for what the test needs.
- THE LOT FRAME A THIRD TIME: building_tool_test.gd:72-80 _roof_ray (with building_part_cases.gd:105-110, building_pick_cases.gd:43-46).
- step_ticks USED BESIDE THE SIM THREAD: camera_feel_test.gd:49-51 new_map (clock running) -> set_speed 0 -> step_ticks(1) on the main thread while the sim thread lives (api §3n: step_ticks unguarded).
- POSITIONAL INDEX INTO UI DATA: building_plan_cases.gd:98 builder tabs.parts.sections[1]; building_tool_test.gd:50-58 interior sections[0].tools[0..2].
- PORTUGUESE: building_plan_cases.gd:31 "forma %d", :46 "paredes levadas", :54 "relevo", :64 "recuo", :68 "ala", :88 "recorte", :104 "desfeito num passo", :110 "desfeito"; building_tool_test.gd:111 "desfeito", :115 "responde", :143 "mover responde", :145 "mover desfeito"; camera_feel_test.gd:176 "FAIL solavanco".
- building_plan_cases.gd:4 SHAPE_SIDES {1: 6, ..., 8: 20} and :12-14 ROOF_MASS 1, CUT_MASS 2, VOID_MODE 1: C++ enums by number.
#### tests chunk 4: core/camera_input_test, camera_road_key_cases, camera_walk_cases, city_mass_test
- PARITY WITH THE 4.0 AS THE PASS RULE (forbidden: never parity arguments): city_mass_test.gd:1-2 "The mass of the generated city, measured as the 4.0's was ... as the same seed gives in the 4.0"; :13 CORE_TOWERS 0.6 "the 4.0, seed 7: 88% and 78% in the two inner bands"; :14 RIM_TOWERS; :17 TWIN_REACH_M "the 4.0's tests/world/variety.spec.ts". Fix: thresholds from this game's design (or the look references), not from the source's output.
- city_mass_test.gd:8 LARGE_MAP := 5: metropolis by its position in maps.json.
- city_mass_test.gd:109-113 its _check prints failures only (printerr) where the other 33 print every line: two output conventions for one runner.
- PORTUGUESE: camera_input_test.gd:78 "Shift+direito", "meio"; :100 "roda: 3 entalhes aproximam (meia altura ...)"; :115 "direito: gira ... (esperado ...)"; :156 "Q gira"; city_mass_test.gd:47 "registros %d centro %s raio %.0f", :105 "andares medio".
#### tests chunk 5: core/edit_fuzz_test, flow_control_test, fuzz_body_edits
- A LOOSENED CHECK ON UNDO: edit_fuzz_test.gd:28-29 "The cells waiting to grow are a queue, not an edit: an undone road leaves its cells there" and :88, :91 compare undo/redo hashes ignoring the "growth" part: the state hash holds the queue, undo does not restore it, and the test exempts it (CLAUDE.md forbids loosening a verification). Fix: either the queue is derived (rebuilt from the network on undo/load, out of the saved state and the hash) or undo restores it; the exemption goes.
- THE SOURCE'S CODE AS THE ORACLE: flow_control_test.gd:1-2 "the translated classes against the Roadcraft 4.0 on one script (the numbers below came from its own code, tools/converter/scripts/call40.mjs, ORACLE)", :8-11 ORACLE values, :50 "the 4.0 gives %s": a test that passes by matching the 4.0's code output.
- TRAFFIC RUN BY FRAMES, NOT TICKS: flow_control_test.gd:55-58 set_speed 2 and 240 process frames, :71-74 1200 frames, then asserts state.measured > 0, advised > 0 and stop_entries >= 40: how many ticks run depends on the machine's frame rate. Fix: open paused and step_ticks a fixed count (as edit_fuzz_test does with new_map(..., true)).
- flow_control_test.gd:16 STOP_NODES 320 "every node of the town" typed for map 0 seed 1; :18 CTL_STOP 3 (NodeControl by number); :34 new_map without the paused flag.
- edit_fuzz_test.gd:136-226 edit kinds as bare numbers (5, 6, 7, 8, 10-18, 18-24, 25) with ranges by comparison; :37-47 constants declared among the vars. fuzz_body_edits.gd:17-39 the same by match on numbers.
- PORTUGUESE: flow_control_test.gd:60 "rodando:", :86 "empurrado:".
#### tests chunk 6: core/inspector_test, interior_test, job_pool_test, lab_test
- step_ticks WHILE THE SIM THREAD RUNS AT NORMAL SPEED: inspector_test.gd:29-30 new_map(TOWN_MAP, SEED, true) (clock running, no set_speed 0) then CityStore.step_ticks(600) on the main thread: two threads ticking one CityState (api §3n: step_ticks takes no guard against the running thread). The concrete call that makes the race real. Fix: step_ticks refuses unless the clock is STOPPED (or takes the sim's place under its locks), and the test opens paused.
- A TEST LEAVES A FILE IN THE PLAYER'S user://: inspector_test.gd:176-181 writes "user://arquivo_ruim.cidade" (the user:// root, not user://saves_tests) and never removes it.
- THE 4.0 AS THE ORACLE: lab_test.gd:1 "the verdict as the 4.0 gives it (answers of its verdictOf, by tools/converter/scripts/call40.mjs)", :16 "(measureCity in the 4.0 refuses a busy GPU)", :83 "The rows the 4.0 answered".
- MEASUREMENT TESTS THAT DEPEND ON THE MACHINE: lab_test.gd:159-177 retries a GPU measure up to CLEAN_TRIES until it is "clean"; :53-55 waits 1.2 s of wall clock twice for the recorder.
- THREE PASS/FAIL OUTPUTS: "FAILURES: n" (most), inspector_test.gd:41 "inspector_test: %d falhas", interior_test.gd:94-95 "OK"/"%d FAILURES" with quit(failures) (the count as exit code).
- inspector_test.gd:159 set_style "roadcraft" enshrines the 4.0 style; :128 marking style 2 by index ("pintura eu"); inspector_test.tscn:5 node named "ShellTest" for the inspector test.
- PORTUGUESE: inspector_test.gd:41 "falhas", :47 "cidade-medida", :55 "save medido", :96 "volume conta entradas", :123 "duplicar", :129 "pintura eu", :176 "arquivo_ruim.cidade", :181 "arquivo ruim recusado", :184 "cidade-quarentena", :189 "quarentena %d"; job_pool_test.gd:12 "fila:"; lab_test.gd:132 "quadro:", "camadas:", :134 "alguma camada desenha", :178 "sombras:".
#### tests chunk 7: core/lamp_light_test, light_color_test, lot_cases
- THE 4.0'S CODE AS THE ORACLE: light_color_test.gd:20 GRADE_TABLE and :131 "The grade against the 4.0's own (postprocess.ts GRADED_OUTPUT_FRAGMENT)", :138 "the 4.0 gives %s".
- (light_color_test.gd:3-4 checks render/tone_map.gd's hand mirror of ACES against the engine: the mirror is guarded, but only when the test runs on both renderers.)
- CONTRACT COPIES: light_color_test.gd:21-36 LENSES 5, LENS_TAG 3, GLOW_TAG 2 (prop_kit.h), ROAD_CONCRETE 5, ROAD_IDS 16, ROAD_PLATE (-1000, 0) (road.gdshader encodings by hand).
- lamp_light_test.gd:26 new_map with the clock running; :47-51 headlamps counted after set_speed 2 and a frame-count settle: how much traffic is near the eye depends on frames, not ticks.
- PORTUGUESE: light_color_test.gd:60 "renderizador:", :89 "lente %d", :123 "brilho noturno".
#### tests chunk 8: core/model_test, people_test
- (model_test's ModelLibrary removals are safe: model_library.gd:6-12 keeps tests in user://models_tests.)
- TESTS PINNED TO DATA COUNTS: model_test.gd:52 list.size() == 53 "42 city models and 11 generic"; :16 LOT_PROJECTS 38: adding a model or a lot project fails the test without any rule broken.
- THE 4.0 AS THE MEASURE: people_test.gd:13 MIN_PARTY_SHARE "the census of the Roadcraft 4.0: 55 %"; :23 NEAR_M "the census of the Roadcraft 4.0: 86 people within this of the camera at 16:00"; model_test.gd:90 "as in the 4.0".
- A MUTATION THROUGH THE QUERY ROUTE: people_test.gd:42, 75 CityStore.query("set_city_hour", ...) changes the clock via StoreApi QUERIES (store_api mixes commands into queries: no mark, no undo boundary).
- model_test.gd:194 writes user://model_test_house.png into the user:// root.
- PORTUGUESE: model_test.gd:80 "igreja:", :165 "Minha casa", :180 "palco:"; people_test.gd:39 "mapa gerado", :45 "censo:", :46 "perto:", "em", :49 "portas lidas", :55 "idosos", :58 "par longe", :60 "corpos:", :62 "par encostado", :66 "depois:", :124 "atravessando", "esperando".
#### tests chunk 9: core/quality_test, reference_test
- THE TEST BLESSES A DEAD SETTING: quality_test.gd:240-244 checks only that low draws no clouds and ultra does (sky_view clouds_drawn); nothing checks cloud_shadows, which has no effect of its own (data chunk 7).
- FRAME-COUNT WAITS FOR LIVE TRAFFIC: quality_test.gd:111-115 up to CAR_FRAMES 1800 frames for a car to come near, with the clock running from new_map (:43).
- quality_test.tscn:5 node named "ShellTest" again (copied scene).
- PORTUGUESE: quality_test.gd:54 "falhas", :172 "camada Grade liga", :212/:217 "nivel_antigo"; reference_test.gd:34 "cinco fotos".
#### tests chunk 10: core/road_commands_test
- road_commands_test.gd:25 FILLED_STYLES ["roadcraft", "brasil"] keeps the 4.0 style a tested requirement; :89 "(the 4.0's worldAtScreen)".
- road_commands_test.gd:28 DECK_OVER_GROUND_M 0.3 with the comment "the ground under a road is dropped 0.6 m below its deck": the bound is half the stated drop with no reason given.
- PORTUGUESE: :50 "construir:", :54 "segmentos", :62 "zona:", :63 "zoneou lados", :76 "cresceram:", :80 "demolir:", "dinheiro", :81 "demoliu", :120 "fim encaixa no eixo e na grade", :204 "estilo %s: placa %s".
#### tests chunk 11: core/road_edit_test, road_grade_seam_cases
- PARITY WITH THE 4.0 AS THE PASS RULE: road_edit_test.gd:65 "with the 4.0's widths and prices", :69 URBAN_PRICE 1520 "an urban road as in the 4.0" (the roadcraft style's prices pinned in a test), :75 "the 4.0's line", :199/:208 "the curve bends as the 4.0's"; road_grade_seam_cases.gd:1 "against the 4.0's tests/world/elevation.spec.ts", :42 "the 4.0's matrix", and every length in t.U, the 4.0's 0.4 m unit.
- PORTUGUESE: road_edit_test.gd:77 "largura total"; road_grade_seam_cases.gd:45, 61 layout names "chão", "toda elevada", "elevada sobre chão", "elevada pousando nas ruas".
- road_edit_test.gd:23 RULE_STOP 1 by number; road_grade_seam_cases.gd:17-73 untyped vars (var bored = ...) against the project's typed style, :61 a tab run inside an expression.
#### tests chunk 12: core/road_grade_test, road_height_cases
- A PORT OF THE SOURCE'S TEST, WITH ITS NUMBERS: road_grade_test.gd:1-3 "The road heights solver against the Roadcraft 4.0's tests/world/elevation.spec.ts, its scenes and its bounds (lengths in the 4.0's 0.4 m units, U, turned to metres)"; :10 SAG "(the 4.0's anatomy)", :79 "the 4.0's measure", :103 "the 4.0's hill(...)", :126 "the 4.0 test reads onSegment", :137 "as the 4.0's anatomy()", :187 "(the 4.0's elevated())"; thresholds 0.052, 0.162, 0.165, 100 U taken from it.
- TUNING VALUES TYPED INTO TESTS: road_grade_test.gd:7-9 CLEARANCE 0.12 (grade.clearance_m), ELEVATED 5.6 (grade.clear_elevated_m), BORE 4.4 + 0.6 + 0.4 + 0.16 (tunnel headroom, arch, ...); road_height_cases.gd:14 GRADE 0.12 (grade.authored): copies, not DataTables.tuning reads; a tuning change leaves the tests checking the old numbers.
- PORTUGUESE: road_grade_test.gd:200 "sozinha".
#### tests chunk 13: core/road_money_cases, road_shape_cases, save_land_cases
- THE 4.0 AS THE MEASURE: road_money_cases.gd:1 "The money rules ... measured on the 4.0", :25 BUILD_TABLE "(the 4.0's buildLengths; ...)"; :177 the cost read from styles[DataTables.REGIONAL_STYLE] (the hard-wired "roadcraft").
- COPIES AND POSITIONS: road_money_cases.gd:15 START_FUNDS 20000000 (tuning economy.start_money typed); :16-17 WIDE_PROFILE 4, NARROW_PROFILE 8 (profiles by catalog position); save_land_cases.gd:8-13 the save header's byte offsets (version at 4, map at 8, tiles at 16) and OLD_HALF_M 5120 typed: the persistence format's layout copied into a test.
- MUTATION THROUGH THE QUERY ROUTE: road_money_cases.gd:73 query("probe_set_money").
- PORTUGUESE: road_money_cases.gd:143 "perfil mais barato devolveu", :146 "perfil mais caro cobrou", :163 "melhorar", :167 "%s recusada", :189 "modos %s".
#### tests chunk 14: core/save_undo_test, shell_test
- SOURCE REFERENCES: save_undo_test.gd:143 "(the 4.0's readEconomy)"; shell_test.gd:193 "a panel at the 4.0's width".
- save_undo_test.gd:30 MARKING_STYLES := 2 while streets.json has five marking styles: the test cycles 0/1 under a name that states a false count; :13 UPGRADE_PROFILE 3, :25 PARK_FN 6 by number.
- TESTS ON PRIVATE MEMBERS: shell_test.gd:162, 168 StartScreen._pick(...).
- PORTUGUESE: save_undo_test.gd:26 SLOT "teste_ida_e_volta", :69-71 "melhorar", "dividir", "demolir", :84 "relevo subiu", :93-96 "clima", "nuvem", "neblina", "solo pintado", :116 "mudou", :118/:139 "desfeito", :120/:140 "refeito", :153 "recusou saldo infinito", :181 "salvou", :186 "abrir:", :190 "abriu", :195 "mesmas alturas"; shell_test.gd:55 "Chip desconhecido", :148 "guardado, nunca apagado", :157 "nao_existe", :222-223 "teste", "detalhe".
#### tests chunk 15: core/shoot_fright_cases, shoot_test, signal_cases, street_crossing_test
- shoot_test.gd:24 SHOTS_TO_BURST 14 while blast.json shot.vehicle_shots_to_burst is 12: the test's own number, not the data's; :26-27 MOTO 5, FIRST_HEAVY 6 (VehicleType by number); street_crossing_test.gd:9 ZEBRA 1.
- PORTUGUESE: shoot_fright_cases.gd:43 "perto", :55 "de"; shoot_test.gd:207 "desfez", :227 "salvou", :231 "abriu"; signal_cases.gd:28-31 "tempo fixo", "desfeito", "refeito", :38 "onda verde:", :41 "onda verde desfeita num passo", :55 "painel desfeito"; street_crossing_test.gd:70 "parados", :71 "gente atravessa".
#### tests chunk 16: core/street_move_cases, street_save_cases, street_works_test
- SOURCE REFERENCES: street_move_cases.gd:23 "(Roadcraft nodeMover.ts carryStreetKit)"; street_works_test.gd:197 "as the 4.0".
- CONTRACT COPIES: street_save_cases.gd:9 TAG_BYTES "STRT" (save_street.h); street_works_test.gd:13-17, 27-28 ITEM_BENCH 2, ITEM_BIN 3, ITEM_SIGN 10, FENCE 0, SIGN_STOP 0, CTL_PRIORITY 2, CONTROL_PRIORITY 1 (enums by number).
- A FOURTH OUTPUT FORMAT: street_works_test.gd:224 "ok: " / "FAIL: ".
- PORTUGUESE: street_save_cases.gd:8 SLOT "teste_rua", :25 "contagens:", :32 "abrir", :35 "save guarda"; street_works_test.gd:109 "plano rural", :137 "banco colocado", :138 "banco contado", :140 "lugar ocupado recusa", :168 "barreira contada", :174 "barreira curta recusa".
#### tests chunk 17: core/strike_test, traffic_hash_test, transit_test
- A TEST WHOSE SAVE CHECKS NEVER RUN: transit_test.gd:35 calls _undo_and_save() without await; that function awaits CityStore.saved (:137) and CityStore.loaded (:141), so it returns at its first await and :36-37 print the result and quit(failures) at once: "a new map with no lines" and "the plan comes back from the file" (:139, :143) are never checked, and the transit save path is untested while the test passes. Fix: `await _undo_and_save()`. (Scan of every .gd for coroutines called without await: the other hits are deliberate fire-and-forget (lab_recorder.gd:33/52 _open with a cut guard, live_driver.gd:35, play.gd:32), and reference_hand.gd:27 act() returns {"ok": true} while build() still awaits its capture: the caller is told the build succeeded before it ran.)
- transit_test.gd:37 quit(failures) (the count as exit code) and :36 a fifth output format; :8 TRAIN 1 by number.
- PORTUGUESE: strike_test.gd:99 "1º golpe", :100 "golpe aceito", :102 "golpe contado", :108 "golpes", :115 "golpe grande", :116 "cratera registrada", :118 "desfez", :122 "refez"; transit_test.gd:11 SLOT "teste_transporte", :50 "paradas postas", :115 "trilho", :125 "trens:".
#### tests chunk 18: core/yard_bench, yard_test; e2e/e2e
- STALE COMMENT ON DETERMINISM: yard_test.gd:19 PLAN_STEPS "the plans run one at a time on a worker, by the clock: the steps only bound the wait"; lorry_traffic.cpp:216 advances the planner by lorry_plan_work units each tick (deterministic, traffic.json:390). The comment describes an older design.
- A BOUND FAR BELOW WHAT IS MEASURED: yard_test.gd:26 WAY_SHARE 0.9 "measured 0.99 (105 of 106 docks)": a regression from 99 % to 90 % passes.
- A TOOL AMONG THE TESTS: yard_bench.gd:4 "A tool, not one of the tests" lives in project/tests/core.
- CONTRACT COPIES: yard_bench.gd:14 / yard_test.gd:13 ZONE_INDUSTRIAL 3 "catalog/zones.json order"; yard_test.gd:25 NO_STOP 1 (YardReach); e2e.gd:14 SUV 2, :16 TURN_MS 6.11 (traffic turn_speed_kmh 22 typed in m/s).
- PORTUGUESE: yard_bench.gd:20 SLOT "bancada_patios", :71 "docas"; yard_test.gd:40 "aquecido", :74 "lugar", "zona", :81 "docas", :93 "planejado", :96 "planejadas", "chamadas", "pior plano", "pior tick", :108 "fim", :116 "marca"; e2e.gd:38 "mapa gerado", :44 "pessoas, desenhados", :54 "lentes acesas".
#### tests chunk 19: perf/camera_pan, layer_gpu, main_thread_probe
- THE FRAME SECTIONS LISTED A THIRD TIME: camera_pan.gd:44-45 COST_PARTS (agents, lock, stream, levels, flush, roads, props, buildings, chunk_max, super_max, b_scan ... trees, overlays) beside lab_views CORE_SECTIONS and health_log COST_PARTS (which already disagree): the render_cost keys have no owner the scripts read.
- SHIMS FOR VERSIONS THAT ARE GONE: main_thread_probe.gd:112-122 "Reads an older core lacks are left out", "an older fog_field took the camera", "an older store has neither the method nor its table": compatibility branches for a past API.
- COPIES: main_thread_probe.gd:10 MAP_DEFAULT 5 (metropolis by position), :15 SPEED_FASTEST 3 typed (inspector_test.gd reads CityStore.constants.speed_fastest).
- layer_gpu.gd:20 ignores boot's errors; :36 0.03 ms threshold typed.
- PORTUGUESE: camera_pan.gd:144-159 movement names "parada", "cruzar", "circular", "girar", "perto", :141 "malhas:", :202 "quadros", :210 "piores:"; layer_gpu.gd:7 views "perto", "rua", "bairro", "cidade", :37 "sombra", "chamadas"; main_thread_probe.gd:83 "quadros:", :119 "leitura".
#### tests chunk 20: perf/open_probe, pending_probe, tier_gpu
- INVESTIGATION PROBES KEPT IN THE TREE: pending_probe.gd:1 "Investigation of the open items (goal log)" (shadow variants, click timings, pause wake-ups, pacing) and main_thread_probe.gd: measurement scripts of past investigations, against CLAUDE.md "Investigar é ler código. Sem medições, perfis nem testes longos e pesados para investigar".
- COPIES: pending_probe.gd:12 MAP_DEFAULT 5, :18 SPEED_FASTEST 3 (the third copy of both); tier_gpu.gd:13 BUDGET_MS 16.7 typed beside render_switches.json doctor.budget_fps 60; tier_gpu.gd:24 and layer_gpu ignore boot errors.
- tier_gpu.tscn:5 node named "CameraPan" (copied scene).
- PORTUGUESE: pending_probe.gd:81 "como configurado", :89 "agentes:", :98/:112 "PAUSA_INICIO"/"PAUSA_FIM", :143-144 "nenhum", "cap+vsync de novo", :145 "tela:", :182 "10%% mais lentos", :183 "demais 90%%", :184 "piores 5"; tier_gpu.gd:9 "rua", "bairro", "cidade", :58 "cabe", "estoura".
#### tests chunk 21: visual/capture, capture_blast, capture_clock, capture_crossing
- STALE REFERENCE: capture.gd:2-3 "Views match the Roadcraft reference shots (docs/referencia-visual ...)": there is no docs/ folder in the project.
- THE 4.0'S CODE VALUES FOR A CAMERA: capture_blast.gd:13-15 REF_CAMERA_OFFSET / REF_FOV_DEG "The reference game's camera ... read out of its own page (viewCamera() after setView at zoom 8 ...)"; :23 "(look-demolition-5.json)". (Matching the 4.0's photographs is the look target; reading its camera code is not.)
- TESTS ON PRIVATE MEMBERS: capture_clock.gd:30-33 hud.top_bar._sky_buttons, _speed_buttons.
- CONTRACT COPIES: capture.gd:9 CHUNK_NEAR_BUILT 1 (building_renderer.h), :16 FASTEST_SPEED 3, :27-28 CIVIC_SHOTS by BuildingFn number; capture_blast.gd:18 DECAL_HALF_PER_RADIUS 2.4 (marks.h CRATER_SCALE); capture_clock.gd:11 FASTEST 3 (the fourth copy of the speed).
- capture.gd:96-99 Settings.apply called twice in a row.
- PORTUGUESE (capture file names are Portuguese throughout): capture.gd:27-28 "escola", "igreja", "hospital", "clinica", "praca", "parque", "posto", "mercado", "prefeitura", "policia", "bombeiros", "shopping", "quadra", :143 "09_retido", :193 "14_pessoa", :199 "civico", prints :60 "desgaste:", :61 "pessoas", :65 "sonda:", "andando", "parados", :69 "medido", :73 "desenhados:", :75 "fim", :97 "qualidade:", :109 "cidade", :110 "ms/quadro", :182 "captura:"; capture_blast.gd:41-52 "terra_f", "topo_f", :58 "ponto achado", :66 "antes", :68 "sonda:", :74 "golpe", :96-98 "escala: centro", "pixels por metro", "raio fisico", "px em x e", "em z", :114 "a"; capture_clock.gd:30-32 "modo_noite", "modo_dia", "modo_relogio", :40 "relogio_%d", :41 "pronto", :54 "hora", "modo", "sol", "noite"; capture_crossing.gd:13-24 "15_travessia", "16_travessia_perto", "16_pintura_%s".
#### tests chunk 22: visual/capture_demolition, capture_directed, capture_falls, capture_features
- A WRONG ENUM COPY: capture_demolition.gd:28 NORMAL_SPEED := 2 "# SimSpeed::NORMAL", but simulation.h:15 has PAUSED 0, NORMAL 1, FAST 2: :189 runs the wound clocks at FAST while it says normal. The cause is the copied number: CityStore.constants exposes only speed_paused and speed_fastest (city_core_query.cpp:95-96), so scripts type the others. Fix: the core exposes every SimSpeed value; scripts read them.
- TESTS ON PRIVATE MEMBERS: capture_features.gd:83 hud.top_bar._history_buttons, :93-109 hud.drawer._tabs, :95 hud.drawer.catalog._cards_row.
- STALE RUN NOTE: capture_features.gd:4 "Run under xvfb ... --capture-dir=/tmp/x" (Linux) while captures run hidden through tools/tasks.
- POSITIONAL: capture_features.gd:15-16 MODE_RAISE 0, MODE_MESA 4, :172-174 cards[1] "parque", cards[2] "escola", :183 zone 2; capture_demolition.gd:216 roof at 3.0 m a floor typed.
- PORTUGUESE: capture_demolition.gd phases "pareada", "torre", "derrubados", "topo", files "bomba_f%d", "bomba_f%d_grande", "ped_parado_%d", "ped_tronco", "ped_perna", "ped_cabeca", "carro_antes", "carro_tiro", "carro_estouro_t%d", :52 "captura:", "escala", :188 "acertou"; capture_directed.gd:8-18 "transporte", "pessoas", "tiro", "lotes", "clipes", "varredura"; capture_falls.gd:21 "torre", :38 "derrubado_%02d"; capture_features.gd:1-2 "Melhorar/Dividir/Rotatória", :82-229 files "feat_undo_2_desfeita", "feat_undo_3_refeita", "feat_upgrade_1_previa", "feat_upgrade_2_avenida", "feat_roundabout_1_previa", "feat_roundabout_2_construida", "feat_terrain_1_chapada", "feat_terrain_2_desfeito", "feat_build_parque_escola", "feat_save_1_abrir", "feat_save_2_aberto", :145 "relevo: chapada subiu", :237 "linha ausente".
#### tests chunk 23: visual/capture_flag, capture_interior, capture_kerb, capture_landscape
- CAPTURES PINNED TO GENERATED IDS: capture_interior.gd:11-18 building ids 146, 695, 1090 of map 0 seed 7 typed: any change to city generation points the shots at other buildings (or none).
- PRIVATE METHOD BY NAME: capture_interior.gd:72 layer.call("_send", true, s[0], s[1]) (InteriorController's private _send through call()).
- THE 4.0'S CODE API AS REFERENCE: capture_landscape.gd:12 "the 4.0 runs' setView", :20 "the view the 4.0 opens on", :36 "the 4.0's always day", :88 "(`__roadcraft.setView`, render/isoViewport.ts)"; capture_flag.gd:2 "the Roadcraft's day hour (16h)".
- COPIES: capture_flag.gd:20 LANTERN_H 19.4 (builder.json lantern size), :76 body height re-derived; capture_landscape.gd:29 ELEMENT_KINDS 18, :222 terrain mode 8 (river) by number.
- PORTUGUESE: capture_flag.gd:36 "feat_build_bandeira"; capture_interior.gd:12-17 "casa146-terreo", "casa146-andar1-giro90", "torre695-andar10", "comercial1090-andar1", "casa146-noite-terreo", "cidade-camada-146"; capture_kerb.gd:21-22 "20_rebaixo", "20_rebaixo_frente"; capture_landscape.gd shot names "cidade", "elementos", "elementos_perto", "elementos_longe", "luz_cidade", "luz_rua", "luz_parede", "ceu_baixo", "ceu_alto", "ceu_noite", "noite_campo", "noite_cidade", "relogio_%d", "modo_", "campo_perto", "campo_meio", "matas_meio", "matas_alto", "horizonte", "chuva", "nuvens_sombra", "nuvens_ceu", "neblina", "chao_pintado", "cobertura", "plantadas", "rio", "mar"; --only keys "rampa", "cubo_a", "lamina", "trama_3_10", "solta_palma", "arv_perto"; prints "janela", "pronto", "inclinacao", "lente", "meia altura", "olho", "relogio: hora", "sol", "noite".
#### tests chunk 24: visual/capture_lorry, capture_lots, capture_memory, capture_money, capture_pair, capture_parity
- THE SOURCE GAME'S CITY LAID INTO THIS GAME: capture_pair.gd:3-5, 19, 72-100 reads a file of "the 4.0's building" documents (--buildings), turns each into a body through the core's doc_body (store_api.gd:55 builder/doc_body, city_core_models.cpp:56, body_doc.cpp) and places them at the 4.0's coordinates, keeping "the 4.0's building id -> the id it has here": the shipped DLL carries a reader for the source game's building documents so captures can stand the 4.0's town in this one. (Pairing photographs is the look target; importing the source's city data is not.)
- capture_money.gd:2-3 "Same steps and camera as the 4.0's run", :19 STREET "the card the 4.0 opens on", :50 "as the 4.0's run"; :17 START_FUNDS 20000000 typed; :121-125 hud.drawer.catalog._cards_row (private).
- COPIES: capture_lorry.gd:6 ZONE_INDUSTRIAL 3; capture_pair.gd:15 ZONE_OF names to codes; capture_lots.gd:17 hours 22.0/16.0 typed (daylight.json day_hour/night_hour).
- PORTUGUESE: capture_lorry.gd:51 "passo", :54 "36_doca_%d", key "doca"; capture_lots.gd:17 "noite", :35 "lote_", :36 "dentro", :39 "lote_dentro_", :40 "aberto:"; capture_memory.gd:11 "sistema usado"; capture_money.gd shot names "dinheiro_barra", "catalogo", "via_na_mao", "via_sem_dinheiro", "recusa_clique", "via_paga", "demolir_mira", "via_demolida", "demolir_gerada_mira", "gerada_demolida", states "inicio", "desfeita", "gerada demolida", prints "janela", "perfil", "dinheiro", "segmentos", "pronto"; capture_pair.gd:90 "documento recusado:"; capture_parity.gd:39 "salto", :51 "paridade:", "tentativa", "em", key "paridade".
#### tests chunk 25: visual/capture_people, capture_road_tools, capture_roads
- SHIMS FOR OLD BUILDS: capture_roads.gd:2-3 "Works on a build without the road paths too (no px argument, no freehand), so one script takes the "before" and the "after"", :132 `if StoreApi.COMMANDS.has(&"build_road_path")`; capture_road_tools.gd:29-32 the same and `if ToolController.Tool.has("MOVE")`: branches for a past API.
- MISPLACED DOC: capture_roads.gd:199 "The tool as the player uses it: the drawer, a stretch held by a click ..." sits on _check (it describes CaptureRoadTools).
- SOURCE REFERENCES: capture_roads.gd:72 "the 4.0's free ground is a bare field" (clear_trees to match it), :175 "(`__roadcraft.setView`)"; capture_people.gd:3 "as the pictures of the Roadcraft 4.0 frame them".
- COPIES: capture_roads.gd:25-26 FURNITURE_LAMP 1, FURNITURE_COLUMN 6 (FurnitureKind by number); :4 "/tmp/x" run note.
- MUTATION THROUGH THE QUERY ROUTE: capture_people.gd:49, 68, 111 query("set_city_hour").
- PORTUGUESE: capture_people.gd:13-15 "30_casal", "31_familia", "32_crianca", "33_idoso", "34_saindo", "35_entrando", "36_espera", "39_celular", "40_sacola", "41_conversa", "42_olhando", "43_fones", "44_ouvindo", :22 "45_agachado", "46_filmando", :60/:97 "_rosto", :62/:94 "_perto", :70 "37_multidao", :116 "clipe"; capture_road_tools.gd:36 "Controle and Mover tabs"; capture_roads.gd:171, 181 "de novo".
#### tests chunk 26: visual/capture_shell, capture_signals, capture_soak, capture_street
- TRANSLATION SURGERY COPIED INTO A CAPTURE: capture_street.gd:142 tr(key).get_slice(" (", 0): the UI's cut of a label at " (" (three places in the UI, project chunk notes) repeated to find a button by its text.
- PRIVATE MEMBERS: capture_shell.gd:56 hud.top_bar._sky_buttons, :89/:105 hud._dialog, :114 Health._slow_frame, :120-122 hud.health._select; capture_street.gd:102 hud.drawer._tabs.
- COPIES: capture_shell.gd:16-18 SPEED_PAUSED 0, SPEED_NORMAL 1, SPEED_FASTEST 3 (the fifth copy); capture_soak.gd:8 TICKS_PER_SECOND 20 (tuning sim.tick_hz); capture_street.gd:8-9 DAY 16, NIGHT 22, :10-16 ITEM_BIN 3, ITEM_SIGN 10, FENCE 0, LAMPS_ALL 2, URBAN 0.
- MISPLACED DOC: capture_shell.gd:199-200 two comments stacked on _tabs, the first ("The clock stands still between the photos ...") belonging to _shot.
- PORTUGUESE: capture_shell.gd:106 "fotos:", :214 "hora"; capture_signals.gd:19 "19_semaforo_%s_%d", "dia", "noite"; capture_soak.gd:55 "entradas no vermelho %d (%d paradas)", :59 "retido:"; capture_street.gd:84 "dica:", :103 "rua_01_aba", :161 "rua_02_postes_opcoes", :168 "rua_03_postes_previa", :176 "trecho encadeado", :177 "rua_04_postes_prontos", :221 "rua_05_pecas", :109 "contagens:".
#### tests chunk 27: visual/capture_street_gestures, capture_street_pairs, capture_street_views, capture_sweep
- A DOC COMMENT INSIDE ANOTHER FUNCTION: capture_street_views.gd:36 "## Each sign of the dropdown on the footway across the street ..." stands at column 0 between views()'s for loop and its last two lines (:37-38, still indented, still part of views()); the comment belongs to signs() at :41. Reading the file, views() seems to end at :33 and the pairs/names calls seem to float.
- THE 4.0 AS THE FRAME: capture_street_pairs.gd:1-4 "The pairs with the Roadcraft 4.0 ... as they did in the 4.0", :87 "the way they did in the 4.0's map".
- TEST-ONLY STATE IN THE GAME: capture_street_views.gd:110, :133-136 read SignSheet.drawn, the list sign_sheet.gd:17 keeps for the whole city only for this capture.
- COPIES: capture_street_views.gd:14 ITEM_NAME 11, :15 NAMES_TRIED 312 "three times what one sheet of 104 held" (the sheet's capacity typed); capture_street_gestures.gd:8-9 LEVEL_BASIC 1, LEVEL_FULL 0; capture_sweep.gd:14 SCHOOL 1 (BuildingFn).
- PORTUGUESE: capture_street_gestures.gd:2 "Mobiliário", :55 "rua_08_conjunto_via", :78 "barreira", :81 "rua_06_barreiras"; capture_street_pairs.gd:14-19 keys "banco", "lixeira", "hidrante", "luminaria", "cerca", "muro", "sebe", "defensa", "gradil", "poste", "sol-rua", :45-55 "noite", "perto", "semaforo", "esquina", "placa", :100 "par_", :25 "qualidade:", "escala"; capture_street_views.gd:29-33 "dia", "noite", "rua_10_rua_", "rua_11_volta_", "rua_13_ao_longo_", "rua_12_alto_", :73 "placa %d colocada", :74 "placa_%d", :77 "rua_07_placa_opcoes", :108 "rua_23_folha_%d", :131 "Rua Teste %d", :149 "rua_20_", :154 "rua_22_placa_%d"; capture_sweep.gd:17-20 "cruzamento", "miolo", "borda", "escola" (and its "30_" prefix also numbers capture_people's "30_casal").
#### tests chunk 28: visual/capture_traffic, capture_transit, capture_transit_site
- A FLAG THAT NEVER MATCHES: capture_parity.gd:1 documents "--only=paridade --pairs=<file>", but capture_traffic.gd:25 dispatches on cap.only.has("parity") (and capture_parity.gd:51 prints "paridade:"): the documented flag never reaches the parity shots. Fix: one key, from one constant.
- THE 4.0'S CAMERA POSES: capture_transit.gd:2-3 "seen from where the photos of the 4.0 stand (ports/transit/evidence/look)", :23 "(the 4.0's __rec of each photo)"; capture_transit_site.gd:1-2 "the 4.0's stops stand on a street with open ground across it ... gives the same poses".
- MUTATION THROUGH THE QUERY ROUTE: capture_transit.gd:65, 67 query("transit_xray", [true/false]).
- COPIES: capture_transit.gd:7-8 TRAIN 1, METRO 2.
- PORTUGUESE: capture_traffic.gd:9-10 "30_vaga_rua", "31_vaga_lote", "32_carro_manobra", "33_bicicleta", "35_ciclofaixa", :48 "37_embarque", keys "doca", "embarque", "trafego" (and a third use of the "30_" prefix); capture_transit.gd:47-249 "39_sem_transporte", "39_com_o_plano", "transporte:", "paradas", "trilho", "40_parada_perto", "41_parada_alto", "48_metro_entrada", "49_metro_corte", "39_com_as_linhas", "quadro", "50_ferramenta_linha", "51_ferramenta_trilho", "trilhos", "52_gaveta_linhas", "42_terminal", "43_onibus", "43_onibus_portas", "44_trilho_perto", "45_passagem", "46_estacao_perto", "47_estacao_predio", "47_estacao_alto".
#### tests chunk 29: visual/capture_ui, capture_vehicles, capture_views, capture_wait, frozen_plan.json
- THE WHOLE VehicleType ENUM COPIED: capture_vehicles.gd:7-16 SEDAN 0 ... BICYCLE 8 "sim VehicleType (traffic_params.h)" by hand.
- CAPTURE NUMBERING COLLIDES: the numeric prefixes that order the shots are reused for different things: "09_" (capture.gd:143 "09_retido", capture_vehicles.gd:51 "09_carro_orbita"), "12_" (capture.gd:213 civic "12_escola", capture_views.gd:19 "12_encosta"), "30_" (capture_people, capture_sweep, capture_traffic), "28_" four vehicles.
- COPIES: capture_views.gd:6-7 DAY 16, NIGHT 22 (daylight.json day_hour/night_hour); capture_ui.gd:21 PROFILE_CATEGORY "avenidas", :62 hud.drawer.catalog._cards_row (private), :77 pick_zone(1); capture_ui.gd:4-5 the xvfb /tmp run note again.
- frozen_plan.json:13-43 steps named "setView" (the source game's API name) for the golden of quick.sh.
- PORTUGUESE: capture_ui.gd:64-97 "ui_01_gaveta_vias", "ui_02_previa_via", "ui_03_via_construida", "ui_04_gaveta_zonas", "ui_05_crescimento_e_transito", "ui_06_inspecionar", "ui_07_demolir", :73 "cobrou", :85 "cresceram", "pessoas", :162 "captura:"; capture_vehicles.gd:27-51 "22_onibus", "_lado", "_alto", "23_caminhao", "24_moto_longe", "27_bicicleta", "25_van_tras", "26_sedan_tras", "28_hatch_perto", "28_picape", "07_carro", "08_carro_alto", "09_carro_orbita_%d"; capture_views.gd:10-23 "01_rua", "02_bairro", "03_cidade", "04_rua_baixa", "05_transito", "06_cruzamento", "13_calcada", "10_noite", "11_relevo", "12_encosta", "17_horizonte", "18_alto", "21_noite_bairro", "21_noite_cidade"; capture_wait.gd:26 "espera", "quadros", "pendente", "pedir", "sem_vez", "pedidos", "prontos", "orcamento", "fila", "substituidos"; frozen_plan.json:22-52 "cidade", "bairro", "rua", "rua_noite".
#### tests chunk 30: visual/landscape_cards, live_driver
- THE 4.0 AS THE FRAME: landscape_cards.gd:1 "with the same camera as the 4.0's runs", :200 "from the distance of the 4.0's run", :209 "at the same spot as in the 4.0's run".
- landscape_cards.gd:17 SHADOW_FILTERS [2, 3, 4, 5] (ShadowQuality by number); :87 sets the renderer's global soft-shadow filter quality and never restores it.
- live_driver.gd:54 hud.actions._open_new (private); a step executor for any dispatch/query from a file (loaded by play.gd only with --live; tests/ left out of the export).
- PORTUGUESE: landscape_cards.gd shot names "rampa", "cubo_a", "cubo_b", "trama_%d_%d", "lamina_vazia", "lamina", "solta_palma", "solta_cipreste", "solta_carvalho", "solta_palma_perto", "arv_perto", "arv_copa", "arv_media", "arv_longe", "arv_alto"; prints :103 "escada vies", "normal", "fim", "lado", "cortes", :108 "escada", :127 "degrau".
#### tests chunk 31: visual/look, look_aim, look_measure, pair_camera, ui_driver
- TWO "WAIT UNTIL DRAWN" WITH DIFFERENT RULES: capture_wait.gd:7-30 waits up to 400 frames and counts a photo that leaves with drawing pending (exit code fails); ui_driver.gd:7, 127-133 drawn() waits up to 600 frames and counts nothing. Captures that call ui.drawn() then ui.shot() (capture_features, capture_money, capture_street*, capture_demolition, capture_blast, capture_pair, capture_interior ...) can save incomplete photos and still pass. Fix: one wait (CaptureWait), ui.drawn() delegating to it.
- A GENERIC HELPER WITH A HARD-CODED SHOT: ui_driver.gd:96-103 drag() always saves "feat_build_previa.png" mid-drag: every capture that drags writes (and overwrites) that one file.
- THE VISUAL TOOLING IS A PORT OF THE SOURCE'S: look.gd:1-5 "as the Roadcraft 4.0's scripts/look.mjs does ... one plan serves both games (the same setView ...)", :145 "the `press` of scripts/look.mjs"; look_measure.gd:1 "as the Roadcraft 4.0's look.mjs takes them"; look_aim.gd:2 "(the 4.0's plan picks the same with `sim.pedViews`)"; pair_camera.gd:1-2 "The camera of a photo of the Roadcraft 4.0: its `viewCamera`", :8 "the 4.0's zoom", :10 PRESET "high" "the quality the 4.0 takes its photos at", :11-12 WORN_ARMS "(look-par13)".
- MUTATION THROUGH THE QUERY ROUTE: pair_camera.gd:36 query("wear_paint").
- look.gd:98-99 "wait" steps sleep by wall clock.
- PORTUGUESE: ui_driver.gd:3 "passo: ...", :68 "clique em", :88 "segura em", :97 "arrasta de %s a %s", :107 "tecla", :128 "espera o render", :140 "captura:", :152 "passo:".

## 3v. `project/assets` (15 chunks), configuration and `addons/terrain_3d`: findings

#### assets chunk 1: assets/lots (artDeco, balconyMid, bank) and the lot pipeline
- THE 4.0'S BUILDINGS SHIPPED AS DATA: tools/assets/cook_signatures.py:1-4 "Cooks the signature buildings of the Roadcraft 4.0 into project/assets/lots: what its lot lab draws for each project, geometry and pictures, as data. Input: the report of a look.mjs run that ran buildSignature on each lot": the 38 *.sig files are the source game's drawn meshes and materials (RCSB/RCSM), drawn by signature_bodies.cpp; the 38 *.json are its lot-lab documents. The game's landmark buildings are the 4.0's output, not this game's.
- READ AT THE WRONG SCALE (proved by the data): the lot documents are in the source's 0.4 m unit and body_doc.cpp reads every length as metres (:159-162 block x/y/w/d, :188-194 element x/y/w/d/z/h, :214-215 groundHeight/storeyHeight, with no factor anywhere in body_doc, lot_projects.gd or signature_bodies). Evidence: a lot bench is w 4, d 1.25, h 1.125 (artDeco.json elements 3, 6, 9, 12; bank.json 3...) = exactly 2.5 x this game's bench [1.6, 0.5, 0.45] (models.json:10) on all three axes; bollards h 2.25 (= 0.9 m x 2.5), lamps h 11.25-12.5, walls h 5.5, storeys 8.75 and ground floors 12.5 (= 3.5 m and 5 m x 2.5), module 7.5 (= 3 m). The editable bodies of the signature lots are 2.5 times too big against the city (and fit under tuning build.max_storey_m 9 and max_ground_m 20, so nothing refuses them). Fix: the converter (cook) writes metres, or body_doc applies the document's unit once; then the lots are this game's own designs, not the 4.0's.
- GENERATOR NOISE: 10.750000000000002, 4.440892098500626e-15, 3.0000000000000004 in the documents.
- PORTUGUESE: body_doc.cpp:164-213 error labels "telhado", "linha", "coroa", "fachada", "peça", "uso", "função"; lot_projects.gd:29 "lote %s recusado".
- STRAY OUTPUT INSIDE THE GODOT PROJECT: project/dist holds 145 capture files (rq/bomb45, bombg, cap, pistolg) with no .gdignore: ignored by git (.gitignore:8) but scanned and imported by Godot as res://dist.

#### assets chunk 2 (beauxArts, beigeClassic) + whole-folder census (script over all 38 lot docs)
- beauxArts: groundHeight 14.0, storeyHeight 9.25, no elements, no lotPlan. beigeClassic 11.25/7.75. Same 0.4 m unit.
- Census of all 38 lots/*.json: module 7.5 in every file (= 3 m in U), groundHeight 9.5..14.0, storeyHeight 7.75..9.25
  (3.1..3.7 m in U, 7.75..9.25 m read as metres = absurd storeys); 33 bench rows are 4 x 1.25 x 1.125 (= 1.6 x 0.5 x 0.45 m
  in U, matches models.json bench exactly), pinkCorner 3.5 x 1.125. Units confirmed for every file.
- 656 float-noise literals (0.30000000000000004 style) across the folder: documents dumped from JS numbers, never cleaned.
- 5 files without elements/lotPlan (beauxArts, decoTower, glassSpire, modernOffice, residentialTerrace): body keys differ
  between documents (elements 33/38, lotPlan 33/38); volume keys vary (materials/facadeGeometry 95/227, facadePattern 2,
  outline 2, open 132). Loose, unschema'd document format: no schema under tools/gates/schemas for lots (check).
- Keys camelCase from the 4.0 (groundHeight, storeyHeight, nextElementId, nextVolumeId, facadeGeometry, lotPlan):
  the 4.0's document format carried verbatim; nextElementId/nextVolumeId are editor counters of the source's lot editor.

#### assets chunks 3-4 (brickFrame, cemetery, church, cityHallBrick, cityHallWhite)
- Scale chain re-verified end to end: body_doc.cpp:159-162/189-194/214-215 copy x,y,w,d,h,groundHeight,storeyHeight straight;
  model_body.cpp:64-100 fit_plan only translates (no scale); model_place.cpp:98-147 place_body never scales;
  city_core_models.cpp:62 is the only caller. => every lot body is 2.5x its intended size (CERTAIN).
  Visible numbers once read as metres: trees h 12.5, lamps 8.75/11.25/12.5, bollards 2.25, perimeter walls 5.5 high,
  gates 4.5, fences 4 high on 1.125 walls, benches 4 m, parking rows 12.5 deep, ground storey 11.25.
- Every civic/commercial doc (cemetery, church, cityHallBrick, cityHallWhite, and the rest in chunk 1) is the SAME
  template "lotPlan":10: two-storey box with shopfront fill + doubleDoor ground, one window storey, planter+tree+bench
  every 15 U along the front, bollards every 5 U, side pavement strip with drains every 12.5, bin+slab pairs, two parking
  rows at the back, perimeter walls 5.5. A cemetery and a church as a shopfront office block: these bodies are
  placeholders from the source's lot generator (lotPlan, blueprint "signature:<name>"); the visible building is the
  cooked .sig mesh, the doc body only gives footprint and lot dressing. The doc content is wrong for its function.
- Duplicate overlapping elements in every doc: two "pavement" rows at the same x/y/w/d, h 0.25 brick + h 0.275 stone
  (z-fight pair; the second always hides the first); two "slab" rows at the same x/y (stacked crate) are deliberate.
- Two volumes with identical footprint stacked by base (volume 1 base 0 two storeys, volume 2 base 2 one storey) where
  one volume of three storeys with a per-storey fill would say the same: source editor artefact.
- facadeGeometry repeats the identical dict for sides "0".."3" in every volume (no default): 4x duplication per volume.
- Colours as decimal ints (10769983) rather than hex strings: unreadable, and data/** elsewhere uses hex/arrays.
- body_doc.cpp:68 error text Portuguese ("documento: ... desconhecido"), :164 "telhado", :175 "linha", :176 "coroa",
  :188 "peça", :210 "uso", :235 "função", :72 "acabamento", :101 "componente", :183 "fachada" (already noted; full list).
- body_doc.cpp:94-95 std::stoi on document text throws std::invalid_argument/out_of_range on a malformed key like "a:1"
  -> exception through a GDExtension call (godot-cpp is built with exceptions? check) = crash on a bad document.
- city_core_models.cpp:64-67 doc_body clamps storey_h/ground_h to build.max_storey_m 9 / max_ground_m 20, comment
  "read as the 4.0 reads it (serialize.ts height)": parity with the source code, and it caps only heights while x/y/w/d
  stay 2.5x. tuning.json:169-171 build limits mix units: min_storey_m 2.6 (metres) vs max_storey_m 9 and max_ground_m 20
  (sized to let the U-unit documents, storeyHeight 7.75..9.25 / groundHeight 9.5..14, through). body_edit.cpp:44-45
  and body_mass.cpp:306-307 accept a 9 m storey and a 20 m ground storey from the player too: the limit was widened to the
  wrong unit (loosened check). Fix: convert documents at the cook (x 0.4 into metres, in cook_signatures/lot export),
  drop the clamp, set max_storey_m/max_ground_m to real storey limits.
- body_doc.cpp:94-95 std::stoi with -fno-exceptions (native/CMakeLists.txt:11,99): a malformed bay key aborts the process
  (libc++ __throw_invalid_argument -> abort). doc_body (city_core_models.cpp:56) is reached by builder documents ->
  a bad document file kills the game instead of returning errors. Fix: std::from_chars with error -> errors_.

#### assets chunks 5-14 (all remaining lots) + people/LICENSES.json + import settings + glTF headers (assets DONE)
- Every lot doc with elements is the same lotPlan 10 template (33 docs). The 5 docs without elements (beauxArts,
  decoTower, glassSpire, modernOffice, residentialTerrace) are hand-written (keys reordered, floats "15.0"): another author,
  no lot dressing, and their bodies contradict their names (glassSpire = plaster box with "window" fill, no glass finish;
  decoTower/glassSpire/modernOffice share one body shape). Content does not match the function/name in civic docs either
  (cemetery/church/school/police/fireStation/hospital = shopfront office block).
- loftBrick.json top-level "use":"commercial" vs body.use "mixed": the two copies of the same field disagree.
- pinkCorner.json volume 4 open "grass": body_doc.cpp:54 DOC_SURFACES has no "grass" -> dropped by design (header says
  "none for plain grass"), but any other unknown "open" value is ALSO dropped silently (no error), unlike every other
  name table in Reader::named. Fix: list "grass" explicitly as no-surface and error on the rest.
- people/LICENSES.json:2 "made by tools/assets/cook_people.py from Roadcraft public/models/citizens/people": the people
  are cooked from the 4.0's shipped model folder, not from the upstream MPFB2/Rocketbox sources it names -> a clone cannot
  rebuild people.bin/atlas.bin without the 4.0 checkout; provenance runs through the reference game (CLAUDE.md "look only").
  atlas.bin 22 MB + people.bin 24 MB in git.
- elements/*.gltf (22 files, Blender I/O 4.0.44): Quaternius-style nature kit names (Grass_Common_Short, Pebble_Round_1,
  Mushroom_Laetiporus); no licence/provenance file in elements/, surfaces/, noise/ (only people/ has one).
- Import settings (all 54 .import grouped):
  * 7 textures mode 0 Lossless = uncompressed RGBA8 in VRAM ("vram_texture": false): surfaces/asphalt.map, footway.map,
    kerb.map, verge.map, grass.detail, grass.detail-normal, elements/Grass.png. They are loaded at runtime from data paths
    (materials.json:8-29, palette.json:92/108) so detect_3d/compress_to=1 never fires (editor-only detection). Road maps are
    sampled on every road pixel (road_body.gdshaderinc:149,158,173): 4x the memory/bandwidth of BC7, inconsistent with their
    own .rough/.normal siblings (mode 2). Fix: compress/mode=2 (VRAM compressed, high_quality for albedo).
  * Normal maps with compress/normal_map=0 (Detect) loaded at runtime: asphalt.detail-normal, concrete.detail-normal
    (BPTC high_quality=false -> DXT1/5 colour compression of a normal map), grass.detail-normal (lossless). Detect never
    runs for runtime-bound textures; the shaders declare hint_normal (road_detail.gdshaderinc:9, terrain_layers.gdshaderinc:87).
    Fix: compress/normal_map=1 on every normal map.
  * building-finishes.normal.png and terrain-land.normal.png are Texture2DArray (no normal_map option) with
    high_quality=false -> S3TC; building_common.gdshaderinc:26 decodes .rgb*2-1 from DXT blocks (blocky normals).
  * Leaves.png size_limit=1024 only on one texture (2.7 MB source) - ad hoc.
  * glTF imports: generate_lods/create_shadow_meshes/light_baking=1 (static lightmaps) on meshes that the C++ reads into
    instanced element models (element_models.h): lightmap/shadow-mesh settings unused; harmless but wrong intent.
- road_body.gdshaderinc:158 `uv / asphalt_m * 2.7` magic number in a shader (grain scale) - no name, not in data.

#### config: project.godot, export_presets.cfg, override.cfg, bin/citycore.gdextension, .gitignore
- project.godot:24 gui/timers/tooltip_delay_sec=3600.0: tooltips "disabled" by a one-hour delay (remendo). Either the UI
  sets tooltip_text it never wants shown, or tooltips are wanted; fix at the owner (no tooltip_text / real delay).
- project.godot:32-101 shader_globals carry tuning defaults (wind 0.86/-0.5/3.0, rooms_awake 0.63, room_daylight 0.2,
  outside_on 0.79, building_* 1.0): numbers outside project/data. They are overwritten at runtime (daylight.gd:133-135,
  sky_view.gd:161-163), so the defaults are dead duplicates that only show before the first frame / in the editor.
  Fix: neutral defaults (0) or set all globals from data at boot in one owner.
- project.godot:28-29 rendering_method.mobile forced to forward_plus (fine for desktop only).
- bin/citycore.gdextension:3 compatibility_minimum = "4.5" while godot-cpp is the 4.7 API (CLAUDE.md "native/godot-cpp
  (API 4.7)"): a 4.5/4.6 editor accepts the library and then misses 4.7 entry points. Fix: "4.7".
- bin/citycore.gdextension:9 linux.debug.x86_64 -> template_release .so: Linux debug runs the release library (no libc++
  hardening, contradicting CLAUDE.md "Testes" rule on the debug DLL). Fix: point at the debug .so.
- export_presets.cfg: export_filter all_resources + exclude "tests/*, dist/*": addons/terrain_3d editor scripts, menus,
  extras and icons (all dead, see below) ship in the pck. include_filter lists data/*.json and data/*/*.json only (two
  levels): a deeper data folder would silently not ship.
- project/dist/rq/bomb45/... 145 untracked capture files (git-ignored by "dist/") INSIDE res:// with no .gdignore: the
  editor imports them (their .import files exist). Written by a run with a relative --capture-dir. Fix: delete; captures
  must resolve outside res:// (tools/tasks owns capture dirs) and add project/dist/.gdignore or refuse relative dirs.
- override.cfg is local (untracked), as CLAUDE.md describes. OK.

#### addons/terrain_3d (49 text files, 334 KB; bin 47 MB)
- Upstream Terrain3D 1.1.0 (plugin.cfg), unmodified since bb58790e "Base antes do porte" (git log: one commit).
- project.godot has no [editor_plugins] section: the Terrain3D editor plugin is NOT enabled, so every addon GDScript
  (src/*, menu/*, tools/*, utils/*, extras/*: 27 .gd, 8 .tscn, 3 .tres, 5 .gdshader) never runs; no project file refers to
  any of them (grep res://addons/terrain_3d outside the addon: none). Only terrain.gdextension + the Windows x86_64 DLLs
  are used (the terrain is driven from C++ render/terrain bridge).
- bin/ ships 14 binaries for platforms the game never targets (android arm32/arm64, ios, macos frameworks, web wasm,
  linux, windows arm64): ~38 of 47 MB in git for nothing. export_plugin.gd itself says it "essentially does nothing".
- Fix: keep terrain.gdextension (trimmed to windows/linux x86_64) + those binaries + LICENSE; drop the rest or add
  .gdignore; never export editor scripts.
- TWO USE FIELDS THAT DISAGREE: top-level "use" and "density" ARE read: lot_projects.gd:15-20 _zone picks the zone from "%s_%s" % [doc.use,
  doc.density]. So they are not dead; but they duplicate body.use and disagree in loftBrick.json (top "commercial" ->
  zone com_high; body "mixed" -> body_doc.cpp:40 USE_RESIDENTIAL): the project lands in a commercial zone with a
  residential body. Fix: one use field; the zone derived from it. lot_projects.gd:9-10 USE_ZONES is a table in code.

## 3w. `tools/` (gates, build, tasks tests, assets, analysis read in full; process, converter, ports classified): root causes

- **T1. The repository carries the source game and the machinery to port its code.** `tools/oracle-4.0` (4,698 files),
  `tools/process/features-4.0` (3,386 files of the 4.0's TypeScript), `tools/process/converter-old/out` (4,612 files),
  `tools/converter` (219 files tracked twice: by the game repository and by its own nested `.git`; 3.2 GB of output),
  `tools/ports` (194 port records), `tools/analysis/live40.mjs` (the 4.0 kept running to answer `eval`),
  `tools/process/tools/extract_feature*.cjs` (a feature's code copied out to Markdown), `tools/assets/export_builder_ui.py`
  (the Build tab's icons and texts taken from the 4.0's source), `cook_people.py`, `cook_signatures.py`, `ktx2_to_png.mjs`,
  `make_road_profiles.py`, `make_pole_parts.py`, `cook_ragdoll.py` (assets and tables taken from the 4.0's files or code).
  Fix: all of it out of the repository; assets and tables rebuilt from upstream sources or this game's own designs.
- **T2. The gates cannot hold the rules they police.** The tree as found fails them (552 problems); they skip most of
  `tools/` and the logs (`rules.json:2-20`); the mirror check runs over an empty list (`rules.json:439`); the tuning gate
  forbids documenting `tuning.json` (`gate_tuning.py:25-32`); repeated JSON keys pass (`gate_data.py:64-65`); translations
  have no gate (duplicate keys, mojibake and 905 unreached keys pass); schemas require dead keys and keep a dead table alive;
  `SEGUNDO_DONO` reports 307 name/value coincidences. Fix: per item in the list below.
- **T3. Generated data is out of step with its generators, and tools edit their own inputs.** `furniture.json` is not
  `make_furniture.py`'s output; the road catalogue holds 40 profiles, the generator writes 37 and would delete the three the
  city generator needs; `make_pole_parts.py` leaves float noise; `calm_stone.py` and `bleed_alpha.py` overwrite their source
  images. Nothing compares a generated file with its generator. Fix: generated files keyed by their inputs and checked.
- **T4. Tool infrastructure has no single owner.** C++ is read by six readers, GDScript by three, JSON by three; JSON Schema
  is checked twice; git is wrapped three times; Python imports are resolved three times; eight processes in the project's
  tools and more in `tools/process` run without a deadline (CLAUDE.md: never wait blindly). Fix: one parser package, one
  schema module, every process through `tools/tasks`.
- **T5. The line-count limits drive splits for size**: `export_builder_ui.py:61` halves the strings "so a file stays under
  the size limit", `make_road_profiles.py:142` splits the catalogue "under the gate's line limit" (CLAUDE.md forbids
  splitting to fit).
- **T6. Portuguese in tools the gates do not reach**: every `tools/process` script, `rules.json` messages and ids,
  `make_flag_icons.py:63`.
- **T7. One-shot and stale tools kept**: `tools/build/rename_english.py` (a finished migration), `tools/process` written for
  another repository (`C:/Projects/Roadcraft-windows`), Claude session UUID files, orphan schemas `roads`/`services`.

#### THE TREE AS FOUND FAILS ITS OWN GATES (run 2026-11-07, `python tools/gates/run_gates.py`, exit 1; full output scratchpad gates_full.txt)
- 552 problems: SEGUNDO_DONO 307, ESTADO_NA_UI 75, MAPA_DESATUALIZADO 59, NUMERO_MAGICO 29, TEXTO_VIA_TR 26,
  JOB_SEM_CHAVE 21, TAMANHO_ARQUIVO 19, DONO_UNICO 9, TAMANHO_FUNCAO 4, SIDECAR_WITHOUT_SOURCE 2, UI_SEM_ESTILO_SOLTO 1.
  The working tree holds other sessions' uncommitted work (git status: tools/analysis files deleted, native files
  modified), so part of this may come from it; the rules CLAUDE.md requires before a push fail as found.
- ESTADO_NA_UI 75: the @onready on plain values in project/ui (compass.gd:8-13 ...) is the dodge of an older gate;
  gate_ui.py:2 now says "@onready changes when a value is set, not what it is" and fails them. Root: the rule "UI keeps no
  game state" is applied to transient interaction state (a drag's press point, a hover flag), which is not game state;
  the scripts had nowhere else to put it. Fix: a rule that separates game numbers (forbidden) from widget interaction
  state (allowed, typed), or a per-widget interaction object; then delete the @onready dodges.
- SEGUNDO_DONO 307: name/value-matching heuristic (WHEEL vs agents.json wheel, NO_FINISH vs finish_m[0]); hundreds of hits a
  reader cannot act on one by one train everyone to ignore the gate (check gate_defs for how it matches).
- TAMANHO_ARQUIVO 19 / TAMANHO_FUNCAO 4: files 1-71 lines over the cap (building_renderer.cpp 351/350, building_renderer.h
  253/250): the size rule is policed by line count, which invites splitting to fit (forbidden by CLAUDE.md).
- SIDECAR_WITHOUT_SOURCE: project/tests/visual/capture_game.gd.uid, game_driver.gd.uid orphans.
- JOB_SEM_CHAVE 21: city_core_jobs.cpp:27-30, canyon_field.cpp:174, fog_field.cpp:252 ... jobs queued without a JobKey
  (the gate's message is Portuguese).
- Gate messages and ids in Portuguese (TEXTO_VIA_TR "Textos visíveis usam tr(...)", JOB_SEM_CHAVE text, DADOS, ESPELHO,
  TUNING_SEM_USO, ESTADO_NA_UI, DONO_UNICO, SEGUNDO_DONO, NUMERO_MAGICO, TAMANHO_*).
#### gates chunk 0 (cpp_syntax.py, gate_data.py; context/facts/gate_clones/gate_code read before)
- gate_data.py:64-65 json.load keeps the last of two equal keys: furniture.json:56/58 "step" and tokens.json:4/:111
  "bg_solid" pass. Fix: object_pairs_hook that errors on a repeated key.
- gate_data.py:56 DADOS_SEM_SCHEMA only for project/data/**: the 38 lot documents (assets/lots/*.json, read by
  DataTables.lot_projects) and people/LICENSES.json are game data with no schema (and no _doc, no units).
- gate_data.py validate: no $ref, no shared definitions -> 46 hand-written schemas mirror the data's shape (agents 15 KB,
  bay_props 19 KB, streets 23 KB); only 25 of 46 are "strict" (unknown keys accepted in 21); a schema "required" list keeps
  dead keys alive (streets.json:317) and a dead table alive (toolbar.json).
- i18n CSVs have no gate: duplicate keys across files (cmd.place.crossing), mojibake (strings_e.csv:381-382) and unreached
  keys (905) all pass.
#### gates chunk 2 (gate_python, gate_text, gate_tuning)
- gate_tuning.py:25-32: every top-level entry of tuning.json must be an object and every value a number, so a "_doc"
  string is an error: the gate itself forbids documenting tuning.json (the cause of "tuning.json has no _doc"). Fix: allow
  "_doc" keys (skip them in the number check) and require one per group.
- gate_tuning.py:58-72 check_mirrors (ESPELHO) runs over rules.json "mirrors", which is EMPTY ([]): the gate exists and
  checks nothing, while §3s lists dozens of C++/shader/data contracts kept by comment (building_bay, people_pose,
  props.gdshader, road_body ...). An empty set passes (CLAUDE.md: "conjunto vazio reprova"). Fix: register every
  contract, or better generate the shader constants from the C++ owner; fail when the set is empty.
- gate_text.py:112-132 sees only direct assignments to .text/... and direct calls; text built in a helper or a variable
  passes (health_panel.gd:133-139 raw keys; lab_views.gd:69-72) - partial coverage presented as the rule.
- gate_python.py: sound (sys.path model per program).
#### gates chunk 3 (gate_ui, gd_syntax, lex_c, lex_gd)
- gate_ui.py: see ESTADO_NA_UI above. gd_syntax/lex_c/lex_gd: hand lexers (sound for the gates' use; lex_c handles raw
  strings, JS regex, templates). Three hand lexers + cpp_syntax + gd_syntax beside tools/analysis's own parsers
  (code_parse.py, arch/adapters/cpp*.py): several parsers of the same languages in tools/ (to confirm in analysis).
#### gates chunks 4-18 (lex_other, owners.json, registry, rules.json in full, 46 schemas by census, selftest, 60 fixture cases listed)
- rules.json:2-20 exclude_dirs: tools/converter, converter-slice, oracle-4.0, process, ports, log and project/addons are
  outside every gate: the language, header, size, junk and doc rules do not reach most of tools/ (tools/process alone
  tracks 8,489 files) nor the logs CLAUDE.md requires in English.
- rules.json:355-380 magic_numbers covers native/src, project/game, project/ui only (as CLAUDE.md says): project/render
  (the shaders hold most look values, §3s) is outside it; native/src/core/ and catalog_loader.cpp are exempt; allowed
  values include 3 and 4, so typed copies like SPEED_FASTEST 3 pass.
- rules.json:382 data_copy_min_abs 4.5: copies of small values are never compared (the kerb 0.15 duplicated in
  traffic.json and streets.json, P6, is invisible to SEGUNDO_DONO).
- rules.json:393-419 data_copy_allow lists CAL_SIDE, LOT_LOOK, LOT_LOOK_M twice each.
- rules.json:439 "mirrors": [] (ESPELHO checks nothing; selftest.py passes because the mirrors fixture overrides rules).
- rules.json:273-280 TEXTO_VIA_TR "pattern" is dead text: gate_forbidden.py:173 routes the id to gate_text.untranslated.
- rules.json messages, ids, map markers ("MAPA:INICIO") and "_except_doc" in Portuguese.
- owners.json: 9 concepts only; :2 cites "CLAUDE.md > Definicoes" (no such section). Its patterns match names, not
  re-implementations: unsaved_changes `\b(unsaved|...)\b` flags city_core_query.cpp:154, city_saves.gd:186,
  health_panel.gd:135 (consumers of the revision, not owners).
- schemas: roads.schema.json and services.schema.json are orphans from the base commit (no roads.json/services.json in
  project/data, no reference); tokens.schema.json (41 B) and tuning.schema.json (104 B) accept anything / numbers only;
  daylight and sky require nothing; palette.schema requires the four dead blocks (vehicle_tints, person_tints,
  foliage_round, sky) and streets.schema requires lamp_light.pool_along/pool_sides/pool_lift_m (dead): the schemas keep
  dead data alive. Only 25 of 46 are strict.
- TWO JSON-SCHEMA CHECKERS: gate_data.py:8-46 validate and tools/tasks/schema.py ("A small JSON Schema checker") implement
  the same subset twice. Fix: one module both import.
- selftest.py: every rule has a passing and a failing fixture (sound design).
#### build/rename_english.py (read in full)
- A one-shot migration (tools/processo -> tools/process, tools/portes -> tools/ports) kept as a permanent tool and listed
  in CLAUDE.md: :18 TREES name folders that no longer exist; :23-86 a 60-line Portuguese->English name map; :307-315
  special cases for "another session's folder"; its --check (:172-266) is a third Python import resolver (gate_python.py,
  and its own). Fix: delete it (the rename is done; PYTHON_IMPORT_UNRESOLVED already checks tools/**/*.py).
#### tasks/tests (support, test_cli, test_photodiff, test_pool, test_proc; read in full)
- test_pool.py:35 asserts the sibling finishes within 3 s of wall clock: a timing bound that fails on a loaded machine.
- test_proc.py:37 and test_photodiff.py:28 create temp folders with mkdtemp and never remove them (no tearDown).
- test_photodiff.py tests tools/build/photodiff.py from inside tools/tasks/tests (a test of another tool's module).
- Otherwise sound: exit codes, empty-set failure, tree kill, cache keyed by content are all tested.
#### assets chunk 0 (bleed_alpha, calm_stone, compress_atlas.gd, cook_people)
- THE PEOPLE ARE THE 4.0'S COOKED CAST: cook_people.py:1-7 reads "<roadcraft dir>/public/models/citizens/people/<id>.bin
  and .ktx2" (the 4.0's own cooked format: JSON header, quantized mesh, morphs, levels, impostor; :92-104), the moods from
  the 4.0's source data "src/people/cast.json" (:200), clips from a separate clips dir baked by other tooling (:198), and
  follows the 4.0's code in comments ("riderPoses.ts helmetShape" :46/:125, "cookedPerson.ts CookedImpostor" :273, "three's
  front is Godot's back" :255). Bodies MakeHuman CC0 and skeleton Rocketbox MIT (both comments in render were right).
  Rebuilding needs the 4.0's checkout. Fix: cook from the upstream MPFB2/Rocketbox sources with this game's own pipeline.
- calm_stone.py:1-2,10-11 rewrites assets/surfaces/building-finishes.map.png in place, following "the 4.0's recipe
  (bake/finishRecipes.ts, cellHash * 0.2)" and its `bond`: the building finish textures are the 4.0's baked finishes,
  patched; the tool edits its own input (no source image kept, not keyed by input: runs twice change nothing only by luck
  of idempotence, which its doc claims).
- bleed_alpha.py edits elements/Leaves.png in place (source overwritten; "Run once per texture"): the same pattern, a
  derived asset with no source kept and no key.
- compress_atlas.gd: sound (DXT5 from sRGB, mipmaps).
#### assets chunk 1 (cook_ragdoll, cook_signatures, export_builder_ui, ktx2_to_png.mjs, make_blue_noise)
- THE BUILD TAB IS EXTRACTED FROM THE 4.0'S SOURCE CODE: export_builder_ui.py:2-6,29-43 writes project/ui/icons/b_<tool>.svg
  from the glyph paths of the 4.0's src/ui/builder/icons.ts and the tool list of src/ui/builder/catalog.ts; :46-68 writes
  data/i18n/strings_d.csv and strings_e.csv from the 4.0's src/ui/i18n/pt-BR.ts and en.ts, every key under seven prefixes
  "that the other parts of the catalogue do not hold yet" - the cause of the ~120 unreached builder.* keys and of the
  duplicate lot.*/builder.lot.* sets (§3t census); :61 halves the keys into two files "so a file of the catalogue stays
  under the size limit" - the cause of the strings_d/e split for size. Fix: icons and texts authored for this game, only
  keys a screen uses; delete the script.
- cook_signatures.py:1-4 "No code of the 4.0 comes along: only what it drew": the 38 *.sig landmark meshes and
  materials.sig (587 KB) are the 4.0's drawn geometry and pictures, read from a look.mjs report of the 4.0 running
  buildSignature; :36-45 GLASS_IN_PICTURE boxes typed per 4.0 facade style; :158 "three's front is Godot's back".
- ktx2_to_png.mjs:1 converts "the Roadcraft cooked KTX2 surfaces": project/assets/surfaces (asphalt, footway, kerb, verge,
  terrain-*, building-finishes) are the 4.0's cooked textures (with calm_stone.py patching one layer).
- cook_ragdoll.py:4 "the 4.0's ragdoll.ts": joints and parts copied from the source's code.
- make_blue_noise.py: sound (void-and-cluster with its citation; an original asset).
#### assets chunk 2 (make_flag_icons, make_furniture, make_lattice_noise)
- make_furniture.py:31-32 writes "cab" then a single "step"; project/data/render/furniture.json:56/58 holds "step" twice:
  the committed table is not this generator's output (hand edit or stale), and no gate or task compares a generated table
  with its generator (CLAUDE.md "o resultado fica guardado pela chave das suas entradas"). :23 "pewter": ["wood", ...].
- make_flag_icons.py:2-3 the fourteen flag designs are "the Roadcraft's flags.ts FLAG_PATTERNS"; :63 prints Portuguese
  ("%d ícones em %s").
- make_lattice_noise.py: sound.
#### assets chunk 3 (make_pole_parts, make_road_profiles)
- THE ROAD CATALOGUE IS THE 4.0'S: make_road_profiles.py:1-5 "the road catalogue of Roadcraft 4.0 (src/world/roads/
  catalog.ts and the classes of src/world/roadTypes.ts), kept in the order the saves index it by. A width here is the
  4.0's"; :124 "The cards of each tab, in the 4.0's order"; :138 "the 4.0's classes"; "antigos" = "the two roads the 4.0
  dropped, for old saves only" (old saves of the 4.0: this game has none to keep).
- DATA OUT OF STEP WITH ITS GENERATOR, AND THE GENERATOR WOULD BREAK THE GAME: the committed catalogue holds 40 profiles,
  the generator writes 37 (:82-122); cityLocal, cityUrban, cityAvenue (road_profiles_b.json, ranks 9/10/9 colliding with
  narrowStreet 9, sharedStreet 10, boulevard 9) were added by hand and are named in C++ by city_gen.cpp:24-31. Running
  the generator "when the catalogue changes" (:5) deletes them and the city generator loses its sections. Fix: one owner
  (the JSON, hand-kept, the script deleted) or the script holds every profile.
- :142-143 the catalogue split in three files "a data file stays under the gate's line limit" (split for size, forbidden
  by CLAUDE.md; the cause of road_profiles_a/b); :81 "The order is the saves' index" (saves refer to profiles by position:
  a contract by position); :9-10 paving and surface as bare numbers in the data.
- make_pole_parts.py:1-3,14,19-31 the pole is the 4.0's (src/world/utilities.ts, src/render/utilities.ts placePole, its
  POLE_* constants; HEIGHTS "roadcraft" 8.5 = "the 4.0's POLE_HEIGHT"); :109-146 rewrites streets.json in place by
  line-pattern surgery (needs `^  "<style>": {$` and one part per line: any reformat of streets.json breaks it); :89 width
  ARM_THICK * 0.8 is not passed through r(): the source of streets.json:368 "width": 0.08000000000000002.
#### analysis chunks 16-18 (causes.py, code_parse.py, detectors.py, live40.mjs, sampler.py, sampler_report.py)
- FOUR C++ PARSERS IN tools/: gates/lex_c.py + cpp_syntax.py (tokens), analysis/code_parse.py (line regex + brace count),
  analysis/arch/adapters/cpp*.py, and the GDScript ones (gates/lex_gd + gd_syntax, arch/adapters/gdscript.py). No single
  owner of "read C++/GDScript". code_parse.py:60-65 counts braces per line with no lexer: a '{' or '}' in a string, char
  or comment shifts every later function span (detectors then misattribute bodies). Fix: one parser package (the gates'
  token lexers) used by gates and analysis.
- live40.mjs: keeps the 4.0 running in a headless Chrome and serves POST /eval of arbitrary JS on it "a question to the
  reference costs milliseconds" - a live oracle of the source game, against the look-and-features-only rule (and /shot
  writes to any path given). Fix: delete.
- causes.py/detectors.py: regex heuristics over text (crude but declared as such); causes.py:51 costly_natives treats any
  function with "for (" as costly.
- sampler.py:125 "the first thread seen: the program's main loop" assumes Toolhelp32 lists the main thread first (not
  documented by Windows); :119/:133 process and thread handles opened and never closed, no SymCleanup; ctypes restype/
  argtypes left at int for OpenProcess/OpenThread (only GetCurrentThread fixed, :36): latent 64-bit handle truncation.
- CLAUDE.md lists sampler.py and pgo.sh among the commands while its "Execução e verificação" says "Sem medições, perfis
  nem testes longos e pesados para investigar": the document contradicts itself on profiling.
#### PROCESSES WITHOUT A DEADLINE (grep of subprocess calls in analysis, assets, build, gates, tasks)
- No timeout: tools/gates/scope.py:19 (git: every gate run), tools/assets/cook_people.py:264 (node per body),
  tools/analysis/arch/repo.py:17, :88 (git), :63, :109 (git cat-file --batch kept open), tools/analysis/arch/adapters/
  cpp_build.py:31 (ninja -t compdb), :62 (compiler -E -v), :158 (compiler -E -dM). CLAUDE.md: "Nunca espere às cegas um
  processo lançado (tempo-limite e acompanhamento)"; tools/tasks/inputs.py:20-22 is the one that does it right
  (timeout, NO_WINDOW, --no-optional-locks). THREE GIT WRAPPERS: gates/scope.py _git, tasks/inputs.py _git, arch/repo.py;
  CLAUDE.md calls tools/tasks "the one orchestration owner of every support tool". Fix: every tool asks git and spawns
  through tools/tasks (inputs._git, proc.run).
#### analysis/arch chunk 0 (arch.py, adapters/cpp.py, cpp_build.py)
- arch/adapters/cpp.py parses with libclang (clang.cindex), a fifth C++ reader beside the four of tools/ (gates lexers,
  code_parse, its own cpp_* adapters).
#### analysis/arch chunks 1-15 (adapters cpp_calls/cpp_cond/cpp_expr/cpp_flow/cpp_plan/cpp_summary/data/gd_body/gd_calls/gdscript/godot_text,
#### cache, catalogue, check/*, cli, contracts, core, costmodel, entries, external, graph, link, model, pipeline, predict, repo, roles, scan,
#### shape, slices, summaries, work_queue, workers, arch_eval/*) - read in full
- Design: libclang + tree-sitter facts, call graph, slices per entry, seven defect classes each proven by a gold item and a
  mutation (check/registry.py verify). Sound as a design; the defects found are the deadline-less processes (above), the
  duplicated readers (C++ x5, GDScript x3 with tree-sitter, JSON x3: gate_data json.load, tasks/schema.py, arch/adapters/
  data.py _Reader) and repo.py:105-117 blob() returning b"" when git cat-file dies (a read failure becomes empty text).
- arch/repo.py:22 reads the commit, never the working tree, so it does not see uncommitted work (stated).
- arch_eval/gold.json (commit e806fc01 = HEAD~1) lists ten defects of native/src (UploadBudget admitting at window 0,
  lost updates in tree/cover renderers, thread_local body caches, XOR body key, linear free-list removal, unbounded count
  from a save, unbudgeted overlay jobs, road refresh on body edits). In the working tree as found these are already being
  reworked (uncommitted: 231 native files changed, +5,383/-2,437; e.g. frame_admission.h:37-47 `whole`, record.cpp body_key,
  building_store.cpp:56-75 free_ids.claim, save_read.cpp:130 get_count(TRACK_MIN_BYTES)); my native findings were read from
  the working tree earlier in this session, so line numbers may drift while that rework lands.
#### tools/process (8,489 tracked files; read: settings.json, bin/gate*, READ-ME.txt, .gitignore, extract_feature.cjs, extract_feature_cpp.cjs,
#### process.py head; census of the other 16 code files; classification of records and copies)
- THE SOURCE GAME'S CODE TRACKED TWICE MORE: tools/process/features-4.0 (traced 3,146 + whole-files 146 + untraced 94 =
  3,386 files) and tools/process/converter-old/out (4,612 files): 7,906 .ts files of the 4.0 (and the old converter's
  output) in the repository beside tools/oracle-4.0 (4,698 files). records/map40/graph.json is 13.7 MB, features.json 461 KB,
  records/map40/specifications/* specs of the 4.0's features, records/extractor-package 560 KB of 4.0 code excerpts.
  Fix: out of the repository (the user's rule: the 4.0 gives look and features only).
- THE PORTING TOOLS COPY SOURCE CODE: extract_feature.cjs:1-16 "Extrai o código completo de uma funcionalidade de um
  projeto TypeScript ... e junta num só Markdown" (the 4.0's code per feature, for porting); extract_feature_cpp.cjs does the
  same over native/src through clangd (a sixth C++ reader); map40/graph.cjs:16 defaults to the absolute path
  'C:/Users/Jonathan/Documents/Labs/roadcraft-4.0-main' (the 4.0 on the user's disk).
- WRITTEN FOR ANOTHER REPOSITORY: READ-ME.txt (Portuguese) describes "jogo/Roadcraft.exe" and a "fonte/" folder; .gitignore
  ignores fonte/, converter/, ports/, jogo/, tools/estado/ (the old C:/Projects/Roadcraft-windows layout); tools/verified/
  *.ok record outputs under C:/Projects/Roadcraft-windows; converter_sessions.txt / coordinator_sessions.txt are Claude
  session UUIDs (one a placeholder 11111111-2222-...): personal process state in git.
- tools/process/tools/process/process.py:1-9 is a Claude Code hook ("Portão de processo") wired in ~/.claude/settings.json,
  i.e. global tooling of the AI sessions living inside the game's repository; ~/bin holds "portao"/"portao-ver" while the
  repo's wrappers are bin/gate and bin/gate-check (renamed in the repo, not on disk).
- All 18 code files are Portuguese (identifiers, comments, messages: census of words per file), and tools/process is
  excluded from every gate (rules.json exclude_dirs). Processes without a deadline: test.py (5 calls), extract_feature_cpp.cjs
  (clangd spawn), graph.cjs, conversations.py, reading_lib.py, session_watch.py (1 each).
- git status: tools/process/tools/hooks/gates_dispatch.py tracked but deleted in the working tree; process tools modified
  (another session's uncommitted work).
#### tools/converter, tools/converter-slice, tools/ports, tools/oracle-4.0 (structure, configs, output in the game; translator not line-read)
- tools/converter has its own .git AND its 219 files are tracked by the game repository as plain files: two histories of
  the same files (the converter's log: "Every translated module checked against the 4.0 with no game", ...); the main
  repo's copy and the nested repo diverge unless both are committed by hand. tools/converter/out holds 3.2 GB.
- configs/roadcraft-machine.json:3 "source": "C:/Users/Jonathan/Documents/Projects/city/roadcraft" (the 4.0's source on
  disk) and the TypeScript compiler from the 4.0's node_modules: the converter needs the source game to run at all.
- ITS OUTPUT IS COMPILED INTO THE GAME (recorded in §2/§3a): native/src/core/js_*.h (a JavaScript runtime in C++, 1,073
  lines) and sim/roads/flow_stats*, control_advisor* ("Translated from Roadcraft 4.0 src/sim/roads/flowStats.ts by the
  converter"), used by sim/traffic/flow_control.h and probed by api/city_core_flow_probe.cpp; tested against the 4.0's
  output (flow_control_test ORACLE from tools/converter/scripts/call40.mjs).
- tools/converter-slice: a git worktree of the converter ("sync from master"), git-ignored by the game repository.
- tools/ports: 194 tracked files (137 .md, 54 .json): per-port plans, reports and coverage of 15 ports (roads 65,
  landscape 25, traffic 13, ...) - records of porting the 4.0, cited by game data and tests ("ports/landscape/research.md",
  "ports/demolition/relatorio.md", "ports/transit/evidence/look").
- tools/oracle-4.0: 4,698 tracked files, 4.1 GB on disk ("a cópia instrumentada do 4.0"), not listed in CLAUDE.md's
  structure; settings.json:2 cites its ARQUITETURA.md.
- Decision recorded for §1: the translator (tools/converter/src 1.2 MB, scripts, test: 82 chunks), converter-slice (71
  chunks), the port records (182 chunks) and the 4.0 copies were classified and their ties to the game traced, not read line
  by line: their purpose is porting the source game's code, which the user's rule forbids, so the fix is their removal
  whatever their internal defects; the game-side effects of each are recorded.

## 4. Documents out of date or contradicting each other

- `log/mapa.md` has no line for `tools/tasks/**` (grep: 0) and still names six deleted tools: `tools/build/runq.sh`,
  `runq_jobs.sh`, `run_tests.sh`, `hidden_run.ps1`, `gates_changed.sh`, `tools/analysis/suite.py` (`log/mapa.md:466`,
  `:476`, `:479`). `MAPA_DESATUALIZADO` must fail on the current worktree.
- `log/work-log.md:561` ("O jogo roda só por `runq`/`hidden_run.ps1`") names deleted tools.
- `tools/process/CLAUDE.md` describes another layout (`fonte/`, `C:\Projects\Roadcraft-windows`) and the invalid copy
  `roadcraft-4.0-main`; the user already called it old (`log/arquitetura.md:303`, R9).
- The memory `no-reference-game` says the source game is "not a reference at all"; the user's order of 2026-11-07 in
  this session is narrower: its look and features are the target, its code never. The memory must say that.
- The worktree holds a large uncommitted state (git status 2026-11-07: 953 modified, 3,007 deleted, 269 untracked): the
  92-cause audit fixes in `native/src` (231 files), the new `tools/tasks`, the removal of `tools/oracle-4.0` files.

## 5. Errors of execution and order made in this review

- 2026-11-07: two `cd` commands into `tools/tasks` and `tools/gates` moved the session's working directory; later
  commands ran with absolute paths. No file was written there. Rule kept from here on: no `cd`, absolute paths only.
- 2026-11-07 (after the context compaction): one more `cd` into `native/src/sim` moved the working directory; the next
  command moved it back to the root. Every command now starts with `cd /c/Users/Jonathan/Desktop/city &&` and no other cd.
- 2026-11-07: §3h first said `ecology.h:4-5` named a reader that does not exist, judged from the one call site of
  `compute_ecology` without following who calls the function around it (`chunk_ground`). Reading `render/cover` showed
  the second reader. Corrected in §3h; rule kept: a "nobody else calls it" claim follows the callers up to the jobs.
- 2026-11-07: the project skills `roadcraft-port` and `roadcraft-reading` were offered by the harness; they were not
  used because both order porting from, or reading, the source game, against the user's order.
- 2026-11-07: §3s first said one of `people_skin.gdshaderinc:6` ("Rocketbox skeleton") and `people_asset.h:1-2`
  ("MakeHuman people") was wrong. `cook_people.py:1-2` shows both are right (MakeHuman bodies, Rocketbox skeleton);
  corrected in §3s. Rule kept: a provenance claim waits for the tool that made the asset.
- 2026-11-07: the lot notes first called the top-level `use`/`density` of the lot documents dead keys from a grep of
  `body_doc.cpp` alone; `lot_projects.gd:15-20` reads them for the zone. Corrected before it entered §3v.
- 2026-11-07: a `cd` into `tools/converter-slice` inside a compound command moved the working directory (read only, no
  file touched); the next command returned to the root with the absolute path.

## 6. Every finding by cause, and the plan to fix them in batches (2026-11-07)

Method: the 837 bullets of §2-§4 were tagged by rules over their text, then every group was read and 268 bullets were
moved to the cause that fixes them (the rules and the hand corrections are in the session scratchpad, `catalogue.py`,
`primary.py`, `overrides.py`). Each finding has one **primary** cause, the one whose fix removes it; §7 lists every
finding under its cause. 14 bullets are notes, not defects (marked N in §7).

### 6.1 The causes

| Id | The design decision that produces the findings | Findings | Area causes merged | The fix, one mechanism |
|---|---|---|---|---|
| C1 | The source game is the reference: its code (translated runtime, generators built for bit parity), its values (style "roadcraft", data docs), its assets (lots, people, textures, icons, strings), its tests (oracles, parity bounds), its copies and porting tools in the repository | 142 | §2, §3a, §3b, R11, P1, T1, A4 (style) | Style in `CityState`, one overlay owner, "roadcraft" blocks deleted; translated code and `js_*.h` deleted; each value sourced or decided here; assets rebuilt from upstream or designed; tests on this game's bounds; copies and porting tools out of the repository |
| C2 | Verification that cannot fail: gates that skip folders, check empty sets or forbid docs; tests with no harness, a running clock, frame waits, private members, pinned counts, two "drawn" waits | 78 | §3 (tools), T2, P9 | One test base (check, output, exit code); `new_map` paused and stepped by ticks; one capture wait; gates over every folder, a populated mirror table, `_doc` allowed, repeated keys refused, an i18n gate |
| C3 | Contracts kept by hand: C++ enums and constants typed again in GDScript, shaders, data and tests; rows by position or key order; one vocabulary in many tables | 96 | P3, P4, L11, A4 | One name table per enum beside it; `constants()` exports every enum by name; a generated `.gdshaderinc` for shader constants; data rows keyed by names resolved at load; the mirror gate reads the same table |
| C4 | One value, two owners (data duplicates, settings in two stores, shader rules copied between levels, derived numbers typed by hand, primitives written twice) | 94 | P6, R10, L11, §3t | One owner per value; derived values computed at load from their inputs; the other side reads the owner |
| C5 | `StoreApi` mixes commands, queries and render settings; tool state kept in `CityState` and the `Catalog` | 14 | P2, A2 (second half) | Three kinds of call filed by what they do; tool state travels with each command |
| C6 | The lock protocol: mesh jobs hold the map for their whole build, reads outside the guard, read-only previews take the exclusive lock, `step_ticks` beside the sim thread | 22 | L1, L2, A1, A2, P5, §3a | Gather under a short lock and build outside it; locked views (`ReadView`, `TrafficView`, `EditView`) as the only access; previews under the shared lock; `step_ticks` refused unless the clock is stopped |
| C7 | Derived data has no cache owner: bodies, street furniture, junction shapes, chunk ground, lots, catalog constants are recomputed by every reader | 25 | R1, R3, R4, R8, R9, L6, L7, L8, A7 | One cache per derived thing (`BodyCache`, per-segment street layout, per-node junction shape, per-chunk placement, lots per block, catalog fields), invalidated through the `ChangeLog` (one dirty channel) |
| C8 | `BuildingStore` files buildings as points; callers pad by guessed reaches | 8 | R2, L10, A7 | File the rotated footprint's box, return overlaps, test with `covers`; delete every reach constant |
| C9 | Journal and damage identity: closures in the journal, damage keyed by place, edits outside the journal, the state hash written in hash-map order | 5 | R5, R6, R7 | Data payloads for every op; damage by building id and furniture slot; fires, holds and shots journalled; one ordered writer in `core/bytes` |
| C10 | Errors abort and loaders are lax (`-fno-exceptions` with `stoi`/`.at()`, unchecked positional arrays, silent defaults) | 9 | A3 | One strict reader (presence, type, size, range) for every loader; errors returned, never thrown or aborted |
| C11 | Three colour rules in the shaders and five sRGB transfers in C++ | 6 | P7, L11(b) | `core/color` owns both directions; one shader include; overlays through the tone inverse |
| C12 | Pipeline structure: two result protocols, a dead level-of-detail owner, transit built on the main thread, god classes, an engine internal for the flush, sim data loaded in the render setup, saves decoded on the main thread | 14 | L3, L4, L5, L9, L12, A5, A6 | Buildings on `ChunkResults`; levels by `DetailView` or `detail.*` deleted; transit as jobs; one owner per responsibility; `force_sync`; catalog loads all sim data; decode in a job |
| C13 | Dead code, data and text; stale or misplaced comments and documents | 88 | P8, §3h, §3l, §4 | Delete; the orphan and i18n gates keep it from coming back |
| C14 | Tunables and look values in code, shaders and translations | 39 | §3b, §3c, render look numbers | Values to `project/data` with their reason; translations take `{values}` from data |
| C15 | Portuguese text in code, data ids, file names, tests and tools | 63 | P10, T6 | Mechanical rename; old save names read once and migrated |
| C16 | Tools: generated data out of step with generators, duplicated readers and wrappers, processes without deadline, files split for size, one-shot tools | 32 | T3, T4, T5, T7 | Generated files keyed by their inputs and checked, or hand-kept with the generator deleted; one parser package; every process through `tools/tasks`; size splits merged; the line rule stops driving splits |
| C17 | Assets and packaging settings (texture compression, normal-map flags, foreign binaries, export filter, project settings) | 14 | §3v | Import settings per texture role; trimmed addon; export filter by include list |
| C18 | Lot documents in the source's 0.4 m unit read as metres, limits widened to let them through | 6 | P11 | Convert once at the cook (or drop the lots, C1); limits back to real storeys |
| C19 | Local wrong results: each its own cause | 38 | §3f, §3j, §3n | One fix and one test each |
| C20 | Local algorithm and allocation choices (quadratic loops, per-frame allocations, scans without an index, whole-buffer copies) | 30 | §3g, §3k | Per finding: the right structure (index, closed form, retained buffer) |

### 6.2 How to fix them all in one pass and know what broke

1. **Baseline first.** Tag the commit (`audit-baseline`), then one full run: `run_gates.py`, `cli.py run tests`, `quick.sh --all`,
   the captures and the state hashes of the seeds used by the tests. Keep the photos and hashes as the reference. What
   already fails is recorded: those failures are findings (for example `transit_test` once its `await` is added).
2. **One commit per batch, the cause in its subject** (`fix(C7): ...`). Every batch below declares, before it is written,
   which outputs it may change and which must stay byte-identical (§6.4).
3. **One full verification at the end** (the same run as the baseline), then compare against §6.4:
   - an output that a batch declared as changing moves: expected; re-bless it with that batch's reason (CLAUDE.md allows
     `--bless` only for an understood visual change);
   - an output that every batch declared identical moves, or a test fails: `git bisect run` over the batch commits with
     that one test as the probe (`python tools/tasks/cli.py run fast <scene>`). It names the batch in log2(n) runs of one
     test; the batch's declared mechanism says why.
4. Batch B0 must land first and alone. Without deterministic tests the bisect gives noise and "test at the end" proves
   nothing.

### 6.3 The batches, in order (each depends on the ones above it)

| Batch | Causes | What changes | Files (main sets) | Expected break if wrong |
|---|---|---|---|---|
| B0 | C2, C9 (R7 only), C6 (`step_ticks` guard) | Test base class; `new_map(..., paused)` everywhere and tick stepping; one capture wait; `transit_test` await; gates: an empty mirror table fails, `_doc` in tuning, repeated keys refused, i18n gate, `log/` in scope; the UI rule on copies of the store's numbers, `@onready` dodges removed (as done: §6.6) | `project/tests/**`, `tools/gates/**`, `core/bytes.h`, `save_nature.cpp`, `elements.cpp`, `simulation.*`, `city_core.cpp:250` | New gate reds are the known list of findings (mirrors, render magic numbers): recorded, fixed by C3/C14. A test that now fails was hiding a defect (it names it). |
| B1 | C1 (copies and porting tools), C13, C16 (one-shot tools), C17 (unused addon parts) | Deletions only: `tools/oracle-4.0`, `features-4.0`, `converter-old`, `converter`, `converter-slice`, `ports`, porting scripts, `live40.mjs`, `export_builder_ui.py`, `rename_english.py`, orphan schemas, dead tables and keys with their schema entries, unreached translation keys, probes and shims, `project/dist`, Terrain3D foreign binaries and editor scripts | `tools/**`, `project/data/**` (dead keys), `project/data/i18n/*`, `project/tests/perf/*` | Nothing at run time: hashes and photos identical. A load error names a key still read by someone (the census missed a reader). |
| B2 | C3, C4 (equal pairs), C6, C7, C10, C12, C14, C16 (parsers, deadlines) | Restructuring with identical outputs: caches, locks, strict reader, contract tables and generated shader include, data keyed by names, values moved to data unchanged, one owner per equal pair, pipeline owners | `native/src/**`, `project/game/**`, `project/ui/**`, `project/render/**`, `project/data/**` | Any change of a state hash, a photo or a test number: this batch broke it. Hash or count differs: a cache invalidation (C7) or a data row resolved to another enum (C3). Hang or timeout: a lock order (C6). Boot refusal: a key the strict reader now checks (C10, the message names it). |
| B3 | C4 (unequal pairs), C5, C8, C9, C11, C15, C17 (import), C18, C19, C20 (float-changing) | Intended behaviour changes, one commit each, each with its predicted diff (§6.4) | as each cause lists | Only the outputs in its prediction may move; anything else is bisected inside B3. |
| B4 | C1 (the rest) | Translated flow control and `js_*.h` replaced by a rule designed here; the style in the state with the "roadcraft" blocks deleted; generators off the source's stream (`seeded.h`, `Map` order, thrown draws); data values re-sourced table by table; 4.0 assets replaced or removed; tests on this game's bounds | `native/src/core/js_*`, `sim/roads/flow_stats*`, `control_advisor*`, `sim/traffic/flow_control.h`, `core/seeded.h`, `sim/citygen/**`, `sim/buildings/b40/**`, `project/data/**`, `project/assets/**`, `project/tests/**` | Every generated city changes: a new baseline after B4, taken on purpose. Money and look tests change by the decided style. |

### 6.4 What each batch may change (the prediction the final run is checked against)

- B0: test output format and exit codes; gate reports (new rules); state hash values of cities with painted ground or elements (R7 writes them in key and value order: the layout of the bytes, not their meaning). Game outputs identical (digest, test numbers).
- B1: nothing at run time. The product zip loses `fonte/tools/oracle-4.0` (`build.sh:67`).
- B2: nothing observable. State hashes, `open_probe --digest`, every photo (photodiff zero) and every test number identical.
- B3, by commit:
  - C8: `camera_feel_test` eye-in-building count; captures with the camera near buildings; metro entrance places; the
    inspector's picked building.
  - C9: save layout (version bump; old saves read by the migration); hash values once (format), then stable; `edit_fuzz_test`
    strict passes.
  - C5: no photo change; snapshot rebuild count; tests that mutated through `query` now dispatch.
  - C11: Compatibility photos (darks), overlay colours on both renderers, tinted leaves; `light_color_test` on Compatibility.
  - C4 unequal pairs: train car length (18 m everywhere: transit captures), cycle-lane colour in cards, zone swatches,
    currency text, smoke drift with the wind (blast captures), near/mid/far facade shades.
  - C18: lot bodies 2.5 times smaller, build limits refuse 9 m storeys (`building_tool_test`, model shelf, `capture_pair`).
  - C17: low-level noise on road and ground photos (compression).
  - C15: capture file names (a rename map old to new; content identical), save extension (migration), error texts.
  - C19 and C20 (float-changing ones): only the outputs their own test names.
- B4: everything generated and the style-driven looks and prices: a new baseline, re-blessed with the decision recorded.

### 6.5 Decisions only the user can take (they block B4 and part of B3)

- **D1. The 4.0's assets** (38 landmark lots, the people, the road and terrain textures, the Build icons and strings): remove
  now (those features are missing until new assets exist) or keep them until replacements are made.
- **D2. The default regional style** once "roadcraft" goes: the sourced Brazilian blocks, or a new generic style (the work log
  records a user rule against a Brazil-only standard).
- **D3. Automatic junction control** once the translated flow control goes: a rule designed now (from published signal
  warrants), or a fixed control until it is designed.
- **D4. Old saves:** migrate them (keep a reader of the old names and format) or drop compatibility.

Decided by the user, 2026-11-07:
- D1: keep the 4.0's assets until replacements exist; the lot unit scale (C18) is fixed meanwhile in B3 (documents
  converted to metres once, limits back to real storeys).
- D2: a new generic style of this game: values from international norms or design decisions stated as such; the
  "roadcraft" blocks are deleted and "brasil" stays a regional option, not the default.
- D3: a new automatic junction rule designed now, from published signal warrants, in B4 with the translated code's removal.
- D4: no compatibility with old saves: the save names, extensions and format change without a migration reader (C9, C15).

### 6.6 B0 as done (2026-11-07)

**Baseline** (`fix-baseline` = `a1a771c4`, the sources of `33c8cf79` with their DLLs; outputs in `dist/baseline/fix-baseline`):
`run_gates.py` 493 problems; `quick.sh --all` 41/43: `camera_input_test` (a jolt of 1.04 m in a frame walking at 25 m and at
90 m, another run 1.72 m: it measured real frame times) and `photos_check` (golden of 2026-11-06, older than `752e4e02`); the
digest `4040792916cf7d78`. Two runs of the frozen photos on the same commit differed in 23% / 21% / 10% / 0.04% of their pixels:
the frozen photos could not hold any golden.

**What B0 changed, by cause:**
- C6: `Simulation::hold_still` (simulation.h/.cpp): true once no tick runs or can run (stopped, or paused with the thread parked:
  a tick in flight when the pause came is waited for); `Simulation::step`, `CityCore::step_ticks` (now bool) and the two probes
  that step the city on the caller's thread (`traffic_hashes`, `flow_push`) refuse otherwise.
- C9 (R7): `ByteWriter::put_by_key` and `put_set` (core/bytes.h), the one owner of order for what the state keeps unordered;
  the painted tiles, the elements (`ElementItem::before`) and the bodies' maps are written through them.
- C17 (found here): the DLLs were rebuilt with a new PE timestamp by every `--all` run (2 bytes at offset 128): the link takes
  `--no-insert-timestamp` (native/CMakeLists.txt), the same sources give the same bytes.
- C2, tests: `TestCase` (project/tests/test_case.gd) for every core and e2e test: one check, one ending, a test with no check
  fails, maps opened paused and moved by `ticks()`, one `save()` that waits on the state, the tests' files in `user://tests`.
  Case files take the test (`t.check`, `t.ticks`). Fixed on the way: `transit_test` (its save checks never ran, and its
  `await CityStore.loaded` after the synchronous `load_game` would have hung); `camera_walk_cases` steps the rig at a fixed step
  (the jolt was the frame time); `flow_control_test`, `lamp_light_test`, `quality_test` run traffic by ticks; `boot_refusal_test`
  through `CityStore.boot(..., catalog)`; `model_test` counts from models.json and the lot files; `city_mass_test` one check;
  `lab_test` waits on `LabRecorder.measuring()` and holds no GPU time; `yard_test` holds every dock to a way (it passes); the
  lot frame read from the core (`body_at`, `BuildingProbe`) instead of three GDScript copies; `_back_to`/`_info` owned once;
  private members replaced by public surfaces (`CityCamera.settle/apply/springs`, `StartScreen.pick`, `HudActions.open_new`,
  `InteriorController.open`); `road_commands_test` reads `terrain.road_sink_m`; scene roots named after their tests.
- C2, captures: `UiDriver.drawn` delegates to `CaptureWait` (one wait, counting incomplete photos); `drag` takes the shot as a
  callback; look's wall-clock `wait` step deleted; capture maps opened paused; the parity key owned by `CaptureParity.ONLY`;
  `Settings.apply` once; the interior capture finds buildings by kind. Frozen photos: the cause of the 23% was timing-dependent
  rendering state, FSR2's history at the `high` tier and pipelines specialized in the background (Godot docs, "Reducing stutter
  from shader compilations"; the compile counter rises at submission, `pipeline_hash_map_rd.h`, so it cannot be waited on). The
  frozen look keeps no temporal filter (`Settings.kept.temporal`) and photographs only when two looks 16 frames apart are the
  same bytes (`CaptureWait.until_steady`). Two runs now differ in 3 to 7 pixels, all at the joints of utility wires (probable
  cause: ties in depth between coincident geometry broken by draw order, which follows the order jobs finish): a render
  finding for B2 (C12), before B2 relies on photodiff zero.
- C2, gates: the UI rule now forbids what the principle forbids, a member holding a copy of the store's numbers (75 -> 2 real
  ones, fixed: `StartScreen._first_seed`, `HudDialog.width_token`); 111 `@onready` on plain values removed; one strict JSON reader
  (`jsonfile.py`: a key written twice fails; `furniture.json` "step" and `tokens.json` "bg_solid" fixed); `_doc` per tuning
  group, required by the schema (16 written); an empty mirror table fails; the i18n gate (one key once, cells as the header,
  no broken encoding) found `hint.transit.trackOnRoad` split in five cells (the English player read Portuguese), fixed with
  the two repeated keys and the two garbled rows; `log/` and `project/render` (magic numbers) in scope; `data_copy_allow`
  deduplicated; tools tests: no stopwatch in `test_pool`, temp folders removed, photodiff's test beside its tool.

**Moved to the batch of their cause** (the B0 row of §6.3 listed them): the growth exemption of `edit_fuzz_test` is C9 (an
edit outside the journal; journalling the queue changes which lot grows on a tie: B3); per-scene inputs in `suites.json` and
the scope diff by paths are speed, not verdicts (C16, B2); populating mirrors (C3 generates the shader constants, B2); the
lot documents' schema (with their conversion, C18, B3); unreached translation keys (with the census, C13, B1); `cloud_shadows`
(a dead setting, C13, B1); capture names, their order prefixes and the `--only` keys in English (C15, B3); the eye-in-building
measure of `camera_feel_test` (C8, B3); the magic-number rule satisfied by naming, render reads by include path, `gate_text`
coverage, the data-copy threshold and `owners.json` (C4/C16, B2).

**Verification** (`dist/baseline/b0`): `quick.sh --all` 42/44, every test scene passes (`camera_input_test` too); the digest
equal to the baseline's; `photos_check` changed as predicted (no temporal filter) and re-blessed with that reason;
`gates_changed` shows only problems the baseline had and the predicted ESPELHO; `run_gates.py` 431 (DONO_UNICO 9, ESPELHO 1,
JOB_SEM_CHAVE 21, NUMERO_MAGICO 41 with project/render in scope, SEGUNDO_DONO 307, SIDECAR_WITHOUT_SOURCE 2, TAMANHO_ARQUIVO 19,
TAMANHO_FUNCAO 4, TEXTO_VIA_TR 26, UI_SEM_ESTILO_SOLTO 1); gate selftest 64 cases OK; tools tests 20 + 6 OK. Non-vacuity: with
`TestCase.new_map` turned to a running clock, `traffic_hash_test`, `street_crossing_test` and `transit_test` fail and say why.

### 6.7 B1 as done (2026-11-07)

**What B1 changed** (`9a056673`): the source game, the porting machinery and its records and the tools that took assets and
tables from it left the repository for `C:\Users\Jonathan\Desktop\city-archive` (moved, not deleted); Terrain3D keeps what the game
ships (its `.gdextension` header is `;`, the comment ConfigFile reads: a `#` header dropped `entry_symbol` and Terrain3D loaded
nowhere, caught and fixed by the Android session before either commit); dead tables and keys went with their schema entries;
dead render code, `(void)` parameters and three warnings went; the sign sheet keeps no list for the captures (they read the core's
laid cells and wait for the painter to be idle); probes and shims for old builds went.

**Verification method.** The shared folder held two other sessions' sources mid-edit, so a build there would have tested their
code with mine: B1 was verified in a worktree of its own commit (`C:\Users\Jonathan\Desktop\city-verify`, its own build folders,
the fetched libraries' sources by `FETCHCONTENT_SOURCE_DIR_*`). `quick.sh --all` 42/43: every test scene passes, the digest is the
baseline's; `photos_check` fails on `00-cidade` and `02-rua` by 2 to 7 pixels. Measured, not assumed: ten frozen runs over two
trees with one DLL (three at `fbfc8720`, three at `9a056673`, the golden and three others) give `01-bairro` and `03-rua_noite`
byte-identical every time, while `00-cidade` and `02-rua` land in one of four and three byte-identical states, the same states in
both trees. The frozen photos still hold run-to-run state at the utility wires' joints (C12, ties broken by the order jobs finish):
the first fix of B2, before B2 relies on photodiff zero.

**Found while verifying, fixed in the B1 close** (each a decision of the build or of a gate, fixed there):
- The map gate failed every checkout since `fbfc8720`: it read every backticked path of the map as a repository file, and a map
  line names the APK that `android.sh` writes (`dist/android/Roadcraft.apk`, outside git). `rules.json` `output_roots` declares
  where the tools write; a path there may be named as a product and is not checked as a file (`gate_docs.py`); the `map__pass`
  fixture names one and fails without the rule.
- The DLL's bytes depended on the folder that built it: godot-cpp's error macros put 24 absolute paths (`__FILE__`) in each DLL,
  the user's folder names among them in the shipped one, so a clone, a worktree or another machine built other bytes from the
  same sources. `-ffile-prefix-map` maps the repository root to nothing and the build folder to `build/` (`native/CMakeLists.txt`).
- The release DLL's profile never matched a static function: LLVM keys a static function by its file's path as compiled
  (`InstrProf.cpp`, `getPGOFuncName`), and `native/pgo/citycore.profdata` had been trained in `city-worktrees/porte-total`, so
  5 329 static functions of the core and 112 of the fetched libraries (4.8 and 0.7 of 35 billion counts) were compiled without
  their counts in every build here. `city_profile_names` strips the separators of each target's base
  (`static-func-strip-dirname-prefix`: the core's names start at `native/src/`, a fetched library's at its `<name>-src/`); the
  profile was retrained (`pgo.sh`, 19 788 functions, 40.6 billion counts) and matches a build in any folder. What stays tied to
  the build folder: the generated godot-cpp bindings' static functions (698, 720 counts: functions that never ran). LLVM notes
  that stripping can stop an indirect-call promotion across modules in ThinLTO; static functions called through a pointer from
  another module are not a pattern of this core.
- `9a056673` changed C++ sources without its DLLs (a build in the shared folder would have taken the other sessions' sources):
  the B1 close carries both DLLs, built in the worktree from HEAD with the flags above.

**Moved:** the B1 deletions in files another session holds (`maps.json` km entries, `start.json` `ui.start.sub`, the
`data_tables.gd` "session-rules" comments, `decay_grace_min` in `catalog.h`, `catalog_loader.cpp`, `growth.json` and its schema,
the unreached translation keys with the census script, `ui.stats.tip`'s `{speed}`) go with C13 in the first commit after that
session's lands.

### 6.8 B2, C10: one strict reader (2026-11-07)

**The owner.** `native/src/render/data_reader.h` (`DataReader`): every key present and of its type, every list of its length,
every index inside its list (godot-cpp's `Array::operator[]` dereferences the engine's `nullptr` past the end), a count or an
index a whole number, a colour `#rrggbb[aa]` of hex digits, a name in its table (`index_of`, `value_of`), the keys a format leaves
out read by `_or` with the value the format gives them. An error names the file and the path (`buildings.json.architectures[0]:
opening: expected 5 items, found 4`) and reading goes on, so one boot reports them all. It lives in `render/`, the lowest layer
that may use Godot, because render reads tables too; its bodies are compiled once (`data_reader.cpp`): defined in the class, they
were inlined at every read site and the release DLL grew by a megabyte for code that runs once, at boot.

**Through it now:** the catalogue (`catalog_loader`, traffic, street furniture, blast, nature, scenic), the building generator's
tables (buildings, models, interiors), the building look, transit, effects, overlays, the lab's doctor rules and the grade
probe's scene. The building look's part bands and far tones were appended at each read: a second boot (the refusal test boots
fifteen times) doubled them and failed on its own count; they are built whole and assigned now. Gone with
it: `catalog_loader`'s `Reader`, `nature_loader`'s `Read`, `scenic_loader`'s `Scene` and three copies of a name lookup.

**What could crash and now refuses:** positional reads past a list's end in every loader; `models_loader` `sizes.at(kind)` and
`% schemes.size()`; `transit_parts` `.at()` on literal part names (resolved when the table is read, an error if one is missing);
`body_doc` `std::stoi` on a bay key (it aborts under `-fno-exceptions`); a people file without a face clip (index -1); the grade
probe's node indices.

**What defaulted silently and now refuses:** an unknown open surface in a lot document (plain grass is a row with no colour); a
side key that is not a number (read as 0) and a side past the last face (skipped); a model's scheme index (it wrapped); every
burst and source row of effects.json and every key of overlays.json (`get(key, 0)`); the doctor's rules (a missing `budget_fps`
was a budget of 1000 / 0 ms); a count with a fraction (truncated); `#zzzzzz` (strtoul read what it could) and an empty colour.

**Saves.** Godot's rename deletes the target before moving on Windows (`DirAccessWindows::rename`), which is why `write_atomic`
kept an old generation, and a crash between its two renames left only `<save>.old`, which `load_city` never opened. The save now
ends in one replacing rename (`std::filesystem::rename`: `MoveFileExW` with `MOVEFILE_REPLACE_EXISTING` on Windows, `rename(2)`
elsewhere; libc++ `posix_compat.h`), so no moment holds neither the old file nor the new one.

**Dead with it:** `CatalogLoader::parse_color`, `IntRange`, `decay_grace_min` (moved from B1).

**Proof that it can refuse:** `boot_refusal_test` now boots once per damaged value, one for each kind of refusal in each table
(5 render tables, 9 catalogue tables and the missing tuning key) and checks the error names the file and the path; `CityStore.boot`
takes render tables as it takes the catalogue.

**Identity kept on purpose, recorded for B3 (C19):** the plan's doubles (`CityPlanDef`) are still read through float, as the old
`Reader::num` returned float; reading them as doubles moves every generated city.

**The render's readers, after the loaders** (`e66a7912`, `1af7f445` and the commit after them): the street look, the tree kit,
placement, atlas and pictures, the vehicles and the people, the street furniture, barriers and signs, the bay props and the
interior's furniture, the ground cover, shore stones and elements, the wear field, the film grade, the effects' bodies and
transit's own numbers. The builders that read as they build (vehicles, props, cover) take `DataValue`, the reader's value with
its path (`d["box_m"][2].f()`): with it the compiler refuses a Variant turned into a float in silence, so every read site had to
say its type. Lists built at each read (the building look's bands and tones, the trees' planted species, the barriers' rail
levels, the bay props' models, the loads' bands, the interior's palette) are the table again at a second read. Rules that
refused without a word now say which (the tree placement's grid and bands, the cabin's seat). `people_renderer.cpp` went past
its limit and split by what it does: `people_boot.cpp` reads once, the renderer fills the frame.

**A process error caught before any commit:** the conversion was done by a script (`.f()`, `.whole()`, `.flag()` from the
compiler's own complaint about each line); where a line held reads of two types, it gave every read the first one's, and
`street_props.cpp` read three floats as booleans (0 or 1) in a build that compiled. An audit of every line with more than one
`.whole()` or `.flag()` found that one and no other; the vehicles' commit had been verified without it (the props were not in
its tree).

**Left of C10, with its reason:** the lot documents (`body_doc.cpp`) keep their format's defaults until their schema (C18, B3);
positional rows turned into named fields (`opening`, `piers`, `pool`, the openings rows, `era_affinity`, `lit_share`, the
vehicle class rows) go with B4, which rewrites those same blocks for the new style: one change of format, written once.

**Found while verifying, fixed here (C2):** `camera_input_test` turned 1.061 rad for a drag worth 0.756 rad in one run of four.
The gestures timed the hand on the wall clock (`Time.get_ticks_usec`) while the springs, the coast and the test's still wait run
on the frames' deltas; when the two clocks parted, a hand that stood still for 0.15 s of frames was still moving by the wall's,
and the view was thrown. The gestures keep their own clock now, the deltas `advance` receives (`camera_gestures.gd`): one time
base for the whole camera.

Carried in the same commit at the scenic city session's request: its scenic key renames (`scenic.json`, its schema,
`scenic_city_test`), applied to the rewritten loader, and one value (barra_funda `blocks_m` 130 -> 128).

### 6.9 The frozen photos' residual, diagnosed again (2026-11-07)

B0 (§6.6) put the run-to-run pixels at the utility wires' joints, from one photo. Measured over ten runs of two trees with one
DLL, they are single pixels anywhere in the picture (`02-rua`: (106,130), (80,131), (25,213), (368,245), (1110,273) in one pair of
runs, (1157-1167, 264-265) on a sign's edge in another), all of 1 level but one of 11, `00-cidade` and `02-rua` in three or four
states, `01-bairro` and `03-rua_noite` always equal. Not a geometry tie then (a tie sits on an edge between two meshes), and not
time (the frozen look sets `Engine.time_scale = 0`, which stops shader TIME too). What is left is how Godot draws a surface whose
specialized pipeline is still compiling: with the ubershader, the same material compiled another way
(`render_forward_clustered.cpp`, `ubershader_iterations` and `wait_for_compilation`), and two codes may round apart. The engine
counts compilations when they are submitted (`get_pipeline_compilations`), and waits for them only for the ubershaders of a mesh
being loaded (`mesh_generate_pipelines`): no script can know when the background ones are done, so the photo cannot wait on it.
`photos_check` fails on these pixels in every run of an unchanged tree; until the engine gives a completion signal, a change is
judged by its photos being byte-equal to one of the states the unchanged tree gives (log/pendencias.md).

### 6.10 B2, C6: the reads that broke the lock protocol (2026-11-08)

The protocol (`city_state.h`): what commands change is read under the map shared; traffic under the map shared and then
`traffic_mutex`. Fixed where the code broke it:
- **The walkers' age in the effects (L2).** `effect_bodies.cpp` read `s.traffic.peds[e.id].age` under the map alone, and the
  event's walker had often been removed already (`strike_people.cpp`, `wounds.cpp` call `remove_pedestrian` right after the
  casualty, a swap-remove): the dead body took another walker's age, or none. The age travels in the event now
  (`damage_event.h`, `cas_age`: bits 19-20 of the casualty flags), and the render reads no traffic. Bodies in the blast
  captures may change body for the better: the body is the walker's.
- **Lot points before the lock (A1 a).** Fourteen body commands turned a world point into a lot point before `run_edit` took the
  lock, while growth may append buildings: `edit_of_body_in_lot` (`body_api.h`) hands the command the lot frame taken inside the
  edit's lock, and `lot_local` owns the conversion that `lot_point` and the handles' drafts use too.
- **Traffic read with one lock of the two (A1 b-d).** `shot_targets` and `damage_probe` held the map alone; `kerb_busy`,
  `graph_fault`, `flow_state` and `transit_run` held `traffic_mutex` alone while commands change traffic under the map's
  exclusive lock without it; `flow_push` read the node and its legs with no lock (and indexed `nodes[-1]` when no junction was
  found); `bay_census` read ground patches after letting the map go. One owner of the traffic read, `TrafficRead`
  (`api/state_locks.h`, map then traffic), and the census holds the map to its end.
- **Previews under the exclusive lock (A2).** `preview_roundabout`, `segment_at`, `preview_lot` read through a const state: they
  take the map shared now, so a pointer move no longer waits for every mesh job and stops the simulation.
- **TOCTOU in `toggle_lane_link`.** The traffic's built links were read under one lock and the edit made under another: both are
  inside the edit now (map exclusive, then `traffic_mutex`).
- **A signal command read a stale graph.** `signal_commands.cpp` asked the traffic's junctions, rebuilt from the change log
  later: it asks the road network (`traffic::control_of`), as the junctions are built from.

**Left of C6, the lock's architecture** (one reader-writer lock for the whole city shared by the frame, the simulation, the
commands and the mesh jobs, which hold it for their whole build; the frame's main-thread work inside it; the camera and the tools
asking the core several times a frame and at every pointer move; the Street tab's settings stored in the state): a design of
its own, with the measures that judge it (`sampler.py`), not an identity change.

### 6.11 B2, C7 (R1, L6, A7): one owner of the buildings' derived bodies (2026-11-08)

**Cause.** A building record holds only what its body is made from, and `record_body`/`record_mass` (`b40/record.h`) ran the
generator into the caller's own `Body` each time. Every reader paid the whole generator per call: picking on every pointer move
(`body_pick.cpp`), parking gates per segment layout (`lot_access.cpp street_gates`), growth's neighbours and candidates
(`growth.cpp`), the interior view (`interior_commands.cpp`), doors, bays and the yard (`ped_doors.cpp`, `parking_*.cpp`,
`yard_input.cpp`), every handle drag frame (`city_core_handles.cpp`) and the API's queries; and five readers kept a private copy to
escape the cost: the render's `BuildingBodies` (emptied whole at 8,192 entries, every body made again in a storm), a second one
per canyon job, `BodyWalls` (cleared on any revision), `GardenBook` (cleared whole at 65,536) and the inspector's
`InspectCache.solids` (never evicted, and written under a shared lock).

**Fix.** The bodies made from the generator's tables are kept by the tables themselves: `b40::BodyCache`
(`sim/buildings/b40/body_cache.h`), held by `Data::made` like the edited bodies' `BodyTable`, so a new filling of the tables
starts empty and a copy of them shares it. `record_body` and `record_mass` are its front and return a shared, never-changed body;
an edited body is the record's own version (`BodyRef::shared`, no copy), its mass kept like a seed body's; an editor copies
(`body_edit.cpp`, `land_commands.cpp`, the handle draft, `fit_plan`, `piece_at`). The bound is memory (`sim.body_cache_mb`,
128), the least recently asked pushed out by `core/lru_cache.h`, which also keeps the render's rooms and cut bodies, the two
derived things that are the render's alone (no more storms). The private caches are gone: `BuildingBodies` places and derives
rooms and cuts only, the canyon job, the walls (`walls_near`), the gardens and the camera's solids read the shared bodies.

**The key had to be exact first.** `body_key` named the frontage and depth to a decimetre while the generator reads them whole, so
two records with one seed and widths 5 cm apart shared one body in the render's cache: the second was drawn with the first's
walls. A shared cache read by the simulation would have handed that wrong body to picking and parking too. The key now holds
the floats' bits, and the function and seed for an edited body as well (an edited body's purpose reads the seed:
`interior.h purpose_of`), so two records with one key have one body and the same derived things; the render's rooms key needs no
seed of its own.

**Why the symptoms do not come back.** Nothing outside the cache makes a record's body: `record_body`/`record_mass` are the only
callers of the generator for a record (`make_seed_body` is called by the cache alone), and a reader holds the shared body it was
given, so no reader has a reason to keep its own. Derived from a body only by the render (rooms, cuts) stays in the render, keyed
by the same exact key.

**Verified** (`city-verify`, `quick --all`): both DLLs, 43 of 45, the digest equal (the city state is the same), `gates_changed`
only the eleven violations the files already had. `photos_check`: `01-bairro`, `02-rua`, `03-rua_noite` equal; `00-cidade` 7 or 8
single pixels of 1 level, in two states over four runs, one of them byte-equal to the state `27ba…` the C6 tree gave at 23:51
(the task cache keeps each run's photos). Two pixels, (122,546) and (123,546), stood in every run of this DLL against the golden
and the other saved state; the one candidate for a real change, the exact key, was tested by building this commit with the old
decimetre key: the same two pixels stood, so they are not the key. They are part of the C6 tree's own state `27ba…`.

**Found on the way, left for B3 (it changes the generated city).** Growth remembers a building's look as the look of the
candidate it chose, made with the middling floors, under the key of the record with the storeys that candidate reached
(`growth.cpp record_for`, `looks[body_key(b)] = kept.look`): when the cap shaped the body (`massing_tall.cpp:146,172,182`), the
standing body is another, and after a load the look is worked out again from the standing body, so growth after a save and a
load differs from growth without them. The fix (looks derived from the kept body, `BuildingStore::looks` deleted, `look_twins`
under the shared lock: A7) moves the generated city where a cap shaped a candidate.

### 6.12 B2, C7 (R8): what is derived from the profiles has an owner (2026-11-08)

**Cause.** The profiles grow in play (a variant is a profile appended: `road_variants.cpp`) and nothing owned what is derived from
all of them, so each reader searched them again: the widest half per call (`Catalog::widest_half`, again in `road_snap.cpp` per
pointer move), the widest sidewalk per traffic tick (`ped_crowd.cpp`), the ring profile per roundabout command; and four places
wrote their own guess of the widest street instead (`footway.cpp` 48, `street_spots.cpp` 40, `road_edit.cpp` 40,
`transit_ground.cpp` 4 x 12), short of a player's own wide road (lanes up to 6 m, sidewalks up to 12 m).

**Fix.** `Catalog::add_profile` is the one way a profile comes (the loader and a variant) and keeps the widest half, the widest
kerbside sidewalk and the first ring profile as each comes; `drop_variants` works them out again over the catalogue's own. Each
guessed pad is now the exact reach of what its search accepts, from the widest half: a point on a segment's band
(`hypot(widest, BESIDE_M)`: `on_band`, `median_at`), a point whose footway is within `reach` (`footway_reach`: `footway_at`, the
items a segment merges, the segments a placed item marks), a point within a profile's half (`transit_paved`). On every map of
the tests the old pads held the exact reach, so nothing moves; on a wider road the search no longer misses what it accepts.

**Verified** (`city-verify`, `quick --all`): 44 of 45, the digest equal, `gates_changed` clean, `00-cidade` byte-equal to the
state the B0 DLL gave (22:49) and to five other runs before C7.

### 6.13 B2, C7 (R3, L8's furniture): a segment's street furniture placed once (2026-11-08)

**Cause.** `traffic::street_furniture_of` placed a segment's kit (`segment_furniture`, O(n2) slot loops), merged what the player put
up and cut (`merge_street`: `footway_at` over every segment in reach for every item) and filtered what a blow knocked out on every
call: three times per segment per props chunk (`street_props.cpp` lamps, props, trees), at every pointer move of the Street tab
(`street_snap.cpp`), and in four probes; only the walkers kept their own derived copy.

**Fix.** `FurnitureKept` (`sim/street/furniture_kept.h`, on the session's `CityState`) keeps each segment's placement of every kind
for the city's revision: every change to the map moves it (`touch()`, which a new map and a load also call, so the revision never
comes back), and the entries are dropped together when it moves. The kinds only choose among the pieces placed (each of the three
steps filters a piece by itself), so the kept placement of every kind, filtered, is the placement of the kinds asked. Only a
reader under the map's shared lock reads it kept (the render's jobs, the walkers, previews, probes): nothing changes while it
reads. An edit may read a segment before and after it changes something in the same command (a row of items: each one placed
counts for the next), so the editors ask `FurnitureRead::PLACED` and place it anew: removing a kit piece, the blow knocking pieces
out, and `snap_item` when a command calls it (the preview passes `KEPT`).

**Left of R3 for B3 and B4.** Items keep only (x, z) and find their footway again on every placement; storing the footway place
when an item is put down changes what an item does when its road moves (B3). The placement living in `city::traffic` (with
`junction_control`) is the misplaced owner the C16 pass moves.

**Verified** (`city-verify`, `quick --all`): 44 of 45, the digest equal, `gates_changed` clean; `00-cidade`, `02-rua` and
`03-rua_noite` each byte-equal to runs before C7 (the last two to the saved state of 21:21).

### 6.14 B2, C7 (R4's book, the lot snap, the zone overlay): the player's lots filed by their book (2026-11-08)

**Cause.** The book of the player's lots was a bare vector anyone wrote, so nothing could keep what is derived from it: the lot
cache built a `LotIndex` (every lot's stand worked out again) on every recompute; `LotBook::find/at`, the journal's
`lot_index`, five lot commands, the lot snap on every pointer move (`lot_corner_near`) and each zone overlay job
(`overlay_build.cpp`) walked every lot.

**Fix.** `LotBook` owns its lots: the edits (`add`, `insert`, `erase`, `set`, `assign`: the journal, undo and redo, a load) file
it again as they change it (each lot's stand, the lots by place, by id, and the zoned ones by their stand's cell), and the readers
ask it: `near(box)` gives the lots round a box in the book's order, so every answer is the one walking the book gave (the first
lot holding a point, the nearest corner on a tie, the order the journal records). `LotIndex` is gone. Splitting by
responsibility what this made too long: the ring geometry left the book (`sim/lots/lot_rings.{h,cpp}`), the paving as polygons
left the lot against the streets (`sim/citygen/lot_paving.{h,cpp}`).

**Left of R4 for B3.** Lots derived per 256 m cell from a 576 m window, under the exclusive lock in `finish_command`: deriving
them per block in jobs outside the lock moves when lots appear, and growth with them (B3, with its predicted diff); the three
constants of one reach (`lot_cache.cpp:15`, `lot_gen.h:20`, `road_network.cpp:16`) go with it.

**Verified** (`city-verify`, `quick --all`, with the splits): 43 of 45, the digest equal; `00-cidade`, `03-rua_noite` byte-equal to
runs before C7; `gates_changed` only what the files had (`overlay_build.cpp` 0xff, two constants that moved with the geometry,
`edit_history.cpp` and `journal_ops.cpp` already past the size limit).

### 6.15 B2, C7 (R9): the transit reads its own list of the change log (2026-11-08)

**Cause.** The change log gave each list one consumer, and the transit needed the segments the traffic drained, so the traffic
handed them on through a second channel of its own (`TrafficState::roads_touched`, filled in `lane_graph_edit.cpp`, taken in
`transit_plan.cpp`): the log was not the one channel its header and CLAUDE.md say it is.

**Fix.** `ChangeLog::transit_segs`, filled by the same producers (`net_seg`, `net_seg_removed` append to the traffic's list and the
transit's), drained by the transit under the log's mutex, cleared with the network's lists and taken in with a city read aside.
The traffic applies the network changes and the transit reads the plan in the same step under the same shared hold
(`traffic_system.cpp`: `apply_network_changes`, then `transit::step`), so the transit sees the same segments at the same tick.
The header now says what is not a channel: an owner's own pending work (the network's shapes and grades to settle, growth's
queue) and what is kept by the revision or by an exact key.

**Verified** (`city-verify`, `quick --all`): 44 of 45, the digest equal, `gates_changed` clean, `00-cidade` byte-equal to runs
before C7.

### 6.16 B2, C7 (L7, L8, fog, verge): the render jobs derive each place once (2026-11-08)

**Causes.** (L7) A chunk's ground (roads, lots, ecosystem: `chunk_ground`) was gathered by the near trees, again by the far cards
for each of the sixteen chunks of a superchunk, and again by the cover; its query grid kept a mutable scratch, so no two jobs could
share one. (L8) A node's ends and each corner's kerb and outer lines were worked out by the road job and again by the verge job, and
a segment's runs of retaining wall by the structure job and again by the verge job (backfill). The fog job copied every dab and
filed all of them again in its buckets at each revision, and scanned its whole 512 x 512 window for the slab; the verge sampled the
ground three times per point across.

**Fix.** One owner of "derived for one place, for the city's revision": `render/kept_by_revision.h` (an LRU keyed by place and
`CityData::revision`, which every change to the map moves; the jobs ask under the map's shared lock). `GroundKept`
(`render/trees/ground_kept.h`, `render.ground_cache_mb`) gives the trees and the cover one ground per chunk; the grid's scratch is the
thread's own, so a ground is only read. `RoadShapes` (`render/roads/road_shapes.{h,cpp}`) gives the road jobs one shape per node
(the construction moved there from `road_junction.cpp`, its owner) and one set of wall runs per segment. The fog files only the dabs
past the first changed one (indices, each leaving dab the last of its buckets, so every bucket keeps the order a whole filing
gives) and keeps each row's slab, working out again only the rows a span wrote (a least and a most are the same in any order); its
job is split by responsibility (`refile`, `paint`, `slab`: `work` 121 lines at HEAD, 76 now). The verge samples each point once and
leans each normal off its neighbours: with `strips` 1 (the data) the old t - dt and t + dt were exactly those neighbours.

**What the gates showed.** The shared shapes made the verge and the structure renderers identical past the duplication limit: they
were one class with another material, shadow, job lane and emitter, so they are two instances of `RoadLayer`
(`render/roads/road_layer.{h,cpp}`), and `verge_renderer.*` and `structure_renderer.*` are deleted. `TAU_D` moved to `core/math.h`.

**Verified** (`city-verify`, `quick --all`, three runs over the batch): the digest equal; every photo byte-equal to runs before
C7. Once, `01-bairro` (equal in every run until then) showed four single pixels of 1 level; three fresh runs of the same DLL gave
it equal to the golden: the run-to-run residual (§6.9) reaches it too. `gates_changed` only what the files had.

### 6.17 B2, C7 (the script side): what the tools and panels ask, asked once (2026-11-08)

**Causes and fixes**, each an owner for something the script derived or asked again:
- **The road list.** `road_profiles` was asked of the core at every move of the road tool (`road_draft.gd`), by the upgrade tool for
  one width, and the road catalogue read the strips from the JSON files again (`DataTables.road_profiles`), against the rule that the
  interface reads the core. The core gives the strips (`STRIP_KIND_NAMES`, now the one owner of the kinds' names, read by the loader
  too) and the snapshot a `profiles_revision` (`Catalog::profiles_stamp`, a stamp that moves with each profile added and each drop and
  never comes back), so `CityStore.cached(&"road_profiles")` asks again only when the list changed; every reader takes it from there.
  `DataTables.road_profiles` is gone. The catalogue's pictures read the core's widths, the floats the roads are built with.
- **One render table read whole.** `DataTables.render()` (every render table) was asked for one: `render_table(key)` everywhere.
- **The render cost twice a frame** (the lab's meter and the Effects panel): `CityStore.frame_render_cost()` reads it once a frame.
- **The pole run planned twice on release** to learn its last pole: `build_pole_run` hands the last pole back (`last`).
- **Per pointer move.** The model in hand asked `place_fit`, `height_at`, `stage_move` and `place_check` (three locks and a check
  against the city each move): `place_hover` gives the fit, the ground and the problem under one lock, and checks again only when the
  model, the place or the city's revision moved. The demolish rectangle asked the ground five times (`heights_at`, once); the cloud
  tool asked the whole landscape to find one cloud (`cloud(id)`). The lots tool reads the street's angle once a gesture and the lot
  under the pointer once a hover. The interior hand read the `interior` view (which works the lamps out) and the floor's height at
  every move: it reads the level from the interior layer, which the hand now asks to refresh when it opens a building, and keeps the
  floor's height and the pieces' sizes until the building, the floor or the city moved.

**Left of C7 for B3 and B4** (each changes what the game does): growth's look memory (§6.11), lot growth drawing four whole bodies
(choose by the look's inputs first), lots per block outside the lock (§6.14), items keeping their footway place (§6.13).

The lots tool, one line too long with this, gave its picks of the drawn lots to `LotOutlines` (`game/tools/lot_outlines.gd`).

**Verified** (`city-verify`, `quick --all`): 43 of 45, the digest equal, every photo byte-equal to runs before C7; after the lots tool's
split, `import`, `scripts`, `e2e` and `building_tool_test` pass; `gates_changed` only what the files had (`inspector_panel.gd`'s size
and two untranslated strings, `profile_types.h`'s two constants, `city_core_query.cpp`'s two lines).

### 6.18 B2, C3 (P3): the core names its enums; scripts, tests and captures read them (2026-11-08)

**Cause.** `constants()` gave the scripts two speeds, the zone count, two minute counts and the render layers, so every other code
the core means was typed again by hand: transit modes, junction rules and controls, crossings, lot cuts, mass actions, the
building enums, vehicle and furniture kinds, items, barriers, lamps, pole kinds, brush modes, plan shapes, building functions,
map and profile indexes by catalogue position, hours, tuning values. Copies had already gone wrong: `capture_demolition.gd`'s
"NORMAL_SPEED 2" is FAST, `shoot_test.gd` capped the shots to burst a car at 14 against the data's 12, `save_undo_test.gd` named
two marking styles where `streets.json` has five; the frame's sections were listed three times, two of them disagreeing (the CPU tab
never showed "lock" or "trees").

**Fix.** `api/script_enums.cpp`: each enum by its C++ name, each value by its enumerator's own name (stringified) and value, so
neither can drift; where the enum states a count, a `static_assert` refuses a list that leaves one out. `constants()` carries it as
"enums", with `chunk_m` (the core's cell) and `frame_sections` (`api/frame_sections.h`). Scripts ask `CityStore.code(&"Enum",
&"NAME")` (an error for a name the core lacks) and `code_name`; tables keyed by a code are keyed by the enumerator's name. Profiles,
maps and zones are found by their ids (`CityStore.profile_index`, `DataTables.map_index`, `DataTables.zone_code`), hours by
`daylight.json`, tunings by `tuning.json`. Owners given on the way: `BaseZone` (the base kinds three `.cpp` typed as 1..4, the
loader now refusing a base past them), `FurnitureLevel` (a constant local to `street_furniture.cpp` that a capture copied). The
hour tables of the captures name `day_hour` or `night_hour`; the speed pill reads its multipliers from the tuning.

**What changes.** The F7 CPU tab shows every section of the frame, "lock", "trees" and "overlays" too (with their translations);
`capture_demolition` runs its wound clocks at the normal speed it names; the shoot test fires the data's twelve shots.

**Left for B4 (data rewrite).** Codes written as numbers in data: the UI's tool values (`builder.json`, `drawers.json`,
`land_panels.json` and the mass tool's SLIDE), positional rows (`blast.json` vehicle boxes, `growth.json` civic kinds, the 18
element kinds in four files, marking styles by index). They become names resolved at load against these same enums.

**Verified** (`city-verify`, `quick`, both DLLs): every test passes (42 of 44, the shoot test with the data's twelve shots), the
digest equal, every photo byte-equal to runs before C7, `scripts` compiling every script; `gates_changed` only lines the files had.

### 6.19 B2, C3 (the shaders' side, first part): what the shaders share, written once (2026-11-08)

**Cause.** A shader cannot include a C++ header, so every value a renderer writes into a vertex or an instance and a shader
reads back was typed on both sides with a comment naming the other: the door's and the brake's flags in an agent's custom
data, the people's clip and body tables (the uniform arrays sized 24, 32, 48 and 96 by hand), the face acts, the helmets'
colours, the building surface kinds, a pane's and a bay's byte steps, the sill's sizes, the opening's shades, the brise fins,
the ruin pieces a row, the finish layers, the tree parts, the leaf atlas regions, the far cards' tint, the wear layers, the
road tones, the lenses and the dry shore level; some of them a third time in a script (`lamp_radiance.gd`, `surface_means.gd`)
and the luma weights five times in `terrain_layers.gdshaderinc` and once in `city_world.gd`. Copies had already moved apart
in form: the shader's `HEAD_SHADE` was the product 0.544 of the C++'s two shades, its `SILL_PAST_M` half the C++'s, its
`SHORE_DRY` half of `NO_WATER`, each kept equal only by a comment.

**Fix.** `tools/build/shader_contracts.json` holds each value once, with what it means and who writes and reads it;
`tools/build/shader_contracts.py` makes `native/src/core/shader_contracts.h` (constexpr values, the `SurfaceKind` and
`TreePart` enums, and `SHADER_CONTRACTS`: every value by the shaders' name) and `project/render/shaders/shader_contracts.gdshaderinc`
(guarded, so any shader and include takes it). The renderers and the shaders read those names, the uniform arrays are sized by
them (Godot reduces a constant expression in an array size: `ShaderLanguage::_parse_array_size`), and a side that needs a
value derived from one derives it in its own code (the head's shade as `REVEAL_SHADE * HEAD_SHADE`, the sill's overhang a side
as half of `SILL_PAST_M`, the dry shore as half of `TERRAIN_NO_WATER`). Where C++ keeps its own enum (`Finish`, `Lens`) a
`static_assert` holds its count to the layers the shaders read. The scripts read `CityStore.constants.shader`, a float as the
table's decimal at double precision, as their literals were. Quick's `shader_contracts` task (`--check`) fails when the header
or the include is not what the table makes; `tools/build/tests/test_shader_contracts.py` tests the generator.

**Why it does not come back.** A value shared across the boundary has one place to be written, and both generated files are
checked against it on every quick run; a new shared value with no entry has no name on the other side to read.

**What changes.** Nothing a player sees: every derived value is exact in float32 (0.68 x 0.8 = 0.544, 0.14 x 0.5 = 0.07,
1 / 0.5 = 2, -1000 x 0.5 = -500, 1 / float(6) = 1 / 6.0) and the scripts get the same numbers. One trap, found by the photos:
`city_world.gd` held the luma weights in a `Vector3`, whose components are float32, so the grass mean was worked out with 0.3
rounded to a float; read as the table's doubles it moved by an ulp and a few pixels of `00-cidade` with it. The script keeps the
weights in a `Vector3`, as before.

**A shader that does not compile passed 42 tests.** The first run of this change had `building_far_window.gdshaderinc`'s own
`REVEAL_SHADE` (0.42, the far level's, another value under the same name) clash with the table's: every building material fell
back, and only the light test and the photos saw it. `tools/tasks/verdicts.py` failed a Godot task on a `SCRIPT ERROR` (which does
not stop a scene) but not on a `SHADER ERROR` (which does not either): it now does (`tools/tasks/tests/test_verdicts.py`, red
before the rule, green after). The far level's shade is `FAR_REVEAL_SHADE`.

**Left for the second part** (§6.20; the same table; the review's §3s list, which this first pass did not cover whole): the bay's piece
kinds, mesh mark, line names and code shifts (`bay_kit.cpp`, `shell.cpp` `Shell::bay`); the finishes by index in the shaders
(`fin != 8 && fin != 7`, `fin == 8`: the `Finish` enum); the surface kinds by number (`building_body` 3, 4, 5; `building_far` 1..4),
the kind bits and the far level's office bit (`shell.cpp`, `building_renderer_build.cpp`), the far windows' `PaneDark` codes, dark
shift and office flag; `CardKind`; the condenser's case (from `bay_props.json`, not typed), the canyon cell; the control map's bit
layout (`terrain_bridge.cpp`) and the biome tints' size; the people's face table, palette span, bone texels and bones; the agents'
`AgentTag` and `Limb` marks and the pane's sill span; `PropTag`, the span's kind stride and the secondary wire's mark; `RoadMat`,
its scale, the pool sentinels and the wear spans; the effect marks (debris, bodies, the frightened); the engine's ACES, mirrored in
`tone_inverse.gdshaderinc` and `tone_map.gd`; the lane tones by the key order of `asphalt_by_class`.

**Verified** (`city-verify`, `quick --all` then `quick` and the photos, both DLLs): every test passes, the digest equal, every
photo byte-equal to a run before C7, `scripts` compiling every script, `shader_contracts` passing; `gates_changed` only lines the
files had (four fewer).

### 6.20 B2, C3 (the shaders' side, second part): every C++/shader contract in the table, by domain (2026-11-08)

**Cause.** The rest of §3s's list, and what a sweep of every shader's decode of its vertex and instance data found besides: the
bay's piece kinds, mesh mark, line names and code shifts (`bay_kit.cpp`, `shell.cpp` `Shell::bay`) read back by thresholds and
masks typed in `building_bay.gdshaderinc`; the finishes by index (`fin != 8 && fin != 7`, `fin == 8`, `fin < 13`); the surface
kinds by number (`building_body` 3, 4, 5; `building_far` 1..4) and their bits (3, 7, 128 in two shaders, `PANE_KIND_BITS` and
`FAR_KIND_BITS` in two C++ files); the far windows' flags (`& 4`, `& 8`, `>> 4 & 3`) and `PaneDark` codes; `CardKind`; the
condenser's size typed in `facade_props.cpp` and in `building_props.gdshader`, both copies of its box in `bay_props.json`; the
canyon cell; the control map's bit layout (`terrain_bridge.cpp` and `ground_paint.gdshaderinc`'s literal shifts and masks) and
the biome tints sized by the data's biome count; the people's face table, palette span and texels, bone texels and bones;
`AgentTag` and `Limb` as `tag == 1..7` and `UV2.y > 0.5, 1.5, 2.5`, the door pane's sill span; `PropTag` as `v_tag - 3.0..9.0`,
the span's kind stride and the secondary wire's mark; `RoadMat` as `M_ROAD..M_ARM_MEET`, its scale 16, the pool sentinels
±1000 and the wear spans; the debris marks, `PuffKind` and `MarkKind` as threshold ladders (`kind > 4.5 && kind < 5.5`); the
engine's ACES in `tone_inverse.gdshaderinc` and again in `tone_map.gd`; the lane tones filled in the key order of
`asphalt_by_class`; the light tests' copies (lens and glow tags, the road's material and scale, the plate's UV2, the walls'
roughness).

**Fix.** Every one is a row of `tools/build/shader_contracts.json`, which now holds its entries in groups by domain (agents,
people, buildings, effects, trees, streets, land, tone): the generator writes one C++ header and one shader include a group
(`native/src/core/shader_contracts/<group>.h`, `project/render/shaders/contracts/<group>.gdshaderinc`) and `all.h` of them for the
scripts' export, so a renderer and its shaders take their own domain's contracts and nothing else (the single header had grown to
409 lines of every domain). The table gained what these needed: a vec3 (a `Vector3` for the scripts, as their literals were),
named sets of ints and floats, enums numbered from a start and declared in a C++ namespace (`b40::Finish` keeps its name). The
shaders read the names and derive the rest (`halfway` between two bay lines, a mask from a field's bits, a threshold half a step
below a mark); the C++ enums `Finish`, `PaneDark`, `CardKind`, `AgentTag`, `Limb`, `PropTag`, `RoadMat`, `PuffKind`, `MarkKind`
and the bay's pieces are declared there, once. The condenser's case is read from its box marked front in `bay_props.json`
(`BayPropLook::ac_case`): `facade_props.cpp` places by it and `BuildingRenderer` sends its front to `building_props.gdshader`. The
people's loader now refuses an asset whose skeleton is not `PEOPLE_BONES`, which the skin shader clamps to. The lane tones are
filled by `RoadClass` (exported to the scripts), not by key order. `--check` also fails on a file in the output folders the table
does not make.

**Kept outside the table.** Terrain3D's `MAX_REGIONS` (the addon's own size, which its shader override must match) and
`signature_look`'s `GLASS_CELL_M` (the cooked signature asset's room, from a tool outside the repository): neither has a second copy
in the game. The engine BRDF copied in `building_glass_body.gdshaderinc` `light()` and `people_look.gdshaderinc` (no test holds them
to the engine's) and the positional data rows (`ground_paint`'s surfaces by slot with literal colours, the crater colours by
index) go to `log/pendencias.md`, the latter with B4's named rows.

**What changes.** Nothing a player sees: integers and marks are exact, every derived float is exact (a half of a dyadic step, a mask),
the condenser's 0.72 x 0.48 x 0.23 m come out of the data's box bit for bit (a doubling of the half extents), and the scripts get
the numbers they had.

**Verified** (`city-verify`, `quick --all`, both DLLs, then the light tests again): every test passes, the digest equal, every
photo byte-equal to a run before C7, `shader_contracts` passing; `gates_changed` only lines the files had. The first run failed
`light_color_test`: it read `CityStore.constants` in member initialisers, before the store's `_ready` fills them; it reads them when
it starts.

### 6.21 B2, C3 (the scripts' and the interface's side): contracts the scripts kept by hand (2026-11-08)

**Cause.** What §3q, §3r and two §3c/§3d findings left after C3a: the popovers built by `call("_build_" + which)` and
`call("_seg_" + r.seg)`, a name in `drawers.json` with no method failing only when pressed; times stored as design tokens and read
through the size reader (`tip_wait_ms`, `cn_reread_s`, `toast_s`); two conventions for the hint signal (`ToolController.hint` says the
text comes translated, `lots_tool` and `road_blocks` sent raw keys, which showed translated only because the label translates again);
four places cutting a translated name at " (" and the sliders recognising metres by the display string " m"; `MS_PER_S`, `PERCENT`,
`MIB` declared again in 30 scripts (`HudWidgets`, `Formatters`, `QualityTier`, `CityCamera` and an inner class of the profiler each
holding one); the editor's strip names mapped to the catalogue's three times (`road_spec.cpp` once each way, `road_spec_ops.gd` again);
a padding read from the `card_radius` token and a bare pixel added to `pad_s`; the element kinds split into things and effects by the
length of a render table's row list (`element_effects.gd` passing `cover.json` `props.size()` to `effects_near`); a transit line's
colour sent as a packed number and unpacked in two scripts, and one text placed by `replace("{n}")`; the setback's and the bevel's
sentinels typed bare (`-1.0, 2`, `-1`); fourteen generator road roles named by profile id in `city_gen.cpp`, two of them the same
profile, an unknown name giving -1 unchecked; the Build tool's handle edit comparing its kind as a string, nine times a drag tick.

**Fix.** The popovers and the menu's segments are tables of callables, every popover and segment of the data checked against them at
start (the segments moved to `ui/pop_menu_rows.gd`, the popover script past its size). The timings live in `hud.json` `timings`, in
seconds, with a schema. The emitters translate (`lots_tool`, `road_blocks`) and the hint and toast labels no longer translate again:
a key that slipped through shows as one. The asides nothing showed are gone from the translations (24 keys, both languages), the
three street cards whose tooltip shows the whole name have a short key and a `.tip` key named by their data (`tip_key`), and no code
cuts text; a slider's unit is an id (`length` formatted as the style writes lengths, the others by an i18n pattern `unit.<id>`).
`game/units.gd` (`Units`) holds the units once. The core maps a composed element to its catalogue strip in one function
(`road_spec.h` `strip_of`), which the reverse reading and the scripts' table (`constants.spec_strips`) use; `SPEC_KIND_NAMES` sits
beside `STRIP_KIND_NAMES`. The theme has its own `choice_pad` and `icons_pad`. `nature.json` names the first effect kind
(`first_effect`, resolved and checked by the loader); `effects_near` reads it, and the element models refuse a props table whose row
count is not the catalogue's laid kinds. The line colour comes as a `Color` (`rgb8_to_color`, the scripts' own float maths), the
stops through `format`. The sentinels are the core's (`SETBACK_BY_MODULE`, `EVERY_CORNER`, exported by `constants()`), the setback's
storeys the card's value in `builder.json`. The road roles are `growth.json` `road_roles` (a schema, the loader refusing a profile it
lacks; `CITY_ROAD_ROLES` the one list, which `GenProfiles` takes). The handle kind is `HandleKind`, read once from its name where the
API parses the request; one name table serves the handles the core sends and the edits it reads.

**Not changed, and why.** The cloud tool's yaw from where a cloud is put and the core's random yaw of the clouds it scatters are two
rules for two origins, not one value copied: moving the tool's rule into the core would change a placed cloud (B3 at most). The UI
data's mode codes written as numbers (`land_panels.json`, the element and cloud tools) were put to B4's named rows in §6.18. The zone
codes typed again (`parking_sources.cpp` and others) go with C4's "six owners", the same cause and the same fix.

**What changes.** Nothing a player sees: every shown text is the one shown before (the cut gave the same words; the tooltips are the
same), and the numbers are the same. The loaders refuse data that would have gone wrong silently: a road role naming no profile, a
`first_effect` that is not a kind, a props table that does not match the kinds.

**Verified** (`city-verify`, `quick --all` then `quick`, both DLLs): every test passes, the digest equal, every photo byte-equal to a
run before C7, `scripts` compiling every script; `gates_changed` only lines the files had. The first run failed `scripts`: four tools
read `PERCENT` inherited from `BrushTool`, whose own copy had gone; every unqualified use now names `Units`.

### 6.22 B2, C3 (the tests' side): what the tests and captures copied (2026-11-08)

**Cause.** What §3u listed and C3a had not reached: the town's node count typed (`flow_control_test` STOP_NODES 320, for map 0 seed
1 only); the Build strip's sections and cards by position (`building_plan_cases` `sections[1]`, `building_tool_test`
`sections[0].tools[0..2]`, the lot cards `cards[1]`, `cards[2]` of `capture_features`, the street signs' first two and from the
eighth on in `capture_street_views`); the street block's tag as four bytes (`street_save_cases`); the yard's reach, a vehicle
type, the turn speed in m/s, the speeds, a zone, a brush mode and the civic functions by number; the chunk's flags as bits (1 near,
2 mid...) read from an int; the crater's decal scale (`marks.h` CRATER_SCALE) and the lantern's height (`builder.json`) typed; 3 m a
floor; the element kinds' count; a sheet's capacity of street names ("three times what one sheet of 104 held"); the fuzz tests' own
edits as bare numbers with ranges by comparison, the primitive and the wall run copied from the strip, the bevel's sentinel typed.

**Fix.** The core gives what a test reads: the chunk as named fields (`BuildingRenderer::chunk_view`), the blast's decal reach
(`blast_probe` `decal_half_m`), `YardReach` among the scripts' enums, the street block's tag (`save_tag_street`); `constants()`
moved to `api/city_core_constants.cpp`, the query file past its size. The tests find a section by its title, a card by its id or the
function it lays, a sign by its type (`SignType`), a speed button and a sky button by what they set (`HudTopBar.sky_button`,
`speed_button`); they read the node count from the snapshot, the turn speed from `traffic.json` (`Units.KMH_PER_M_S`), the kinds
from the catalogue, a roof from the body's storeys, the lantern from the strip; the names test fills the sign sheet until three
layers are in use. The fuzz tests name their edits (`Edit`, `BodyEdit`, the same numbers, so the same draws and the same hashes) and
take the primitive and the run from the strip's cards.

**Left, and why.** `save_land_cases` (the save header's byte offsets) and the street save test's file without a street block test
the reading of an older format, which D4 drops with C9's save layout: they go with that change.

**What changes.** No test asks less: the stop test now covers every node of whatever town it opens, the names test fills three
layers however many names a layer holds, and the captures aim at the same places by their names.

**Verified** (`city-verify`, `quick --all`, both DLLs): every test passes (the fuzz tests with their hashes, the flow test over every
node), the digest equal, every photo byte-equal to a run before C7; `gates_changed` only lines the files had.

### 6.23 B2, C4 (the core, first part): what the core wrote twice and merges without a bit moving (2026-11-08)

**Cause.** §3a/§3b/§3c/§3e/§3j/§3l/§3n's copies of one value or one primitive: lowbias32 twice (`core/random.h` `hash_u32`,
`seeded.h` `hash32`); pi and a turn typed again in eleven files (`interior/plan.h` even shadowing `PI_F` in `b40`); the chunks a
superchunk spans in four files and the sub-chunks a chunk holds in two; √2 computed in two; a ruin piece's size twice
(`ruin_fracture.cpp` `LONGEST_SIDE_M`, `ruin_cells.h` `VORONOI_EDGE_M`) and a mound's spread twice; a roof's parapet, attic, coping,
machine room and slab lift in both levels (`roof_emit.cpp`, `far_mass.cpp`) and the machine room once more as `3` in `roof_plant.cpp`;
the casing three times (`road_grade.cpp`, `road_structures.cpp`, `road_walls.cpp`); `in_ds` twice; the clock's hour set by hand beside
`CityClock::set_hour`; the convex hull twice; the point-in-ring test three times; where two lines meet three times; the chunks a box
touches written by hand five times beside `for_chunks`; the arc table read at a share three times.

**Fix.** One owner each, and only where the merge is exact: `hash_u32`; `core/math.h`'s `PI_F`, `TAU_F`, `PI_D`, `TAU_D`, `SQRT2_F`
and `core/units.h`'s `DEG_TO_RAD_D` (every replaced literal was checked to round to the same float or double); `core/grid.h`'s
`CHUNKS_PER_SUPER` and `SUBCHUNKS_PER_CHUNK`; `VORONOI_EDGE_M`; `MOUND_SPREAD` in `mounds.h`; `render/buildings/roof_sizes.h`;
`casing_half` declared in `road_grade.h`; `traffic::in_ds`; `CityClock::set_hour`; `core/plane_geometry.h` with `line_shares`,
`inside_ring` and `monotone_hull`, written once over any point type, each caller keeping its own tolerance and range (the hull of the
survey still rounds and dedups its points first; the open segment of `segments_cross` still drops its ends); `for_chunks`; `arc_at`.

**Not merged here, and why.** Each would move a number, so it is B3 with a predicted diff: two chunk half-diagonals that differ
(`lamp_lights.cpp` 0.7072, `prop_renderer.cpp` 0.7071); two truncated π (`agent_cabin.cpp` 3.14159f, `car_shape.cpp` 1.5707963f);
two value noises that are not one (`mounds.cpp` 2D, `shell.cpp` 3D); the private hashes of `growth.cpp` and `street_names.cpp`;
RDP twice with different metrics; `closest_t` in closed form; the float polygon engine (`plan_area.cpp`) against Clipper2; paints
deduplicated in `Body::add_paint`; the squares' LCG; the three rules for a junction's main road, the two for what kind of place a
building is and the two for the micro window. The seeded generators (`Mulberry32`, the double noises) go with B4's removal of the
parity code. The seven 2D vector types are the next part.

**What changes.** Nothing: every merge is the same arithmetic on the same types.

**Verified** (`city-verify`, `quick --all`, both DLLs): every test passes, the digest equal, every photo byte-equal to a run before C7
(02-rua once off by the known residual's pixels, equal on two reruns); `gates_changed` only lines the files had.

### 6.24 B2, C4 (the zones): the zone codes read from their table (2026-11-08)

**Cause.** `data/catalog/zones.json` gives each zone code its base and density, and `Catalog::zones` carries them; the codes were
typed again in six places (the "six owners" of §3c, and §3h's copies left from C3): the generator's own `enum Zone`
(`road_gen.h`), `massing_core.cpp`'s mapping of codes to building kinds (an unknown code falling to a house by `default:`),
`building_store.cpp` and `growth.cpp` (both calling codes 7 and 8 `ZONE_IND_LOW`/`ZONE_IND_HIGH`, though the table says
`com_low`/`com_high`, and mapping them to the commercial base: right by accident), `models_loader.cpp`'s use names, and
`lot_cache.cpp`'s proposal zone.

**Fix.** `zones.json` says what the generator builds on each zone (`building`, a b40 kind by name, in the schema); the catalogue
resolves the zones the generator lays by what they are (`Catalog::zone_code(base, density)`, `gen_zones`: none, industrial, mixed,
commercial and residential by density), and the loader refuses a table without one. The generator's plan carries those codes
(`CityPlan::zones`, `zone_of(zones, ...)`), and the road rules, the scenic rules, the lot strips and the lot cache read them; the
generator tables take each zone's kind and whether it is a low shop street at boot (`set_zone_kinds`), so the body generator never
sees a code. The store and growth read a zone's base from the catalogue (`BuildingStore` holds the catalogue, not its occupancy alone;
`occupancy(b, base, ...)`); the use names are the `BaseZone` values.

**Why it does not come back.** A zone's code exists once, in its table; what it means is read from the table everywhere.

**What changes.** Nothing: every zone resolves to the code and the kind it had (checked against the table: none 0, industrial 3,
mixed 4, commercial 7/2/8 and residential 5/1/6 by density; house, flats, tower, shop, office, works as before).

**Verified** (`city-verify`, `quick --all`, both DLLs): every test passes, the digest equal, every photo byte-equal to a run before C7;
`gates_changed` only lines the files had.

### 6.25 B2, C4 (the shaders): what the shaders wrote twice, merged where not a pixel moves (2026-11-08)

**Cause.** §3s's copies between shaders, and two lists kept beside their owner: the pane's albedo typed four times
(`building_far.gdshader`, `building_far_window.gdshaderinc`, twice in `building_pane.gdshaderinc`) and the dark pane's once; the far
room's cell hash, lit rule and lamp mix written in both far levels; the lamp's own lot (`fract(lot * 17.31)`) at every caller of
`room_lamp` and the lamp level of `pane_lamp` copied into `signature_look`; the integer hash `pcg` in two includes that could not
share a shader (`building_hash`, `lattice_noise`) and the lattice cell's primes in two; the finish laid on a face twice
(`building_body`, `building_parts_body`); the lantern's pool pasted into `building_wash` beside `lamp_pool`'s; a local tau in
`people_far`; the detail layer's second read in `road_detail` and again in `terrain_layers`; the mip expression twice in
`vegetation_cards` (its leaf branch not indented); the wind's direction normalized three times a pixel in `water`; the sky's horizon
read back from the sky material by `sky_view` though `Daylight` computes it; and the shader lists of `city_world.gd`
(`NATIVE_SHADERS`) and `effect_materials.gd` (`SHADERS`), copies of `materials.json` and of the core's names kept only so the orphan
gate would see them.

**Fix.** `building_lamps.gdshaderinc` owns the glass's albedo (`PANE_ALBEDO`, `PANE_ALBEDO_DARK`), the lamp by the room's lot
(`room_lamp` takes its own lot), `room_level`, the far room (`room_cell`, `far_room_lit`, `far_room_lamp`) and the two globals the
lit rooms read (`rooms_awake`, `building_night_windows`, no longer declared by each includer). `hash.gdshaderinc` (the renamed
`building_hash`, guarded) is the one integer hash, with `spatial_hash` (Teschner's primes), and `lattice_noise` includes it.
`building_common`'s `finish_surface` lays a finish; `building_wash` includes `lamp_pool`; `people_far` uses the language's `TAU`;
`detail_pair.gdshaderinc` reads a close-up map twice for the streets and the grass (`detail_tone`, `detail_bend`);
`vegetation_cards`' `texel_mip`; `water` takes the wind and the drift once. `Daylight.horizon_linear` is the horizon the air takes.
The orphan gate reads what else names a resource by path, the data tables and the core's strings (`gate_code.py`
`OTHER_REFERRERS`, `facts.py` reads a data file's whole text), and the two lists are gone; its fixtures show a shader named only by
a table and only by the core passing, and a name in a C++ comment or a tool's JSON naming nothing.

**Why it does not come back.** Each value and rule has one file; a shader that needs it includes that file, and the hash and detail
includes are guarded so two paths to them are one.

**Not merged here, and why** (each would move pixels: B3 with a predicted diff). The two hand copies of the engine's GGX
(`building_glass_body` light(), `people_look` `ggx_specular`) differ (a roughness floor, the visibility's epsilon, the view's clamp);
`water`'s view depth is the full inverse where `screen_depth`'s `view_at` takes six terms, and `canyon_light`'s normal picks its side
by another rule; `sky_view`'s cloud hash has seeded.h's constants but 64-bit arithmetic, so it is not seeded.h's hash; the sin
hashes (`agents`, `people_look`), `person_hash`, `marks`' and `atmosphere`'s noises; `FAR_ROOM_LEVEL` 0.68 is not the mean of
`room_level` (0.3 + 0.85 / 4 = 0.5125 for two independent uniform lots), and `materials.json`'s far `window_glow` 0.42 and
`signature_look`'s `ROOM_GLOW` 0.62 are worked out from it and from the near glow by hand; `building_room_tone`'s `HUNG_*` means and
`buildings.json`'s far finish tones typed by hand; the near and mid levels' reveal and sill shades; `building_pane`'s frame share
read from a value in metres; the far foot shade; `building_card`'s railing beside the near level's models; the bay picks in C++ and
in the mid level's shader.

**What changes.** Nothing: the same arithmetic in the same order (a uint sum regrouped, which is exact; the constants the same
floats; `TAU_RAD` 6.28318530718 rounds to `TAU`'s float).

**Verified** (`city-verify`, `quick --all`): every test passes with no shader error, the digest equal, the four photos byte-equal to
known states (00-cidade off its golden in the known residual box, equal to an earlier run); the gate self-test passes and fails with
the old gate on the new case; `gates_changed` reports only what the touched files had (`daylight.gd`'s and `sky_view.gd`'s length,
`sky_view.gd`'s hash literals, `daylight.gd`'s cascade side).

### 6.26 B2, C4 (the scripts): one owner each for the scripts' doubled values (2026-11-08)

**Cause.** §3q/§3r/§3s/§3 copies on the script side: the decimal mark chosen by `get_locale().begins_with("pt")` in
`formatters.gd` while the thousands mark comes from a translation (`economy.group`); the inspector's refresh and the edits F8 marks
read from the health monitor's table (`health.json` `refresh_s`, `changes_shown`), a pace and a count filed under another owner's
name; the road-blocks grid's first values merged by `road_options.gd` as a copy of `RoadBlocks.settings()`, and the base road looked
up twice a refresh; `pgo.sh` naming its own Godot and llvm-mingw (not overridable for Godot) and `build.sh` defaulting Godot to
`godot`, beside the machine vars `tools/tasks/suites.json` owns.

**Fix.** `economy.decimal` beside `economy.group` (`format_tenths` replaces the point by it); `hud.json` holds
`timings.inspector_refresh_s` and `changes_shown` (schema), `health.json` keeps what is the monitor's; `road_options.gd` takes
`RoadBlocks.settings()` and hands the base road to `_class_rows`; `python tools/tasks/cli.py var <name>` prints a machine var as
the tasks resolve it (its environment variable first, then the platform's default; no newline, so `$(...)` holds no `\r`), and
`pgo.sh` and `build.sh` take Godot and llvm-mingw from it (tests: the default, the environment first, an unknown var exits 2).

**Why it does not come back.** A locale rule, a panel's pace and a tool's path each have one table; the scripts read it.

**Not here, and why.** The cut-walls choice kept by `Settings` and by `CityState.new_road_flags` (and dispatched twice by
`road_options.gd`) is tool state in the city state: C5, with the state's hash moving. `road_catalog.gd` drawing a profile's picture
from the JSON strips while a variant the core made has none draws the variants' pictures: B3. `android.sh` keeps its own Godot
default while the Android session edits it (log/pendencias.md).

**What changes.** Nothing: Portuguese still shows a comma and English a point (a `pt_*` locale picks the `pt_BR` column, any other
the fallback's), the same pace and count, the same grid, the same Godot 4.7.2 (the console build of the same install).

**Verified** (`city-verify`, `quick --all`): every test passes, the digest equal, the four photos byte-equal to known states;
`tools/tasks` tests 26 OK; `gates_changed` reports only what `inspector_panel.gd` had (its length, two texts).

### 6.27 B2, C4 (the data): one owner for each value the tables held twice (2026-11-08)

**Cause.** §3t's and §3u's equal pairs: a person's stay at work or on an errand under two key pairs in `traffic.json`
(`work_stay_h`/`errand_stay_h` read by `ped_demand.cpp`, `stay_work_h`/`stay_errand_h` by `lot_demand.cpp`); the footway's height in
`traffic.json` `curb_m` (the walkers, `lane_geometry.cpp`) and in `streets.json` `heights.sidewalk` (the road, the furniture and a
tram's flush track); `buildings.json` `lod.fov_deg` copying `camera.json` `far_fov_deg` because render data could not read a game
table; the fireball's law in `effects.json` beside the scar's in `tuning.json`; a knocked-down person's time down under `people` and
under `shot` in `blast.json`; `materials.json` carrying the finishes' maps and scales twice, the glass three times and the vegetation
wind four (`cover_rock` and `cover_shore` whole twins); the road-blocks tool's grid in `settings.json`, the graphics options; the
zones' names twice (`zone.name.0..8` read by the inspector, `zones.json` `name_key` read by nothing and five of its keys untranslated);
the eight primitive tools' one hint under eight keys; and 83 translation keys reached by no code whose text another, live key holds
(`builder.lot.*` beside `lot.*`, `design.finish.*` beside `building.finish.*`, `ui.toolName.*` beside `tool.*`, the
`game.hint.*`/`hint.tool.*` twins, `game.toast.missed`/`hit`/`saveBlocked`, `game.confirm.flattenMap`, `cmd.place.bomb`/
`junctionControl`, `save.file.notMap`, `save.importFailed`, `economy.money` with its dollar sign for reais); and `road_grade_test.gd`
typing the clearances and the bore's cover it judges.

**Fix.** One pair of stays, both readers on it; `street_furniture.json` `sidewalk.height_m` (schema), read by the walkers through the
catalogue and by the road, prop and transit renderers through `RenderContext::catalog`; the boot hands the camera table to the render
(`CityStore.boot`) and `BuildingLook::load` reads `far_fov_deg` from it; `EffectWorld::init` takes the fireball from
`strike.scar_radius_*`; the shot reads `people.knock_*`; `materials.json` `_shared` holds `finishes`, `glass`, `sky_in_glass`, `gust`,
`sway` and `stone`, a row names what it `uses` and `CityWorld.material_params` lays them under its own params (the schema keeps a
shared key out of the rows); `hud.json` `tool_options.road_blocks`; the inspector reads `zones.json` `name_key`
(`DataTables.zone_name_key`), the missing five carry the texts `zone.name.N` had, and the core no longer stores a name it never read;
`hint.builder.prim` and a tool's `hint_key`; the dead twins deleted (each checked: no code, scene or table names it, no prefix
builds it); the grade test reads the tuning.

**Why it does not come back.** Each value is in one table; the schemas are strict, so a shared key written again in a material row,
or a moved key left behind, is refused.

**Not here, and why.** Through the styles, which B4 rebuilds (D2): the 57 street names and the signals copied into both
`street_furniture.json` styles (moved to the base they would name the `eua` and `europa` streets), `models.json`'s module and
storeys beside `tuning.json` `build.*` and its fence and wall beside the barrier kinds, the plates' "Rua" beside `street_names.prefix`,
the flags' colours. Unequal pairs (B3): the train car (18 m simulated, 17.5 m drawn), the zone colours of the drawer and the map,
`cityAvenue` and `boulevard`, the railing's height, a model and its plain twin under one name; the numbers derived by hand
(`daylight.json`, `agents.json`, the far finish tones) and the bore's cover typed in `road_shape.cpp` (0.4 + 0.16) beside
`structure.bore_over_m` 0.56, whose sums round apart. Not one value: `transit.json` `default_line` (a stop no line serves) and the
`transit_bus` token (the bus mode's colour in the HUD) equal a line colour without being one.

**What changes.** Nothing: every reader gets the value it had (0.15 m, 35 degrees, 2.5 + 1.1 x force, 3.2 + 1.8 s, the same
material parameters, checked key by key by the script that moved them), every text shown is the one it was.

**Verified** (`city-verify`, `quick --all`, both DLLs): every test passes, the digest equal, the four photos byte-equal to known
states; `gates_changed` reports nothing the files did not have at HEAD.

### 6.28 B2, C4 (the core, second part): two point types where seven stood (2026-11-08)

**Cause.** R10's seven 2D vector types: `core/math.h` `V2` (float x, y, with its operations), b40's `Point2` (float x, y, bare),
`LotPoint` (`lot_rings.h`), `RoutePoint` (`pole_routes.h`) and `StrokePoint` (`commands.h`), each double x, z and bare, the
snap's private `Vec` (double x, z with its own operators, `place_snap.cpp`) and the block cutter's `std::pair<double, double>`
(`block_lots.cpp`). A point of the plan and a point of the ground were spelled anew by each area, and code moving one into another
copied it field by field.

**Fix.** Two types in `core/math.h`: `V2` for the float plans (every `Point2`) and `XZ`, an aggregate of doubles with the
operations `Vec` had, for the ground (`LotPoint`, `RoutePoint`, `StrokePoint`, `Vec`, the pair, which sorts as a pair did).

**Why it does not come back.** A new area finds the two types in `core`; the plain duplicates are gone, so nothing invites a third.

**What changes.** Nothing: the same fields of the same types in the same order, the same arithmetic; every overload a call could now
reach through `city` was checked (each pair of same-named functions is a declaration and its definition, or lives in one file).

**Verified** (`city-verify`, `quick --all`, both DLLs): every test passes, the digest equal, the four photos byte-equal to known
states; `gates_changed` reports only `commands.h`'s length, shorter than it was.

### 6.29 B2, C12 (first part): the pipeline's owners where nothing moves (2026-11-08)

**Causes and fixes.**
- **The source of truth included the layer that mutates it** (`city_state.h:12` included `sim/commands/edit_journal.h` for
  `EditHistory`, and through it every render file saw the journal's mutations). The history's records (operations, entries,
  `EditHistory`) are the state's data: `sim/state/edit_history.h`; the journal that writes them and runs them backwards and forwards
  stays `sim/commands/edit_journal.h`, included by the eleven commands and the survey that call it. `rules.json` `cpp_layers` lists
  what `sim/state` may include, `sim/commands` not among it, so the dependency cannot come back.
- **The frame's upload flush rested on an engine internal (L12)**: `frame_admission.h` called `get_video_adapter_name()` for its side
  effect; it calls `RenderingServer::force_sync`, the call Godot binds for it.
- **Two protocols for a revisioned field**: `shore_view.gd` built the shore's whole Dictionary every frame to compare a revision;
  `shore_revision` (`ShoreField::view_revision`) is asked each frame and the field only when it moved, as the fog and the canyon.
- **The sky view owned what is not the sky**: it set the map's biome and the water's look from its landscape read and drove the
  contact shade. It tells what it read (`landscape_read`) and CityWorld lays the biome and the water; CityWorld drives the sky's step
  (`step`, after its own frame, the order it had) and the contact shade from the sun it owns.
- **The simulation's data loaded in the render setup, from render files (A5)**: the building generator's tables, the Build > Models
  catalogue and the interiors' recipes go to `data/catalog/` (`buildings.json`, `models.json`, `interiors.json`) and load in
  `load_catalog`; what the render draws them with stays `data/render/building_look.json` (the facade parts, the lot fence, the roof
  plant by style, the far finishes, the levels of detail). The catalogue and the building tables are read aside and taken in whole:
  a refused building table no longer leaves the core with a catalogue and no generator (the refusal test's "no roads to offer"
  found it when the cases moved). Schemas follow the files.
- With these, `city_world.gd` and `data_tables.gd` passed their length: the world's materials are a responsibility of their own
  (`render/world_materials.gd`, `WorldMaterials`), and so are the zones' lookups (`game/data/zone_table.gd`, `ZoneTable`).

**Not here, and why.** Transit built on the main thread (L3) and the buildings' second result protocol (L4) are the pipeline's
redesign; `DetailView` (L5) is either the families' levels chosen by their error (a behaviour change, B3) or deleted; the god classes
(L9, A6, `RenderWorld`) and opening a city in a job are refactors of their own, next. `JOB_SEM_CHAVE` is the gate contradicting
itself (its pattern in `rules.json` allows `submit(JobKey{`, its check forbids it): C16. The jobs' own lock (C6's architecture) is
the measured design §6.10 left.

**What changes.** Nothing: the same tables, read in the same order before any body; the same frame order; the same shore and sky.

**Verified** (`city-verify`, `quick --all`, both DLLs): every test passes, the digest equal, the four photos byte-equal to known
states; `gates_changed` reports only what the files had (`sky_view.gd`'s length and hash literals, `inspector_panel.gd`'s, the
flag colours in `types.h`, the fuzz test's length).

### 6.30 B2, C12 (opening a city): the file decoded aside, the city taken in with no job inside it (2026-11-08)

**Cause.** Opening a city froze the frame (§3n `city_core_persist.cpp:109-136`, §3q `city_saves.gd:81-92`): `load_city` read the
file, decoded it (`decode_city`: the land from its seed, every road graded, every lot cell) and took it in, all on the main thread.

**Fix.** `load_city` reads the file on the main thread (the engine's compressed `FileAccess`) and decodes it on a thread of its own
into the city aside (`LoadJob`); `load_poll` answers `pending` until it is read, then takes it in as before. `CitySaves.load_game`,
`load_last` and `open_file` wait frames for it (`_opened`), and the game and the tests `await` them.

**What the change found.** With the frames going on during the decode, two tests died off the main thread (an access violation, a
division by zero, four rounds in five): `take_city` swaps the whole `CityData`, the catalogue and the building tables with it, and
frees the old city, while render jobs that copied their inputs under a short lock went on reading those tables outside it (the
canyon's `record_mass` on `s.buildings.designs`, the ruins' pointer to them, the shore's and the fog's reads of the catalogue). The
synchronous opening had the same race, hidden by timing: no frame submitted jobs while it decoded. `JobPool::pause` hands out no
work and waits for the running jobs; the opening pauses the pool around the swap, and the jobs that start after it block on the map
lock or read whole tables. Six rounds of the opening tests after it: none died.

**What changes.** The frame no longer stops while a city decodes; the city taken in is the same (the digest equal).

**Verified** (`city-verify`, `quick --all`, both DLLs, and the six save-opening tests six times): every test passes, the digest
equal, the photos byte-equal to known states; `gates_changed` reports only what the files had.

### 6.31 B2, C16 (first part): one owner for each of the tools' mechanisms (2026-11-08)

**Causes and fixes.**
- **A build freed the DLL it did not write** (found by the C12 runs): `free_dll.sh` moved every DLL aside, so the release build
  took the debug DLL away while the tests keyed their inputs on it; it frees the DLLs it is named, `build.sh`'s one-DLL target
  names its own (`f8a6c765`).
- **The orchestrator re-read git after every task and sorted in quadratic steps** (`pool.py`): the snapshot keeps the index for a
  run (a task writes files, it never stages them) and reads git status and the changed files' hashes again (`Snapshot.refresh`);
  a task's users and the ready order are worked out in one pass each.
- **Three git wrappers, one without a deadline**: `tasks.inputs.git` is the tools' one (deadline, no window, no optional locks)
  and the gates' scope asks it.
- **Two JSON Schema checkers** (`gate_data.py`, `tasks/schema.py`): `tasks/schema.py` holds both dialects (the suites' standard
  keywords, the data tables' `strict`, `additional`, `pattern_prefix`, `unique_key`, and `pattern`, `maxItems`, `maximum`), and the
  data gate asks it. The old checker never read `additionalProperties`, so nothing under a style, a builder tool or the road options
  was checked: one colour, three tool keys and three road options the schemas did not name are named now.
- **A generated table out of step with its generator** (T3): `make_furniture.py` read a `sizes` the interiors' table no longer
  had at its top; it reads them under the style the game plays (`start.json` `default_style`), the table is its output again (two
  palette rows had swapped places by hand), and `--check` is the quick suite's `furniture_table` task.
- **The line limits drove splits of data tables** (T5): a table is one owner whatever its length, and the `.json` and `.csv` caps
  could only split by size. They are gone from `max_lines`; the profiles are one `road_profiles.json` again (in the order the core
  numbers them, the fragments' merge in `DataTables` gone), `land_elements` is `land_panels`' block again, and the text is one
  `strings.csv`.
- **`JOB_SEM_CHAVE` contradicted itself and failed twenty correct lines**: its pattern allowed `submit(JobKey{`, its check forbade any
  `JobKey{` outside `job_pool.h`. With `JobLane` a key collides only when two producers share a lane, so `JOB_LANE_SHARED` (a
  whole-repo rule, its passing and failing fixtures) reports a lane named by more than one file, and `JOB_SEM_CHAVE` reports a job
  given a braced key with no type. The pool's probe keys its own pool on lanes of its own (`PROBE_*`).

**Not here, and why.** The four C++ readers of the tools (the gates' lexers, `code_parse.py`, the analyzer's libclang and
tree-sitter adapters) and the analyzer's own git and compiler calls are the analysis tools' rewrite, next. `bleed_alpha.py` and
`calm_stone.py` edited 4.0 assets in place; their sources go with those assets in B4. The code files' line limits stay, read as
"split by responsibility".

**What changes.** Nothing in the game: the same profiles in the same order, the same panels, the same text, the same furniture.

### 6.32 B2, C16 (the analysis tools): one C++ lexer, deadlines, a sampler that knows the main thread (2026-11-08)

**Causes and fixes.**
- **The analysers' C++ reader counted braces in strings and comments** (`code_parse.py`: line regexes and a brace count per line,
  so a `{` in a string or a comment shifted every later function's span): heads and braces are read on the code the gates'
  lexer leaves (`lex_c.py`, comments, strings and characters blanked), the bodies are the source as written, and the lexer's source
  keys the parses with the parser's. A test function whose string holds `}{` and whose comment holds `}` keeps its five lines.
- **The analyser's processes had no deadline** (`arch/repo.py` git, `arch/adapters/cpp_build.py` ninja and the compiler): each call
  has one, and `causes` and `arch_scan` are tasks of the orchestrator (CLAUDE.md's commands run them through it), so the analyser's
  long-lived `git cat-file` streams end with the task's deadline too.
- **The sampler took the first thread Toolhelp32 listed for the main one** (no documented order), opened handles it never
  closed, left the handle types at `int` (a 64-bit handle truncated) and stopped on any module whose debug file was elsewhere:
  the main thread is the oldest (`GetThreadTimes`), the handles and dbghelp's state are given back (`Sampler.close`), the handle
  types are declared, and a module whose PDB is not on the machine is reported as without debug information. Its Windows access is
  a responsibility of its own (`win_process.py`), which also keeps the sampler under its length.

**Not here, and why.** The analyser's libclang and tree-sitter adapters read C++ for a call graph, which a token lexer cannot
give: they stay the analyser's, the other readers use the gates' lexer.

**Verified.** The parse of a function with braces in a string and a comment; `causes` as a task (it passes); the sampler on a
short Python process (1 392 samples, the main thread named, the modules without symbols said); the `tools/tasks` tests and the
gates of the changed files.

### 6.33 B2, C14 (first part): the tools' steps, the interface's choices and the numbers in text, from their tables (2026-11-08)

**Causes and fixes.**
- **The player's tools kept their steps as constants** (`brush_tool.gd`, `handle_drag.gd`, `lots_tool.gd`, `roundabout_tool.gd`,
  `terrain_tool.gd`, `zone_tool.gd`, `demolish_tool.gd`): how far apart a brush lays its dabs, how many steps a handle turns in,
  what a lot's corner catches, how a roundabout scales, the zone a stroke paints with none chosen. They are interaction choices of
  this game, so they are one table, `data/game/tools.json` (its schema, its `_doc` giving each one's reason), read by `DataTables`
  and taken by each tool as static values. The handle's step is `TAU / turn_steps` (24, the old `PI / 12`); the default zone is a
  `zones.json` id resolved by `ZoneTable.code`, not the bare `1`.
- **Interface choices typed in the scripts**: the zone densities (`drawers.json` `zone_densities`), the builder's choices
  (`builder.json` `choices`, which `building_tool_test` reads too), the share of a help row the key takes (`tokens.json`
  `help_key_share`), the road flash's fade times (`hud.json` `timings`), and the languages the start screen offers
  (`start.json` `languages`). A language's own name is text, so it is `language.self` in `strings.csv` (each language
  names itself), and `StartScreen.languages()` is the one list the start screen and the menu rows read.
- **Numbers written into translations beside the value that decides them** (`strings.csv`): the health window's 30 s, the
  junction step's 0.5 m and the most clouds (24) were typed in both languages, so a change of `health.json`, of the panel's step
  or of `nature.json` left the text wrong. They are `{seconds}`, `{m}` and `{n}`, filled by `health_panel.gd`, `road_panel.gd`
  and `cloud_tool.gd` from those owners. `game.toast.maxClouds` said the same as `hint.cloud.full` and nothing read it.
- `weather_view.gd` divided by `60.0` where `Units.S_PER_MIN` is the owner; `camera_gestures.gd:128` held four statements on
  one line.

**Not here, and why.** `road_panel.gd`'s limits (the curve's least and most, the height step) are rules the core should give
(B3, the panel asks the core); `WATER_EXTENT_M` comes from the map (B3); the plain quarter turn of `building_tool.gd`, the road
section's and the swatch's pixel sizes, the shader look values and the other numbers typed in text are the next parts of C14.

**What changes.** Nothing: the same steps, densities, choices, times and texts (verified by `quick --all`: 45 of 47 tasks pass;
`gates_changed` reports only lines the baseline holds; `photos_check` differs by 0.001 % on two photos whose bytes are states
already captured before this batch, the capture's known variation).

### 6.34 B2, C14 (second part): the road views' sizes, the turn and height steps, the texts' measures (2026-11-08)

**Causes and fixes.**
- **Pixel sizes in interface code** (CLAUDE.md: sizes only through `data/ui/tokens.json`): the profile editor's section kept its
  label threshold (34 px), its way mark (7 px) and its divider (1 px) as constants, and the catalogue card drew its centre lines
  and rails at offsets typed in the calls (-2, +1, -1; 1 and 2 px). They are tokens (`rp_label_least`, `rp_way_mark`,
  `rp_divider`; `rc_line`, `rc_solid`, `rc_double_gap`), and the offsets follow from them: a double line is two lines
  `rc_double_gap` apart around the edge, a solid one `rc_solid` thick across it.
- **The plain quarter turn of a model in hand was `PI / 2` while the Shift turn was data**: `tools.json` `building.turn_steps`
  (4), the turn `TAU / turn_steps`, as the handle's.
- **The road's height step was the number 1 in the calls** (`road_tool.gd` `step_height(1)`, the option buttons and the keys),
  and its texts typed "1 m": the step is `tools.json` `road.height_step_m`, `step_height` takes a direction and lands on whole
  steps, and `roadKeys.height*` take `{step_m}` from it.
- **Measures typed in texts whose owner is a table**: the footway's clear width (`roadFurniture.tip`, "1,20 m", from
  `street_furniture.json` `sidewalk.free_width_m`, written with two decimals by the new `Formatters.format_hundredths`) and the
  snap grid (`v2.snap.grid`, from tuning `road_grid_m`, filled by the road options' one table of fills).

**Not here, and why.** `v2.snap.angles` lists "90°, 45°, 15°" where the rule is multiples of `road_snap_step_deg`: its text has to
say the rule, a change the player reads (B3). `confirm.newWorld` and `confirm.newLargeWorld` give wrong sizes (B3, recorded).
`inspector.impossibleBody`, `road.bend.sharp`, `road.sub.*`, `ui.catalog.tooMany`, `ui.problem.maxLanes`, `ui.problem.wide` and
`ui.money.unlimited` hold numbers too, but no code reads them: they go with the unreached keys (C13), not filled.

**What changes.** Nothing: the same pixels, turns, steps and texts (`quick`: 45 of 46 pass; `photos_check` differs by 0.001 % on
one photo whose bytes are a state captured before this batch).

### 6.35 B2, C16 (the gates' rules on numbers): a copy by name, place and value; a constant says its source (2026-11-08)

**Causes and fixes.**
- **`SEGUNDO_DONO` matched names alone** (`gate_defs.py` `check_data_copies`): two words shared by a constant and any data key
  anywhere was a hit, so `SEED_SCALE` was a copy of `cover.json` `rocks.seed` and `KEEP` of a tree band's `keep`: 289 lines on the
  tree, almost all coincidences, which nobody can act on and everybody learns to skip (T2). Measured on the tree: names alone 289,
  names and values 79 (still `DOOR_W` 0.9 against a bus's door), exact names 67 (`W`, `D`, `PAD`). A copy is about the same
  thing, so the rule now asks for the place too: a word of the code file's path must be a word of the key's table or path (the
  repository's places, `data_copy_places` in `rules.json`, and a style's id are not words of a domain), and then either the
  constant bears the key's whole name of two words or more (a drifted copy keeps the name), or shares two words of it and holds
  one of its values (not a trivial one). `VALOR_DUPLICADO` (a distinctive value typed again) asks for the place too: its 13
  shader hits were the coordinates of a clothes airer and of a scenic lake. What is left are eight copies one can act on:
  `ruin_pieces.cpp` `DUST_*` against `effects.json` `down.dust_*`, `civic_services.cpp` `STALL_W`/`STALL_D` against
  `buildings.json` `lots` (the second one drifted, 5 m against 5.5 m), `road_gen_graph.cpp` `PARALLEL_COS` against tuning
  `road.parallel_cos`, and `terrain_gen.cpp` `RIVER_BED_FLOOR`/`ROUGH_REACH_MIN` against tuning `terrain` (the same profile
  read by the brush from data and by the generator from code). The hand list of 22 exceptions (`data_copy_allow`) existed only
  for the old matching and is gone, with the unused `data_copy_min_abs`.
- **The magic-number rule was satisfied by naming** (`constexpr float K = 0.37f;` passed with no source): a constant holding a
  value that is not trivial says in a comment what it is (a definition, a norm, a derivation), on its own line or in the comment
  over the block of constants it belongs to; a value one tunes goes to data (`CONSTANT_WITHOUT_SOURCE`). Enumerator lists name
  codes and are left out. CLAUDE.md says the rule.
- **The core and the shaders were outside the rule** (`rules.json` exempted `native/src/core/` whole and `catalog_loader.cpp`;
  `.gdshader` and `.gdshaderinc` were not in its extensions): both are in. Exempt stay only the runtime translated from the
  source game (`core/js_*`) and its stream (`core/seeded.h`), which B4 replaces. The core's numbers are named with their
  sources: the hash finaliser is Wellons' lowbias32, the generators Marsaglia's xorshift (the 64-bit one of `Rng` and the 32-bit
  one `wear_field.cpp` had written again, now `xorshift32` in `core/random.h`), the golden-ratio gamma (three copies:
  `job_pool.h`, `pane_rooms.cpp`, `kept_by_revision.h`), Teschner's spatial primes (two copies, `mounds.cpp`), the cell key's
  halves, the colour parser's digits, the geometry's tolerances; `city_core_query.cpp` read the colour base from its own copy.
  The DLLs built from these sources are byte for byte the ones of the commit before.

**What the gates say now.** 2,172 problems on the whole tree (it was 401): `NUMERO_MAGICO` 1,392 (1,356 of them in the shaders,
now under the rule), `CONSTANT_WITHOUT_SOURCE` 719, `SEGUNDO_DONO` 8, and the rest as before (`TEXTO_VIA_TR` 23, `TAMANHO_ARQUIVO`
17, `DONO_UNICO` 9, `TAMANHO_FUNCAO` 3, `ESPELHO` 1). The two new counts are the values of C14 the old rules hid: they are the rest
of C14, domain by domain.

### 6.36 B2, C14 (the tuning's types and the generated land): a count is an int, the land is data (2026-11-08)

**Causes and fixes.**
- **Every tunable was a float** (`tuning_fields.h` `CITY_TUNING_DECLARE`): counts were read back through `int(...)` at every
  use (`build_max_volumes` in eight places), a fraction in the table passed silently, and a generator working in double could not
  share a value with the brush. The X-macro gives each field its type: `X` a measure or share (float), `XI` a count (int, which
  the loader refuses as a fraction: 24 fields), `XD` a value read in double precision. The casts the counts no longer need are
  gone; the tuning gate reads all three.
- **The generated land kept its whole shape in code** (`terrain_gen.cpp`, about forty `constexpr double`: hills, valley, canal,
  lakes, ranges, the flood), and two of its values were second owners of tuning's `terrain` group (`RIVER_BED_FLOOR` and
  `ROUGH_REACH_MIN`, the bed's profile the brush read from data and the generator from code; `SEGUNDO_DONO` named both). The
  shape is tuning's `land` group (doubles, the counts as ints, a `_doc` saying each one's part), the generator keeps the tuning
  it was made with and derives the canal's reach and cut from it, the bed reads `terrain` like the brush (which still works in
  float, as before), and the height limits are tuning's (`MIN_Y`/`MAX_Y` were copies). What stays in code is named with its
  source: the keys' packing, the lattice strides and salts, the neighbour tables, the end guard. Godot's JSON reader gives the
  correctly rounded double for every value moved (checked bit by bit), and the build contracts no floating expression
  (`-ffp-contract=off`), so the run-time products equal the folded ones.

**Not here, and why.** `civic_services.cpp` `STALL_D` (5 m against `lots.stall_d` 5.5 m) is a second owner that drifted:
reading the table changes the market's parking (B3, C4 unequal pairs). `road_gen_graph.cpp` `PARALLEL_COS` (0.93) is the city
generator's own hugging test, not the road tool's overlap rule (0.9), and `ruin_pieces.cpp` `DUST_*` are a falling piece's dust,
not a person going down: both go to data with their domains (the citygen and effects passes).

**What changes.** Nothing: the digest of the generated maps, the hash tests and the numbers are the same (`quick`: 44 of 46; the
photo that differed by 0.004 % was a state of the capture's variation: taken again with the same DLL it is one seen before).

### 6.37 B2, C14 (the building generator, bodies): every value of a generator file in buildings.json "shapes" (2026-11-08)

**Cause and fix.** Every massing file opened with a block of `constexpr float` (storey heights, shares, counts: §3b), passing
the magic-number rule by being named; the review's fix was to read them from the buildings table. A generator file's values are
now a block of `buildings.json` `styles.roadcraft.shapes` (`civic`, `services`, `facade`, `massing`, `house`, `tall`, `works`,
`paint`: 318 values), typed by an X-macro list per block in `sim/buildings/b40/shapes.h` (float, count, double, lists), read
by `buildings_loader.cpp` (a missing key refuses the catalogue), checked by a strict schema, each block with a `_doc` that
carries the sources the code's comments held (the COE and NBR heights, the CTBUH comparison). A function reaches its block
through the `Data` it is given (`roof_shape`, `roof_of`, the house's and the tall bodies' helpers now take it; `nudge` takes
the paint block). The values were moved by a tool that rewrites the declarations and the uses and checks, value by value, that
the float the loader makes from the JSON double is the float the literal made.

Not moved, and named with their sources: the facade grammar's bay rules (`DEFAULT_BAY`, `BEST_WITHIN`), which every saved body
and the commands depend on; the hash constants and the HSL hexcone; tolerances. Three copies made one: the era count was in
`buildings_loader.cpp`, `massing_core.cpp` and the size of `Data::era` (now `ERAS` in `types.h`); the palette count `PALETTES`
(7) in three massing files was the lot walls' last index (now `Data::last_palette()`, eight walls in the table).

**What changes.** Nothing: the generated maps' digest, the hashes and the numbers are the same (`quick`: 44 of 46; the photos
are states seen before).

### 6.38 B2, C14 (the building generator, lots): the lot zones' values, and one owner for the parts they share (2026-11-08)

**Cause and fix.** The same as §6.37 for the lot layout (`lot_back`, `lot_boundary`, `lot_cars`, `lot_front`, `lot_ground`,
`lot_plan`, `lot_space`, `lot_yard`, `open_lots`): their values are blocks of `buildings.json` `shapes` (`back`, `boundary`,
`cars`, `front`, `ground`, `plan`, `space`, `yard`, `open`), with the comments' meaning in each `_doc`. The lot plan takes the
`Data` (`plan_lot`), the boundary's helpers, the open lots' layout and the tree test (`open_lot_tree_ok`, read by the tree
renderer through `s.buildings.designs`) take their blocks.

Several files laid the same part with the same number, written once a file: a ground cover's thickness (`COVER_H`,
`SURFACE_H` in civic, services and open lots), the sports court (school and open lot: 15 x 28 m, 0.12 m lines), a bench's
seat, a drain, a pallet, a lamp post, a yard swing, and a car way's margin (`WAY_SIDE`, in `lot_ground.cpp` and again in
`lot_cars.cpp` with a comment pointing at the first). They are one block, `shapes.parts`, every file reading it. The market's
bays read `lots.stall_w` (the second owner `SEGUNDO_DONO` named; same value); its `stall_d` drifted (5 m against 5.5 m) and
waits for B3. Parts that only look alike stay their files' own: the bins, benches' widths, planters and lamps that differ.

The headers' last constants say what they are (the packed colour's channels, the sRGB transfer, the bay key's bytes, the
flag's default colours); the skin's lightness tint is the paint block's.

**What changes.** Nothing: 681 values in 18 blocks, the generated maps' digest, the hashes and the numbers the same (`quick`:
45 of 46; the photos are states seen before).

### 6.39 B2, C14 (the interiors): flights, rooms and homes in buildings.json shapes (2026-11-08)

**Cause and fix.** About 150 named constants in `interior_homes.cpp` and as many in `interior_rooms.cpp` placed every piece of
furniture (§3c); `interior.cpp` held the flights' rise and tread and the rooms' counts, `plan.cpp` the floor plan's margins.
They are four more blocks of `buildings.json` `shapes` (`interior`, `homes`, `rooms`, `floor_plan`: 260 values), read through
the `Plan`, `RoomCell` and `Home` a function is given (`flat_plan`, `spaces_of`, `floor_finish` and `level_flights` take the
`Data`; the render's three callers pass theirs). With 22 blocks the declarations are split by part of the generator
(`shapes_bodies.h`, `shapes_lots.h`, `shapes_interior.h`, the field macros in `shape_fields.h`, the list in `shapes.h`).

One owner each: a doorway's half width was `HALF_WAY` in two files and `HALF_DOOR` in a third (now `HALF_DOOR` in
`plan.h`), and the lot access search wrote the garage's wall again (`GARAGE_WALL`, now `shapes.cars.garage_wall`, the
`SEGUNDO_DONO` the gate named). The render's lift door share was `DOOR_SHARE`, like the flats' door share it is not: it is
`LIFT_DOOR_SHARE`. Kept in code with their sources: the tolerances, the pick hash, the cooking line's count (its array's size).

**What changes.** Nothing: the digest, the hashes, `interior_test` and the numbers the same (`quick`: 44 of 46; the photos are
states seen before).

### 6.40 B2, C14 (the building render, first part): facades, lots and roofs drawn from building_look.json shapes (2026-11-08)

**Cause and fix.** The render files that draw a building's pieces held their sizes, shares and shades as constants (§3l: about
90 in `facade_parts.cpp`, the lot props, the roof plant's 70, the terrace, the gear). Thirteen files now read blocks of
`building_look.json` `styles.roadcraft.shapes` (408 values: `facade_parts`, `facade_emit`, `part_models`, `placed`, `lot_emit`,
`lot_props`, `lot_parts`, `roof_emit`, `roof_plant`, `roof_terrace`, `roof_gear`, `interior_emit`, `far_mass`), typed by the
X-macro lists of `render/buildings/look_shapes*.h` (by what they draw: facades, lots, roofs, levels) and read by
`building_look.cpp` into `BuildingLook::shapes`. Each emitter reaches its block through the placed building it draws
(`pl.look`): the emitter structs carry it as their last member (`Ctx`, `SideEmit`, `LotEmit`, `PropEmit`, `RoofEmit`,
`PlantEmit`), the helpers that had no `Placed` take their block (`band`, `pilaster`, `relief_caps`, `chance`, the part models).
Reading a list of numbers was written in two loaders: it is `DataReader::floats` and `counts`.

The bay lines of a parking element were spaced by a `CAR_BAY_M` of 2.5 m: the lot rules' stall width (`lots.stall_w`, the same
value), which they now read. Kept in code with their sources: the hash multipliers and stream keys, the lantern's glass share
(measured on the source model), the entrance flight's most steps. The material colours a few files name (brass, gas cylinders,
shutters, clocks) and the roof plant's catalogue array are commented as the source game's; their move to a render palette is
the next part, with the files that remain (`facade_bay`, `facade_props`, `shell`, `ruin_fracture`, `roof_pitched`, the renderer).

**What changes.** Nothing: every photo is a state captured before (`quick`: 45 of 46), the hashes and numbers the same.

### 6.41 B2, C14 (the building render, second part): windows, balcony props, panes' rooms and pitched roofs (2026-11-08)

**Cause and fix.** The rest of §3l's look numbers in the building render: the window grammar of `facade_bay.cpp` (the opening's
least sizes, the pane picks, the rolled shutters, the rain streaks), the balcony and window props of `facade_props.cpp`, the
rooms seen behind a pane (`pane_rooms.cpp`), the pitched roof (`roof_pitched.cpp`) and the body's tone (`building_bodies.cpp`):
five more blocks of `building_look.json` `shapes` (511 values in 18 blocks now). The opening reads the placed building
(`opening_of(pl, c)`, its six callers), the window helpers and the wall run take their block, `Furnish` and `Pitch` carry it.
`facade_bay.cpp` drew the frames with `FRAME_D` and `FRAME_THIN` of its own, the facade parts' frames (`SEGUNDO_DONO`): it reads
`shapes.facade_parts`. The structural numbers left in the render files say what they are (buffer layouts, the signature files'
format, hash multipliers, tolerances, tessellations); the spatial primes of the ruin cells are `core/random.h`'s; an opaque white
instance colour and a channel mask are named once.

Found on the way: the body's place read `pl.look` before it set it; the binding reads the look it is handed.

**Not here, and why.** The shell's baked tone and grime, the material colours some emitters name, the roof plant's catalogue
arrays and the foliage leaf's shape: recorded in `log/pendencias.md` (their table is a render palette and the plant's rows).

**What changes.** Nothing: every photo is a state seen before (`quick`: 45 of 46), hashes and numbers the same.

### 6.42 B2, C14 (the effects): a blow's events, the ruins and the puffs from effects.json shapes (2026-11-08)

**Cause and fix.** The effects files held their sizes, counts and speeds as constants (§3i: the blast's sparks and dust, the
bodies' parts measured on NASA-STD-3000, the things thrown, the heap and the fall of a building, the mounds, the puffs): ten
blocks of `effects.json` `shapes` (220 values), declared by what they play (`render/effects/effect_shapes*.h`) and read into
`EffectParams::shapes`. The modules already kept a copy of the parameters (`params_`, `law_`); `Puffs` now keeps one too, the
mounds' hill takes its block (and `Mounds::hill_at`, read by the fall), the pile reads under the ground from the height it is
given, and `EffectWorld::heap_of` stopped being static to read its block. What stays in code says what it is: the blast's soil
codes and the buffer's rows (they were moved by the tool and put back, an event's code and a layout are not values one tunes),
the parts' count of a body (its array's size), the hashes, the random streams' seeds, the pieces' tints (sRGB, the source game's;
with the building render's colours in `log/pendencias.md`).

Reading a table's shapes was about to be written a third time: `render/shape_reader.h` is the one reader the buildings'
loader, the look and the effects use.

The `SEGUNDO_DONO` the gate named in `ruin_pieces.cpp` (`DUST_*` against `down.dust_*`) was two effects with one word: a falling
piece's dust and a person going down's. They are the `ruin_pieces` block and `down`, each documented.

**What changes.** Nothing: `strike_test`, `shoot_test` and `people_test` pass, every photo is a state seen before (`quick`:
45 of 46).

### 6.43 B2, C14 (the commands): their values in tuning and in buildings.json, their definitions sourced (2026-11-08)

**Cause and fix.** The commands held their tunables beside their definitions with nothing to tell them apart (§3d: the survey's
60 numbers, the body editor's sizes, the roundabout's samples, the shot's rider, the blow's splinters). The values one tunes
went where their owner reads: the body editor's (`body_*.cpp`) to tuning's `build` group, the lot editor's to a new `lot`
group, the roundabout's to `roundabout`, the shot's to a new `shoot`, the blow's to `strike`; the survey of a 3D model
(`body_survey.cpp`, `survey_picture.cpp`) to a `survey` block of `buildings.json` `shapes` (it reads the generator's tables), and
the furniture reach of the floor editor to the `interior` block (it is handed the generator's tables, not the tuning). The
definitions stay and say what they are: the streams' salts are the words they name in ASCII, the tolerances and sentinels,
the bay key's and the colours' layouts, the luma of ITU-R BT.601; three hand-written quarter turns and the degree are the
core's (`PI_F / 2`, `DEG_TO_RAD`, the same floats); the survey's flag white and red are the default flag's.

The mover moved a few definitions with the tunables (a polygon's least corners, the frame's two end bays, a tolerance, a
search's sentinel, a direction's guard, a hash scale): each was put back with its reason, and the mover now refuses sentinels
and tolerances unless told. `survey_body` passed its function limit by the binding: the plinth's reading is a function of its
own (`plinth_of`).

**Not here, and why.** `body_survey.cpp`, `journal_ops.cpp`, `edit_history.cpp` and `commands.h` are over the file limit as the
baseline found them: their split by responsibility goes with the large files (C12).

**What changes.** Nothing: the hashes, the numbers and every photo the same (`quick`: 44 of 46).

### 6.44 B2, C14 (the traffic): the near and far models' numbers in traffic.json, the definitions sourced (2026-11-08)

**Cause and fix.** The traffic files held what one tunes beside what is defined, with nothing to tell them apart: a car's tries
at a spawn place, its free-speed spread and start, the arms that make a junction signalled, a forced lane change's gaps, the far
model's headways, retries and handovers, a walker's tries and scale, the speeds under which a car waits or is stuck, the near
window's pads and pace. The values one tunes went to traffic.json `params`, the one table of the traffic's numbers
(`CITY_TRAFFIC_FIELDS`, `api/traffic_loader.cpp` refuses a missing key), named by the file that reads them (`demand_*`,
`junction_*`, `lc_force_*`, `meso_*`, `ped_spawn_*`, `speed_drift_rate`, `vehicle_*`, `window_*`), each described in the table's
`_doc` with the reason the code's comment gave. The definitions stay and say what they are: the tolerances (a stale heap entry,
a leg's end, a straight step, Reeds and Shepp's rounding), the guards before a division (a step of no time, a leg of no length,
the IDM's desired speed and gap), the IDM's exponent (Treiber, Hennecke and Helbing 2000: 4, squared twice in the code), the
hash streams and the golden-ratio multiplier, the 24 bits a float holds, the walker cells (a power of two, so their index
divides exactly), the largest Reeds-Shepp word, the planner's grains and its memory's blocks and chunks, √2 and √½.

The mover bound the params where `p` already named a pose or a profile (`mid_of`, `in_window`, `check_window`,
`grow_ped_window`) and missed a loop's own binding (`pick_seg`): each was bound by hand where the function reads it. Two
bindings left unused by the last batches (`RuinSim::write`, `park` of the open lots) are gone: the build is again free of warnings.

**What changes.** Nothing: the hashes, the numbers and every photo the same (`quick`: 45 of 46, the gate's two baseline counts
in `traffic_params.h` sourced since).

### 6.45 The DLLs' bytes changed with the link and the hour: the functions' order from native/pgo, mimalloc at the epoch (2026-11-08)

**Symptom.** A change that expands to the same code (one owner for the shapes' field macros, §6.46) gave a debug DLL byte for byte
the same and a release DLL with 834 thousand bytes changed. Rebuilt from HEAD's own sources, the release DLL differed from the
committed one; linked twice from the same objects, it differed from itself (746 thousand bytes), single-threaded too. CLAUDE.md's
"the same sources give the same bytes" held for the debug DLL only: every committed release DLL was one of many.

**Cause.** The two links' maps lay the same sections out in another order, every difference among the sections lld's call-graph
sort places (`.llvm.call-graph-profile`, written by the profile: on by default in lld COFF). The sort itself is ordered (a `MapVector`
of edges, a stable sort by density, `lld/COFF/CallGraphSort.cpp`), but its result is a `DenseMap` keyed by the sections' addresses,
walked to store each section's priority under its symbol's name (`lld/COFF/Writer.cpp`, `sortSections`: `for (auto it : order)`,
`config.order[sym->getName()] = it.second`). Sections that share a name take whichever priority the walk gives last, and the walk
follows the heap's addresses: this DLL has 48 such names (a `__cxx_global_var_init` per file, 227 of them; godot-cpp's static
`make_property_info` six times), and every section after them moves. With the sort off, two links differ in one byte, the output's
own name in the export table.

**Fix.** The order is an input, as the profile is: `tools/build/pgo.sh` builds the release once more with the new profile and the
sort on (`CITY_PROFILE=order`), which prints the order lld chose (`/print-symbol-order`) to `native/pgo/citycore.order`; the release
link reads it (`/order`, the sort off, `native/CMakeLists.txt`) and relinks when it changes (`LINK_DEPENDS`). The profiled
functions keep the sort's sequence; the rest follow in input order, the same names in one place. The order keeps only the names
every folder gives alike: a local function ThinLTO promotes is named after its module's hash (`mi_free_generic_mt.llvm.<n>`), and
the module holds its source's absolute path, so the 73 such names of a build in one folder are other names in another and their
functions would move there. Left out, they follow in input order: 2,202 names stay. A name the code no longer has is an order out
of date, as the profile's counts are: LNK4037 is left out as `-Wno-profile-instr-out-of-date` is. The order committed now is the
current profile's.

**Why it does not come back.** No priority comes from walking a map of addresses, and no name in the order depends on the folder:
lld reads the file's names in order. The same objects linked twice give the same bytes; two copies of the repository in two
folders, each built from scratch, give the same bytes.

**The debug DLL and the hour.** Built from scratch, the debug DLL differed from the committed one in seven bytes: mimalloc's banner
prints the date and hour it was compiled (`src/options.c`: `__DATE__`, `__TIME__`), so every compile of that file changed the DLL. Its
sources compile with the clock at the epoch (`SOURCE_DATE_EPOCH=0` through the target's compiler launcher, read by Clang since
D135045 and by GCC); mimalloc is the only source that reads the date. The release DLL had none of it (the banner is dropped at the
link) and does not change; the debug DLL carries "Jan  1 1970" and is the same after its objects are compiled again.

**Process.** The verifying checkout (`city-verify`) had been configured to take the fetched libraries' sources from the main
checkout's build folder (`FETCHCONTENT_SOURCE_DIR_*`): its objects of mimalloc, Clipper2 and meshoptimizer were compiled from
another path than a clone compiles them from, and its release DLL was not the one a clone builds. Its build folders were made
again from scratch, fetching their own.

**What changes.** The release DLL's layout once (the colliding initializers, the promoted functions and the unprofiled ones in input
order) and the debug DLL's banner date; everything the game does, nothing. Upstream: `log/pendencias.md`.

### 6.46 B2, C16 (one owner): the shapes' field macros written once (2026-11-08)

**Cause and fix.** Each family of shapes (the building generator's, the building look's, the effects') had its own header of the
same five macros, a float, a count, a double and the two lists, under its own prefix (`b40/shape_fields.h`,
`look_shape_fields.h`, `effect_shape_fields.h`), and every block's struct spelled the five out. The city generator's shapes would
have been the fourth copy. One header owns them (`core/shape_fields.h`, the core: the simulation and the render both declare
shapes) with `SHAPE_DECLARE(LIST)`, which makes a block's list its struct's members; the three copies are gone and every block
reads `SHAPE_DECLARE(<its list>)`. `render/shape_reader.h` reads them as before.

**What changes.** Nothing: both DLLs are byte for byte the ones built before the change.

### 6.47 B2, C14 (the city generator): every value of a sim/citygen file in growth.json shapes (2026-11-08)

**Cause and fix.** The generator's files held the 4.0's numbers beside their definitions with nothing to tell them apart: the grid's
blocks, avenues and grades, the graph's buckets, merges and caps, the tidy pass's angles and gaps, the lots' rasters, strips and
sizes, a lot's fit to the land and the paving, the squares' lawn and plaza. Each file's values went to a block of growth.json
`shapes` (the generator's table: its plan and road roles are there), read into `GrowthDef::shapes` by `api/catalog_loader.cpp`
through the one reader of shapes, each block described with the reasons the code's comments gave. The blocks reach the functions
as the earlier families do: the classes hold their block (`PlanRules`, `GraphBuilder`, `Tidy`, `Blocks`, `LotStrips`, the scenic
`Layout`), the entry points take `GenShapes` (`generate_roads`, `build_graph`, `tidy_graph`, `dry_crossings`, `road_gen::land`,
`build_lots`, the lot land and paving functions the lot commands and the core's lot tools call).

Two values had two owners: the lots' frontage and least width (`LOT_FRONTAGE`/`MIN_LOT` of the block cutter, `STRIP_FRONTAGE`/
`STRIP_LEAST` of the strips, both lots.ts's) are one `lots` block both read, and a node's most legs (the graph's `MAX_LINKS`, the
tidy pass's `MAX_LEGS`) is the graph's `max_links`. The graph's hug test is `hug_cos`: the road tool's `road.parallel_cos` is
another rule (a new road over an old one), not this one. A lot's end inset is derived, a cell and a half of the strips' grid,
no longer a second number. The definitions stay and say what they are: the keys' layouts, the streams and the golden-ratio and
Numerical Recipes multipliers, the tolerances, a junction's three arms, the ground's five probes.

**What changes.** Nothing: the generated and scenic cities' hashes, the numbers and every photo the same (`quick`: 44 of 46).

### 6.48 B2, C14 (the roads): the junctions' geometry in tuning, the roads' definitions sourced (2026-11-08)

**Cause and fix.** The road geometry's junction rules held the 4.0's numbers (roads/geometry.ts) beside their tolerances: how far a
mouth retreats, when a bend tapers and over how long, a dead end's bulb, a kerb return's radius, taper, fillet and bevel. They went
to a `junction` group of tuning (doubles, as the code computes them), read through the `Tuning` the four functions now take
(`arm_retreats`, `bulb_retreat`, `junction_retreats`, `corner_fillet`), handed by their callers from the catalogue the network holds
(`net.cat->tune`) or the state's. The angles are data in degrees and the code turns them as it did (`deg * PI_D / DEG_PER_HALF_TURN`:
the same doubles, 30 degrees giving `PI_D / 6` bit for bit). The longest retreat, written in both files, is one field. The
definitions stay and say what they are: the tolerances, the nearest-point search's samples and halvings, the arc tables' chords, the
road hash's cells, the flags' bit shifts, the grade solver's passes and disc, √2; the solver's own 2π is the core's `TAU_F` (the
same float).

**What changes.** Nothing: the hashes, the numbers and every photo the same (`quick`: 44 of 46).

### 6.49 B2, C14 (the traffic agents' meshes): every part size in agents.json shapes (2026-11-08)

**Cause and fix.** The vehicles' part builders held their sizes by eye beside their definitions: a steering wheel's spokes, a
seat's nose and lap, a wheel's hub, rim, spokes and brake disc, a pod's squareness, a wreck's dents (the 4.0's waves), the bicycle's
stays and saddle, the bus's partition, the heavy vehicles' pillars and seams, the car's lamps, grille, plates, handles and mirrors,
the lofted shape's rows. They went to agents.json `shapes`, one block a builder file, read once at boot through the one reader of
shapes into `AgentShapes`, which the trim (handed to every part builder), the cabin look and the car style carry; the pod, the
wreck and the bus and truck readers take it. A size two files had is one field: a door's split (bus and truck), a wheel arch's room
round its tyre (the car's `CAR_ARCH_PAD_M` and the heavy vehicles' pad: every vehicle's, in the shared parts). The blob under a
walker gets its sides with its other sizes (agents.json `people.blob`). The definitions stay and say what they are: the pod's turns
and rings (the sizes of its arrays), the cooked cast's file layout, the hash streams and primes, the ring rows each level keeps,
the units, the tolerances. `people_asset.h` named a cooker no longer in the repository: it says where it is kept.

`agent_renderer.cpp` was over the file limit and the reading made it more: the boot's mesh building and the shapes' reading went
to `agent_looks.cpp`, the file of what the agents read of agents.json at boot, and the renderer is under the limit.

**Not here, and why.** The ragdoll's and the bodies' numbers (air and ground friction, limb thickness, the folds, the rest, the
fixed step two files write) are the next batch: their owner is the body renderer, not the part builders.

**What changes.** Nothing: the hashes, the numbers and every photo the same (`quick`: 44 of 46).

### 6.50 B2, C14 (the ragdoll): the bodies' law in agents.json shapes, the step one field (2026-11-08)

**Cause and fix.** The ragdoll held the 4.0's numbers (ragdoll.ts) beside its definitions: how much speed a particle keeps in the
air and dragging on the ground, a limb's half thickness, the relaxation's passes, the head's top and the belly, how far a knee, an
elbow and the hip fold, when a body lies still; the body renderer held the fixed step and the most steps a frame. They are a
`ragdoll` block of agents.json shapes, handed from the agents' renderer to the people's and the bodies' renderer at init, and from
there to each ragdoll at `begin`, which keeps it. The step was written twice (`ONE_STEP_S` of the ragdoll's first move, `STEP_S` of
the bodies' clock): one field, the same float (1/60). The definitions stay and say what they are: gravity to three figures, the
tolerance, the limbs' and parts' joint tables, the bodies' seed.

**What changes.** Nothing: the hashes, the numbers and every photo the same.

### 6.51 B2, C14 and C16 (the rest of the render): the street emitters' sizes in streets.json shapes, one colour unpack (2026-11-08)

**Cause and fix.** The street emitters held the 4.0's and by-eye sizes beside their definitions: a viaduct's build-up, piers,
bents and parapets, a tunnel portal's jambs, crown, lintel, headwall and wings, a cable-stayed span's strands and struts, where a
junction's kerb control point may stand, the reserve a ramp leaves of a footway, the street trees' scales. They went to
streets.json `shapes`, read by `StreetLook::load` through the one reader of shapes and reached by every emitter through the street
look it is handed (`e.L.shapes`, `L.shapes`). The street trees' choice of species (`hash % 2`, the first two of `trees.species`) is
data too, a list of their indices, the same pick.

Two overlays unpacked a packed colour by hand (`overlay_build.cpp` zone colours, `overlay_renderer.cpp` congestion ramp: shifts,
masks and 255 a second and third time beside `rgba8_to_color`, `render/mesh_builder.cpp`): both call it, the same floats. The frame's
cost read the committed queues' times by number (`hand_ms_[5]`): the queues are named in the order they are committed. The
definitions stay and say what they are: byte tops, bit layouts, the FNV-1a offset basis and prime, the golden ratio and the
icosahedron, a display's gamma, the hectare, a frustum's planes, the culling boxes' height, the hysteresis of a dropped chunk,
the tolerances. The structures' and the stays' tones join the pending render palette (`log/pendencias.md`).

**What changes.** Nothing: the hashes, the numbers and every photo the same.

### 6.52 B2, C14 (the rest of the simulation and the core's tools): their tunables to their tables, their definitions sourced (2026-11-08)

**Cause and fix.** The last tunables of `sim` and `api` sat beside their definitions: how alike two buildings look (form, roof,
storeys, colour, a neighbour's distance, the columns a body is read in), when a lot is taken by a building, the roof a volume
takes when it names no pitch (a 30 degree gable, a 12 degree shed, sawtooth teeth of 6 m), the drawn lots' cut and join rules, how
far the inspector and a drawn lot look round a point, where the body editor's handles stand. They went to their tables: a
`likeness` group of tuning (the `Tuning` growth and the park probe hand the likeness functions), `growth.json` `occupied_share`
(`GrowthDef`), a `roof` block of buildings.json shapes (the roof helpers take it, from the generator's data their five callers
hold), a `lot_rings` block of growth.json shapes (the lot ring functions the commands, the core's lot tools and the lot land call
take the generator's shapes), tuning's `inspect.radius_m`, `lot.angle_look_m` and `build.handle_*`.

The least lot was written twice (`MIN_LOT_M` of the lot rings, the generator's `lots.least_m`, both lots.ts's `MIN_LOT`): the lot
rings read the generator's. A degree was written by hand beside the core's (`180 / 3.14159...` in the commands: the core's
`RAD_TO_DEG_D`, added beside its float, the same double), a quarter turn too (`PI_F / 2`, the same float). The stand's search, a
curved side's chords and the ray's march stay as the methods' resolution, said so. The definitions say what they are: the integer
mixes (Knuth's, xxHash's PRIME32_2, MurmurHash3's fmix32 and fmix64), the save versions, the casualty word's layout, the zone
table's rows, the units, the sentinels and tolerances.

**What changes.** Nothing: the hashes, the numbers and every photo the same.

### 6.53 B2, C14 (the scripts): the interface's tunables in data/game/tools.json, the scripts' definitions sourced (2026-11-08)

**Cause and fix.** The scripts held what one tunes in the interface beside their units and tolerances: the Inspector's road panel
(how much a curve and a raise move, where a curve may stand), the profile editor's widening and speed steps, the split mark, the
handles' arrow and chevron, the Models shelf studio's picture, frame, sun and ambient, a reference model's picture resolution and
opacity steps. They went to data/game/tools.json, blocks `inspector`, `profile_editor`, `split`, `model_studio`, `reference_model`
and the `handle` block's `arrow_back` and `chevron` (schema in step), read the way the tools already read it (`static var ... =
DataTables.game("tools")...`). The sky's clouds hashed with bare numbers: they are named (xxHash's PRIME32_5 and PRIME32_4, an odd
multiplier, two xorshifts, a word and its range), the sky's kilometre is the units' one, the rain's still wind is named. The
definitions say what they are: the units, a frame's least time, the short hash, the compass points, the --shot wait (a developer's
picture, not a test), the save thread's poll, the unread cell, the sea plane's extent, the chords of discs, arcs and caps, the
clouds' slots and puffs (the shader's array sizes), a PCM sample's top, the vignette's side.

`sky_view.gd` was over the scripts' limit before and the named hash made it more: the clouds' puffs (their shape per cloud, their
laying where the wind and the view put them, the hash that jitters them, the arrays the shader reads) are a class of their own,
`render/cloud_puffs.gd`, which the sky owns; the sky is under the limit. `daylight.gd` was over it too, and naming the engine's
default cascade side (Godot's 4096 atlas over two cascades a side) made it more: the sun's cascades fitted to the view are
`render/sun_cascades.gd`, which the daylight owns and the graphics tier sets (`daylight.cascades`); the daylight is under the limit.

**What changes.** Nothing: the numbers and every photo the same.

### 6.54 B2, C12 (one file, one responsibility): the model survey's blocks apart from their looks (2026-11-08)

**Cause and fix.** `body_survey.cpp` read a 3D model in two steps in one file: the blocks it stands on (the faces that look up,
filed in a grid, joined into tops that merge, each a block with its hull) and what each block shows (wall shades, bays, the crown,
a plinth of another colour) with the body made of them. It was over the file limit before the commands batch and that batch's
bindings made it more (§6.43). The blocks are `survey_blocks.cpp` with their header (`SurveyBlock`, `survey_blocks`); the looks and
the assembly stay in `body_survey.cpp`. Both under the limit; the stage closes with no file worse than the baseline found it.

**What changes.** Nothing: the survey makes the same bodies (`quick`).

## 7. The catalogue: every finding under its cause (generated from §2-§4 by the method of §6)

Each line: the review section, the first file and line the finding names, its lead. The full text and the fix are at
that place in the section named.

### C1. the source game as the reference (142)

- §2 `GODOT-PORT-GUIDE.md:82` The base architecture was written from another TypeScript game's code
- §2 `ferramentas.md:158` A written rule allowed copying
- §2 `conversor.md:100-117` A tool exists to do it
- §2 `conversor.md:130` The runtime was put in `native/src/core` because that folder is exempt from the magic-number gate
- §2 `base.md:99` Features ported from its source files, by the records
- §2 `causa-raiz.md` Decisions justified by the source's code even in the 2026-11-07 audit
- §2 Size of it in the code
- §2 The source game is committed inside this repository
- §2 `CLAUDE.md:7-8` Documents that still order porting from it
- §3 `build.sh:67` The product zip ships the source game
- §3a `seeded.h:1-3` The procedural world is built to reproduce the source game bit for bit
- §3a `js_runtime.h:23-27` A translated `throw` kills the game
- §3a `js_value.h:162` `js::Value` breaks the semantics it claims
- §3a `js_map.h:103-104` JavaScript containers in a C++ game
- §3a `city_state.h:74` A comment in the source of truth cites the source game
- §3b `blocks.h:1` The folder is the source game's generator, renamed
- §3b `massing_tall.cpp:162` A random draw thrown away to keep the source's stream
- §3c `city_plan.cpp:1` Generation is designed for parity with the source's code
- §3d `road_gen_graph.cpp:28-29` The road generator keeps JavaScript `Map` order for parity
- §3d `commands.h:1` The source game cited in 39 of 70 command files
- §3e `geometry.cpp:1` R11. The source game's code and numbers inside the sim
- §3l `tree_look.cpp:23-24` Placement and geometry chosen for parity with the source game
- §3o `city_core_flow_probe.cpp:1-2` Source-game code as the reason or the oracle
- §3p `data_tables.gd:65` P1. The source game's code is the reference, against the rule "look and features only"
- §3q `camera_pull.gd:2` SOURCE CITES
- §3q `data_tables.gd:63-65` THE STYLE OVERLAY HAS TWO MECHANISMS AND A FIFTH OWNER OF "roadcraft"
- §3q `game_flow.gd:37` game_flow.gd:37 "(Roadcraft saveOnClose)"
- §3q `interior_controller.gd:3` SOURCE CITES IN THE GAME SCRIPTS
- §3q `lab_recorder.gd:1` lab/lab_recorder.gd:1 "(the 4.0's lab/recorder)", resource_watch.gd:1 "(the 4.0's lab/resources/diagnostics)"
- §3q `render_switches.gd:7` SOURCE-GAME KEYS AND CITES
- §3q `formatters.gd:38-42` THE CITY STYLE IS A HALF FEATURE
- §3q `lot_projects.gd:1` lot_projects.gd:1 "The lots of the Roadcraft's lot lab
- §3q `session.gd:23-24` THE CITY'S STYLE IS NOT PART OF THE CITY
- §3q `barrier_tool.gd:1` barrier_tool.gd:1 "Muros
- §3q `building_tool.gd:1` building_tool.gd:1 "(Massa, Fachada, Telhado, Cores)", control_tool.gd:1-2 "(tab Controle) 
- §3q `furnish_hand.gd:1` furnish_hand.gd:1 "Construir › Interior, as in The Sims", :52 "(Roadcraft openInside, seeInside)"
- §3q `lots_tool.gd:1` SOURCE CITES AND PORTUGUESE UI NAMES
- §3q `road_draft.gd:37` SOURCE CITES AND PORTUGUESE UI NAMES
- §3q `terrain_tool.gd:3` SOURCE CITES AND PORTUGUESE
- §3r `approach_panel.gd:1` approach_panel.gd:1 "(Roadcraft ui/roads/junctionPanel.ts)", connector_panel.gd:1 "(Roadcraft ...
- §3r `hud_popover.gd:40` hud_popover.gd:40 cites ".v2-pop-body.icons"
- §3r `junction_marks.gd:1-2` junction_marks.gd:1-2 "(Roadcraft main.ts)", "the Controle tab"
- §3r `preview_tag.gd:1` preview_tag.gd:1 "(Roadcraft main.ts drawRoadPreview)"
- §3r `road_options.gd:1` SOURCE CITES AND PORTUGUESE
- §3r `road_section_view.gd:1` road_section_view.gd:1 "(Roadcraft ui/roads/crossSection.ts)"
- §3r `start_screen.gd:51-56` CONFIRMED HALF FEATURE
- §3r `theme_art.gd:46` THE SOURCE GAME'S WEB UI AS THE REFERENCE OF THE THEME
- §3r `theme_builder.gd:42` THE THEME IMITATES THE SOURCE'S BROWSER
- §3s `daylight.gd:1` SOURCE CITES
- §3s `interior_lights.gd:2-3` BEHAVIOUR COPIED FROM SOURCE LINES
- §3s `agent_door.gd` SHADER LOOK COPIED FROM THE SOURCE
- §3s `building_parts_body.gd` THE FLAGS ARE THE SOURCE GAME'S
- §3s `lamp_glow.gd` SOURCE CITES
- §3s `people_look.gd` SOURCE CITES
- §3s `puffs.gd` THE BLAST PUFFS MOVE AS THE SOURCE'S DID
- §3s `road_body.gd` SOURCE CITES
- §3s `sky.gd` sky.gdshader:1 "(Roadcraft render/scene.ts makeSky)"
- §3s `terrain_layers.gd` SOURCE CITES
- §3s `water.gd` SOURCE ARGUMENT
- §3t `blast.json:2` SOURCE GAME VALUES IN THE DATA
- §3t `nature.json:83-87` HALF-DONE STATE
- §3t `nature.json:82` nature.json:82 element kinds and growth.json:33 profile ids in the source's camelCase
- §3t `road_profiles.json:2-31` DEAD STYLE DATA
- §3t `city_gen.cpp:24-31` SOURCE IDS: profile ids are the source game's camelCase
- §3t `data_tables.gd:65` THE GAME PLAYS THE SOURCE GAME'S CODE VALUES
- §3t `streetNames.ts` SOURCE BEHAVIOUR AS DATA
- §3t `lorryDocks.ts` SOURCE CODE AS DATA ORIGIN
- §3t `data_tables.gd:65` :410 "United States data, the default style's"
- §3t `tuning.json` TUNING WITH NO DOC, AND SOURCE CONSTANTS IN IT
- §3t `interior.json:2` interior.json:2 is the source code, line by line
- §3t `daylight.json:2` daylight.json:2 source code as origin
- §3t `camera.json:2` camera.json:2 "Roadcraft's perspective rig and its feel"
- §3t `lab.json:2` lab.json:2 "(lab/recorder/analysis.ts SETTLE_MS)"
- §3t `optimizations.json:2` optimizations.json:2 the tab mirrors the source's three.js panel
- §3t `CLAUDE.md` THE SOURCE GAME'S WHOLE CODE IS IN THE REPOSITORY
- §3t `render_switches.json:4` render_switches.json:4 the Diagnosis is "the 4.0's doctor", its keys named after the source's functions
- §3t `saves.json:2` saves.json:2 "Roadcraft game.ts:756", "Roadcraft persistence.ts onSaveFailed", "DEBOUNCE_MS, MAX_WAIT_MS"
- §3t `sky.json:7` sky.json:7 "_doc" sits after five keys
- §3t `builder.h` The builder's table
- §3t fx.* labels name the source's levels and passes
- §3t `agents.json:2` SOURCE CODE AS DATA
- §3t `buildings.json:4` buildings.json:4 all twelve architectures sit under styles."roadcraft"
- §3t `buildings.json:118` SOURCE CODE AS DATA
- §3t `interiors.json:4` interiors.json:4 everything under styles."roadcraft"
- §3t SOURCE VALUE
- §3t `models.json:4` models.json:4 everything under styles."roadcraft" again
- §3t `overlays.json:2` overlays.json:2 "Roadcraft hud.ts:508-522", "Roadcraft lotTool.ts fillAlpha"
- §3t `streets.json:29` THE DEFAULT MARKINGS ARE THE 4.0'S CODE, AND THE TABLE'S OWN DOC SAYS OTHERWISE
- §3t `utilities.ts` SOURCE CODE AS ORIGIN
- §3t `signs.ts` SOURCE CODE AS ORIGIN
- §3t `transit.json:2` SOURCE CODE AS ORIGIN
- §3t `drawers.json:2` SOURCE CODE AS ORIGIN
- §3u `city_mass_test.gd:1-2` PARITY WITH THE 4.0 AS THE PASS RULE
- §3u `flow_control_test.gd:1-2` THE SOURCE'S CODE AS THE ORACLE
- §3u `lab_test.gd:1` THE 4.0 AS THE ORACLE
- §3u `inspector_test.gd:159` inspector_test.gd:159 set_style "roadcraft" enshrines the 4.0 style
- §3u `light_color_test.gd:20` THE 4.0'S CODE AS THE ORACLE
- §3u `people_test.gd:13` THE 4.0 AS THE MEASURE
- §3u `road_commands_test.gd:25` road_commands_test.gd:25 FILLED_STYLES ["roadcraft", "brasil"] keeps the 4.0 style a tested requirement
- §3u `road_edit_test.gd:65` PARITY WITH THE 4.0 AS THE PASS RULE
- §3u `road_grade_test.gd:1-3` A PORT OF THE SOURCE'S TEST, WITH ITS NUMBERS
- §3u `road_money_cases.gd:1` THE 4.0 AS THE MEASURE
- §3u `save_undo_test.gd:143` SOURCE REFERENCES
- §3u `street_move_cases.gd:23` SOURCE REFERENCES
- §3u `capture_blast.gd:13-15` THE 4.0'S CODE VALUES FOR A CAMERA
- §3u `capture_landscape.gd:12` THE 4.0'S CODE API AS REFERENCE
- §3u `capture_pair.gd:3-5` THE SOURCE GAME'S CITY LAID INTO THIS GAME
- §3u `capture_money.gd:2-3` capture_money.gd:2-3 "Same steps and camera as the 4.0's run", :19 STREET "the card the 4.0 opens on", :50 "as the ...
- §3u `capture_roads.gd:72` SOURCE REFERENCES
- §3u `capture_street_pairs.gd:1-4` THE 4.0 AS THE FRAME
- §3u `capture_transit.gd:2-3` THE 4.0'S CAMERA POSES
- §3u `frozen_plan.json:13-43` frozen_plan.json:13-43 steps named "setView"
- §3u `landscape_cards.gd:1` THE 4.0 AS THE FRAME
- §3u `look.gd:1-5` THE VISUAL TOOLING IS A PORT OF THE SOURCE'S
- §3v `cook_signatures.py:1-4` THE 4.0'S BUILDINGS SHIPPED AS DATA
- §3v 656 float-noise literals
- §3v 5 files without elements/lotPlan
- §3v Keys camelCase from the 4.0
- §3v Every civic/commercial doc
- §3v Duplicate overlapping elements in every doc
- §3v Two volumes with identical footprint stacked by base
- §3v facadeGeometry repeats the identical dict for sides "0".."3" in every volume
- §3v Colours as decimal ints
- §3v Every lot doc with elements is the same lotPlan 10 template
- §3v `LICENSES.json:2` people/LICENSES.json:2 "made by tools/assets/cook_people.py from Roadcraft public/models/citizens/people"
- §3w `live40.mjs` T1. The repository carries the source game and the machinery to port its code
- §3w `cook_people.py:1-7` THE PEOPLE ARE THE 4.0'S COOKED CAST
- §3w `calm_stone.py:1-2` calm_stone.py:1-2,10-11 rewrites assets/surfaces/building-finishes.map.png in place, following "the 4.0's recipe
- §3w `export_builder_ui.py:2-6` THE BUILD TAB IS EXTRACTED FROM THE 4.0'S SOURCE CODE
- §3w `cook_signatures.py:1-4` cook_signatures.py:1-4 "No code of the 4.0 comes along
- §3w `ktx2_to_png.mjs:1` ktx2_to_png.mjs:1 converts "the Roadcraft cooked KTX2 surfaces"
- §3w `cook_ragdoll.py:4` cook_ragdoll.py:4 "the 4.0's ragdoll.ts"
- §3w `make_flag_icons.py:2-3` make_flag_icons.py:2-3 the fourteen flag designs are "the Roadcraft's flags.ts FLAG_PATTERNS"
- §3w `make_road_profiles.py:1-5` THE ROAD CATALOGUE IS THE 4.0'S
- §3w `make_pole_parts.py:1-3` make_pole_parts.py:1-3,14,19-31 the pole is the 4.0's
- §3w `live40.mjs` live40.mjs: keeps the 4.0 running in a headless Chrome and serves POST /eval of arbitrary JS on it "a question to ...
- §3w `graph.json` THE SOURCE GAME'S CODE TRACKED TWICE MORE
- §3w `extract_feature.cjs:1-16` THE PORTING TOOLS COPY SOURCE CODE
- §3w `roadcraft-machine.json:3` configs/roadcraft-machine.json:3 "source"
- §3w `flowStats.ts` ITS OUTPUT IS COMPILED INTO THE GAME
- §3w `CLAUDE.md` tools/oracle-4.0
- §3w Decision recorded for §1

### C2. verification that cannot fail (78)

- §3 `suites.json:162` Every test scene reruns on any change in `project/`
- §3 `suites.json:79` `gates_changed` is keyed by the whole tree
- §3 `scope.py:70` The gate diff reads the whole worktree
- §3 `suites.json:93` A golden-file verdict depends on the golden being an input
- §3 `gate_defs.py:92-99` The magic-number gate is satisfied by naming
- §3 `gate_forbidden.py:169` "Render reads only" is checked only by include path
- §3p `inspector_test.gd:29-30` P9. The tests have no harness and no control of the clock
- §3r `compass.gd:7-13` compass.gd:7-13 and effects_panel.gd:11-20 put @onready on plain initial values
- §3r `health_panel.gd:9-15` @onready on plain values again
- §3r `hud_drawer.gd:17-29` @onready on plain values again
- §3r `hud_top_bar.gd:13-22` @onready on plain values again
- §3r `preview_tag.gd:6-7` @onready on plain values again
- §3r `roadcraft_hud.gd:10-25` @onready on plain values again
- §3r `selection_overlay.gd:9-13` @onready on plain values again
- §3s `city_world.gd:9-23` A LIST KEPT ONLY TO SATISFY A GATE
- §3t `strings_e.csv:381` GARBLED TEXT THE PLAYER SEES
- §3u NO SHARED HARNESS
- §3u `city_store.gd:69-70` THE CLOCK RUNS BEFORE THE TEST HOLDS IT
- §3u `boot_refusal_test.gd:19` boot_refusal_test.gd:19 reaches the core as CityStore.get_child(0), past the store's API
- §3u `building_part_cases.gd:105-110` THE LOT FRAME REWRITTEN IN TESTS
- §3u `building_inside_cases.gd:44-51` COPIED HELPERS
- §3u `camera_feel_test.gd:81-83` A TEST THAT MEASURES WITH THE DEFECT IT SHOULD CATCH
- §3u `camera_feel_test.gd:87` TESTS ON PRIVATE MEMBERS
- §3u `building_tool_test.gd:72-80` THE LOT FRAME A THIRD TIME
- §3u `camera_feel_test.gd:49-51` step_ticks USED BESIDE THE SIM THREAD
- §3u `city_mass_test.gd:109-113` city_mass_test.gd:109-113 its _check prints failures only
- §3u `flow_control_test.gd:55-58` TRAFFIC RUN BY FRAMES, NOT TICKS
- §3u `inspector_test.gd:29-30` step_ticks WHILE THE SIM THREAD RUNS AT NORMAL SPEED
- §3u `inspector_test.gd:176-181` A TEST LEAVES A FILE IN THE PLAYER'S user://
- §3u `lab_test.gd:159-177` MEASUREMENT TESTS THAT DEPEND ON THE MACHINE
- §3u `inspector_test.gd:41` THREE PASS/FAIL OUTPUTS
- §3u `lamp_light_test.gd:26` lamp_light_test.gd:26 new_map with the clock running
- §3u `model_test.gd:52` TESTS PINNED TO DATA COUNTS
- §3u `model_test.gd:194` model_test.gd:194 writes user://model_test_house.png into the user:// root
- §3u `quality_test.gd:240-244` THE TEST BLESSES A DEAD SETTING
- §3u `quality_test.gd:111-115` FRAME-COUNT WAITS FOR LIVE TRAFFIC
- §3u `quality_test.ts` quality_test.tscn:5 node named "ShellTest" again
- §3u `road_commands_test.gd:28` road_commands_test.gd:28 DECK_OVER_GROUND_M 0.3 with the comment "the ground under a road is dropped 0.6 m below its ...
- §3u `shell_test.gd:162` TESTS ON PRIVATE MEMBERS
- §3u `street_works_test.gd:224` A FOURTH OUTPUT FORMAT
- §3u `transit_test.gd:35` A TEST WHOSE SAVE CHECKS NEVER RUN
- §3u `transit_test.gd:37` transit_test.gd:37 quit(failures)
- §3u `yard_test.gd:26` A BOUND FAR BELOW WHAT IS MEASURED
- §3u `layer_gpu.gd:20` layer_gpu.gd:20 ignores boot's errors
- §3u `tier_gpu.ts` tier_gpu.tscn:5 node named "CameraPan"
- §3u `capture_clock.gd:30-33` TESTS ON PRIVATE MEMBERS
- §3u `capture.gd:96-99` capture.gd:96-99 Settings.apply called twice in a row
- §3u `capture_features.gd:83` TESTS ON PRIVATE MEMBERS
- §3u `capture_interior.gd:11-18` CAPTURES PINNED TO GENERATED IDS
- §3u `capture_interior.gd:72` PRIVATE METHOD BY NAME
- §3u `capture_shell.gd:56` PRIVATE MEMBERS
- §3u `capture_parity.gd:1` A FLAG THAT NEVER MATCHES
- §3u `capture.gd:143` CAPTURE NUMBERING COLLIDES
- §3u `live_driver.gd:54` live_driver.gd:54 hud.actions._open_new
- §3u `capture_wait.gd:7-30` TWO "WAIT UNTIL DRAWN" WITH DIFFERENT RULES
- §3u `ui_driver.gd:96-103` A GENERIC HELPER WITH A HARD-CODED SHOT
- §3u `look.gd:98-99` look.gd:98-99 "wait" steps sleep by wall clock
- §3w `rules.json:2-20` T2. The gates cannot hold the rules they police
- §3w `CLAUDE.md` 552 problems
- §3w `compass.gd:8-13` ESTADO_NA_UI 75
- §3w `agents.json` SEGUNDO_DONO 307
- §3w `gate_data.py:64-65` gate_data.py:64-65 json.load keeps the last of two equal keys
- §3w `gate_data.py:56` gate_data.py:56 DADOS_SEM_SCHEMA only for project/data/**
- §3w `gate_data.py` gate_data.py validate
- §3w `strings_e.csv:381-382` i18n CSVs have no gate
- §3w `gate_tuning.py:25-32` gate_tuning.py:25-32
- §3w `gate_tuning.py:58-72` gate_tuning.py:58-72 check_mirrors
- §3w `gate_text.py:112-132` gate_text.py:112-132 sees only direct assignments to .text/... and direct calls
- §3w `gate_ui.py` gate_ui.py: see ESTADO_NA_UI above. gd_syntax/lex_c/lex_gd
- §3w `rules.json:2-20` rules.json:2-20 exclude_dirs
- §3w `rules.json:355-380` rules.json:355-380 magic_numbers covers native/src, project/game, project/ui only
- §3w `rules.json:382` rules.json:382 data_copy_min_abs 4.5
- §3w `rules.json:393-419` rules.json:393-419 data_copy_allow lists CAL_SIDE, LOT_LOOK, LOT_LOOK_M twice each
- §3w `rules.json:439` rules.json:439 "mirrors"
- §3w `owners.json` owners.json: 9 concepts only
- §3w `test_pool.py:35` test_pool.py:35 asserts the sibling finishes within 3 s of wall clock
- §3w `test_proc.py:37` test_proc.py:37 and test_photodiff.py:28 create temp folders with mkdtemp and never remove them
- §3w `test_photodiff.py` test_photodiff.py tests tools/build/photodiff.py from inside tools/tasks/tests

### C3. contracts kept by hand (96)

- §3c `city_gen.cpp:22-39` Profile ids hard-coded by name in the generator
- §3d `body_blocks.cpp:141-160` The Build tool's handle edit dispatches on strings
- §3h `parking_sources.cpp:21` Zone codes typed again
- §3i `shell.cpp:26-27` L11. Contracts with no owner
- §3m `catalog_loader.cpp:80` A4. One vocabulary, many tables
- §3p `city_core_query.cpp:93-104` P3. C++ enums and constants are mirrored by hand
- §3p `surface_means.gd:18-21` P4. Contracts by position or key order
- §3q `health_log.gd:10` health_log.gd:10 COST_PARTS names must follow api render_cost's keys by hand
- §3q `body_edits.gd:18` body_edits.gd:18 body_add_setback(id, vol, -1.0, 2) and :32 bevel corner -1
- §3q `control_tool.gd:15-17` THE JUNCTION CONTROL COUNT IS OWNED BY THE HUD'S DATA
- §3q `cloud_tool.gd:10-11` cloud_tool.gd:10-11 YAW_PER_M_X / YAW_PER_M_Z
- §3q `lot_proposals.gd:5` lot_proposals.gd:5 CORE_CELL_M = 256.0 "core/grid.h CHUNK"
- §3q `lots_tool.gd:6-9` C++ ENUMS MIRRORED BY HAND IN GDSCRIPT
- §3q `tool_controller.gd:12` tool_controller.gd:12 the hint signal's contract is "text already translated"
- §3q `transit_tool.gd:7-8` transit_tool.gd:7-8 MODE_BUS / MODE_TRAIN mirror the core's TransitMode by hand
- §3r `approach_panel.gd:6` approach_panel.gd:6 RULES
- §3r `hud_actions.gd:7` hud_actions.gd:7 TOWN_MAP = 0 typed again
- §3r `hud_messages.gd:31-39` TWO CONVENTIONS FOR ONE SIGNAL
- §3r `hud_popover.gd:45` STRING-BUILT METHOD CALLS FROM DATA
- §3r `hud_tip.gd:27` TOKENS READ AS THE WRONG KIND
- §3r `hud_top_bar.gd:11` (checked) hud_top_bar.gd:11 SPEEDS sends 0..3
- §3r `hud.json` (checked) data/ui/hud.json control_marks has 7 rows = road_network.h:63 NodeControl CTL_COUNT
- §3r `lab_views.gd:9` THE FRAME SECTIONS HAVE TWO LISTS THAT DISAGREE
- §3r `land_strips.gd:10-11` TEXT SURGERY ON TRANSLATIONS AND UNITS BY DISPLAY TEXT
- §3r `pop_bodies.gd:52-53` pop_bodies.gd:52-53 _short cuts the translated text at "
- §3r CONSTANTS TYPED IN MANY SCRIPTS
- §3r `road_panel.gd:18-19` road_panel.gd:18-19 CROSS_ZEBRA / CROSS_SIGNAL mirror NodeCrossing by hand
- §3r `road_spec_ops.gd:111` THE TWO STRIP VOCABULARIES TRANSLATED IN GDSCRIPT
- §3r `street_strip.gd:113` street_strip.gd:113 cuts translated card names at "
- §3r `theme_builder.gd:180-184` theme_builder.gd:180-184 _wide sets content margins from the "card_radius" token
- §3r `transit_strip.gd:5` transit_strip.gd:5 MODE_KEYS mirrors TransitMode by hand
- §3s `city_world.gd:7` city_world.gd:7 LUMA
- §3s `element_effects.gd:33` THE ELEMENT/EFFECT BOUNDARY IS A RENDER TABLE'S LENGTH
- §3s `lamp_radiance.gd:5` lamp_radiance.gd:5 LENSES = 5 "props.gdshader lit_radiance
- §3s `agent_door.gd` C++/SHADER CONTRACTS BY COMMENT
- §3s `building_bay.gd` C++/SHADER CONTRACTS BY COMMENT
- §3s `building_glass_body.gd` building_glass_body.gdshaderinc:90-107 light() re-implements the engine's Lambert + GGX by hand
- §3s `building_glass_body.gd` C++/SHADER CONTRACTS BY COMMENT
- §3s `building_pane.gd` C++/SHADER CONTRACTS
- §3s `building_props.gd` C++/SHADER CONTRACTS
- §3s `ground_paint.gd` C++/DATA/SHADER CONTRACTS
- §3s `people_pose.gd` C++/COOK/SHADER CONTRACTS
- §3s `props.gd` C++/SHADER CONTRACTS
- §3s `road_body.gd` C++/SHADER CONTRACTS
- §3s `shore.gd` CONTRACTS: shore.gdshaderinc:23 SHORE_DRY -500 "(TerrainGen::NO_WATER)"
- §3s `tone_inverse.gd` AN ENGINE INTERNAL MIRRORED TWICE
- §3s `vegetation.gd` CONTRACTS: vegetation.gdshaderinc:20 PART_* "tree_part.h TreePart", :21 FAR_TINT_SPAN "far_cards.cpp"
- §3s `vegetation_cards.gd` vegetation_cards.gdshader:10 REGION_W/LEAF_U/FROND_U mirror leaf_atlas.h by comment
- §3s `wear.gd` wear.gdshaderinc:8 WEAR_RES/TILE/TILES mirror wear_field.h
- §3s `surface_means.gd:18-21` CONTRACT BY JSON KEY ORDER
- §3s `surface_means.gd:7` surface_means.gd:7 LANE_TONES mirrors road_look.h:33 TONE_SLOTS and road_body.gdshaderinc:28 lane_tone[8]
- §3s `tone_map.gd:11-19` tone_map.gd:11-19 mirrors Godot's tonemap_aces constants
- §3s `transit_overlay.gd:6-8` transit_overlay.gd:6-8 MODE_BUS/TRAIN/METRO mirror the core's transit modes by value
- §3t `blast.json:30` CONTRACTS BY POSITION
- §3t OPAQUE NUMBERS
- §3t POSITIONAL MATRICES
- §3t `settings.json:12-19` settings.json:12-19 filter_radius mirrors the engine's table per ShadowQuality
- §3t POSITIONAL: :10 exhaust.classes repeats VehicleType's order by name list
- §3t `buildings.json:75` POSITIONAL: buildings.json:75 era_affinity 12 rows in architecture order
- §3t `effects.json` effects.json bursts "kind"
- §3t `session.gd:62` STYLES BY INDEX
- §3t POSITIONAL: :393 drawings "in SignType order"
- §3t `builder.json:2` ENUMS BY NUMBER IN UI DATA
- §3t `transit_overlay.gd:6-8` ENUMS BY NUMBER, AND ONE MAPPING TWICE
- §3t `nature.json:82` ONE ORDER IN FOUR FILES
- §3t `land_panels.json:309-326` NAMES THAT DISAGREE
- §3t `nature.json` ENUMS BY NUMBER
- §3u `building_edit_test.gd:29-44` ENUMS BY NUMBER IN TESTS
- §3u `building_light_test.gd:14-18` CONTRACT COPIES
- §3u `building_plan_cases.gd:98` POSITIONAL INDEX INTO UI DATA
- §3u `building_plan_cases.gd:4` building_plan_cases.gd:4 SHAPE_SIDES {1
- §3u `city_mass_test.gd:8` city_mass_test.gd:8 LARGE_MAP := 5
- §3u `flow_control_test.gd:16` flow_control_test.gd:16 STOP_NODES 320 "every node of the town" typed for map 0 seed 1
- §3u `edit_fuzz_test.gd:136-226` edit_fuzz_test.gd:136-226 edit kinds as bare numbers
- §3u `light_color_test.gd:21-36` CONTRACT COPIES
- §3u `road_edit_test.gd:23` road_edit_test.gd:23 RULE_STOP 1 by number
- §3u `road_money_cases.gd:15` COPIES AND POSITIONS
- §3u `save_undo_test.gd:30` save_undo_test.gd:30 MARKING_STYLES := 2 while streets.json has five marking styles
- §3u `street_save_cases.gd:9` CONTRACT COPIES
- §3u `yard_bench.gd:14` CONTRACT COPIES
- §3u `camera_pan.gd:44-45` THE FRAME SECTIONS LISTED A THIRD TIME
- §3u `main_thread_probe.gd:10` COPIES: main_thread_probe.gd:10 MAP_DEFAULT 5
- §3u `pending_probe.gd:12` COPIES: pending_probe.gd:12 MAP_DEFAULT 5, :18 SPEED_FASTEST 3
- §3u `capture.gd:9` CONTRACT COPIES
- §3u `capture_demolition.gd:28` A WRONG ENUM COPY
- §3u `capture_features.gd:15-16` POSITIONAL: capture_features.gd:15-16 MODE_RAISE 0, MODE_MESA 4, :172-174 cards[1] "parque", cards[2] "escola", :183 ...
- §3u `capture_flag.gd:20` COPIES: capture_flag.gd:20 LANTERN_H 19.4
- §3u `capture_lorry.gd:6` COPIES: capture_lorry.gd:6 ZONE_INDUSTRIAL 3
- §3u `capture_roads.gd:25-26` COPIES: capture_roads.gd:25-26 FURNITURE_LAMP 1, FURNITURE_COLUMN 6
- §3u `capture_street.gd:142` TRANSLATION SURGERY COPIED INTO A CAPTURE
- §3u `capture_shell.gd:16-18` COPIES: capture_shell.gd:16-18 SPEED_PAUSED 0, SPEED_NORMAL 1, SPEED_FASTEST 3
- §3u `capture_street_views.gd:14` COPIES: capture_street_views.gd:14 ITEM_NAME 11, :15 NAMES_TRIED 312 "three times what one sheet of 104 held"
- §3u `capture_transit.gd:7-8` COPIES: capture_transit.gd:7-8 TRAIN 1, METRO 2
- §3u `capture_vehicles.gd:7-16` THE WHOLE VehicleType ENUM COPIED
- §3u `capture_views.gd:6-7` COPIES: capture_views.gd:6-7 DAY 16, NIGHT 22
- §3u `landscape_cards.gd:17` landscape_cards.gd:17 SHADOW_FILTERS [2, 3, 4, 5]

### C4. one value, two owners (94)

- §3 `pgo.sh` `pgo.sh` names its own Godot path
- §3a `random.h:7-13` Two owners of the same hash
- §3b `massing_core.cpp:17-18` A second owner of the zone ids
- §3b `plan_area.cpp` A second polygon engine
- §3b `types.h:225-228` `Body::add_paint` never deduplicates
- §3c `zones.json` The zone codes have six owners, two of them wrong
- §3c `plan.h:14` Two copies of pi and of a turn
- §3d `random.h` Squares have their own LCG beside `core/random.h`
- §3d `body_finish.cpp:23-26` Two owners of paint de-duplication, none in `Body`
- §3d `body_survey.cpp:55` A second convex hull
- §3e `geometry.cpp:73-91` R10. Geometry primitives written several times
- §3f `junction_control.cpp:218-224` Two rules for the main road through a junction
- §3f `ped_doors.cpp:21-26` Two rules for what kind of place a building is
- §3f `window.cpp:55-64` Two rules for "inside the micro window"
- §3j `ruin_fracture.cpp:20` `ruin_fracture.cpp:20` `LONGEST_SIDE_M = 2.8` repeats `ruin_cells.h:16` `VORONOI_EDGE_M`
- §3l `building_renderer.cpp:25` Constants typed more than once
- §3n `city_core_people.cpp:197-201` `set_city_hour`
- §3p `traffic.json:137` P6. One value, two owners
- §3q `settings.gd:108-111` THE "CUT WALLS" SETTING HAS TWO OWNERS
- §3q `formatters.gd:27-30` formatters.gd:27-30 the decimal comma is chosen by `get_locale().begins_with("pt")` in code while the thousands ...
- §3r `inspector_panel.gd:277` inspector_panel.gd:277 the inspector's refresh period is DataTables.game("health").refresh_s
- §3r `inspector_rows.gd:2` inspector_rows.gd:2 "data/game/health.json is the panels' pace"
- §3r `road_catalog.gd:25-30` A PROFILE'S STRIPS HAVE TWO SOURCES
- §3r `road_options.gd:140-142` THE TWO-OWNER WALLS SETTING WRITTEN TWICE
- §3r `road_options.gd:105-109` road_options.gd:105-109 the block grid's default merge is a copy of road_blocks.gd:7-9
- §3s `change_marks.gd:39` change_marks.gd:39 changes_shown read from data/game/health.json
- §3s `effect_materials.gd:7-9` A SECOND LIST KEPT FOR THE ORPHAN GATE
- §3s `facade_bay.cpp:13` THE NEAR AND MID LEVELS PAINT REVEALS AND SILLS DIFFERENTLY
- §3s `building_far_window.gd` ONE BAY RULE IN C++ AND IN SHADERS
- §3s `building_far.gd` THE SAME SHADER CONSTANTS TWICE OR THRICE
- §3s `building_far.gd` building_far.gdshader:32-33 FOOT_SHADE 0.62 over FOOT_STOREYS 0.47 darkens the foot of every far wall
- §3s `building_card.gd` building_card.gdshader:14 BRISE_PITCH "part_models.cpp BRISE_FINS" and :47-49 railing proportions
- §3s `building_far_window.gd` building_far_window.gdshaderinc:10 FAR_ROOM_LEVEL 0.68 "the mean of pane_lamp's level
- §3s `building_hash.gd` TWO HASH FAMILIES IN THE SHADERS
- §3s `building_pane.gd` A DATA VALUE COPIED INTO A SHADER WITH ANOTHER UNIT
- §3s `building_pane.gd` CONFIRMS THE MID LEVEL IS THE ODD ONE
- §3s `building_pane.gd` PANE ALBEDO vec3(0.0075, 0.0096, 0.0115) typed again at building_pane.gdshaderinc:247 and :259
- §3s `building_parts_body.gd` THE FINISH LOGIC TWICE
- §3s `building_room_tone.gd` DERIVED CONSTANTS TYPED BY HAND
- §3s `building_wash.gd` THE SAME FUNCTION PASTED
- §3s `building_hash.gd` HASHES AND NOISES MULTIPLY
- §3s `people_look.gd` GGX TYPED AGAIN
- §3s `people_far.gd` people_far.gdshader:11 TAU_RAD
- §3s `screen_depth.gd` "ONE OWNER" OF THE DEPTH NORMAL, AND A SECOND ONE
- §3s `materials.json` A DATA VALUE DERIVED BY HAND FROM ANOTHER AND A SHADER CONSTANT
- §3s `signature_look.gd` A RULE COPIED BETWEEN SHADERS
- §3s `terrain_layers.gd` THE SAME CONSTANTS IN TWO INCLUDES
- §3s `vegetation_cards.gd` The mip-level expression is typed twice in vegetation_cards.gdshader
- §3s `water.gd` water.gdshader:75-82 reconstructs view depth by hand
- §3s `sky_view.gd:170-171` STATE READ BACK FROM A MATERIAL
- §3s `sky_view.gd:178-181` HASH COPY: sky_view.gd:178-181 repeats seeded.h:94-95
- §3t `blast.json:28` blast.json:28 people.knock_min_s/knock_span_s 3.2/1.8 and :30 shot.knock_min_s/knock_span_s 3.2/1.8
- §3t `road_profiles_b.json:340-365` SAME SECTION TWICE IN ONE TAB
- §3t THE SAME DATA TWICE
- §3t `traffic.json:133-134` ONE VALUE, TWO KEYS, TWO READERS
- §3t `traffic.json:137` KERB HEIGHT, TWO OWNERS
- §3t `settings.json` WRONG OWNER: settings.json
- §3t `start.json` CURRENCY, TWO OWNERS AND A WRONG SYMBOL
- §3t `hint.sh` SAME TEXT UNDER TWO KEYS
- §3t `game.h` THE SAME HINT THREE TIMES
- §3t `strings_b.csv` TWO KEY FAMILIES FOR THE SAME HINTS
- §3t `strings.csv:419` DUPLICATE: :427 save.blocked repeats strings.csv:419 game.toast.saveBlocked
- §3t SAME NAME, TWO KEYS, TWO ENGLISH WORDS
- §3t `strings.csv:146` DUPLICATE KEYS ACROSS FILES
- §3t `zones.json:3-11` ZONE NAMES, TWO OWNERS, ONE BROKEN
- §3t `strings.csv:328` DUPLICATE: :556 confirm.clearTerrain repeats strings.csv:328 game.confirm.flattenMap
- §3t builder.lot.*
- §3t `strings_e.csv:79-91` TWO KEY SETS FOR THE FINISHES
- §3t :222-229 eight hint.builder.prim* keys hold the same sentence
- §3t `model.h` SAME NAME FOR DIFFERENT MODELS
- §3t `strings_d.csv:102-139` THE SAME 38 BUILDINGS UNDER TWO KEY SETS WITH DIFFERENT NAMES
- §3t `strings_b.csv` :81 hint.openFailed overlaps strings_b.csv save.file.notMap/save.importFailed
- §3t `data_tables.gd:65` THE PLAYED STYLE HAS SIX OWNERS, NONE IN THE STATE
- §3t `buildings.json:134` A VALUE COPIED BECAUSE OF A WALL BETWEEN TABLES
- §3t `buildings.json:133` DERIVED DATA TYPED BY HAND
- §3t `effects.json:3` SMOKE IGNORES THE WIND
- §3t `effects.json:4` ONE LAW, TWO OWNERS
- §3t `daylight.json` DERIVED NUMBERS TYPED BY HAND, ACROSS TABLES
- §3t THE SAME TABLES IN TWO MATERIALS
- §3t `models.json:5-7` ONE MEASURE, TWO OWNERS
- §3t DERIVED NUMBERS TYPED BY HAND
- §3t `agents.json` DERIVED BY HAND
- §3t `street_furniture.json:109` IN-WORLD WORDS IN RENDER DATA
- §3t `traffic.json:241` ONE TRAIN CAR, TWO LENGTHS
- §3t `transit.json:6` transit.json:6 default_line "#2f7de1" repeats traffic.json:3 line_colours[1]
- §3t `rail_run.cpp:185` (readers) traffic train_car_m
- §3t `builder.json:286-298` FLAG COLOURS, THREE OWNERS
- §3t `tokens.json:213` THE ROAD CARDS SHOW ANOTHER ROAD
- §3t `tokens.json:66-69` ZONE COLOURS, TWO OWNERS
- §3t `tokens.json:4` tokens.json:4 and :111 duplicate key "bg_solid"
- §3u `road_grade_test.gd:7-9` TUNING VALUES TYPED INTO TESTS
- §3u `shoot_test.gd:24` shoot_test.gd:24 SHOTS_TO_BURST 14 while blast.json shot.vehicle_shots_to_burst is 12
- §3v `loftBrick.json` loftBrick.json top-level "use":"commercial" vs body.use "mixed"
- §3v `lot_projects.gd:15-20` TWO USE FIELDS THAT DISAGREE

### C5. store API kinds and tool state (14)

- §3d `body_furnish.cpp:75` A query that mutates the body
- §3f `road_variants.cpp:128-160` Road variants are player state stored in the `Catalog`
- §3p `store_api.gd:47-68` P2. `StoreApi` mixes commands and queries and decides the snapshot mark by a name prefix
- §3q `store_api.gd:47-68` COMMANDS AND QUERIES ARE MIXED, SO THE SNAPSHOT GOES STALE
- §3q `road_tool.gd:43-44` road_tool.gd:43-44 the tool reads its own height from the core
- §3q `road_keys.gd:16-17` road_keys.gd:16-17 static var waiting / note
- §3r `road_profile_panel.gd:146` road_profile_panel.gd:146,154 and road_panel.gd:68 road_spec_make
- §3r `road_spec_ops.gd:41-78` road_spec_ops.gd:41-78 where a new element goes
- §3s `sky_view.gd:124-126` A RENDER VALUE THROUGH THE COMMAND PATH, EVERY FRAME AT DUSK
- §3u `people_test.gd:42` A MUTATION THROUGH THE QUERY ROUTE
- §3u `road_money_cases.gd:73` MUTATION THROUGH THE QUERY ROUTE
- §3u `capture_people.gd:49` MUTATION THROUGH THE QUERY ROUTE
- §3u `capture_transit.gd:65` MUTATION THROUGH THE QUERY ROUTE
- §3u `pair_camera.gd:36` MUTATION THROUGH THE QUERY ROUTE

### C6. the lock protocol (22)

- §3a `city_state.h:115-120` One reader-writer lock for the whole city is shared by the frame, the simulation, the commands and the mesh jobs
- §3a `simulation.cpp:130-136` The simulation's only escape from that lock is to skip work
- §3a `render_world.cpp:236-272` The frame's main-thread work is all inside the shared lock
- §3f `signal_commands.cpp:14-17` A signal command reads the traffic's derived graph
- §3f `city_core_strike.cpp:20-21` To verify in `api`
- §3i `mesh_queue.h:70-74` L1. The mesh jobs hold the map lock for their whole build
- §3i `effect_bodies.cpp:118-119` L2. Data race on the walkers
- §3j `building_renderer_probe.cpp:54-58` Data race (resolved in `api`, §3m)
- §3k `building_renderer.cpp:304-305` `building_renderer.cpp:304-305,328` takes `queue_mutex_` per chunk per frame on the main thread
- §3m `body_api.h:43-48` A1. State read outside its lock: the protocol is not enforced
- §3m `city_core_edit.cpp:205` A2. Read-only previews take the exclusive lock
- §3n `city_core_strike.cpp:15-28` `shoot` (`city_core_strike.cpp:15-28`) changes the city outside the journal, then a burst calls the undoable ...
- §3n `city_core.cpp:250-252` `step_ticks`
- §3n `city_core_lanes.cpp:110-118` `toggle_lane_link`
- §3o `city_core_people.cpp:64-87` Probe cost under the locks
- §3p `city_camera.gd:241-259` P5. Core queries under the map lock on every frame and every pointer move
- §3q `city_camera.gd:241-259` THE CAMERA TAKES THE MAP LOCK ABOUT SEVEN TIMES A FRAME
- §3q `inspect_tool.gd:16-17` inspect_tool.gd:16-17 asks `inspect` every pointer move
- §3q `roundabout_tool.gd:28-29` THE EXCLUSIVE-LOCK PREVIEWS RUN ON EVERY POINTER MOVE
- §3q `tool_base.gd:65-70` tool_base.gd:65-70 road_under asks segment_at
- §3q `tool_controller.gd:151` tool_controller.gd:151,159 every mouse event asks the core for a raycast
- §3s `daylight.gd:167` daylight.gd:167 follow_view

### C7. derived data without a cache owner (25)

- §3c `lot_access.cpp:217-240` Parking rebuilds whole building bodies to find car gates
- §3c `likeness.h:28` Lot growth draws four whole bodies per lot
- §3c `lot_land.cpp:223-239` The player's lot snap scans every lot on every pointer move
- §3d `body_pick.cpp:117-118` Root cause shared by many findings: the sim keeps no derived body
- §3e `interior_commands.cpp:17-32` R1. The sim keeps no derived body
- §3e `ped_furniture.cpp:80-95` R3. Street furniture is derived again by every reader
- §3e `lot_cache.cpp:42-72` R4. Lots are derived per 256 m cell from a 576 m window, inside the commands, under the exclusive lock
- §3e `catalog.h:100-104` R8. Catalog constants are recomputed per call
- §3e `traffic_state.h:196` R9. More dirty channels than the `ChangeLog`
- §3i `building_bodies.cpp:61-65` L6. R1 in the render: five private body caches, regenerated bodies
- §3i `tree_place.cpp:74-79` L7. One chunk's ground is placed three times
- §3i `road_junction.cpp:187-207` L8. Shapes derived by two jobs
- §3k `overlay_build.cpp:128-129` `overlay_build.cpp:128-129` each zone chunk job walks every player lot of the city
- §3k `fog_field.cpp:141-144` `fog_field.cpp:141-144` re-buckets every dab ever painted on each fog revision
- §3k `road_verge.cpp:42-43` `road_verge.cpp:42-43` samples the ground three times per ribbon vertex
- §3m `body_api.h:37-41` A7. R1 and R2 reach the API
- §3q `data_tables.gd:157-162` data_tables.gd:157-162 road_profiles() parses three files again on every call
- §3q `frame_meter.gd:166-168` frame_meter.gd:166-168 render_cpu_ms asks render_cost every recorded frame, and cpu_watch.gd:26 again
- §3q `building_tool.gd:71-82` PER-POINTER-MOVE CORE CALLS
- §3q `furnish_hand.gd:86-92` THE INTERIOR HAND RUNS TWO GENERATORS PER POINTER MOVE
- §3q `lots_tool.gd:184-199` PER-POINTER-MOVE CORE CALLS
- §3q `pole_tool.gd:77-78` pole_tool.gd:77-78 release plans the run twice
- §3q `road_tool.gd:215` THE ROAD DRAG REBUILDS THE WHOLE PROFILE LIST EVERY MOVE
- §3q `upgrade_tool.gd:12` upgrade_tool.gd:12 asks the whole road_profiles list for one width
- §3s `city_world.gd:49` DataTables.render()

### C8. buildings filed as points (8)

- §3e `building_store.cpp:80` R2. `BuildingStore` files each building as a point
- §3i `canyon_field.cpp:27` L10. R2's guessed pads in the render
- §3n `city_core_sight.cpp:132` The camera does not see most buildings
- §3n `city_core_inspect.cpp:113-122` The inspector names the building by its centre
- §3n `city_core_clearance_probe.cpp:81-82` `lot_clearance` cannot fail on long ways
- §3q `camera_ahead.gd:24-26` camera_ahead.gd:24-26,33,74 the deocclusion stands on api eye_free, which R2 breaks
- §3q `camera_pull.gd:31` camera_pull.gd:31 sight_pull is also broken by R2
- §3q `interior_controller.gd:142-147` interior_controller.gd:142-147 _building_at asks api `inspect`

### C9. journal and damage identity (5)

- §3e `damage_state.cpp:67-71` R5. Damage is keyed by place, not by building id, and part of it lives outside the journal
- §3e `nature_commands.cpp` R6. The edit journal mixes data and opaque closures
- §3e `save_nature.cpp:54-58` R7. The state hash is not a function of the state
- §3f `wounds.cpp:22` Wound events bypass the event cap
- §3u `edit_fuzz_test.gd:28-29` A LOOSENED CHECK ON UNDO

### C10. errors abort, loaders lax (9)

- §3j `people_renderer.cpp:65` `people_renderer.cpp:65` `acts.set(asset_.clip(f.clip), f.act)` with no check for -1
- §3j `transit_parts.h:52` `transit_parts.h:52` `part_of.at(name)` and `transit_parts.cpp:104,116,118` ask for literal names `read()`
- §3j `effect_params.h:1-2` `effect_params.h:1-2` says missing keys are reported
- §3m `packed_arrays.cpp:220-228` A3. Loaders are hand-written, inconsistent, and some crash on bad data
- §3n `city_core_persist.cpp:39-49` A crash between the two renames of `write_atomic`
- §3n `city_lab.cpp:31-43` `rules_of` (`lab/city_lab.cpp:31-43`) reads keys without `has()`
- §3v `body_doc.cpp:94-95` body_doc.cpp:94-95 std::stoi on document text throws std::invalid_argument/out_of_range on a malformed key like ...
- §3v `body_doc.cpp:94-95` body_doc.cpp:94-95 std::stoi with -fno-exceptions
- §3v `pinkCorner.json` pinkCorner.json volume 4 open "grass"

### C11. colour pipeline (6)

- §3j `tree_parts.cpp:146` `tree_parts.cpp:146,160,163-164,180-182` average the far card tone in sRGB though `tree_parts.h:20-21` says tones ...
- §3p `color_space.gd` P7. Three colour rules in the shaders
- §3s `building_colour.gd` TWO COLOUR-SPACE INCLUDES THAT DISAGREE ON COMPATIBILITY
- §3s `overlay.gd` THE MAP OVERLAY IS TONE-MAPPED THOUGH IT SAYS IT SHOWS SCREEN COLOURS
- §3s `vegetation.gd` A THIRD COLOUR RULE, AND A FALSE CITATION
- §3s `vegetation_cards.gd` TWO COLOUR RULES IN ONE SHADER

### C12. pipeline structure (14)

- §3a `city_state.h:12` `CityState` includes the commands layer
- §3a `render_world.h:168-209` `RenderWorld` is a façade of eighteen renderers and three fields
- §3i `transit_renderer.cpp:213-227` L3. Transit meshes are built on the main thread
- §3i `chunk_jobs.h:125-194` L4. Two result protocols for one pipeline
- §3i `detail.h:1-5` L5. The single owner of level of detail is dead code
- §3i `building_renderer.h:41-251` L9. God classes split across files
- §3i `frame_admission.h:93-97` L12. The frame's upload flush rests on an engine internal
- §3m `city_core.cpp:79-83` A5. The sim's data loads inside the render setup, from render files
- §3m `city_core.h:30-228` A6. The facade is a god class with its split half done
- §3n `city_core_persist.cpp:109-136` Opening a city freezes the frame
- §3q `city_saves.gd:81-92` city_saves.gd:81-92 load_game calls the core's load_city, which decodes on the main thread
- §3s `shore_view.gd:16` TWO PROTOCOLS FOR A REVISIONED FIELD
- §3s `sky_view.gd:146-147` SKY VIEW OWNS THINGS THAT ARE NOT THE SKY
- §3w `city_core_jobs.cpp:27-30` JOB_SEM_CHAVE 21

### C13. dead code, data, text and stale documents (88)

- §3h `road_network.cpp:16` `road_network.cpp:16` names `LOT_MARGIN`
- §3h `road_network.h:57-63` Comments above the wrong declaration
- §3h `kerb_stops.cpp:25-27` Empty anonymous namespaces left by splits
- §3h `nature_commands.cpp:58` Misnamed: `dab_chunk` is a distance
- §3h `junction.cpp:2` `junction.cpp:2,6` includes `core/math.h` twice
- §3h `city_state.h:83` A parked feature in a comment
- §3h `parking_sources.cpp:182-195` Probe-only code inside `sim/traffic`
- §3l `road_renderer.cpp:93` Dead code: `road_renderer.cpp:93` `update_view`
- §3l `building_renderer.cpp:2` Stale or misplaced comments
- §3l `region_table.h:1-2` Head comment `region_table.h:1-2` cites a Portuguese process file and parks a feature
- §3o `catalog_loader.cpp:79` Parked features in code
- §3o `city_core.h:45` Misplaced or stale comments
- §3p `toolbar.json` P8. Dead data and text kept alive
- §3q `road_tool.gd:12` road_tool.gd:12 the comment "A long stroke keeps every sample up to DENSE_SAMPLES, then one a metre, up to ...
- §3q `tool_controller.gd:3-4` tool_controller.gd:3-4 the header's list of tools lacks LOTS
- §3r `hud_top_bar.gd:112` hud_top_bar.gd:112 the stats tip's "speed" is always "—"
- §3r `inspector_panel.gd:4` inspector_panel.gd:4 parks features in a comment
- §3s `contact.gd` contact.gdshader:4-5 "Godot 4.8 has this built in
- §3s `road_body.gd` road_body.gdshaderinc:18-19 "Mean colour of each map, computed once in project/render/city_world.gd"
- §3s `water.gd` FALSE HEADER
- §3s `sign_sheet.gd:17` TEST DATA IN THE GAME
- §3t `nature.json:2` STALE PATH: nature.json:2 and :109 cite "ports/landscape/research.md"
- §3t `maps.json` maps.json: no _doc
- §3t `growth.json:49-50` growth.json:49-50 decay_grace_min/decay_span_min
- §3t `road_profiles.json:127` UNREACHABLE PROFILES
- §3t `streets.json:351` STALE DOC: :173 "height_m
- §3t `pendencias.md` PLACEHOLDERS
- §3t `data_tables.gd:63` :169 and data_tables.gd:63 cite "session-rules.md, section 10"
- §3t DEAD DATA BY ITS OWN WORDS
- §3t `demand.cpp:63` UNDOCUMENTED
- §3t `sim.md` STALE REFERENCE
- §3t :4 _doc is one line of about 6,000 characters
- §3t `zones.json` zones.json: no _doc
- §3t `render_switches.json:2` STALE: render_switches.json:2 "Switches of systems the game does not have yet
- §3t `settings.json` A SETTING WITH NO EFFECT
- §3t `settings.json:2` A BROWSER CONCEPT
- §3t `start.json:35-38` start.json:35-38 sizes town and metropolis only
- §3t `strings.csv:234` BROWSER TEXT IN A DESKTOP GAME
- §3t A TEST TOOL AS A PLAYER OPTION
- §3t The file is not sorted
- §3t `strings.csv:434` BROWSER AND JAVASCRIPT TEXT
- §3t BROWSER TEXT AGAIN
- §3t `nature.json` ORPHAN CANDIDATE
- §3t :357 hint.road.clearance sits among refuse.*
- §3t BROWSER TABS IN A DESKTOP GAME
- §3t :391 road.section.title "· experimental"
- §3t `strings_c.csv:138` WRONG CREDITS AND LICENCES
- §3t `jogar.sh` WEB RUNTIME TEXT
- §3t `start.json` WRONG TEXT: :298 ui.start.sub "cidades inteiras de 10 a 24 km" while start.json offers town and metropolis only
- §3t KEYS NAMED BY VERSION
- §3t :228 ui.menu.testCity "Carregar o pior caso
- §3t `strings.csv` 905 of 2,559 keys
- §3t `maps.json:4-7` maps.json:4-7 name_key menu.size.km10/16/20/24 have no translation
- §3t Key naming mixes conventions
- §3t TWO PLACEHOLDER CONVENTIONS
- §3t `world.md` STALE REFERENCE
- §3t `materials.json:2` DOC AGAINST VALUE
- §3t `palette.json:2-11` DEAD DATA: palette.json:2-11 vehicle_tints, :12-21 person_tints, :22-28 foliage_round and :60-64 sky are read by no code
- §3t `palette.json:116` CORRUPTED DOC
- §3t `materials.json` STALE REFERENCE
- §3t Layout: :52 two keys on a line, :87 mis-indented, upper- and lower-case hex mixed
- §3t Layout: :28 a tab among spaces
- §3t `streets.json:317` DEAD DATA KEPT FOR A SCHEMA
- §3t `streets.json:482-483` streets.json:482-483 eua/europa placeholders again
- §3t `maps.json` :236 large_maps lists "km24", a map no screen offers and whose name has no translation
- §3t `ui.md` (ui.md) drawers.json:2 "specs ui.md 4 and 6"
- §3t `toolbar.json` A DEAD TABLE WITH A LIVE SCHEMA
- §3u `yard_test.gd:19` STALE COMMENT ON DETERMINISM
- §3u `yard_bench.gd:4` A TOOL AMONG THE TESTS
- §3u `main_thread_probe.gd:112-122` SHIMS FOR VERSIONS THAT ARE GONE
- §3u `pending_probe.gd:1` INVESTIGATION PROBES KEPT IN THE TREE
- §3u `capture.gd:2-3` STALE REFERENCE
- §3u `capture_features.gd:4` STALE RUN NOTE
- §3u `capture_roads.gd:2-3` SHIMS FOR OLD BUILDS
- §3u `capture_roads.gd:199` MISPLACED DOC
- §3u `capture_shell.gd:199-200` MISPLACED DOC
- §3u `capture_street_views.gd:36` A DOC COMMENT INSIDE ANOTHER FUNCTION
- §3u `capture_street_views.gd:110` TEST-ONLY STATE IN THE GAME
- §3w `capture_game.gd` SIDECAR_WITHOUT_SOURCE
- §3w `rules.json:273-280` rules.json:273-280 TEXTO_VIA_TR "pattern" is dead text
- §3w `roads.schema.json` schemas: roads.schema.json and services.schema.json are orphans from the base commit
- §3w `CLAUDE.md` CLAUDE.md lists sampler.py and pgo.sh among the commands while its "Execução e verificação" says "Sem medições, ...
- §3w `gates_dispatch.py` git status: tools/process/tools/hooks/gates_dispatch.py tracked but deleted in the working tree
- §4 `mapa.md` `log/mapa.md` has no line for `tools/tasks/**`
- §4 `work-log.md:561` `log/work-log.md:561`
- §4 `CLAUDE.md` `tools/process/CLAUDE.md` describes another layout
- §4 The memory `no-reference-game` says the source game is "not a reference at all"
- §4 The worktree holds a large uncommitted state

### C14. tunables and look values in code (39)

- §3a `math.h:80` Whole core is outside the magic-number gate
- §3b `civic.cpp:17-29` Hundreds of tunable design values in code, not in data
- §3c `nature_types.h:73-77` Regional facts fixed in code
- §3c `tuning_fields.h:47` Every tunable is a float
- §3c `traffic_params.h:11-63` One flat struct of about 500 traffic numbers
- §3c `interior_homes.cpp:13-32` Interior layout numbers in code
- §3d `body_survey.cpp:19-26` Survey numbers in code
- §3h `ecology.cpp:58-121` Data in code
- §3l `facade_bay.cpp:12-20` Look numbers and colours in code
- §3o `city_core_commands.cpp:22` Bare numbers and local constants
- §3q `camera_gestures.gd:128` camera_gestures.gd:128 four statements on one line with `;`
- §3q `brush_tool.gd:8-11` brush_tool.gd:8-11 STEP_SHARE, MIN_STEP_M, MAX_STEPS
- §3q `building_tool.gd:89` building_tool.gd:89 the plain quarter turn is PI / 2 in code while the Shift turn is data
- §3q `handle_drag.gd:6` handle_drag.gd:6 TURN_STEP = PI / 12
- §3q `lots_tool.gd:11-16` lots_tool.gd:11-16 TAKE_PX, CATCH_PX, SNAP_LEAST_M, MARK_M
- §3q `roundabout_tool.gd:7` roundabout_tool.gd:7 SCALE_STEP = 1.2
- §3q `terrain_tool.gd:8` terrain_tool.gd:8 DAB_S = 0.1, zone_tool.gd:6,9 DAB_SPACING, MOST_DABS, :15,22 zone 1 by default
- §3r `builder_strip.gd:6-7` builder_strip.gd:6-7 CHOICES
- §3r `pop_bodies.gd:8` pop_bodies.gd:8 KEY_SHARE = 0.42
- §3r `road_panel.gd:7-17` GAME RULES DECIDED IN THE INTERFACE
- §3r `CLAUDE.md` SIZES IN INTERFACE CODE
- §3r `start_screen.gd:10` start_screen.gd:10 LANGUAGES
- §3r `zone_strip.gd:6` zone_strip.gd:6 DENSITIES
- §3s `city_water.gd:8` city_water.gd:8 WATER_EXTENT_M = 70000
- §3s `CLAUDE.md` Magic numbers in shaders
- §3s `road_flash.gd:5-6` road_flash.gd:5-6 GLOW_IN_S, DIE_OUT_S
- §3s `atmosphere.gd` LOOK VALUES IN SHADER CODE
- §3s `building_pane.gd` building_pane.gdshaderinc:97-106, 119, 124, 127, 136, 138, 185, 188
- §3s `lightning.gd` lightning.gdshader:7 screen default vec2(1600, 900)
- §3s `puffs.gd` puffs.gdshader:53 and :57 two gravities in one shader
- §3s `verge.gd` verge.gdshader:40 ROUGHNESS mix(0.95, 0.6, rain_wet) typed
- §3s `weather_view.gd:83` weather_view.gd:83 `dt * rate / 60.0`
- §3t `health.json` A DATA VALUE TYPED IN TEXT
- §3t DATA VALUES TYPED IN TEXT
- §3t `road.bend.sh` DATA VALUES TYPED IN TEXT
- §3t DATA VALUES IN TEXT
- §3t `strings_c.csv:540` A DATA VALUE IN TEXT, BESIDE A KEY THAT FILLS IT
- §3t `strings_ui.csv:7` WRONG NUMBERS TYPED IN TEXT
- §3v `road_body.gd` road_body.gdshaderinc:158 `uv / asphalt_m * 2.7` magic number in a shader

### C15. Portuguese text (63)

- §3l `render_context.h:30` Portuguese error text
- §3o `blast_loader.cpp:18` Portuguese error text
- §3p `data_tables.gd:92` P10. Portuguese text in the repository
- §3q `data_tables.gd:63` data_tables.gd:63 cites "session-rules.md, section 10"
- §3q `data_tables.gd:92` data_tables.gd:92, :210, :222 and game_flow.gd:49 errors in Portuguese
- §3q `interior_controller.gd:41` interior_controller.gd:41 warning in Portuguese
- §3q `play.gd:18` play.gd:18 error in Portuguese
- §3q `city_saves.gd:54-56` city_saves.gd:54-56 map copies are named "mapa-<seed>"
- §3q `save_slots.gd:11-19` PORTUGUESE FILE NAMES AND EXTENSIONS
- §3q `city_store.gd:54` city_store.gd:54,93,103 assert messages in Portuguese
- §3q `road_models.gd:1` road_models.gd:1 "(Meus modelos)", road_spec_edit.gd:1 "Personalizar..."
- §3r `CLAUDE.md` TEXT SHOWN WITHOUT tr()
- §3r `hud_popover.gd:129-131` SAVE FILE NAMES ARE SHOWN TO THE PLAYER
- §3r `hud_popover.gd:240` hud_popover.gd:240 "X %d · Y %d" written without tr()
- §3r `hud_widgets.gd:37` hud_widgets.gd:37 comment "<name> · em construção"
- §3r `lab_views.gd:69-72` lab_views.gd:69-72 the inspector tab shows raw dictionary keys as labels
- §3r `profiler_hud.gd:43` profiler_hud.gd:43,49 English text written without tr()
- §3t `nature.json:2` nature.json:2 "compared with reference-minha photos"
- §3t `drawers.json:147-170` PORTUGUESE IDS IN DATA
- §3t `data_tables.gd:93` data_tables.gd:93 push_error("JSON inválido
- §3t UNTRANSLATED
- §3t :422 game.toast.stationOnTrack
- §3t English terms for one thing vary
- §3t :150 hint.builder.extrude pt_BR "o handle do topo"
- §3t English uses "blueprint"
- §3t "sobrado" (:61)
- §3u `building_edit_test.gd:15` PORTUGUESE IN TESTS
- §3u `building_light_test.gd:45` PORTUGUESE: building_light_test.gd:45 "renderizador:", :51 "parede perto", "parede longe"
- §3u `building_plan_cases.gd:31` PORTUGUESE: building_plan_cases.gd:31 "forma %d", :46 "paredes levadas", :54 "relevo", :64 "recuo", :68 "ala", :88 ...
- §3u `camera_input_test.gd:78` PORTUGUESE: camera_input_test.gd:78 "Shift+direito", "meio"
- §3u `flow_control_test.gd:60` PORTUGUESE: flow_control_test.gd:60 "rodando:", :86 "empurrado:"
- §3u `inspector_test.gd:41` PORTUGUESE: inspector_test.gd:41 "falhas", :47 "cidade-medida", :55 "save medido", :96 "volume conta entradas", :123 ...
- §3u `light_color_test.gd:60` PORTUGUESE: light_color_test.gd:60 "renderizador:", :89 "lente %d", :123 "brilho noturno"
- §3u `model_test.gd:80` PORTUGUESE: model_test.gd:80 "igreja:", :165 "Minha casa", :180 "palco:"
- §3u `quality_test.gd:54` PORTUGUESE: quality_test.gd:54 "falhas", :172 "camada Grade liga", :212/:217 "nivel_antigo"
- §3u PORTUGUESE: :50 "construir:", :54 "segmentos", :62 "zona:", :63 "zoneou lados", :76 "cresceram:", :80 "demolir:", ...
- §3u `road_edit_test.gd:77` PORTUGUESE: road_edit_test.gd:77 "largura total"
- §3u `road_grade_test.gd:200` PORTUGUESE: road_grade_test.gd:200 "sozinha"
- §3u `road_money_cases.gd:143` PORTUGUESE: road_money_cases.gd:143 "perfil mais barato devolveu", :146 "perfil mais caro cobrou", :163 "melhorar", ...
- §3u `save_undo_test.gd:26` PORTUGUESE: save_undo_test.gd:26 SLOT "teste_ida_e_volta", :69-71 "melhorar", "dividir", "demolir", :84 "relevo ...
- §3u `shoot_fright_cases.gd:43` PORTUGUESE: shoot_fright_cases.gd:43 "perto", :55 "de"
- §3u `street_save_cases.gd:8` PORTUGUESE: street_save_cases.gd:8 SLOT "teste_rua", :25 "contagens:", :32 "abrir", :35 "save guarda"
- §3u `strike_test.gd:99` PORTUGUESE: strike_test.gd:99 "1º golpe", :100 "golpe aceito", :102 "golpe contado", :108 "golpes", :115 "golpe ...
- §3u `yard_bench.gd:20` PORTUGUESE: yard_bench.gd:20 SLOT "bancada_patios", :71 "docas"
- §3u `camera_pan.gd:144-159` PORTUGUESE: camera_pan.gd:144-159 movement names "parada", "cruzar", "circular", "girar", "perto", :141 "malhas:", ...
- §3u `pending_probe.gd:81` PORTUGUESE: pending_probe.gd:81 "como configurado", :89 "agentes:", :98/:112 "PAUSA_INICIO"/"PAUSA_FIM", :143-144 ...
- §3u `capture.gd:27-28` PORTUGUESE (capture file names are Portuguese throughout)
- §3u `capture_demolition.gd` PORTUGUESE: capture_demolition.gd phases "pareada", "torre", "derrubados", "topo", files "bomba_f%d", ...
- §3u `capture_flag.gd:36` PORTUGUESE: capture_flag.gd:36 "feat_build_bandeira"
- §3u `capture_lorry.gd:51` PORTUGUESE: capture_lorry.gd:51 "passo", :54 "36_doca_%d", key "doca"
- §3u `capture_people.gd:13-15` PORTUGUESE: capture_people.gd:13-15 "30_casal", "31_familia", "32_crianca", "33_idoso", "34_saindo", "35_entrando", ...
- §3u `capture_shell.gd:106` PORTUGUESE: capture_shell.gd:106 "fotos:", :214 "hora"
- §3u `capture_street_gestures.gd:2` PORTUGUESE: capture_street_gestures.gd:2 "Mobiliário", :55 "rua_08_conjunto_via", :78 "barreira", :81 "rua_06_barreiras"
- §3u `capture_traffic.gd:9-10` PORTUGUESE: capture_traffic.gd:9-10 "30_vaga_rua", "31_vaga_lote", "32_carro_manobra", "33_bicicleta", ...
- §3u `capture_ui.gd:64-97` PORTUGUESE: capture_ui.gd:64-97 "ui_01_gaveta_vias", "ui_02_previa_via", "ui_03_via_construida", ...
- §3u `landscape_cards.gd` PORTUGUESE: landscape_cards.gd shot names "rampa", "cubo_a", "cubo_b", "trama_%d_%d", "lamina_vazia", "lamina", ...
- §3u `ui_driver.gd:3` PORTUGUESE: ui_driver.gd:3 "passo
- §3v `body_doc.cpp:164-213` PORTUGUESE: body_doc.cpp:164-213 error labels "telhado", "linha", "coroa", "fachada", "peça", "uso", "função"
- §3v `body_doc.cpp:68` body_doc.cpp:68 error text Portuguese
- §3w `rules.json` T6. Portuguese in tools the gates do not reach
- §3w Gate messages and ids in Portuguese
- §3w `rules.json` rules.json messages, ids, map markers
- §3w `rules.json` All 18 code files are Portuguese

### C16. tools (32)

- §3 `pool.py:249-250` The orchestrator re-reads git after every task
- §3 `rules.json` The line limits push splitting by size
- §3q `CLAUDE.md` FILES SPLIT TO FIT A SIZE
- §3t `road_profiles.json` The profile list is split over road_profiles.json, _a and _b for size
- §3t Three files in three layouts
- §3t `data_tables.gd:69-76` The table is split into six files
- §3t `furniture.json:56` GENERATED FILE OUT OF STEP WITH ITS GENERATOR
- §3t `make_pole_parts.py` GENERATOR NOISE
- §3t `land_elements.json` land_elements.json exists only because land_panels was split for size
- §3v GENERATOR NOISE
- §3w `furniture.json` T3. Generated data is out of step with its generators, and tools edit their own inputs
- §3w `CLAUDE.md` T4. Tool infrastructure has no single owner
- §3w `export_builder_ui.py:61` T5. The line-count limits drive splits for size
- §3w `rename_english.py` T7. One-shot and stale tools kept
- §3w `building_renderer.cpp` TAMANHO_ARQUIVO 19 / TAMANHO_FUNCAO 4
- §3w `gate_data.py:8-46` TWO JSON-SCHEMA CHECKERS
- §3w `CLAUDE.md` A one-shot migration
- §3w `bleed_alpha.py` bleed_alpha.py edits elements/Leaves.png in place
- §3w `make_furniture.py:31-32` make_furniture.py:31-32 writes "cab" then a single "step"
- §3w `road_profiles_b.json` DATA OUT OF STEP WITH ITS GENERATOR, AND THE GENERATOR WOULD BREAK THE GAME
- §3w `CLAUDE.md` :142-143 the catalogue split in three files "a data file stays under the gate's line limit"
- §3w `lex_c.py` FOUR C++ PARSERS IN tools/
- §3w `detectors.py` causes.py/detectors.py
- §3w `sampler.py:125` sampler.py:125 "the first thread seen
- §3w `scope.py:19` No timeout: tools/gates/scope.py:19
- §3w `cpp.py` arch/adapters/cpp.py parses with libclang
- §3w `registry.py` Design: libclang + tree-sitter facts, call graph, slices per entry, seven defect classes each proven by a gold item ...
- §3w WRITTEN FOR ANOTHER REPOSITORY
- §3w `process.py:1-9` tools/process/tools/process/process.py:1-9 is a Claude Code hook
- §3w tools/converter has its own .git AND its 219 files are tracked by the game repository as plain files
- §3w tools/converter-slice
- §3w `research.md` tools/ports: 194 tracked files

### C17. assets and packaging (14)

- §3v STRAY OUTPUT INSIDE THE GODOT PROJECT
- §3v elements/*.gltf
- §3v `materials.json:8-29` Import settings
- §3v `project.godot:24` project.godot:24 gui/timers/tooltip_delay_sec=3600.0
- §3v `project.godot:32-101` project.godot:32-101 shader_globals carry tuning defaults
- §3v `project.godot:28-29` project.godot:28-29 rendering_method.mobile forced to forward_plus
- §3v `citycore.gd` bin/citycore.gdextension:3 compatibility_minimum = "4.5" while godot-cpp is the 4.7 API
- §3v `citycore.gd` bin/citycore.gdextension:9 linux.debug.x86_64 -> template_release .so
- §3v `export_presets.cfg` export_presets.cfg
- §3v `.gd` project/dist/rq/bomb45/... 145 untracked capture files
- §3v `plugin.cfg` Upstream Terrain3D 1.1.0
- §3v `project.godot` project.godot has no [editor_plugins] section
- §3v `export_plugin.gd` bin/ ships 14 binaries for platforms the game never targets
- §3v `terrain.gd` Fix: keep terrain.gdextension

### C18. lot unit scale (6)

- §3p `body_doc.cpp:159-162` P11. The lot documents are in the source's 0.4 m unit and are read as metres
- §3v `body_doc.cpp` READ AT THE WRONG SCALE
- §3v beauxArts: groundHeight 14.0, storeyHeight 9.25, no elements, no lotPlan. beigeClassic 11.25/7.75. Same 0.4 m unit
- §3v `models.json` Census of all 38 lots/*.json
- §3v `body_doc.cpp:159-162` Scale chain re-verified end to end
- §3v `city_core_models.cpp:64-67` city_core_models.cpp:64-67 doc_body clamps storey_h/ground_h to build.max_storey_m 9 / max_ground_m 20, comment ...

### C19. local wrong results (38)

- §3a `slot_ids.h:10` `SlotIds` sits outside `namespace city`
- §3a `polygon_ops.cpp:24` Plans cannot hold holes, so the boolean cuts them and gives up after four cuts
- §3b `seeded.h:42` Palette index off by one, hidden by a modulo
- §3d `squares.cpp:156` Generated squares are stored as player edits
- §3d `body_mass.cpp:279-302` Removing a block points the entrance at the wrong block
- §3f `road_copies.cpp:56-62` `duplicate_road` does not make a parallel road
- §3f `lane_graph.cpp:198` Bicycles may use every road, motorways included
- §3f `lane_graph.cpp:196` Lanes past 4 a direction are drawn but not driven
- §3f `transit_plan.cpp:37` A bus line whose first stop is a terminal starts its new buses at a later terminal
- §3f `terrain_brush.cpp:70-73` The smooth brush reads what it just wrote
- §3f `vehicle_motion.cpp:161-167` A route retry stalls a whole lane for one tick
- §3f `flow_control.cpp:62` Two `count_entry` overloads with different meanings
- §3f `road_grade_solve.cpp:184-195` The grade solver hides non-convergence
- §3j `facade_props.cpp:199` The old-building look of balconies never shows
- §3j `placed.cpp:47-56` Shaped blocks hide neighbours by their box
- §3j `roof_util.h:11-15` `built_on` (`roof_util.h:11-15`) tests `o.base == v.top()` only, not that `o` stands over `v`
- §3j `facade_emit.cpp:221-237` The bay census counts other bays than the drawn ones
- §3j `shell.cpp:276-280` `shell.cpp:276-280` `bytes()` counts 4 custom arrays per vertex when `rooms`, though the far shell uploads 1
- §3j `agent_meshes.h:70` `agent_meshes.h:70` `CabinLook` carries `mutable std::vector<SeatSpot> *spots`
- §3j `heavy_meshes.cpp:84` `heavy_meshes.cpp:84` the deflector side quad repeats a corner
- §3j `mounds.cpp:97` `mounds.cpp:97` heaps past `MOUND_SLOTS`
- §3j `sign_atlas.cpp:54-66` `sign_atlas.cpp:54-66` cells are only added until a new city
- §3j `agent_parts.cpp:265` Plausible, not certain
- §3n `city_core_lots.cpp:76-91` Lots on a curved street are squared to its chord
- §3n `city_core_street.cpp:64` The barrier preview allows every refusal but one
- §3n `city_core_body.cpp:137` `body_mass_rect` indexes `items[id]` with no range check
- §3q `frame_governor.gd:12-15` frame_governor.gd:12-15 _at_percentile indexes sorted[floor(size * p)]
- §3q `formatters.gd:105` formatters.gd:105 key "STAT_DATE"
- §3q `furnish_hand.gd:131` furnish_hand.gd:131 and handle_drag.gd's callers
- §3q `road_blocks.gd:13-19` road_blocks.gd:13-19 takes `tool
- §3r `connector_panel.gd:89-90` connector_panel.gd:89-90 after toggle_lane_link the panel re-reads the lanes after a fixed delay
- §3r `model_shelf.gd:171` A SAVED MODEL IS ANNOUNCED AS A REFUSAL
- §3s `model_studio.gd:16` model_studio.gd:16 SETTLE_FRAMES = 3 "(measured
- §3s `reference_model.gd:183-184` reference_model.gd:183-184 DRAWN_FRAMES = 2 and model_studio's 3
- §3s `thunder.gd:59-63` WRONG SOUND ON OVERLAPPING STRIKES
- §3t `road_catalog.gd:31` UNDEFINED ORDER
- §3t `game.h` :358 game.hint.pistol "ela cai e some"
- §3t `furniture.json:25` furniture.json:25 "pewter"

### C20. local algorithm and allocation choices (30)

- §3 `pool.py:88-97` Small quadratic steps in the scheduler
- §3a `grid.h:35-91` The spatial hash is not flat
- §3a `math.h:5-9` `std::floor` where the project's own `ifloor` exists
- §3a `job_pool.cpp:126-143` Job priorities go stale
- §3a `job_pool.cpp:159` Nested `parallel_for` runs serially
- §3a `simulation.cpp:114-116` All traffic runs on one thread
- §3b `types.h:120-122` The body's per-bay data are three hash maps per volume
- §3b `data.h:84-88` Named colours looked up by string, per element, per body
- §3c `lot_gen.h:58-61` The ground of every lot sample through `std::function`
- §3g `traffic_system.cpp:153-188` Per tick: `publish_poses` walks the whole fleet
- §3g `road_network_deck.cpp:9-61` Per pointer move
- §3g `pedestrians.cpp:247-254` Per edit or blow
- §3g `ground_paint.cpp:55-60` Per sample: `GroundPaint::cell` a hash find per sample
- §3g `road_rules.cpp:26-36` Smaller: allocations per call in `wanted_retreat`
- §3k `overlay_renderer.cpp:107-141` The congestion layer re-uploads whole meshes to change colours
- §3k `terrain_bridge.cpp:76-121` Terrain: `terrain_bridge.cpp:76-121` re-sends each touched region whole
- §3k `water_renderer.cpp:98-109` Water: `water_renderer.cpp:98-109` builds and keeps every wet chunk of the map at any distance, one free-standing ...
- §3k `rs_util.h:101-104` `rs_util.h:101-104` `MultiMeshDraw::set(PackedFloat32Array)` copies the whole buffer whenever the count is below the ...
- §3k `body_renderer.cpp:226` `body_renderer.cpp:226` and `ruin_pieces.cpp:215-216` make a new `Image` of the whole buffer every frame something moves
- §3k `fleet_shadow.cpp:61-67` `fleet_shadow.cpp:61-67` frees a sector's MultiMesh after one empty frame and creates it again
- §3k `agent_renderer.cpp:263` `agent_renderer.cpp:263` rolls the odometer of every published vehicle before the distance cull
- §3k `wear_field.cpp:104` `wear_field.cpp:104,107` two engine calls per wheel and foot sample
- §3k `pane_rooms.cpp:85` `pane_rooms.cpp:85` copies the floor's partitions per pane
- §3r `selection_overlay.gd:66-68` selection_overlay.gd:66-68 redraws every frame while anything is picked
- §3s `element_effects.gd:46-73` element_effects.gd:46-73 a new ParticleProcessMaterial, QuadMesh and StandardMaterial3D each time an emitter changes ...
- §3s `handle_marks.gd:22-24` handle_marks.gd:22-24 redraws every frame while any handle exists
- §3s `agents.gd` THE SIN HASH
- §3s `atmosphere.gd` atmosphere.gdshader:177-187 the clouds are taken nearest first by scanning all 24 for each pass
- §3s `water.gd` water.gdshader:58, :71, :98 normalize(vec2(wind_x, wind_z)) three times per pixel although sky_view.gd:117-118 ...
- §3s `sign_sheet.gd:76` GPU READBACK

### N. notes, not defects (14)

- §3s `cook_people.py:1-2` (Settled in step 8, cook_people.py:1-2 and LICENSES.json
- §3s `sign_atlas.h:2` (sign_atlas.h:2 layers only advance, so sign_sheet.gd:64's "kept only for the layer begun" is right.)
- §3t `agents.json:2` (agents.json:2 agrees with cook_people.py
- §3u `light_color_test.gd:3-4` (light_color_test.gd:3-4 checks render/tone_map.gd's hand mirror of ACES against the engine
- §3u `model_library.gd:6-12` (model_test's ModelLibrary removals are safe
- §3v `override.cfg` override.cfg is local
- §3w `gate_python.py` gate_python.py
- §3w `selftest.py` selftest.py: every rule has a passing and a failing fixture
- §3w Otherwise sound
- §3w `compress_atlas.gd` compress_atlas.gd
- §3w `make_blue_noise.py` make_blue_noise.py
- §3w `make_lattice_noise.py` make_lattice_noise.py
- §3w `repo.py:22` arch/repo.py:22 reads the commit, never the working tree, so it does not see uncommitted work
- §3w `gold.json` arch_eval/gold.json
