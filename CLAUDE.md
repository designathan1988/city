# City: a city builder in Godot 4.7 with a C++ core (GDExtension), to be sold on Steam

This file is the project's only source of instructions. Reference (every file, the structure, the threads, the full command
list, the archive tags) lives in `log/map.md`; nothing here is status.

## Authority
- This file alone holds the method, every order of the owner, the constraints, the architecture rules and the commands the work
  needs. Memory, records, skills, goal files, nested instruction files and Spec Kit's constitution hold no order and claim no
  precedence; auto memory is off (`.claude/settings.json`). An order given in a conversation enters here with its tag and its
  guard (`tools/gates/contracts.json`) in the same change; one that contradicts a rule here is shown to the owner, whose decision
  replaces the old text. [c:authority]
- The owner's standing authorisation (2026-11-12): never ask and never wait; what a task of an accepted specification names is
  done (removals once what has value is kept under an `archive/<date>/*` tag, settings, a skill deployed once its self-test and
  a real cycle pass, commits, the push to `origin/main`, the export). It revoked the notes that said otherwise (the mandatory
  pause, hands off the skill, never delete records, the old tracer and `tools/analysis` untouched). Never: a force push, a
  rewritten history, `rm -rf` or any deletion by a relative path, a deletion outside the project and its listed copies, the
  owner's open game closed, what a running process uses touched. [c:standing-authorisation]
- One current document per subject: `log/map.md`; `log/problems.jsonl`, the one register of known problems, written only through
  `python tools/atlas/cli.py problems` (view `log/problems.md`); `log/specification.md` (goals, reference hardware, budgets);
  `log/research.md` (research with proofs); `log/history/<area>.md` (one history per area); `.specify/` and `specs/NNN-name/`
  (Spec Kit). Read your area's history before starting. A file created or deleted enters the map in the same commit; nothing of
  value is lost (what leaves the tree stays under a named tag, listed in the map). [c:records]

## Method (every task, in this order)
1. **Read.** The code involved, whole, with the Read tool, personally (an agent's report is a lead; `cat` or `grep` is no reading); ask "why" until
   the design decision that produces the symptom; list every problem. A reported problem of the game goes through the `trace-fix`
   skill; a problem only found on the way enters the register and is left for its area. The defects the game shows are found
   first by `python tools/hunt/hunt.py`, which names each one's owning code. [c:identify]
2. **Research.** Context7 (`resolve-library-id`, then `query-docs`) on every implementation, fix and research, or the vendored
   source: what the API guarantees, on which thread, at what cost, its known bugs in the exact version; never memory. A feature
   the reference game has is read there first, for what it must do (the owner, 2026-11-12: nothing is worked out from nothing
   that the reference already shows working). A quick
   research test runs in an isolated folder outside the game tree, its real output recorded in `log/research.md`. [c:research]
3. **Understand.** Problem, cause and solution tied through the code and the research. [c:understand]
4. **Plan** the fix of the cause with one owner, the costliest and most laborious solution that solves it; a cheap one that does
   not, and offering cheap options, are forbidden. [c:plan-costliest]
5. **Code the whole block with nothing run inside it.** A block is every open problem the owner named, or with none named every
   open problem of the area in hand, each fixed at its cause, one after the other, without trial and error.
   `python tools/tasks/cli.py block open <name>` starts it: until `block verify`, the orchestrator, `build.sh`, `deliver.sh`, the
   git pre-commit hook and the project's hook refuse every build, test, game run, measurement, photo, gate run, export and commit.
   A doubt is settled by reading. Keeping work about to be removed (an archive tag) is no commit of the block. [c:no-trial]
6. **Verify once, at the block's end** (`block verify`): what the block reaches by the table below; `quick.sh --ready` once; the
   whole gates against the register (`run_gates.py --register log/problems.jsonl`: a finding that is no open problem fails; the
   gates are never loosened, baselined or silenced); the photos by the image protocol; km24 measured on the previous build and
   this one for a change to the game. [c:verify-reach]
7. **Stop and check.** A failure is read to its cause (`superpowers:systematic-debugging`), fixed in the code (`block open`
   again) and the verification run once more; a pass is confirmed (`superpowers:verification-before-completion`). [c:stop-check]
8. **Close** (`block close`): the operations the verified code serves, the area's history (problems, sources, reasoning, plan,
   files created, changed and deleted, the time and the commit), a commit per theme, one push to `origin/main`, the executable
   when the game changed, and a report to the owner in Portuguese: done, failed, open. [c:theme-record]

## Fixing the cause
- The fix changes the design decision that produces the symptom. If the right fix asks for a rewrite, rewrite. [c:fix-decision]
- It is a patch, and forbidden: changing a timeout, threshold, thread count or tolerance; creating a special case; swallowing an
  error; loosening a check; writing the reference again (`--bless`) to make a test pass; moving the cost elsewhere. [c:no-patch]
