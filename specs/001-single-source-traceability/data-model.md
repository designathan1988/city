# Data Model: One Source of Truth, a Clean Project and End-to-End Feature Traceability

Phase 1 of `plan.md`. Entities of the spec, their fields, relationships, validation and state transitions.

## Problem (the register: `log/problems.jsonl`)

| Field | Type | Rule |
|---|---|---|
| `id` | string `P<area>-<n>` | unique, never reused |
| `title` | string | one line, English |
| `symptom` | string | what the player or the owner sees |
| `cause` | string or `"unknown"` | the design decision that produces it; `"unknown"` requires `reading` |
| `reading` | string | what must be read to find the cause (required while `cause` is unknown) |
| `owner` | string | the one owner (file and symbol) of the decision, or the tool |
| `area` | enum | `simulation`, `traffic`, `walkers`, `transit`, `roads`, `street`, `lots`, `buildings`, `terrain`, `nature`, `render`, `interface`, `performance`, `product`, `tools`, `docs`, `process` |
| `kind` | enum | `defect`, `missing-feature`, `performance`, `copy`, `architecture`, `process` |
| `status` | enum | see transitions |
| `criterion` | string | the check that closes it (a test, a measure, an analysed photo, a gate) |
| `evidence` | list of paths or git refs | `dist/evidence/<id>/...` or a named tag/commit |
| `origin` | list of strings | where it came from (`pendencias.md:S1`, `trace:port-from-nov7#P412`, `performance.md:P-13`, `audit-2026-11-12:E`, `gate:<RULE>:<file>`, `run:<run id>`) |
| `related` | list of ids | same cause or dependent |
| `spec` | string or null | the specification that will fix it |
| `opened`, `traced`, `closed` | dates | `traced` set by the setter when the status first leaves `open` for `traced` (the time from a report to its located cause, SC-013); `closed` only with status `fixed` or `dropped` |
| `fix` | string or null | what changed and why it cannot return (required for `fixed`) |

State transitions (one setter: `tools/atlas problems`):

```
open ──(cause found)──> traced ──(fix planned)──> planned ──(code changed)──> fixed-pending-check ──(criterion passes at block close)──> fixed
  │                                                                                    └──(criterion fails)──> traced
  └──(owner decides it is not a problem, with why)──> dropped
```

Validation: schema at `tools/gates/schemas/problem.schema.json`; a gate fails on a duplicate id, a `fixed` without `fix`, an
`unknown` cause without `reading`, an evidence path that does not exist, or a problem tracked anywhere else (a ledger or table of
problems outside the register).

## Instruction source and subject documents

| Entity | Fields | Rule |
|---|---|---|
| Instruction source | CLAUDE.md: sections, rules tagged `[c:<id>]` | under 200 lines; each tag has guards in `tools/gates/contracts.json`; the only file with a precedence statement |
| Subject document | path, subject, owner | one per subject; the declared set: `log/map.md`, `log/problems.jsonl` (+ generated `log/problems.md`), `log/specification.md`, `log/research.md`, `log/history/<area>.md`; Spec Kit's `.specify/` and `specs/` |

## Artifact (anything the work produces on disk)

| Field | Type | Rule |
|---|---|---|
| `path` | folder under `dist/`, a worktree, a branch, a stash, a tag | — |
| `owner` | the task or tool that writes it | one owner |
| `bound` | `max_mb`, `keep` (count) or `while-open` (evidence) | enforced by the owner on every write |
| `end` | `merged`, `superseded`, `closed problem`, `age` | when it goes |

Worktree/branch/stash triage record (written whole in `log/history/process.md` before any removal; an item not done at the block's
close, such as one a running process uses, is entered in the register as a `process` problem): `ref`, `class` (`in-main-clean`,
`in-main-dirty`, `copy`, `behaviour-to-redo`, `to-verify`, `tool-history`), `uncommitted` count, `reason`, `kept-as` (tag or commit
id), `action`, `done`.

## Declarations (the game's own architecture: core X-macros, dumped to `dist/atlas/declared.json`)

| Entity | Fields | Relationships |
|---|---|---|
| State part | `name`, `owner` (file:symbol), `controller` (setter and transition table, or `none` with why), `writers` (command areas, system steps, load, generation) | written by Commands and Systems; announced as Change kinds |
| Command | `name`, `area`, `writes` (state parts), `announces` (change kinds), `undoable` | sent by Features' entry points; the script side's `StoreApi.COMMANDS` is generated from it |
| Change kind | `name` (a `ChangeLog` list), `payload`, `producers`, `consumers`, `touches` (the area a consumer may rebuild) | produced by Commands/Systems; consumed by render, traffic, transit, lots |
| System step | `name`, `order` in the tick, `reads`, `writes`, `locks`, `budget share` | steps State parts each tick |

Validation: a declaration that names a missing symbol, a command writing an undeclared part (run time, debug and profile builds), a
consumer draining a kind it does not declare, or `StoreApi.COMMANDS` differing from `CITY_COMMANDS` fails.

## Feature (derived; `dist/atlas/index.sqlite`)

| Field | Source |
|---|---|
| `id`, `name` | the player's tool (`ToolController.Tool`), the interface's data buttons, a simulation system |
| `entry` | file:line of the input handling |
| `commands` | the tool's dispatches (tracer) joined to Command declarations |
| `state` | the commands' `writes` |
| `changes` | the commands' `announces` |
| `consumers` | the change kinds' consumers, with counted needless work (profile runs) |
| `draws` | the consumers' render outputs |
| `tests` | engine-free tags and engine scenes that reach its files |
| `cost` | per-owner timing zones of the last profile run, against its budget share |
| `marks` | `single_owner`, `controller`, `source_of_truth`, `optimised` (`inside`/`over`/`unmeasured`), `covered` |
| `problems` | open register entries whose owner is on its chain |

## World rule and run report

| Entity | Fields |
|---|---|
| World rule | `id` (B1, B2, G1, G2, M1-M4, D1-D4, S1, S2, P1 of the skill's list), `rule`, `state` it limits, `owner` of its check, `cost` budget of the check |
| Run report | `run`, `commit`, `build` (debug/profile), `scenario` (seed, commands, camera path), `violations` (rule, owner, place, tick, bodies or elements, evidence), `budgets` (metric, limit, measured, over count), `needless` (change kind, consumer, units, identical outputs) |

A violation becomes a register problem (deduplicated by rule, owner and cause) when the run ends.

## Budget

| Field | Values |
|---|---|
| `profile` | `median`, `floor` |
| `tier` | `low`, `medium`, `high`, `ultra` |
| `metric` | frame p90, hitches, GPU p90, main thread per frame, tick, process CPU, RAM, VRAM, load time |
| `limit` | from `log/specification.md` (one owner: a data table read by the measuring task) |

## Check

| Field | Rule |
|---|---|
| `name`, `inputs` (content keys), `deadline`, `heartbeat` (≤ 5 s), `resources`, `verdict` | one owner: `tools/tasks/suites.json`; a task silent past its heartbeat is reported; past its deadline stopped by PID |
