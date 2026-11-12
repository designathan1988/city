---
name: tree-back-to-nov7
description: On 2026-11-10 the owner had main taken back to a1a771c4 (2026-11-07); ports and perf fixes are redone one at a time from there
metadata:
  node_type: memory
  type: project
  originSessionId: 00332dfd-d4c9-4cf0-ab01-b7d5a510442a
  modified: 2026-11-10T11:17:14.458Z
---

On 2026-11-10 the owner ordered `native/`, `project/`, `tools/`, `log/` and `CLAUDE.md` back to commit `a1a771c4`
(2026-11-07 20:29): commit `dcfd4c7c` on main, no history rewritten. `.claude/` was kept. Main as it stood before is the
tag `antes-da-volta-2026-11-10` (pushed); that day's uncommitted work (mimalloc hooks via `MI_WIN_USE_FLS`, memory census by
thread and by asker, `memory_test`) is the local branch `wip/allocator-2026-11-10` and `dist/compare/wip-2026-11-10/`.

**Why:** photos of the 24 km city (map index 4, `km24`, seed 1) at five dates showed the city whole at 06/11 and 07/11 and
broken after the lab ports that began 2026-11-08 21:30 (`b56db4a3`): outer districts one shed a lot on grass. The owner
judged the 07/11 tree the better base. Measured cost of that base: about twice the video and static memory of the later
tree (centre view 5,454 MiB video, 1,351 MiB static), which is the performance work to do.

**How to apply:** work from the tree that stands; read the tag only for WHAT was built or found (its `log/performance.md`),
never copy code back. Redo each performance fix and each lab feature one at a time, designed for this architecture, measured
in the 24 km city (memory, CPU, GPU, frame) and photographed before and after. The 07/11 tree still holds code translated
from the 4.0 (translated runtime, `flow_control`): redo it too, do not regenerate it. The repository's first commit is
"Base antes do porte" (`bb58790e`, 2026-11-03), with no DLLs. See [[no-reference-game]] and [[label-and-wait-photos]].
