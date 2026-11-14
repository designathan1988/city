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

## One functionality, start to end: the skill `port`

The procedure is the project's skill **`port`** (`.claude/skills/port/SKILL.md`): invoke it with the Skill tool before the first
tool call of any port work, and again after every compaction. What it says in steps, the tools hold by force; these hold always:

- **Defects are found by reading, never by running.** The limits the code must hold are printed at `block open`
  (`tools/gates/rules.json`); the hook reads each file's gates after every edit and they are fixed there. Between `block open`
  and `block verify` no build, test, measurement, photo or engine. The coding ends with the whole diff read and
  `block review` (the gates of every changed file): `block verify` is refused before it passes.
- **A verification is small and takes seconds**: one run of a few checks (the functionality's own test, its photo plan, `e2e`:
  six tasks at most, named in one command) and one measure (`lab_test`). No whole suite, no second run: the tools refuse them.
  The whole suite, `km24`, `hunt` and `quick --ready` run only on the owner's order.
- **A block is verified three times at most**, failed or not. It opens again only with a cause at a `file:line`
  (`block open --cause "<check or what was found>: <file:line>: <why>"`), read in the code (`superpowers:systematic-debugging`),
  never guessed, and everything found is fixed in that one opening. Two verifications may fail; a third stops the work and
  the owner is told.
- **Memory is no source.** Context7 for every API the code calls; the internet for every design decision and every value of
  the real world, the source's own words cited in the data.
- **Skills are used, not named**: `port` for the procedure, `superpowers:systematic-debugging` on a failure,
  `superpowers:verification-before-completion` before a commit, the `speckit-*` skills when the owner asks for a
  specification. A skill's step that runs a test while coding, writes a plan document or stops to ask does not apply here.
- **The note `doc/work/<name>.md`** holds the owner's orders in their words, what was read (facts with `file:line`), each
  case's design, what is done and what is left. Written before the first line of code and as the work goes, never more
  than fifteen minutes behind it. Its first section, `## Now`, is the state at this moment, rewritten each time: the
  owner's standing orders, where the block stands, what is in hand, the next steps. No other document is written on the way.
- **A compaction keeps a summary, and a summary is no source.** After one (and at a session's start with a block not
  closed) nothing is edited or run before the skill `port` is invoked again and the note is Read whole: the hook refuses
  it. The orders are this file (Claude Code gives it again whole), the procedure is the skill, the facts are the note.
- **The session does its own work**: no agent reads, traces or codes the port.

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
- A run of the project's tools is one command of its own, in the background, never piped, redirected or joined to another;
  `python tools/tasks/cli.py status` shows it. A file is changed with Edit or Write and a script is a file written with Write:
  nothing is typed into the shell as a heredoc, no file of the project is written by the shell. The hook refuses the rest
  (`.claude/hooks/port_gate.py`, its tests `tools/tasks/tests/test_port_gate.py`); a refusal is obeyed, never worked round.
- The port's tracing, reading and coding are the session's own work: no agent does them.
- Work on `main`; commit and push per feature. The owner hears, in Portuguese, once the work is done: what was done, what
  failed, what is open, with proof.
- Records, and no other: the board `doc/port.md`; `doc/map.md` (a file created or deleted enters it in the same commit, the
  gate MAP_OUTDATED asks it); the register (`python tools/atlas/cli.py problems`); the functionality's note
  `doc/work/<area>.md` (above). Claude Code's auto memory is on: an owner's correction
  or preference is saved there.
- The work stays in this folder; outside it only the reference is read.

## Commands

| What | Command |
|---|---|
| Where the port stands | `python .claude/hooks/port_state.py` (the board: `doc/port.md`) |
| The procedure | the skill `port` (Skill tool) |
| Both games traced for a functionality | `python tools/tracer/port.py sheet <area> [--feature N]` · `python tools/tracer/port.py check` |
| Trace | `python tools/tracer/trace.py <START> --depth N --state --root <feature files>` (`--list` names the starts) |
| What each game has | `python tools/atlas/cli.py inventory [--area A] [--game reference\|city] [--unpaired]` |
| A block | `python tools/tasks/cli.py block open <name>` · `block review` · `block verify` · `block open --cause "<check>: <file:line>: <why>"` · `block close` · `block status` |
| Build both DLLs | `tools/build/build.sh core-windows` |
| One test or photo plan | `python tools/tasks/cli.py run <task>` (names: `python tools/tasks/cli.py run tests --list`) |
| Gates of a file · nothing copied | `python tools/gates/run_gates.py --file X` · `python tools/gates/lab_scan.py` |
| The register | `python tools/atlas/cli.py problems [--area A --status S]`, `add`, `update`, `close`, `drop` |
| Deliver | `tools/build/deliver.sh export` (`dist/builds/<date>_<time>_<commit>/Roadcraft.exe`) |
| Only on the owner's order | the whole suite (`python tools/tasks/cli.py run tests`), measurements at scale (`python tools/tasks/km24.py`), `tools/build/quick.sh --ready`, the whole gates, `python tools/hunt/hunt.py` |

The DLLs in `project/bin` are versioned. A clone needs Godot 4.7.2, llvm-mingw `20261020-ucrt-x86_64`, CMake 4.4, Python 3.12, Node
23.6+ (`npm ci` in `tools/tracer/tracer/tools`), an x86-64-v3 processor.
