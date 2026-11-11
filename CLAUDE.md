# City: a city builder in Godot 4.7 with a C++ core (GDExtension)

The project's rules, methods and structure. Update this file in the same change that alters what it describes.
A file created or deleted enters `log/mapa.md` in the same commit: the `MAPA_DESATUALIZADO` gate checks it.
The detail lives in `log/`: `mapa.md` (file by file), `ferramentas.md` (what each tool taught), `pendencias.md`
(what is missing). Each line's records: `work-log.md` (port and graphics), `conversor.md`, `causa-raiz.md`, `lab.md`, `base.md`,
`arquitetura.md` (the cause analyser), `review.md` (the full reading: findings, problems, process errors),
`feature-trace.md` (the road flow traced by hand and the generic feature tracer's causes), `how-we-work.md` (the binding way, step by step, and the contradictions settled), `live-instance-design.md` (the live instance, the contracts' guards, the player's input, the order of the work).
Before starting, read your line's record. When closing the theme, update it with the time and the commit, in the same commit. [c:records]

**Where the tree stands (2026-11-10).** `native/`, `project/`, `tools/` and `log/` were taken back to `a1a771c4` (2026-11-07 20:29) by the owner's order: the city generated there is whole, and the lab ports made from 2026-11-08 21:30 on broke it.
Main as it stood is the tag `antes-da-volta-2026-11-10`: read it for WHAT was built and found; its lab ports never come back (the earlier fixes to this game's own code were brought back by the owner's authorisation, being nothing of the roadcraft). Each performance fix and each feature is made again one at a time, designed for this architecture,
measured (memory, CPU, GPU, frame) and photographed in the 24 km city (map `km24`) before and after. Code translated from the 4.0 still stands here (the translated runtime in `core/`, `flow_control`): it is redone the same way, not regenerated. [c:tree-back]

## Method (every task, in this order; direct work)
1. **Identify:** read the code involved and ask "why" until you reach the design decision that produces the symptom. List every problem. [c:identify]
2. **Research** with context7 on every implementation and fix that touches an outside API (Godot, godot-cpp, Terrain3D, a library): the official documentation and sources, never memory alone. [c:research]
3. **Understand:** tie problem, cause and solution through the code and, when there was one, through the research. [c:understand]
4. **Plan** the fix of the cause, with one owner, always choosing the most expensive and laborious solution that solves the
   cause (owner, 2026-11-11): a cheap solution that does not solve the cause is forbidden, and so is offering cheap options. [c:plan-costliest]
5. **Code** without trial and error: if it did not work, go back to reading until you understand. [c:no-trial]
6. **Verify what the change reaches**, once (the table of what each kind of change asks for, below). A photo is judged on its own (nothing broken,
   out of shape, overlapping or missing), never made equal pixel for pixel to another run, an older commit or a kept reference. [c:verify-reach]
7. **Stop and check:** did it pass or fail? If it failed, go back to step 1 with what the failure showed. [c:stop-check]

At a theme's close, the line's record gets the problems, the sources, the reasoning, the plan and the files created, changed and deleted. [c:theme-record]
A reported problem is solved with the `trace-fix` skill (`python .claude/skills/trace-fix/ledger.py resume` first): the flow traced end to end, read whole with the Read tool, recorded on disk, fixed at the cause, tested and photographed; its hooks refuse an edit of a file not read whole. [c:trace-fix]

## Fixing the cause
- The fix changes the design decision that produces the symptom. If the right fix asks for a rewrite, rewrite. [c:fix-decision]
- It is a patch, and forbidden: changing a timeout, threshold, thread count or tolerance; creating a special case; swallowing an
  error; loosening a check; writing the reference again (`--bless`) to make a test pass; moving the cost elsewhere. [c:no-patch]
- Every fix says, with file and line: what the cause is, why the symptom does not come back another way and which other
  symptoms go with it. [c:fix-says]
- Expensive does not prove a cause and cheap does not prove a patch: the criterion is the one above. [c:cost-no-proof]
- **Nothing of the roadcraft is copied, ever** (owner, 2026-11-11, binding in every prompt, skill and order): no line,
  line-by-line translation, structure, name or function of the roadcraft lab, which is only a reference for what the
  features must reach; the lab ports in the tag `antes-da-volta-2026-11-10` came from it and never come back. The fixes made
  earlier to this game's own code were brought back from the tag by the owner's authorisation (2026-11-10), being nothing of
  the roadcraft: do not confuse the two. Code of the roadcraft that came in is redone at its cause, designed for this tree. [c:no-roadcraft-copy]

## Principles
- The cause of performance problems is the architecture. Shadows, polygons and GPU time are symptoms and change with the card. [c:perf-architecture]
- Do what was asked, whole. No dressing up results and no leaving things for later. [c:do-it-whole]
- Never wait blind on a launched process (a deadline and following it). Stop only your own processes, by PID. [c:no-blind-wait]
- Never close the game open on the user's screen: `tools/build/free_dll.sh` frees the DLL. [c:user-game]
- Tests and captures never open a window on the user's screen (`tools/tasks` runs them `hidden`, on a private desktop). [c:hidden-runs]
- Every change to the game is delivered, at the theme's close, with a new executable: the user opens `dist/windows/Roadcraft.exe`. [c:deliver-exe]
- Direct work on `main`, in `C:\Users\Jonathan\Desktop\city`, without a worktree.
  A commit per theme and a push to `origin/main` (`https://github.com/designathan1988/city`). [c:main-direct]
- **The lab is a reference, never code to copy.** `C:\Users\Jonathan\Documents\Projects\lab\roadcraft` is the game with everything working. Read it only to write down WHAT the game must do: the behaviour, the rules, the cases, the values, what the player sees.
  Never copy its code, translate it line by line or reproduce its structure (it is a browser's, on one thread): the code is designed for this game's architecture and checked against that behaviour in tests and photos. `roadcraft-4.0-main` is old and no reference. [c:lab-reference]
- A launched process runs in the foreground or under a monitor, with a short deadline and its lines shown as they come: never in the background unwatched. [c:watched-process]
- A photo is taken only when the view is drawn (`{"until": "render_pending", "equals": 0}` before each `shot`) and is shown with its commit and date written on it. [c:drawn-photo]

## Running and checking
- Investigating is reading code: no measurements, profiles or long, heavy tests to investigate. A measurement only checks the effect of a change its cause already explains; every feature and performance fix is measured in the 24 km city at its end, the previous build and this one. [c:investigate-reading]
- Check only what the change reaches: [c:check-reach]

| Changed | Check |
|---|---|
| Tool under `tools/` | its own tests and `run_gates.py --file` on the changed files |
| GDScript, scene or data | `quick.sh <linked scenes>` and the gates of the changed files |
| C++ | both DLLs and `quick.sh <linked scenes>` |
| Theme's close, before the push | `quick.sh --ready`, the whole `run_gates.py` and the executable |

- Visual proof, only for a visual change: of the element, from near and from several angles. [c:visual-proof]

## Tools and processes
- Independent tasks run in parallel, each with its own deadline; one that hangs or fails is stopped and recorded, and the others go on. [c:parallel-tasks]
- Nothing is redone without a change of input: the result is kept by the key of its inputs. [c:cached-results]
- Each tool processes only what the change reaches. The whole run is an explicit option. [c:change-scope]
- A checking tool has to be able to fail: it exits non-zero on a failure, and an empty set fails. [c:checks-can-fail]
- **The feature tracer is generic, never "engessado"** (owner, binding): no rule, table or case for a program, library,
  API or pattern; specs and official declarations only; a gap fixes the mechanism (gate: `featuretrace/tests/test_generic.py`). [c:generic-tracer]

## Language
- Every new or changed text in the repository is written in English: code, file and folder names, comments,
  identifiers, data, records, reports, documents and commit messages. Write in English even when the file,
  the record or this document is in Portuguese (the translation of what exists is in `log/pendencias.md`). [c:english]
- Exceptions: the texts the player sees (translations in `project/data/i18n`) and outside copies. The conversation with the user is
  in Portuguese. [c:player-texts]

## Architecture rules
- **Single source of truth:** `CityState` (`native/src/sim/state/city_state.h`). Only `sim/commands` changes the state at the
  player's request; only `sim/simulation` advances time. [c:single-truth]
- **C++ layers:** `core` → `sim/*` (pure C++, no Godot) → `render` (only reads) → `api`. [c:layers]
- **No god file:** no god class split over several files either, nor a scattered feature. Split by
  responsibility, never to fit a limit. [c:no-god-file]
- **One owner per definition:** rules with a single owner in `tools/gates/owners.json`. No duplicated code: extract an owner. [c:one-owner]
- **Data:** tunable values only in `project/data/**` (JSON with a schema), read only by `DataTables`. The tuning's names in
  the X-macro `CITY_TUNING_FIELDS`, the values in `project/data/catalog/tuning.json`. [c:data]
- **No magic numbers** in `native/src`, `project/game`, `project/ui`: name them or move them to data. [c:no-magic]
- **Interface:** keeps no game numbers: it reads `CityStore.snapshot` and sends `CityStore.dispatch(...)`. Colours and sizes only
  through `data/ui/tokens.json` and `ui/theme/theme_builder.gd`; texts only through `tr("KEY")`; buttons and options in `data/ui/*.json`. [c:interface]
- **Flow:** screen changes, pause and quit only through `GameFlow`. [c:flow]
- **Header:** every file starts with a comment that says its responsibility. [c:header]
- **Pending work** goes to `log/pendencias.md`, never to TODO or FIXME comments. [c:pending]

## Tests
- They check what the player sees: the HUD, the instance buffer sent to the GPU, the core's probes. [c:tests-see]
- **State hash:** `probes/state_hash` is the city as a number. The `edit_fuzz_test` requires undo, redo, save and
  load to pass through the same hashes: every new edit enters its draw. [c:state-hash]
- **Tests and captures (`--no-flow`) run on the data:** they neither read nor write `settings.cfg`, nor read `health.json`. They open at the
  `tests_preset` tier (another by name, `--preset=`). Saves in `user://saves_tests`. [c:tests-on-data]
- To isolate a clone's `user://`: `project/override.cfg` with `application/config/use_custom_user_dir=true` and
  `custom_user_dir_name`. [c:user-dir]
- Memory and timing bugs are not hunted with load: the debug DLL has libc++'s hardening, and the failure is read in the stack and the code. [c:no-load-hunt]

## Commands
| What | Command |
|---|---|
| Run the game | `godot --path project` (without Vulkan: `--rendering-method gl_compatibility`) |
| Build both DLLs | `MINGW=... tools/build/build.sh core-windows` (Linux: `core`) |
| Check a change | `tools/build/quick.sh [task,task]` (= `python tools/tasks/cli.py run quick`: only the tasks whose inputs changed; `--ready` at a theme's close, `--all` nothing from the cache, `--bless` the city's digest written again, only for a change meant to move it; its frozen photos are for looking, compared with nothing) |
| Run one test | `python tools/tasks/cli.py run fast <scene>` |
| Suite in parallel | `python tools/tasks/cli.py run tests` (logs in `dist/tasks/<run>`, `cli.py status`; every task has its own deadline and is killed by PID) |
| Gates | `python tools/gates/run_gates.py` (`--file X`, `--files-from F`, `--summary`, `--selftest`); changed files only: `python tools/tasks/cli.py run gates_changed` |
| Scripts that do not compile | `python tools/tasks/cli.py run scripts` (`tools/build/check_scripts.gd`) |
| Orchestrator | `python tools/tasks/cli.py run <suite or task> [--list]`, `serve` (JSON lines, for Node tools), `status`; its tests: `python -m unittest discover tools/tasks/tests` |
| Export and package | `tools/build/build.sh export && tools/build/build.sh package` |
| Captures | `tools/build/build.sh capture [folder]` (= `cli.py run capture`, hidden, one task per plan) |
| Look at the game through a plan | `godot --path project --resolution 1920x1080 res://tests/visual/look.tscn -- --no-flow --plan=<json> --capture-dir=<folder>` |
| Where two photo sets differ | `python tools/build/photodiff.py <a> <b> <out>` (a help to find where to look, never a verdict: two runs of the same code differ in pixels) |
| Where each thread spends | `python tools/analysis/sampler.py --hidden --cmd "<godot> --path project <scene>" --seconds 30 --symbolizer <llvm-symbolizer>` |
| Causes by the code's shape | `python tools/analysis/causes.py` |
| Causes by the architecture | `python tools/analysis/arch.py scan`, `status`, `next`, `explain <id>`, `predict <id>`, `done <id>`, `selfcheck` (`--only x` redoes only what failed; queue in `dist/arch/queue.json`) |
| The release DLL's profile | `tools/build/pgo.sh` (redo it after changing hot loops) |
| The roadcraft's code still standing | `python tools/gates/lab_scan.py` (the report in `dist/`, exit 1 while anything stands) |

**What a clone needs:** `native/godot-cpp` (API 4.7) and the DLLs in `project/bin` come in the repository: the game opens without
building. Meshoptimizer and Clipper2 come through CMake on the first build. Apart: Godot 4.7.2, llvm-mingw
`20261020-ucrt-x86_64` (`C:/Projects/toolchains`), CMake 4.4, Node 23.6+, Python 3.12. The DLL asks for x86-64-v3 (AVX2, FMA, BMI).
**DLLs in a rebase:** they always conflict. Never pick a side: rebuild both from the joined sources and run the suite. [c:dll-rebase]

## Project structure
`C:\Users\Jonathan\Desktop\city` is the whole project. The detail file by file is in `log/mapa.md`.

**`native/src`: the C++ core**
- `core/`: bytes, colour, grid, job pool, maths, memory, polygons, random, units; the translated runtime still there is a debt, redone at its cause.
- `sim/state/`: `CityState`, `ChangeLog` (the single "dirty" channel), `PoseBuffer`.
- `sim/catalog/`: the catalogue's types, no values.
- `sim/commands/`: the player's commands, the edit journal, undo and redo.
- `sim/simulation/`: tick, speeds, clock and growth.
- `sim/roads/`, `sim/street/`: road network, grade, profiles, junctions; street furniture and street works.
- `sim/traffic/`: vehicles, lanes, crossings, signals, walkers, lorries, yard.
- `sim/transit/`: buses, trains and lines.
- `sim/buildings/`: records, the body generator (`b40/`) and interiors (`interior/`).
- `sim/lots/`, `sim/citygen/`: lots; the generated city.
- `sim/terrain/`, `sim/nature/`: relief; landscape, weather and ecology.
- `sim/demolition/`, `sim/economy/`, `sim/persistence/`: blast and damage; the roads' money; save.
- `render/`: `RenderWorld` (consumes the `ChangeLog`, applies the budgets), `rs_util.h`, `mesh_builder`, `chunk_queue.h`,
  `chunk_jobs.h` (upload budget), queues, mesh levels and order, lights, film grading and the bridge to Terrain3D.
- `render/{agents,buildings,roads,props,trees,cover,water,transit,effects,overlays,atmosphere,wear}/`: each thing drawn.
- `api/`: `CityCore` (split by area into `city_core_*.cpp`) and the script areas (`builder`, `landscape`, `transit`,
  `street`, `probes`, `lab`), data loaders.

**`project`: Godot**
- `game/`: `store/` (`CityStore`, `StoreApi`, saves), `flow/` (`GameFlow`), `data/` (`DataTables`), `camera/`, `tools/`
  (the player's tools), `settings/` (options, graphics tiers), `interior/`, `lab/` (F7), `health/`, `play.gd`.
- `render/`: world, sky, light, water and screen passes; `render/shaders/`: every shader.
- `ui/`: HUD, panels and drawers; `ui/theme/`: styles; `ui/icons/`: icons.
- `data/`: `catalog/` (rules and tuning), `game/` (camera, light, options), `render/` (looks), `ui/` (interface), `i18n/` (texts).
- `assets/`: textures, baked people, lots and models. `addons/terrain_3d/`: Terrain3D. `bin/`: the core's DLLs.
- `tests/`: `core/` (tests), `e2e/`, `perf/` (probes), `visual/` (captures, `look.gd`, `ui_driver.gd`).

**`tools`**
- `gates/`: checked rules, `rules.json`, `owners.json`, `contracts.json` (each rule above and its guards), `schemas/`.
- `tasks/`: the one orchestration owner of every support tool (tasks as data: inputs keyed by content, a deadline each,
  a verdict; parallel pool, cache in `dist/cache/tasks`, Job Object kill, hidden desktop; `suites.json`).
- `build/`: `build.sh`, `quick.sh` (a thin call of `tools/tasks`), `pgo.sh`, `photodiff.py`, `free_dll.sh`,
  `check_scripts.gd` (scripts that do not compile), `rename_english.py` (the English rename, with `--check`).
- `analysis/`: `sampler.py` with `sampler_report.py` and `unwind.py`, `causes.py` with `detectors.py` and `code_parse.py`, `arch.py` and `arch/` (the architecture analyser), `arch_eval/` (answer keys only its selfcheck reads).
- `assets/`: the kitchen of people, landmarks, noise, furniture, poles and road profiles.
- `converter/`: the old porting tool, with its own git; it produces and regenerates no game code (translating the roadcraft is forbidden). What it translated and still stands (`flow_control`) is a debt, redesigned at its cause.
- `converter-slice/`: the converter's older slice, with its own git.
- `process/`: process tools and records. `ports/`: each port's plans, coverage and reports.

**Others:** `dist/` (builds, captures and logs, out of git) and `log/` (records).

## Data flow and threads
- `DataTables` reads `data/**` → `CityStore.boot` sends the catalogue to the `CityCore`, which refuses a missing key. [c:catalogue-boot]
- `Simulation` runs on its own thread with a fixed tick (`sim_tick_hz`), under the shared `map_mutex` and `traffic_mutex`. [c:sim-thread]
- Commands and the clock take `map_mutex` exclusively, each hold naming its writer (`map_hold::WriterHold`: command, tick, load, generation, save, probe); growth decides under the shared lock and takes the exclusive one only to add (`grow_step`). [c:exclusive-lock]
- `ChangeLog` is the single "dirty" channel, drained under the shared lock and the log's mutex. [c:change-log]
- `RenderWorld.sync` never takes the map exclusively and is handed the city const, the `ChangeLog` apart. It sends the meshes to the `JobPool`, which read only the map, never the traffic. [c:render-shared]
- A mesh job copies what it reads under a short shared lock (`MapRead`, `CityCut`, `HeightPatch`) and builds with the lock let go. [c:mesh-cut]
- The land's maps are `TerrainBridge`'s (fine ground round the view, far ground whole); Terrain3D keeps no region. A record's derived body has one owner, the tables' `BodyCache`, bounded in bytes. [c:land-owner]
- Mesh conversion enters `render.upload_budget_mb` per frame; the commit only swaps meshes, within `render.commit_budget_ms`. [c:frame-budgets]
- Poses through `PoseBuffer`. The save writes on its own thread. [c:poses-save]

## Light and clock
- `Daylight` is the owner of the sun, the ambient and the sky. At night the sun becomes the moon, which casts all night (P347); the street's light is `LampLights`. [c:daylight]
- `CityWorld.set_sky_mode("day"|"night"|"cycle")`: day and night fix the hours of `daylight.json` (16h and 22h). [c:sky-mode]
- Tests and captures move the clock and the sky together (`UiDriver.at_hour`). [c:clock-sky]

## State machines
- `GameFlow.State`: BOOT → MENU → PLAYING → QUITTING (the `TRANSITIONS` table; closing the window also goes through `quit`). [c:gameflow-states]
- `SimSpeed` (C++): STOPPED, PAUSED, NORMAL, FAST, FASTEST; only `start()/stop()` enter and leave STOPPED. [c:sim-speed]
- `ToolController.Tool`: one per tool, INSPECT when empty-handed; only `set_tool` changes it; held `drag`/`tick`, released `release`. [c:tool-states]