- Every fix says, with file and line: what the cause is, why the symptom does not come back another way and which other
  symptoms go with it. [c:fix-says]
- Expensive does not prove a cause and cheap does not prove a patch: the criterion is the one above. [c:cost-no-proof]
- **Nothing of the roadcraft is copied, ever**: no line, line-by-line translation, structure, name or function. The lab
  (`C:\Users\Jonathan\Documents\Projects\lab\roadcraft`, the game with everything working) is read only to write down what the
  game must do (behaviour, rules, cases, values, what the player sees); the code is designed for this game's architecture and
  checked against that behaviour. The lab ports of the tag `antes-da-volta-2026-11-10` never come back (this game's own earlier
  fixes from it were authorised); code translated from the old 4.0 is redone at its cause, never regenerated; `roadcraft-4.0-main`
  is no reference. [c:no-roadcraft-copy]
- **Proof, not claims.** Say what was read, run and verified, how, and where the proof is; read the whole output of every build,
  test and gate; a slow, heavy, crashing or failing result is a failure, said plainly with its numbers; a cause not proven is
  "unknown"; never a fake (a stand-in for a real element, a feature switched off, quality lowered, a test weakened or skipped, a
  metric massaged). [c:proof]

## Principles
- The cause of performance problems is the architecture. Shadows, polygons and GPU time are symptoms and change with the card. [c:perf-architecture]
- **The bar is a game sold on Steam**: the reference profiles and budgets of `log/specification.md` (median: 6 cores, 16 GB,
  RTX 3060 class, 1080p `medium` at 60 fps; floor: 4 cores, 8 GB, integrated GPU, 1080p `low` at 30 fps). Performance,
  optimisation and visual quality are acceptance criteria of every feature; every change to the game is measured against them in
  the 24 km city (map `km24`), the previous build and this one. [c:budgets]
- Do what was asked, whole: the goal, not the literal words; the neighbouring cases; every rule the owner cares about a mechanism
  that can fail; a limit found is a cause to resolve, never a list at the end. [c:do-it-whole]
- A check takes seconds: the most likely place first (the newest folders, one targeted search); an empty result of a fragile
  search is a failed search, not a "no". Slowness of the work is a defect, recorded and fixed at its cause. [c:fast-checks]
