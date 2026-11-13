# City Constitution

City is a city builder in Godot 4.7.2 with a C++ core (GDExtension), to be sold on Steam. This file holds the owner's orders;
`CLAUDE.md` carries the same orders for Claude Code.

## Core Principles

### I. The owner's orders

- This constitution and the owner's words in the conversation are the orders; no other text adds rules. An order of the owner that
  changes how the work is done enters this file and `CLAUDE.md` in the same commit.
- Skills and Context7 are used at every step, while coding too.
- Routine doubts are settled by the standard games' practice and the project's documents; the work does not stop to ask.
- Never: a force push, a rewritten history, `rm -rf` or a deletion by a relative path, a deletion outside the project, the owner's
  open game closed.
- The owner hears from the work in Portuguese when it is done: what was done, what failed, what is open, with proof. Asked why,
  the answer is the real reason.

Rationale: the work stalled for days under rules the owner never gave (constitution 1.0.0 to 1.4.0, 2026-11-13).

### II. Port the reference's features by the tracer, never its code

- The priority: bring every feature of the reference game (`C:\Users\Jonathan\Documents\Projects\lab\roadcraft\src`) into this
  game, complete, the main systems first, in the order of `doc/specs/003-roadcraft-features/tasks.md`. An order of the owner names
  a start, never the whole: what it implies (the neighbouring features, the causes, the rest of the area) is found and done.
- Why this game exists: the reference was abandoned because it could not make a medium or a large city: memory, CPU and GPU grew
  with the map until it was too heavy and too slow to play. Its behaviour is mostly right; its performance, architecture and
  engineering are not. Every feature here is judged at the large city's scale (the 24 km map): what it holds per record, what an
  edit, a tick and a frame cost there, measured at the feature's end.
- Nothing of the reference is copied: no line, translation, structure, name or data key (the gate LAB_COPY and
  `tools/gates/lab_scan.py` find them). Its faults are identified and never brought: whole-map work on an edit or a frame,
  per-frame work on unchanged inputs, allocation or search in a hot loop, quadratic proximity, heavy or hidden geometry, a
  simulation tied to the camera, swallowed errors, diagnostics left running (the full list:
  `doc/specs/003-roadcraft-features/architecture.md`).
- The tracer (`tools/tracer/trace.py`) is how a feature is understood, in both games: `--feature <path:line>` from where it shows,
  `START --depth N --state`, `--conditions`, `--cases`, `--cost` on the reference, `--wrong` and `--broken` here. The files it
  reaches are read. What it finds goes to the feature's sheet: what the feature does, when, with which values and cases, and the
  reference's faults not to bring.
- The feature is built in this game's architecture (below), with this game's own design and names. Where the game models
  something real, its values come from a norm or a published method, not from the reference's numbers.

### III. One feature at a time: one look, research, code whole, test at the end

