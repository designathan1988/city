---
name: read-before-delegating
description: "The user demands Claude read docs and code in full itself, never by sampling or via agents, before planning or launching agents; findings go to log/review.md"
metadata:
  node_type: memory
  type: feedback
  originSessionId: 09b4e653-7360-49e1-9241-e55d6143adc7
  modified: 2026-11-07T19:43:36.021Z
---

The user (2026-11-07) ordered: read CLAUDE.md and every project document in full, in chunks, personally; understand the
tools and how the code is written before acting; never outsource the initial reading to agents, because only that
understanding lets Claude brief agents correctly. Past refusals to read caused the user real losses.

**Why:** decisions and agent briefs made without reading repeated mistakes (source-game code copied, wrong causes).

**How to apply:** read whole files with Read in chunks; say exactly what was and was not read; record findings, problems
and Claude's own execution or ordering errors in `log/review.md` as they happen, with file and line. Related:
[[no-reference-game]], [[fix-cause-effort-neutral]].
