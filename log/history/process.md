# History: the process

One history of how the work is done (the method, the instruction source, the triage and cleanup of the project, the time each step
takes), newest last.

## Specification 001: progress (resume here after a compaction, with `specs/001-single-source-traceability/tasks.md`)

| When | Block | State | Commit |
|---|---|---|---|
| 2026-11-12 | speckit-analyze | 12 findings (no critical): coverage of FR-052's mechanism, FR-015's bringing in, FR-050's times, SC-002, SC-009 through the engine, FR-032's drawn elements and unread updates, FR-010's notes; FR-006 made checkable; the quickstart's rule names; the removal list written in this file (standing authorisation). Applied to spec, plan, research, data model, contracts, quickstart and tasks (T020a, T022a, T038a, T038b, T074a added) | done | (with the first block) |
| 2026-11-12 | Setup and Foundational (T001 to T011): the register | verified and committed | `8588f342` |
| 2026-11-12 | US1 (T012 to T023): CLAUDE.md the only source of instructions, memory off, the block rule and the gates' close by mechanism | verified and committed | see `git log --grep "the only source of instructions"` |

## US1, the instruction source (2026-11-12)

- **Problems** (spec.md A to C, G): the method lived in a dozen places, three of which claimed to win over the others; 33 memory
  notes held orders found nowhere else, some superseded, all loaded every session; CLAUDE.md delegated its binding method to
  `log/how-we-work.md`, held dated status and a file inventory at its 200-line ceiling, and had facts wrong (the executable's path,
  `converter-slice`, the tracer's guard); the block rule had conduct guards only; the whole gates could never pass at a close.
- **Causes**: orders were kept where they are neither authoritative nor enforced (J2); nothing checked where an order lives.
- **Sources**: Claude Code's memory, settings and hooks documentation through Context7 `/websites/code_claude` (CLAUDE.md vs auto
  memory; `autoMemoryEnabled`; `claudeMdExcludes` matched against absolute paths; hooks with `${CLAUDE_PROJECT_DIR}` and the exec
  form `args`; exit 2 of a PreToolUse hook blocks the call; `permissions.deny` of `Agent`).
- **Done**: every order read whole and placed (research R14); CLAUDE.md rewritten (191 lines, an Authority section, the method
  whole, D1 to D5 settled, every order of R14 tagged); its reference parts moved to `log/mapa.md`; `tools/gates/contracts.json`
  rewritten (a new guard kind, `setting`, and hooks of `.claude/hooks/`); `.claude/settings.json` (memory off, the fixtures'
  instruction files excluded, no agents, no transcript reads, the two shell hooks); the gates INSTRUCTION_ELSEWHERE,
  PRECEDENCE_CLAIM, NESTED_CLAUDE, MEMORY_ON (`tools/gates/gate_docs.py`) and LINE_ENDINGS (`tools/gates/gate_files.py`, with
  `tools/tasks/inputs.git_bytes`), FR-006's contradiction check left with its one owner CONTRACT_UNGUARDED; the block rule's
  mechanism (`tools/tasks/block.py`, `cli.py block`, `build.sh`, `deliver.sh`, `tools/build/git-hooks/pre-commit`,
  `.claude/hooks/block_guard.py`); the standing authorisation's limits (`.claude/hooks/destructive_guard.py`); FR-052's join
  (`run_gates.py --register`, `--json`; `atlas problems import-gates`, `stats`; `tools/atlas/areas.py`, `imports.py`); the
  gates' lexer reads a script by its #! line.
- **Removed**: `log/how-we-work.md` (folded above), `tools/process/CLAUDE.md`, `tools/oracle-4.0/CLAUDE.md`, the old CLAUDE.md copy
  under `tools/ports`, the 34 memory notes, `.claude/metas.md`, `.claude/metas/`, `.claude/REGISTRO-GOAL.md` (all kept under
  `archive/2026-11-12/claude-records`). The old goal record describes the branch `fix/root-causes` (far vehicle level, inspector
  publication, the simulation asleep in pause, the MultiMesh room rule, the wake counter): its triage is US2's.
