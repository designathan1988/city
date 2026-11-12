---
description: "Tasks of 001-single-source-traceability"
---

# Tasks: One Source of Truth, a Clean Project and End-to-End Feature Traceability

**Input**: Design documents from `specs/001-single-source-traceability/` (spec.md revision 2, plan.md, research.md R1 to R13,
data-model.md, contracts/, quickstart.md)

**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/

**Tests**: no test-first tasks (CLAUDE.md `[c:no-trial]`). The tests this feature creates (engine-free tests, the tools' own tests
CLAUDE.md requires of every tool) are code of their block; they run only at the block's close.

**Organization**: one phase per user story; each phase is one **block** in CLAUDE.md's sense, in three parts:
1. **Code**: the block's code and documents, written one after the other with nothing run (no build, test, measurement, photo,
   game run, export or commit; no tool written in the block is used before part 2).
2. **Close: verify**: the block's single verification of its code (CLAUDE.md's table: what the block reaches; `quick.sh --ready`
   once; the whole gates, failing on any finding that is not an open problem of the register, FR-052; photos only for a visual
   change, by the image protocol; km24 measured, the previous build and this one, for every change to the game's code; both DLLs
   for C++). A failure is read to its cause, fixed in the code and the verification run once more.
3. **Close: apply and deliver**: the operations that use the verified code (migrations, removals), the quickstart steps of the
   story, the line's record with time and commit, a commit per theme, one push, the executable when the game changed.

From US2 on (the mechanism is built in US1), each phase starts with `python tools/tasks/cli.py block open <story>`, its close
starts with `block verify`, and its last task ends with `block close`.

**Removals**: the owner gave standing authorisation (2026-11-12) for every removal these tasks name; nothing waits for an answer.
A removal runs only in part 3, after the whole list is written, and after what has value is kept under an `archive/2026-11-12/*`
tag (preservation, not a commit of the block: FR-051). Never touched: a worktree or folder a running process uses, the owner's
open game (`tools/build/free_dll.sh` frees the DLL).

**Problems**: a problem only found on the way is entered in the register (`log/problems.jsonl`) and left for its area's
specification. A reported problem of the game this feature fixes (L3, S2, M2) is traced with the `trace-fix` skill, one trace per problem,
closed at its block's close (CLAUDE.md `[c:trace-fix]`).

## Format: `[ID] [P?] [Story] Description`

- **[P]**: can be done in parallel with the other [P] tasks of its part (different files, no dependency on an unfinished task)
- **[Story]**: US1 to US8 of spec.md

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: a safe starting point and Spec Kit inside the project's rules

- [X] T001 Create the tag `archive/2026-11-12/before-cleanup` at the current `main` head and list it in `log/mapa.md` (an archive tags section)
- [X] T002 [P] Declare `.specify/` and `specs/` as document places in `tools/gates/gate_files.py` (DOC_UNICO and RAIZ_LIMPA accept them), with a pass and a fail fixture under `tools/gates/tests/fixtures/`
- [X] T003 [P] Replace `.specify/memory/constitution.md` with a pointer: "this project's constitution is CLAUDE.md; the Constitution Check is CLAUDE.md's tagged contracts, kept by `tools/gates/contracts.json`" (no rule text of its own)
- [X] T004 [P] Track `.claude/settings.json`, `.claude/hooks/` and `.claude/skills/` and keep the rest of `.claude/` ignored: edit `.gitignore` and remove the `/.claude/` line of `.git/info/exclude`, keeping its `**/.claude/<runtime>` lines
- [X] T005 Enter `.specify/`, `specs/001-single-source-traceability/` and their files in `log/mapa.md` (MAPA_DESATUALIZADO)

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: the register exists, verified, before anything is migrated, triaged or reported into it

- [X] T006 Write `tools/gates/schemas/problem.schema.json` from data-model.md "Problem": `id` "string `P<area>-<n>`, unique, never reused"; `cause` "string or `\"unknown\"`; `\"unknown\"` requires `reading`"; `area` enum `simulation, traffic, walkers, transit, roads, street, lots, buildings, terrain, nature, render, interface, performance, product, tools, docs, process`; `kind` enum `defect, missing-feature, performance, copy, architecture, process`; `status` enum `open, traced, planned, fixed-pending-check, fixed, dropped`; `traced` "date the status first left `open` for `traced`"; `closed` "only with status `fixed` or `dropped`"; `fix` "required for `fixed`"
- [X] T007 Create `tools/atlas/__init__.py`, `tools/atlas/cli.py` (entry, `--json`, exit codes 0/1/2 per contracts/atlas-cli.md) and `tools/atlas/problems.py`: `problems` list with `--area --owner --status --cause`, `add`, `update`, `close --fix`, `drop --why`, the transitions of data-model.md enforced in one setter, writing `log/problems.jsonl` one problem per line as bytes with LF (never `write_text`)
- [X] T008 Add `problems render` in `tools/atlas/render.py` writing the generated view `log/problems.md` (by area, then status), registered as generated in `tools/gates/gate_generated.py`
- [X] T009 [P] Add the register's gate in `tools/gates/gate_data.py`: `log/problems.jsonl` against `problem.schema.json`; a duplicate id, `fixed` without `fix`, `unknown` without `reading`, an evidence path that does not exist: each a failure
- [X] T010 [P] Write `tools/atlas/tests/test_problems.py` (add, every allowed and refused transition, render, a malformed line refused) with a fixture register under `tools/atlas/tests/fixtures/`
- [X] T011 Close: `python -m unittest discover tools/atlas/tests`, `python tools/gates/run_gates.py --file` on every file of Phases 1 and 2, `python tools/gates/run_gates.py --selftest`; commit `tools(atlas): the problem register` and push

