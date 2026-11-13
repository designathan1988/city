# City: a city builder in Godot 4.7.2 with a C++ core (GDExtension), to be sold on Steam

This file is the project's orders, whole; nothing else adds a rule. The owner's words in the conversation come first. The owner
is spoken to in Portuguese; repository text is English.

## The work: port the reference game, one whole functionality at a time

- The reference: `C:\Users\Jonathan\Documents\Projects\lab\roadcraft\src` (TypeScript). It was abandoned because it could not
  make a medium or a large city: memory, CPU and GPU grew with the map. Its behaviour is mostly right; its performance,
  architecture and engineering are not. Every feature it has is brought here, complete, and judged at the large city's scale.
- Porting is not copying: the tracer shows what the reference does; the feature is built in this game's architecture, with this
  game's own design and names. An order of the owner names a start, never the whole: what it implies is found and done.
- A functionality is a whole system the player uses (the complete traffic, the complete roads, the complete buildings),
  never one of its rows and never a button. It is ported whole, in one block.
- **Where the port stands: `doc/port.md`.** One line a functionality, in order, its cases under it; the first unchecked line
  is the next. The session's
  start prints it with the command that traces it (`python .claude/hooks/port_state.py` prints it again). Told to port, start
  there at once; never ask what to do next.

## One functionality, start to end

No table, sheet, plan or report is written on the way: the port tool's report and the code are the record. No step is added.

1. **Load, before anything.** The skills the work needs (the Skill tool) and Context7's tools (ToolSearch,
   `select:mcp__plugin_context7_context7__resolve-library-id,mcp__plugin_context7_context7__query-docs`). The hook refuses an
   edit of the game's code until both were used in the session.
2. **The functionality.** The first unchecked line of `doc/port.md` with every case under it; its area is the port tool's
   argument.
3. **One look, once**: the one test or photo that shows the functionality's state now (`python tools/tasks/cli.py run <task>`). It is
   the only run before the end.
4. **Trace both games, then read.** `python tools/tracer/port.py sheet <area>` traces the whole functionality in both games
   in one run, about a minute (what each side holds that the other lacks; each side's loops; the files it reached, in
   `dist/port/<area>.json`). For a closer question: `python tools/tracer/trace.py <START> --depth N --state --root <files>`
   (`--list` names the starts; `--feature <path:line>`, `--conditions`, `--cases`, `--cost`). Then its code is read
   whole, in both games, by the session itself (no agent reads for it), until what it does, when, with which values and cases
   is understood, and which of the reference's costs must not come.
5. **Context7** (`resolve-library-id`, then `query-docs`) for each API the code will call; the internet for a design decision
   or a value of the real world (a norm or a published method, not the reference's number).
6. **Code the whole functionality.** `python tools/tasks/cli.py block open <area>`, then every case of it, complete, in
   this game's architecture and names. While it is coded nothing runs: no build, test, measurement, photo or gate.
7. **Check at the end, once.** `python tools/tasks/cli.py block verify`; both DLLs (`tools/build/build.sh core-windows`); its
   tests (`python tools/tasks/cli.py run <tasks>`); `python tools/gates/run_gates.py --file X` on the changed files
   and `python tools/gates/lab_scan.py` (nothing copied); photos aimed at its elements (day and night when light
   changes), each one opened, looked at and analysed for errors. A failure: `superpowers:systematic-debugging`, the cause read
   in the code and fixed there, only the failed check run again; never a guess tried by running.
8. **A pass.** The line checked in `doc/port.md`; commit (with `file:line`) and push to `origin/main`; `block close`; the next
   line at once.

Skills: `superpowers:systematic-debugging` on a failure, `superpowers:verification-before-completion` before saying done, the
`speckit-*` skills when the owner asks for a specification. A skill's step that runs a test while coding, writes a plan
document or stops to ask does not apply here: this file decides.

## Focus

- A defect seen outside the functionality goes to the register in one line (`python tools/atlas/cli.py problems add`) and is not worked
  now. Register problems are worked only when they block the functionality in hand or the owner names them.
- A functionality is done when its cases work and its checks pass; a done one is not polished further.

## Never

- Copy the reference: no line, translation, structure, name or data key (the gate LAB_COPY, `tools/gates/lab_scan.py`).
- Bring its costs: whole-map work on an edit or a frame, per-frame work on unchanged inputs, allocation or search in a hot loop,
  quadratic proximity, heavy or hidden geometry, a simulation tied to the camera (the full list, with the reference's places:
  `doc/specs/003-roadcraft-features/architecture.md`). Performance is settled by the design, not by measuring while coding.
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
- The port's tracing, reading and coding are the session's own work: no agent does them.
- Work on `main`; commit and push per feature. The owner hears, in Portuguese, once the work is done: what was done, what
  failed, what is open, with proof.
- Records, and no other: the board `doc/port.md`; `doc/map.md` (a file created or deleted enters it in the same commit, the
  gate MAP_OUTDATED asks it); the register (`python tools/atlas/cli.py problems`). A feature too long for one session leaves
  what is done and what is left in `doc/work/<block>.md`, a few lines. Claude Code's auto memory is on: an owner's correction
  or preference is saved there.
- The work stays in this folder; outside it only the reference is read.

## Commands

| What | Command |
|---|---|
| Where the port stands | `python .claude/hooks/port_state.py` (the board: `doc/port.md`) |
| Both games traced for a functionality | `python tools/tracer/port.py sheet <area> [--feature N]` · `python tools/tracer/port.py check` |
| Trace | `python tools/tracer/trace.py <START> --depth N --state --root <feature files>` (`--list` names the starts) |
| What each game has | `python tools/atlas/cli.py inventory [--area A] [--game reference\|city] [--unpaired]` |
| A feature's block | `python tools/tasks/cli.py block open <name>` · `block verify` · `block close` · `block status` |
| Build both DLLs | `tools/build/build.sh core-windows` |
| One test or photo plan | `python tools/tasks/cli.py run <task>` (names: `python tools/tasks/cli.py run tests --list`) |
| Gates of a file · nothing copied | `python tools/gates/run_gates.py --file X` · `python tools/gates/lab_scan.py` |
| The register | `python tools/atlas/cli.py problems [--area A --status S]`, `add`, `update`, `close`, `drop` |
| Deliver | `tools/build/deliver.sh export` (`dist/builds/<date>_<time>_<commit>/Roadcraft.exe`) |
| Only on the owner's order | measurements at scale (`python tools/tasks/km24.py`), `tools/build/quick.sh --ready`, the whole gates, `python tools/hunt/hunt.py` |

The DLLs in `project/bin` are versioned. A clone needs Godot 4.7.2, llvm-mingw `20261020-ucrt-x86_64`, CMake 4.4, Python 3.12, Node
23.6+ (`npm ci` in `tools/tracer/tracer/tools`), an x86-64-v3 processor.
