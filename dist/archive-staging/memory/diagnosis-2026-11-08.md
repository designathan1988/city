---
name: diagnosis-2026-11-08
description: "2026-11-08 diagnosis of why sessions were slow and contaminated: prompt-only enforcement, whole-tree test keys, measuring instead of reading, tools without oracles; the three fixes proposed"
metadata:
  node_type: memory
  type: project
  originSessionId: 00332dfd-d4c9-4cf0-ab01-b7d5a510442a
  modified: 2026-11-11T19:14:05.246Z
---

Diagnosis delivered 2026-11-08 (report in chat; research notes were in the session scratchpad, not kept). HISTORY: the facts
below are as of that date; since then the project has hooks (the trace-fix skill's, `.claude/skills/trace-fix`) and scene inputs
in tools/tasks. Check the current tree before acting on any of it.

Causes found in the repository and the records:
- Enforcement is prompt-only: `.claude/` has no settings.json and no hooks; user settings have none; nothing blocks reads of
  the source game, still readable at `C:\Users\Jonathan\Desktop\city-archive` (7.9 GB) and
  `C:\Users\Jonathan\Documents\Projects\city\roadcraft`. 284 lines in 159 files of native/src and project/ still cite it.
- `tools/tasks/suites.json`: template `godot_test` keys all 37 scenes by `@project`; no scene declares its inputs;
  `gates_changed` keyed by `**`. One changed .gd reruns the whole suite (deadlines 600-900 s each).
- Roads: `render/roads/road_emit.cpp:32-43` uses a uniform 3 m step on any curve beyond 0.5 m chord or any grade change;
  every strip emits top + walls + skirts per stretch; verge ribbon is a port of the source's vergeTop; LOD levels exist
  (`mesh_levels.h`), so the excess is the near level. No probe asserts a geometry budget.
- Past tools failed for lack of an oracle on real data and a failing test of the tool itself; designs iterated against results.

**Why:** the user keeps restating the same rules; Anthropic's docs say rules that cannot bend belong in deny rules and hooks.

**How to apply:** three session cards were proposed (harness-hooks, roads-geometry, tasks-inputs-tools). Before any work,
check whether they were done (`.claude/settings.json`, `project/tests/inputs.json`, `tests/perf/geometry_census`). Hooks and
`.claude/settings*` are the user's files: write and test them, the user enables. Related: [[no-reference-game]],
[[fix-cause-effort-neutral]], [[study-before-coding]].
