# History: the tools

One history of the work on the project's tools (the atlas, the gates, the orchestrator, the build scripts), newest last. Each
entry: when, the commit, the problems, the cause, the plan, the files created, changed and deleted, how it was verified.

## 2026-11-12 — the problem register (specification 001, Setup and Foundational: T001 to T011)

- **Problems**: known problems were kept in at least seven places that do not agree, two of them unversioned (spec.md, audit D);
  the gates refused Spec Kit's own layout (`.specify/`, `specs/`: 8 DOC_UNICO, 2 RAIZ_LIMPA); `.claude/` was kept out of git
  whole by `.git/info/exclude`, so the project's own settings and hooks could not be versioned.
- **Cause**: nothing owned the problems; the gates' document rule knew only CLAUDE.md and `log/`.
- **Done**: `doc/problems.jsonl`, the one register, one problem a line against `tools/gates/schemas/problem.schema.json`;
  `tools/atlas/problems.py` its one owner (ids `P<area>-<n>` never reused; the status moved only through `Register.move` by the
  transitions of the data model; written as LF bytes; `faults` the rule the gate holds); `tools/atlas/cli.py problems` (list with
  filters, add, update, close, drop, render); `tools/atlas/render.py` the generated view `doc/problems.md` (`--check` for the
  generated-files gate); the gate PROBLEM_REGISTER (`tools/gates/gate_data.py check_register`) with its fixtures; `.specify/` and
  `specs/` declared document places (`rules.json doc_places`, `root_allowed`); the constitution a pointer to CLAUDE.md;
  `.claude/settings.json`, `.claude/hooks/` and `.claude/skills/` versionable (`.gitignore`), the rest of `.claude/` ignored; the
  data lexer reads `.jsonl`; the names the documents cite in Portuguese (gate ids, file, branch and tag names) entered the English
  gate's lexicon as names; the archive tag `archive/2026-11-12/before-cleanup` at `89be3acd`.
- **Verified** (once, at the block's close): `python -m unittest discover tools/atlas/tests` 17 tests OK in 0.06 s;
  `run_gates.py --selftest` 91 cases, 39 rules OK; `run_gates.py --files-from` the block's 49 files OK; the whole gates 159 findings
  (170 before: the 10 of Spec Kit's layout and the one non-English text are gone; the 2 DOC_UNICO left are the trace-fix skill's
  `SKILL.md` and `INSTALL.md`, for its redesign in US7).

## 2026-11-12 — the owner's tracer, read for bringing in (specification 001 T053a, block `tracer`)

- **The request**: "see whether the tracing tool at C:\Projects\tracer helps and serves; if it serves, use it and copy it into
  the repository; check first" (the owner, during US2).
- **What it is**: the same design as `tools/tracer` (generic: a tree-sitter grammar and a pattern file per language, the
  language's own rules, no name of a program, library or platform, held by its `test_generic.py`), developed apart to commit
  `a550ef4` (its own repository, Spec Kit specification `001-e2e-feature-tracing`): about 1,900 lines more, a new `configs.py`
  (the files each language reads to resolve names: tsconfig/jsconfig paths, project.godot autoloads and `res://`, a compilation
  database's include folders), objects written in place as shapes, what code not read gives back, guesses narrowed by arity and
  by the other members read, ambiguous calls said, tables of names followed by the caller's key. Its bench (outside this
  repository) measures it against the TypeScript checker and libclang: exact recall 97.9 % and 97.8 %, precision 99.6 % and
  99.98 %.
- **Checked here**: its 91 tests pass (5.5 s); on this game (`project` and `native/src`) the analysis takes about 14 s;
  `TransitStrip._drop` traces exactly from the interface through `CityStore.dispatch` to `CityTransit::transit_drop_line` (by the
  command's text) and on to `run_edit`, `WriterHold`, `EditHistory.begin` and `end`. Gaps on this game, for the atlas (US4):
  nothing is shown reaching `_drop`, which is handed as `_drop.bind(...)` to a button's signal (`transit_strip.gd:36`); back
  from `transit_drop_line` the callers of `dispatch` whose command's text names no function the tracer knows are all kept;
  owners, state controllers, the `ChangeLog`'s consumers and budgets are not its question (the declarations of US4 are).
- **Read whole** (CLAUDE.md, bringing work in): its fourteen modules, its five pattern files, the tests that changed or are new;
  verdict: this project's own kind of tool, no code of the roadcraft (its `bench/` measures against the 4.0 and stays in its
  repository).
- **What the import must change**: four of its modules pass the gates' 300 lines of a Python file (`calls.py` 388, `flow.py`
  393, `program.py` 314, `scopes.py` 325); they are split by responsibility as they come in, each part with state of its own
  (never one class spread over files), the rest byte for byte from `a550ef4`.
- **Done**: `tools/tracer` replaced by `a550ef4`'s `trace.py`, `tracer/` and `tests/` (git archive, `core.autocrlf=false`:
  the first extraction took the other repository's `core.autocrlf=true` and wrote CRLF, which LINE_ENDINGS caught as 83 new
  findings; every file not changed here now hashes to its blob). Split by responsibility: `links.py` (the records `Link`,
  `Site`, `Result`), `shapes.py` (objects written in place, with their own state, asked by the expressions and the calls),
  `run.py` (the whole program run to its end), `adopt.py` (definitions written out of their scope), the guesses by name
  (`by_name`, `takes`, `holder`) moved to `reads.py`, their only reader. The five `cfg` fixtures without a header got one
  (CABECALHO), `test_configs` reading their lines one further down; `test_shapes` takes the walk through `support`, as every
  other test takes the tracer (PYTHON_IMPORT_UNRESOLVED).
