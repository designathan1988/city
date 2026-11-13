# t042-tool-defects: the tools' and the lab's open defects, fixed (implementation plan)

> **For agentic workers:** REQUIRED SUB-SKILL: superpowers:subagent-driven-development. The owner's order replaces the TDD
> run cycle: each task writes its test and its code with nothing run (no test, build, gate, Godot); every check runs once at
> the block's end (`block verify`). Steps use checkbox (`- [ ]`) syntax.

**Goal:** the checks the port relies on fail when something is wrong, and the tools stop growing or breaking what they
manage: engine errors fail a test, `clean` keeps what runs need, help never acts, the lab keeps a bounded set of captures,
the line-endings gate survives files that vanish, every key the core shows has its text.

**Architecture:** six independent tasks on disjoint files (no task reads another's output), each in the tool's own owner
module, values in data files where the code is the game's.

**Tech stack:** Python 3.12 (tools/tasks, tools/gates, unittest), GDScript (Godot 4.7.2), JSON data with schemas.

**Spec:** the defects registered or found in t039/t040 (doc/history/tools.md, sections t039 and t040; the agents' reports
quoted in the work files of those blocks), each re-read at its site below.

## Global constraints

- Files start with a comment stating their responsibility; repository text in English; bytes written with LF.
- No magic numbers in `project/game`, `project/ui`: tunable values in `project/data/**` with a schema in
  `tools/gates/schemas`, read through `DataTables`.
- Nothing is deleted outside the project (the constitution): a fix never removes files it did not write.
- A file created enters `doc/map.md` in the same commit.

## Review focus

1. A test that provokes an engine error on purpose must still pass, by declaring it (`expected_errors`), never by the rule
   being loosened (Task 1).
2. `clean` run while a task holds a lock: the lock folder must survive (Task 2).
3. The player's captures from before the fix (826 MB in `user://lab`) must never be removed by the game (Task 4).
4. A file listed as changed and deleted before the gate reads it (the tracer tests' temporary folders) is no error (Task 5).
5. A key built at run time (`"cmd." + name`) is not a literal: the key rule must not invent misses for it (Task 6).

---

### Task 1: an engine error line fails a Godot task

**Files:** Modify `tools/tasks/verdicts.py:11-24,32-41` · Test `tools/tasks/tests/test_verdicts.py`
**Interfaces:** Produces `failure_lines(log: str, limit: int = SHOWN_LINES, expected: tuple = ()) -> list[str]`; task
param `expected_errors: list[str]` (regular expressions; a line matching one is no failure).

- [ ] Test `test_an_engine_error_fails` (a log with `ERROR: Condition "x" is true.` and exit 0 → FAIL naming the line);
  `test_a_colored_error_fails` (`\x1b[1;31mERROR:\x1b[0m ...` → FAIL); `test_a_declared_error_passes` (same log, task
  params `{"expected_errors": ["Condition \"x\""]}` → PASS); `test_a_warning_passes` (`WARNING: ...` → PASS).
- [ ] `GODOT_FAILED` gains `^(?:\x1b\[[\d;]*m)*(?:USER )?ERROR:` (Godot prints `ERROR:` for engine and `push_error` errors,
  coloured on a terminal); `godot()` passes `task.params.get("expected_errors", ())` to `failure_lines`, which drops a line
  any of them matches. The comment above `GODOT_FAILED` says an engine error is a failure unless the task declares it.

### Task 2: `clean` keeps the locks, bounds the lists, refuses an unknown folder

**Files:** Modify `tools/tasks/suites.json` (lifecycle.outputs), `tools/tasks/clean.py:131-145` · Test
`tools/tasks/tests/test_lifecycle.py` (class with `run_clean`, :110)
**Interfaces:** lifecycle entries: `{"path": "dist/tasks", "owner": "tools/tasks/runs.py", "keep": 30, "index": ["locks"]}`
and a new `{"path": "dist/tasks/lists", "owner": "tools/tasks/suites.py (_changed_list)", "keep": 64}`.

- [ ] Tests: `test_the_locks_of_runs_stay` (dist/tasks holds 31 run folders and `locks/`: after clean `locks/` exists);
  `test_the_lists_are_bounded` (70 files under dist/tasks/lists → 64 after clean, the newest kept);
  `test_an_unknown_folder_is_a_usage_error` (`clean --only dist/nosuch` → exit 2 and a message naming the folder, no
  traceback).
- [ ] suites.json: the two entries above (the run folder's `index` keeps `locks` out of its entries: `_children` already
  skips index names; the lists, declared inside dist/tasks, are their own entry: `_declared_inside`).
- [ ] clean.py `cmd_clean`: `outputs.bound(a.only, a.suites)` inside `try`; `KeyError` → print `clean: {message}` to stderr,
  return 2 (a `USAGE = 2` constant beside `OK, FAILED`).

### Task 3: help never acts; the stale references

**Files:** Modify `tools/tasks/cli.py:1-3,276-284`, `tools/hunt/hunt.py` (its argument reading),
`tools/build/pgo.sh`, `tools/build/free_dll.sh`, `project/tests/core/light_color_test.gd:2`,
`project/tests/core/building_light_test.gd:2` · Test `tools/tasks/tests/test_cli.py`
**Interfaces:** `cli.main(["--help"]) -> 0` printing the usage; `-h` the same.

- [ ] Test `test_help_prints_the_usage_and_succeeds`: `main(["--help"])` and `main(["-h"])` return 0 and print `usage:`;
  `main([])` returns 2.
- [ ] cli.py: its first comment block becomes the module docstring (the HEADER gate takes a docstring), so `__doc__` holds it;
  `--help`/`-h` print it and the usage line and return `OK`.
- [ ] hunt.py: arguments through `argparse` (`--help` prints and exits 0 before anything runs).
- [ ] pgo.sh, free_dll.sh: a first `case "${1:-}" in -h|--help) <usage to stdout>; exit 0;; esac` before any action.
- [ ] The two test headers: `tools/build/run_tests.sh` (removed) → "tools/tasks/suites.json runs it on Forward+ and on
  Compatibility (its `@forward_plus` sibling)".

### Task 4: the lab keeps a bounded set of the captures it makes

**Files:** Modify `project/game/lab/frame_capture.gd:8,85-96`, `project/game/lab/lab_recorder.gd` (where it writes its
report to `user://lab`), `project/data/game/lab.json`, `tools/gates/schemas/lab.schema.json`,
`project/tests/core/lab_test.gd:119-131`
**Interfaces:** data `lab.json` `"kept": {"captures": 8, "reports": 16}` (schema: integers ≥ 1); `user://lab/kept.json`
`{"captures": [paths], "reports": [paths]}` (the files the lab wrote, oldest first); `FrameCapture.file` unchanged.

- [ ] lab_test `_capture`: before `take()` it writes `user://lab/stray_check.json` (a file the lab did not make); after,
  `check(kept.json lists the new file and at most kept.captures of them)` and `check(the stray file is still there)`, then
  the test removes the stray file it wrote.
- [ ] One helper (a static function in a new `project/game/lab/lab_files.gd`, `class_name LabFiles`):
  `static func keep(kind: String, path: String) -> void` appends `path` to `kept.json[kind]` and, past
  `DataTables.game("lab").kept[kind]`, removes the oldest listed file (and its `.png` beside a capture) and drops it from the
  list. Only listed files are ever removed: what the folder held before stays.
- [ ] FrameCapture `_keep` and LabRecorder's report write call `LabFiles.keep("captures", file)` /
  `LabFiles.keep("reports", path)` after writing.
- [ ] `doc/map.md`: the line of `project/game/lab/lab_files.gd`.

### Task 5: the line-endings gate survives a file that vanished

**Files:** Modify `tools/gates/gate_files.py:96-107` · Test fixture `tools/gates/tests/fixtures/line_endings__pass`
(or the unit test the gates' tests use for whole rules)
- [ ] Test: a changed file listed by git and absent from disk → no finding, no exception.
- [ ] `check_line_endings`: `open` inside `try`; `FileNotFoundError` → `continue` (a file the change listed and that is gone
  is no longer the work's).

### Task 6: every key the core shows has its text

**Files:** Create `tools/gates/gate_keys.py`; Modify `tools/gates/registry.py` (whole rules, beside "tuning"),
`project/data/i18n/strings.csv` (beside `cmd.build.road`, :116), `project/data/i18n/strings_e.csv` (beside
`hint.road.join`, :374), `doc/map.md` · Test fixtures `tools/gates/tests/fixtures/keys__pass`, `keys__fail` (the form of
`tuning__pass`/`tuning__fail`: `case.json` + `repo/`)
**Interfaces:** rule `keys`, finding `TEXT_KEY_MISSING <file>:<line>`: a string literal in `native/src/**/*.{h,cpp}` that is
a whole key `^(cmd|hint)\.[A-Za-z0-9_.]+$` has a row in some `project/data/i18n/*.csv` (first column).

- [ ] Fixtures: fail repo with a `.cpp` returning `"hint.road.nowhere"` and a CSV lacking it → `TEXT_KEY_MISSING`; pass repo
  with the row present, and a key built at run time (`"cmd." + name`) giving no finding.
- [ ] gate_keys.py: `inputs(ctx, path)` (the C++ files under native/src and the i18n CSVs) and
  `check_keys(ctx, err)` over `ctx.facts(f)["strings"]` (the cached string facts, facts.py) of the C++ files; the CSV keys read
  with `csv.reader`.
- [ ] registry.py: `Whole("keys", ["TEXT_KEY_MISSING"], _in_scope(gate_keys.inputs), gate_keys.check_keys)`.
- [ ] The four missing rows (pt_BR, en): `cmd.reverse` "inverter via" / "reverse road"; `cmd.duplicate` "duplicar via" /
  "duplicate road"; `cmd.paint` "trocar as marcações" / "change the markings" (set_marking_style, city_core_edit.cpp:80);
  `hint.road.invalid` "Nenhuma via da grade pôde ser construída aqui." / "No road of the grid could be built here."
  (road_blocks.cpp:25).

---

## Checks (block verify, once)

- `python -m unittest discover -s tools/tasks/tests` (94 before), `python -m unittest discover -s tools/gates/tests`,
  `python tools/gates/run_gates.py --selftest`, the gates of each changed file.
- `python tools/tasks/cli.py run lab_test` (the capture kept and bounded) and the scripts check.
- Each new test seen failing without its fix where it costs one revert (Tasks 1, 2, 5, 6).

## Next plans (each its own block)

- Gates: the feature map (features.json), the ChangeLog's producers and consumers, a ratchet on new numbers, LAB_COPY's idioms.
- Tracer: Ptools-7 (a function handed with `.bind()` to a signal), Ptools-126 (the core's missed calls), long formulas (T5).
- Then T036-T038 (errors seen, records kept true, the scale probe) and the port of roads.

## Outcome (2026-11-13, closed early for the port)

- Task 1 built and checked: an engine `ERROR:` line fails a Godot task unless the task declares it in `expected_errors`
  (tools/tasks/verdicts.py:11-17, 26-39; tools/tasks/suites.py `_params`; tools/gates/schemas/suites.schema.json
  `expected_errors`); the version of a machine tool moved to tools/tasks/versions.py so suites.py stays under 300 lines.
  tools/tasks/tests: 106 tests, the only failure Ptools-1 (the scheduler's timing test, one run in two, registered).
- Tasks 2 to 5 set aside so the port goes on (the owner, 2026-11-13: no more tool detours): Ptools-127, Ptools-128,
  Pinterface-143, Ptools-129.
- Task 6 (every key the core shows has its text) is part of T036 (its refusal-key gate) and is built there.

## Outcome of the leftovers (2026-11-13, reopened: "finish what is pending")

Tasks 2 to 6 built by one implementer each (nothing run while coded), each reviewed for its brief and its quality, one fix round
for Tasks 2-4, then checked once:

- Task 2 (Ptools-127): `dist/tasks` keeps `locks` and `specs` out of its runs (tools/tasks/suites.json:31), the changed-file
  lists bounded (`dist/tasks/lists`, keep 64, :32), `clean --only` an unknown folder exits 2 (tools/tasks/clean.py `USAGE`).
  `dist/tasks/specs` now has no bound of its own: Ptools-130.
- Task 3 (Ptools-128): `cli.py --help`/`-h` print the usage and exit 0 (tools/tasks/cli.py, its docstring the header);
  tools/hunt/hunt.py reads its arguments by argparse before anything runs; pgo.sh and free_dll.sh answer help first and refuse
  an unknown argument with exit 2; a failed task's lines leave out the errors it declares; tools/tasks/tests/test_help.py.
- Task 4 (Pinterface-143): project/game/lab/lab_files.gd keeps the lab's own files to `kept` (data/game/lab.json): only files
  listed in `<folder>/kept.json` are ever removed; tests write to `user://lab_tests` (GameFlow.tests_drive), never the player's
  folder; lab_test fills the list past its bound and sees the oldest listed removed and a file it did not make kept.
- Task 5 (Ptools-129): tools/gates/gate_files.py skips a listed file removed before it is read; tools/gates/tests/test_line_endings.py.
- Task 6: tools/gates/gate_keys.py (TEXT_KEY_MISSING) and its fixtures; rows `cmd.duplicate`, `cmd.paint`, `cmd.reverse`
  (strings.csv, its text matched to the tool's `cmd.place.reverseRoad`) and `hint.road.invalid` (strings_e.csv). T036 widens
  the key prefixes to the refusal keys.
- Also: a test's pipe left open (tools/tasks/tests/test_cli.py, serve with its input open) closed, so the suite's output is clean.

Checks (fresh, 2026-11-13): tools/tasks/tests 114 OK (no warning); tools/gates/tests 33 OK; gates self-test 97 cases OK; the
gates of every changed file pass but CLAUDE_MD, which judges the owner's own uncommitted CLAUDE.md (not part of this block);
`run scripts lab_test lab_test@forward_plus reference_test`: 6/6 PASS. Tasks 2, 5 and 6's tests seen failing with their fix
reverted (test_the_locks_of_runs_stay; both line-endings tests; TEXT_KEY_MISSING on road_blocks.cpp:25 without its row).
