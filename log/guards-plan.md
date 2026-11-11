# The contracts' guards (Part B) Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking. In this project the owner forbids agents (2026-11-11): executing-plans, inline.

**Goal:** every contract of the project kept by a check that refuses the change: the roadcraft never copied (even renamed), one writer of the city, every state with its controller, no number moved to pass, no contract without its guard.

**Architecture:** five new rules in `tools/gates` (the one owner of verified rules), each a `PerFile` or `Whole` rule of `registry.py` with fixtures under `tools/gates/tests/fixtures/` (the self-test demands a passing and a failing case per id), configured in `rules.json`, cached through `tools/tasks` like every gate. The copy detector keeps the reference's fingerprints in a store keyed by the reference tree's content; the reference's place comes from `tools/tasks/locations.py` (the one owner of this machine's paths).

**Tech Stack:** Python 3.12 stdlib, the gates' lexers (`lex_c` for C++ and TypeScript, `lex_gd`), tools/tasks (`key_of`, `memo`, `locations`).

**Spec:** `log/live-instance-design.md`, Part B (B1 to B6) and Part C step 1; `log/how-we-work.md`; CLAUDE.md.

## Global Constraints

- Nothing of the roadcraft is copied: no line, translation (by hand or machine), structure, name or function; a rename is still a copy.
- Every new file starts with a header comment of its responsibility, is written in English, and enters `log/mapa.md` in the same commit.
- No machine path in the repository (gate MACHINE_PATH): the reference tree's place comes from `tools/tasks/locations.py`.
- A verification tool must be able to fail: non-zero exit on a problem, an empty set fails, every id has a failing fixture.
- No patch: thresholds are fixed by the fixtures' tests before the rule guards anything, never moved to make a run pass.
- Python files: the gates' own size limits (`rules.json` max_lines, max_function_lines) and imports that resolve.

## Review Focus

- A reference identifier that is a plain domain word (`lane`, `segment`, `junction`, `width`): never a copy by itself; only multi-part names count (test `test_domain_words_pass`).
- Our own code that implements the same behaviour independently (same rule, different structure): must pass (test `test_independent_passes`).
- A copy renamed identifier by identifier: must still fail by structure and constants (test `test_renamed_translation_fails`).
- The reference tree missing on a machine (a clone without it): the rule fails loudly naming what to set, never passes silently (test `test_missing_reference_fails`).
- A changed file with only comments citing the reference ("Roadcraft 4.0 powerLine.ts openStretch" in a comment): comments are not code; a citation passes, a copied identifier in code fails (test `test_comment_citation_passes`).

---

### Task 1: The reference's fingerprint store (`tools/gates/lab_prints.py`)

**Files:**
- Create: `tools/gates/lab_prints.py`
- Modify: `tools/tasks/locations.py` (a `roadcraft_lab` location: env `ROADCRAFT_LAB`, then `tools/tasks/local.json` key `roadcraft_lab`, else a refusal naming what to set)
- Test: `tools/gates/tests/test_lab_prints.py`, fixtures `tools/gates/tests/lab/ref/*.ts`

