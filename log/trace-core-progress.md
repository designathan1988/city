# SDD ledger — plan: log/trace-core-plan.md
Ruling: execute on main, no worktree — CLAUDE.md (work directly on main) — cost if wrong: a broken commit on main (each task commits only after its tests pass).
Ruling: native execution; the owner said no more questions and asked for the costliest complete solution — cost if wrong: none.
Pre-flight: Tasks 2-6 consume Task 1 (heap.analysis.analyse, Result.points, the lowering); Task 4 consumes Task 3 (contexts for generic summaries); Tasks 7-10 consume all.
