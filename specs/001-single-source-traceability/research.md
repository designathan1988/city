# Research: One Source of Truth, a Clean Project and End-to-End Feature Traceability

Phase 0 of `plan.md`. Each decision names the cause it removes (J1 to J6 of the spec), the sources read, and the alternatives
rejected. Sources marked "to prove" are tested in an isolated folder outside the game tree before their task is coded (CLAUDE.md:
quick research tests in an isolated folder are allowed; no trial and error in game code).

## R1. Where instructions live, and memory (J2)

- **Decision**: CLAUDE.md is the only instruction file of the project and holds every order, under 200 lines. No path-scoped
  `.claude/rules/`, no imports. Reference material (folder structure, threads, the full command catalogue) moves to `log/map.md`,
  which CLAUDE.md names as a document, not as instructions. Auto memory is off (`autoMemoryEnabled: false`), nested CLAUDE.md files
  are removed, and the gate fixtures named CLAUDE.md are excluded from loading (`claudeMdExcludes`), all in the project's own
  `.claude/settings.json`, which becomes a tracked file.
- **Rationale**: the owner's order ("the work method must be in CLAUDE.md and not in files scattered elsewhere"); the official
  documentation: CLAUDE.md holds the owner's instructions, auto memory Claude's learnings, conflicts are resolved arbitrarily, each
  file under 200 lines, a rule that must always happen is a hook. `claudeMdExcludes` matches absolute paths by glob and merges across
  scopes; `autoMemoryEnabled: false` in a project's settings turns auto memory off for that project only.
- **Sources**: code.claude.com/docs/en/memory ("CLAUDE.md vs auto memory", "Write effective instructions", "Organize rules with
  .claude/rules/", "Disable auto memory"); code.claude.com/docs/en/settings-reference (`claudeMdExcludes`); read through Context7
  `/websites/code_claude`, 2026-11-12.
- **Alternatives rejected**: path-scoped rules per area (official, but splits the instructions again, against the owner's words);
  keeping memory for non-rule notes (a second place to audit; the owner had turned it off before); `@imports` (official, but they do
  not reduce what loads and they spread the text over files again).

## R2. One register of problems (J1)

- **Decision**: `log/problems.jsonl`, one problem per line, schema-validated (`tools/gates/schemas/problem.schema.json`); a generated,
  read-only `log/problems.md` view grouped by area and status; one CLI owns writes (`tools/atlas problems add|update|close`). Evidence
  is a path under `dist/evidence/<problem id>/` (kept while the problem is open) or a named git reference.
- **Rationale**: one line per problem merges cleanly in git and is read in milliseconds; the owner reads the generated view; the
  schema makes a missing cause, owner or criterion a gate failure instead of a habit. The trace ledgers' useful fields (symptom,
  cause, fix, status, evidence) map one to one.
- **Alternatives rejected**: a markdown table as the source (fragile to parse, the current `pendencias.md` shows the drift); a
  database file in git (binary diffs); the ledgers under `dist/` (unversioned, the cause of 360 problems outside git).

## R3. The game declares its own architecture (J3)

- **Decision**: three X-macro registries in the core, next to their owners, read by the C++ compiler and by the atlas alike:
  `CITY_STATE_PARTS(X)` in `sim/state/city_state.h` (each part of `CityState`: name, owner, controller or none, writers allowed);
  `CITY_COMMANDS(X)` in `sim/commands/` (each command: name, area, state parts it writes, change kinds it announces), from which the
  script side's `StoreApi.COMMANDS` is generated and checked; `CITY_CHANGE_KINDS(X)` in `sim/state/change_log.h` (each kind of
  the `ChangeLog`: payload, producers, consumers, the area it may touch). The engine-free test executable dumps the three as JSON
  (`dist/atlas/declared.json`) in milliseconds, with no engine boot. In debug and profile builds every state part carries a write
  revision; a command or a system step that moves a part it did not declare is reported (run-time half of FR-024/FR-027), and every
  consumer counts the work it did per change kind and how much of it produced an output identical to the one before (FR-028's
  "without need").