**Interfaces:**
- Produces: `neutral_tokens(source: Source) -> list[tuple[str, int]]` (language-neutral token texts with their line), `fingerprints(tokens, k: int, w: int) -> list[tuple[int, int]]` (winnowed hashes with their line), `defined_names(source: Source) -> set[str]` (identifiers a file defines: functions, classes, consts, fields), `normal_name(name: str) -> str | None` (case and separators removed; None for names of one part or under `min_name_len`), `Store(names: dict[str, list[str]], prints: dict[int, list[tuple[str, int]]], numbers: dict[tuple, list[tuple[str, int]]])`, `build_store(ref_root: str, cfg: dict) -> Store`, `load_store(ctx) -> Store` (cached by the digest of every reference file's bytes, through `ctx.cache.get`).

- [ ] **Step 1: Write the failing tests** in `test_lab_prints.py`: `test_neutral_tokens_drop_types` (a TS `const a: number = 3.5;` and a C++ `const double a = 3.5;` give the same neutral stream `D I = N:3.5 ;`), `test_keywords_map_across_languages` (`===`→`==`, `elif`→`else if`, `function`/`func`/a C++ definition head → `F`), `test_fingerprints_stable` (the same stream gives the same winnowed set in two runs), `test_normal_name` (`openStretch`, `open_stretch`, `OpenStretch` → `openstretch`; `lane` → None), `test_defined_names_ts` (a fixture TS file's functions, classes and consts listed).
- [ ] **Step 2: Run** `python -m unittest tools.gates.tests.test_lab_prints -v` — expected: FAIL (module missing).
- [ ] **Step 3: Implement** with these values in `rules.json` under `"lab_copy"`: `"k": 12, "w": 8, "min_name_len": 8, "trivial_numbers": [0, 1, 2, -1, 0.5, 100, 1000]`, the reference given as `"@roadcraft_lab"` (the locations key) or a path relative to the repository (fixtures). Declarations (`let const var auto int float double bool func def` and type names of each language) become `D`; identifiers `I`; numbers `N:<value as float>`; strings `S`; operators kept, normalised as above. Winnowing: k-gram hashes by `facts.token_id` over the neutral texts, Karp-Rabin as `facts.clone_windows`, the minimum of each window of `w` kept (rightmost on ties).
- [ ] **Step 4: Run** the tests — expected: PASS.
- [ ] **Step 5: Commit** `tools(gates): the roadcraft's fingerprints, kept by the tree's content`.

### Task 2: COPIA_DO_LAB (`tools/gates/gate_lab_copy.py`), calibrated before it guards

**Files:**
- Create: `tools/gates/gate_lab_copy.py`
- Modify: `tools/gates/registry.py` (a `PerFile("lab_copy", ["COPIA_DO_LAB"], applies, check, context=lambda ctx: [store key])`), `tools/gates/rules.json` (`lab_copy.paths`: `native/src`, `project/game`, `project/ui`, `project/render`, `project/data`; `extensions`: `.cpp`, `.h`, `.gd`, `.gdshader`, `.json`)
- Test: `tools/gates/tests/test_lab_copy.py`; fixtures `tools/gates/tests/fixtures/lab_copy_*/` (case.json + repo + a `ref/` tree named by the case's override)

**Interfaces:**
- Consumes: Task 1's `load_store`, `neutral_tokens`, `fingerprints`, `defined_names`, `normal_name`.
- Produces: `check_lab_copy(ctx, src, err)` reporting `COPIA_DO_LAB <path>:<line>` with the reference's file and line in the message; `matches(store, prints, numbers, names) -> list[Match]`.

- [ ] **Step 1: Write the calibration tests** (`test_lab_copy.py`) on fixtures written for this purpose (behaviours invented for the test, nothing taken from the roadcraft): `test_line_translation_fails` (a TS function and its line-by-line C++ translation), `test_renamed_translation_fails` (the same translation with every identifier renamed), `test_machine_translation_fails` (the TS function through a literal statement-by-statement C++ rewrite with types added), `test_independent_passes` (the same behaviour written another way: different loop form, different decomposition, constants from data), `test_domain_words_pass`, `test_comment_citation_passes`, `test_missing_reference_fails`, `test_unchanged_lines_not_judged` (a copy on lines the change does not touch is left to the scan, Task 3).
- [ ] **Step 2: Run** `python -m unittest tools.gates.tests.test_lab_copy -v` — expected: FAIL.
- [ ] **Step 3: Implement the rule:** judge the changed lines' functions (the whole function a changed line falls in, by `cpp_syntax` / `gd_syntax` function bodies); refuse when (a) a name the function or file defines normalises to one the reference defines, or (b) its winnowed fingerprints match one reference function's in a run whose share of the function's fingerprints is at least `lab_copy.min_share`, or (c) `lab_copy.min_numbers` or more non-trivial numeric constants appear in the same order in one reference function. Fix `min_share` and `min_numbers` as the values that make every calibration test pass with margin on both sides, write them in `rules.json` with the reason in its `_doc`, and never move them afterwards without a recorded cause (Task 7, REMENDO).
- [ ] **Step 4: Add the self-test fixtures** `lab_copy_pass` and `lab_copy_fail` (case.json with `"overrides": {"lab_copy": {... "reference": "ref"}}`), run `python tools/gates/run_gates.py --selftest` — expected: `Self-test OK`.
- [ ] **Step 5: Run** the unit tests and `run_gates.py --file tools/gates/gate_lab_copy.py` — expected: PASS, Gates OK.
- [ ] **Step 6: Commit** `tools(gates): COPIA_DO_LAB refuses the roadcraft's code, renamed or not`.

### Task 3: The scan of what already stands

**Files:**
- Create: `tools/gates/lab_scan.py` (`python tools/gates/lab_scan.py [--out dist/lab_scan.json]`: every file of `lab_copy.paths` judged whole, every match listed with ours and theirs)
- Modify: `log/pendencias.md` (one entry per area found), the trace (`ledger.py problem` per area, under its cause C23 to C33)
- Test: `tools/gates/tests/test_lab_scan.py` (`test_scan_reports_fixture_copy`, `test_scan_exit_code`: exit 1 when anything is found, 0 when nothing)

- [ ] **Step 1: Write the failing tests.** **Step 2: Run** — FAIL. **Step 3: Implement** with Task 2's `matches`. **Step 4: Run** — PASS.
- [ ] **Step 5: Run the scan on the tree**, read its report whole, and enter each area found as a problem with its file and line (what it copies, the cause it belongs to, the redesign it needs: Part C step 6).
- [ ] **Step 6: Commit** `tools(gates): the roadcraft's code still standing, found and entered`.

### Task 4: CONTRATO_SEM_GUARDA (`tools/gates/gate_contracts.py`, `tools/gates/contracts.json`)

**Files:**
- Create: `tools/gates/gate_contracts.py`, `tools/gates/contracts.json`, `tools/gates/schemas/contracts.schema.json`
- Modify: `CLAUDE.md` (a marker `[c:<id>]` at the end of each rule's bullet, within its 200-line limit), `tools/gates/registry.py` (a `Whole("contracts", ["CONTRATO_SEM_GUARDA"], inputs = CLAUDE.md, log/how-we-work.md, contracts.json, check)`)
- Test: fixtures `contracts_pass`, `contracts_fail` (a marker with no entry; an entry whose guard names a gate id no rule has; a bullet in a rule section with no marker)

**Interfaces:**
- Produces: `contracts.json` entries `{"id": str, "rule": str, "guards": [{"kind": "gate"|"test"|"hook"|"runtime"|"tool"|"conduct", "name": str}]}`; a `conduct` guard is allowed only for a rule no mechanism can hold (speaking Portuguese to the owner), and the gate lists those in its message so they stay seen.

- [ ] **Step 1: Write the failing fixtures.** **Step 2: Run** `run_gates.py --selftest` — FAIL. **Step 3: Implement** (a gate guard must name an id `registry.all_rules` declares; a test guard a file that exists; a hook guard a file under `.claude/skills/trace-fix/`; a runtime guard a probe name the store declares). **Step 4: Mark CLAUDE.md and fill contracts.json** for every rule, the new gates of this plan included. **Step 5: Run** the self-test and `run_gates.py` on CLAUDE.md — PASS. **Step 6: Commit** `tools(gates): no contract without its guard`.

### Task 5: ESCRITOR_DO_ESTADO (`tools/gates/gate_writer.py` and the debug DLL's named writer)

**Files:**
- Create: `tools/gates/gate_writer.py`
- Modify: `tools/gates/rules.json` (`writer.allowed`: `native/src/sim/`, and in `native/src/api/` only inside `run_edit`'s lambda and the entry points listed by name in `writer.entries` with their reason), `native/src/sim/state/map_hold.h/.cpp` (an exclusive hold names its writer: command, tick, load, generation; a write under no named writer stops the tests' DLL, the CHECKS family)
- Test: fixtures `writer_pass`, `writer_fail` (a `render/` function taking `CityState &`; an `api/` method locking `map_mutex` exclusively outside `run_edit`); the core's `map_hold_test` extended with the named writer

- [ ] **Step 1: Read** `native/src/sim/state/map_hold.h/.cpp`, `native/src/api/edit_result.h`, every `unique_lock<std::shared_mutex>` on `map_mutex` in `native/src/api` (grep), whole, recorded in the trace. **Step 2: Write the failing fixtures and test.** **Step 3: Run** — FAIL. **Step 4: Implement.** **Step 5: Run** the self-test, both DLLs, `python tools/tasks/cli.py run fast map_hold_test --jobs 1` — PASS. **Step 6: Commit** `tools(gates), sim(state): one writer of the city, named on every exclusive hold`.

### Task 6: CONTROLADOR_DE_ESTADO (`tools/gates/gate_states.py`)

**Files:**
- Create: `tools/gates/gate_states.py`
- Modify: `tools/gates/rules.json` (`states.enums`: an enum whose name ends in `State`, `Mode`, `Phase`, `Speed`, `Tool` or is listed), `project/game/tools/tool_controller.gd` and `native/src/sim/simulation/` (SimSpeed) brought to one member, one setter, one transition table where they are not
- Test: fixtures `states_pass` (GameFlow's shape), `states_fail` (a second assignment outside the setter; a setter with no table)

- [ ] **Step 1: Read** `project/game/flow/game_flow.gd`, `tool_controller.gd`, the SimSpeed owner, whole, recorded. **Step 2: Write the failing fixtures.** **Step 3: Run** — FAIL. **Step 4: Implement the rule, then bring the two controllers to it.** **Step 5: Run** the self-test, `quick.sh shell_test,camera_input_test` and both DLLs — PASS. **Step 6: Commit** `tools(gates): every state with its controller`.

### Task 7: REMENDO (`tools/gates/gate_patch.py`)

**Files:**
- Create: `tools/gates/gate_patch.py`
- Modify: `tools/gates/rules.json` (`patch.watched`: numeric literals in `project/tests/**` checks, `deadline_s` in `tools/tasks/suites.json`, the frozen digest file, `rules.json`'s own thresholds, thread counts in `project/data/**`), `registry.py`
- Test: fixtures `patch_pass` (the same change with a cause recorded: a trace `fix` naming the file, or `log/how-we-work.md`'s causes table), `patch_fail` (a tolerance raised with nothing recorded)

- [ ] **Step 1: Write the failing fixtures.** **Step 2: Run** — FAIL. **Step 3: Implement** (a changed watched number needs, in the same change, a line of `dist/trace/*/ledger.json` fixes or of a log record naming that file and line). **Step 4: Run** the self-test — PASS. **Step 5: Commit** `tools(gates): a number moved to pass is refused`.

### Task 8: Close

- [ ] Run `python tools/gates/run_gates.py --selftest`, the full `python tools/gates/run_gates.py`, `python -m unittest discover tools/gates/tests`, `python -m unittest discover tools/tasks/tests`; every new file in `log/mapa.md`; the record `log/review.md` with time and commit; commit and push; tell the owner in Portuguese what each guard refuses and what the scan found.
