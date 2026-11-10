# SDD ledger — plan: log/trace-rules-plan.md
Spec: log/trace-rules.md
Ruling: execute on main, no worktree — CLAUDE.md orders direct work on main — a broken commit lands on main (each task commits only after its tests pass).
Ruling: uncommitted flow work from before the research (flows.py and edits in 16 files) is the working-tree base; Task 2 deletes flows.py and replaces its text markers — keeping it avoids redoing correct pieces (value flows, translation unit), and the plan removes every text marker — cost if wrong: leftover marker code survives, caught by Task 2's no-marker test.
Pre-flight: T2 produces IR builders consumed by T3 (readers) and T4 (typer); T3 produces Ref.base/args/types as IR consumed by T4/T5; T1 HANDLERS registry is filled by T2-T5 — no conflict found.
Task 1: Ruling: the test that every rule has a handler moves to Task 5 — handlers are written in Tasks 2-5, a Task 1 test for them could only fail — cost if wrong: none, the check still lands before the close.
Task 1: complete (commits b72d1e5..bb6f1b6, tests: python -m unittest discover -s tools/analysis/featuretrace/tests -t tools/analysis → OK)
Task 2: Ruling: flows.py is deleted in Task 3, with the readers that still import it — deleting it in Task 2 breaks the suite between tasks — cost if wrong: none, Task 3's no-marker check covers it.
Task 2: complete (commits bb6f1b6..4ab4b3c, tests: python -m unittest discover -s tools/analysis/featuretrace/tests -t tools/analysis → OK)
