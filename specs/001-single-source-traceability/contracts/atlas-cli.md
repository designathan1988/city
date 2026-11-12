# Contract: `tools/atlas` command line

One entry: `python tools/atlas/cli.py <command> [args] [--json]`. Every command prints its answer as text (or JSON with `--json`),
exits 0 on an answer, 1 when the answer reports a failure (a check), 2 on a usage error. No command takes longer than 5 s on a
warm index; the index is refreshed first for changed inputs only, with one progress line per second while it refreshes.

## Queries

| Command | Answer |
|---|---|
| `feature <id or words>` | the chain: entry (file:line) → commands → state parts (owner, controller) → change kinds → consumers (needless work counted) → draws → tests → cost vs budget → open problems; each link with its marks |
| `state <part>` | owner, controller and transitions, declared writers, writers found in code, readers, the change kinds it feeds |
| `writers <symbol or part>` | every function that writes it, by thread and lock; flags writers outside the declaration |
| `triggers <command or change kind>` | every update and creation it causes, transitively; flags those outside the declared `touches` |
| `owner <file or symbol>` | the declarations and features it belongs to, and its open problems |
| `unoptimised` | features and owners over budget or unmeasured, worst first |
| `uncontrolled` | stateful members with no controller; state kept in the interface; parts with more than one owner |
| `features` | the derived features list with marks |

## Checks (exit 1 on a finding; each one a task of `tools/tasks`)

| Command | Fails when |
|---|---|
| `check declared` | a declaration names a missing symbol; `StoreApi.COMMANDS` differs from `CITY_COMMANDS`; a consumer drains an undeclared kind |
| `check dead` | a text, data key, command, question or bound function nothing uses (the former static audit) |
| `check shapes` | a code shape with a cost (the former cause analyser's detectors) |
| `check skill` | a trace-fix command calls a tool that does not exist |

## Register

| Command | Effect |
|---|---|
| `problems [--area A] [--owner O] [--status S] [--cause words]` | lists matching problems |
| `problems add --title ... --symptom ... --area ... --kind ... --origin ...` | new problem, status `open`, id assigned |
| `problems update <id> --cause ... --owner ... --criterion ... --evidence ...` | fields set; status moves by the transitions of `data-model.md` |
| `problems close <id> --fix "..."` / `drop <id> --why "..."` | ends it; evidence under `dist/evidence/<id>/` released |
| `problems import <run report>` | violations of a run entered, deduplicated |
| `problems import-gates <gates json>` | each standing gate finding not yet registered entered as one problem per rule and file (origin `gate:<RULE>:<file>`) |
| `problems stats` | counts by area, status and origin; the median time from `opened` to `traced` |
| `problems render` | regenerates `log/problems.md` |
