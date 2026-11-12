---
name: never-delete-records
description: "never delete research tables, plans, ledgers or progress notes, even when a skill says to clean its workspace at the end"
metadata:
  node_type: memory
  type: feedback
  originSessionId: bceccebc-2abc-4510-8d0b-e2ad9391ef75
  modified: 2026-11-10T19:30:53.243Z
---

Nothing recorded in research, notebooks, ledgers or progress files is ever deleted (owner's order, 2026-11-10). Records are appended to, corrected in place when wrong, never pruned.

**Why:** the owner relies on these records (e.g. `log/trace-rules.md`, `log/trace-rules-plan.md`, the executor ledger) to see what was researched, decided and done; superpowers:executing-plans says to delete the plan workspace at the end, which would lose the ledger.

**How to apply:** skip any skill step that deletes a workspace, ledger or notes; keep a version-controlled copy of ignored ledgers in `log/` (e.g. `log/trace-rules-progress.md`) and update it with each ledger line. Related: [[study-before-coding]], [[request-is-one-node]].
