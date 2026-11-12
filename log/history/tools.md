# History: the tools

One history of the work on the project's tools (the atlas, the gates, the orchestrator, the build scripts), newest last. Each
entry: when, the commit, the problems, the cause, the plan, the files created, changed and deleted, how it was verified.

## 2026-11-12 — the problem register (specification 001, Setup and Foundational: T001 to T011)

- **Problems**: known problems were kept in at least seven places that do not agree, two of them unversioned (spec.md, audit D);
  the gates refused Spec Kit's own layout (`.specify/`, `specs/`: 8 DOC_UNICO, 2 RAIZ_LIMPA); `.claude/` was kept out of git
  whole by `.git/info/exclude`, so the project's own settings and hooks could not be versioned.
- **Cause**: nothing owned the problems; the gates' document rule knew only CLAUDE.md and `log/`.
- **Done**: `log/problems.jsonl`, the one register, one problem a line against `tools/gates/schemas/problem.schema.json`;
  `tools/atlas/problems.py` its one owner (ids `P<area>-<n>` never reused; the status moved only through `Register.move` by the
  transitions of the data model; written as LF bytes; `faults` the rule the gate holds); `tools/atlas/cli.py problems` (list with
  filters, add, update, close, drop, render); `tools/atlas/render.py` the generated view `log/problems.md` (`--check` for the
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