- **Verified**: tools/tasks tests 79 (one pre-existing timing failure, the scheduler's backfilling test, failing 4 of 8 on a copy of
  HEAD too: registered Ptools-1); atlas tests 20 OK; gates' tests 31 OK; self-test 95 cases, 41 rules OK; the block mechanism
  shown refusing a test, a build, an export, an orchestrator run and a commit while open and letting them through at `verify`; the
  whole gates 162 findings, all registered (93 problems imported), 0 NEW; CLAUDE.md 191 lines.
- **Problems found on the way**: Ptools-1 (above), Proads-1 (roads meshed at a uniform step), Proads-2 (the verge band).
- **Not run**: the quickstart's "fresh session" question (no new session may be opened); the three PRECEDENCE_CLAIM findings left
  are in records US2 and US3 remove (registered).

## Block times

Appended by `python tools/tasks/cli.py block close` (FR-050: the time each step of the method takes, so its trend shows).

| Block | Opened | Verified | Closed | Coding (min) | Verifying (min) |
|---|---|---|---|---|---|
| US1-mechanism-check | 2026-11-12T15:47 | 2026-11-12T15:47 | 2026-11-12T15:47 | 0 | 0 |

## How the work was done before 2026-11-12 (history; the orders themselves live in CLAUDE.md)

### The tree taken back to 2026-11-07 (2026-11-10)

On 2026-11-10 the owner had `native/`, `project/`, `tools/`, `log/` and CLAUDE.md taken back to `a1a771c4` (2026-11-07 20:29), in
commit `dcfd4c7c` on main, no history rewritten. Main as it stood is the tag `antes-da-volta-2026-11-10`. Photos of the 24 km city
(map 4, seed 1) at five dates showed the city whole on 06/11 and 07/11 and broken after the lab ports begun 2026-11-08 21:30
(`b56db4a3`). That base cost about twice the video and static memory of the later tree (centre view 5,454 MiB video, 1,351 MiB
static). The day's uncommitted work (mimalloc hooks through `MI_WIN_USE_FLS`, a memory census by thread and by asker,
`memory_test`) is the branch `wip/allocator-2026-11-10`. The repository's first commit is "Base antes do porte" (`bb58790e`).

### The diagnosis of 2026-11-08

Why sessions were slow and contaminated then: enforcement by prompt only (no project settings or hooks); the source game readable
at `city-archive` and `Documents/Projects/city/roadcraft`; `tools/tasks/suites.json` keying all 37 scenes by the whole project, so
one changed script reran the whole suite; roads emitted at a uniform 3 m step on any curve; tools without an oracle on real data.
Since then the trace-fix skill's hooks and the scenes' declared inputs exist.

### The mandatory pause (2026-11-11, revoked by the owner on 2026-11-12)

The owner stopped the work on 2026-11-11 ("Pausa obrigatória: levantar, pesquisar, provar e corrigir"): three phases (survey,
research proven in isolated tests, fixes) before any new game code, with fourteen standing orders and the image protocol. Where it
stood: Phase 1 recorded in `log/specification.md`, `log/pendencias.md` (50 structured items) and `log/research.md` (Q1, Q2, Q4,
Q5, Q6, Q16 proven; Q3, Q7 to Q15, Q17 to Q20 open); km24 measured on the median profile (GPU p95 up to 14.9 ms, 151 frames over
33 ms, 73.6 s to generate on the debug DLL). The standing orders moved into CLAUDE.md (specification 001, research R14); the pause
itself was revoked by the owner's order of 2026-11-12.

### Contradictions found on 2026-11-11 and how they were settled (from `log/how-we-work.md`, removed)

