# Feature Specification: One Source of Truth, a Clean Project and End-to-End Feature Traceability

**Feature Branch**: none — the work is direct on `main` (CLAUDE.md `[c:main-direct]`); feature id `001-single-source-traceability`

**Created**: 2026-11-12

**Status**: Draft (revision 2: the second audit of the same day merged in)

**Input**: The owner's requests of 2026-11-12, given in Portuguese, summarised.
First: "Why does this project have dozens of documents? Audit everything once and for all and leave one source of truth. Not
everything goes in CLAUDE.md, but the work method must be in CLAUDE.md and not in files scattered elsewhere. Why does memory win
over CLAUDE.md? Check in the official documentation which approach is right, and whether CLAUDE.md is written right. Tests, tools
and everything else are scattered: fix all of it now. Identify the project's and the game's problems and the constraints that must
hold (the roadcraft is a reference for how a feature must behave, never code to copy; every feature built whole for this game's
architecture). Always implement with proper optimisation, performance and quality: the game will be sold on Steam. Development is
far too slow: accelerate it a lot, intelligently, always solving causes and never effects, however costly. Tests, measurements and
processes must take seconds and bring everything needed; nothing in the background unwatched; no task may stop another. I want to
trace features end to end, precisely and fast: what has an owner or not, a state controller or not, follows the source of truth or
not, is optimised or not; identify everything that breaks without hunting item by item or running endless processes — traffic,
pedestrians, simulation, terrain, buildings, things that update or create other things without need."
Second: "There is also the trace-fix skill: I do not know whether it helps or needs to change. The project folder is absurdly large,
over 30 GB: find what is no longer used or useless and remove it. Worktrees that are worthless, delete; worktrees that are worth
something and were not merged, merge — but verify. Audit the whole project, find every problem in every possible area, join it with
the previous specification to assess and judge, and so find the right solution."

## Clarifications

### Session 2026-11-12

- Q: Auto memory: turn it off for this project, or keep it for non-instruction notes only? → A: Off for this project. Every durable
  fact lives in the repository (CLAUDE.md for orders, the subject documents for the rest). The owner had already decided the same in
  the earlier project ("Claude's automatic memory stays off, the user's decision", `tools/process/CLAUDE.md`, there in Portuguese), and the
  inversion that made memory "win" came from memory itself. The owner may reverse this answer.
- Q: Obsolete trees and the old feature tracer: history only, or an outside archive? → A: History only. They leave the working tree
  and stay reachable in the repository's history under a named tag (the second request: "find what is no longer used or useless and
  remove it"). The old feature tracer, stopped by the owner's order of 2026-11-11, leaves with them; its stash is kept as a tagged
  commit before any stash is dropped.

- Q: Must the owner answer before removals and before the skill's redesign is deployed? → A: No. The owner gave standing
  authorisation (2026-11-12: "I do not want to keep answering things and giving permission"): every removal the tasks name runs
  once its list is written and what has value is kept under a tag; the skill is deployed when its self-test and a real cycle pass
  whole (that authorisation is the word FR-045 asks for).

## Audit Findings (2026-11-12, tree at `89be3acd`)

What was read: CLAUDE.md whole; `log/how-we-work.md` whole; `log/specification.md` whole; every structured row of
`log/pendencias.md`; the 33 auto-memory notes and their index; the trace-fix skill (`SKILL.md`, `rules.json`, how its scripts reach
the tracers); the open trace ledgers; `.claude/`; the user-level hook settings; the nested instruction files; every worktree, branch
and stash; the disk use of every top folder; the official Claude Code documentation on CLAUDE.md, auto memory, rules, skills and
hooks. Run (read-only, whole output read): the gates (3 s), the static audit (10 s), the copy scan (9 s). Inventoried by size,
subject and date but not read line by line: `log/review.md` (354 KB), `log/mapa.md` (190 KB), the carried prose of
`log/pendencias.md`, `tools/process/records` (384 files), `tools/ports` (137), `docs/` (31). Their content is migrated or retired by
this feature's own work (FR-008 to FR-012).

### A. The work method is written in many places, and three of them each claim to win

