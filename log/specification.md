# Specification: what the game must reach, what constrains it, and what is still not true

Opened 2026-11-11 (night) at the owner's order "Pausa obrigatória: levantar, pesquisar, provar e corrigir". CLAUDE.md holds the
rules, the structure and the commands and is not repeated here; `log/pendencias.md` holds every open item; `log/research.md`
holds every research with its proof. This page is written as things are found: an entry without evidence says so.

## Where the three phases stand

| Phase | State | Last update |
|---|---|---|
| 1. Survey (records read, claims checked in the code, pending items listed) | in progress | 2026-11-11 night |
| 2. Research until each question has a proven solution | not started | |
| 3. Fixes, one theme each | not started (no new game code before phases 1 and 2) | |

Survey progress (each line is ticked only with what was actually read or run):

- [x] CLAUDE.md (in the session's context), `log/how-we-work.md` read whole.
- [x] `log/live-instance-design.md` read whole and held against the tree (see "Development tools" below).
- [x] `log/pendencias.md` read whole and restructured (session 2: 50 items with symptom, cause, tried, criterion; the older prose carried below them).
- [x] The other records (`log/*.md`, `tools/process/**`, `tools/ports/**`, `docs/`, `.claude/`) inventoried and marked (the documents table below); their contents are not yet read one by one: the claims of each are checked when its area's pending item is traced.
- [x] `tools/gates/lab_scan.py` and every gate run, their whole output read (2026-11-11, session 2; see "Gates and copy scan" below).
- [x] Simulations inventoried by their step and locks (the table below); each one's check against the researched correct way waits for Phase 2 (Q7 to Q11).
- [ ] Features tested as a player: not done; it needs the player-input tool (T3, Q6). The features list F* is still to be written.
- [~] Whole game measured at each tier: the town on both profiles, GPU time only (session 2). Owed: km24, frame time, CPU, tick, memory, load, and the photos of each tier by the image protocol.
- [~] Photos of earlier fixes re-analysed: the T-joint and bend photos of 149e5767 and the km24 traffic photos (below). Owed: the regions of `lines_edge`, the night pools and columns, the gates, covers, corners and seams of 2026-11-11, by the same protocol.
- [x] Development tools of `live-instance-design.md` checked on disk (the table below): none of Part A or E exists; Part B's guards exist and were run (168 problems, 1,858 scan findings).

## State of the tree when the pause came (2026-11-11, HEAD a44dc486)

- Uncommitted game code, built in the debug DLL only: P476/P485/P486 (a building keeps the cap its seed body was made under apart
  from its storeys; a lot's edges are read before its body is made; `edges_read`; save version 9) in `sim/buildings`,
  `sim/commands`, `sim/persistence`, `sim/simulation/growth.cpp`; P487 (`ped_edge_segs`: a walker with no door comes in at the
  outside end of a street at the walking window's edge) in `sim/traffic`. With them `city_mass_test`, `save_undo_test`,
  `edit_fuzz_test`, `strike_test`, `model_test`, `people_test` passed and `walker_motion_test` FAILED one check ("no walker stands
  in another": a pair, 3 mm inside each other, P489). The city's digest moved and was not blessed. Nothing of this is committed,
  delivered or photographed.
- P489 is traced, not fixed: its reference behaviour and this game's flow are in `dist/trace/port-from-nov7/rec-549.json` and
  `rec-550.json`, its cause is C45 of the ledger (the crowd step keeps bodies apart only as on one shared way). No test written.
- Another session's files are modified in the tree (`tools/analysis/**`, `tools/analysis/engine/**`): not this line's.
- The executable the owner opens: the Desktop shortcut points at `dist/builds/2026-11-11_2223_6d1cc9af/Roadcraft.exe`
  (`tools/build/deliver.sh export`). `dist/windows/Roadcraft.exe` is of 2026-11-10 17:15.

## Divergences between documents and code (found so far)

| # | Where | What it says | What is true | Status |
|---|---|---|---|---|
| D1 | CLAUDE.md `[c:deliver-exe]` | "the user opens `dist/windows/Roadcraft.exe`" | `deliver.sh export` writes `dist/builds/<date>_<commit>[-dirty]/Roadcraft.exe` and points the Desktop shortcut there; `dist/windows` is stale since 2026-11-10 | to fix in CLAUDE.md with its guard |
| D2 | `log/how-we-work.md` step 1, CLAUDE.md `[c:investigate-reading]` | no measurement or test to investigate | the owner's order of 2026-11-11 allows quick research tests in an isolated folder and measuring the whole game against the budgets; causes are still found by reading | to fix in both |
| D3 | `log/how-we-work.md` step 2, CLAUDE.md `[c:research]` | context7 when an outside API is touched | context7 on every implementation, fix and research | to fix in both |
| D4 | CLAUDE.md `[c:hidden-runs]` | tests and captures never open a window | the live instance may open a window that does not steal focus; tests and captures stay hidden | to fix |
| D5 | `log/how-we-work.md` step 7, CLAUDE.md `[c:verify-reach]` | photos judged, km24 before and after | the image protocol (expectation written first, every photo opened, 3x3 regions, defect list, analysis recorded, a guard at the close); every measurement also against the median-profile budgets | to fix, with the guard |
| D6 | `native/src/sim/traffic/ped_kerb.cpp:4` | "no two ways to it converge" | `onto_approach` gives one approach point to every walker from outside the span on one side and to two abreast coming along the kerb (ledger P494) | open |
| D7 | `street_works_test` (census of spans) | the lines are strung | the owner's photo of the delivered build shows a line pole with no wire; nothing checks that a strung span is drawn (ledger P495) | cause found and reproduced in isolation (session 2): the two wire levels swapped by the engine's visibility ranges leave a band of distance where neither is drawn (`log/research.md` Q16; trace `pole-no-wires` P1); fix pending (V1) |

## Why development is slow (found so far; each is a pending item to fix at its cause)

| # | What is slow or imprecise | Why | Evidence |
|---|---|---|---|
| S1 | A compaction costs the re-reading of whole files | the trace-fix edit hook refuses every edit, even of a memory note, until every file opened before the compaction is read again to its end | 2026-11-11: `walk.ts` (2,275 lines) had 1,200 lines re-read only to write a note |
| S2 | The flow of one trace holds 1,709 files and 271 open problems | one trace (`port-from-nov7`) was opened for "everything", so every read anywhere is owed a record | `ledger.py status` |
| S3 | A failing whole-town test does not reproduce after any change | `walker_motion_test` runs the town at random draws: P488 "did not occur" after an unrelated change; there is no deterministic scenario probe for the crowd | this session |
| S4 | Photos were taken and not analysed | the only guard of photos is `ledger.py close` (`photos.py:61`: every photo and crop opened, every photo named in `analysis.md`), and the trace `port-from-nov7` was never closed: opened 2026-11-10 08:06 for "everything", it holds 495 problems (271 open), so some sixty game commits were made, pushed and delivered with the guard never run. Nothing on the path a theme really closes by (the commit, `deliver.sh`, `quick.sh --ready`) asks for a photo's analysis; and nothing asks for the expectation before the shot, the regions at real size or the defect list. `photos/analysis.md` itself says "Crops: still to be read" (photos 00, 02, 03, 05, 06, 07 of `lines_edge`) and "Not read crop by crop" (the columns and T-joint sets) under commit 149e5767 | `dist/trace/port-from-nov7/photos/analysis.md` lines 11, 17, 21, 25, 37, 130, 154; `ledger.json` (`closed` empty) |
| S5 | One Godot at a time, each scene opening a whole town | to measure and trace | `suites.json` capacity, memory `one-godot-at-a-time` |
| S6 | Adding two lines to `log/mapa.md` cost the reading of its 686 lines (the same hook rule as S1, met again in session 2); a `sed` on a record forces its whole re-reading before the next edit | the READ rule counts the whole file, whatever the size of the change | session 2, 2026-11-12 |
| S7 | The tier measure on the floor profile took 451 s; a town is generated again for every scene | every scene builds its own city (L5) | `dist/research/measure/tier_floor.log` |

## Development tools: what `log/live-instance-design.md` designed against what exists (checked on disk, 2026-11-11)

| Piece of the design | Exists? | Evidence |
|---|---|---|
| Part A.1 development engine (`tools/live/engine/`, Godot 4.7.2 + Tracy + reload patch, `C:/Projects/toolchains/godot-dev`) | NO | neither folder exists |
| Part A.2 the core survives a reload (`native/src/api/city_core_reload.*`, `reload_carry`) | NO | file absent |
| Part A.3 instrumentation (`native/src/core/trace.h`) | NO | file absent |
| Part A.4 telemetry (`native/src/core/telemetry.*`, memory-mapped ring) | NO | file absent |
| Part A.5 the link (`project/game/dev/dev_link.gd`) | NO | `project/game/dev/` absent; only the older polled-file `project/tests/visual/live_driver.gd` stands |
| Part A.6 the supervisor (`tools/live/live.py`) | NO | folder absent |
| Part A.7 shots with provenance and `--proof` | NO | depends on A.5/A.6 |
| Part E real-player input (`project/game/dev/player.gd`, `ui.tree`, recorded plays) | NO | file absent; `project/tests/visual/ui_driver.gd` stands with the gaps the design lists (no button mask in drags, no typing, wheel, double click, modifiers) |
| Part B guards: LAB_COPY (`tools/gates/gate_lab.py`, `lab_scan.py`), CONTRACT_UNGUARDED (`gate_contracts.py`, `contracts.json`), MAP_WRITER_UNNAMED, STATE_CONTROLLER (`gate_states.py`), NUMBER_MOVED (`gate_patch.py`) | YES (files present; `log/guards-progress.md` says self-test 89 cases, 38 rules) | to be re-run and their whole output read (not yet done in this survey) |
| Part B6 run-time probes in the live instance | NO | no live instance |

So the owner's three tools (a live instance that reflects changes, a real-time monitor, playing as a player) and the measurement
independent of his machine do not exist. Each is a pending item; each is studied (Phase 2) before it is built: the design above is
an input to that study, not a decision already proven.

`log/guards-progress.md` itself lists what the guards do not catch (each a pending item): STATE_CONTROLLER saw a member written
from a variable only in the controller's own file (later extended by P477/P478/P480, to re-check); NUMBER_MOVED accepts a file's
basename; the digest's golden (`dist/quick/golden.txt`) has no guard; a test scene whose script fails to parse waits out its
600 s deadline; the full `run_gates` had 186 standing problems (ESTADO_NA_UI 69, TEXTO_VIA_TR 21, NUMERO_MAGICO 20, SEGUNDO_DONO
17, TAMANHO_ARQUIVO 14, ...) when the guards' plan closed; the copy scan found 1,779 names and 72 functions sharing passages
with the reference (386 names in `project/data/ui`, the Build drawer).

## Gates and copy scan (run 2026-11-11, session 2, HEAD a44dc486 plus the uncommitted tree; whole output read)

- `python tools/gates/run_gates.py`: exit 1, **168 problems in 3,064 files**: ESTADO_NA_UI 69, TEXTO_VIA_TR 21, NUMERO_MAGICO 20,
  SEGUNDO_DONO 17, TAMANHO_ARQUIVO 14, VALOR_DUPLICADO 11, DONO_UNICO 9, TAMANHO_FUNCAO 4, DOC_UNICO 2, UI_SEM_ESTILO_SOLTO 1.
  The 14 oversized files: `tools/build/rename_english.py`, `project/ui/inspector_panel.gd`, `project/tests/core/edit_fuzz_test.gd`,
  `project/render/sky_view.gd`, `project/game/settings/settings.gd`, `project/game/camera/city_camera.gd`,
  `native/src/sim/roads/road_grade_solve.cpp`, `road_grade_profile.cpp`, `native/src/sim/commands/journal_ops.cpp`,
  `edit_history.cpp`, `commands.h`, `body_survey.cpp`, `native/src/render/chunk_jobs.h`, `render/buildings/building_renderer.h`.
  DOC_UNICO refuses the trace-fix skill's own `SKILL.md` and `INSTALL.md` (a rule that contradicts where a skill must live).
  So "the gates pass" is false today for the whole tree; a theme closes only on its changed files.
- `python tools/gates/lab_scan.py`: exit 1, **1,858 findings in 1,028 files** (10 s): 1,778 are a defined name the reference
  also defines; 80 are structure (shared fingerprint passages, or numbers in the reference's order). By folder: `project/data/ui`
  386, `project/data/render` 157, `sim/commands` 144, `sim/traffic` 123, `sim/buildings/b40` 96, `api` 92, `sim/roads` 91,
  `project/data/catalog` 77, `project/render/shaders` 74, `project/data/game` 55, `sim/buildings/interior` 48,
  `render/buildings` 44, `sim/street` 41, `sim/transit` 38, `sim/nature` 32, `project/game/tools` 28. Structural hits past 90 %:
  `roof_outline.cpp:102/106`, `roof_pitched.cpp:57/61` (strip shape), `body_survey.cpp:43` (97 %), `road_grade_profile.cpp:44/50`
  (100 %), `swaths.cpp:98` (94 %), `atmosphere.gdshader:89` (98 %), `building_lamps.gdshaderinc:57`, `building_props.gdshader:27`,
  `marks.gdshader:47` (100 %); `reeds_shepp.cpp` has nine functions between 33 and 71 %.
- What the scan cannot say: it compares with `lab/roadcraft` only, by names and fingerprints. The history says more: the game
  was built by eight "Porte" lines (roads, build, landscape, interface, traffic/transit, people, demolition, interior; commits
  `Integra a Porte N` of 2026-11-04/05, records in `tools/ports/*`) that ported the older 4.0 of the same game, and `log/pendencias.md`
  describes most areas as "como no 4.0". So the question "what of the roadcraft still stands" is answered per area by reading its
  port record and its code, not by the scan: see the pending items R1 onward in `log/pendencias.md`.

## Documents: every record and loose file, and its state (inventory 2026-11-11, session 2)

| Where | Files | What it is | State |
|---|---|---|---|
| `CLAUDE.md` | 1 | the rules, structure, commands | current; to amend by the owner's orders (D1 to D5) |
| `log/how-we-work.md` | 1 | the way step by step, contradictions settled | current; to amend (D2, D3, D5) |
| `log/specification.md`, `log/research.md`, `log/pendencias.md` | 3 | this order's records | current (pendencias is half Portuguese and has no symptom/cause/criterion per item: to restructure) |
| `log/mapa.md` | 1 | every file | current |
| `log/work-log.md`, `conversor.md`, `causa-raiz.md`, `lab.md`, `base.md`, `arquitetura.md`, `review.md`, `feature-trace.md`, `ferramentas.md`, `performance.md`, `live-instance-design.md` | 11 | the lines' records | current as history; `conversor.md` describes a forbidden route (outdated); `live-instance-design.md` is a design not built (an input to Phase 2) |
| `log/guards-plan.md`, `guards-progress.md` | 2 | plan and progress of the guards (Part B) | closed work: to fold into `ferramentas.md` |
| `log/trace-*.md` (22 files: core, engine, research, rules and seven `trace-rules-*-plan/progress` pairs) | 22 | the feature tracer's plans and progress (another line) | duplicated subject over 22 files: one current record (`feature-trace.md`) with the plans as its sections; not CLAUDE.md-listed |
| `docs/` (ignored by `.git/info/exclude`, 2026-10-21 to 10-26) | 31 | research reports before the Godot port (large cities, verification, roads), geometry audits, three sprite sheets, run logs | obsolete as documents of this tree (they speak of the JavaScript game); the research in them is input for Phase 2, then the folder leaves |
| `tools/process/records/` | 384 | the old porting process's surveys, queues, readings of the 4.0 | obsolete (the port process is forbidden); kept only as history of WHAT the 4.0 does |
| `tools/process/GODOT-PORT-GUIDE.md`, `session-rules.md`, `CLAUDE.md`, `READ-ME.txt`, `converter-old/`, `features-4.0/`, `.claude/` | 49 | the port guide and rules of the port sessions | obsolete and contradictory (they instruct porting and translating): to remove after migrating what is behaviour |
| `tools/ports/*` (15 areas) | 137 | each port's plan, coverage and evidence | outdated: "what is left to port" lists; the behaviour lists in them are the only useful part |
| `tools/oracle-4.0/` | 31 | the instrumented 4.0 | obsolete (`roadcraft-4.0-main` is no reference) |
| `tools/converter/`, `tools/converter-slice/` | own git | the translator | obsolete (translating is forbidden) |
| `.claude/REGISTRO-GOAL.md`, `.claude/metas.md`, `.claude/metas/` | 3 | an earlier /goal's record | obsolete (the goal was cleared by the pause) |
| `.claude/skills/trace-fix/SKILL.md`, `INSTALL.md` | 2 | the skill | current (the owner's: hands off) |
| memory (`~/.claude/projects/.../memory`, 31 notes) | 31 | notes across sessions | see "Contradictory instructions" |

## Simulations: what exists and how each is stepped (read in the code 2026-11-11, session 2)

Read whole: `sim/simulation/simulation.{h,cpp}`, `sim/traffic/traffic_system.cpp`, `sim/traffic/window.cpp`; the other owners by
their headers and step functions (the deep reading of each is its pending item's trace). Sizes: traffic 95 files / 13,726
lines, commands 72 / 10,567, buildings 57 / 6,190, roads 45 / 4,951, citygen 19 / 2,282, terrain 13 / 1,707, street 14 / 1,481,
transit 12 / 1,416, nature 12 / 1,202, demolition 6 / 510, economy 2 / 161, simulation 5 / 562; render 256 / 29,708; api 70 / 10,250.

| Simulation | Owner | Time step | Order in a tick | Thread and locks | Detail by distance | What the code shows against a correct simulation |
|---|---|---|---|---|---|---|
| Clock and speeds | `Simulation` (`simulation.cpp`) | fixed `1/sim_tick_hz`, own thread, sleeps to a deadline; behind by more than 2 periods the debt is dropped | at speed N a tick runs N steps back to back | own thread; `wake_mutex_` for the speed | none | sound (fixed step, one setter with `SPEED_MOVES`); FAST/FASTEST multiply the cost of a tick by N with no budget: a tick over its period is silently dropped time (no probe counts it) |
| Traffic (vehicles, lanes, junctions, signals, kerb stops, lots' cars, lorries, yard) | `traffic::step` (`traffic_system.cpp:184`) | the tick's dt | network changes, route compaction, lights, window, parking sources, governor, spawn, meso, lane changes, vehicles, flow control, kerb stops, lots | sim thread, `map_mutex` shared + `traffic_mutex`; routes synchronous in the tick | a "micro window" round the **camera** (`window.cpp`): inside it vehicles are bodies (IDM, MOBIL), outside they are queue events (meso) kept only to `meso_window_out_m`; a car leaving the window is despawned, the demand refills what enters | the population follows the camera, not the city: `camera_x/z` is read in `window.cpp`, `demand.cpp`, `parking_sources.cpp`, `pedestrians.cpp`, `traffic_system.cpp`, `rail_run.cpp`. Two runs of one city with two camera paths give two traffic states (not deterministic in the state; the state hash cannot cover it); no trip has an origin and a destination in the city beyond the window; `publish_poses` walks every slot of `vehicles` and `peds` each tick |
| Walkers | `step_pedestrians` (`pedestrians.cpp`), 30 `ped_*.cpp` | the tick's dt | after transit, last | same locks | exist only within `ped_window_m` of the camera | same camera dependence; bodies kept apart only as on one shared way (ledger C45, P489: a pair 3 mm inside each other); a failing whole-town test is not reproducible (S3) |
| Transit (buses, trains, metro, level crossings) | `transit::step` (`transit_sim.cpp:144`), `step_bus`, `step_train` | the tick's dt | after the lots, before walkers | same locks | buses near the camera are vehicles, away are on their timetable | riders are not counted (pendencias: "sem contar quem entrou"); stops and rails searched in a list, no index |
| Growth | `grow_step` (`city_growth.cpp:82`), `growth.cpp` | per step, `sim_growth_per_s` | after the clock | decides under the shared map lock, exclusive only to add | only cells whose lots changed | fills free lots nearest the last grown: no demand, population, jobs or land value drives it |
| Damage, fire, wounds | `damage_step` (`damage_state.cpp:79`), `bleed_step` (`wounds.cpp:12`) | `owed_ + dt` | last in a step | exclusive map lock by **try_to_lock**; when a mesh job holds it the time is owed to a later tick | none | the partition of time into steps depends on the mesh jobs' timing: not deterministic unless every rule is linear in dt (to check in its trace) |
| Economy | `road_economy.cpp` (161 lines) | none | none | inside commands | none | not a simulation: a price per road edit and a refund on demolition; no income, upkeep, budget or taxes |
| Weather, clouds, fog | `sim/nature/sky.*` | none | none | commands write, render reads | none | a state the player sets, not simulated: no evolution in time |
| Ecology | `sim/nature/ecology.*` | none | none | worked out where read (render) | per chunk | a derived field of the land, not simulated in time |
| Terrain | `sim/terrain/*` | none | at edits | commands; jobs for the heightfield | patches | a field edited by brushes and by roads' and lots' shapes; no erosion or water flow (none asked) |
| Blast | `blast_model.*` | per event | at a strike | command | none | Hopkinson-Cranz / Kingery-Bulmash curves, pure functions |

Machine-translated code still standing (its own headers say "Translated from Roadcraft 4.0 ... by the converter"):
`core/js_array.h`, `js_map.h`, `js_number.h`, `js_runtime.h`, `js_string.h`, `js_value.h` (1,073 lines, the ECMAScript runtime);
`sim/roads/control_advisor.h`, `control_advisor_enums.h`, `control_advisor_types.h`, `control_advisor_query.cpp`, `flow_stats.h`,
`flow_stats_types.h`, `flow_stats_query.cpp`; `sim/traffic/flow_control.{h,cpp}` (their caller); `api/city_core_flow_probe.cpp`
(`flow_replay`, a test that a number equals the 4.0's). Hand ports that name their source: **266 of the 1,028 code and data
files** cite a `.ts` file or "Roadcraft" in their own comments (`sim/commands` 39, `project/game` 34, `project/render` 31,
`project/data` 30, `sim/traffic` 28, `sim/roads` 25, `project/ui` 20, `sim/citygen` 11, `render/buildings` 11, `render/agents` 10,
...). `window.cpp:1` reads "The micro window around the camera (Roadcraft traffic.ts checkWindow/materialize)".

## Photos of earlier fixes, analysed again by the image protocol (session 2, 2026-11-11)

Source: `dist/trace/port-from-nov7/photos/` (294 photos, 3,314 crops, of the fixes since 2026-11-10). Each photo below was
opened whole with Read, then cut at real size (regions enlarged 2x from the 1920x1061 frame) where the changed element stands.
Positions are pixels of the whole photo. The earlier verdicts are in that folder's `analysis.md`.

### `00-tjoint_near_a_day.png` (proof photo of commit 149e5767, P471-P473 "the straight side of a T"; earlier verdict: "right")
Expected by the change: line poles past the crosswalks, wires strung pole to pole over the T, signal posts apart from line poles.
- Whole: a signalled T, an avenue with a median, a park (top), lots; four timber line poles at (715,215-420), (1420,330-550),
  (1890,180-400), (640,580-870); two grey lighting columns; three signal masts; HUD "0 veículos · 0 pessoas".
- **DEFECT, wires missing** (regions (450-1100,150-470), (1280-1920,280-600), (540-960,540-900) at 2x): every one of the four
  line poles carries its cross-arm and three white insulators and **no conductor leaves any of them**; no wire is drawn
  anywhere in the frame. The thin dark lines on the road are the poles' and columns' own shadows (each starts at a pole's foot).
  The same junction photographed from the other side in the same run (`01-tjoint_near_b_day.png`) shows the wires of the same
  poles as clear black lines. So the conductors are drawn from one view and not from the other: this is the owner's photo
  (a line pole with no wire, D7/P495) **reproduced in the fix's own proof photo**, which the earlier analysis called right.
- **DEFECT, bodies against each other**: at (690-720,215-420) the timber corner pole and the grey signal post stand side by
  side, about 0.3 m apart, and the signal's mast arm runs through the timber pole; the same at (1400-1430,330-550) (the pole with
  the transformer and the signal post touching, the mast arm starting at the pole). P395's rule ("no pole against a signal
  post") is not met, though the earlier analysis says "the signal posts stand apart from them".
- At (640-670,850-870) the line pole and a litter bin stand touching on the footway.
- At (430-1180,700-1040) a row of shrubs stands half inside the parapet wall of the building below (a hedge cut by a wall).
- Checked and sound: crosswalk stripes, stop lines, median nose and kerbs, the "30"/"60" signs, the street-name blades, the
  gates' leaves swung into the lot at (1680-1830,450-520), roofs and roof gear, tree shadows, no floating or sunk body.

### `01-tjoint_near_b_day.png` (same commit; earlier verdict: "right"; "not read crop by crop")
- Wires drawn here: from the corner pole (540,260) to the left and to the pole at (1150,120); from the pole (1270,410) down
  the stem street to the lower right.
- **DEFECT**: at (1210-1300,410-700) the timber pole, a signal post and its mast arm stand in one another's space (arm across
  the pole at (1265,575)); at (530-570,260-480) the corner pole, a signal post and a hydrant stand within about a metre.
- No wire crosses the junction between the two corner poles in view (the fix's claim "the line runs on over that side" shows
  only along the top footway).
- A tonal seam where the junction's plate meets the lanes ((880,740) to (1000,560)): the plate is lighter and bluer.
- Checked and sound: crosswalks, medians, columns' feet on the footways, lots' walls, gardens, the foreground tree.

### `03-bend_near_a_day.png` (same commit, the 90 degree bend)
- One line pole at the inner corner (1160-1190,700-965), wires leaving along both arms; a hydrant 0.5 m from its foot.
- The right warehouse's shadow on the grass has a stepped outline in four steps ((1060-1390,210-360)): to check against the
  roof's shape (a shadow-map step is a defect, a sawtooth roof is not).
- Checked and sound: kerbs round the bend, medians, parking bays' lines, fences and gates, columns on the footways, no floating body.

### `00-lines_edge_junction_near_a_day.png` (P393-P395; earlier: "Crops: still to be read")
- Wires drawn and continuous over the junction from this view; poles on the footways. The foreground building hides the near
  corner. No defect seen at whole-photo scale; its regions are still owed (listed in the pending item of the photos).

### `00-traffic_km24_street_day_a.png`, `02-traffic_km24_near_day.png` (2026-11-11 00:58, proof of b994917f "the streets round the view carry their traffic in the 24 km city")
- `00`: a view over the towers' roofs; the streets are slivers between towers ((230-420,180-470), (540-720,700-980)); the HUD
  reads "2.784 veículos · 2.050 pessoas" but no vehicle or person is large enough to judge (a car is 3 to 6 pixels). It proves
  the counters, not the traffic.
- `02` ("near"): the camera stands against a tower's facade: the frame is window mullions and one room's floor seen through
  the glass; the street is a strip 130 px wide at the left edge. **The photo missed its subject**; nothing of the fix is in it.
- After 2026-11-11 00:58 there is **no photo with a vehicle or a person at judging size**, though eleven later commits change
  movers (76551040, 527a5a4a, 8a358b3a, 0008178d, e8610ea1, a23ac3c3, 2896366f, 865f1321, 2cdf6a76, ee1d5aa8, 7ccee67b); every
  later photo's HUD reads "0 veículos · 0 pessoas" (the clock is paused and the town empty when the shot is made).

**Verdict of the re-analysis so far:** the proof photos of commit 149e5767 show two defects that the commit's own analysis
denies (wires absent from one view; signal posts against line poles). The theme was closed, pushed and delivered with them.
Both are pending items (P495 and the posts), with the photos above as their "before" image.

## Goals, with measurable targets

| # | Goal (the owner's orders) | Target that can be checked |
|---|---|---|
| G1 | Nothing of the roadcraft copied: every feature this game's own build | no file "translated by the converter"; no file whose design is the reference's (each area's pending item R* closed by its own design, tests from behaviour, photos, budget); `lab_scan` 0 structural findings; the name findings reviewed against the domain lexicon |
| G2 | Every feature works whole | every feature of the list F* played with mouse and keyboard through the player-input tool, its expected result written first, 0 failures |
| G3 | Every simulation made the correct way | each row of the simulations table meets its researched design (Phase 2): fixed step, the same state from the same inputs whatever the camera does, an order of update written down, a cost per agent inside the tick budget |
| G4 | The game runs well on a median computer, and runs on the floor | the budgets below, measured on the two profiles in km24 and in the town, at each tier the profile is meant for |
| G5 | Visual quality at each tier | photos of each tier by the image protocol, 0 defects |
| G6 | Root causes only; proof, not claims | every closed item has its test, measure or analysed photo recorded |
| G7 | Fast development without cutting | the times of the table "Why development is slow" inside their targets (a check of a C++ change under 2 minutes; a photo under 30 s from a live instance) |
| G8 | Contracts kept by mechanisms | every `[c:...]` has a guard that can fail (`contracts.json`); the whole `run_gates.py` at 0 |
| G9 | One document per subject, English | the documents table above reduced to its "current" rows; pendencias structured |

## Constraints

CLAUDE.md's rules whole (not repeated). Added by the owner's orders of 2026-11-11: no agents; context7 on every implementation,
fix and research; the image protocol on every photo; records written at once; the costliest solution at the cause; tests and
captures hidden, the live instance alone may show a window, without taking focus; one Godot at a time while working.

## Reference hardware and budgets (from `log/research.md` Q1, Q2)

| Profile | CPU | RAM | GPU | Display | Tier it must hold | How it is run on this machine |
|---|---|---|---|---|---|---|
| **Median** (Steam, October 2026: 25th to 50th percentile) | 6 cores / 12 threads | 16 GB | RTX 3060 class, 8 GB VRAM | 1920x1080 | `medium` at 60 fps; `high` at 60 fps wanted | `--gpu-index 0`, logical processors 0-11 (six P-cores), Job memory 16 GB |
| **Floor** (10th to 15th percentile) | 4 cores | 8 GB | integrated (UHD 770 class), shared memory | 1920x1080 | `low` at 30 fps | `--gpu-index 1`, logical processors 16-19 (four E-cores), Job memory 8 GB |

Budgets (a frame is the whole frame the player sees; p90 over a camera path, hitches apart):

| Budget | Median, `medium` | Floor, `low` |
|---|---|---|
| Frame time p90 | 16.7 ms (60 fps) | 33.3 ms (30 fps) |
| Hitches | none over 33 ms on a camera path | none over 66 ms |
| GPU time p90 | 13 ms (80 % of the frame) | 27 ms |
| Main thread CPU per frame | 8 ms | 16 ms |
| Simulation tick (at 1x, and each of the N steps at 2x/4x) | under half its period | under its period |
| Process CPU, all threads | under 50 % of the profile's processors on average | under 75 % |
| RAM (working set), km24 | 4 GB | 3 GB |
| Video memory, km24 | 4 GB | 1.5 GB |
| A new town opens in | 8 s | 20 s |
| km24 opens in | 15 s | 45 s |

x86-64-v3: excludes about 2.8 % of x86 Steam machines, none inside the floor profile (Q1). Kept; the start-up message for a
machine without it is pending item M2.

## The game against the budgets today (measured 2026-11-11, session 2; `dist/research/measure/`, town map 0 seed 1, 1920x1080, clock stopped, debug DLL of the uncommitted tree)

`tests/perf/tier_gpu.tscn`, GPU ms p50 / p90 of the frame at three views (street 90 m, neighbourhood 500 m, city 2,200 m):

| Tier | Median: street | neighbourhood | city | Floor: street | neighbourhood | city |
|---|---|---|---|---|---|---|
| low | 4.5 / 5.0 | 6.0 / 6.2 | 3.4 / 4.4 | **35.2 / 36.1** | **62.7 / 64.8** | **39.1 / 40.2** |
| medium | 6.7 / 6.8 | 11.6 / 11.8 | 6.0 / 6.2 | 79.9 / 84.8 | 129.3 / 131.6 | 73.6 / 74.9 |
| high | 10.8 / 11.0 | **13.2 / 13.3** | 7.1 / 7.3 | 137.7 / 140.7 | 171.1 / 172.6 | 129.7 / 135.6 |
| ultra | 11.1 / 11.1 | **13.3 / 13.6** | 7.5 / 7.6 | 153.6 / 157.3 | 184.9 / 200.9 | 106.0 / 107.9 |

**km24 on the median profile** (`tests/perf/camera_pan.tscn --map=4`, the tests' tier `medium`, 1920x1080, vsync, the debug
DLL, 473,845 buildings; `dist/research/measure/pan_km24_median.log`, 2026-11-12):

| Move | Frames | Frame p50 / p95 / p99 (ms) | Over 33 ms / 50 ms | GPU mean / p95 / max (ms) | Core on the main thread mean / p95 / max (ms) | Static / video memory (MiB) |
|---|---|---|---|---|---|---|
| still | 597 | 16.7 / 16.8 / 21.6 | 1 / 1 | 12.1 / 12.5 / 12.8 | 1.2 / 2.1 / 3.0 | 668 / 2,501 |
| cross the city | 5,702 | 16.6 / 20.4 / **41.8** | **119 / 10** | 13.3 / **14.9** / 16.3 | 1.8 / 3.8 / **157.0** | 860 / 2,664 |
| circle | 899 | 16.7 / 18.3 / 33.7 | 18 / 0 | 13.0 / **14.0** / 14.3 | 1.0 / 1.8 / 26.1 | 846 / 2,776 |
| turn | 357 | 16.7 / 19.2 / 27.2 | 2 / 1 | 10.2 / 12.3 / 14.0 | 2.0 / 3.7 / **84.3** | 960 / 2,633 |
| zoom | 478 | 16.6 / 21.4 / 35.2 | 5 / 0 | 10.3 / **13.4** / 14.1 | 4.3 / **8.6** / 28.5 | 707 / 1,983 |
| close to walls | 707 | 16.6 / 22.6 / 29.5 | 6 / 1 | 11.6 / **13.9** / 15.2 | 1.9 / 3.9 / 20.1 | 798 / 2,418 |

- Against the budgets: the frame's p95 passes 16.7 ms in every move but standing still; **151 frames over 33 ms** (119 while
  crossing the city) where the budget is none; the GPU's p95 passes 13 ms in four of six moves (up to 14.9); the main
  thread's core time reaches 157 ms in one frame (budget 8 ms) and 84 ms on the first frame of the turn (66 ms of it in
  `levels`); video memory 2.0 to 2.8 GB (inside 4 GB), static 0.7 to 1.0 GB. The city took **73.6 s to generate** (debug DLL;
  23.6 s on the release DLL in `log/performance.md`; budget 15 s). Pending items M4, M6.
- Owed: the same on the floor profile at `low` (its run does not fit one 590 s deadline: the task must run it in parts, T4),
  the process's RAM and CPU by thread (Q5), and all of it on the release DLL (T5).
- Median: every tier is inside 16.7 ms in the town; `high` and `ultra` pass the 13 ms GPU budget at the neighbourhood view
  (13.3, 13.6). The town is not the measure that matters: km24 on this GPU was 11.75 and 13.67 ms at 1600x900 `medium`
  (`log/performance.md`), so at 1920x1080 (1.44 times the pixels) it is expected over budget: to measure (pending item M4).
- **Floor: `low` fails its 27 ms GPU budget at every view** (36 to 65 ms: 15 to 28 fps), with 107 to 561 draw calls and 0.6 to
  3.6 million triangles: the cost is per pixel, not per object (the city view draws 0.59 M triangles and still takes 39 ms).
  The tier has no render scale under 1 and its screen passes run at full resolution: pending item M5.
- The CPU side cannot be judged from this run: a project run loads the debug DLL (hardened libc++); the release DLL runs only
  in the exported game, which carries no probe. Measuring CPU and tick against the budgets needs the probes to run on the
  release DLL (pending item T5).
- Under an affinity the game's log repeats "Warning: SetThreadGroupAffinity failed / SetThreadIdealProcessorEx failed" (10 on
  the median run, 17 on the floor run); the bare Godot probe of Q2 under the same affinity prints none, and neither string is
  in this game's sources: it is a library the game brings in that pins its threads (the engine's occlusion culler, Embree, is
  the candidate: to confirm in its source, Q3). Harmless to the numbers; recorded so that it is not read as a new defect twice.

## To be written as the survey goes

The optimisation, performance and quality architecture; how each simulation must be made; the features list F*; the research
questions (drafted in `log/research.md` as they open).
