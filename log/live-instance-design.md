# Live instance: the game kept open while it is developed, with a real-time monitor

Written 2026-11-11 from the owner's order of the same day, after a study of the engine's source (Godot 4.7: core/extension,
core/object, core/profiling, main/main.cpp, platform/windows/display_server_windows.cpp), the docs (context7: godot-docs 4.7,
godot-cpp, Tracy) and this machine (its GPU counters). Approach 1 below was chosen by the owner; every other decision of how is
the builder's, written here as such.

## Intent

The owner, in his words: keep an instance of the game open so that a change is seen in it in real time (changes, behaviour,
visual aspects that are hard to detect from the code); a real-time monitor of CPU, GPU, memory and the rest, to tell whether a
change hurt anything; precise and complete diagnosis of simulation, performance, optimisation, conflicts, features, interactivity,
graphics and anomalies; nothing slow, nothing heavy; the cause solved, never the effect; the costliest and most laborious
solution. The window opens without taking the focus of his computer. Total autonomy for diagnosis, debugging, speed and
precision.

Why it is needed (the cause of slow, blind work today): every check starts Godot cold (the engine, the city generated, every chunk
streamed, pipelines compiled) and its result exists only after the process has exited; the in-game readings (the F7 panel) live in
the window and cannot be read from outside; nothing measures per-thread CPU, the process's GPU use, its VRAM, the waits on the
map's locks or the simulation's tick budget live; the official engine reloads an extension only inside the editor process, so a
C++ change always costs a new process.

## Goals

- One game process kept open on the owner's screen, never taking focus, holding a city, that takes a change of C++, GDScript,
  shaders or data in place in seconds, keeping the city, the camera and the clock.