- **Rationale**: the architecture already has single channels (`CityState`, the commands, `ChangeLog`, `GameFlow`) and already
  uses X-macros (`CITY_TUNING_FIELDS`, the furniture fields): declaring them is the costliest and most precise answer to "who owns,
  who writes, what a change triggers", and the compiler keeps the declaration and the code joined. Reading syntax alone (three
  tracers so far) can only approximate what the game can state.
- **Alternatives rejected**: inferring owners and triggers from the code by a tracer only (imprecise across threads, bound methods
  and data keys; the cause of the three tracers); a hand-kept JSON beside the code (drifts; a second source of truth); reflection
  through the engine (needs the engine booted, slow, misses the engine-free core).

## R4. The atlas: one traceability tool (J3)

- **Decision**: `tools/atlas` joins, into one index rebuilt incrementally by content hash: the declarations (R3); the code links
  from the generic tracer `tools/tracer` (kept as the code-reading library, unchanged in its genericity); the gates' and the copy
  scan's findings; the static audit's dead links (folded in as an atlas check); test coverage (`tools/tasks/godot_deps.py` for engine
  scenes, the Catch2 test list for engine-free tests); the last profile run's per-owner costs against the budgets; the register.
  Queries: `feature`, `state`, `writers`, `triggers`, `owner`, `unoptimised`, `uncontrolled`, `problems`. Index in SQLite (Python's
  standard library), one file under `dist/atlas/`, rebuilt only for changed inputs.
- **Rationale**: SC-006/SC-007 ask for answers in seconds without reading code; the three fast checks already exist (3 s, 9 s,
  10 s), and what was missing is the join and the declarations. SQLite answers the joins in milliseconds and needs nothing outside
  Python.