- Agents are used to go faster, up to five at once (the owner, 2026-11-12): any kind, reading, researching and writing code as
  the task needs, but **an agent executes nothing** (no Bash or PowerShell: no build, test, game run or script; an agent's run
  froze the owner's machine): what must run is run by the main session, one at a time, watched. Each agent is sent a prompt that
  names the goal, the files, what is known, what it may change and the answer's form; the main session checks what it returns
  against the files before it is used, never trusted blind (`.claude/hooks/agent_guard.py` keeps the count and refuses an
  agent's Bash and PowerShell). [c:agents-read-only]
- Skills: `speckit-*` for a specification; `trace-fix` for a reported problem of the game; `superpowers:systematic-debugging`
  for a failure's cause; `superpowers:verification-before-completion` at each close; `update-config` for settings; the LSP tool
  for C++; context7 always. Never: brainstorming, writing-plans, executing-plans, test-driven-development,
  subagent-driven-development, dispatching-parallel-agents, requesting-code-review, finishing-a-development-branch,
  using-git-worktrees. [c:skills]
- Never wait blind on a launched process (a deadline and following it). Stop only your own processes, by PID. [c:no-blind-wait]
- A launched process runs in the foreground or under a monitor, with a short deadline and its lines shown as they come. [c:watched-process]
- One engine process at a time while working (the live instance counts as the one); the suite runs only at a verification. [c:one-engine]
- Never close the game open on the owner's screen: `tools/build/free_dll.sh` frees the DLL. [c:user-game]
- Tests and captures run hidden on a private desktop (`tools/tasks`); the live instance alone may show a window, never taking
  focus. [c:hidden-runs]
- Every block that changes the game delivers an executable: `tools/build/deliver.sh export` writes
  `dist/builds/<date>_<commit>/Roadcraft.exe`, points the Desktop's Roadcraft shortcut at it and keeps the last two builds (and
  any an open problem names). Every folder under `dist/` has one owner and a bound (`tools/tasks/suites.json` lifecycle). [c:deliver-exe]
- Direct work on `main`, in `C:\Users\Jonathan\Desktop\city`, without a worktree; a commit per theme at the block's close and one
  push to `origin/main` (`https://github.com/designathan1988/city`). A worktree or copy exists only with an owner and an end:
  `python tools/tasks/cli.py worktrees` fails while one is older than its work. [c:main-direct]
- Work from another branch, worktree, stash or session enters `main` only read whole against the copy rule, redone on today's
  tree where its base is gone, and verified at a block's close; otherwise its behaviour enters the register. [c:bring-in]
- Never read another session's transcript (its `.jsonl`, tool results, subagent folders, `list_events`, the episodic archive):
  its work is checked by its branch, diff, files and logs. [c:no-transcripts]
- Repository files are changed with the Edit and Write tools, never by a shell command (`sed`, a script's `write_text`); a
  script writes bytes; a file whose last commit had no carriage return gains none. [c:lf-bytes]
- In a long C++ session the session's own clangd is watched and stopped by its PID past about 3 GB. [c:clangd]
- A photo is taken only when the view is drawn (`{"until": "render_pending", "equals": 0}` before each `shot`) and is shown with
  its commit and date written on it. [c:drawn-photo]
- **The image protocol**, every photo: written first what it must prove; planned by what the change reaches (the element from
  near, mid and far, several angles, day and night, each tier it touches, movers with the clock running) plus a few general
  views; every photo and crop opened with Read, whole, then by regions at real size; each defect kind looked for (missing or
  extra, broken, deformed, scale, orientation, bodies in one another, floating or sunk, holes, the street's seams, textures,
  colour, light, shadow, detail pops, artefacts, interface text); the analysis written per photo with positions; judged on its
  own, never against another run or an older commit's photo. A defect, or a photo not analysed, fails the block. [c:image-protocol]

## Running and checking
- Investigating is reading code: no measurement, profile or heavy test to look for a cause; a measurement checks a change its
  cause already explains. [c:investigate-reading]
- Check, at the block's end, what the block reaches: [c:check-reach]

| Changed | Check |
|---|---|
| Tool under `tools/` | its own tests and `run_gates.py --file` on the changed files |
| GDScript, scene or data | `quick.sh <linked scenes>` and the gates of the changed files |
| C++ | both DLLs and `quick.sh <linked scenes>` |
| Block's close, before the push | `quick.sh --ready` once, the whole gates against the register, the photos, the executable |

## Tools and processes
- Independent tasks run in parallel, each with its own deadline; one that hangs or fails is stopped and recorded, and the others go on. [c:parallel-tasks]
- Nothing is redone without a change of input: the result is kept by the key of its inputs. [c:cached-results]
- Each tool processes only what the change reaches. The whole run is an explicit option. [c:change-scope]
- A checking tool has to be able to fail: it exits non-zero on a failure, and an empty set fails. [c:checks-can-fail]
- **The feature tracer is generic, never "engessado"**: no rule, table or case for a program, library, API or pattern;
  specs and official declarations only; a gap fixes the mechanism (`tools/tracer`). [c:generic-tracer]

## Language
- Every new or changed text in the repository is written in English: code, file and folder names, comments, identifiers, data,
  records, reports, documents and commit messages. [c:english]
- Exceptions: the texts the player sees (`project/data/i18n`) and outside copies. The conversation with the owner is in
  Portuguese. [c:player-texts]

## Architecture rules
- **Single source of truth:** `CityState` (`native/src/sim/state/city_state.h`). Only `sim/commands` changes the state at the
  player's request; only `sim/simulation` advances time. [c:single-truth]
- **C++ layers:** `core` → `sim/*` (pure C++, no Godot) → `render` (only reads) → `api`. [c:layers]
- **No god file:** no god class split over several files either, nor a scattered feature. Split by responsibility, never to fit
  a limit. [c:no-god-file]
- **One owner per definition:** rules with a single owner in `tools/gates/owners.json`. No duplicated code: extract an owner. [c:one-owner]
- **Data:** tunable values only in `project/data/**` (JSON with a schema), read only by `DataTables`; the tuning's names in the
  X-macro `CITY_TUNING_FIELDS`, the values in `project/data/catalog/tuning.json`. [c:data]
- **No magic numbers** in `native/src`, `project/game`, `project/ui`: name them or move them to data. [c:no-magic]
- **Interface:** keeps no game numbers: it reads `CityStore.snapshot` and sends `CityStore.dispatch(...)`. Colours and sizes only
  through `data/ui/tokens.json` and `ui/theme/theme_builder.gd`; texts only through `tr("KEY")`; buttons in `data/ui/*.json`. [c:interface]
- **Flow:** screen changes, pause and quit only through `GameFlow`. [c:flow]
- **Header:** every file starts with a comment that says its responsibility. [c:header]
- **Known problems** go to the register, never to TODO or FIXME comments. [c:pending]
- `DataTables` reads `data/**`; `CityStore.boot` sends the catalogue to the `CityCore`, which refuses a missing key. [c:catalogue-boot]
- `Simulation` runs on its own thread with a fixed tick (`sim_tick_hz`), under the shared `map_mutex` and `traffic_mutex`. [c:sim-thread]
- Commands and the clock take `map_mutex` exclusively, each hold naming its writer (`map_hold::WriterHold`); growth decides under
  the shared lock and takes the exclusive one only to add. [c:exclusive-lock]
- `ChangeLog` is the single "dirty" channel, drained under the shared lock and the log's mutex. [c:change-log]
- `RenderWorld.sync` never takes the map exclusively and is handed the city const; mesh jobs read the map, never the traffic. [c:render-shared]
- A mesh job copies what it reads under a short shared lock (`MapRead`, `CityCut`, `HeightPatch`) and builds with it let go. [c:mesh-cut]
- The land's maps are `TerrainBridge`'s; Terrain3D keeps no region. A record's derived body has one owner, the `BodyCache`. [c:land-owner]
- Mesh conversion enters `render.upload_budget_mb` per frame; the commit only swaps meshes, within `render.commit_budget_ms`. [c:frame-budgets]
- Poses through `PoseBuffer`. The save writes on its own thread. [c:poses-save]
- `Daylight` owns the sun, the ambient and the sky; at night the moon casts all night; the street's light is `LampLights`. [c:daylight]
- `CityWorld.set_sky_mode("day"|"night"|"cycle")`: day and night fix the hours of `daylight.json` (16h and 22h). [c:sky-mode]
- Tests and captures move the clock and the sky together (`UiDriver.at_hour`). [c:clock-sky]
- `GameFlow.State`: BOOT → MENU → PLAYING → QUITTING by its `TRANSITIONS` table (closing the window goes through `quit`). [c:gameflow-states]
- `SimSpeed`: only `start()`/`stop()` enter and leave STOPPED; every move through `enter` and its table `SPEED_MOVES`. [c:sim-speed]
- `ToolController.Tool`: only `set_tool` changes it; the stroke is its `Phase` machine, which a tool change cancels. Every enum
  named like a state is a machine with one setter and its table, or a declared kind (`rules.json` states). [c:tool-states]

## Tests
- They check what the player sees: the HUD, the instance buffer sent to the GPU, the core's probes. [c:tests-see]
- **State hash:** `probes/state_hash` is the city as a number; `edit_fuzz_test` requires undo, redo, save and load to pass through
  the same hashes: every new edit enters its draw. [c:state-hash]
- Tests and captures (`--no-flow`) run on the data: no `settings.cfg`, no `health.json`; the `tests_preset` tier (another by
  `--preset=`); saves in `user://saves_tests`. [c:tests-on-data]
- A clone's `user://` is isolated by `project/override.cfg` (`application/config/use_custom_user_dir`, `custom_user_dir_name`). [c:user-dir]
- Memory and timing bugs are not hunted with load: the debug DLL has libc++'s hardening; the failure is read in the stack and the code. [c:no-load-hunt]

## Commands (the whole list: `log/map.md`)
| What | Command |
|---|---|
| A block | `python tools/tasks/cli.py block open <name>` · `block verify` · `block close` · `block status` |
| Build both DLLs | `tools/build/build.sh core-windows` |
| Check a change | `tools/build/quick.sh [task,task]` (`--ready` at a block's close) |
| One test | `python tools/tasks/cli.py run fast <scene>`; the orchestrator's state: `python tools/tasks/cli.py status` |
| Gates | `python tools/gates/run_gates.py` (`--file X`, `--summary`, `--selftest`, `--register log/problems.jsonl`, `--json`) |
| The register | `python tools/atlas/cli.py problems [--area A --status S]`, `add`, `update`, `close`, `drop`, `render` |
| Deliver | `tools/build/deliver.sh export` |
| Find the game's defects | `python tools/hunt/hunt.py`: one hidden run of the town that lists every defect the core's probes see (vehicles in one another classed by mechanism, street pieces in one another, wires drawn at no level, road surfaces in one another, the engine's errors), each with where and the owning file; `dist/hunt/hunt.json` |
| Photograph a place | `TRACE_FIX_SLUG=<slug> python .claude/skills/trace-fix/ledger.py photos <plan.json>` (a plan of look.gd's steps; the crops printed to open with Read); `... crops <photo>` |
| How the reference does a feature | read its code in `C:\Users\Jonathan\Documents\Projects\lab\roadcraft\src` for the behaviour (never copied: [c:no-roadcraft-copy]) |

The DLLs in `project/bin` are versioned, so a clone opens the game without building: no ignore pattern may drop them, and in a
rebase they are rebuilt from the joined sources, never one side picked. A clone also needs Godot 4.7.2, llvm-mingw
`20261020-ucrt-x86_64`, CMake 4.4 (meshoptimizer, mimalloc and Clipper2 come through it), Python 3.12, Node 23.6+ for the asset
kitchen's `ktx2_to_png.mjs`, and a processor with x86-64-v3 (AVX2, FMA, BMI). [c:dlls]