- One real-time monitor read by the owner (an overlay and the Tracy profiler's window) and by the assistant (a command line):
  frame, per-thread CPU, GPU time and use, VRAM, RAM, the simulation's tick, the render's budgets, lock waits, errors, anomalies.
- Before and after of a change in the same process, same view, same state, with the measurement's noise stated.
- Photos taken from the live instance in seconds, each carrying its provenance; proof photos only from a state this build made.
- Crashes and hangs caught, their stacks symbolised, the instance brought back from its last checkpoint.

## Non-goals

- Replacing the test suite: `quick.sh` and the scene tests still run clean and hidden; the live instance is where work is seen and
  measured, the tests are what gates a change.
- Shipping any of it: the exported game keeps the official 4.7.2 templates; the release DLL carries no profiler and no reload.

## Anti-goals (a failure even if it works)

- A reload that leaves the city subtly different from a fresh load (stale derived data, leaked RIDs, a thread still running old
  code): the state hash before a reload equals the one after, or the reload is refused.
- A monitor that costs the frame it measures: instrumentation off is free (one predictable branch), on is a few percent at most,
  and the cost is stated.
- A window that steals focus, or a hidden process nobody watches.
- Readings that cannot be trusted: every number names its source and its sample window.

## Constraints

- CLAUDE.md: one owner per definition; values in `project/data/**` read by `DataTables`; no magic numbers; English in the
  repository; every file in `log/mapa.md`; processes owned by `tools/tasks` (Job Object, deadlines); tests and captures stay
  hidden (the live window is the owner's explicit exception, shown without focus); `CityState` the one source of truth.
- Nothing of the roadcraft is copied.
- The machine: llvm-mingw 20261020 (C++ and the engine), CMake 4.4, Python 3.12, Windows 11; one Godot for the live instance and
  at most one more for a test at a time.

## Approaches considered

1. **A development build of the engine (chosen).** Godot 4.7.2-stable built from source with our toolchain, with Tracy compiled in
   (memory tracking on) and a small patch that lets a game process reload extensions; the core made reload-safe (it hands its city
   across the reload and rebuilds); our own instrumentation and telemetry; a local command link and a supervisor. Costliest; it
   removes the cause (no cold start for any kind of change) and gives the engine's own frame zones beside ours.
2. Official engine, restart on every C++ change from a fresh save with the same view; scripts, shaders and data reloaded in place;
   Tracy in the core alone. Cheaper; every C++ change still pays a process start, the city's load and the stream.
3. The Godot editor as host (its debugger's monitors and profilers, script sync). Cheapest; readings only on the editor's screen,
   no C++ reload in the game, no per-thread CPU, no GPU use, no VRAM per process.

## What already exists, and what each piece becomes

- `project/tests/visual/live_driver.gd` (loaded by `play.gd` with `-- --live=<dir>`, left out of the export): the game open on the
  owner's screen driven from outside through a polled file (`steps.json` every 0.25 s, `result.txt` back) with steps `new`, `view`,
  `dispatch`, `query`, `frames`, `hour`, `shot`. Lacks: an answer per command (a file race and a quarter-second poll instead), its
  shot does not wait `render_pending` 0 and carries no commit, date or provenance, and its steps repeat `look.gd`'s vocabulary. It
  becomes the link (section 5): the same entry point, the file replaced by a local socket, its steps and look.gd's one step runner
  (one owner of the vocabulary, used by look plans, the link and the tests), its shot the one rule of shots.
- `Health` (`project/game/health/health_log.gd`, autoload): the one owner of what went wrong (a `Logger` taking the engine's errors
  from any thread, slow frames blamed on the core's render_cost part). The link and the telemetry read it; no second Logger.
- The F9 monitor (`project/ui/health_panel.gd`), the Shift+F9 profiler (`project/ui/profiler_hud.gd`: fps, process ms, draw calls,
  primitives, render_cost parts, video/texture/static memory) and the F7 lab (`FrameMeter`, `CpuWatch`, `ResourceWatch`, `CityLab`):
  the owner's in-game views, fed by the one telemetry owner (section 4) instead of each reading the engine on its own.
- `tools/analysis/sampler.py`: the stacks of every thread, used by the supervisor on a hang.
- `tools/analysis/live40.mjs`: the old 4.0 kept open in a browser; no reference any more (CLAUDE.md), not part of this.

## Design

### 1. The development engine (`tools/live/engine/`)

- Source: `godotengine/godot` at tag `4.7.2-stable` and `wolfpld/tracy` at one pinned release tag, cloned by
  `tools/live/engine/build.py` into `C:/Projects/toolchains/godot-dev/src` (outside the repository, as the other toolchains), built
  with SCons through llvm-mingw: `platform=windows use_mingw=yes use_llvm=yes target=editor production=no optimize=speed
  profiler=tracy profiler_path=<tracy> profiler_track_memory=yes`.
- Patch, kept as files in `tools/live/engine/patches/` and applied by the build, each a few lines, each with its reason:
  - `main/main.cpp`: a command-line flag `--extension-reload` (only in TOOLS_ENABLED builds) that calls
    `Engine::set_extension_reloading_enabled(true)` for a game process, as the editor process does (main.cpp:2224).
  - `core/extension/gdextension_library_loader.cpp`: `generate_temp_files` also when extension reloading is enabled, so the
    loaded library is a temporary copy and the build writes `project/bin` freely (today only `is_editor_hint`).
  - `core/profiling/SCsub`: `TRACY_EXPORTS` on `TracyClient.cpp`, so the executable exports Tracy's C API and a module resolves it
    at run time (Tracy's manual: one client per process; other modules import it).
- Output: `C:/Projects/toolchains/godot-dev/<source hash>-<patch hash>/godot.dev.exe` with a manifest (tags, patch hashes, flags,
  compiler); built again only when one of those changes; `tools/tasks/locations.py` finds it (`GODOT_DEV`). The Tracy profiler,
  `tracy-capture` and `tracy-csvexport` are built from the same Tracy tag with CMake into the same folder.

### 2. The core survives a reload (`native/src/api/city_core_reload.*`, one owner)

The engine's reload (gdextension.cpp) stores every `PROPERTY_USAGE_STORAGE` property of each extension instance, destroys the
old instance in the old code (`clear_internal_extension` calls `free_instance`), loads the new library, recreates the instance
(`recreate_instance`, godot-cpp `GODOTCPP_USE_HOT_RELOAD`), sets the properties back and sends `NOTIFICATION_EXTENSION_RELOADED`.

- `CityCore` gets one storage property, `reload_carry` (PackedByteArray), empty except while a reload is prepared: its getter
  stops the simulation and the job pool, waits for the save thread, and writes the city in the save format (the one owner of a
  city's bytes, `sim/persistence`) followed by the session (camera, clock, speed, tool, selection, the build that generated the
  state and every build that carried it since); its setter, in the new code, reads it back.
- The destructor is the teardown: every thread joined, every RID the core made freed (RenderWorld, lights, pools, the Terrain3D
  bridge's maps), every static and cache of the module reset (`FurnitureKept`, thread_local scratch dies with the joined threads).
  A teardown that leaves a RID or a thread is a failure the reload probe reports.
- `NOTIFICATION_EXTENSION_RELOADED`: the simulation restarts, RenderWorld rebuilds from the state (the pipelines stay compiled in
  the device, the textures stay loaded by the scripts).
- A data change runs the same cycle without a library swap: the catalog read again by `DataTables`, the city carried into a new
  core built on it. One mechanism for both.
- The release DLL is built without hot-reload accounting; the debug DLL with it (it costs nothing under the official engine, where
  no reload is ever started).

### 3. Our instrumentation (`native/src/core/trace.h`, one owner)

- Zones, plots, messages, frame marks, lock instrumentation and allocation events through Tracy's C API, resolved once at load
  from the host executable (`GetProcAddress` on the main module): under the official engine the functions are absent and every
  call is one branch on a null pointer; the release DLL compiles it out.
- Zones use allocated (transient) source locations, so nothing the profiler holds points into a library that a reload unloaded.
- Placed at the causes: each simulation tick and its stages (traffic, walkers, transit, growth, clock), each job of the pool by
  kind (chunk meshes, bodies, lots), RenderWorld's sync, upload and commit, the save thread, and `map_mutex` / `traffic_mutex`
  through wrappers that report waits and holds, shared and exclusive. Plots: vehicles, walkers, render_pending, upload bytes against
  `render.upload_budget_mb`, commit ms against `render.commit_budget_ms`, ticks behind the clock.
- Allocation events from mimalloc's hooks (the core's allocator) so the profiler shows where our memory is.

### 4. Telemetry (`native/src/core/telemetry.*`, one owner)

- A sampler thread at `dev.telemetry_hz` (tuning, data) reads what the operating system knows: CPU time of every thread of the
  process (thread enumeration and `GetThreadTimes`, names from `GetThreadDescription`; our threads are named), process memory
  (`GetProcessMemoryInfo`), the GPU's use and this process's VRAM (PDH counters `GPU Engine(pid_*)\Utilization Percentage` per
  engine type and `GPU Process Memory(pid_*)\Dedicated Usage`, present on this machine), mimalloc's heap statistics.
- The main thread publishes each frame (frame interval, render CPU, GPU ms per viewport and per pass from FrameMeter's stamps,
  draws, primitives, Godot's memory monitors, render_pending, uploads, commit ms); the simulation each tick (tick ms by stage, ticks
  behind, counts, jobs queued and busy).
- Written as fixed records with a sequence lock into a memory-mapped ring (`dist/live/telemetry`), so the supervisor reads it even
  while the main thread is stalled; the same values go to Tracy as plots and to the in-game overlay (F7 reads the same owner).

### 5. The link (`project/game/dev/dev_link.gd`, only with `--dev-link=<port>`)

- Local TCP (127.0.0.1), JSON lines, one command per line, one answer per command. The commands are the look plans' steps
  (`setView`, `hour`, `speed`, `frames`, `until`, `shot`, `dispatch`, `query`) so a plan runs unchanged against the live instance,
  plus `reload {core|scripts|shaders|data}`, `regen` (a city generated by this build), `save`, `load`, `mark`, `probe <name>`.
- Errors and warnings: read from `Health`, the one owner of the engine's errors (its Logger already takes them from any thread);
  `live.py errors` gives its entries with their detail, and their count stands in the telemetry.
- The window: the dev user folder's `override.cfg` sets `display/window/size/no_focus` and the rectangle; the engine shows such a
  window with `SW_SHOWNA` (never activated); the owner's first click in it clears the flag, so he can use it.

### 6. The supervisor (`tools/live/`, a component of `tools/tasks`)

- `python tools/live/live.py up [--map town|km24] [--seed N]`: starts the dev engine on the project (Job Object, visible desktop,
  no focus), waits for the link and for `render_pending` 0, prints progress line by line, returns; the instance stays, watched.
- Watching (a mechanism, not a person): a heartbeat on the telemetry ring; no new record for `dev.hang_s` → the stacks of every
  thread sampled (`tools/analysis/sampler.py`), written to `dist/live/hangs/`, the instance restarted from its last checkpoint
  (a save every `dev.checkpoint_s` and before every reload). A crash: the engine's crash log symbolised with llvm-symbolizer (as
  sampler.py does), kept in `dist/live/crashes/`, the instance restarted from the checkpoint.
- `status`, `watch` (a live table), `mark <name>` and `diff <a> <b>` (medians, tails and the noise of a repeat, as CityLab.compare
  does), `errors`, `probe`, `shot`, `view`, `run <plan.json>` (a look plan through the link), `trace <seconds>` (tracy-capture,
  then tracy-csvexport, then the zones that moved), `reload`, `regen`, `down`.
- A file watcher: a change under `project/**/*.gd` reloads those scripts, `*.gdshader` those shaders, `project/data/**` the data;
  a finished `build.sh dll-debug` reloads the core. Each reload is a mark, so `diff` before and after is one command.

### 7. Photos and their provenance

- Every shot is labelled with the commit, the DLL's hash, the engine's id, the hour, `render_pending` at the shot (0, or no shot),
  and the state's origin: generated by this build, or carried since build X.
- `live.py shot --proof` refuses a state not generated by the current build (`regen` first): a proof photo of a fix always shows
  what this code makes, in seconds instead of a cold start. Photos are planned by what the change touches (poles near, far and from
  several angles for a pole fix; people with the clock running for a walker fix), plus a few general views.

## Verification of the instance itself

- Reload probe: carry, reload, `state_hash` equal before and after, for the town and for km24; twenty cycles with RIDs, threads,
  working set and VRAM flat (a slope as ResourceWatch computes it).
- Telemetry checked against the operating system's own counters (Get-Counter) on the same seconds.
- Cost: the frame time of km24 at one view with instrumentation absent, present and disconnected, and connected to Tracy.
- The window: launched while another window has focus; the focus stays there.
- Targets to meet, measured in km24: a script or shader reload under a second; a data or core reload within the time a fresh load
  of the same save takes without the engine's start and the pipelines (recorded, then held); the telemetry's lag under 250 ms.

## Risks and how they are met

- The engine build (time, toolchain): built once per engine version, cached by its manifest; llvm-mingw is the toolchain that
  already builds the core.
- Module state that survives badly (statics, mimalloc's static heap in an unloaded library): the reload probe's flat slopes prove
  it; what is not freed is a defect of the teardown.
- Terrain3D and the scripts hold objects across the reload: they hold Godot objects and resources, which survive; method binds of
  our classes are re-registered by the engine.
- The patch must follow the engine's versions: three small hunks with their reasons, applied by the build script, which fails
  loudly when one does not apply.

## Left to the builder

- The exact record layout of the telemetry ring and the overlay's look (the owner sees it before it is final).
- The lock wrappers' shape (they keep `std::shared_mutex`'s interface).
- The order of the plan's steps; each step is tested and measured as CLAUDE.md asks.

# Part B: the project's contracts, each kept by a mechanism

The owner's order of 2026-11-11 (joined to this design): every implementation fully optimised, fast and of quality; the roadcraft read
only to understand a behaviour, never copied; everything that holds state has a state controller, one owner and one source of truth;
the architecture kept; no contract already stipulated is ever broken again; the root cause always, the costliest solution, no patch.
A promise does not keep a contract: a check that refuses the change does. What exists and what is missing (tools/gates, 2026-11-11):

| Contract (CLAUDE.md and the owner's orders) | Kept today by | Missing |
|---|---|---|
| C++ layers core -> sim -> render -> api | CAMADA, CAMADA_DESCONHECIDA, SIM_SEM_GODOT, RENDER_SO_LEITURA | |
| One owner per definition, no duplicate | DONO_UNICO, SEGUNDO_DONO, DONO_AUSENTE, UM_DONO_DO_CORE, CODIGO_DUPLICADO, VALOR_DUPLICADO | |
| Values only in data, with schema, read by DataTables | DADOS_SEM_SCHEMA, DADOS_POR_UM_LEITOR, TUNING, TUNING_SEM_USO, NUMERO_MAGICO | |
| UI reads the snapshot, dispatches, styles by tokens, texts by tr | ESTADO_NA_UI, UI_SEM_ESTILO_SOLTO, TEXTO_VIA_TR | |
| Flow only by GameFlow | FLUXO_CENTRAL | |
| No god file; header; English; map; no loose TODO | TAMANHO_ARQUIVO, TAMANHO_FUNCAO, CABECALHO, NON_ENGLISH_TEXT, MAPA_DESATUALIZADO, SEM_MARCADORES_SOLTOS | |
| Nothing of the roadcraft copied (no line, translation, structure, name, function) | nothing | COPIA_DO_LAB |
| CityState the one source of truth: only sim/commands change it at the player's request, only sim/simulation moves time | the layers (indirectly), debug lock checks | ESCRITOR_DO_ESTADO |
| Every stateful controller a state machine with one transition table and one setter (GameFlow, SimSpeed, ToolController, the live instance, the reload) | FLUXO_CENTRAL for GameFlow alone | CONTROLADOR_DE_ESTADO |
| No patch: no threshold, timeout, tolerance, thread count or golden moved to pass | `--bless` only by flag | REMENDO |
| Every contract has its guard | nothing | CONTRATO_SEM_GUARDA |

## B1. COPIA_DO_LAB: the roadcraft is never copied (`tools/gates/gate_lab_copy.py`)

- The reference tree (`references.json`: `lab/roadcraft`, 865 TypeScript files) is read once per content into a fingerprint store
  under `dist/cache` (keyed by the tree's content): every identifier it defines (functions, classes, constants, fields), normalised
  (case and separators removed: `openStretch` = `open_stretch`), and the winnowed k-gram fingerprints (Schleimer, Wilkerson, Aiken,
  the method behind MOSS) of each function body as a stream of language-neutral tokens: keywords and declarations as their
  category, types dropped, identifiers as one token, operators kept, numeric literals kept by value (the same constants in the same
  order are a translation's strongest trace).
- Every changed C++, GDScript and data line is held against the store: a defined identifier whose normalised name is one the
  reference defines (and not a word of the domain's own dictionary, `tools/gates/lexicon.json`), and any run of matching
  fingerprints past the threshold that tells a translation from an independent implementation of the same behaviour, refuse the
  change with the reference's file and line beside ours.
- Its threshold is fixed by its own tests before it guards anything: fixtures of reference functions with their line-by-line
  translations (must fail) and independent implementations of the same behaviour (must pass).
- The whole tree is scanned once: what it finds already standing in the code is entered in `log/pendencias.md` and in the trace as
  problems (the causes C23 to C33 name the areas), to be redone at their cause; new code is refused from the first day.
- A renamed copy is still a copy (the owner, 2026-11-11): the gate matches structure and constants, not names alone, so renaming
  passes nothing. What it finds is not renamed or reshuffled: each area is designed again for this game's architecture (CityState
  the one source of truth, its state controller, one owner, the job pool and the map's locks, data in `project/data`), written as
  new code, and closed only when it is whole: its behaviour checked against what the reference shows the player (tests from that
  behaviour), its performance measured in km24 against a budget written before the work (frame, tick, memory, GPU), its pictures
  taken where it shows, and the gate passing on it.

## B2. ESCRITOR_DO_ESTADO: one writer of the city

- Static: a non-const `CityState &` (or a member of it taken mutable) may be reached only from `sim/commands` (the player's
  edits), `sim/simulation` and the systems it steps (time), `sim/persistence` (load), `sim/citygen` (a new city) and the api's
  `run_edit`; anything else taking it mutable is refused (the gate reads the parse `tools/analysis/code_parse.py` already makes).
- Run time, in the debug DLL: every exclusive hold of `map_mutex` names its writer (command, tick, load, generation); a write of the
  state under no named writer stops the debug build where it happens (the existing CHECKS family), and the live instance's
  telemetry counts holds by writer.

## B3. CONTROLADOR_DE_ESTADO: every state has its controller

- A GDScript or C++ type whose behaviour turns on an enum of states (State, Mode, Phase, Speed, Tool, ...) keeps them in one
  member written by one setter that consults one transition table; any other assignment, or a setter without a table, is refused.
  GameFlow's TRANSITIONS is the model; SimSpeed and ToolController are brought to it where they are not; the live instance's
  supervisor (DOWN, STARTING, READY, RELOADING, HUNG, CRASHED, RESTORING) and the core's reload (RUNNING, CARRYING, TORN_DOWN,
  REBUILDING) are built on it from the start.

## B4. REMENDO: a number moved to pass is refused

- A change to a test's tolerance, a deadline in `tools/tasks/suites.json`, a frozen digest, a threshold or a thread count is refused
  unless the same change records its cause (the trace's `fix` naming the file and the reason, or the owner's order quoted).

## B5. CONTRATO_SEM_GUARDA: no contract without its guard

- `tools/gates/contracts.json`: every rule of CLAUDE.md and every standing order of the owner, each with the gates, tests or run-time
  checks that keep it. The gate refuses a contract with none, a guard that no longer exists, and a CLAUDE.md rule not listed. A new
  rule cannot enter the project without its guard.

## B6. At run time, in the live instance

- The probes the tests already have (walkers in furniture, furniture in gates, lots on paving, road overlaps, the line's close poles
  and unjoined junctions, the state hash) run on a background job at `dev.probe_s` and their counts stand in the telemetry: an
  anomaly is seen the moment it appears, with its place, not at the next test run.

# Part D: the right way, written once, and the contradictions removed (done 2026-11-11)

The owner ordered every instruction contrary to doing it right found and corrected, and the right way made explicit. Done before
anything else: `log/how-we-work.md` holds the way step by step (read, research with context7 every time, the lab for behaviour only,
design for the architecture with state controllers and one owner, the cause the costliest way, test first, verify what the change
reaches with photos planned by it, measure both builds at the end, a mechanism for every contract, close the theme whole, work alone
and watched, the skills that fit) and the table of what contradicted it: four statements of CLAUDE.md (the converter regenerating
translated code, the translated runtime as a part of core, "every port", the moon without shadow) and six memories (no "before"
measurement, agents from the machine translation, subagents allowed, agents in handfuls, blessing to pass, a stale diagnosis) were
corrected; two account skills named for porting are not used. CONTRATO_SEM_GUARDA (B5) keeps it so: a contradiction found later is
settled in that page and in CLAUDE.md in the same change.

# Part E: playing the game as a player (in the live instance)

The owner's order: the assistant must be able to test the game as a real player, with the mouse (click, move, drag, release) and the
keyboard, to find problems of features, interface and interactivity.

What was studied: the engine's input path (context7, godot-docs 4.7: the platform's DisplayServer turns each operating-system message
into an InputEvent and hands it to `Input.parse_input_event`, which accumulates motion per frame and dispatches to the root window:
`_input`, the GUI's `_gui_input`, `_shortcut_input`, `_unhandled_key_input`, `_unhandled_input`; a script's `Input.parse_input_event`
enters the same path at that point, without moving the operating system's cursor); the project's `UiDriver`
(`project/tests/visual/ui_driver.gd`: moves, clicks, holds, drags and keys through `Input.parse_input_event`, but its drags send
motion without a held button mask, relative motion or velocity, and it has no typing, wheel, double click or modifiers on the mouse);
prior art, GdUnit4's scene runner (context7: simulate_mouse_move, simulate_mouse_button_press and release, simulate_key_press,
await_input_processed flushing buffered input and waiting a process and a physics frame). Operating-system injection (SendInput,
posted window messages) was weighed and refused: it moves the owner's cursor or needs the focus the owner said not to take, and the
engine reads the real cursor in places, so it would test the owner's desk instead of the game.

The design (`project/game/dev/player.gd` behind the link, one owner of synthetic input; UiDriver moves onto it so tests and the live
instance press the same way):
- Every event as the DisplayServer makes it: positions in the window and global, `button_mask` held through a drag, `relative`,
  `screen_relative` and `velocity` from the path's own timing, modifiers on mouse and key events, `double_click`, the wheel's
  `factor`, keys with `keycode`, `physical_keycode`, `key_label`, `unicode` and `echo` while held, text typed character by character;
  a gesture is spread over frames at the display's rate, the input flushed and a process and a physics frame awaited after each step.
- Targets by screen point, by a control of the interface (`ui.tree`: every visible Control with its path, class, translated text,
  rectangle, disabled and focus state, read from the scene tree), or by a world point (unprojected by the camera).
- Observation after every step: the photo (render_pending 0), the interface tree, the store's snapshot and the dispatches the gesture
  caused (a tap on `CityStore.dispatch`), the errors `Health` caught, the telemetry's frame and probes; a gesture whose effect is
  not what a player expects is a problem with its replay attached.
- Every session's input recorded with its frames (`dist/live/plays/`): a play is replayed exactly (the simulation's fixed tick) and
  becomes a regression test when it found a defect; journeys of a player (draw a road and its junction, zone, build, demolish, open
  every panel and tool, save and open) are plans of the same steps.
- The window keeps the owner's focus untouched: synthetic input needs none.

# Part C: everything the owner asked, in one order

1. These guards (Part B), COPIA_DO_LAB first: they protect every line written after them.
2. The live instance (Part A) with the player's input (Part E): every change after it is seen, played, measured and photographed in
   seconds.
3. The street line (P393, P394, P395: one pole per corner shared by the lines meeting there, spans across the junction, no pole
   against a signal post, the line through joints), then P369's remaining cases and the player's pole runs.
4. The owner's lists, at their causes, each photographed by what it changes: road joins (P370), blocks while drawing roads (P371),
   the build mode (P372), what vanishes when zooming out (P374), far buildings' quality (P375), terrain and the sea at the edge
   (P376), the street-level camera (P377), pre-baked UI pictures (P378); then the trace's other open problems (P360, P361 and the
   rest), the areas the copy scan finds redone at their cause.
5. Each theme closed as CLAUDE.md asks: both DLLs, quick.sh --ready, every gate, the executable, the record, commit and push.
6. The areas the copy scan finds are rebuilt whole, one at a time, in the order of what the player meets most (traffic and walkers,
   roads and street furniture, buildings and lots, the city generator, the rest): each a new design of its own, never a rename, with
   its behaviour tests, its measured budget in km24 and its photos, so the game works whole at the end of each and is fully its own
   at the end of all.
