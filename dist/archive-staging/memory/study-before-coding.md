---
name: study-before-coding
description: "Before coding a tool, research the dependencies' known bugs and fix the evaluation design; never iterate the design against results"
metadata:
  node_type: memory
  type: feedback
  originSessionId: c4e5a5a6-0d2e-46cb-b405-53388197c500
  modified: 2026-11-07T21:31:02.787Z
---

Before writing code, study: the external dependencies' known bugs and limits (issue trackers, changelogs of the exact
installed versions), how the inputs behave, and the full evaluation design (oracle correctness, criteria). Then build
once. Changing the design run after run against results is trial and error, and the user rejects it furiously.

**Why:** in the feature-trace tool (2026-11-07) I had to discover mid-run, in turn, a py-tree-sitter 0.26.0
memory-corruption bug (#505, Point.row), permissive grammars accepting foreign text, and two defects in my own V8
oracle (CRLF offsets, start-line mapping). Each one cost a cycle. The user called it "always the same shit" and
demanded research, understanding, then the fix.

**How to apply:** for any new tool, first list the external APIs and their exact versions and search their issues.
Write the oracle and check it on raw data before trusting it. Present the researched design, then implement in one
pass. When a result fails, read the code and the data to find the cause; do not probe variants. Related:
[[fix-cause-effort-neutral]], [[read-before-delegating]].