- The unit of work is a feature of the port (a system the player uses), never a single register problem.
- Before the code: one look, once (the test, measurement or photo that shows the feature's state now), the trace and the sheet,
  and the research: Context7 for each API the code calls, the internet for the design decisions, as deep as the feature matters
  to the player.
- Then the whole feature is coded, every case of its sheet, with no build, test, measurement or photo while coding (`block open`
  makes the tools refuse them); Context7 and the skills go on being used while coding.
- At the end (`block verify`): both DLLs, the feature's test scenes and the suites it reaches, its scale probe on the 24 km map
  against the budgets,
  photos aimed at its elements (day and night when light changes), each photo looked at. A failure is read to its cause
  (`superpowers:systematic-debugging`) and fixed in the code; only the failed check runs again.
- A pass: commit and push to `origin/main`, then the next feature.

Rationale: a check after every small change made 80 builds and about 175 runs in one session (doc/history/process.md).

### IV. Focus on the feature

- A defect seen outside the feature (in a photo, a log, a file) goes to the register in one line (`python tools/atlas/cli.py
  problems add`) and is not worked now.
- Register problems are worked only when they block the feature in hand or the owner names them.
- A feature is done when its sheet's cases work and its checks pass; a done feature is not polished further.

Rationale: the people area spent a day on the fallen bodies' details while the main systems waited (git log, 2026-11-13).

### V. Fix the cause

- The fix changes the design decision that makes the symptom, whatever its size. No patch: a timeout, threshold or tolerance moved
  to pass, a special case, a swallowed error, a loosened check, a feature switched off, quality lowered, a reference rewritten
  (`--bless`) to pass.
- The commit names the cause with its `file:line`.

## Architecture and the bar

- One source of truth, `CityState` (`native/src/sim/state/city_state.h`): only `sim/commands` changes it, only `sim/simulation`
  advances time. Layers: `core` -> `sim/*` (pure C++, no Godot) -> `render` (only reads) -> `api`.
- Threads: `Simulation` on its own thread at a fixed tick (`sim_tick_hz`); commands and the clock take `map_mutex` exclusively;
  `ChangeLog` is the one dirty channel; a mesh job copies under a short shared lock and builds unlocked, never reading the traffic;
  mesh conversion within `render.upload_budget_mb` a frame, the commit within `render.commit_budget_ms`; poses through `PoseBuffer`.
- Data: tunable values only in `project/data/**` (JSON with a schema in `tools/gates/schemas`), read by `DataTables`; tuning names
  in `CITY_TUNING_FIELDS`, values in `project/data/catalog/tuning.json`; no magic numbers in `native/src`, `project/game`,
  `project/ui`.
- Owners: one per definition (`tools/gates/owners.json`); `TerrainBridge` the land's maps, `BodyCache` a record's body, `Daylight`
  the sun, ambient and sky, `LampLights` the street's light, `CityCamera` the camera.
- Interface: no game numbers; it reads `CityStore.snapshot` and sends `CityStore.dispatch(...)`; texts through `tr("KEY")` from
  `project/data/i18n`; screens, pause and quit through `GameFlow`.
- Files start with a comment on their responsibility and are split by responsibility; the gates of `tools/gates/rules.json` hold
  (sizes, magic numbers, English, LAB_COPY, the map).
- The bar: the budgets of `doc/specification.md` (median: 6 cores, 16 GB, RTX 3060 class, 1080p `medium` at 60 fps; floor: 4
  cores, 8 GB, integrated GPU, 1080p `low` at 30 fps). Performance and look are part of every feature.

## Tools, processes and records

- A process shows its output as it runs and has a deadline; tests and captures run hidden through `tools/tasks`; one engine at a
  time; only your own processes are stopped, by PID; the owner's open game is never closed (`tools/build/free_dll.sh` frees the
  DLL). A check over a minute is a defect of its tool.
- Agents may read, trace and research beside the work; what one brings is checked in the files before it is used.
- Work on `main`; commit and push per feature. Repository text in English; the owner is spoken to in Portuguese.
- A feature's scale is measured by its own probe on the 24 km map (memory per record, time per edit, per tick, per frame), a
  check under a minute. Only on the owner's order: `tools/build/quick.sh --ready`, the whole gates, `python tools/tasks/km24.py`
  (two builds compared), `python tools/hunt/hunt.py`.
- Records: the register `doc/problems.jsonl` through `python tools/atlas/cli.py problems`; `doc/map.md` lists every file (a file
  created or deleted enters it in the same commit); each feature's sheet in `doc/specs/003-roadcraft-features/sheets/`; the work's
  memory `doc/work/<block>.md`, short (goal, sheet, plan, state, next), folded into `doc/history/<area>.md` at the close.
- The work stays in `C:\Users\Jonathan\Desktop\city`; outside it only the reference is read.

## Governance

- The owner's words win over every text. An order of the owner that changes the work amends this file and `CLAUDE.md` in the
  same commit; its reason goes to `doc/history/process.md`.
- Versions: MAJOR when a principle is removed or redefined, MINOR when one is added or widened, PATCH for wording.
- `CLAUDE.md` names the version it carries (the gate CLAUDE_MD).

**Version**: 2.1.1 | **Ratified**: 2026-11-13 | **Last Amended**: 2026-11-13
