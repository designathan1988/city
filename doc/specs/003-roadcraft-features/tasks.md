# Tasks: Bring the reference game's features, correctly and fast

**Input**: doc/specs/003-roadcraft-features/ (spec.md, plan.md, research.md, data-model.md, contracts/, quickstart.md)

**Every task is an item of the loop** (quickstart.md section 2, CLAUDE.md method): one look before; the reference traced and every
file reached read whole; Context7 for every API; code whole with nothing run; its own scenes and photos in seconds, the photos
analysed; commit and push; next. Each task's **Done when** is its verifiable goal.

## Format: `[ID] [P?] [Story] Description`

## Phase 1: Setup

- [ ] T001 Register the problems this specification found and that the register lacks, through `python tools/atlas/cli.py problems add`: this game's 1536 text keys equal to the reference's (project/data/i18n/*.csv); LAB_COPY and tools/gates/lab_scan.py not reading the text tables; camera_feel_test at 57-66 s; the camera tests judged by a blind sight before 2026-11-13 (project/tests/core/camera_feel_test.gd, camera_walk_cases.gd); the tracer's `--cost` empty on `city.traffic.step` (tools/tracer). **Done when**: each has its id in doc/problems.jsonl and doc/problems.md is rendered.
- [ ] T002 Commit the inventory (tools/atlas/inventory.py, inventory_reference.py, inventory_game.py, inventory_rules.py, inventory_ts.py, inventory_json.py, inventory_report.py, tools/atlas/cli.py, tools/atlas/tests/test_inventory.py, tools/tasks/suites.json dist/inventory line, doc/map.md lines) after `python tools/gates/run_gates.py --file` on each and `python -m unittest discover tools/atlas/tests`. **Done when**: tests pass, gates 0 findings, pushed.
- [ ] T003 Add the specification folder to doc/map.md (`doc/specs/003-roadcraft-features/**`: the specification of the port, its research, plan, contracts, sheets and tasks). **Done when**: the map gate passes.

## Phase 2: Foundational (blocks every story)

- [ ] T004 Make the block's close check the method (contracts/block-close.md) in tools/tasks/block.py, with tests in tools/tasks/tests/test_block.py: refuses without `## Read` (a file read to its last line), `## Research` (a Context7 line when code changed), `## Skills` (city-method, verification-before-completion; systematic-debugging after a failed check), or with two failing runs of one check and no cause between. **Done when**: the tests prove each refusal and a complete file closing; `python -m unittest discover tools/tasks/tests` passes under 60 s.
- [ ] T005 Bring camera_feel_test under 60 s (project/tests/core/camera_feel_test.gd): its own eye check reuses the rig's answer for the same eye (CameraQueries), and rides that prove the same case twice are removed only where two rides are shown to measure the same thing (research R5). **Done when**: `python tools/tasks/cli.py run camera_feel_test` ends under 60 s with every check it had.

## Phase 3: User Story 6 - One source of truth (P1)

**Goal**: no contradicting, stale, scattered document; no dead tool. **Independent test**: the surveys (research R7) find nothing; the whole gates (owner's order) 0 NEW.

- [ ] T006 [US6] Fix every document finding of the survey (research R7) in CLAUDE.md, .claude/skills/city-method/SKILL.md, doc/map.md, doc/specification.md, doc/research.md and doc/history/*.md: each order said once (CLAUDE.md), each stale command, path, name or number made true of the tree, each prose file outside doc/ moved or removed. **Done when**: a re-read of the survey's items finds each fixed; gates on the changed files 0 findings.
- [ ] T007 [US6] Fix every tool finding of the survey (research R7) under tools/: an unused tool removed (git keeps it), a broken one fixed at its cause, a duplicate merged into its owner, each removal's map line and callers updated. **Done when**: each remaining tool has a caller and its tests pass, each under 60 s.
- [ ] T008 [US6] Fix the tracer's empty `--cost` on `city.traffic.step` and the start `traffic::step` naming nothing (tools/tracer/tracer/cost.py and its readers), test in tools/tracer/tests. **Done when**: `python tools/tracer/trace.py city.traffic.step --cost --root native/src/sim/traffic` prints the loops; `python -m unittest discover -s tools/tracer/tests` passes.

## Phase 4: User Story 3 - Each item checked in seconds (P1)

**Goal**: an item's check in seconds, photos aimed at the element. **Independent test**: an item's `cli.py run <scenes> <plan>` under 60 s; plans name elements, not coordinates.

- [ ] T009 [P] [US3] Add the look step `setViewNear [kind, x, y, zoom, azimuth, elevation]` (contracts/look-plan-steps.md) in project/tests/visual/look_aim.gd and look.gd, kinds pole, vehicle, walker, building, sign, each found by the core's existing probes (`shot_targets`, `street_watch`, `probe_building`); finding none is a FAIL. **Done when**: a plan with one of each kind takes its shots and prints what it found; look.gd stays under 250 lines.

## Phase 5: User Story 2 - What each game has, and the sheets (P1)

**Goal**: the inventory and one sheet per feature. **Independent test**: the inventory under 5 s; a sheet per contract.

- [ ] T010 [US2] Write the camera's comparison sheet doc/specs/003-roadcraft-features/sheets/camera-deocclusion.md from research R1 (contracts/comparison-sheet.md): the reference's isoViewport.ts and cameraSolids.ts behaviour, this game's project/game/camera/* and native/src/api/city_core_sight.cpp, the differences, the design. **Done when**: every section of the contract is filled with file:line evidence.

## Phase 6: User Story 1 - The camera (P1)

**Goal**: the eye never inside, no jolt from the rig's smoothing, the reference's behaviour. **Independent test**: quickstart US1 rows.

- [ ] T011 [US1] Add the camera's values to project/data/game/camera.json and tools/gates/schemas/camera.schema.json: `eye_roof_clearance_m` 1, `eye_wall_reach_m` 2, `eye_step_back_m` 4, `pull_soft_m` 1.5, `pull_wait_s` 0.35, `pull_in_s` 0.12, `pull_hold_s` 0.4, `pull_out_s` 0.6, `near_share` 0.02, `near_gap_share` 0.5, `near_min_m` 0.08 (data-model.md, the reference's behaviour). **Done when**: the data gate passes and DataTables reads them.
- [ ] T012 [US1] Give the core the floor field and the reference's eye rules in native/src/api/city_core_sight.cpp (contracts/camera-solids.md): `floor_at(x, z)` (inside a plan its roof plus the clearance; within the reach round it easing to the foot by a smoothstep of the distance), `eye_free(focus, eye, skin)` by rules 1-4 (both inside walls: both over the roof plus the eye's lift; eye in, point out: in along the line to where it leaves, plus skin; eye in the margin: back along the line up to the step back out of it; still inside: over the floor), the values passed by the rig from camera.json (no literal in native/src); bound in city_core_bind.cpp, declared in city_core.h, routed in project/game/store/store_api.gd. **Done when**: both DLLs build with no warning.
- [ ] T013 [US1] Bring the rig to the reference's behaviour in project/game/camera/camera_ahead.gd, city_camera.gd, camera_pull.gd: the corrections at once from `eye_free` (the point lifted only when the eye and the point are both inside), the pull eased by Cinemachine's four times (wait, in, hold, out), the near plane `max(near_min, min(distance * near_share, (gap + 1 m) * near_gap_share))` with `gap` the eye's height over `floor_at`; the look ahead kept, checked where the eye will stand (confirm). **Done when**: look_view_test and camera_input_test pass.
- [ ] T014 [US1] State the camera tests' criteria by research R1 in project/tests/core/camera_feel_test.gd and camera_walk_cases.gd: the eye inside walls at most 1 frame in 1,000 (`INSIDE_SHARE`), no jolt past the limit from the rig's own smoothing, the at-once corrections counted and printed with their largest size; the cause of the change written in doc/history/game.md. **Done when**: `python tools/tasks/cli.py run camera_feel_test camera_input_test look_view_test` passes, each under 60 s.
- [ ] T015 [US1] Photograph the camera at a wall: a look plan project/tests/visual/camera_wall_plan.json (an item of `looks` in tools/tasks/suites.json) with `setViewNear building` at street level, mid and far, four bearings, and `setViewRoof` near and mid; analyse each photo region by region (CLAUDE.md image protocol). **Done when**: every photo shows the view outside the buildings, no wall cut by the near plane, the roofs from above; analysis written in the work file.

## Phase 7: User Story 5 - The open items (P2)

**Goal**: the 24 km opening fast, the text keys the game's own, the frozen view right. **Independent test**: quickstart US5 rows.

- [ ] T016 [US5] A candidate's look from its massing only in native/src/sim/simulation/growth.cpp (`draw_candidate`) and native/src/sim/buildings/b40 (the massing stage the look reads: volumes, main wall paint, main roof, use), the chosen look kept in the look cache (body_cache.cpp), candidates out of the body cache (research R4). **Done when**: city_mass_test, lot_front_test and lot_overlap_test pass; the digest's change, if any, has its cause in doc/history/game.md.
- [ ] T017 [US5] `fill_all` in a 2 x 2 checkerboard of 256 m cells over the job pool in native/src/sim/simulation/growth.cpp (each cell's lots in their order; every neighbour read under 256 m: look reach 100 m, organic reach 30 m), the same city for the same seed whatever the pool size. **Done when**: edit_fuzz_test, save_undo_test and traffic_hash_test pass; the digest written again with its cause.
- [ ] T018 [US5] `settle_sites` reusing the edges `record_for` read and making its sites in parallel, inserted in order, in native/src/sim/commands/lot_sites.cpp. **Done when**: lot_front_test passes; window_traffic_test ends under 60 s with the same check.
- [ ] T019 [US5] LAB_COPY and lab_scan read the text tables (tools/gates/gate_lab.py, lab_scan.py, rules.json lab_copy paths): a key of project/data/i18n equal to one the reference defines is a finding. **Done when**: the gate's fixtures prove it (tools/gates/tests/fixtures), selftest passes, and the scan lists today's keys.
- [ ] T020 [US5] Rename this game's text keys the reference defines, one area a task (interface, buildings, roads, ...), in project/data/i18n/*.csv and every `tr("KEY")` and data `*_key` that names them (found by Grep, changed with Edit), names of this game's own words. **Done when**: the scan of T019 finds none in that area; the scenes that read those texts pass.
- [ ] T021 [US5] Check the frozen street view and the roofs plan with the camera of US1 (project/tests/visual/frozen_plan.json, roofs_plan.json): `python tools/tasks/cli.py run photos roofs`, each photo analysed. **Done when**: the street photo shows a street with the eye outside every building; near and mid roof photos show roofs from above.

## Phase 8: User Story 4 - The port, area by area (P2)

**Goal**: every reference feature the player uses paired or justified. **Independent test**: per area, `python tools/atlas/cli.py inventory --area <a> --unpaired` shows only justified entries.

For each area in this order (the inventory's player-facing gaps, 2026-11-13): people (159 reference-only), traffic (73), street furniture (138), lots (36), roads (181), effects (32), saves (16), camera (20), terrain (121), sky (34), junctions (44), signals (13), zones (23), transit (9), growth (9), buildings (347), interface (130), lab (129, kept only where the player uses it):

- [ ] T022 [US4] People: one sheet per unpaired feature (doc/specs/003-roadcraft-features/sheets/people-*.md), built as items of the loop. **Done when**: the area's unpaired list holds only justified entries, each feature's scene and photos pass.
- [ ] T023 [US4] Traffic: as T022. **Done when**: as T022.
- [ ] T024 [US4] Street furniture: as T022. **Done when**: as T022.
- [ ] T025 [US4] Lots: as T022. **Done when**: as T022.
- [ ] T026 [US4] Roads: as T022. **Done when**: as T022.
- [ ] T027 [US4] Effects, saves, camera, terrain, sky: as T022, one area at a time. **Done when**: as T022 for each.
- [ ] T028 [US4] Junctions, signals, zones, transit, growth: as T022, one area at a time. **Done when**: as T022 for each.
- [ ] T029 [US4] Buildings and interface: as T022. **Done when**: as T022.

## Phase 9: User Story 7 - Beyond the reference (P2)

- [ ] T030 [US7] Research the genre's systems with cited sources (the publishers' feature pages and manuals of Cities: Skylines, Cities: Skylines II, SimCity 2013) into doc/specs/003-roadcraft-features/genre-gaps.md: each system marked present here (its scene), reference only, or neither. **Done when**: every row has its source and status.
- [ ] T031 [US7] Bring each "neither" system of genre-gaps.md as items of the loop, each by its cited method (sheet, code, scene, photos). **Done when**: each row is present or justified.
- [ ] T032 [US7] Judge each sheet's reference rules against their norms (FR-021) as the areas are brought; the differences recorded in the sheets. **Done when**: every sheet has its norm check.

## Phase 10: Polish & cross-cutting

- [ ] T033 Per block: the area's history (doc/history/<area>.md), the register's entries closed, the map's lines, the export (`tools/build/deliver.sh export`) when the game changed. **Done when**: each block's close passes the method check (T004).
- [ ] T034 On the owner's order: `tools/build/quick.sh --ready`, the whole gates against the register, km24 on two builds (`python tools/tasks/km24.py`). **Done when**: 0 NEW findings; km24 within the budgets of doc/specification.md (SC-012).

## Dependencies & order

- Setup (T001-T003) → Foundational (T004-T005) → US6 (T006-T008) and US3 (T009) → US2 (T010) → US1 (T011-T015) → US5 (T016-T021) → US4 (T022-T029) → US7 (T030-T032) → Polish per block.
- T012 before T013; T011 before T012; T016 before T017 before T018; T019 before T020; T021 after T015.
- US4's areas are independent of each other once US1-US3 stand.

## Parallel opportunities

- T006 (documents) and T007 (tools) and T008 (tracer) touch different files: [P] across them.
- T009 (look step) is independent of T010 (the sheet).
- Within US4, two areas whose files differ may be worked by helping agents in parallel; every result checked here (CLAUDE.md agents rule).

## Implementation strategy

- MVP: Phase 1-2, US6, US3, US2's camera sheet and US1 (the camera): the ground true and the camera right.
- Then US5's open items, then the port area by area, each item committed and pushed as it passes.
