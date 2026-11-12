# Quickstart: proving the feature works

Run at the block's close (CLAUDE.md: nothing is run while a block is open). Each step names the requirement it proves and the
expected outcome. Commands are run from the repository root.

## 1. One instruction source (FR-001 to FR-007; SC-001)

```bash
python tools/gates/run_gates.py --summary
```

Expected: no `INSTRUCTION_ELSEWHERE`, `PRECEDENCE_CLAIM`, `CONTRACT_UNGUARDED` (FR-006's contradiction check), `NESTED_CLAUDE`
or `MEMORY_ON` finding;
`python tools/gates/run_gates.py --register log/problems.jsonl` reports 0 NEW findings; CLAUDE.md under 200 lines; the
project's `.claude/settings.json` has `autoMemoryEnabled: false` and the fixtures in `claudeMdExcludes`. In a fresh session, ask
"where is the work method and what wins in a conflict": the answer is CLAUDE.md, alone.

## 2. A clean, bounded project (FR-014 to FR-020; SC-003, SC-004)

```bash
python tools/tasks/cli.py worktrees
```

```bash
python tools/tasks/cli.py clean --dry-run
```

Expected: every worktree listed has an owner and is younger than its work; `clean --dry-run` frees nothing (every bound already
holds); the project folder and its siblings measure under 12 GB; `git tag -l "archive/*"` lists the archive tags; `git stash list`
is empty.

## 3. One register (FR-021 to FR-023; SC-005)

```bash
python tools/atlas/cli.py problems --status open --area traffic
```

Expected: an answer in under 5 s; the counts per origin match the migration's table (every ledger, `pendencias.md`, `performance`,
`review`, `specification` item accounted for); no other tracker file exists.

## 4. Trace a feature (FR-024 to FR-031; SC-006, SC-007)

```bash
python tools/atlas/cli.py feature "build road"
```

```bash
python tools/atlas/cli.py triggers build_road
```

```bash
python tools/atlas/cli.py uncontrolled
```

Expected: each answer in under 5 s, with file:line for every link and every mark set or "unknown"; `atlas check declared` exits 0.
Read one chain against the code by hand: every link is right.

## 5. Engine-free and deterministic checks (FR-035 to FR-039; SC-008, SC-009)

```bash
python tools/tasks/cli.py run sim_tests
```

Expected: the engine-free tests finish in under 10 s; the determinism scenario gives the same state hash twice; a progress line at
least every 5 s; `python tools/tasks/cli.py run quick --ready` finishes in under 10 minutes with a summary of ran, passed, failed and
skipped.

## 6. The game reports what breaks (FR-032 to FR-034; SC-010)

```bash
python tools/tasks/cli.py run scenario_overlap
```

Expected: the run report lists the planted overlap and the planted missing span by rule and owner, and `atlas problems import`
enters them once; the release build's library has no symbol of a probe (`llvm-nm` on the release DLL lists no `*_probe*`).

## 7. The skill (FR-041 to FR-045; SC-011)

```bash
python tools/atlas/cli.py check skill
```

Expected: exit 0; the skill's hooks appear only in the project's `.claude/settings.json`; its text cites CLAUDE.md's tags.

## 8. The product (FR-046 to FR-049; SC-012)

Export with `tools/build/deliver.sh export`, then check: the game shows its version; on a processor without AVX2 (the gate's test
mode) it shows the message and quits; a forced crash writes a backtrace to `user://logs/`; 0 texts nothing shows (`atlas check dead`).
