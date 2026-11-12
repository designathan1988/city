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
