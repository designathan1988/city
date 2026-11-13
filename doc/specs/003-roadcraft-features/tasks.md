# Tasks: Bring the reference game's features, correctly and fast

**Input**: doc/specs/003-roadcraft-features/ (spec.md, plan.md, research.md, data-model.md, contracts/, quickstart.md, genre-gaps.md)

**Order**: the port first (each feature's behaviour learnt and built in this game's design, never copied); then what the reference
lacks or does wrong; the method's tools and the records where the port meets them; the camera last.

**Every task is an item of the loop** (quickstart.md section 2): the feature's outputs found in both games by a Grep for the thing
itself; the tracer on each (`--feature`, `START --depth N --state`, the feature's files as roots); every file it reached read whole into
the feature's sheet (contracts/comparison-sheet.md: rules, cases, values with file:line, what the player sees, the reference's faults
never to bring, its rule judged against its norm); the design here; Context7 for every API; code whole with nothing run; its own scenes
and a photo plan aimed at it, in seconds, the photos analysed region by region; commit and push; next. **Done when** is each task's
verifiable goal.

## Format: `[ID] [P?] [Story] Description`

## Phase 1: Setup (done)

- [x] T001 Register the problems this specification found (Pinterface-139, Ptools-112, Ptools-113, Ptools-114, Pinterface-140 in doc/problems.jsonl).
- [x] T002 Commit the inventory (tools/atlas/inventory*.py, tests, cli; commit d07ecd14).
- [x] T003 The constitution and CLAUDE.md generated from it (commit 300d89a9).

## Phase 2: What the port needs before its first feature

- [ ] T004 [US3] Add the look step `setViewNear [kind, x, y, zoom, azimuth, elevation]` (contracts/look-plan-steps.md) in project/tests/visual/look_aim.gd and look.gd, kinds pole, vehicle, walker, building, sign, each found by the core's existing probes (`shot_targets`, `street_watch`, `probe_building`); finding none is a FAIL. Every port feature's photos aim at its element with it. **Done when**: a plan with one of each kind takes its shots and prints what it found, under 60 s; look.gd stays under 250 lines.
- [ ] T005 [US5] The frozen street view and the roofs plan aimed with T004 (project/tests/visual/frozen_plan.json, roofs_plan.json): the street view at a walker's place (`setViewNear walker`), the roofs at `setViewRoof`. **Done when**: `python tools/tasks/cli.py run photos roofs` under 60 s; the street photo shows a street with the eye outside every building; near and mid roof photos show roofs from above; analysis written per photo.
- [X] T006 Set the fix-three block's camera work aside until the camera area (T027): commit its parts that pass their checks (the lot-box index `BuildingRecords::meeting`, Psimulation-83; `highest_roof` and `setViewRoof`; CameraHeights and CameraAim split out of the rig, Pinterface-28; camera_motion's north turn reading `turn_smooth_s`), and leave the eased-margin rig for T027, so the port's DLLs build from committed sources. **Done when**: look_view_test passes; the committed tree builds both DLLs with no warning; camera_feel_test's state recorded as Pinterface-140's evidence.

- [ ] T031 [US3] Make the block's close check the method (contracts/block-close.md) in tools/tasks/block.py with tests in tools/tasks/tests/test_block.py: refuses without `## Read`, `## Research` (a Context7 line when code changed) and `## Skills`, or with two failing runs of one check and no cause between (the constitution, principle IV, states the close refuses: it must before the port's first item). **Done when**: the tests prove each refusal and a complete file closing, under 60 s.

## Phase 3: User Story 4 - The port, area by area (P1, first)

**Goal**: every reference feature the player uses brought, by its behaviour, in this game's design, or justified. **Independent test**:
per area, `python tools/atlas/cli.py inventory --area <area> --unpaired` lists only entries a sheet justifies; each feature's scene and
photos pass in seconds. Order: the player's gaps by area (inventory, 2026-11-13: reference-only entries in brackets).

For each area, two tasks: first the area's features grouped and their sheets written (no code); then each feature built as one item.