- **Alternatives rejected**: keeping `tools/analysis/arch` (17 GB of work files, overlaps the tracer); the old `featuretrace`
  (stopped by the owner's order of 2026-11-11); a graph database or a server (another moving part to own).
- **Retired into it**: `tools/analysis/causes.py` and `detectors.py` become atlas checks (code shapes with a cost); `tools/audit`
  becomes the atlas's `dead` check; `tools/analysis/{arch,arch_eval,engine,featuretrace}`, `trace_feature.py`, `trace_validate.py`,
  `live40.mjs` leave (history under the archive tag). `sampler.py` stays in `tools/build` (thread sampling of a run).

## R5. Engine-free tests of the simulation (J4)

- **Decision**: split the native build into `city_sim` (static library: `core/` and `sim/`, which include no engine header — 0 of
  the 421 files do) and `citycore` (the GDExtension: `render/` and `api/`, linking `city_sim`); add `city_tests` (an executable
  linking `city_sim` and Catch2 v3, fetched with FetchContent like meshoptimizer and Clipper2). Tests are tagged by area
  (`[traffic]`, `[walkers]`, `[roads]`...); `catch_discover_tests(... ADD_TAGS_AS_LABELS)` registers them with CTest; the orchestrator
  runs only the tags a change reaches. Scenario tests build a small city in memory from a seed and a command list, step the
  simulation a fixed number of ticks with a fixed camera path, and compare the state hash.
- **Rationale**: 50,000 lines of simulation are checked today only by booting the engine and generating a town per scene; a native
  test of a rule runs in milliseconds. Catch2 v3 is maintained, has tags, durations, streaming console output and machine reporters.
- **Sources**: Catch2 `docs/cmake-integration.md` (`FetchContent_MakeAvailable`, `Catch2::Catch2WithMain`, `include(Catch)` after
  appending `extras` to `CMAKE_MODULE_PATH`, `catch_discover_tests` options), through Context7 `/catchorg/catch2`, 2026-11-12.
- **To prove**: Catch2 v3 builds with the project's llvm-mingw `20261020-ucrt` toolchain and `-fno-exceptions` is kept off the test
  target only (Catch2 needs exceptions; the game's library keeps them off).
- **Alternatives rejected**: doctest (lighter, but not found in Context7's index to check its current state); GoogleTest (heavier,
  slower to build); keeping engine scenes for rules (the cause of the minutes).

## R6. Deterministic scenarios (J4)

- **Decision**: every random draw of the simulation comes from streams seeded by the scenario's seed and the agent's id; the damage
  clock steps on the tick's schedule (the writer waits for the map instead of `try_to_lock`, S2), so the partition of time no longer
  depends on mesh jobs; the camera path is a scenario input. Removing the camera dependence of traffic and walkers (S1) is the
  simulation's own specification.
- **Rationale**: a failure that does not reproduce cannot be read (L3, S3); these three are the causes the code shows.

## R7. Three builds of the core, and how each is chosen (J5, J6)

- **Decision**: `debug` (libc++ hardening, `CITY_CHECKS`, probes; what project runs load today), `profile` (optimised as release,
  no hardening, `CITY_CHECKS`, probes and per-owner timing zones), `release` (no probes, no checks, what players get). The probes'
  sources (`api/*_probe*.cpp`, `render/**/*_probe.cpp`) compile only when `CITY_PROBES` is on. The profile library is chosen by a
  custom feature tag `profile` in its own export preset, listed first in `citycore.gdextension`'s `[libraries]`
  (`windows.profile.x86_64`); measurements run on the exported profile build.
- **Rationale**: T5 (probes only on the debug library, whose hardening distorts CPU numbers) and FR-034 (no probes shipped). The
  `.gdextension` entries are matched top to bottom and every tag of an entry must be active; custom tags come from the export preset.
- **Sources**: Godot docs "The .gdextension file" (`[libraries]`, ordering, custom export flags) and "Feature tags" (custom
  features; not applied to editor runs of a runnable preset), Context7 `/websites/godotengine_en`, 2026-11-12.

## R8. The product's needs (J6)

- **Decision**: a version in `project.godot` (`application/config/version`) written by `deliver.sh` from the commit; Godot's file
  logging kept on (`user://logs/godot.log`, five rotated) with the crash handler's message pointing at that folder and an interface
  entry that opens it; release symbols kept beside each delivered build (not shipped) so a player's backtrace can be read; a
  processor gate: a small extension compiled for baseline x86-64 that the project loads first, checks AVX2/FMA/BMI, and only then
  loads the core through `GDExtensionManager`, or shows a message in the player's language and quits. The store integration
  (overlay, achievements, cloud saves) is its own specification.
- **Sources**: Godot `ProjectSettings` (`debug/file_logging/*`, `debug/settings/crash_handler/message`), "Logging" tutorial (crash
  logs written to the log file; usable backtraces need symbols), Context7 `/websites/godotengine_en`, 2026-11-12.
- **To prove**: loading the core through `GDExtensionManager.load_extension` from a gate extension at start, before any scene uses
  it, in Godot 4.7.2 (isolated project); the gate's own code and its static initialisers carry no instruction above baseline.

## R9. Bounded outputs and the lifecycle of artifacts (J1)

- **Decision**: every folder under `dist/` has one owner and a bound, written in `tools/tasks/suites.json` (`outputs` with
  `max_mb` or `keep`): the task cache evicts least-recently-used entries past its bound on every write; `deliver.sh` keeps the last
  two builds plus any build an open problem names; run folders keep the last N per task; evidence lives under `dist/evidence/<id>/`
  and goes when its problem closes. `tools/tasks/cli.py clean` applies every bound now, and every write applies its own. Worktrees:
  `cli.py worktrees` lists each with its branch, age, merged state and uncommitted count, and fails when one is older than its work
  (merged, or untouched for 7 days).
- **Rationale**: 45 GB of `dist` and 55 GB of worktrees and copies grew because nothing has an end; the bound sits in the one owner
  of each output, so it holds on every write, not on a cleaning day.

## R10. Triage of worktrees, branches and stashes (J1)

Read for this plan: each worktree's head, merged state and uncommitted count; each branch's commits missing from `main`, their
messages and whether their added lines name the reference. Classes and the action each implies (the exact list is written whole in
`log/history/process.md` before anything is removed; the owner's standing authorisation of 2026-11-12 stands for his answer):

| Class | Members | Action |
|---|---|---|
| Already in `main`, clean | 22 worktrees under `.claude/worktrees`, `city-scenic`, `city-verify`, `city-realism-before`, temp `before_main`, about 90 merged branches | remove worktree; delete branch |
| Already in `main`, with uncommitted changes | the Codex worktree (434 files), temp `before5f` (3,427), temp `base` (3), six `city-bisect` copies (4 each), `city-head` (5), `city-nov07` (4), `city-preport` (4), agents `a4ad025` (8), `a5089bf` (46), `a5ffcd3` (4), `a6b9f2b` (2), `ac81c2a` (2) | read the changes; keep any with value as a tagged commit; remove |
| A plain copy | `city-archive` (7.9 GB) | compare with history; tag what history lacks; remove |
| Behaviour to redo (made on the ported tree or from the reference) | `realism`, `realism-port`, `worktree-port-zoning-bomb`, agents `ab9695a` (zoning, demolition), `a1fc4a1` (city generator), `aa66def` (floor markings), `a59a187` (street styles), `ad40db2` (vehicle lamps, lane change), `a8bf1bc` (poles), `portes/*`, `arquivo/*` | read; write each behaviour wanted into the register as a feature; tag the branch; delete |
| Worth bringing in, to verify | `wip/allocator-2026-11-10` (allocator hooks, memory census: this game's own); agents `a07591d`/`abeaa03` (terrain owns the ground beside a road), `a70fca1` (bus boarding, riders counted: S7), `a40909` (umbrellas, backpacks), `a486021` (coast stones, rock strata), `ae22101` (interior furniture), `a31bf43` (corner lots); stashes "9 of 22 fixed" (vehicles) and P476/P485/P486/P487 | read whole against the copy rule; redo on today's `main` where the base is gone; built, tested and photographed at the block's close; merged if it passes, else registered |
| Old tools' histories | `tools/converter/master`, `tools/converter-antigo/master`, `tools/converter/slice-envelope`, `tools/processo-historia/master`, `fix/root-causes`, `copia-b6785d7` | tag; delete branch |

## R11. The trace-fix skill (J1, J2, J3, J5)

- **Decision**: a staged redesign (copy, whole self-test, a real cycle, deployed on the owner's word): the skill keeps its procedure
  (read the flow whole, record, find the decision, fix it) and its read guard; it records problems in `log/problems.jsonl` through
  the atlas instead of ledgers under `dist/`; its `trace`, `writers` and `card` commands call the atlas (the broken `ftrace` import
  goes); its world rules move to the core's run-time checks (`rules.json` becomes the list the checks implement, kept by the atlas);
  its text cites CLAUDE.md's tags instead of restating rules, and the block rule replaces "test it, photograph it" per problem; its
  hooks move from the user's settings to the project's `.claude/settings.json`; the read rule counts what the edit touches and its
  callers, not every file opened since the last compaction (S1, S6). A trace is one problem, closed when the problem is.
- **Rationale**: the skill's discipline is what the owner wants; its second register, broken commands, contradicting text,
  machine-wide hooks and its compaction cost are what slows the work (spec G).

## R12. Spec Kit inside the project's rules (J2)

- **Decision**: `.specify/` and `specs/` are declared document places: the gates' DOC_UNICO and RAIZ_LIMPA accept them, CLAUDE.md
  names them, and `.specify/memory/constitution.md` is a short pointer ("this project's constitution is CLAUDE.md; the Constitution
  Check is its tagged contracts, kept by `tools/gates/contracts.json`"). Spec Kit's templates stay as shipped.
- **Rationale**: the gates refuse Spec Kit's files today (10 DOC_UNICO, 2 RAIZ_LIMPA), and a constitution with rules of its own
  would be an eighth home of the method.

## R13. Fast, watched, non-blocking processes (J4)

- **Decision**: `tools/tasks` stays the one orchestrator (the `slow-checks` trace fixed its admission, streaming and per-task
  deadlines). Added: a progress heartbeat every 5 s per running task in its streamed output (a task silent past it is reported, and
  stopped at its deadline by PID through its Job Object); `quick --ready` split into what a block reaches (engine-free tags first, in
  seconds; engine scenes after); the engine-free tests as a task with its own deadline.
- **Rationale**: the mechanisms exist; what is missing is the engine-free level and a heartbeat check.
