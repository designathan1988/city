# City: a city builder in Godot 4.7.2 with a C++ core (GDExtension), to be sold on Steam

Generated from `.specify/memory/constitution.md` (version 2.0.0): the owner's orders, nothing else. `doc/map.md` lists every file.

## What the work is

Port the reference game's features (`C:\Users\Jonathan\Documents\Projects\lab\roadcraft\src`, TypeScript) into this game, the main
systems first, in the order of `doc/specs/003-roadcraft-features/tasks.md`. Porting is not copying: the tracer shows what the
reference does, and the feature is built in this game's architecture with this game's own design and names.

## One feature, in this order

1. Take the next feature of `tasks.md`. The unit of work is a feature (a system the player uses), never a single register problem.
2. One look, once: the test, measurement or photo that shows the feature's state now (`python tools/tasks/cli.py run <task>`).
3. Trace both games with `tools/tracer/trace.py` (`--feature <path:line>`, `START --depth N --state`, `--conditions`, `--cases`,
   `--cost` on the reference, `--wrong` and `--broken` here) and read the files it reaches. Write the feature's sheet in
   `doc/specs/003-roadcraft-features/sheets/`: what it does, when, values and cases, the reference's faults not to bring.
4. Research: Context7 for each API the code calls; the internet for the design decisions; as deep as the feature matters to the
   player. Values of the real world from a norm or a published method, not the reference's numbers.
5. `python tools/tasks/cli.py block open <feature>`, then code the whole feature, every case of its sheet, with nothing run: no
   build, test, measurement or photo until the feature is written.
6. `python tools/tasks/cli.py block verify`, then the checks: both DLLs (`tools/build/build.sh core-windows`), the feature's test
   scenes and the suites it reaches, its measure against the budgets, photos aimed at its elements (day and night when light
   changes), each photo opened and looked at. A failure: `superpowers:systematic-debugging`, the cause fixed in the code, only the
   failed check run again.
7. A pass: commit (the cause or the feature, with `file:line`) and push to `origin/main`; `block close`; the next feature.

## Focus

- A defect seen outside the feature goes to the register in one line (`python tools/atlas/cli.py problems add`) and is not worked
  now. Register problems are worked only when they block the feature in hand or the owner names them.
- A feature is done when its sheet's cases work and its checks pass; a done feature is not polished further.

## Never

- Copy the reference: no line, translation, structure, name or data key (the gate LAB_COPY, `tools/gates/lab_scan.py`).
- Bring its costs: whole-map work on an edit or a frame, per-frame work on unchanged inputs, allocation or search in a hot loop,
  quadratic proximity, heavy or hidden geometry, a simulation tied to the camera.
- Patch a symptom: a timeout, threshold or tolerance moved to pass, a special case, a swallowed error, a loosened check, a feature
  switched off, quality lowered, `--bless` to pass. The fix changes the design decision that makes the symptom, whatever its size.
- A force push, a rewritten history, `rm -rf` or a deletion by a relative path, a deletion outside the project, the owner's open
  game closed.

## Architecture (build every feature to it)

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
- Interface: no game numbers; reads `CityStore.snapshot`, sends `CityStore.dispatch(...)`; texts through `tr("KEY")` from
  `project/data/i18n`; screens, pause and quit through `GameFlow`.
- Files start with a comment on their responsibility, split by responsibility; the gates of `tools/gates/rules.json` hold.
- The bar: `doc/specification.md` (median: 6 cores, 16 GB, RTX 3060 class, 1080p `medium` at 60 fps; floor: 4 cores, 8 GB,
  integrated GPU, 1080p `low` at 30 fps). Performance and look are part of every feature.

## Working

- Skills are loaded whenever they help; none is forbidden. The one order a skill cannot change: nothing runs while a feature is
  coded.
- Routine doubts are settled by the standard games' practice and the project's documents; the work does not stop to ask.
- Processes show their output as they run and have a deadline; tests and captures run hidden through `tools/tasks`; one engine
  at a time; only your own processes stopped, by PID; `tools/build/free_dll.sh` frees the DLL the owner's open game holds.
- Agents may read, trace and research beside the work; what one brings is checked in the files before it is used.
- Work on `main`; commit and push per feature. Repository text in English; the owner is spoken to in Portuguese, once the work is
  done: what was done, what failed, what is open, with proof.
- Records: the register through `python tools/atlas/cli.py problems`; `doc/map.md` (a file created or deleted enters it in the same
  commit); the work's memory `doc/work/<block>.md`, short (goal, sheet, plan, state, next), folded into `doc/history/<area>.md` at
  the close. The work stays in this folder; outside it only the reference is read.
- Only on the owner's order: `tools/build/quick.sh --ready`, the whole gates, `python tools/tasks/km24.py`, `python tools/hunt/hunt.py`.

## Commands

| What | Command |
|---|---|
| A feature's block | `python tools/tasks/cli.py block open <name>` · `block verify` · `block close` · `block status` |
| Trace | `python tools/tracer/trace.py <START> --depth N --state --root <feature files>` (`--list` names the starts) |
| What each game has | `python tools/atlas/cli.py inventory [--area A] [--game reference\|city] [--unpaired]` |
| Build both DLLs | `tools/build/build.sh core-windows` |
| One test or photo plan | `python tools/tasks/cli.py run <task>` (names: `python tools/tasks/cli.py run tests --list`) |
| Gates of a file | `python tools/gates/run_gates.py --file X` |
| The register | `python tools/atlas/cli.py problems [--area A --status S]`, `add`, `update`, `close`, `drop` |
| Deliver | `tools/build/deliver.sh export` (`dist/builds/<date>_<time>_<commit>/Roadcraft.exe`) |

The DLLs in `project/bin` are versioned. A clone needs Godot 4.7.2, llvm-mingw `20261020-ucrt-x86_64`, CMake 4.4, Python 3.12, Node
23.6+ (`npm ci` in `tools/tracer/tracer/tools`), an x86-64-v3 processor.