| Where | What it said | Settled |
|---|---|---|
| CLAUDE.md, tools/converter | "Translated code (e.g. flow_control) is regenerated, never fixed by hand" | Never regenerated: translated code is a debt redesigned at its cause; the converter produces no game code |
| CLAUDE.md, core/ | "the runtime of the translated code" as a part of core | A debt to redo, in the register |
| CLAUDE.md, Execution | "every port ... measured before and after" | There are no ports: every feature and fix is measured, both builds at the end |
| CLAUDE.md, Light and clock | "at night the sun becomes the moon without shadow" | The moon casts all night (P347, 2026-11-11) |
| Memory `code-first-measure-at-end` | "no before measurement even though CLAUDE.md says before and after" | Both measured, at the end: previous build and this one |
| Memory `delegate-port-with-trace` | agents start from the converter's machine translation | Superseded: no translation of the roadcraft, no agents |
| Memory `coordinate-parallel-sessions` | subagents in worktrees are fine | No agents unless ordered |
| Memory `never-wake-stopped-agents` | an order for agents means a handful | No agents (2026-11-11); an explicit order is the only exception |
| Memory `judge-photos-by-quality` | if photos are sound, bless the goldens and go on | Frozen photos are compared with nothing; `--bless` only for the digest of a change meant to move it, its cause written |
| Memory `diagnosis-2026-11-08` | ".claude has no hooks" | History of that date: hooks exist now |
| Skills `roadcraft-port`, `roadcraft-reading` (account skills) | their names speak of porting | Not used: porting the roadcraft is forbidden |

### Contradictions found on 2026-11-12 and how they were settled (from `log/how-we-work.md`, removed)

The owner ordered the work done in the code, with nothing run until the end of a big block, and the session went on in test-first
cycles closed a theme at a time.

| Where | What it said | Settled |
|---|---|---|
| how-we-work step 6 | "Test first: the check that fails because of the cause is written before the code" | No test is written or run inside a block |
| how-we-work step 7 | "Its tests (one Godot at a time while working)" | Nothing is run while working; one verification at the block's end |
| how-we-work step 9 | a theme closed at a time (DLLs, quick --ready, gates, executable, commit, push) | The block is closed once |
| CLAUDE.md, Method 5 to 7 and the check table | a change verified by its kind as it is made | Coded as a block with nothing run; checked once at the block's end |
| CLAUDE.md, Principles | the executable and the commit at a theme's close | At the block's close |
| Memory `no-city-tests-mid-work` | "no tests in generated cities mid-work" | No tests at all inside a block (the owner's words) |
| Memories `commit-push-regularly`, `deliver-exe-after-each-fix` | commit, push and export after each fix | At the block's close |
| Skill `trace-fix` (one problem at a time, test it, photograph it; a failing check first) | a test and photos per problem | The block's, at its end (the skill's redesign, specification 001 US7) |
| Skill `superpowers:test-driven-development` | red and green for every change | Not used in this project |

On 2026-11-12 (specification 001) the method moved whole into CLAUDE.md, the precedence claims of memory and records went with
`log/how-we-work.md` and the memory notes, and the block rule got its mechanism (`tools/tasks/block.py`).

### How the promo video was recorded (2026-11-08)

`dist/promo/roadcraft_promo.mp4` (1080p30, 69 s) and a light copy. Recorded from a clean `git archive HEAD project` copy with
`.godot` copied in and `--import` run first; a director scene run from outside the repository with `--path <project>`, hidden
through `tools/tasks/proc.run(..., hidden=True)`; `--fixed-fps 30` (not Movie Maker), the map opened paused with
`catalog.tuning.sim.tick_hz = 30` and `CityStore.step_ticks(1)` each frame, about 75 ms of sleep after each step (the render
samples poses at the steady clock less `pose_delay_ticks`); frames grabbed on `RenderingServer.frame_post_draw`, written raw as
rgb24 and assembled by ffmpeg (xfade, drawtext); the hidden window is 1920x1061, cropped to 16:9 and scaled to 1080p; low shots in
the residential ring (r > 350 m on the town map).
