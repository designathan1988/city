---
name: one-godot-at-a-time
description: "never run the whole test suite mid-work; it opens ~6 Godot at once (85% RAM, 76% GPU) and the user called it absurd"
metadata:
  node_type: memory
  type: feedback
  originSessionId: 00332dfd-d4c9-4cf0-ab01-b7d5a510442a
  modified: 2026-11-11T19:14:02.959Z
---

On 2026-11-10, in the middle of bringing C9 back piece by piece, I ran `python tools/tasks/cli.py run tests digest` (43 scenes). The orchestrator opened six Godot processes at once, each loading a city: memory went to 85%, GPU to 76%. The user: "por que tanto Godot aberto? ... nada justifica um absurdo desse".

**Why:** the change reached only the trees, the cover and the cut; the full suite belongs to the theme's close (CLAUDE.md: verify only what the change reaches) and the user had already ordered code first, tests once at the end ([[code-first-measure-at-end]]). The machine is also the user's desktop.

**How to apply:** while working, one Godot process at a time: a single named scene per command (`cli.py run <one_test>`), only the one the piece reaches. The full suite, `quick.sh --ready` and the captures run once, at the close, and I say so before launching them. A timing test that fails inside a parallel run (camera_input_test did) is re-run alone before it is judged. Once the live instance exists (owner's order 2026-11-11), it is the one Godot kept open; a single test may run beside it, never the suite mid-work. Related: [[no-unwatched-processes]], [[watch-session-clangd]].
