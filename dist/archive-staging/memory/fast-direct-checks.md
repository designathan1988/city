---
name: fast-direct-checks
description: "a check or small fix must take seconds; look at the most likely place first, never launch wide scans or long chains"
metadata:
  node_type: memory
  type: feedback
  originSessionId: 82e887f0-3bf5-483b-8e1a-9df0f96ca163
  modified: 2026-11-10T03:00:27.192Z
---

A verification or a small correction must be fast: go straight to the most likely place (the newest folders, one `ls -t`,
one targeted grep), answer, and stop. No wide `find` over `dist/` (gigabytes), no chain of preliminary surveys, no
re-deriving what is already known.

**Why:** on 2026-11-10 the user complained twice ("que demora pra verificar") after a question about whether a session
had taken photos took minutes: a broad time-filtered `find` returned zero, was taken as the answer (wrong), and the
recheck scanned gigabytes again. `ls -lt dist | head` answered it in one second.

**How to apply:** for "did X happen / does X exist" questions, list by modification time where the thing would be
written; treat an empty result of a fragile search as "search failed", not as "no". Give the answer in a few lines.
Related: [[study-before-coding]], [[fix-cause-effort-neutral]].
