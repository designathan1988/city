---
name: coordinate-parallel-sessions
description: The parallel sessions were stopped by the user; this session integrates their saved branches and owns all continuation, with no agents
metadata:
  node_type: memory
  type: project
  originSessionId: 09b4e653-7360-49e1-9241-e55d6143adc7
  modified: 2026-11-11T19:13:48.474Z
---

2026-11-08: the user first put the review session in charge of two parallel sessions, "Visual realismo" and "Portar funcionalidades". Later the same day the user cancelled both. Their work is saved on their own branches and worktrees. This session reviews it, verifies it, integrates it into main, and continues all the remaining work itself.

**Why:** the user saw unoptimised work, excess and wrong geometry, unported functions and unverified results. They judged that parallel sessions without a strict integrator cost more than they deliver.

**How to apply:**
- Never put another session's work on main unreviewed. For each piece: read the whole diff (CLAUDE.md rules, no roadcraft code, names, structure or values), build, run the tests, photograph what it changes and look at every crop, measure the hot paths before and after. Then merge.
- No subagents and no new sessions unless the owner orders them in so many words (2026-11-11: "NÃO USE AGENTES"); an earlier line here allowing subagents in worktrees was removed that day as contrary to it. See [[never-wake-stopped-agents]], [[no-reference-game]], [[fix-cause-effort-neutral]].
