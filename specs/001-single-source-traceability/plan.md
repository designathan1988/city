# Implementation Plan: One Source of Truth, a Clean Project and End-to-End Feature Traceability

**Branch**: none (direct on `main`, CLAUDE.md `[c:main-direct]`) | **Date**: 2026-11-12 | **Spec**: [spec.md](spec.md)

**Input**: Feature specification from `specs/001-single-source-traceability/spec.md` (revision 2)

## Summary

Remove the six causes the audit found (spec, "Judgement"), each at its owner: CLAUDE.md becomes the one instruction source with
memory off and nothing else carrying orders (J2); every artifact gets an owner, a bound and an end, and the 106 GB of copies,
worktrees, caches and obsolete trees are triaged and removed with history kept under tags (J1); one versioned register replaces the
seven trackers (J1); the game declares its state parts, commands and change kinds in X-macros the compiler keeps honest, and one
tool, the atlas, joins those declarations with the generic tracer, the existing fast checks, test coverage and measured costs to
answer "who owns, who writes, what triggers what, what is not optimised" in seconds (J3); the simulation's rules get an
engine-free, deterministic test level in seconds (J4); the game checks the world rules while it runs and reports by owner (J5);
the product gets its version, logs, processor gate, a release library without probes and a profile build to measure on (J6). The
trace-fix skill keeps its procedure and gives its rules, register and queries to those owners (staged, deployed on the owner's word).

## Technical Context

**Language/Version**: C++20 (llvm-mingw `20261020-ucrt-x86_64`, CMake 4.4) for the core; GDScript (Godot 4.7.2) for the game;
Python 3.12 for tools; Markdown/JSON for records.

**Primary Dependencies**: godot-cpp (API 4.7, vendored); meshoptimizer, mimalloc, Clipper2 (FetchContent, existing); Catch2 v3
(FetchContent, new, test target only); Python standard library only for tools (`sqlite3`, `json`, `subprocess`); the existing
`tools/tracer` as the code-reading library.

**Storage**: files in the repository (`log/problems.jsonl`, `log/*.md`, X-macro headers); generated, bounded indexes and reports
under `dist/` (`dist/atlas/index.sqlite`, `dist/atlas/declared.json`, `dist/runs/`, `dist/evidence/`).

**Testing**: engine-free Catch2 tests (`native/tests`, CTest labels by area), engine scenes (`project/tests/scenes`), performance
probes (`project/tests/perf`), visual captures (`project/tests/visual`), tool tests (`tools/<tool>/tests`, `unittest`); all run by
`tools/tasks`.

**Target Platform**: Windows 10/11 x86-64-v3 (players); development on the same. Android leaves the targets (its build folder goes).

**Project Type**: desktop game (Godot + C++ GDExtension) with its development tools.

**Performance Goals**: atlas query < 5 s; register query < 5 s; engine-free rule check < 10 s; engine scene check < 2 min; whole
block-close check < 10 min; run report < 10 s after a run; the game's budgets of `log/specification.md` (median 1080p `medium` 60 fps,
floor 1080p `low` 30 fps).