| Where | What it claims |
|---|---|
| CLAUDE.md, Method step 5 | "binding over every skill, memory and record: ... his words in `log/how-we-work.md`" |
| `log/how-we-work.md` | "Where anything (a memory, a record, a skill, an old plan) says otherwise, this page and CLAUDE.md win" |
| Memory `mandatory-pause-2026-11-11` | "It wins over CLAUDE.md, memory, skills and records where they differ" |
| Memories `no-city-tests-mid-work`, `fix-cause-effort-neutral`, `hook-hands-off`, `never-wake-stopped-agents`, `feature-tracer-generic`, `no-unwatched-processes`, `one-godot-at-a-time`, `judge-photos-by-quality`, `photos-follow-the-change` and others | each marked "BINDING", each holding orders found nowhere else in full (the image protocol, the fourteen standing orders, staging of skill changes, one Godot at a time, the tracer's genericity) |
| `log/specification.md` "Constraints" | orders added on 2026-11-11 that CLAUDE.md does not yet carry (divergences D1 to D5) |
| `.claude/skills/trace-fix/SKILL.md` | its own order ("test it, photograph it" per problem; `superpowers:test-driven-development` for the fix), contradicted by the block rule |
| `tools/process/CLAUDE.md`, `session-rules.md`, `GODOT-PORT-GUIDE.md` | the old port process: they order porting and translating the reference, now forbidden; this nested CLAUDE.md loads whenever a file under `tools/process` is read |
| `tools/oracle-4.0/CLAUDE.md`, `tools/ports/integration/evidence/merge-flags/resolution/tree/CLAUDE.md` | the old JavaScript game's instructions and an old copy of the project's CLAUDE.md, loaded on demand the same way |
| `.claude/metas.md`, `.claude/REGISTRO-GOAL.md` | goals of earlier sessions, cleared since |

Superseded memories are still loaded into every session: `delegate-port-with-trace`, `commit-push-regularly`,
`deliver-exe-after-each-fix`, `diagnosis-2026-11-08`, `code-first-measure-at-end`, part of `coordinate-parallel-sessions`.
`mandatory-pause-2026-11-11` also keeps the project's status, a second copy of `log/specification.md`.

### B. What the official Claude Code documentation says (code.claude.com/docs/en/memory, read 2026-11-12)

- CLAUDE.md is written by the owner and holds instructions and rules; auto memory is written by Claude and holds learnings and
  context Claude cannot derive from the code. Both are loaded as context; neither is enforced configuration.
- When two instructions contradict each other, "Claude may pick one arbitrarily": no file wins by declaring it.
- A rule that must always happen at a given point is a hook, not a sentence; a procedure needed only for some tasks is a skill;
  rules for part of the tree go in path-scoped `.claude/rules/` files; a CLAUDE.md in a subfolder loads when files there are read.
- Each CLAUDE.md should stay under 200 lines; long reference material belongs in documents it points to or in skills.
- Auto memory can be turned off for one project (`autoMemoryEnabled: false` in the project's settings).

So "memory wins over CLAUDE.md" has no basis: the inversion was written into the memory notes themselves, and the conflicts it
created are resolved by chance, session by session.

### C. CLAUDE.md as written

- 200 lines, at the official ceiling; about half is reference (folder structure, commands, threads) rather than instructions.
- It keeps dated status ("Where the tree stands (2026-11-10)") that goes stale.
- It hands the binding method to another file (`log/how-we-work.md`) instead of holding it.
- It lists fifteen per-line record files as required reading, several with Portuguese names against its own English rule.
- It describes obsolete folders (`converter`, `converter-slice`, `process`, `ports`) as live parts of the project.
- Known divergences still unfixed: the executable path (the export preset still writes `dist/windows/Roadcraft.exe`, stale; the
  delivered build is under `dist/builds/` with a Desktop shortcut), the research-test allowance, the scope of documentation lookups,
  the live instance's window, the image protocol (D1 to D5 of `log/specification.md`).
- It says nothing about precedence between itself, memory, skills and records.
- Orders the owner repeated are missing from it: no agents (the only "agent" in it is the folder `render/agents`), one engine
  process at a time while working, the reference hardware with its budgets and the quality of a game sold on Steam, the image
  protocol, never reading other sessions' transcripts, repository files written as bytes with LF.
- The order repeated most, the block rule (`[c:no-trial]`), and `[c:investigate-reading]` have only "conduct" guards in
  `tools/gates/contracts.json` (eight rules in all): nothing refuses a build, a test or a run inside an open block.
- Its block close asks for "the whole `run_gates.py`", which fails today on 170 standing problems: as written, no block can close.
- Facts that are wrong: `tools/converter-slice/` does not exist on disk; "What a clone needs" omits mimalloc and asks for Node only
  the obsolete tools use; the guard of `[c:generic-tracer]` is the stopped old tracer's test
  (`tools/analysis/featuretrace/tests/test_generic.py`) while the current tracer's is `tools/tracer/tests/test_generic.py`;
  `deliver.sh` is not named though `build.sh export` runs it.

### D. Documents and problem trackers

- 667 tracked text documents. `log/` alone: 45 files, 1.6 MB, of which 22 are the old feature tracer's plan and progress pairs.
- Known problems are kept in at least seven places that do not agree: `log/pendencias.md`; five open trace ledgers under
  `dist/trace`, which is not versioned (`port-from-nov7` 271 open, `performance-root-causes` 67, `vehicles-in-one-another` 13,
  `pole-no-wires` 9, `slow-checks` closed in content but not in state); `log/performance.md` (P-01 to P-20); `log/review.md`;
  `log/specification.md` (D1 to D7, S1 to S7); `log/live-instance-design.md`; `log/guards-progress.md`.

### E. Disk: about 106 GB for one game whose sources are a few megabytes

| Where | Size | What fills it | Why it grew |
|---|---|---|---|
| `dist/` (187 entries) | 45 GB | the architecture analyser's work, jobs and caches 17 GB; the task orchestrator's cache 11 GB; 36 exported builds, one per commit, 6.8 GB; trace photos and crops 6.5 GB (`port-from-nov7` 4 GB); analysis 0.9 GB; measures, PGO, held runs, captures, a promo video, twenty-odd `review-*` folders, an old Android build, three copies of the exported game | no output has a bound, an owner or an end of life: every run adds, nothing ever evicts |
| `.claude/worktrees` | 33 GB | 43 worktrees of agents and of two port lines, about 1 GB each | an agent's worktree is never removed after its work is merged or abandoned |
| Sibling folders on the Desktop | 22 GB | `city-archive` 7.9 GB (a plain copy, not a worktree), `city-bisect` 4.9 GB (six worktrees), `city-verify` 4.6 GB, `city-realism` 1.6 GB, `city-scenic`, `city-head`, `city-nov07`, `city-preport`, `city-realism-before`, `city-realism-pre` | each experiment, bisect or "before" was made as a full copy, never removed |
| `native/` | 3 GB | `build-android` 1.5 GB (Android is no target), `build-win` 1.3 GB, `build-win-debug` 0.2 GB | build folders for every platform ever tried |
| `.git` | 1.7 GB | history (1.1 GB packed, 0.5 GB loose) | the DLLs are versioned and rebuilt with every C++ change |
| `tools/` | 1 GB | the converter 736 MB (own git), the old game 101 MB, the old process 88 MB | obsolete trees kept |
| Elsewhere | about 1 GB | a Codex worktree (`.codex/worktrees/performance-causes`), three worktrees in an old session's temp scratchpad | the same |

Junk in the tree: `tools/analysis/-`, `-E`, `-v`, `-x`, `c++` (folders made by mistyped commands), `.claude/state` (62 files),
`.claude/.pipeline_state_*` (4), `dist/trace/*.txt` dumps of 77 MB each.

### F. Worktrees, branches and stashes

- **59 worktrees besides `main`.** 44 point at a commit already in `main`'s history (12 of them with uncommitted changes: the
  Codex worktree 434 files, a temp `before5f` copy 3,427 files, the six bisect copies 4 each, `city-head` 5, a few agents' 2 to 46).
  15 hold commits that `main` lacks.
- **33 branches with commits `main` lacks; about 90 branches fully merged.** The 33: the converter's and the old process's own
  histories (40 to 100 commits each), the port lines of 2026-11-04 (`portes/*`), `fix/root-causes` and `copia-b6785d7` (an earlier
  root-cause line, 2026-11-05), `realism` and `realism-port` (physical light, sky and night lighting, 11 and 14 commits),
  `wip/allocator-2026-11-10` (allocator hooks and a memory census, this game's own), and fifteen agent branches of 2026-11-08 and
  2026-11-09 (terrain beside roads, umbrellas and backpacks, coast stones, bus boarding with riders counted, rounded interior
  furniture, corner lots, lane arrows, styles of street furniture, vehicle indicators and exhaust, zoning fit to lots, demolition).
- **All of these were made before `main` was taken back to the tree of 2026-11-07 on 2026-11-10**, many on top of the ported code
  that was taken out; several state in their own messages that they bring the lab's behaviour or code ("the lab's junction solver",
  "the lab's lane change", "the lab's 3.5 m lanes"). None can be merged as it stands: each must be read whole against the copy rule,
  redone on today's `main` where it is this game's own design, built, tested and photographed.
- **Seven stashes**, three of them holding game fixes never committed: "9 of 22 fixed" of the overlapping vehicles; P476/P485/P486/
  P487 (a building's cap, lots' edges first, walkers in at the window's edge, save version 9); the allocator census.

### G. The trace-fix skill: what helps and what hurts

Helps:
- It makes the reading whole and recorded, and refuses an edit of a file not read — the discipline the owner asked for.
- Its fourteen world rules (`rules.json`: no body in another, nothing floats, surfaces meet, motion continuous, nothing appears in
  sight, nothing stands with its way free, each mover keeps its place, surfaces drawn, nothing flickers, no visible change of
  detail, light makes sense, the interface says what the game holds, one owner per state, a frame's work follows what changed) are
  the right list of what "broken" means in this game.
- `writers <state>` answers "who writes this", one of the owner's questions.

Hurts:
- **Two of its commands are broken**: `trace` and `card` call `tools/analysis/ftrace`, which no longer exists (the tracer was renamed
  `featuretrace`, then replaced by `tools/tracer`); they refuse with "this repository has no feature tracer".
- It is a second problem register: its ledgers sit under `dist/trace`, unversioned, with 360 open problems and 6.5 GB of photos.
- One trace opened for "everything" (`port-from-nov7`, 495 problems) never closes, so the photo guard that runs only at `close` never
  ran on some sixty delivered commits (S4).
- Its read rule makes every edit after a compaction cost the re-reading of whole files, records included (S1, S6).
- Its own text contradicts the owner's block rule (a test and photos per problem; test-first), so sessions follow one or the other.
- Its hooks are installed in the user-level settings: they run in every project on this machine, and in every worktree.
- The world rules are checked by a person looking at photos, after the fact, instead of by the game while it runs.

Judgement: the skill's discipline and its rules are right; its place is wrong. The rules belong in the game as run-time checks
(FR-024), the problems in the one register (FR-013), the "who writes / who calls" questions in the one traceability tool (FR-018),
and the skill keeps only the procedure of tracing one problem, pointing at those three, scoped to this project.

### H. Checks as they stand (run 2026-11-12, whole output read)

- **Gates**: 170 problems in 3,211 files, in 3 s (ESTADO_NA_UI 69, TEXTO_VIA_TR 21, NUMERO_MAGICO 16, SEGUNDO_DONO 14,
  TAMANHO_ARQUIVO 13, DOC_UNICO 10, VALOR_DUPLICADO 10, DONO_UNICO 9, TAMANHO_FUNCAO 4, RAIZ_LIMPA 2, NON_ENGLISH_TEXT 1,
  UI_SEM_ESTILO_SOLTO 1). DOC_UNICO and RAIZ_LIMPA now refuse Spec Kit's own files (`.specify/`, `specs/`): the project's rule of
  where documents live and Spec Kit's layout contradict each other.
- **Static audit**: 1,013 causes in 10 s: 940 texts nothing shows, 40 data keys nothing reads, 3 fields nothing reads, 14 commands
  only tests send, 8 commands nothing sends, 4 questions nobody asks, 4 functions bound and never called.
- **Copy scan**: 1,820 findings in 1,055 files, in 9 s (names shared with the reference and structural passages).
- These three already answer in seconds; what is missing is one place that joins them per feature and per owner, and the run-time
  half (what breaks while the game runs).

### I. Tests and processes

- Tests live in at least nine folders (`project/tests/{core,e2e,perf,visual}`, the gates', both tracers', the orchestrator's, the
  audit's, the converter's, the old game's).
- 57 test scenes, 6 performance probes and about 50 capture scripts: every one boots the engine and generates a whole town (S5, S7).
  The simulation core is 50,000 lines with no engine dependency, yet no rule of traffic, walkers or roads can be checked without
  booting the engine.
- 22 probes are compiled into the shipped library's interface layer.
- Whole-town results do not reproduce: traffic and walkers exist only round the camera, so the same city gives different states
  for different camera paths (S1, S3, L3).
- The block-close check took 471 s for 62 tasks before the `slow-checks` fixes; at most one engine process may run at a time.

### J. The game's problems (registered and open; each row is in `log/pendencias.md` or a trace ledger)

- **Simulation**: traffic and walkers exist only round the camera, with no trips from an origin to a destination (S1); the damage
  clock depends on lock timing (S2); ticks dropped with no count (S3); economy, growth, weather and ecology are not simulations
  (S4); every agent slot walked each tick, lists searched linearly (S6); transit riders not counted, nobody walks to a stop (S7);
  signal and stop rules (S8 to S10).
- **Bodies in one another**: vehicles (13 open problems), walkers (P489), furniture against line poles (V2, V3), hedges in walls
  (V4), tree crowns through eaves, fountains and walkers (V11, V21, V29); no single owner decides where things stand (V2).
- **Visual defects**: V1 to V36 (a pole with no wires, tone seams, stop lines on zebras, gates with no leaf, lamps that light
  nothing, ...).
- **Performance** against the reference budgets: the 24 km city has 151 frames over 33 ms on a camera path, a main-thread frame of
  157 ms, GPU p95 14.9 ms (budget 13), 73.6 s to generate (budget 15 s); the `low` tier misses its 27 ms GPU budget at every view on
  the floor profile (35 to 65 ms); 67 open problems in `performance-root-causes`, among them work redone for unrelated changes (far
  meshes rebuilt per building, whole-texture uploads per edit, rebuilds while the city grows).
- **Code from the reference**: 11 machine-translated files and a JavaScript runtime still stand (R1); 266 files cite a reference
  source; the copy scan's findings (R2 to R11).
- **Architecture**: the gates' 170 problems (A1 to A3); data, texts and commands nothing uses (A7 to A10); engine errors in runs
  that pass (A6, A12).
- **Missing tools**: no live instance, no real-time monitor, no real-player input, no measurement against the profiles, no probes
  on the release build (T1 to T5); photo analysis not enforced where work closes (T6).

### K. Readiness to sell on Steam (read in the project's settings and sources)

- No version number is set in the project; saves carry their own version (9) but the game does not.
- No check of the processor's instruction set at start: the library needs AVX2, FMA and BMI, and a machine without them crashes with
  no message (M2).
- No crash report and no log the player can send.
- No store integration (overlay, achievements, cloud saves) and no decision recorded about it.
- The export preset writes to a stale folder (C above); the release library carries test probes (I above).
- The texts of the reference's whole shell are shipped (940 texts nothing shows), in the player's language files.

### L. Why development is slow (causes, not symptoms)

1. Every session re-reads a method spread over a dozen sources and 1.6 MB of records, and resolves their contradictions by chance.
2. The owner's tools answer "is this file well formed" but not "who owns this, what does it change, what breaks": each question is
   answered by reading code by hand, item by item.
3. Every check of a simulation rule boots the engine and generates a town, one at a time; results do not reproduce.
4. Problems are tracked in seven places, two of them unversioned; the same defect is found, traced and recorded more than once.
5. Tool building displaced game work: since 2026-11-10, 73 of 213 commits went to one tracer, and 1,427 file changes landed in
   `tools/` against 1,293 in the game; three tracers now exist and the skill calls a fourth name that does not.
6. The read guard's whole-file rule and a trace that never closes make every edit after a compaction expensive.
7. Parallel lines (agents, sessions, port lines) were started without one integrator: their work sits in 15 worktrees and 33
   branches, unmerged and now mostly unmergeable.

### Judgement: the causes behind every finding

| Cause | Findings it produces |
|---|---|
| **J1. Nothing the work produces has an owner or an end of life.** Records, trackers, tools, caches, builds, photos, worktrees, copies and branches are made by each line and retired by none. | A, D, E, F, the junk folders, L4, L5, L7 |
| **J2. Orders are kept where they are neither authoritative nor enforced** (memory, records, the skill, nested files), and CLAUDE.md delegates instead of holding them. | A, B, C, G (text), L1 |
| **J3. The game does not declare its own architecture in a form a tool can read**, so "who owns, who writes, what a change triggers" is reconstructed by tracing syntax or by hand, and three tracers were built to approximate what the game could state. | G (broken commands), H (no join), L2, L5 |
| **J4. Checks sit at the wrong level and on a non-deterministic simulation**: every rule is checked through the engine and a whole town, and the simulation's state depends on the camera. | I, J (S1, S3), L3 |
| **J5. "Broken" is judged by people after the fact** (photos, ledgers) instead of by the game while it runs. | G (rules as a photo checklist), J (defects that pass every test) |
| **J6. The product's own needs were never written as requirements** (version, processor check, crash report, store, release build without probes). | K |

The right solution removes each cause, not its findings one by one: an owner and a lifecycle for every artifact (J1); CLAUDE.md as
the one source, with mechanisms (J2); the game declaring its commands, state, owners, controllers and change consumers so the tools
read them instead of guessing (J3); engine-free deterministic checks of the simulation (J4); invariants checked by the game as it
runs (J5); the product's requirements written and checked (J6). Each requirement below names the cause it removes.

## User Scenarios & Testing *(mandatory)*

The actors are the **owner** (the game's developer, who gives the orders) and the **coding agent** (Claude Code working in this
repository). The **player** benefits through the game's quality and performance.

### User Story 1 - One place for the method and every order (Priority: P1)

The owner gives an order once. It is written in CLAUDE.md, with a mechanism that makes it hold, and from then on every session
follows it without the owner repeating it. No other file — a memory note, a record, a nested instruction file, a skill, a goal
record or the Spec Kit constitution — carries an order of its own or claims a precedence above CLAUDE.md.

**Why this priority**: every other problem is made worse by sessions that follow the wrong one of several contradictory
instructions; the owner has repeated the same orders many times. (J2)

**Independent Test**: Start a fresh session in the repository and ask it where the work method is and what wins in a conflict;
then run the contradiction check: it finds one instruction source and zero conflicting or superseded instructions anywhere.

**Acceptance Scenarios**:

1. **Given** the consolidated tree, **When** any file outside CLAUDE.md is searched for orders or precedence claims, **Then** none
   is found, and the check that searches fails if one is added.
2. **Given** a new order from the owner in a conversation, **When** the session records it, **Then** it enters CLAUDE.md with its
   tag and its guard in the same change, and nothing is written to memory.
3. **Given** CLAUDE.md, **When** it is measured against the official guidance, **Then** it is under 200 lines, holds instructions
   (not status or file inventories), points to one current document per subject, and states that it is the only source of
   instructions.
4. **Given** a session that reads a file under any subfolder, **When** nested instruction files would load, **Then** none exists
   that contradicts CLAUDE.md.

---

### User Story 2 - A project folder that holds only what is used (Priority: P1)

The owner opens the project folder and finds the game's sources, its tools, its records and a bounded set of outputs; nothing
abandoned, no copy of the project per experiment, no worktree of finished work. Work worth keeping that was never merged is either
brought into `main` after verification or recorded as a pending feature; nothing is lost.

**Why this priority**: about 106 GB for a game whose sources are megabytes slows every search, backup and session, and hides which
copy is current. (J1)

**Independent Test**: Measure the project folder and its siblings; list worktrees, branches and stashes: the size is under the
target, every remaining worktree and branch has an owner and a reason, and every removed piece is reachable by a named reference.

**Acceptance Scenarios**:

1. **Given** the worktrees, branches and stashes listed in the audit (F), **When** the triage runs, **Then** each one is classed
   (already in `main`; worth bringing in; behaviour to redo; worthless), and the class is recorded with its reason before anything
   is removed.
2. **Given** a branch classed "worth bringing in", **When** it is verified, **Then** it is read whole against the copy rule, redone on
   today's `main` where needed, built, tested and photographed, and merged only if it passes; otherwise its behaviour enters the
   register as a pending feature.
3. **Given** uncommitted changes in a worktree or a stash, **When** the worktree or stash is to be removed, **Then** the changes are
   first kept as a tagged commit (or judged empty of value and said so), never discarded silently.
4. **Given** any output folder (caches, builds, photos, analyses, measures), **When** it passes its bound, **Then** its oldest or
   unreferenced entries are evicted by its owner, and evidence still referenced by an open problem is kept.

---

### User Story 3 - One register of every known problem (Priority: P1)

The owner and the agent see every known problem of the game and of the project in one versioned register: symptom, cause (or
"unknown"), owner, area, status, the criterion that closes it, evidence and where it came from. Nothing is tracked anywhere else.

**Why this priority**: today the same defect is traced several times, 360 open problems sit in unversioned ledgers, and nobody can
say what is open. (J1)

**Independent Test**: Count the open problems in every current tracker before migration; after it, the register holds each one
exactly once (duplicates merged by cause), and every other tracker is gone.

**Acceptance Scenarios**:

1. **Given** the trackers listed in the audit (D), **When** the migration runs, **Then** every open item of each appears in the
   register with its origin noted, and an item that cannot be migrated is listed by name, never dropped silently.
2. **Given** the stashes and branches with game fixes (F), **When** they are triaged, **Then** each fix is either verified and
   merged and registered as fixed, or registered as open with the stash or branch as evidence.
3. **Given** a problem found by a run (User Story 6), **When** it is reported, **Then** it enters the register with its evidence,
   deduplicated against the problems already there.

---

### User Story 4 - Trace any feature end to end in seconds (Priority: P1)

The owner names a feature (a road tool, traffic at a junction, a walker crossing, terrain painting, a building's growth), an element
of the city, or a piece of state, and gets at once its whole chain: where the player triggers it, the command, the state it changes
and its single owner, the state controller and its transitions, the systems that step it, the change it announces, everything that
change rebuilds or creates, what is drawn, the tests and photos that cover it, its measured cost against its budget, and its open
problems. Each link is marked as conforming or not: owner single or not, state controller present or not, source of truth followed
or not, optimised (measured inside budget) or not, or unknown.

**Why this priority**: it replaces hunting item by item in the code — the owner's main complaint about speed and precision. (J3)

**Independent Test**: Ask for three features from different areas (a road tool, traffic, walkers) and get each chain, with its
conformity marks, in under 5 seconds each, without reading code by hand; check one chain against the code by reading it.

**Acceptance Scenarios**:

1. **Given** any player-facing feature on the features list, **When** its trace is asked, **Then** the chain is returned from input
   to drawing with every link located (file and line) and every conformity mark set or "unknown".
2. **Given** a piece of state, **When** its writers are asked, **Then** every place that writes it is listed, and more than one
   owner, a writer outside the allowed writers, or a value kept in the interface is flagged.
3. **Given** a kind of change (a road edit, a building grown, the camera moving), **When** its consequences are asked, **Then** every
   update and every creation it triggers is listed, and those whose area the change does not touch are flagged as unneeded.
4. **Given** a link no declaration or reading resolves, **When** the trace is shown, **Then** the gap is shown as a gap, never
   omitted, and the code-reading part stays generic (no rule written for one program, library or name).

---

### User Story 5 - Checks that take seconds, stream what they do, and never block each other (Priority: P2)

At the end of a block the agent verifies what the block reaches in seconds for the simulation's rules and in minutes for the whole
game; every process shows its progress as it runs, has a deadline, can be followed and stopped by itself, and a slow or hung task
never holds the others.

**Why this priority**: the block-close check and the measurements are the slowest step of every theme, and whole-town tests that do
not reproduce waste the time they take. (J4)

**Independent Test**: Change one simulation rule and run its check: it finishes in seconds without starting the engine; run the
block-close check: every task streams progress, finishes inside its deadline, and a task forced to hang is stopped while the others
finish.

**Acceptance Scenarios**:

1. **Given** a change to a simulation rule, **When** its check runs, **Then** it completes in under 10 seconds without booting the
   engine or generating a town.
2. **Given** the same scenario run twice with the same inputs (seed, commands, camera path), **When** the state is compared,
   **Then** it is identical.
3. **Given** any check or measurement, **When** it runs, **Then** a line of progress appears at least every 5 seconds, and its result
   says what ran, what passed, what failed and what was skipped.
4. **Given** a task that hangs, **When** its deadline passes, **Then** that task alone is stopped (by its own process id), recorded as
   failed, and the rest of the run completes.

---

### User Story 6 - Whatever breaks is reported by the game itself, grouped by owner (Priority: P2)

Every development run of the game — a test, a capture, a measurement, the live instance — checks the world rules while it runs (no
two bodies in one another, nothing floating, every strung span drawn, nothing appearing in sight, every budget kept, no work redone
for nothing) and ends with one report of everything broken, grouped by owner, entered into the register.

**Why this priority**: the owner finds defects in photos that the checks passed (vehicles inside one another "pass every test"; a
pole with no wires in a fix's own proof photo). (J5)

**Independent Test**: Run a scenario with a known overlap and a known missing element: the report names both, by owner, with where
and when, without anyone opening a photo.

**Acceptance Scenarios**:

1. **Given** a run in which two vehicles overlap, **When** it ends, **Then** the report lists the overlap with both bodies, the
   place, the time and the owner of the rule that let it happen.
2. **Given** a frame or a tick over its budget on the reference profile, **When** it happens, **Then** it is counted and reported with
   what was running.
3. **Given** the release build sold to players, **When** it runs, **Then** the checks cost nothing measurable (absent or off), and the
   shipped library carries no test probes.

---

### User Story 7 - One predictable layout for tests, tools and the skill (Priority: P2)

The agent and the owner find any test or tool in one place by its kind; each question has one tool; the trace-fix skill holds only
its procedure and uses the register, the traceability tool and the run-time checks; obsolete trees no longer sit in the working tree.

**Why this priority**: the scatter costs reading time in every session and hides which tool is current; the skill's broken commands
and second register mislead every session that follows it. (J1, J3)

**Independent Test**: List the tree: tests in one folder per kind, tools in one folder per question, no obsolete tree, the skill's
every command working against the current tools; every removed file reachable in the history.

**Acceptance Scenarios**:

1. **Given** the consolidated tree, **When** a new test is written, **Then** its place is decided by one written rule.
2. **Given** two tools that answer the same question, **When** the layout is reviewed, **Then** one remains and the other is
   retired, its useful checks kept as parts of the one.
3. **Given** the trace-fix skill, **When** each of its commands runs, **Then** it works against the current tools, records problems
   in the one register, and its hooks act only in this project.

---

### User Story 8 - The game's and the product's constraints hold by mechanism (Priority: P3)

Every change to the game is designed for this game's architecture, uses the reference game only to know how a feature must behave,
meets the performance budgets of the reference hardware, and fixes causes, never effects; the product carries what a game sold on
Steam needs. Each of these is checked by a mechanism that fails, not by a sentence.

**Why this priority**: these constraints exist today but are broken repeatedly; the product's needs were never written. (J6)

**Independent Test**: Submit a change that copies the reference's structure, one that moves a threshold to hide a symptom, and one
that pushes a frame over budget: each is refused by a check, with the reason. Build the release: it has a version, refuses an
unsupported processor with a message, writes a crash report, and carries no test probe.

**Acceptance Scenarios**:

1. **Given** a change with code whose structure matches the reference, **When** it is checked, **Then** it fails with the matching
   passage.
2. **Given** a change that only moves a threshold, timeout, tolerance or count, **When** it is checked, **Then** it fails unless the
   cause is written with it.
3. **Given** a change that raises a measured cost over its budget on either reference profile, **When** the block closes, **Then** the
   close fails with the measure.
4. **Given** the release build on a processor without the required instructions, **When** it starts, **Then** it shows a message in
   the player's language and exits cleanly.

---

### Edge Cases

- An order in a conversation contradicts CLAUDE.md: the contradiction is shown to the owner, who decides; the decision replaces the
  old text in CLAUDE.md in the same change.
- A record holds both history and an order: the order moves to CLAUDE.md, the history stays in the subject's one current document.
- The owner's earlier order "never delete records" meets "remove what is useless": content with value is merged, never lost; a file
  leaves the working tree only when its content lives elsewhere or in the history under a named reference.
- A worktree has uncommitted changes nobody remembers: they are kept as a tagged commit and listed before the worktree is removed.
- A branch made on the ported tree touches code that `main` no longer has: it is not forced in; its behaviour is written down and
  redone on today's `main` as this game's own design.
- A branch's work came from the reference: it is not merged; its behaviour, if wanted, enters the register as a feature to design.
- The owner's game window is open, or a Godot process of another session runs from a worktree: no removal or check closes it; a
  worktree in use is left and reported.
- Trace photos referenced by an open problem: kept as its evidence until the problem closes, whatever the bound of the photo folder.
- A problem in the register has no known cause: it stays with "unknown" and the reading that would find it; it is never closed by a
  guess.
- The tracer meets a language construct it cannot read: the gap is reported with the place; the fix is to the general mechanism.
- The 24 km city (about 470,000 buildings): tracing and run-time checks must not push a development run out of its budgets by more
  than a stated overhead, and cost nothing in the release build.
- A hung task, an engine that waits forever, a script that does not parse: each ends at its deadline with its reason.
- The trace-fix skill and its hooks change only in a staged copy, tested whole, deployed on the owner's word (owner, 2026-11-12).
- The Spec Kit constitution and specification folders must not become another home of the method, and the gates must not refuse
  Spec Kit's own layout.

## Requirements *(mandatory)*

### Functional Requirements

**Instructions and precedence (J2)**

- **FR-001**: CLAUDE.md MUST be the only source of the work method and of every order of the owner: method, principles, constraints,
  architecture rules and the commands needed to work. It MUST state that it is the only source of instructions and that memory,
  records, skills, goal files and the Spec Kit constitution hold no orders.
- **FR-002**: CLAUDE.md MUST follow the official guidance: under 200 lines, specific and checkable instructions, no dated status, no
  file-by-file inventory; reference material moves to the subject's one current document, which CLAUDE.md names.
- **FR-003**: Every order now living only in memory, `log/how-we-work.md`, `log/specification.md`'s constraints, the skill's text or
  a nested instruction file MUST be moved into CLAUDE.md in the owner's sense, not narrowed, with its guard; the source then stops
  holding it.
- **FR-004**: The divergences D1 to D5 between CLAUDE.md and the owner's later orders MUST be resolved in CLAUDE.md.
- **FR-005**: Nested instruction files that can load in a session (`tools/process/CLAUDE.md`, `tools/oracle-4.0/CLAUDE.md`, the old
  copy under `tools/ports/.../tree/`) MUST leave the working tree; gate test fixtures named CLAUDE.md are kept only where they cannot
  load as instructions.
- **FR-006**: A check MUST fail when any file other than CLAUDE.md carries an order or a precedence claim, or when two instructions
  contradict each other; it is part of the gates. Since only CLAUDE.md may hold instructions, a contradiction is checked there as:
  a tag defined twice, two rules under one tag, a tag with no guard in `tools/gates/contracts.json`, or a guard naming no tag
  (CONTRACT_UNGUARDED, `tools/gates/gate_contracts.py`, owns it).
- **FR-007**: Auto memory MUST be turned off for this project; its 33 notes are migrated (orders to CLAUDE.md, facts to the subject
  documents, history to the register or nowhere) and the memory folder emptied of them.
- **FR-051**: The block rule MUST be kept by a mechanism, not by conduct: while a block is open, every build, test, game run,
  measurement, photo, export and commit of the block is refused by the tools they go through (the orchestrator, `build.sh`,
  `deliver.sh`, a git pre-commit hook) and by a project hook for a direct engine run; the block's close opens them, once.
  Preservation of work about to be removed (archive tags and commits) is not a commit of the block.
- **FR-052**: A block's close MUST run the whole gates and fail on any finding that is not an open problem of the register; the
  gates themselves are not loosened, baselined or silenced: the standing findings keep them failing, visibly, until the area
  specification that owns each fixes it.

**Documents (J1, J2)**

- **FR-008**: Each subject MUST have exactly one current document: the project map, the problem register, the goals and budgets
  (specification), the research with its proofs, and one history per area of work. Duplicated subjects (the 22 tracer plan and
  progress files, the guards' plan and progress, per-line records that repeat each other) MUST be merged into their subject's
  document.
- **FR-009**: Every document in the repository MUST be in English and named in English (the player's texts and outside copies
  excepted), per CLAUDE.md.
- **FR-010**: Documents with no current use (the old process's records and guides, the port records, the JavaScript-era `docs/`, the
  earlier goal records, leftover agent state) MUST leave the working tree after the notes in them that describe how a feature must
  behave are moved to the register or to the area's document.
- **FR-011**: Nothing of value MUST be lost: every file, worktree change, stash and branch that leaves MUST stay reachable through a
  named reference in the repository's history, listed in the project map.
- **FR-012**: Obsolete trees (the converter, the old process, the instrumented old game, the port records, the old feature tracer
  and its stash) MUST leave the working tree, kept in the history under a named tag.
- **FR-013**: Spec Kit's layout (`.specify/`, `specs/`) MUST be one of the project's declared document places: the gates accept it,
  CLAUDE.md names it, and the constitution holds no rule of its own (it points to CLAUDE.md).

**Disk, worktrees, branches and stashes (J1)**

- **FR-014**: Every worktree, branch and stash MUST be triaged and its class recorded with the reason: already in `main`; worth
  bringing in (this game's own design, still applicable); behaviour to redo on today's `main`; worthless.
- **FR-015**: A piece classed "worth bringing in" MUST be read whole against the copy rule, brought onto today's `main` (redone where
  its base is gone), built, tested and photographed at the block's close, and merged only if it passes; otherwise its behaviour
  enters the register.
- **FR-016**: Worktrees and copies classed "already in main" or "worthless" MUST be removed after their uncommitted changes are kept
  as tagged commits or judged empty of value; branches fully merged MUST be deleted; the sibling copies on the Desktop and in other
  tools' folders included.
- **FR-017**: Every output folder (task cache, analyser work, exported builds, photos, measures, captures, analyses) MUST have one
  owner and a bound in size or count, enforced by that owner on every write; evidence referenced by an open problem is exempt until
  the problem closes.
- **FR-018**: Exported builds MUST keep only the latest delivered build and the one before it, plus any build a problem references.
- **FR-019**: Build folders for platforms that are not targets MUST leave the tree; junk folders made by mistyped commands MUST be
  removed and the commands that made them fixed.
- **FR-020**: A worktree MUST be created only with an owner and an end: it is removed by whoever made it when its work is merged or
  dropped, and a check lists any worktree older than its work.

**Problem register (J1)**

- **FR-021**: One versioned register MUST hold every known problem of the game and of the project: id, symptom, cause or "unknown",
  owner, area, status, closing criterion, evidence, origin.
- **FR-022**: Every open item of the current trackers (the structured and carried rows of `log/pendencias.md`, the five trace
  ledgers, `log/performance.md`, `log/review.md`, `log/specification.md`'s D and S rows, the live-instance design's and guards' open
  items, this audit's findings) MUST be migrated into the register, duplicates merged by cause; afterwards no other tracker exists.
- **FR-023**: The register MUST be queryable by area, owner, status and cause in under 5 seconds.

**Feature traceability (J3)**

- **FR-024**: The game MUST declare its own architecture in a form a tool reads: every command with the state it writes; every piece
  of state with its one owner and, if stateful, its controller and transition table; every kind of change with the consumers that
  rebuild or create on it. A declaration that disagrees with the code fails a check.
- **FR-025**: A features list MUST be derived from the game's entry points (the player's tools, the commands the interface sends,
  the simulation's systems), never kept by hand only; every feature has an id.
- **FR-026**: For any feature, element or piece of state, the traceability tool MUST return the end-to-end chain: player input,
  command, state and its owner, state controller and transitions, systems that step it, announced changes, what each change rebuilds
  or creates, what is drawn, tests and photos covering it, measured cost against budget, open problems.
- **FR-027**: Each link MUST carry conformity marks: single owner or not; state controller or not; state in the single source of
  truth or not (state kept in the interface or a copy flagged); measured inside budget, over it, or unmeasured; covered by a test or
  not.
- **FR-028**: For each kind of change, the tool MUST list every update and creation it triggers and flag those outside the area the
  change touches (updates and creations without need).
- **FR-029**: The code-reading part of the tool MUST be generic: it reads the languages' rules and official declarations, holds no
  rule written for one program, library, API or name, and reports a gap where it cannot resolve a link.
- **FR-030**: A query MUST answer in under 5 seconds on the whole tree; after a change only the changed files are read again.
- **FR-031**: The tool MUST join what the existing fast checks report (gates, static audit, copy scan) per feature and per owner, and
  replace the overlapping analysers: each question is answered by one tool; the retired tools' still-useful checks become its parts.

**Run-time detection (J5)**

- **FR-032**: Every development run (test, capture, measurement, live instance) MUST check the world rules while it runs: no body in
  another (vehicles, walkers, furniture, plants, buildings), nothing floating or sunk, every strung or placed element drawn, nothing
  made or removed in sight, ticks and frames inside budget, no update whose result is unchanged or unread.
- **FR-033**: Every run MUST end with one report of everything broken, grouped by owner, with place, time and evidence, entered into
  the register without duplicates.
- **FR-034**: The release build MUST carry no test probes and pay no measurable cost for the checks.

**Checks and processes (J4)**

- **FR-035**: The simulation's rules MUST be checkable without booting the engine or generating a town, in seconds.
- **FR-036**: The simulation MUST give the same state from the same inputs, so a scenario reproduces: its random draws come from
  the scenario's seed, and no rule depends on lock timing or on the wall clock (S2). The camera path is one of those inputs until the
  simulation's own specification removes the camera dependence (S1); that removal is registered, not done here.
- **FR-037**: Every check, test, measurement and capture MUST stream its progress at least every 5 seconds, have a deadline sized to
  its work, be stoppable by its own process id, and never hold another task.
- **FR-038**: Tests MUST live in one folder per kind (engine-free rule tests, engine scenes, performance probes, visual captures,
  tool tests), placed by one written rule.
- **FR-039**: The block-close verification MUST run as one command that reports what ran, passed, failed and was skipped, and
  finishes the whole game's check in under 10 minutes.
- **FR-040**: Performance MUST be measured against the two reference profiles and their budgets (`log/specification.md`) on the
  release build, and a measure over budget fails the close.

**The trace-fix skill (J1, J2, J3, J5)**

- **FR-041**: The skill MUST keep only the procedure of tracing and fixing one problem; its world rules move into the game's run-time
  checks (FR-032), its problems into the register (FR-021), its "who writes / who calls" into the traceability tool (FR-026).
- **FR-042**: Every command of the skill MUST work against the current tools; a command that calls a missing tool fails a check.
- **FR-043**: The skill's text MUST agree with CLAUDE.md (the block rule included); where it differs, CLAUDE.md's text is the one
  kept and the skill points to it.
- **FR-044**: The skill's hooks MUST act only in this project (the project's own settings, not the user's).
- **FR-045**: The skill and its hooks MUST change only through a staged copy, tested whole and deployed on the owner's word.

**Constraints on every change and the product (J6)**

- **FR-046**: A check MUST fail on code whose structure, names or functions are copied or translated from the reference game; the
  reference is read for behaviour only.
- **FR-047**: A check MUST fail on a change that only moves a threshold, timeout, tolerance or count, swallows an error or loosens a
  check, unless its cause is written with it.
- **FR-048**: Every change MUST be measured against the budgets of the reference profiles; performance, optimisation and visual
  quality are acceptance criteria of every feature, at the level of a game sold on Steam.
- **FR-049**: The product MUST carry a version, refuse an unsupported processor with a message in the player's language, write a
  crash report the player can send, and ship only texts and data the game uses; the decision on store integration (overlay,
  achievements, cloud saves) MUST be recorded as a requirement of its own.

**Process cost (all)**

- **FR-050**: The causes of slow development (L1 to L7) MUST each be removed at its cause, and the time of each step of the method
  (finding a cause, checking a change, closing a block) recorded so its trend is visible.

### Key Entities

- **Instruction source**: CLAUDE.md; the only holder of method and orders; each rule tagged and tied to its guard.
- **Subject document**: the one current document of a subject (map, register, specification, research, area history).
- **Problem**: an entry of the register — id, symptom, cause or unknown, owner, area, status, closing criterion, evidence, origin.
- **Artifact**: anything the work produces on disk (cache, build, photo, analysis, worktree, branch, stash), with its owner, bound and
  end of life.
- **Feature**: a player-facing capability with an id; its entry points, commands, state, systems, outputs, tests, budget share and
  problems.
- **Declaration**: what the game states about itself — a command and the state it writes, a state and its owner and controller, a
  change kind and its consumers.
- **Owner**: the single place that defines a value, a rule or a piece of state.
- **State controller**: the one member, setter and transition table of a stateful thing.
- **Change trigger**: a kind of change and the updates and creations it causes, each needed or not.
- **World rule**: a rule about the city checked while it runs, with its owner (the fourteen of the skill to start with).
- **Budget**: a limit of a metric (frame, GPU, main thread, tick, memory, load time) on a reference profile and tier.
- **Check**: a task with inputs, a deadline, a progress stream and a verdict.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Exactly one file holds the method and the owner's orders; it is under 200 lines; the contradiction check finds 0
  orders, precedence claims or conflicts in any other file; auto memory is off and holds 0 notes for this project.
- **SC-002**: Current documents outside Spec Kit's folders and third-party code fall from 667 to at most 25, one per subject, all in
  English.
- **SC-003**: The project folder and its sibling copies fall from about 106 GB to under 12 GB, and stay under it for 30 days of
  normal work.
- **SC-004**: 0 worktrees, branches or stashes without an owner and a recorded class; 0 fully merged branches kept; every removed
  piece reachable by a named reference.
- **SC-005**: 100% of the open items of the current trackers are in the one register; 0 other trackers remain.
- **SC-006**: A trace of any feature on the list returns its whole chain with conformity marks in under 5 seconds; 100% of the
  player-facing features are on the list; 100% of commands, state and change kinds are declared and checked against the code.
- **SC-007**: The owner's questions "who owns X", "what writes Y", "what does Z trigger", "what is not optimised", "what has no
  state controller" are each answered in under 1 minute without reading code by hand.
- **SC-008**: A check of a simulation rule takes under 10 seconds; a check of a change that reaches engine scenes under 2 minutes;
  the whole block-close verification under 10 minutes; every process shows progress at least every 5 seconds; 0 runs in which one
  task held another.
- **SC-009**: The same scenario run twice with the same inputs (seed, commands, camera path) gives the same state in 100% of runs,
  engine-free and through the engine alike.
- **SC-010**: Each run ends with one report by owner in under 10 seconds after it stops; the defect classes of the audit (bodies in
  one another, things missing or floating, budget breaches, redundant work) are detected in a run without a photo being opened.
- **SC-011**: Every command of the trace-fix skill works; its hooks run in this project only; its text has 0 contradictions with
  CLAUDE.md.
- **SC-012**: The release build has a version, refuses an unsupported processor with a message, writes a crash report, carries 0
  test probes and 0 texts nothing shows.
- **SC-013**: After this feature, the share of commits that go to tools instead of the game falls below one in five over any two
  weeks, and the median time from a reported problem to its located cause falls under 15 minutes.

## Assumptions

- Scope: this feature builds the foundation (one instruction source, a clean and bounded project, one register, the game's
  declarations and the traceability tool, run-time detection, fast checks, the layout, the skill's redesign, the product's
  requirements) and the complete audit and register of the game's problems. Fixing the game's registered problems (simulation,
  traffic, walkers, terrain, buildings, visual defects, performance, code from the reference) is done after it, one area per
  specification, each using this foundation and meeting the budgets.
- The owner's standing orders stay as they are and move into CLAUDE.md unchanged in sense: blocks coded with nothing run inside; one
  verification at the block's end; no agents unless ordered; the costliest fix at the cause; the reference read for behaviour only;
  the image protocol; one engine process at a time while working; the repository in English, the conversation in Portuguese.
- The second request ("worktrees worth something and not merged, merge, but verify") is read with the copy rule: work made on the
  ported tree or from the reference is not merged as code; its behaviour is kept to be redone.
- The reference hardware profiles and budgets of `log/specification.md` (median: 6 cores, 16 GB, RTX 3060 class, 1080p `medium` at
  60 fps; floor: 4 cores, 8 GB, integrated GPU, 1080p `low` at 30 fps) stand as the Steam-quality targets.
- The repository's history keeps every removed file; a named tag marks the state before removal.
- Removing worktrees, copies, branches, stashes and output folders is permanent on disk: the owner gave standing authorisation for
  every removal the tasks name (2026-11-12); the whole list is written in the process history before the first removal, what has
  value is kept under a tag first, and nothing in use (a running process, the owner's open game) is touched.
- The official Claude Code documentation consulted on 2026-11-12 is the reference for where instructions, memory, rules, skills and
  hooks belong.
- Work is direct on `main`, as CLAUDE.md orders; no feature branch is created for this specification.