**Checkpoint**: the register works; US1 first (every later block reads the new CLAUDE.md)

---

## Phase 3: User Story 1 — One place for the method and every order (Priority: P1) 🎯 MVP

**Goal**: CLAUDE.md correct, holding the method and every order; nothing else carries orders; memory off; the block rule kept by a
mechanism.

**Independent Test**: quickstart.md step 1, plus: a build, a test and a commit are refused while a block is open and allowed at its
close.

### Code

- [ ] T012 [US1] Read whole and list every order in: the 33 notes of `C:/Users/Jonathan/.claude/projects/C--Users-Jonathan-Desktop-city/memory/`, `log/how-we-work.md`, `log/specification.md` ("Constraints", D1 to D5), `.claude/skills/trace-fix/SKILL.md`, `tools/process/CLAUDE.md`, `tools/oracle-4.0/CLAUDE.md`, and the owner's goal of 2026-11-12 (standing authorisation, the skills to use and not to use); append the migration table (order, source, its CLAUDE.md tag, its guard, or "fact → document", "history → `log/history/process.md`", "superseded") as R14 in `specs/001-single-source-traceability/research.md`
- [ ] T013 [US1] Rewrite `CLAUDE.md` under 200 lines, every rule tagged `[c:<id>]`, correcting each defect of spec.md "C. CLAUDE.md as written": (a) an Authority section: the only source of instructions; auto memory off; records, skills, goal files and Spec Kit's constitution hold no orders; a conflicting order is settled here in the same change; (b) no dated status; (c) the method held whole, step by step, no delegation to another file; (d) D1 to D5 settled: the delivered build is `dist/builds/<date>_<commit>/Roadcraft.exe` opened by the Desktop shortcut (`tools/build/deliver.sh`), quick research tests in an isolated folder, Context7 on every implementation, fix and research, the live instance may open a window without taking focus, the image protocol (expectation first, every photo and crop opened, regions at real size, the defect list, the analysis recorded); (e) every order of R14: no agents unless the owner orders them, one engine process at a time while working, the reference profiles and budgets of `log/specification.md` as the bar of a game sold on Steam, never read other sessions' transcripts, repository files written as bytes with LF, the session's clangd watched; (f) the block's close: the whole gates fail it on any finding that is not an open problem of the register, the gates never loosened (FR-052); (g) facts: `deliver.sh`, mimalloc among the fetched libraries, Node only if a remaining tool needs it, no `converter-slice`, Spec Kit's folders, one record per area under `log/history/`; (h) Commands kept to what the work needs
- [ ] T014 [US1] Move the reference parts of the old CLAUDE.md (project structure, data flow and threads, light and clock, the state machines' descriptions, the full command table) into sections of `log/mapa.md`, keeping in `CLAUDE.md` only the rules they carried
- [ ] T015 [US1] Update `tools/gates/contracts.json`: every `[c:<id>]` of the new `CLAUDE.md` has guards, no guard names a removed tag; `[c:generic-tracer]` guarded by `tools/tracer/tests/test_generic.py`; `[c:no-trial]` and `[c:investigate-reading]` guarded by the mechanism of T020 (kind `tool` and `hook`) instead of conduct only
- [ ] T016 [P] [US1] Write `.claude/settings.json`: `"autoMemoryEnabled": false`; `"claudeMdExcludes": ["**/tools/gates/tests/fixtures/**/CLAUDE.md"]`; the hook of T020 (through the `update-config` skill; the hook's file is written by T020 in the same part)
- [ ] T017 [US1] Fold `log/how-we-work.md` into `CLAUDE.md` (steps) and `log/history/process.md` (its two contradiction tables, as history); remove `log/how-we-work.md`
- [ ] T018 [P] [US1] Remove the nested instruction files `tools/process/CLAUDE.md`, `tools/oracle-4.0/CLAUDE.md`, `tools/ports/integration/evidence/merge-flags/resolution/tree/CLAUDE.md`
- [ ] T019 [US1] Add to `tools/gates/gate_docs.py` the rules INSTRUCTION_ELSEWHERE (a `[c:` tag in any tracked file but `CLAUDE.md` and `tools/gates/contracts.json`), PRECEDENCE_CLAIM (a precedence claim — "binding over", "wins over", "BINDING", "overrides CLAUDE.md" — in any tracked file but `CLAUDE.md`), RULE_CONFLICT (in `CLAUDE.md`: a tag defined twice or two rules under one tag; a tag with no guard in `tools/gates/contracts.json`; a guard naming no tag: FR-006), NESTED_CLAUDE (a `CLAUDE.md` other than the root and the excluded fixtures) and MEMORY_ON (`.claude/settings.json` without `autoMemoryEnabled: false`, or a note left in the project's memory folder), each in `tools/gates/registry.py` with pass and fail fixtures under `tools/gates/tests/fixtures/`
- [ ] T020 [US1] Give the block rule its mechanism (FR-051): `tools/tasks/block.py` with `python tools/tasks/cli.py block open <name>|verify|close|status` (state in `dist/block/state.json`); while a block is `open`, `tools/tasks/cli.py run`, `tools/build/build.sh`, `tools/build/deliver.sh` and the git pre-commit hook `tools/build/git-hooks/pre-commit` (installed by `tools/build/build.sh hooks` through `core.hooksPath`) refuse with the reason; `.claude/hooks/block_guard.py` (PreToolUse on Bash and PowerShell) refuses a direct `godot` run while open; `verify` allows them, `close` ends the block; each block's open, verify and close times appended by `block close` to the block-times table of `log/history/process.md` (FR-050); `tools/tasks/tests/test_block.py`
- [ ] T020a [US1] Give FR-052 its mechanism: `python tools/gates/run_gates.py --register log/problems.jsonl` marks each finding with the open problem whose `origin` names `gate:<RULE>:<file>` or as NEW, and exits 1 on any NEW finding (without `--register` the gates fail on every finding, as today: nothing loosened); `block verify` runs it; `python tools/atlas/cli.py problems import-gates <gates json>` enters each standing finding not yet registered as one problem per rule and file (area by the file's place, kind `architecture`, owner the file, criterion "the gate passes on this file"); tests in `tools/gates/tests/` and `tools/atlas/tests/`
- [ ] T021 [US1] Migrate the memory notes by the R14 table (facts into `log/mapa.md` or `log/history/process.md`), and list the 33 notes, `MEMORY.md` and `.claude/metas.md`, `.claude/metas/`, `.claude/REGISTRO-GOAL.md` (their useful content moved into `log/history/process.md`) for removal in T023

### Close

- [ ] T022 [US1] Verify: `python -m unittest discover tools/tasks/tests`, `python tools/gates/run_gates.py --selftest`, the whole gates (FR-052), `wc -l CLAUDE.md` under 200, the block mechanism refusing a build, a test and a commit while open and allowing them at `verify`
- [ ] T022a [US1] Enter the standing gate findings into the register (`run_gates.py --json`, then `atlas problems import-gates`), then the whole gates against the register (`run_gates.py --register log/problems.jsonl`): 0 NEW findings
- [ ] T023 [US1] Apply and deliver: install the git hook (`tools/build/build.sh hooks`); remove the 33 memory notes and `MEMORY.md`, `.claude/metas.md`, `.claude/metas/`, `.claude/REGISTRO-GOAL.md`; quickstart.md step 1; record in `log/history/process.md`; commit `docs(method): CLAUDE.md is the only source of instructions, the block rule kept by a mechanism`; push

**Checkpoint**: one source of instructions; the block rule cannot be broken by accident

---

## Phase 4: User Story 2 — A project folder that holds only what is used (Priority: P1)

**Goal**: the triage done and recorded; nothing abandoned on disk; every output bounded by its owner; nothing of value lost.

**Independent Test**: quickstart.md step 2.

### Code

- [ ] T024 [US2] Add `tools/tasks/lifecycle.py` and `python tools/tasks/cli.py worktrees [--all]`: every worktree (`git worktree list --porcelain`), every sibling copy of the repository on the Desktop, every nested git repository in the tree, every branch (merged or not, commits missing from `main`), every stash; each with size, uncommitted count, last commit date, the class of research R10; exit 1 when one is older than its work (merged, or untouched for the bound in `tools/tasks/suites.json`)
- [ ] T025 [US2] Add an `outputs` bound per owner to `tools/tasks/suites.json` (`max_mb` or `keep`; evidence `while-open`) with its schema in `tools/gates/schemas/suites.schema.json`; enforce it on every write in `tools/tasks/cache.py` (least recently used past `max_mb`) and `tools/tasks/runs.py` (last `keep` runs per task)
- [ ] T026 [US2] Add `python tools/tasks/cli.py clean [--dry-run]` in `tools/tasks/lifecycle.py`: every bound applied now, what it frees printed folder by folder
- [ ] T027 [P] [US2] Make `tools/build/deliver.sh` keep the last two builds under `dist/builds/` plus any build an open problem's evidence names, removing the others on each export
- [ ] T028 [P] [US2] Point `project/export_presets.cfg`'s `export_path` at the folder `deliver.sh` writes (D1)
- [ ] T029 [P] [US2] Write `tools/tasks/tests/test_lifecycle.py` (classes, bounds, `clean --dry-run`, the stale-worktree failure) with fixtures under `tools/tasks/tests/`

### Close

- [ ] T030 [US2] Verify: `python -m unittest discover tools/tasks/tests`, `run_gates.py --file` on the changed files, the whole gates (FR-052)
- [ ] T031 [US2] Write the whole removal list in `log/history/process.md` from `python tools/tasks/cli.py worktrees --all` and `clean --dry-run`: worktrees, copies, nested repositories, branches, stashes, the folders of T039 to T041, each with class, size, what is kept and under which tag
- [ ] T032 [US2] For each worktree classed "already in `main`, with uncommitted changes" (the Codex worktree, temp `before5f` and `base`, the six `city-bisect` copies, `city-head`, `city-nov07`, `city-preport`, agents `a4ad025`, `a5089bf`, `a5ffcd3`, `a6b9f2b`, `ac81c2a`): read the changes; keep those with value as a commit under `archive/2026-11-12/wt-<name>`; write in `log/history/process.md` which were judged empty and why
- [ ] T033 [US2] Keep every stash as a tag `archive/2026-11-12/stash-<subject>` (the old tracer's stash included); then `git stash clear`
- [ ] T034 [US2] Fetch every ref of every nested git repository (`tools/converter/.git` and any other the listing found) into the main repository under `archive/2026-11-12/<repo>/*` before its folder is removed
- [ ] T035 [US2] Tag every branch classed "behaviour to redo" or "old tool history" as `archive/2026-11-12/branch-<name>` and delete it; delete every fully merged branch with `git branch -d`
- [ ] T036 [US2] Remove the worktrees by class (`git worktree remove`, then `git worktree prune`): `.claude/worktrees/*`, `C:/Users/Jonathan/Desktop/city-{bisect,head,nov07,preport,realism,realism-before,realism-pre,scenic,verify}`, `C:/Users/Jonathan/.codex/worktrees/performance-causes`, the three temp scratchpad worktrees; one a running process uses is left and registered
- [ ] T037 [US2] Compare `C:/Users/Jonathan/Desktop/city-archive` with the history file by file, keep what history lacks under `archive/2026-11-12/city-archive`, remove the folder
- [ ] T038 [US2] For each piece classed "worth bringing in" (`wip/allocator-2026-11-10`; agents `a07591d`/`abeaa03` terrain beside roads, `a70fca1` bus boarding, `a40909` umbrellas and backpacks, `a486021` coast stones, `ae22101` interior furniture, `a31bf43` corner lots; stashes "9 of 22 fixed" and P476/P485/P486/P487): read its diff whole against the copy rule, write the verdict, register it with `python tools/atlas/cli.py problems add` (kind `missing-feature` or `defect`, evidence its tag, `spec` its area's future specification; the allocator census: this feature's US6)
- [ ] T038a [US2] For each piece T038 judged this game's own and applicable to today's `main` (FR-015): redo it on today's `main` as a block of its own (code whole, then both DLLs, `quick.sh` on its scenes, the whole gates against the register, photos by the image protocol, km24 measured before and after), merged only if it passes; one that fails is registered with its failure and its tag as evidence
- [ ] T038b [US2] Read the documents of `docs/`, `tools/process/` (records and guides) and `tools/ports/` (reports) for notes of how a feature must behave; move each into the register (kind `missing-feature` or `defect`, origin the file) or the area's history before T039 removes them (FR-010); list in `log/history/process.md` the files judged to hold none
- [ ] T039 [P] [US2] Remove from the working tree (kept by T001 and T034): `tools/converter/`, `tools/process/`, `tools/oracle-4.0/`, `tools/ports/`, `docs/`; drop their lines from `.gitignore` and `.git/info/exclude`; update `log/mapa.md`
- [ ] T040 [P] [US2] Remove the junk `tools/analysis/{-,-E,-v,-x,c++}`, `.claude/state/`, `.claude/.pipeline_state_*`, `dist/trace/*.txt`, `native/build-android/`; find the command that made the dash-named folders (an unquoted argument under `tools/`) and fix it at its cause
- [ ] T041 [US2] Move the proofs `log/research.md` cites (`dist/research/q*/`, `dist/research/measure/`) to `dist/evidence/research/` and update the paths in `log/research.md`; remove `dist/arch/`, `dist/analysis/`, `dist/cache/feature_trace/`, `dist/cache/ftrace/`, `dist/windows/`, `dist/windows-antes-da-volta/`, `dist/Roadcraft.zip`, the `dist/review-*` folders and every other `dist` folder with no owner; run `python tools/tasks/cli.py clean`
- [ ] T042 [US2] Deliver: quickstart.md step 2; the sizes of the project folder and its siblings measured and written in `log/history/process.md`; commits per theme; push

**Checkpoint**: about 95 GB freed; every output bounded; every removed piece reachable by its tag

---

## Phase 5: User Story 3 — One register of every known problem (Priority: P1)

**Goal**: every open item of every tracker in `log/problems.jsonl`, once; no other tracker; one current document per subject.

**Independent Test**: quickstart.md step 3.

### Code

- [ ] T043 [US3] Write `tools/atlas/migrate.py`: read `dist/trace/*/ledger.json` (open and traced problems with cause, fix, evidence), the structured rows of `log/pendencias.md`, `log/performance.md` (P-01 to P-20), the D and S tables of `log/specification.md`, the open parts of `log/live-instance-design.md` and `log/guards-progress.md`, and spec.md's audit findings; write each into the register with its `origin`; print the counts per origin
- [ ] T044 [P] [US3] Add PROBLEMS_ELSEWHERE to `tools/gates/gate_docs.py` (a tracked problem ledger or a symptom/cause table outside `log/problems.jsonl` fails), with fixtures
- [ ] T045 [P] [US3] Rename every Portuguese gate id to English in `tools/gates/registry.py`, `tools/gates/rules.json` and the gates that raise them (MAPA_DESATUALIZADO to MAP_OUTDATED, DOC_UNICO, RAIZ_LIMPA, ESTADO_NA_UI, TEXTO_VIA_TR, NUMERO_MAGICO, SEGUNDO_DONO, TAMANHO_ARQUIVO, TAMANHO_FUNCAO, VALOR_DUPLICADO, DONO_UNICO, UI_SEM_ESTILO_SOLTO), the map's name read as `log/map.md`
- [ ] T046 [P] [US3] Write `tools/atlas/tests/test_migrate.py` on fixture ledgers and tables, and the gates' fixtures for T044 and T045

### Close

- [ ] T047 [US3] Verify: `python -m unittest discover tools/atlas/tests`, `run_gates.py --selftest`, `run_gates.py --file` on the changed files
- [ ] T048 [US3] Run `python tools/atlas/migrate.py`; keep its counts per origin in `log/history/process.md`
- [ ] T049 [US3] Read whole the carried prose of `log/pendencias.md` (after its line 114) and the findings of `log/review.md`; register by hand each open item the importer could not parse; merge duplicates by cause (`related`, one survivor per cause)
- [ ] T050 [US3] Move the photos and crops open problems name from `dist/trace/<slug>/photos/` to `dist/evidence/<id>/`, update each problem's `evidence`, then remove `dist/trace/`
- [ ] T051 [US3] Merge the per-line records into one history per area under `log/history/`: `work-log.md`, `conversor.md`, `causa-raiz.md`, `lab.md`, `base.md`, `arquitetura.md`, `feature-trace.md`, `ferramentas.md`, `tracer.md`, the 22 `trace-*.md`, `guards-plan.md`, `guards-progress.md`, the history parts of `review.md` and `performance.md`; move `live-instance-design.md`'s design into `log/research.md` as an open question; remove the merged files and `log/pendencias.md`; keep in `log/specification.md` only the goals, the reference hardware and the budgets
- [ ] T052 [US3] Rename `log/mapa.md` to `log/map.md` and every reference to it (`CLAUDE.md`, the gates)
- [ ] T053 [US3] Deliver: `python tools/atlas/cli.py problems render`; the whole gates (FR-052); count the current documents (tracked `.md` outside `.specify/`, `specs/`, third-party code and test fixtures; SC-002: at most 25) and write the count; quickstart.md step 3; record in `log/history/process.md`; commits; push

**Checkpoint**: one register; about 25 current documents

---

## Phase 6: User Story 4 — Trace any feature end to end in seconds (Priority: P1)

**Goal**: the game declares its state parts, commands and change kinds; the atlas answers the owner's questions in seconds.

**Independent Test**: quickstart.md step 4.

### Code

- [ ] T054 [US4] Read whole `native/src/sim/state/city_state.h` and the owners of each member; declare `CITY_STATE_PARTS(X)` there (`X(name, owner_symbol, controller, writers)` per contracts/declarations.md), every stateful part with its controller and transition table or `NONE("why")`
- [ ] T055 [US4] Read whole `native/src/sim/state/change_log.h` and every producer and consumer of its lists; declare `CITY_CHANGE_KINDS(X)` (`X(name, payload, producers, consumers, touches)`) and generate the struct's lists from it, so the declaration and the members cannot differ
- [ ] T056 [US4] Declare `CITY_COMMANDS(X)` (`X(name, area, writes, announces, undoable)`) in `native/src/sim/commands/command_registry.h` for every command of `project/game/store/store_api.gd` `COMMANDS`; make the core's dispatch read it
- [ ] T057 [US4] Write `tools/atlas/generate.py`, which writes `project/game/store/store_api_commands.gd` from `CITY_COMMANDS` (run at the close); make `store_api.gd` use it; register it in `tools/gates/gate_generated.py`
- [ ] T058 [P] [US4] Write `tools/atlas/declared.py`: the three X-macros parsed from their headers into the declared model (no engine, no build)
- [ ] T059 [P] [US4] Write `tools/atlas/features.py`: the features from `ToolController.Tool` (`project/game/tools/tool_controller.gd`), the interface's buttons (`project/data/ui/*.json`) and the simulation's step order (`native/src/sim/simulation/simulation.cpp`), each with an id and its entry file:line
- [ ] T060 [US4] Write `tools/atlas/index.py`: the SQLite index `dist/atlas/index.sqlite`, rebuilt by content hash for changed inputs only, joining declarations, `tools/tracer` links, `run_gates.py --json`, `dist/lab_scan.json`, the dead check, `tools/tasks/godot_deps.py` coverage, the engine-free test list, the last profile run report and the register; a progress line each second while it refreshes
- [ ] T061 [P] [US4] Add `--json` output to `tools/gates/run_gates.py` if absent (rule, file, line, owner)
- [ ] T062 [US4] Write `tools/atlas/queries.py`: `feature`, `state`, `writers`, `triggers`, `owner`, `unoptimised`, `uncontrolled`, `features`, with the marks of data-model.md (`single_owner`, `controller`, `source_of_truth`, `optimised` inside/over/unmeasured, `covered`) and gaps shown as gaps
- [ ] T063 [P] [US4] Write `tools/atlas/checks/declared.py` (missing symbols, `StoreApi` against `CITY_COMMANDS`, undeclared kinds drained), `tools/atlas/checks/dead.py` (from `tools/audit/static.py`), `tools/atlas/checks/shapes.py` (from `tools/analysis/causes.py` and `detectors.py`)
- [ ] T064 [P] [US4] Write `tools/atlas/tests/` for declared, features, index (incremental refresh), queries and checks, on a fixture tree
- [ ] T065 [US4] Register `atlas_index` and `atlas_checks` in `tools/tasks/suites.json` with deadlines and inputs keyed by content

### Close

- [ ] T066 [US4] Verify: `python tools/atlas/generate.py`; both DLLs; `quick.sh` on the scenes the dispatch reaches, then `quick.sh --ready` once (`edit_fuzz_test`'s hashes unchanged); `python -m unittest discover tools/atlas/tests`; the whole gates (FR-052); km24 measured, the previous build and this one (memory, CPU, GPU, frame)
- [ ] T067 [US4] Deliver: quickstart.md step 4, one chain read against the code by hand; record in `log/history/tools.md`; commits; push; the executable

**Checkpoint**: the owner's questions answered in seconds

---

## Phase 7: User Story 5 — Checks in seconds, streamed, never blocking (Priority: P2)

**Goal**: the simulation's rules checked engine-free and deterministically in seconds; every process streams and stops alone.

**Independent Test**: quickstart.md step 5.

### Research (before the block's code; an isolated folder, CLAUDE.md D2)

- [ ] T068 [US5] Prove in an isolated folder of the scratchpad that Catch2 v3 builds and runs with llvm-mingw `20261020-ucrt-x86_64` and CMake 4.4, exceptions on for the test target only (Context7 `/catchorg/catch2`); record the proof in `log/research.md`

### Code

- [ ] T069 [US5] Split `native/CMakeLists.txt`: `city_sim` (static: `src/core`, `src/sim`), `citycore` (shared: `src/render`, `src/api`, linking `city_sim`), `city_tests` (Catch2 v3 by FetchContent, `Catch2::Catch2WithMain`, `include(Catch)` after appending `extras` to `CMAKE_MODULE_PATH`, `catch_discover_tests(city_tests ADD_TAGS_AS_LABELS)`); PGO and hardening options kept per target
- [ ] T070 [US5] Write `native/tests/support/scenario.h` and `scenario.cpp`: a city built in memory from a seed and a command list, stepped a fixed number of ticks with a fixed camera path, returning the state hash
- [ ] T071 [US5] Trace L3 with the `trace-fix` skill: read every random draw of `native/src/sim/` and route it through streams seeded by the scenario's seed and the agent's id (owner `native/src/core/random*`)
- [ ] T072 [US5] Trace S2 with the `trace-fix` skill: read whole `native/src/sim/simulation/simulation.cpp` and the damage owners; the damage clock steps on the tick's schedule (the writer waits for the map instead of `try_to_lock`)
- [ ] T073 [US5] Write engine-free tests by area under `native/tests/{traffic,walkers,roads,lots,buildings,terrain,commands,state}/` for the rules the engine scenes check today through a town (vehicle motion, walkers apart, grade, junction sweep, kerb drive, save/undo hashes), tagged by area
- [ ] T074 [US5] Write `native/tests/state/determinism_test.cpp`: the same scenario twice gives the same hash
- [ ] T074a [US5] Write the engine scene `project/tests/core/determinism_test.tscn`: the same seed, commands and camera path run twice through the engine give the same state hash (SC-009 "through the engine alike")
- [ ] T075 [US5] Add the `sim_tests` task to `tools/tasks/suites.json`: build `city_tests`, run the labels the changed files reach, deadline 60 s
- [ ] T076 [US5] Add the heartbeat in `tools/tasks/proc.py`: a progress line every 5 s per running task (the period named in `suites.json`); a task silent past it is reported; past its deadline stopped by PID through its Job Object
- [ ] T077 [US5] Split `quick --ready` in `tools/tasks/suites.json`: engine-free tests first, engine scenes after, one summary of ran, passed, failed, skipped
- [ ] T078 [P] [US5] Write `tools/tasks/tests/test_heartbeat.py` (a silent task reported, a hung task stopped, the others finish)

### Close

- [ ] T079 [US5] Verify: both DLLs, `city_tests`, `quick.sh --ready` once, `tools/tasks/tests`, the whole gates (FR-052), km24 measured, the previous build and this one; photos only if the damage timing changes what is seen (a blast with the clock running)
- [ ] T080 [US5] Deliver: close the L3 and S2 traces; quickstart.md step 5; record in `log/history/tools.md`; commits; push; the executable

**Checkpoint**: a rule checked in seconds; results reproduce

---

## Phase 8: User Story 6 — Whatever breaks is reported by the game, by owner (Priority: P2)

**Goal**: world rules, declared writes, needless work and budgets checked while the game runs; one report per run into the register.

**Independent Test**: quickstart.md step 6.

**Depends on**: US4 (declarations), US5 (scenario runner)

### Code

- [ ] T081 [US6] Add write revisions per state part and write scopes (debug and profile builds) in `native/src/sim/state/state_scope.h`, entered by the command dispatch and each simulation step, kept out of the state hash and the save (`[c:state-hash]`); a write outside the declared scope is recorded for the run report
- [ ] T082 [US6] Count needless work per change kind and consumer (output hash equal to the one before) in each consumer of `CITY_CHANGE_KINDS` (`native/src/render/render_world.cpp` and the render adapters, the traffic's, transit's and lot cache's drains), and the entries announced that no consumer drains (an update whose result nobody reads, FR-032)
- [ ] T083 [US6] Implement the world rules of `.claude/skills/trace-fix/rules.json` as run-time checks at their owners, on `native/src/core/checks.h`: B1/B2 bodies apart (vehicles, walkers, furniture, plants, buildings) in `native/src/sim/traffic/` and `native/src/sim/street/`; G1 resting on the ground; M1/M2 continuous motion and nothing made or removed in sight; M3/M4 movers' ways; S2 writes inside declarations; P1 a frame's work follows what changed; a strung span has its wire drawn (`native/src/render/props/`); every placed element (furniture, poles, trees, lot elements) has its instance in what is drawn once its chunk is built (FR-032); each check's cost inside a budget of its own
- [ ] T084 [US6] Write the run report (contracts/run-report.md) from the engine (`native/src/api/city_core_run_report.cpp`, debug and profile only) and from the engine-free scenario (`native/tests/support/scenario.cpp`); its limits come from the game's data through `DataTables` (`[c:data]`), never read by a tool; schema `tools/gates/schemas/run_report.schema.json`; counts streamed every 5 s through the orchestrator
- [ ] T085 [US6] Add `problems import <report>` to `tools/atlas/problems.py`: violations entered once, deduplicated by rule, owner and cause
- [ ] T086 [P] [US6] Compile the probes (`native/src/api/*_probe*.cpp`, `native/src/render/**/*_probe.cpp`, their registration in `native/src/api/city_probes.h`) only with `CITY_PROBES` in `native/CMakeLists.txt`; off for release
- [ ] T087 [US6] Add the profile build (release optimisation, `CITY_CHECKS`, `CITY_PROBES`, no libc++ hardening) in `native/CMakeLists.txt` and `tools/build/build.sh`, its line `windows.profile.x86_64` first in `project/bin/citycore.gdextension`, the export preset "Windows Profile" with custom feature `profile` in `project/export_presets.cfg`
- [ ] T088 [US6] Add timing zones by owner in `native/src/core/zones.h` (profile and debug), summarised into the run report against `project/data/game/budgets.json` (the budgets of `log/specification.md`, with its schema, read by `DataTables`)
- [ ] T089 [US6] Bring in the allocator census of `archive/2026-11-12/branch-wip-allocator-2026-11-10` redesigned for this build (memory by thread and by asker into the run report), if T038 judged it this game's own
- [ ] T090 [US6] Write the planted scenarios `native/tests/state/scenario_overlap_test.cpp` (engine-free) and `project/tests/core/scenario_overlap_test.tscn`; the `scenario_overlap` task in `tools/tasks/suites.json`

### Close

- [ ] T091 [US6] Verify: the three builds, `city_tests`, `quick.sh --ready` once, the whole gates (FR-052), `llvm-nm` on the release DLL lists no probe, km24 measured, the previous build and this one, on the release build (the checks cost nothing there) and on the profile build (their cost stated)
- [ ] T092 [US6] Deliver: quickstart.md step 6; record in `log/history/tools.md`; commits; push; the executable

**Checkpoint**: defects reported by the game, not found in photos

---

## Phase 9: User Story 7 — One layout for tests, tools and the skill (Priority: P2)

**Goal**: tests one folder per kind; tools one per question; the skill working, agreeing with CLAUDE.md, scoped to this project.

**Independent Test**: quickstart.md step 7.

**Depends on**: US4 (the atlas replaces the retired tools and serves the skill)

### Code

- [ ] T093 [US7] Move `project/tests/core/` and `project/tests/e2e/` to `project/tests/scenes/` (scenes, scripts and their uids); update every path in `tools/tasks/suites.json` and the scripts that load them
- [ ] T094 [US7] Write the placement rule in `CLAUDE.md` (engine-free in `native/tests/<area>`, engine scenes in `project/tests/scenes`, probes in `project/tests/perf`, captures in `project/tests/visual`, a tool's tests in `tools/<tool>/tests`) and its gate TEST_PLACE in `tools/gates/gate_files.py`
- [ ] T095 [US7] Retire `tools/analysis/` (moving `sampler.py`, `sampler_report.py`, `unwind.py` to `tools/build/`) and `tools/audit/` (folded into `atlas check dead`); update `tools/tasks/suites.json`, `CLAUDE.md`'s commands, `log/map.md`, and `tools/gates/contracts.json` (`[c:perf-architecture]` guarded by `atlas check shapes`); the removed part kept by T001's tag
- [ ] T096 [US7] Stage the skill's redesign (research R11) in a copy under the scratchpad: `SKILL.md` citing CLAUDE.md's tags and the block rule; problems written through `tools/atlas` into `log/problems.jsonl`; `trace`, `writers` and `card` calling the atlas (the `ftrace` import gone from `route.py` and `card.py`); `rules.json` kept as the list the core's checks implement; the read rule counting the file changed and its callers
- [ ] T097 [P] [US7] Write `tools/atlas/checks/skill.py` (`atlas check skill`: every command of the skill reaches an existing tool)

### Close

- [ ] T098 [US7] Verify: the whole gates (FR-052), `tools/atlas/tests`, `tools/tasks/tests`, the moved scenes through `quick.sh --ready` once; the copy's `selftest.py` and `selftest_guard.py` whole and one real trace cycle in the copy, the result recorded in `log/history/tools.md`
- [ ] T099 [US7] Deliver: only if T098 passed whole (the owner's standing authorisation is the word FR-045 asks for), deploy the copy over `.claude/skills/trace-fix/` in one swap and move its hooks from `C:/Users/Jonathan/.claude/settings.json` to the project's `.claude/settings.json` (through `update-config`); otherwise the live skill stays as it is and the failure is registered; `atlas check skill`; quickstart.md step 7; record; commits; push

**Checkpoint**: one place per test kind and per question

---

## Phase 10: User Story 8 — The game's and the product's constraints by mechanism (Priority: P3)

**Goal**: copy, patch and budget refused by checks; the release has a version, a processor gate, a crash report, no probes, no
unused texts.

**Independent Test**: quickstart.md step 8.

**Depends on**: US6 (profile build, budgets)

### Research (before the block's code; an isolated folder)

- [ ] T100 [US8] Prove in an isolated Godot 4.7.2 project in the scratchpad that a baseline-x86-64 extension can check AVX2/FMA/BMI and load the core with `GDExtensionManager.load_extension` before any scene uses it (Context7 `/websites/godotengine_en`); record the proof in `log/research.md`

### Code

- [ ] T101 [P] [US8] Add the failing cases to `tools/gates/tests/` for LAB_COPY on a changed line with a structural match (`tools/gates/gate_lab.py`) and NUMBER_MOVED without its cause written (`tools/gates/gate_patch.py`); fix either gate where its case passes
- [ ] T102 [US8] Turn `dist/evidence/research/measure/run_game.py`'s profiles into the task `measure <profile> <tier> <map>` in `tools/tasks/` on the exported profile build, run in parts each under its deadline, failing on the run report's budgets (T4)
- [ ] T103 [P] [US8] Write `application/config/version` in `project/project.godot` from `tools/build/deliver.sh` (commit and date) and show it in the interface (a `tr` key in `project/data/i18n`, the panel's data in `project/data/ui`)
- [ ] T104 [P] [US8] Keep file logging on and set `debug/settings/crash_handler/message` in `project/project.godot`; an interface entry that opens `user://logs` (data in `project/data/ui`, text by `tr`); release symbols kept beside each build under `dist/builds/<build>/symbols/` by `tools/build/deliver.sh`, not shipped
- [ ] T105 [US8] Trace M2 with the `trace-fix` skill and implement the processor gate if T100 proved it: `native/cpu_gate/` built for baseline x86-64 in `native/CMakeLists.txt`, `project/bin/cpu_gate.gdextension`, the core loaded only by it; the refusal through `GameFlow`'s boot refusal (`[c:flow]`, `boot_refusal_test`), its message in the player's language (`project/data/i18n`)
- [ ] T106 [US8] Remove the 940 texts nothing shows from `project/data/i18n/*.csv` and the data keys nothing reads (`atlas check dead`); register the 8 commands nothing sends and the 14 only tests send as problems of their areas
- [ ] T107 [P] [US8] Register the store-integration decision (overlay, achievements, cloud saves) as a problem of area `product`, kind `missing-feature`, `spec` its own future specification

### Close

- [ ] T108 [US8] Verify: the three builds, the export, `quick.sh --ready` once, the whole gates (FR-052), km24 measured, the previous build and this one; photos of what changed on screen (the version, the logs entry, the refusal screen) by the image protocol
- [ ] T109 [US8] Deliver: quickstart.md step 8; record in `log/history/product.md`; commits; push; the executable

---

## Phase 11: Polish & Cross-Cutting Concerns

- [ ] T110 Run `/speckit-converge`; do every task it appends; then quickstart.md whole once, the whole gates (FR-052), `quick.sh --ready` once
- [ ] T111 Measure the project folder and its siblings (SC-003), the share of commits to tools against the game since the feature began and the median time from a problem's `opened` to its `traced` (`atlas problems stats`, SC-013), and the current documents' count (SC-002); write them in `log/history/process.md`
- [ ] T112 Update `log/map.md` (every file created or deleted, every archive tag), spec.md's status, the line's records with time and commit
- [ ] T113 Commit per theme, one push to `origin/main`, the executable with `tools/build/deliver.sh export`; report to the owner in Portuguese: what was done, what failed, what stays open, and the next specifications, one per area

---

## Dependencies & Execution Order

- **Setup → Foundational → US1** (first: every later block reads the new CLAUDE.md and runs under the block mechanism)
- **US2** after US1; **US3** after US2 (the ledgers' photos and `dist/trace` handled once)
- **US4** after US1; **US5** after Foundational; **US6** after US4 and US5; **US7** after US4; **US8** after US6
- **Polish** last
- One block is open at a time (CLAUDE.md): blocks run in sequence; [P] marks tasks of one part that touch different files

## Implementation Strategy

1. Setup, Foundational, US1 (MVP): one correct source of instructions, the block rule kept by a mechanism, the register.
2. US2: about 95 GB freed, the triage recorded, outputs bounded.
3. US3: one register; about 25 documents.
4. US4: the atlas; the owner's questions in seconds.
5. US5: rules checked in seconds, reproducible.
6. US6: the game reports what breaks.
7. US7: one layout; the skill working.
8. US8: the product's needs.