**Constraints**: CLAUDE.md whole (block rule: coded with nothing run, verified once at the end; agents only to read, the project's
`reader`, at most two at once, every report checked, `[c:agents-read-only]` since the owner's addendum of 2026-11-12; costliest fix at the cause;
the reference read for behaviour only; English in the repository; one engine process at a time while working; never close the
owner's game; processes watched with deadlines); the trace-fix skill and hooks change only staged and on the owner's word; every
removal written whole in `log/history/process.md` before it happens (the owner's standing authorisation of 2026-11-12 stands for
his answer); nothing of value lost (tags).

**Scale/Scope**: 1,184 game files (core 24, sim 397, render 266, api 77; about 95,000 lines of C++); about 150 commands; 20-odd change
kinds; 667 documents to bring to about 25; 59 worktrees, 123 branches, 7 stashes to triage; 106 GB to bring under 12 GB; about 400
open problems to migrate.

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

`.specify/memory/constitution.md` is still Spec Kit's blank template; by this plan (research R12) it becomes a pointer to CLAUDE.md.
The check below is therefore made against CLAUDE.md's tagged rules.

| CLAUDE.md rule | This plan | Verdict |
|---|---|---|
| `[c:single-truth]`, `[c:layers]`, `[c:one-owner]` | declarations sit in their owners' headers; `city_sim` keeps core and sim free of the engine; nothing new writes `CityState` | pass |
| `[c:no-god-file]` | the atlas split by responsibility (index, queries, checks, register); declarations next to each owner | pass |
| `[c:data]`, `[c:no-magic]` | bounds of outputs in `suites.json`; budgets in a data table read by the measuring task | pass |
| `[c:fix-decision]`, `[c:no-patch]`, `[c:plan-costliest]` | each cause removed at its owner (declarations instead of a fourth tracer; engine-free tests instead of faster engine boots; bounds in each owner instead of a cleaning script) | pass |
| `[c:no-roadcraft-copy]`, `[c:lab-reference]` | branches built from the reference are not merged; their behaviour is registered to be designed | pass |
| `[c:no-trial]`, `[c:verify-reach]` | the work is coded as blocks; `quickstart.md` runs once at each block's close | pass |
| `[c:agents-read-only]` (the owner's addendum, 2026-11-12) | every edit, removal, build and run done by the main session; a `reader` only surveys, its report checked against the files | pass |
| `[c:hidden-runs]`, `[c:watched-process]`, `[c:no-blind-wait]`, `[c:parallel-tasks]` | every run through `tools/tasks` with deadline, heartbeat and PID stop | pass |
| `[c:checks-can-fail]` | every new check exits non-zero on a finding and fails on an empty input | pass |
| `[c:generic-tracer]` | `tools/tracer` stays generic; the game's own knowledge comes from the game's declarations, not from rules in the tracer | pass |
| `[c:english]` | every new file in English; Portuguese record names renamed | pass |
| `[c:records]` ("a file created or deleted enters `log/mapa.md`") | kept, with the map renamed `log/map.md` and the archive tags listed in it | pass |
| `[c:trace-fix]`, the owner's "hands off the skill" | the skill's redesign is staged in a copy and deployed only on the owner's word | pass, gated by the owner |
| DOC_UNICO / RAIZ_LIMPA (gates) | refuse Spec Kit's folders today | **violation until Block A**: justified below |

Re-check after Phase 1: the design adds no rule that contradicts CLAUDE.md; the only standing violation is the gates' refusal of
Spec Kit's layout, removed by the first block.

## Project Structure

### Documentation (this feature)

```text
specs/001-single-source-traceability/
├── spec.md              # the specification (revision 2)
├── plan.md              # this file
├── research.md          # Phase 0: decisions R1 to R13
├── data-model.md        # Phase 1: entities, fields, transitions
├── quickstart.md        # Phase 1: the proof, run once at each block's close
├── contracts/
│   ├── atlas-cli.md     # the atlas's commands
│   ├── declarations.md  # the core's X-macro registries
│   └── run-report.md    # what every run writes
├── checklists/requirements.md
└── tasks.md             # Phase 2 (/speckit-tasks)
```

### Source Code (repository root), after this feature

```text
CLAUDE.md                         # the only instructions (< 200 lines)
.claude/
├── settings.json                 # tracked: autoMemoryEnabled false, claudeMdExcludes, the project's hooks, the skill's hooks
├── hooks/                        # block_guard, destructive_guard, agent_guard (agents only read, at most two at once)
├── agents/reader.md              # the one agent: tools that read and nothing else
└── skills/trace-fix/             # the procedure; uses the atlas and the register
.specify/                         # Spec Kit; constitution = pointer to CLAUDE.md
specs/NNN-name/                   # one folder per specification
log/
├── map.md                        # every file and folder; archive tags; was mapa.md
├── problems.jsonl                # the one register (source)
├── problems.md                   # generated view
├── specification.md              # goals, budgets, reference hardware
├── research.md                   # research with proofs
└── history/<area>.md             # one history per area (merged from the per-line records)
native/
├── CMakeLists.txt                # city_sim (static) + citycore (GDExtension) + city_tests; debug/profile/release
├── src/core, src/sim             # engine-free: city_sim
├── src/render, src/api           # citycore; probes only with CITY_PROBES
└── tests/                        # engine-free Catch2 tests and scenarios, by area
project/
├── bin/citycore.gdextension      # profile entry first, then debug, release
├── bin/cpu_gate.gdextension      # the processor gate (baseline x86-64)
└── tests/{scenes,perf,visual}/   # engine tests (scenes = former core/ and e2e/)
tools/
├── atlas/                        # the one traceability tool + the register's CLI + its checks; tests/
├── tracer/                       # the generic code reader (unchanged role); tests/
├── gates/                        # rules, copy scan, contracts; tests/
├── tasks/                        # the one orchestrator; bounds, clean, worktrees; tests/
├── build/                        # build, deliver, pgo, sampler, photodiff, free_dll
└── assets/                       # the asset kitchen
dist/                             # bounded, one owner per folder: atlas/, cache/, builds/ (last 2), runs/, evidence/, measure/
```

Leaves the working tree (kept under `archive/2026-11-12-*` tags): `tools/converter`, `tools/converter-slice`, `tools/process`,
`tools/oracle-4.0`, `tools/ports`, `tools/analysis` (except `sampler*.py` and `unwind.py`, which move to `tools/build`),
`tools/audit` (folded into the atlas), `docs/`, the 22 `log/trace-*` and the `guards-*` files (merged into `log/history/tools.md`),
`log/how-we-work.md` (into CLAUDE.md), `.claude/metas*`, `.claude/REGISTRO-GOAL.md`, `.claude/state`, `.claude/.pipeline_state_*`,
the junk folders `tools/analysis/{-,-E,-v,-x,c++}`, `native/build-android`, and the worktrees and copies of research R10.

**Structure Decision**: one repository, the game's three layers unchanged (`core`→`sim` engine-free, `render`, `api`), the
engine-free part built as its own library so it can be tested alone; tests one folder per kind; tools one folder per question; one
document per subject.

## Delivery order (blocks; each coded whole, verified once at its end by the `quickstart.md` steps it reaches)

1. **Block A — Authority** (J2): CLAUDE.md rewritten (method and every order, from memory, `how-we-work.md`, `specification.md`'s
   constraints and the skill's text; D1 to D5 settled; precedence stated; under 200 lines); `.claude/settings.json` tracked
   (memory off, excludes); the 33 memory notes migrated and removed; nested CLAUDE.md files removed; gates for instructions
   elsewhere, precedence claims, nested files and memory; the block rule given a mechanism (FR-051: the orchestrator, `build.sh`,
   `deliver.sh`, a git pre-commit hook and a project hook refuse runs and commits while a block is open); the whole gates' close
   semantics (FR-052); Spec Kit declared; constitution as pointer. Unblocks everything: every later session reads one source and
   cannot break the block rule by accident.
2. **Block B — Triage and cleanup** (J1): the list of research R10 written whole in `log/history/process.md`; archive tags; worktrees, copies,
   branches and stashes handled by class; obsolete trees removed; `dist/` bounds in each owner, `clean` and `worktrees` commands;
   `deliver.sh` keeps two builds; the export preset's stale path fixed. The "to verify" branches are read and registered here;
   their code is redone in the area specs, or in Block E when it is this feature's (the allocator census).
3. **Block C — Register** (J1): schema, CLI, migration of every tracker (ledgers, `pendencias.md`, `performance`, `review`,
   `specification`, live-instance design, guards, this audit), duplicates merged by cause; trackers removed; documents merged into
   `log/history/<area>.md`; `log/mapa.md` renamed `log/map.md`; Portuguese names renamed.
4. **Block D — Engine-free level** (J4): the native split, Catch2, `city_tests`, scenario runner, seeded streams, the damage clock on
   schedule (S2), the `sim_tests` task with labels by area.
5. **Block E — Declarations and run-time checks** (J3, J5): the three X-macros, write revisions and scopes, needless-work counters,
   the world rules as checks, run reports, `StoreApi.COMMANDS` generated; probes behind `CITY_PROBES`; the profile build and its
   preset; timing zones by owner.
6. **Block F — Atlas** (J3): index, queries, checks (`declared`, `dead`, `shapes`, `skill`), the register's commands, retiring
   `tools/analysis` and `tools/audit`.
7. **Block G — The skill** (J1, J2, J3, J5): staged redesign (research R11), its self-test and one real cycle in the copy; deployed
   on the owner's word.
8. **Block H — Product** (J6): version, logging and crash message, the processor gate (after its isolated proof), symbols beside each
   build, the store-integration decision registered.

Each block ends with: both DLLs, the steps of `quickstart.md` it reaches, the gates, the record in `log/history/<area>.md`, a commit
per theme, one push, the executable when the game changed.

## Complexity Tracking

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| DOC_UNICO and RAIZ_LIMPA refuse `.specify/` and `specs/` until Block A | the specification had to exist before the gates could be told where it lives | committing Spec Kit's files with the gates failing would empty `[c:checks-can-fail]` of meaning; Block A declares the places first |
| A new test dependency (Catch2) | engine-free tests need a runner with tags, durations and CTest integration | hand-written test mains (no discovery, no labels, no reporters); booting the engine (the cause of the minutes) |
| A third build of the core (profile) | measuring CPU on a hardened debug library is wrong, and the release library must carry no probes | measuring on debug (distorted); probes in release (shipped test code) |
| A processor-gate extension | the core is built for x86-64-v3 and must refuse an older processor before any of its code runs | a check inside the core (its static initialisers may already use AVX2); dropping x86-64-v3 (costs the performance the budgets need) |