- [ ] T007 [US4] People [159]: group the unpaired entries into features, one sheet each in doc/specs/003-roadcraft-features/sheets/people-<feature>.md (state `read`: every file the trace reached read whole in both games), the area's index sheets/people.md. **Done when**: every unpaired entry belongs to a sheet or is justified there.
- [ ] T008 [US4] People: build each feature of T007 as one item (sheet `built` then `paired`). **Done when**: the area's unpaired list holds only justified entries; each feature's scene and photos pass; the copy scan finds nothing.
- [ ] T009 [US4] Traffic [73]: as T007. **Done when**: as T007.
- [ ] T010 [US4] Traffic: as T008. **Done when**: as T008.
- [ ] T011 [US4] Street furniture [138]: as T007. **Done when**: as T007.
- [ ] T012 [US4] Street furniture: as T008. **Done when**: as T008.
- [ ] T013 [US4] Lots [36] and growth [9]: as T007. **Done when**: as T007.
- [ ] T014 [US4] Lots and growth: as T008, with the 24 km opening (research R4): a candidate's look from its massing only (native/src/sim/simulation/growth.cpp, native/src/sim/buildings/b40, body_cache.cpp); `fill_all` in a 2 x 2 checkerboard of 256 m cells over the job pool, the same city for any pool size; `settle_sites` reusing the edges `record_for` read, its sites made in parallel and inserted in order (native/src/sim/commands/lot_sites.cpp). **Done when**: as T008; city_mass_test, lot_front_test, lot_overlap_test, edit_fuzz_test, save_undo_test, traffic_hash_test pass; window_traffic_test ends under 60 s with the same check; the digest written again with its cause in doc/history/game.md.
- [ ] T015 [US4] Roads [181]: as T007. **Done when**: as T007.
- [ ] T016 [US4] Roads: as T008. **Done when**: as T008.
- [ ] T017 [US4] Junctions [44] and signals [13]: as T007 then T008. **Done when**: as T008.
- [ ] T018 [US4] Zones [23] and transit [9]: as T007 then T008. **Done when**: as T008.
- [ ] T019 [US4] Effects [32] and sky [34]: as T007 then T008. **Done when**: as T008.
- [ ] T020 [US4] Terrain [121]: as T007 then T008. **Done when**: as T008.
- [ ] T021 [US4] Saves [16]: as T007 then T008. **Done when**: as T008; edit_fuzz_test passes undo, redo, save and load through the same hashes.
- [ ] T022 [US4] Buildings [347]: as T007. **Done when**: as T007.
- [ ] T023 [US4] Buildings: as T008. **Done when**: as T008.
- [ ] T024 [US4] Interface [130]: as T007 then T008, with the text keys (FR-015): LAB_COPY and lab_scan read the text tables (tools/gates/gate_lab.py, lab_scan.py, rules.json), then this game's keys the reference defines renamed area by area in project/data/i18n/*.csv and every `tr("KEY")` and data `*_key`. **Done when**: as T008; the gate's fixtures prove the new finding; the scan finds no reference key.
- [ ] T025 [US4] Lab [129]: each entry justified (a lab-only tool) or brought where the player uses it. **Done when**: the area's unpaired list holds only justified entries.
- [ ] T026 [US4] Camera [20]: the sheet (doc/specs/003-roadcraft-features/sheets/camera.md) from research R1 and R1b, with the proof before code that R1b names (the point's worst vertical acceleration from `floor_slope` and the ground spring; the sweep's continuity). **Done when**: every section of the sheet filled with file:line and the proof's numbers below the jolt limit.
- [ ] T027 [US4] Camera: build R1b (contracts/camera-solids.md): the camera floor (the core's derived tiles, bounded LRU, ChangeLog), `eye_clear`, the point's rest over the floor by the zoom weight, the eye's body pushed toward the point, the pull unchanged, the values in camera.json and its schema; `eye_free`, `eye_floor` and the margin values removed. **Done when**: `python tools/tasks/cli.py run camera_feel_test camera_input_test look_view_test` passes, each under 60 s: eye inside walls at most 1 frame in 1,000, no frame's jolt past 1% of the view outside the counted push-outs, push-outs at most 0.1% of frames (SC-001, SC-002); camera photos at a wall (street, mid, far, four bearings) analysed.

## Phase 4: User Story 7 - Beyond the reference (P2)

- [x] T028 [US7] The genre's systems with cited sources: genre-gaps.md (18 systems; Cities: Skylines II's economy, lifepath and traffic pages checked).
- [ ] T029 [US7] Bring each system of genre-gaps.md that neither game has (water and sewage networks, happiness and well-being, land value, pollution, disasters, modding), in that order, each an item of the loop by its cited method, measured against the budgets. **Done when**: each row present (its scene and photos) or justified.
- [ ] T030 [US7] Each port sheet judges the reference's rule against its norm (FR-021) and records where this game goes further. **Done when**: every sheet of Phase 3 has its norm check.

## Phase 5: The method and the records, where the port meets them

- [ ] T032 [US6] Fix each document and tool finding of research R7 when a port item meets it, and the rest after Phase 3 (an unused tool removed, a broken one fixed at its cause, each stale command or path made true). **Done when**: a re-read of R7's items finds each fixed; each remaining tool has a caller and its tests pass under 60 s.
- [ ] T033 [US6] The tracer's empty `--cost` on `city.traffic.step` (tools/tracer/tracer/cost.py), needed by the traffic sheets (T009). **Done when**: `python tools/tracer/trace.py city.traffic.step --cost --root native/src/sim/traffic` prints the loops; `python -m unittest discover -s tools/tracer/tests` passes.

## Phase 6: Polish

- [ ] T034 Per block: the area's history, the register's entries closed, the map's lines, the export (`tools/build/deliver.sh export`) when the game changed. **Done when**: each block's close passes T031's check.
- [ ] T035 On the owner's order: `tools/build/quick.sh --ready`, the whole gates against the register, km24 on two builds. **Done when**: 0 NEW findings; km24 within doc/specification.md's budgets (SC-012).

## Dependencies & order

- Phase 2 (T004-T006, T031) → Phase 3 in its order (T007 → T008 → ... → T027; T033 before T009) → Phase 4 → Phase 5 as met → Phase 6 per block.
- Within an area: its grouping and sheets (no code) before any of its features is built.

## Parallel opportunities

- The sheets of one area (T007, T009, ...) may be read by helping agents in parallel, one feature each; every claim checked here before a line is coded (the constitution's agents rule). The building of features stays this session's.

## Implementation strategy

- MVP: Phase 2 and the first area (people, T007-T008): the loop proven on a real feature, committed and pushed per feature.
- Then each area in order; the camera last.
