---
name: commit-push-regularly
description: "commit and push to origin/main after each closed piece of work, not batched at the end"
metadata:
  node_type: memory
  type: feedback
  originSessionId: 1f873cde-74ee-4e61-83c3-86a49786a4af
  modified: 2026-11-09T19:46:47.142Z
---

SUPERSEDED IN CADENCE on 2026-11-12 by [[no-city-tests-mid-work]]: commits are made at the END OF THE BLOCK (a commit per theme, one push), never after each fix.

Original (2026-11-09): commit and push to origin/main as each piece closes (each fixed problem or tooling batch), not one big commit at the end.

**Why:** the owner asked on 2026-11-09 ("faça commit regularmente e push") during a long multi-problem session.
**How to apply:** after a fix passes its area's tests and the changed files' gates, commit with its work-log entry and push; leave the release DLL for the theme's close (rebuilt then).
