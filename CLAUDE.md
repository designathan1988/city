# City: a city builder in Godot 4.7.2 with a C++ core (GDExtension), to be sold on Steam

This file is the project's orders, whole; nothing else adds a rule. The owner's words in the conversation come first. The owner
is spoken to in Portuguese; repository text is English.

## The work: port the reference game, feature by feature

- The reference: `C:\Users\Jonathan\Documents\Projects\lab\roadcraft\src` (TypeScript). It was abandoned because it could not
  make a medium or a large city: memory, CPU and GPU grew with the map. Its behaviour is mostly right; its performance,
  architecture and engineering are not. Every feature it has is brought here, complete, and judged at the large city's scale.
- Porting is not copying: the tracer shows what the reference does; the feature is built in this game's architecture, with this
  game's own design and names. An order of the owner names a start, never the whole: what it implies is found and done.
- **Where the port stands: `doc/port.md`.** One line a feature, in order; the first unchecked line is the next. The session's
  start prints it with the open block and its work file (`python .claude/hooks/port_state.py` prints it again). Start there;
  never ask what to do next.

## One feature, start to end

1. **The feature.** The first unchecked line of `doc/port.md` and its row in `doc/specs/003-roadcraft-features/sheets/<area>.md`
   (what the player meets, what differs, the evidence on both sides with `file:line`). The unit of work is a feature the player
   uses, never a single register problem.
2. **One look, once**: the test, measurement or photo that shows the feature's state now (`python tools/tasks/cli.py run <task>`).
3. **Trace both games and read.**
   - `python tools/tracer/port.py sheet <area> --feature N`: both games traced from the row's evidence in one run (what each
     side holds that the other lacks: constants, state, data; each side's loops past linear), report in `dist/port/<area>.json`.
   - `python tools/tracer/trace.py --list --root <file>` names the starts; then `<START> --depth N --state --root <files>`,
     `--feature <path:line>`, `--conditions`, `--cases`, `--cost` on the reference; `--wrong` and `--broken` here.
   - Every file the trace reaches is read. The feature's sheet is written in `sheets/<area>-<feature>.md` (its sections:
     `contracts/comparison-sheet.md`): what it does, when, values and cases, the reference's faults not to bring, the design here.
4. **Research.** Context7 (`resolve-library-id`, then `query-docs`) for each API the code calls; the internet for the design
   decisions, as deep as the feature matters to the player. Values of the real world from a norm or a published method, not the
   reference's numbers.
5. **Code it whole.** `python tools/tasks/cli.py block open <feature>`; `doc/work/<feature>.md` kept current (goal, sheet, plan,
   state, next) so a compaction or a new session goes on from it; every case of the sheet coded, with no build, test,
   measurement or photo until the feature is written.
6. **Check at the end.** `python tools/tasks/cli.py block verify`, then: both DLLs (`tools/build/build.sh core-windows`); the
   feature's test scenes and the suites it reaches; `python tools/tracer/port.py check` when the feature carries a law of the
   reference (its pair in `project/tests/port`); `python tools/gates/lab_scan.py` (nothing copied) and
   `python tools/gates/run_gates.py --file X` on each changed file; the scale probe on the 24 km map against the budgets (built
   by T038, the board's first line); photos aimed at the feature's elements (day and night when light changes), each one opened
   and looked at. A failure: `superpowers:systematic-debugging`, the cause fixed in the code, only the failed check run again.
7. **A pass.** The feature's line checked in `doc/port.md`; commit (the feature or the cause, with `file:line`) and push to
   `origin/main`; `block close`; the work file folded into `doc/history/<area>.md`; the next line.

Skills and Context7 are used at every step that needs them (the Skill tool; Context7 for every library, engine or API the work
touches), in every session.

## Focus

- A defect seen outside the feature goes to the register in one line (`python tools/atlas/cli.py problems add`) and is not worked
  now. Register problems are worked only when they block the feature in hand or the owner names them.
- A feature is done when its sheet's cases work and its checks pass; a done feature is not polished further.

## Never

- Copy the reference: no line, translation, structure, name or data key (the gate LAB_COPY, `tools/gates/lab_scan.py`).
- Bring its costs: whole-map work on an edit or a frame, per-frame work on unchanged inputs, allocation or search in a hot loop,
  quadratic proximity, heavy or hidden geometry, a simulation tied to the camera (the full list, with the reference's places:
  `doc/specs/003-roadcraft-features/architecture.md`).
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
- How a feature lands (the error channel, outcomes, record invariants, removal, cost): `doc/specs/003-roadcraft-features/architecture.md`.
- Files start with a comment on their responsibility, split by responsibility; the gates of `tools/gates/rules.json` hold.
- The bar: `doc/specification.md` (median: 6 cores, 16 GB, RTX 3060 class, 1080p `medium` at 60 fps; floor: 4 cores, 8 GB,
  integrated GPU, 1080p `low` at 30 fps). Performance and look are part of every feature.

## Working

- Routine doubts are settled by the standard games' practice and the project's documents; the work does not stop to ask.
- Processes show their output as they run and have a deadline; tests and captures run hidden through `tools/tasks`; one engine
  at a time; only your own processes stopped, by PID; `tools/build/free_dll.sh` frees the DLL the owner's open game holds.
- Agents may read, trace and research beside the work; what one brings is checked in the files before it is used.
- Work on `main`; commit and push per feature. The owner hears, in Portuguese, once the work is done: what was done, what
  failed, what is open, with proof.
- Records: the board `doc/port.md`; the work's memory `doc/work/<block>.md`; the register through
  `python tools/atlas/cli.py problems`; `doc/map.md` (a file created or deleted enters it in the same commit); the histories
  `doc/history/<area>.md`. Claude Code's auto memory is on: an owner's correction or preference no document holds is saved
  there; the port's state lives in the board and the work file, never in memory alone.
- The work stays in this folder; outside it only the reference is read.

## Commands

| What | Command |
|---|---|
| Where the port stands | `python .claude/hooks/port_state.py` (the board: `doc/port.md`) |
| Both games traced for a feature | `python tools/tracer/port.py sheet <area> [--feature N]` · `python tools/tracer/port.py check` |
| Trace | `python tools/tracer/trace.py <START> --depth N --state --root <feature files>` (`--list` names the starts) |
| What each game has | `python tools/atlas/cli.py inventory [--area A] [--game reference\|city] [--unpaired]` |
| A feature's block | `python tools/tasks/cli.py block open <name>` · `block verify` · `block close` · `block status` |
| Build both DLLs | `tools/build/build.sh core-windows` |
| One test or photo plan | `python tools/tasks/cli.py run <task>` (names: `python tools/tasks/cli.py run tests --list`) |
| Gates of a file · nothing copied | `python tools/gates/run_gates.py --file X` · `python tools/gates/lab_scan.py` |
| The register | `python tools/atlas/cli.py problems [--area A --status S]`, `add`, `update`, `close`, `drop` |
| Deliver | `tools/build/deliver.sh export` (`dist/builds/<date>_<time>_<commit>/Roadcraft.exe`) |
| Only on the owner's order | `tools/build/quick.sh --ready`, the whole gates, `python tools/tasks/km24.py`, `python tools/hunt/hunt.py` |

The DLLs in `project/bin` are versioned. A clone needs Godot 4.7.2, llvm-mingw `20261020-ucrt-x86_64`, CMake 4.4, Python 3.12, Node
23.6+ (`npm ci` in `tools/tracer/tracer/tools`), an x86-64-v3 processor.
