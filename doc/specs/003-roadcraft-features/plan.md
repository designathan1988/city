# Implementation Plan: Bring the reference game's features, correctly and fast

**Branch**: `main` | **Date**: 2026-11-13 | **Spec**: [spec.md](spec.md)

**Input**: Feature specification from `doc/specs/003-roadcraft-features/spec.md`

## Summary

Six stories, one loop. The loop for every item (CLAUDE.md method, the owner's orders of 2026-11-13): one look before the code; the
reference traced and every file the trace reached read whole into a comparison sheet; Context7 for every API; the code written
whole with nothing run; then the item's own test scenes and a photo plan aimed at what changed, in seconds, the photos analysed
region by region; commit, push, next item. First the ground the rest stands on: the documents and tools made one true set (US6),
the loop's tools fast and honest (US3), the inventory and the sheets (US2); then the camera rebuilt to the reference's behaviour
(US1), the open items (US5), and the port of every reference feature area by area (US4) until the inventory shows nothing the
player uses unpaired or unjustified.

## Technical Context

**Language/Version**: C++20 (GDExtension core, llvm-mingw 20261020), GDScript (Godot 4.7.2), Python 3.12 (tools), TypeScript
(the reference, read only)

**Primary Dependencies**: godot-cpp, Terrain3D, mimalloc, meshoptimizer, Clipper2; the project's tools: tools/tasks (orchestrator),
tools/gates, tools/atlas (register, inventory), tools/tracer

**Storage**: the city's save files; doc/problems.jsonl (the register); dist/ outputs held to their lifecycle bounds

**Testing**: the test scenes and look plans through `python tools/tasks/cli.py run <task>`; the tools' unittest suites; the gates

**Target Platform**: Windows desktop (Steam), median and floor profiles of doc/specification.md

**Project Type**: desktop game with a native core and its support tools

**Performance Goals**: median profile 60 fps at 1080p `medium`, floor 30 fps at `low` (doc/specification.md); the 24 km opening
within its budget (Pperformance-7); every check, tool and suite task under 60 s; the inventory under 5 s

**Constraints**: CLAUDE.md whole: the architecture (CityState, commands, simulation, render read-only, DataTables, one owner), no
magic numbers, file sizes, English, nothing of the reference copied, no slow/silent/unwatched/blocking process, no hooks

**Scale/Scope**: the reference: 866 files, 3647 features in the inventory; this game: 1172 files, 4902 features; 2998 pairs today;
"reference only" per area from 9 (growth) to 347 (buildings)

## Constitution Check

The constitution is CLAUDE.md. Gates, each with how this plan meets it:

| Gate (CLAUDE.md) | Met by |
|---|---|
| Read whole before code | each item's sheet lists every file read `path: 1-N of N` with the hour (city-method section 0, 3 item 4) |
| Context7 before every implementation | each item's work file `Research` section; the block's close refuses without it (R8, T006) |
| Trace both games before building | each feature's comparison sheet from the tracer on both (R3) |
| One look, then code whole with nothing run | the per-item loop (quickstart step 2); the block tool refuses runs while open |
| Checks under a minute, lines shown, deadlines | the orchestrator (live lines, no lock reservation, start deadlines: FR-012, done in 56926e1b); the two slow scenes fixed (R4, R5) |
| Fix the cause, never a patch | each item names the design decision at fault with file:line in its sheet or work file |
| Nothing of the reference copied | the copy scan per item; the text keys renamed and LAB_COPY extended to the text tables (FR-015) |
| Architecture | camera solids as the core's queries over CityState under a shared lock; the rig the camera's one writer; values in camera.json |
| Commit and push per item | the per-item loop's last step |

No violation needs justifying.

## Project Structure

### Documentation (this feature)

```text
doc/specs/003-roadcraft-features/
├── spec.md
├── plan.md              # this file
├── research.md          # R1-R8
├── data-model.md
├── quickstart.md        # the per-item loop and the validations
├── contracts/
│   ├── inventory-cli.md
│   ├── comparison-sheet.md
│   ├── look-plan-steps.md
│   ├── camera-solids.md
│   └── block-close.md
├── sheets/              # one comparison sheet per feature brought (US2, US4)
├── checklists/requirements.md
└── tasks.md             # /speckit-tasks
```

### Source Code (repository root)

```text
native/src/
├── sim/buildings/building_store.{h,cpp}     # BuildingRecords::lots / meeting (done)
├── sim/simulation/growth.cpp                # the opening's lot fill (R4)
├── sim/buildings/b40/likeness.*             # a candidate's look from its massing (R4)
├── sim/commands/lot_sites.cpp               # settle_sites (R4)
└── api/city_core_sight.cpp                  # the camera's solids: roof, floor field, eye_free, highest_roof (R1)
project/
├── game/camera/{city_camera,camera_ahead,camera_pull,camera_profile}.gd   # the rig to the reference's behaviour (R1)
├── data/game/camera.json (+ tools/gates/schemas/camera.schema.json)       # the camera's values (R1)
├── data/i18n/*.csv and every tr() caller                                   # the text keys renamed (FR-015)
└── tests/{core/camera_*_test.gd, core/window_traffic_test.gd, visual/look*.gd, visual/*_plan.json}
tools/
├── atlas/inventory*.py                       # done (R2)
├── tasks/{block.py, pool.py, ...}            # the close's method check (R8)
└── gates/{gate_lab.py, lab_scan.py, rules.json}   # LAB_COPY over the text tables (FR-015)
CLAUDE.md, .claude/skills/city-method/SKILL.md, doc/**                    # US6
```

**Structure Decision**: the existing layout; no new top-level folder. Comparison sheets live under the specification (documents
in doc/, CLAUDE.md).

## Phases

1. **Ground** (US6, US3, US2): the document and tool surveys (R7) and their fixes; the block close's method check (R8); the
   inventory committed with its tests; the camera_feel_test under a minute (R5).
2. **Camera** (US1, R1): the floor field, the margin step back, the reference's lift rule, the near plane by the gap, the values in
   camera.json; the camera tests' criteria (R1 last paragraph); photos at street, mid and far.
3. **Open items** (US5): the opening (R4), the text keys (FR-015), the frozen street view and roofs plan (photos).
4. **Port** (US4): area by area in the inventory's order (people, traffic, street furniture, lots, roads, effects, saves, ...), each
   feature an item of the loop with its sheet.

## Complexity Tracking

None.