- **Verified** (block `tracer`, 2026-11-12): the tracer's 91 tests OK in 5.8 s; the trace of `TransitStrip._drop` on this game
  byte for byte the original's (24,463 links, about 15 s); the whole gates against the register 161 findings, 0 NEW. The gaps
  a real trace showed are registered: Ptools-7 (nothing shown reaching a function handed as a bound callable), Ptools-8 (back
  from a command, every caller of the dispatcher whose text names no function kept), for the atlas (US4).

## 2026-11-12 — the tracer's endless walk; the comments that named removed tools (block `pending-five`)

- **Ptools-94**: `trace.py --list --root world/roadTypes.ts` (the reference) stopped with RecursionError. The file nests five deep,
  so it was a cycle, not depth: the traceback repeats `value -> of_definition -> called -> given_back -> guard -> condition ->
  value`. **Cause** (read in `tools/tracer/tracer/conditions.py`): `given_back` asked `guard(f, node, d)` for each return's
  condition without its `depth` and `seen`, and `guard` and `chosen` weighed every test with `condition(f, test)` at depth 0 and an
  empty `seen`; a return guarded by a call back into a function on the way began the walk again, so neither the depth ceiling
  (`DEPTH`) nor `seen` ever stopped it. **Fix**: `guard` and `chosen` take the caller's `depth` and `seen` and hand them to every
  `condition` and `value`; `given_back` passes its own. A top-level guard keeps depth 0 and an empty set, so what a start asks is
  unchanged; a nested one now stops as the value around it does. The register's owner (the owner's other copy) moved to this
  repository's `tools/tracer/tracer/conditions.py`.
- **The tracer's cache**: `index.py:116` writes its saved analyses to `tools/tracer/.index`, while `.gitignore` ignored
  `tools/tracer/tracer/.index/` (a folder nothing writes): the pattern now names the folder written.
- **Ptools-95**: comments and `_doc`s that described a format by the script that wrote it (`cook_people.py`,
  `cook_signatures.py`, `make_furniture.py`, `make_pole_parts.py`, `rider_poses.py`, the yard bench, the landscape capture, the
  sampler). Each now says the format itself or where it is read (`people_asset.cpp` load order, the palette's fixed point, the
  ragdoll's joint and part order, `signature_bodies.cpp`, `furniture.json`'s part shapes and options, CELESC E-313.0002 for the
  pole), or that the tool left the tree and git keeps it before 18782f55; `lab.cpu.note` no longer points the player at a script.
  Left as records: the register, the histories, `doc/research.md`, the dated survey in `doc/specification.md`.

## 2026-11-13 — everything left open closed, the contracts system removed (block `fix-all`)

- **Ptools-96** (the gates' selftest failed 5 fixtures): 18782f55 moved `log/` to `doc/` and removed Spec Kit, and the fixtures,
  `rules.json` (`doc_places`, `instruction_skip`) and `tools/tasks/tests/test_block.py` kept the old layout. The fixtures moved
  to `doc/` (`register__fail`, `register__pass`, `problems_elsewhere__pass`, `patch_pass`), Spec Kit's files and entries left
  (`root__pass`, `single_doc__pass` now shows `.claude/agents`, `single_doc__fail` a near miss of it). `tools/atlas/migrate.py`
  read the old `log/pendencias.md` (folded into the register in 45ba8a5d) while its test wrote `doc/`: that source removed, the
  test on `doc/` paths. Selftest 95 cases, 41 rules, OK; the tasks' 86 tests and the atlas tests OK.
- **Ptools-97 and the bureaucracy** (the owner's order): the `[c:...]` markers, `tools/gates/contracts.json`,
  `gate_contracts.py`, its schema and fixtures, and INSTRUCTION_ELSEWHERE (a marker outside CLAUDE.md) removed; the `hook`
  guard kind went with them (Context7 /anthropics/claude-code: a hook is declared in a settings file's `hooks`, which this
  project no longer has). `block.command_refusal`, the removed hook's, and its test removed.
- **Ptools-98** (the street capture's views all one frame): `CityCamera.settle` → `apply` rewrites the camera every frame
  (`process_priority` EARLIEST) and `capture_street_views.gd` wrote `rig.camera` directly. `CityCamera.pin`/`unpin` (the rig
  stops processing, Godot's PROCESS_MODE_DISABLED: no settle, gesture, input or shake) are the one way to hold a view;
  `capture_street`, `capture_street_views`, `pair_camera`, `capture_blast`, `capture_roads` and `look_aim` use them.
- **Ptools-99**: `tools/tasks/km24.py` (doc/history/performance.md).
- Dropped as their subject left the tree: 21 problems of removed tools (the old tracer, tools/analysis, tools/process,
  tools/audit, the oracle, the trace-fix skill, rename_english.py, the sampler, the cooks).
